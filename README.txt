
Models and Optional code for CP SC 819.

        Models
       --------

The directory "models" contains several obj files that are "clean" in the sense needed for level set generation.


The files demo.C and noiseimage.C shows examples of using the Image class in conjunction with OpenImageIO.

        Code Files
      -------------

*CmdLineFind.h
   Routines for specifying key/value pairs on the command line, along with optional short explanations.
*Image.h and Image.C
   Image file format
*Camera.h and Camera.C
   Simple camera class
*Color.h
   Simple class for Color data and operations on color
*Vector.h, Matrix.h and Matrix.C
   Classes for 3D vectors and matrices and operations on them.
*LinearAlgebra.h and LinearAlgebra.C
   Simple linear algebra operations on Vectors and Matrices
*OIIOFiles.h and OIIOFiles.C
   Write files in many formats, including EXR.  Suitable replacement for ImageMagick.
*ProgressMeter.h
   Easy to use wall clock meter that estimates how much time is left
*Volume.h
   Templated classes for representing fields
*demo.C
   main routine for illustrating usage
*noiseimage.C
   main routine for illustrating usage of OpenImageIO and noise
*Makefile
   You know what this is
*demo.jpg
   Example of the output of the demo routine.
*LABLogo.h
   Dressing
*Noise.h and Noise.C
   This file provides several pieces of generic functionality
   1. A struct Noise_t containing noise parameters for various types of noise
   2. A Noise base class from which various types of spatial noise can be derived.
   3. a PRN base class from which various types of pseudo-random number generators can be derived.
   4. A templated FractalSum class which lets you turn an spatial noise to a fractal-summed noise
*UniformPRN.h
   Uniform random number generator class based on the Mersenne Twistor
*GaussianPRN.h
   Gaussian random number generator
*Lognormal.h
   Lognormal random number generator
*PerlinNoise.h and PerlinNoise.C
   Perlin noise implemented two different ways.  The class PerlinNoise is based on the java code Ken Perlin has on his website.
   The class PerlinNoiseGustavson is based on the implementation by Stefan Gustavson in his DSOnoise library.  This version has
   4D Perlin noise, and so is more useful than the first.
*FFTNoise.h and FFTNoise.C
   Noise generated from spatially homogeneous and isotropic statistics, implemented in terms of FFTs.






   Writing image files using OpenImageIO 

The demo routine is built by

make demo

An example run is

demo -name test.jpg

The commandline options can be found by typing "./demo -h":

========================================
CmdLine Usage:

Option	Default	Help
-NX	512	Image width
-NY	512	Image height
-name	demo.exr	Name of output image file
-h		Obtain command line help
========================================

Note that image formats other that exr can be had by using the appropriate postfix.  For example, 

   ./demo -name demo.jpg

will produce a jpeg file.  Also note that if you use exr, several bits of handy information are included in the metadata in the file.  To see the metadata, use the iinfo utility that comes with OpenImageIO. For example

   iinfo -v demo.exr

produces

demo.exr :  512 x  512, 4 channel, float openexr
    channel list: R, G, B, A
    oiio:ColorSpace: "Linear"
    compression: "zip"
    -NX: "512 (default)"
    -NY: "512 (default)"
    -name: "demo.exr (default)"
    Artist: "tessendorf"
    DocumentName: ""
    HostComputer: "snicker.local"
    Keywords: ""
    Software: "imageTools"
    DateTime: "2012-12-29 9:43:4"
    ImageDescription: ""
    Copyright: ""
    PixelAspectRatio: 1
    screenWindowWidth: 1



   OpenImageIO

This library is an open source kit for writing and reading image files, including EXR files. 
Google "Open Image IO" to find the library.  Follow the directions to install it.  You will probably 
need to first install IlmBase and OpenEXR, which are available by Googling "openexr".




Noise
------


The file noiseimage.C shows how to generate images of noise. You can also use the wedge tool
to generate a sequence of frames with animated noise from the PerlinNoiseGustavson class.





