#include "version.h"

#define __BDATE__      "2024-04-24 11:13:06"
#define __BVERSION__   "2.0.009"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
