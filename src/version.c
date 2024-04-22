#include "version.h"

#define __BDATE__      "2024-04-22 13:21:59"
#define __BVERSION__   "2.0.003"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
