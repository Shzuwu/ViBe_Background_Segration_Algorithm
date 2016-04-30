#include <vil/vil_image_view.h>
#include <vbl/vbl_array_2d.h>

#include <vnl/vnl_random.h>

#include "ViBe_Model.h"

#ifndef _VIL_LOAD_
#define _VIL_LOAD_
#include <vil/vil_load.h>
#endif

ViBe_Model::ViBe_Model()
{

}

ViBe_Model::~ViBe_Model()
{
}

void ViBe_Model::Init(int Samples, int Radius, int MinSamplesBackground, int RandomSubsampling, int Width, int Height)
{
    int samples = Samples;
    int radius = Radius;
    int minSamplesBackground = MinSamplesBackground;
    int randomSubsampling = RandomSubsampling;
    int width = Width;
    int height = Height;

    //ViBe_Pixel*** model = NULL;

    vcl_cout << model << vcl_endl;

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

    vcl_cout << "Echoing the two-dimensional array: " << vcl_endl;
    for (int i=0; i<width; i++)
    {
        for (int j=0; j<height; j++)
        {
            vcl_cout << model[i][j]->getNumSamples() << vcl_endl;
        }
    }

        /*
        for (int j=0; j<height; j++)
        {
            vcl_cout << model[i][j] << vcl_endl;
            model[i][j] = new ViBe_Pixel();
            //model[i][j]->debugString();
            vcl_cout << model[i][j]->getNumSamples() << vcl_endl;
        }
        */
    }
/*
    ViBe_Pixel* model[width][height];
    for (int i=0; i<width; i++)
    {
        for (int j=0; j<height; j++)
        {
            ViBe_Pixel* test = model[i][j];
            vcl_cout << test << vcl_endl;
        }
    }
    */

void ViBe_Model::InitBackground(int numTrainingImages, vcl_vector<vcl_string> filenames)
{
    vcl_cout << numTrainingImages << vcl_endl;
    for (int n = 0; n < numTrainingImages; n++)
    {
        vil_image_view<unsigned char> inputImage = vil_load(filenames[n].c_str());
        for (int i=0; i<inputImage.ni(); i++)
        {
            for (int j=0; j<inputImage.nj(); j++)
            {
                unsigned char pixel[3] = { inputImage(i,j,0),inputImage(i,j,1),inputImage(i,j,2) };
                //vcl_cout << (int) inputImage(i,j,0) << (int)inputImage(i,j,1) << (int)inputImage(i,j,2) << vcl_endl;

                //vcl_cout << model << vcl_endl;
                //int* test = (int *)model;
                //vcl_cout << test << vcl_endl;
                //ViBe_Pixel* test = model[i][j];

                //ViBe_Pixel* background_memory =  (ViBe_Pixel*)model + sizeof(ViBe_Pixel*)*i*inputImage.ni() + sizeof(ViBe_Pixel*)*j;

                ViBe_Pixel* background_memory = model[i][j];

                background_memory->addSample(pixel);
                if (n > 14)
                {
                    vcl_cout << n << vcl_endl;
                }
                //vcl_cout << myPixel << vcl_endl;

                //myPixel->debugString();


                //ViBe_Pixel** row = model[i];
                //vcl_cout << row << vcl_endl;

               // ViBe_Pixel* col = row[j];
                //vcl_cout << col << vcl_endl;

               // ViBe_Pixel memory = *col;
               // memory.debugString();

                //ViBe_Pixel** access_ptr = model;
                //ViBe_Pixel memory = *(model + i*inputImage.ni() + j*inputImage.nj() );

                //ViBe_Pixel* sample = model[i][j];
                //sample->debugString();

                //sample->addSample( pixel );
            }

        }
    }
}
// output is a single plane image
void ViBe_Model::Segment(vil_image_view<unsigned char>& input, vil_image_view<unsigned char>& output)
{
}

void ViBe_Model::UpdateModel( ViBe_Pixel& background_model, unsigned char* pixel, int x, int y)
{
    // 1. Compare pixel to background model
    int count = ComparePixel(background_model, pixel);
    /// Foreground or background? If our pixel is similar to at least
    /// MINSAMPLES pixels, then we have seen this colour before, and
    /// the pixel is background.

    if (count >= MINSAMPLES)
    {

    }
}

int ViBe_Model::ComparePixel(ViBe_Pixel& background_model, unsigned char* pixel)
    {
        int count=0; int index = 0; int dist = 0;
        while ((count < MINSAMPLES) && (index < NUM_SAMPLES) )
        {
            unsigned char** samples = background_model.getSamples();
            dist = ViBe_Pixel::euclideanDist( samples[index], pixel);
            if (dist < RADIUS)
            {
                count++;
            }
            index++;
        }
        return count;
    }

void ViBe_Model::PickNeighbour(int x, int y, int& nX, int& nY)
{
}

void ViBe_Model::CreateModel()
{
}

