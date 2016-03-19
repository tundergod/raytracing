#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "primitives.h"
#include "raytracing.h"

#define OUT_FILENAME "out.ppm"

#define ROWS 512
#define COLS 512
#define NUM_OF_THREADS 8

/*
struct thread_args
{
        uint8_t *pixels;
        color *background_color;
        rectangular_node rectangulars;
        sphere_node spheres;
        light_node lights;
        const viewpoint *view;
        int width;
        int height;
};
*/

static void write_to_ppm(FILE *outfile, uint8_t *pixels,
                         int width, int height)
{
    fprintf(outfile, "P6\n%d %d\n%d\n", width, height, 255);
    fwrite(pixels, 1, height * width * 3, outfile);
}

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

int main()
{
//    uint8_t *pixels;
    light_node lights = NULL;
    rectangular_node rectangulars = NULL;
    sphere_node spheres = NULL;
//    color background = { 0.0, 0.1, 0.1 };
    struct timespec start, end;

//    define threads and data
    pthread_t mythreads[NUM_OF_THREADS];
    thread_args thread_data;

#include "use-models.h"
    
    thread_data.lights = lights;
    thread_data.rectangulars = rectangulars;
    thread_data.spheres = spheres;
    thread_data.view = &view;
    thread_data.width = COLS;
    thread_data.height = ROWS; 
    
    /* allocate by the given resolution */
    thread_data.pixels = malloc(sizeof(unsigned char) * ROWS * COLS * 3);
    if (!thread_data.pixels) exit(-1);

    printf("# Rendering scene\n");
    /* do the ray tracing with the given geometry */
    clock_gettime(CLOCK_REALTIME, &start);



    //crete 4 thread for 4 parts
    for(int i = 0; i < NUM_OF_THREADS; i++){
        int check_thread = pthread_create(&mythreads[i],NULL,(void*)raytracing, (void *)&thread_data);
	if(check_thread){
	    fprintf(stderr,"Error creating thread.\n");
	    return -1;
	}
//	else
//	    printf("cre sc");
    }
    for(int i = 0;i < NUM_OF_THREADS; i++){
        int check_thread = pthread_join(mythreads[i],NULL);
	if(check_thread){
	    fprintf(stderr,"Error joining thread.\n");
	    return -1;
	}
    }

    //raytracing(pixels, background,rectangulars, spheres, lights, &view, ROWS, COLS);

    clock_gettime(CLOCK_REALTIME, &end);
    {
        FILE *outfile = fopen(OUT_FILENAME, "wb");
        write_to_ppm(outfile, thread_data.pixels, ROWS, COLS);
        fclose(outfile);
    }

    delete_rectangular_list(&thread_data.rectangulars);
    delete_sphere_list(&thread_data.spheres);
    delete_light_list(&thread_data.lights);
    free(thread_data.pixels);
    printf("Done!\n");
    printf("Execution time of raytracing() : %lf sec\n", diff_in_second(start, end));
    return 0;
}   
