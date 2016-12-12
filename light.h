#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "Vector.h"
#include "Color.h"

namespace lux{
class light
{
    public:
        light(){};
        light(const light& oldLight) :
            c(oldLight.c),
            dir(oldLight.dir),
            pos(oldLight.pos),
            intensity(oldLight.intensity){};

        light(Color C, Vector Dir, Vector Pos, float I) :
            c(C),
            dir(Dir.unitvector()),
            pos(Pos),
            intensity(I){};
        ~light(){};
        Color c;
        Vector dir;
        Vector pos;
        float intensity;
};
}

#endif
