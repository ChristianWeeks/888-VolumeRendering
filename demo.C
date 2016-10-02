


// Code to demo the usage of several piecies of code

//===============================================================================
//
//  Ray marcher for rendering volumes
//
//===============================================================================





// Standard utilities and system includes, plus project specific items
//===============================================================================
using namespace std;

#include "CmdLineFind.h"
#include "Image.h"
#include "LABLogo.h"
#include "OIIOFiles.h"
#include "ProgressMeter.h"

using namespace std;
using namespace lux;


void draw( Image& im )
{
   int nx = im.Width();
   int ny = im.Height();


   ProgressMeter meter( ny, "demo" );

   for( int j=0;j<ny/4;j++ )
   {
      for( int i=0;i<nx;i++ )
      {
         float r = 0;
         if( i <= nx/2 )
	 {
            r = 2.0*(float)i/(float)nx;
	    int ir = (int)(16*r);
	    r = (float)ir/16.0;
         }
	 im.value(i,j,0) = im.value(i,j,1) = im.value(i,j,2) = r;
	 im.value(i,j,3) = 1.0;
      }
      meter.update();
   }
   for( int j=ny/4;j<ny;j++ )
   {
      for( int i=0;i<nx/8;i++)
      {
	 im.value(i,j,0) = im.value(i,j,1) = im.value(i,j,2) = 0.5;
	 im.value(i,j,3) = 1.0;
      }
      for( int i=nx/8;i<2*nx/8;i++)
      {
	 im.value(i,j,0) = 1.0;
	 im.value(i,j,1) = 1.0;
	 im.value(i,j,2) = 0.0;
	 im.value(i,j,3) = 1.0;
      }
      for( int i=2*nx/8;i<3*nx/8;i++)
      {
	 im.value(i,j,0) = 0.5;
	 im.value(i,j,1) = 0.5;
	 im.value(i,j,2) = 1.0;
	 im.value(i,j,3) = 1.0;
      }
      for( int i=3*nx/8;i<4*nx/8;i++)
      {
	 im.value(i,j,0) = 0.0;
	 im.value(i,j,1) = 1.0;
	 im.value(i,j,2) = 0.0;
	 im.value(i,j,3) = 1.0;
      }
      for( int i=4*nx/8;i<5*nx/8;i++)
      {
	 im.value(i,j,0) = 1.0;
	 im.value(i,j,1) = 0.0;
	 im.value(i,j,2) = 1.0;
	 im.value(i,j,3) = 1.0;
      }
      for( int i=5*nx/8;i<6*nx/8;i++)
      {
	 im.value(i,j,0) = 1.0;
	 im.value(i,j,1) = 0.0;
	 im.value(i,j,2) = 0.0;
	 im.value(i,j,3) = 1.0;
      }
      for( int i=6*nx/8;i<7*nx/8;i++)
      {
	 im.value(i,j,0) = 0.0;
	 im.value(i,j,1) = 0.0;
	 im.value(i,j,2) = 1.0;
	 im.value(i,j,3) = 1.0;
      }
      for( int i=7*nx/8;i<nx;i++)
      {
	 im.value(i,j,0) = 1;
	 im.value(i,j,1) = 1;
	 im.value(i,j,2) = 1;
	 im.value(i,j,3) = 1.0;
      }
      meter.update();
   }
}

// Main program
//===============================================================================
int main(int argc, char** argv)
{
    cout << LABLOGOSTRING << flush;
    CmdLineFind clf( argc, argv );


    int imageWidth  = clf.find( "-NX", 512, "Image width");
    int imageHeight = clf.find( "-NY", 512, "Image height");
    string filename = clf.find( "-name", "demo.exr", "Name of output image file" );

    clf.usage("-h");


    Image image;
    image.reset( imageWidth, imageHeight );

    draw( image );

    writeOIIOImage( filename.c_str(), image, clf.mapFinds() );


    clf.printFinds();
}
