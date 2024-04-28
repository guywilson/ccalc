#include "version.h"

#define __BDATE__      "2024-04-28 21:22:34"
#define __BVERSION__   "2.1.004"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
