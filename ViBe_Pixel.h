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
    /*
     ctor, requires integer input of the maximum number of background samples the ViBe_Pixel
     object should store
     */
    ViBe_Pixel(int maxSamples);
    /*
     dtor
     */
    ~ViBe_Pixel();

    /*
     * Stores the given pixel as a sample of the background within a ViBe_Pixel object.
     * If an index is specified, the existing sample pixel at that index will be replaced with
     * the current pixel. If an index is not specified, the next available empty index will be
     * filled
     * Inputs: pixel - a pointer to an array which contains the rgb triplet of the
                     the pixel in the input frame which has been classfied as background
                   - (optional) index - the indexed position within the array of samples stored
                    in the ViBe_Pixel.
     */
    void addSample(unsigned char* pixel, int index);
    void addSample(unsigned char* pixel);

    /*
     * getSamples() returns an array of pointers to the pixel values stored within
                    the samples member variable. Used within ViBe_Model::ComparePixel
                    to allow all samples to be easily looped over as a 2D array. A BIT
                    OF AN ANTI-PATTERN to pass values in this way however this was the
                    best fit for the classes as they were designed
     */

    unsigned char** getSamples();

    /*
        euclideanDist() returns the linear distance between two points in the rgb colour space.
        The method itself is static to avoid being tied to a particular instance of ViBe_Pixel.
        Inputs: pixel - an array pointer to an rgb triplet which represents the input frame pixel
                background_sample - an array pointer to an rgb triplet which represents a sample
                                    stored in the background model
    */

    static int euclideanDist(unsigned char* pixel, unsigned char* background_sample);

    /*
     debugString() - a dirty hack to test class instantiation early in development.
     */

    void debugString();

    /*
      getNumSamples() - returns the "numSamples" member variable of a ViBe_Pixel object. Used to
                        ensure
     */
    int getNumSamples();

protected:
private:
    unsigned char** samples;
    int numSamples;
};

#endif
