#include "version.h"

#define __BDATE__      "2023-09-21 10:24:44"
#define __BVERSION__   "1.0.007"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
