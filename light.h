#include "Vector.h"
#include "Color.h"

namespace lux{
class light
{
    public:
        light(){};
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



