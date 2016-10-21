#include "Vector.h"
namespace lux{
    std::ostream &operator<< (std::ostream& output, const Vector &v) {
       output << '(' << v[0] << ", "  << v[1] << ", " << v[2] << ')';
       return output;
    }
}
