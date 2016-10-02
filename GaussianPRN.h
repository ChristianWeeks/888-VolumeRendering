
#ifndef __GAUSSIANPRN_H__
#define __GAUSSIANPRN_H__

#include "UniformPRN.h"


namespace lux
{


class GaussianPRN : public UniformPRN
{
  public:

    GaussianPRN(){}

   ~GaussianPRN(){}

    void setParameters( const Noise_t& n ) 
    {
       standardDeviation = n.gaussianstandarddeviation;
       generator.setParameters( n ); 
    }

    const float eval()
    {
       // from 
       //    http://www.bearcave.com/misl/misl_tech/wavelets/hurst/random.html
       // with slight modification
       float x, y, r2;

       do
          {
             /* choose x,y in uniform square (-1,-1) to (+1,+1) */
             x = -1 + 2 * generator.eval();
             y = -1 + 2 * generator.eval();

             /* see if it is in the unit circle */
             r2 = x * x + y * y;
          } while (r2 > 1.0 || r2 == 0);

          /* Box-Muller transform */
          return y * sqrt (-2.0 * log (r2) / r2);
    }

  private:

    UniformPRN generator;
    float standardDeviation;


};



}

#endif
