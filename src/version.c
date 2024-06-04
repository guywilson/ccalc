#include "version.h"

#define __BDATE__      "2024-06-04 16:45:07"
#define __BVERSION__   "2.1.010"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
