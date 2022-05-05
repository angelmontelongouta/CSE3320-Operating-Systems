
#include "bitmap.h"
#include <pthread.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>

//Struct parameters created for the threading. Data would be transfers through here
//and stored in array of however many threads the user selects
struct parameters
{
  int thread_id;
	struct bitmap *bm;
  double xmin;
	double xmax;
	double ymin;
	double ymax;
	int num_threads;
	int max;
};

int iteration_to_color( int i, int max );
int iterations_at_point( double x, double y, int max );
void * compute_image( void * arg );

void show_help()
{
	printf("Use: mandel [options]\n");
	printf("Where options are:\n");
	printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
	printf("-x <coord>  X coordinate of image center point. (default=0)\n");
	printf("-y <coord>  Y coordinate of image center point. (default=0)\n");
	printf("-s <scale>  Scale of the image in Mandlebrot coordinates. (default=4)\n");
	printf("-W <pixels> Width of the image in pixels. (default=500)\n");
	printf("-H <pixels> Height of the image in pixels. (default=500)\n");
	printf("-o <file>   Set output file. (default=mandel.bmp)\n");
	printf("-h          Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
	printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
	printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}

int main( int argc, char *argv[] )
{
	char c;

	// These are the default configuration values used
	// if no command line arguments are given.

	const char *outfile = "mandel.bmp";
	double xcenter = 0;
	double ycenter = 0;
	double scale = 4;
	int    image_width = 500;
	int    image_height = 500;
	int    max = 1000;
	int 	 number_threads = 1;

	// For each command line argument given,
	// override the appropriate configuration value.

	while((c = getopt(argc,argv,"x:y:s:W:H:m:n:o:h"))!=-1) {
		switch(c) {
			case 'x':
				xcenter = atof(optarg);
				break;
			case 'y':
				ycenter = atof(optarg);
				break;
			case 's':
				scale = atof(optarg);
				break;
			case 'W':
				image_width = atoi(optarg);
				break;
			case 'H':
				image_height = atoi(optarg);
				break;
			case 'm':
				max = atoi(optarg);
				break;
			case 'n':
				number_threads = atoi(optarg);
				break;
			case 'o':
				outfile = optarg;
				break;
			case 'h':
				show_help();
				exit(1);
				break;
		}
	}

	// Display the configuration of the image.
	printf("mandel: x=%lf y=%lf scale=%lf max=%d number of threads=%d outfile=%s\n",xcenter,ycenter,scale,max,number_threads,outfile);

	// Create a bitmap of the appropriate size.
	struct bitmap *bm = bitmap_create(image_width,image_height);

	// Fill it with a dark blue, for debugging
	bitmap_reset(bm,MAKE_RGBA(0,0,255,0));


	//Initialize the thread array because they are about to be created and use to be 
	//passed to compute_image
	pthread_t tid[number_threads];
	struct parameters params[number_threads];

	int i;

	//Running a for loop to insert the data for each specific thread. 
	//this is the data that would passed in normally through the comput_image line
	//but since we are running thread I had to do it like this. Storing the parameters
	//in struct and the passing them through the (void *)
	for( i = 0; i < number_threads; i++ )
  {
	  params[i].bm = bm;
	  params[i].xmin = xcenter-scale;
		params[i].xmax = xcenter+scale;
		params[i].ymin = ycenter-scale;
		params[i].ymax = ycenter+scale;
		params[i].num_threads = number_threads;
		params[i].max = max;
		params[i].thread_id = i;

    pthread_create( &tid[i], NULL, compute_image, (void *) &params[i] );
  }

 //For loop waits for every thread to fininsh before moving on to the rest of the
 //program, in the case end of the program.  

	for( i = 0; i < number_threads; i++ )
  {
    pthread_join( tid[i], NULL );
  }

	// Compute the Mandelbrot image
	//compute_image(bm,xcenter-scale,xcenter+scale,ycenter-scale,ycenter+scale,number_threads,max);

	// Save the image in the stated file.
	if(!bitmap_save(bm,outfile)) {
		fprintf(stderr,"mandel: couldn't write to %s: %s\n",outfile,strerror(errno));
		return 1;
	}

	return 0;
}

/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/
//Did change the command line to be able to accept threads 
void * compute_image( void * arg )
{
	//Initializing all of the variable need so I can then fill them in with the 
	// struct parameters. 
	int i,j;
	struct bitmap *bm;
  double xmin;
	double xmax;
	double ymin;
	double ymax;
	int number_threads;
	int max;

	struct parameters * params = (struct parameters *) arg;

	//Bring the parameters from the struct to the variables of the function
	//to then be used in the actual process of making the image. 
	bm = params -> bm;
  xmin = params -> xmin;
	xmax = params -> xmax;
	ymin = params -> ymin;
	ymax = params -> ymax;
	number_threads = params -> num_threads;
	max = params -> max;

	int width = bitmap_width(bm);
	int height = bitmap_height(bm);


	//calculating the work each thread will actually be doing. Dividing it up
	//evenly between the number of threads they are
	int begin = height / number_threads * params->thread_id;
    int end   = ( begin + height / number_threads );

	//Did not need to use height in this for loop and replaced it with begin
	//and end beause each thread was taking a specific section and not the whole 
	//thing.
	for( j = begin; j < end; j++ )
	{
		// For every pixel in the image...
			for(i=0;i<width;i++)
			{

				// Determine the point in x,y space for that pixel.
				double x = xmin + i*(xmax-xmin)/width;
				double y = ymin + j*(ymax-ymin)/height;

				// Compute the iterations at that point.
				int iters = iterations_at_point(x,y,max);

				// Set the pixel in the bitmap.
				bitmap_set(bm,i,j,iters);
			}
	}
	return NULL;
}

/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point( double x, double y, int max )
{
	double x0 = x;
	double y0 = y;

	int iter = 0;

	while( (x*x + y*y <= 4) && iter < max ) {

		double xt = x*x - y*y + x0;
		double yt = 2*x*y + y0;

		x = xt;
		y = yt;

		iter++;
	}

	return iteration_to_color(iter,max);
}

/*
Convert a iteration number to an RGBA color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/

int iteration_to_color( int i, int max )
{
	int gray = 255*i/max;
	return MAKE_RGBA(gray,gray,gray,0);
}
