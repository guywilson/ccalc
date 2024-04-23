#include "version.h"

#define __BDATE__      "2024-04-23 14:40:06"
#define __BVERSION__   "2.0.007"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
