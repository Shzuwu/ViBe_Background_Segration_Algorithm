#include "ViBe_Model.h"

#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

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
	///    - a descritpion of this arguement, i.e. what it's for
	///    - an optional default value.
	vul_arg<vcl_string>
		arg_in_path("-path", "Input path, i.e. C:/somefiles/"),
		arg_in_glob("-glob", "Input glob, i.e. *png, this will get all png's.");

	/// now we have some integer arguments
	vul_arg<unsigned> arg_number("-n", "A number.", 4),
		arg_number_again("-n2", "another number.", 4);

	/// finally, we have some floats
	vul_arg<float> arg_float("-f", "A float", 4.0);

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

	if (filenames.size() == 0)
    {
        vcl_cout << "No input files, exiting." << vcl_endl;
        return 0;
    }

    vil_image_view<unsigned char> anImage = vil_load(filenames[0].c_str());

    ViBe_Model Model;
    Model.Init(NUM_SAMPLES, RADIUS, MINSAMPLES, SUBSAMPLING, anImage.ni(), anImage.nj());

    Model.InitBackground(NUM_TRAINING_IMAGES, filenames);



	/// filenames now contain all of the files with our target extension in our directory, if we want to loop through them, we can now do
	for (int i = 0; i < filenames.size(); i++)
	{
		//vcl_cout << filenames[i].c_str() << vcl_endl;

		/// do something with filenames[i]
		/// if filenames[i] is an image, we might want to load it, so we could do:
		vil_image_view<unsigned char> anImage = vil_load(filenames[i].c_str());

		vcl_stringstream outputFilename;
        outputFilename << "output/" << "basicTest_" << i << ".png";
        vil_save(anImage, outputFilename.str().c_str());

		// we could now do other things with this file, such as run it through a motion segmentation algorithm
	}
}
