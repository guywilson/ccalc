#include "version.h"

#define __BDATE__      "2024-05-06 18:19:25"
#define __BVERSION__   "2.1.009"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
