#include "ViBe_Model.h"

#include <vil/vil_image_view.h>

#ifndef _VIL_LOAD_
#define _VIL_LOAD_
#include <vil/vil_load.h>
#endif

#ifndef _VIL_SAVE_
#define _VIL_SAVE_
#include <vil/vil_save.h>
#endif

#include <vul/vul_file_iterator.h>

#ifndef _VUL_FILE_
#define _VUL_FILE_
#include <vul/vul_file.h>
#endif

#ifndef _VCL_VECTOR_
#define _VCL_VECTOR_
#include <vcl_vector.h>
#endif


#include <vul/vul_arg.h>

/*
 * Main program to run the ViBe motion detection algorithm.
 * This program will
 *  - take a list of images on the command line, specified as a path and a partial filename (i.e. *.png to get all png files)
 *  - take a set of parameters for the ViBe segmenter from the command line, if no value is given defaults will be used
 *  - will save a set of images showing the motion segmented output
 *  - will optionally compute performance metrics using a given ground truth image and index
 */

int main (int argc, char * argv[])
{
                                //vil_image_view<unsigned char> test= vil_load("Data/Sequence1/groundtruth.bmp");

                                //vcl_cout << "HelloWorld!" << vcl_endl;
                                //double test_var = test(100,100,0);
                                //vcl_cout <<  test_var << vcl_endl;

	/// specify all the arguments you expect/need
	/// group them according to type, i.e. all the strings first here, so we have vul_arg<vcl_string>
	/// for each command line argument, you have create a "vul_arg", this takes the following three arguments:
	///	  - the argument itself, this is what you expect to see on the command line when you call it
	///    - a description of this arguement, i.e. what it's for
	///    - an optional default value.
	vul_arg<vcl_string>
		arg_in_path("-path", "Input data path, i.e. C:/somefiles/", "Data/Sequence1"),
		arg_in_glob("-glob", "Input glob, i.e. *png, this will get all png's.", "*jpeg"),
		arg_in_outputPath("-out", "Path to output background segmented images (default = 'C:/../source file directory/output')", "output");


	/// now we have some integer arguments
	vul_arg<unsigned int>
        arg_in_minSamples("-min", "minimum number of similar samples required to declare a pixel is background (default = 2)", 2),
		arg_in_radius("-r", "radius of sphere for acceptably similar nearby pixels in the rgb colour space (default = 20)", 20),
		arg_in_subsampling("-sub","stochastic rate at which background pixels are updated (default = 16)",16),
		arg_in_numTrainingImages("-train","the number of images used to train the initial background model (default = 20)",20),
        arg_in_accuracy("-acc", "Run comparison to ground truth? Boolean value either 1 or 0 (default == 0)");

	/// finally, we have some floats
	//vul_arg<float> arg_float("-f", "A float", 4.0);

	/// call vul_arg_parse(argc, argv) to parse the arguments, this will go through the command line string, look for all the specified argument string,
	/// and extract the provided values.
	vul_arg_parse(argc, argv);

	/// now we can start to access our arguments, to do it, simply call the created vul_arg object. i.e. arg_in_path() will return the value that was found after the path argument.
	/// So, the command line: "<program name> -path c:\nothing -glob * -n 6 -f 8" would yield the following:
	///		arg_in_path() 		-> c:\nothing
	///		arg_in_glob() 		-> *
	///		arg_number()  		-> 6
	///		arg_number_again()	-> 4 			(NOTE: default value is used as this paramter is not specified on the command line)
	///		arg_float()			-> 8.0
	///
	/// we can use this to check that required variables were provided
	/// if arg_in_path() or arg_in_glob() are empty strings, it means they weren't provided
	if (((arg_in_path() == "") || (arg_in_glob() == "")))
	{
		/// if we need these arguments to proceed, we can now exit and print the help as we quit. vul_arg_display_usage_and_exit() will display the available arguments
		/// alongside the help message specified when the vul_arg objects were created
		vul_arg_display_usage_and_exit();
	}


	/// Parsing a directory of images
	/// this is a list to store our filenames in
	vcl_vector<vcl_string> filenames;

	/// Get the directory we're going to search, and what we're going to search for. We'll take these from the vul_arg's we used above
	vcl_string directory = arg_in_path();
	vcl_string extension = arg_in_glob();

	/// loop through a directory using a vul_file_iterator, this will create a list of all files that match "directory + "/*" + extension", i.e. all files in the directory
	/// that have our target extension
	for (vul_file_iterator fn=(directory + "/*" + extension); fn; ++fn)
	{
		/// we can check to make sure that what we are looking at is a file and not a directory
		if (!vul_file::is_directory(fn()))
		{
			/// if it is a file, add it to our list of files
			filenames.push_back (fn());
		}
	}

	if (!vul_file::is_directory(arg_in_outputPath() ) )
    {
        vcl_cout << "Invalid output directory, exiting." << vcl_endl;
        return 0;
    }


	if (filenames.size() == 0)
    {
        vcl_cout << "No input files, exiting." << vcl_endl;
        return 0;
    }

    int minSamples = arg_in_minSamples();
    int radius = arg_in_radius();
    int subSampling = arg_in_subsampling();
    int numTrainingImages = arg_in_numTrainingImages();
    int compareGroundTruth = arg_in_accuracy();

    /// Validate user input
    if ( (minSamples < 1) || (minSamples > 5000) )
    {
        vcl_cout << "-min is out of range, changing to default" << vcl_endl;
        minSamples = 2;
    }
    if ( (radius < 1) || (radius > 442) )
    {
        vcl_cout << "-r is out of range, changing to default" << vcl_endl;
        radius = 20;
    }
    if ( ( subSampling < 1 ) || (subSampling > 5000))
    {
        vcl_cout << "-sub is out of range, changing to default" << vcl_endl;
        radius = 16;
    }
    if ( ( numTrainingImages < 1 ) || (subSampling > filenames.size()))
    {
        vcl_cout << "-train is out of range, changing to default" << vcl_endl;
        numTrainingImages = 20;
    }
    if ( ( compareGroundTruth == 1 ) || (compareGroundTruth == 0))
    {
        ///Do nothing
        //compareGroundTruth = compareGroundTruth;
    }
    else
    {
        vcl_cout << "-acc is out of range, changing to default" << vcl_endl;
        compareGroundTruth = 0;
    }

    vcl_cout <<  "-min = ";
    vcl_cout << minSamples  << vcl_endl;
    vcl_cout << "-r = ";
    vcl_cout << radius  << vcl_endl;
    vcl_cout << "-sub = ";
    vcl_cout << subSampling  << vcl_endl;
    vcl_cout << "-train = ";
    vcl_cout << numTrainingImages  << vcl_endl;
    vcl_cout << "-acc = ";
    vcl_cout << compareGroundTruth << vcl_endl;
    vcl_cout << "Segmenting images in: " << directory << vcl_endl;
    vcl_cout << "With extension: " << extension << vcl_endl;
    vcl_cout << "Output dir: " << arg_in_outputPath() << vcl_endl;
    vil_image_view<unsigned char> anImage = vil_load(filenames[0].c_str());

    ///
    ViBe_Model Model;
    Model.Init(numTrainingImages, radius, minSamples, subSampling, anImage.ni(), anImage.nj());

    Model.InitBackground(filenames);

	/// filenames now contain all of the files with our target extension in our directory, if we want to loop through them, we can now do
	for (int i = 0; i < filenames.size(); i++)
	{
		//vcl_cout << filenames[i].c_str() << vcl_endl;

		/// do something with filenames[i]
		/// if filenames[i] is an image, we might want to load it, so we could do:
		vil_image_view<unsigned char> srcImage = vil_load(filenames[i].c_str());
        vil_image_view<unsigned char> resultImage( srcImage.ni(), srcImage.nj(), 1);

        Model.Segment(srcImage, resultImage);

		vcl_stringstream outputFilename;
        outputFilename << arg_in_outputPath() << "/BackgroundSegmentation_" << i << ".png";
        vil_save(resultImage, outputFilename.str().c_str());

		// we could now do other things with this file, such as run it through a motion segmentation algorithm
	}
}
