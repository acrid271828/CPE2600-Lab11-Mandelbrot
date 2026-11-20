/**********
 * batch_runner.c
 * Alex Grant
 * 11/18/2026
 * Creates a zooming video of the madelbrot set
 * to compile: make
 **********/
#define _GNU_SOURCE
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>
#include "mandel.h"
// A slight modification Myrberg-Feigenbaum point (x= -1.401155189116)
#define FOCAL_X -1.40116569029
#define DEFAULT_SCALE 4
#define DECAY_RATE .65
#define DEFAULT_N_PROCS 1
#define N_IMGS 50
#define VIDEO_LENGTH 5

int main(int argc, char* argv[]){
    const int n_procs = getopt(argc, argv, "p:") != -1 ?
        atoi(optarg) : DEFAULT_N_PROCS;
    if (n_procs <= 0){
        printf("Number of processes must be positive (is %d)\n", n_procs);
        exit(1);
    }
    sem_t* active_procs = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    sem_init(active_procs, 1, n_procs);
    MandelSpecs specs = {
        .outfile = "mandel.jpg",
        .xcenter = 0,
        .ycenter = 0,
        .xscale = DEFAULT_SCALE / DECAY_RATE ,
        .yscale = 0, // calc later
        .image_width = 1000,
        .image_height = 1000,
        .max = 1000,
    };

    for(int img = 1; img <= N_IMGS; img++){
        specs.xscale *= DECAY_RATE;
        sem_wait(active_procs);
        int pid = fork();

        if (pid == -1){
            printf("fork failure\n");
            exit(1);
        } else if (pid == 0){
            specs.xcenter = FOCAL_X - FOCAL_X * specs.xscale / DEFAULT_SCALE;
            sprintf(specs.outfile, "imgs/mandel-%d.jpg", img);
            generate(specs);
            sem_post(active_procs);
            exit(1);
        }
    }

    while(wait(NULL) > 0);

    sem_destroy(active_procs);
    char command[256];
    int framerate = N_IMGS / VIDEO_LENGTH;

    sprintf(command, "ffmpeg -framerate %d -i imgs/mandel-%cd.jpg -r %d mandel%dfps.mpg",
                framerate, '%', N_IMGS, framerate);

    system(command);

    return 0;
}