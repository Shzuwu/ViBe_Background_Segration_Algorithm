#ifndef __VIBE_BARNICH_H__
#define __VIBE_BARNICH_H__

#include <vil/vil_image_view.h>
#include <vnl/vnl_random.h>

#include "ViBe_Pixel.h"

#ifndef _VCL_VECTOR_
#define _VCL_VECTOR_
#include <vcl_vector.h>
#endif

/*
 * Implementation of "ViBe: A Universal Background Subtraction Algorithm for Video Sequences"
 *
 * A citation:

		@ARTICLE{5672785,
		author={Barnich, O. and Van Droogenbroeck, M.},
		journal={Image Processing, IEEE Transactions on},
		title={ViBe: A Universal Background Subtraction Algorithm for Video Sequences},
		year={2011},
		volume={20},
		number={6},
		pages={1709-1724},
		keywords={image motion analysis;image sequences;computation speed;detection rate;downscaled version performance;motion detection;neighboring pixel background model;universal background subtraction algorithm;video sequences;Adaptation model;Computational modeling;Estimation;Image color analysis;Lighting;Pixel;Subtraction techniques;Background subtraction;computer vision;image motion analysis;image segmentation;learning (artificial intelligence);pixel classification;real-time systems;surveillance;video signal processing;vision and scene understanding;Algorithms;Artificial Intelligence;Image Enhancement;Image Interpretation, Computer-Assisted;Pattern Recognition, Automated;Reproducibility of Results;Sensitivity and Specificity;Signal Processing, Computer-Assisted;Subtraction Technique;Video Recording},
		doi={10.1109/TIP.2010.2101613},
		ISSN={1057-7149},}
 */

class ViBe_Model
{
public:

    /*
     * Constructor
     */
	ViBe_Model();

    /*
     * Destructor
     */
	~ViBe_Model();

    /*
     * Initialise the model, the following parameters are provided
     * Samples -              how many samples to keep for each pixel
     * Radius -               how close a sample needs to be to an input pixel for it to be a match
     * MinSamplesBackground - the number of matches required for a pixel to be considered background (i.e. how many of
                              "samples" must be within distance "radius")
     * RandomSubsampling -    how often to randomly replace a sample. Larger values will result in less random sampling
                              (i.e. 10 indicates approx 1 replacement every 10 frames)
     * Width -                width of the input images
     * Height -               height of the input images
     */
	void Init(int Samples, int Radius, int MinSamplesBackground, int RandomSubsampling, int Width, int Height);

    /*
     * Compute the background segmentation for an image
     * input - input image to be segmented, expected to be a RGB image of size Width x Height
     * output - output image, same size as input image, single channel image (i.e. gray scale). Pixels that are background
                should be set to 0, pixels that are foreground should be 255
     */

    void InitBackground(int numTrainingImages, vcl_vector<vcl_string> filenames);

	void Segment(vil_image_view<unsigned char>& input, vil_image_view<unsigned char>& output);

	void UpdateModel( ViBe_Pixel& background_model, unsigned char* pixel, int x, int y);

protected:

    /*
     * Pick a neighbouring pixel to update. Need to ensure that the selected pixel is a valid location (i.e. in the image)
     * x -  X location of pixel to find a neighbour for
     * y -  Y location of pixel to find a neighbour for
     * nX - X locaon of neighbouring pixel to update. |x - nX| <= 1, 0 <= nX < Width
     * nY - Y locaon of neighbouring pixel to update. |y - nY| <= 1, 0 <= nY < Height
     */
	void PickNeighbour(int x, int y, int& nX, int& nY);

    /*
     * Create the model. Initialise all data structures. Should be called from Init once all parameters have been set
     */
	void CreateModel();

	int ComparePixel(ViBe_Pixel& background_model, unsigned char* pixel);

    // Model Parameters
	int samples;                // number of samples per pixel
	int radius;                 // target distance when matching pixel
	int minSamplesBackground;   // number of samples to match to be considered background
	int randomSubsampling;      // how often to update the samples

	int width;                  // model width
	int height;                 // model height

	ViBe_Pixel*** model;        // array of ViBe_Pixel's that models the background
								// this is a 2D array of pointers to ViBe_Pixel's, that represents the background
								// each pixel in the image has a corresponding entry in the background model (this array)
								// that models what we expect to see at each location

	int numUpdates;             // how many updates bave been performed

	vnl_random* randomNumberGenerator;  // a random number generator to generate values to determine the random sampling

private:
};

#endif
