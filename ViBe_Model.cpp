#include <vil/vil_image_view.h>
#include <vbl/vbl_array_2d.h>

#include <vnl/vnl_random.h>

#include "ViBe_Model.h"

#ifndef _VIL_SAVE_
#define _VIL_SAVE_
#include <vil/vil_save.h>
#endif

#ifndef _VIL_LOAD_
#define _VIL_LOAD_
#include <vil/vil_load.h>
#endif

#ifndef __VIBE_PIXEL_H__
#define __VIBE_PIXEL_H__
#include "ViBe_Pixel.h"
#endif

#ifndef _TIME_
#define _TIME_
#include <time.h>
#endif

ViBe_Model::ViBe_Model()
{

}

ViBe_Model::~ViBe_Model()
{
    delete randomNumberGenerator;
    /// deallocate model***
    for (int i=0; i<width; i++)
    {
        for (int j =0; j<height; j++)
        {
            //~model[i][j];
            delete model[i][j];
        }
        delete[] model[i];
    }
    delete[] model;
}

void ViBe_Model::Init(int Samples, int Radius, int MinSamplesBackground, int RandomSubsampling, int Width, int Height)
{
    samples = Samples;
    radius = Radius;
    minSamplesBackground = MinSamplesBackground;
    randomSubsampling = RandomSubsampling;
    width = Width;
    height = Height;

    unsigned long seed = 9667566;
    randomNumberGenerator = new vnl_random(seed);

    model = new ViBe_Pixel**[width];
    for (int i=0; i<width; i++)
    {
        model[i] = new ViBe_Pixel*[height];
        for (int j=0; j<height; j++)
        {
            model[i][j] = new ViBe_Pixel(samples);
        }
    }
}

void ViBe_Model::InitBackground(vcl_vector<vcl_string> filenames)
{
    //vcl_cout << numTrainingImages << vcl_endl;
    for (int n = 0; n < samples; n++)
    {
        vil_image_view<unsigned char> inputImage = vil_load(filenames[n].c_str());
        for (int i=0; i<inputImage.ni(); i++)
        {
            for (int j=0; j<inputImage.nj(); j++)
            {
                unsigned char pixel[3] = { inputImage(i,j,0),inputImage(i,j,1),inputImage(i,j,2) };

                ViBe_Pixel* background_memory = model[i][j];

                background_memory->addSample(pixel);
            }
        }
    }
}

// output is a single plane image
void ViBe_Model::Segment(vil_image_view<unsigned char>& input, vil_image_view<unsigned char>& output)
{
    for (int i=0; i< input.ni(); i++)
    {
        for (int j=0; j< input.nj(); j++)
        {
            unsigned char pixel[3] = { input(i,j,0),input(i,j,1),input(i,j,2) };

            // 1. Compare pixel to background model
            int count = this->ComparePixel( *(model[i][j]), pixel);
            /// Foreground or background? If our pixel is similar to at least
            /// minsamplesBackground pixels, then we have seen this colour before, and
            /// the pixel is background.
            //vcl_cout << count << vcl_endl;
            int rand;
            if (count >= minSamplesBackground)
            {
                output(i,j,0) = BACKGROUND;
                //update current pixel model
                rand = randomNumberGenerator->lrand32(randomSubsampling-1);
                //vcl_cout << rand << vcl_endl;
                if (rand == 0)
                {
                    this->UpdateModel( *(model[i][j]), pixel);
                }
                // update a random neighbouring pixel's model
                rand = randomNumberGenerator->lrand32(randomSubsampling-1);
                if (rand == 0)
                {
                    int newX; int newY;
                    this->PickNeighbour(i,j,newX,newY,input);
                    this->UpdateModel( *(model[newX][newY]), pixel);
                }
            }
            else
            {
                output(i,j,0) = FOREGROUND;
            }
        }
    }

}

void ViBe_Model::UpdateModel( ViBe_Pixel& background_model, unsigned char* pixel)
{

    /// random subsampling
    /// replace randomly chosen sample
    int rand = randomNumberGenerator->lrand32( background_model.getNumSamples() - 1 );
    background_model.addSample( pixel, rand);
    numUpdates++;
}

void ViBe_Model::PickNeighbour(int x, int y, int& nX, int& nY, vil_image_view <unsigned char>& input)
{
    while(1)
    {
        int rand = randomNumberGenerator->lrand32(7);

        switch (rand)
        {
        case 0:
            nX = x-1;
            nY = y+1;
            break;
        case 1:
            nX = x;
            nY = y+1;
            break;
        case 2:
            nX = x+1;
            nY = y+1;
            break;
        case 3:
            nX = x+1;
            nY = y;
            break;
        case 4:
            nX = x+1;
            nY = y-1;
            break;
        case 5:
            nX = x;
            nY = y-1;
            break;
        case 6:
            nX = x-1;
            nY = y-1;
            break;
        case 7:
            nX = x-1;
            nY = y;
            break;
        }
        if ( (nX>=0) && nX<input.ni() )
        {
            if ( (nY>=0) && (nY<input.nj()) )
            {
                return;
            }
        }
    }
}

//void ViBe_Model::CreateModel()
//{
//}

int ViBe_Model::ComparePixel(ViBe_Pixel& background_model, unsigned char* pixel)
{
    int count=0; int index = 0; int dist = 0;
    while ((count < minSamplesBackground) && (index < samples) )
    {
        unsigned char** samples = background_model.getSamples();
        dist = ViBe_Pixel::euclideanDist( samples[index], pixel);
        if (dist < radius)
        {
            count++;
        }
        index++;
    }
    return count;
}
