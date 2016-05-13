#include "ViBe_Model.h"



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
    // initialise member variables from input
    samples = Samples;
    radius = Radius;
    minSamplesBackground = MinSamplesBackground;
    randomSubsampling = RandomSubsampling;
    width = Width;
    height = Height;

    // instantiate random number generator object
    unsigned long seed = 9667566;
    randomNumberGenerator = new vnl_random(seed);

    // create a 2D array of ViBe_Pixel pointers columns = width, rows = height
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
    // loop over the first "samples" number of images in the sequence to pre-populate the
    // background model
    for (int n = 0; n < samples; n++)
    {
        vil_image_view<unsigned char> inputImage = vil_load(filenames[n].c_str());
        for (int i=0; i<inputImage.ni(); i++)
        {
            for (int j=0; j<inputImage.nj(); j++)
            {
                // create an rgb array for the current input pixel to pass into the addSample method
                unsigned char pixel[3] = { inputImage(i,j,0),inputImage(i,j,1),inputImage(i,j,2) };

                // create an easy alias for the desired ViBe_Pixel pointer to access its methods
                ViBe_Pixel* background_memory = model[i][j];

                // insert the pixel value into the memory of the background model
                background_memory->addSample(pixel);
            }
        }
    }
}

void ViBe_Model::Segment(vil_image_view<unsigned char>& input, vil_image_view<unsigned char>& output)
{
    // loop over each pixel in an input image
    for (int i=0; i< input.ni(); i++)
    {
        for (int j=0; j< input.nj(); j++)
        {
            // create an rgb array for the current input pixel to pass into the addSample method
            unsigned char pixel[3] = { input(i,j,0),input(i,j,1),input(i,j,2) };

            // 1. Compare input pixel to background model - returns the number of similar pixels
            int count = this->ComparePixel( *(model[i][j]), pixel);
            /// Foreground or background? If our pixel is similar to at least
            /// minsamplesBackground pixels, then we have seen this colour before, and
            /// the pixel is background.

            int rand;
            if (count >= minSamplesBackground)
            {
                // classify the current pixel as background
                output(i,j,0) = BACKGROUND;

                //update current pixel model at a stochastic rate
                // generate a random number between 0 and 15 by default
                rand = randomNumberGenerator->lrand32(randomSubsampling-1);
                //if the random number is zero update the model
                if (rand == 0)
                {
                    this->UpdateModel( *(model[i][j]), pixel);
                }
                // update a random neighbouring pixel's model at a stochastic rate
                rand = randomNumberGenerator->lrand32(randomSubsampling-1);
                if (rand == 0)
                {
                    int newX; int newY;
                    // randomly select a pixel to update within the 8 surrounding pixels
                    this->PickNeighbour(i,j,newX,newY,input);
                    this->UpdateModel( *(model[newX][newY]), pixel);
                }
            }
            else
            {

                // otherwise, classify the current pixel as foreground
                output(i,j,0) = FOREGROUND;
            }
        }
    }

}

void ViBe_Model::UpdateModel( ViBe_Pixel& background_model, unsigned char* pixel)
{
    /// random subsampling
    /// replace randomly chosen sample
    // generate a random number between 0, and one less than the number of samples.
    // This shall be the number of the sample in the ViBe_Pixel to be replaced
    // currently stored in the ViBe_Pixel object
    int rand = randomNumberGenerator->lrand32( background_model.getNumSamples() - 1 );
    // replace the randomly selected sample with the current pixel
    background_model.addSample( pixel, rand);
    // increment the number of updates to keep track
    numUpdates++;
}

void ViBe_Model::PickNeighbour(int x, int y, int& nX, int& nY, vil_image_view <unsigned char>& input)
{
    // generate a random number between 0 and 7
    // use a switch case statement to assign the x and y coords of the neighbouring pixel
    // based on the random number generated

    // the only way to exit the infinite while loop is to satisfy conditionals at the end
    // of the loop which check whether the pixel selected is within the bounds of the image

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

    // find the distance in the rgb colour space between each of the background
    // sample pixels

    // if more than a selected threshold of those are outside a given sphere in the rgb space
    // stop the loop and return the count of pixels outside the sphere
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


