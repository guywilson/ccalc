#include "version.h"

#define __BDATE__      "2024-04-28 16:49:44"
#define __BVERSION__   "2.1.003"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
