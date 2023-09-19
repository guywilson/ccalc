#include "version.h"

#define __BDATE__      "2023-09-19 15:59:54"
#define __BVERSION__   "1.0.002"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
