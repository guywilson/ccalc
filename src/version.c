#include "version.h"

#define __BDATE__      "2024-06-06 21:49:34"
#define __BVERSION__   "2.1.016"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
