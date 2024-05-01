#include "version.h"

#define __BDATE__      "2024-05-01 15:34:13"
#define __BVERSION__   "2.1.007"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
