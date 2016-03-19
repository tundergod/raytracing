#ifndef __RAYTRACING_H
#define __RAYTRACING_H

#include "objects.h"
#include <stdint.h>

typedef struct
{
        uint8_t *pixels;
        color *background_color;
        rectangular_node rectangulars;
        sphere_node spheres;
        light_node lights;
        const viewpoint *view;
        int width;
        int height;
} thread_args;

void raytracing(void *argp);
#endif
