
typedef struct {
    char outfile[30];
	double xcenter;
	double ycenter;
	double xscale;
	double yscale; // calc later
	int    image_width;
	int    image_height;
	int    max;
} MandelSpecs;

int generate(MandelSpecs specs);