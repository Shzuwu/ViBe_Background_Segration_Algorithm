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
    samples[index][0] = pixel[0];
    samples[index][1] = pixel[1];
    samples[index][2] = pixel[2];
}

void ViBe_Pixel::addSample(unsigned char* pixel)
{
    if (numSamples < 20)
    {
    numSamples++;
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
    int distance = sqrt( (background_sample[0]-pixel[0])*(background_sample[0]-pixel[0]) +
                     (background_sample[1]-pixel[1])*(background_sample[1]-pixel[1]) +
                     (background_sample[2]-pixel[2])*(background_sample[2]-pixel[2]) );
    return distance;
}
