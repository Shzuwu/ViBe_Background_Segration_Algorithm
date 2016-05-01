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
    //vnl_random RandomNumberGenerator = vnl_random(seed);
    randomNumberGenerator = new vnl_random(seed);

    //ViBe_Pixel*** model = NULL;

    //vcl_cout << model << vcl_endl;

    model = new ViBe_Pixel**[width];
    for (int i=0; i<width; i++)
    {
        model[i] = new ViBe_Pixel*[height];
    }

    for (int i=0; i<width; i++)
    {
        for (int j=0; j<height; j++)
        {
            model[i][j] = new ViBe_Pixel();
        }
    }

    /*
    vcl_cout << "Echoing the two-dimensional array: " << vcl_endl;
    for (int i=0; i<width; i++)
    {
        for (int j=0; j<height; j++)
        {
            vcl_cout << model[i][j]->getNumSamples() << vcl_endl;
        }
    }
    */
}

void ViBe_Model::InitBackground(int numTrainingImages, vcl_vector<vcl_string> filenames)
{
    //vcl_cout << numTrainingImages << vcl_endl;
    for (int n = 0; n < numTrainingImages; n++)
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
    ///Checking that the data structure is working correctly
    /*
    vil_image_view<unsigned char> inputImage = vil_load(filenames[15].c_str());

    vil_image_view<unsigned char> checkBackground( inputImage.ni(), inputImage.nj(), inputImage.nplanes() );
    checkBackground.fill(0);
    for (int i=0; i<inputImage.ni(); i++)
    {
        for (int j=0; j<inputImage.nj(); j++)
        {
            unsigned char** samples = model[i][j]->getSamples();
            checkBackground(i,j,0) = samples[15][0];
            checkBackground(i,j,1) =samples[15][1];
            checkBackground(i,j,2) =samples[15][2];
        }
    }
    vil_save(checkBackground, "CheckBackground.jpeg");
    */
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
            int count = model[i][j]->ComparePixel( *(model[i][j]), pixel);
            /// Foreground or background? If our pixel is similar to at least
            /// MINSAMPLES pixels, then we have seen this colour before, and
            /// the pixel is background.
            //vcl_cout << count << vcl_endl;
            int rand;
            if (count >= MINSAMPLES)
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
                    vcl_cout << i << vcl_endl;
                    vcl_cout << j << vcl_endl;
                    this->PickNeighbour(i,j,newX,newY,input);

                    //vcl_cout << newX << vcl_endl;
                    //vcl_cout << newY << vcl_endl;

                    this->UpdateModel( *(model[newX][newY]), pixel);

                }
            }
            else
            {
                output(i,j,0) = FOREGROUND;
            }




        }
    }
    //vil_save(output,"TestImage.jpeg");

}

void ViBe_Model::UpdateModel( ViBe_Pixel& background_model, unsigned char* pixel)
{

    /// random subsampling
    /// replace randomly chosen sample
    int rand = randomNumberGenerator->lrand32( background_model.getNumSamples() - 1 );
    background_model.addSample( pixel, rand);
}

void ViBe_Model::PickNeighbour(int x, int y, int& nX, int& nY, vil_image_view <unsigned char>& input)
{
    while(1)
    {
        nX = this->getRandomNeighbourCoord(x);
        nY = this->getRandomNeighbourCoord(y);
        if ( (nX>=0) && nX<input.ni() )
        {
            if ( (nY>=0) && (nY<input.nj()) )
            {
                vcl_cout << nX << vcl_endl;
                vcl_cout << nY << vcl_endl;
                return;
            }
        }
    }


}

void ViBe_Model::CreateModel()
{
}

int ViBe_Model::getRandomNeighbourCoord(int coord)
{
    int rand = randomNumberGenerator->lrand32(1);

    if (rand)
    {
        return coord+1;
    }
    else
    {
        return coord-1;
    }

}
