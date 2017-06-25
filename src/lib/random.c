#include <tlpiexer/random.h>

#include <stdlib.h>

int rrand(size_t min, size_t max, size_t *result)
{
    if (min >= max || max > RAND_MAX)
        return -1;
    *result = min + 1 + rand() % (max - min);
    return 0;
}
