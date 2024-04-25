#include "version.h"

#define __BDATE__      "2024-04-25 15:04:34"
#define __BVERSION__   "2.0.011"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
