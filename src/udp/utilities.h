#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdlib.h>

class utilities
{
public:
    /**
   * @brief  Convert const char* to size_t
   * @note   When there is an error it returns the maximum of size_t
   * @param  *number: const char*
   * @retval size_t
   */
    static size_t to_size_t(const char* number);

private:
    utilities() = default;
};

#endif // UTILITIES_H
