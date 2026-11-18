/**********
 * batch_runner.c
 * Alex Grant
 * 11/18/2026
 * Creates a zooming video of the madelbrot set
 * to compile: make
 **********/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "mandel.h"
// A slight modification Myrberg-Feigenbaum point (x= -1.401155189116)
#define FOCAL_X -1.40116569029
#define DEFAULT_SCALE 4
#define DECAY_RATE .75
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

    int pid = 0;
    for(int e = 0; e < log2(n_procs); e++){
        int id = fork();
        if (id == -1){
            printf("fork error\n");
            exit(1);
        }
        pid |= (id == 0) << e;
    }
    if (pid >= n_procs){
        exit(0); // excess children not needed
    }

    int start = (pid * N_IMGS / n_procs) + 1;
    specs.xscale *= pow(DECAY_RATE, start - 1);
    for(int i = start; i < ((pid+1) * N_IMGS / n_procs) + 1; i++){
        sprintf(specs.outfile, "imgs/mandel-%d.jpg", i);
        specs.xscale *= DECAY_RATE;
        specs.xcenter = FOCAL_X - FOCAL_X * specs.xscale / DEFAULT_SCALE;
        generate(specs);
    }
    while(wait(NULL) > 0);

    if (pid == 0){
        char command[256];
        int framerate = N_IMGS / VIDEO_LENGTH;

        sprintf(command, "ffmpeg -framerate %d -i imgs/mandel-%cd.jpg -r %d mandel%dfps.mpg",
                    framerate, '%', N_IMGS, framerate);

        system(command);
    }

    return 0;
}