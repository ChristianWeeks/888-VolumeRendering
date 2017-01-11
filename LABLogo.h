

#ifndef __LABLOGO_H__
#define __LABLOGO_H__

#define LABSHORTLOGO ":o)"
#define LABLOGO \
    std::cout << "\n=========================================================================\n\n"; \
    std::cout <<   "                               LAUGHING\n\n"; \
    std::cout <<   "            Laboratory for the Advancement, Understanding and\n"; \
    std::cout <<   "               Generation of High Impact Natural Graphics\n\n"; \
    std::cout <<   "                                 " << LABSHORTLOG << "\n\n"; \
    std::cout <<   "=========================================================================\n\n";  

#define LABLOGOSTRING  "\n=========================================================================\n\n                               LAUGHING\n\n            Laboratory for the Advancement, Understanding and\n               Generation of High Impact Natural Graphics\n\n                                 :o)\n\n=========================================================================\n\n"

#include <string>
namespace lux
{
std::string LabLogo();
std::string LabShortLogo();
}


#endif
