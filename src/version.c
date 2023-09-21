#include "version.h"

#define __BDATE__      "2023-09-21 22:33:53"
#define __BVERSION__   "1.0.012"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
