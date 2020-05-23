#include "utilities.h"

#include <sstream>

size_t utilities::to_size_t(const char *number)
{
    size_t sizeT;
    std::istringstream iss(number);
    iss >> sizeT;
    if (iss.fail())
        {
            return std::numeric_limits<size_t>::max();
        }
    else
        {
            return sizeT;
        }
}
