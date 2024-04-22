#include "version.h"

#define __BDATE__      "2024-04-22 08:43:38"
#define __BVERSION__   "2.0.001"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
