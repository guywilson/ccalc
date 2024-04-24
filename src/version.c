#include "version.h"

#define __BDATE__      "2024-04-24 14:07:21"
#define __BVERSION__   "2.0.010"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
