#include "version.h"

#define __BDATE__      "2024-06-06 22:06:54"
#define __BVERSION__   "2.1.018"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
