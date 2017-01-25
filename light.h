#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "Vector.h"
#include "ColorSlider.h"
#include <iostream>

namespace lux{
class light
{
    public:
        light(){};
        light(const light& oldLight) :
            c(oldLight.c),
            dir(oldLight.dir),
            pos(oldLight.pos),
            intensity(oldLight.intensity),
            USE_SLIDER(oldLight.USE_SLIDER),
            cSlider(oldLight.cSlider){};

        light(Color C, Vector Dir, Vector Pos, float I) :
            c(C),
            dir(Dir.unitvector()),
            pos(Pos),
            intensity(I),
            USE_SLIDER(0),
            cSlider(){};
        ~light(){};

        const Color getColor(float transmission) {
            //std::cout << "slider1: " << cSlider.get<< "\n";
            if(1){ return cSlider.getColor(transmission);}
            return c;
        }

        Color c;
        Vector dir;
        Vector pos;
        float intensity;
        int USE_SLIDER;
        lux::ColorSlider cSlider; 
};
}

#endif
