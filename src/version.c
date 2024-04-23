#include "version.h"

#define __BDATE__      "2024-04-23 15:26:07"
#define __BVERSION__   "2.0.008"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
