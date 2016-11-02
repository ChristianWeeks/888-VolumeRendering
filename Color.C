#include "Color.h"
namespace lux{
    std::ostream &operator<< (std::ostream& output, const Color &c) {
       output << '(' << c[0] << ", "  << c[1] << ", " << c[2] << ", " << c[3] << ')';
       return output;
    }
}
