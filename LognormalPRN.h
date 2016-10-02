
#ifndef __LOGNORMALPRN_H__
#define __LOGNORMALPRN_H__

#include "GaussianPRN.h"


namespace lux
{


class LognormalPRN : public PRN
{
  public:

    LognormalPRN() : mean (1.0), devFactor (-0.5) {}

   ~LognormalPRN(){}

    void setParameters( const Noise_t& n ) 
    { 
       mean = n.lognormalmean;
       devFactor = -0.5*n.gaussianstandarddeviation*n.gaussianstandarddeviation;
       generator.setParameters( n ); 
    }

    const float eval()
    {
       float x = devFactor + generator.eval();
       return std::exp(x) * mean;
    }

  private:

    GaussianPRN generator;
    float mean;
    float devFactor;


};



}

#endif

