#ifndef __VIBE_PIXEL_H__
#define __VIBE_PIXEL_H__



#ifndef _DEFINES_
#include "defines.h"
#endif

#ifndef _INCLUDES_
#include "includes.h"
#endif


/*
 * Class to represent a single pixel within the ViBe background model
 * Stores a set of values that capture the historical value at the pixel location, and
 * allows for new pixels to be compared to this list to determine if they are foreground
 * or background. The list of pixels can also be updated, to allow new information on
 * the background to be incorporated.
 *
 */

class ViBe_Pixel
{
public:
    ViBe_Pixel();
    void addSample(unsigned char* pixel, int index);
    void addSample(unsigned char* pixel);
    unsigned char** getSamples();
    static int euclideanDist(unsigned char* pixel, unsigned char* background_sample);
    void debugString();
    int getNumSamples();
protected:
private:
    unsigned char** samples;
    int numSamples;
};

#endif
