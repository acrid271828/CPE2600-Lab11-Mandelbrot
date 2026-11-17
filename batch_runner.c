#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "mandel.h"
// #define FOCAL_X -1.401155189116 
// A slight modification Myrberg-Feigenbaum point
#define FOCAL_X -1.40116569029
#define DEFAULT_SCALE 4
#define DECAY_RATE .75
#define N_PROCS 14
#define N_IMGS 50

int main(){
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
    for(int e = 0; e < log2(N_PROCS); e++){
        int id = fork();
        if (id == -1){
            printf("fork error\n");
            exit(1);
        }
        pid |= (id == 0) << e;
    }
    if (pid >= N_PROCS){
        return 1;
    }

    int start = (pid * N_IMGS / N_PROCS) + 1;
    specs.xscale *= pow(DECAY_RATE, start - 1);
    for(int i = start; i < ((pid+1) * N_IMGS / N_PROCS) + 1; i++){
        sprintf(specs.outfile, "imgs/mandel-%d.jpg", i);
        specs.xscale *= DECAY_RATE;
        specs.xcenter = FOCAL_X - FOCAL_X * specs.xscale / DEFAULT_SCALE;
        generate(specs);
    }
    while(wait(NULL) > 0);

    return 0;
}