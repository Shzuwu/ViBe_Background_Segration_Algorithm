#include "ViBe_Pixel.h"

#ifndef _MATH_
#include <math.h>
#endif

#ifndef _VUL_FILE_
#define _VUL_FILE_
#include <vul/vul_file.h>
#endif

#ifndef _VCL_STRING_
#define _VCL_STRING_
#include <vcl_string.h>
#endif

#ifndef _VCL_IOSTREAM_
#define _VCL_IOSTREAM_
#include <vcl_iostream.h>
#endif

ViBe_Pixel::ViBe_Pixel(int maxSamples)
{
    // dynamically allocate an array of unsigned char pointers, each pointer contains the
    // address of one sample in the model of that pixel. By default there are twenty sample
    // pointers, each of which points to an array of 3 unsigned chars.

    numSamples = 0;
    samples = new unsigned char*[maxSamples];
    for (int i=0; i<maxSamples; i++)
    {
        samples[i] = new unsigned char[3];
    }
    //vcl_cout << &numSamples << vcl_endl;
}

ViBe_Pixel::~ViBe_Pixel()
{
    for (int i=0; i<numSamples; i++)
    {
        delete[] samples[i];
    }
    delete[] samples;
}

void ViBe_Pixel::debugString()
{
    vcl_cout << "You have access to the ViBe_Pixel!" << vcl_endl;
}

void ViBe_Pixel::addSample(unsigned char* pixel, int index)
{
    // insert individual pixel values into the 2D array
    samples[index][0] = pixel[0];
    samples[index][1] = pixel[1];
    samples[index][2] = pixel[2];
}

void ViBe_Pixel::addSample(unsigned char* pixel)
{
    if (numSamples < 20)
    {
    // increment the count of samples contained in the object
    numSamples++;
    // insert individual pixel values into the 2D array
    samples[numSamples-1][0] = pixel[0];
    samples[numSamples-1][1] = pixel[1];
    samples[numSamples-1][2] = pixel[2];
    }
}

unsigned char** ViBe_Pixel::getSamples()
{
    return samples;
}

int ViBe_Pixel::getNumSamples()
{
    return numSamples;
}

int ViBe_Pixel::euclideanDist(unsigned char* pixel, unsigned char* background_sample)
{
    // equivalent psuedo-code:
    // d = sqrt( (r2-r1)^2 + (g2-g1)^2 + (b2-b1)^2 );
    // calculates the distance in 3D space between two points
    int distance = sqrt( (background_sample[0]-pixel[0])*(background_sample[0]-pixel[0]) +
                     (background_sample[1]-pixel[1])*(background_sample[1]-pixel[1]) +
                     (background_sample[2]-pixel[2])*(background_sample[2]-pixel[2]) );
    return distance;
}
