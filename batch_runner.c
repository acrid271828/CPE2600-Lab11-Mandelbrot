#include <stdio.h>
#include <math.h>
#include "mandel.h"
// #define FOCAL_X -1.401155189116 
// A slight modification Myrberg-Feigenbaum point
#define FOCAL_X -1.40116569029
#define DEFAULT_SCALE 4
#define DECAY_RATE 0.75


int main(){
    MandelSpecs specs = {
        .outfile = "mandel.jpg",
        .xcenter = 0,
        .ycenter = 0,
        .xscale = DEFAULT_SCALE / DECAY_RATE,
        .yscale = 0, // calc later
        .image_width = 1000,
        .image_height = 1000,
        .max = 1000,
    };

    


    for(int i = 1; i <= 50; i++){
        sprintf(specs.outfile, "imgs/mandel-%d.jpg", i);
        specs.xscale *= DECAY_RATE;
        specs.xcenter = FOCAL_X - FOCAL_X * specs.xscale / DEFAULT_SCALE;
        generate(specs);
    }
}