#include "version.h"

#define __BDATE__      "2024-04-30 10:13:09"
#define __BVERSION__   "2.1.006"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
