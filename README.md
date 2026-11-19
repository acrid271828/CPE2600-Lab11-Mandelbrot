# Mandelbrot Visualizer

This project is for creating videos of the mandlebrot set.

## Execution
Edit `batch_runner.c` to have your desired constants. Most important on top:
```
FOCAL_X -1.40116569029
    The x-coordinate to zoom in towards
DECAY_RATE .75
    The amount of zoom per frame, scaled proportianally to the last frame. Make this above 1 to have the video zoom out
N_IMGS 50
    The number of frames to generate
VIDEO_LENGTH 5 
    The numbers of seconds for the video to last. N_IMGS divided by this will be the framerate

DEFAULT_SCALE 4
    The first frame will cover [-this/2, this/2]. Keep at 4 to start at a picture of the whole set
DEFAULT_N_PROCS 1
    Number of processes to use if it is not passed in as an argument
```

If you haven't already,
```bash
sudo apt install ffmpeg
sudo apt install libjpeg-dev
```

Then, run the program
```bash
make
./mandel [-p <num>]
```
where `<num>` is how many processes to use. The resulting video file will be named "mandel....mpg" with the framerate listed.

## Implementation Details
To run multiprocessing, the program loops `fork()` a number of times equal to `ceil(log2(n_procs))`, so `2^(ceil(log2(n_procs))) >= n_procs` processes are running, indexed starting from 0 (parent). Excess processes created from this operation return immediatly and don't do any work. Each process uses it's index to and generates images for that fraction of the dataset.

For example,
```
n_procs = 10
N_IMGS = 50
```
- `Fork()` runs 4 times, creating 16 processes (as 8 would be too few)
- 6 processes (indexed 10-15 inclusive) return immediately
- process index 0 (true parent) calculates `0/10 --> 1/10` of the dataset, the first 5 images.
- ... process index 2 (`0b0010`: parent's child's parent's parent) calculates `2/10 --> 3/10` of the dataset, images 11 to 15

After all processes finish, the true parent runs ffmpeg to generate the video file.

## Results
Increasing the number of processes approximately inversly scales the runtime, except minimal gains going past the number of processors on the CPU (14). The 20fps runs took over double to time on each # of processors despite being exactly double the amount of images.
![timeGraph](imgs/timeGraph.png)