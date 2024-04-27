#include "version.h"

#define __BDATE__      "2024-04-27 12:47:20"
#define __BVERSION__   "2.1.002"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
