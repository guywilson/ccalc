#include "version.h"

#define __BDATE__      "2024-04-23 14:22:17"
#define __BVERSION__   "2.0.006"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
