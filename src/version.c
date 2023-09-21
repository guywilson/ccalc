#include "version.h"

#define __BDATE__      "2023-09-21 10:58:13"
#define __BVERSION__   "1.0.009"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
