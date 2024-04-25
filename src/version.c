#include "version.h"

#define __BDATE__      "2024-04-25 16:46:45"
#define __BVERSION__   "2.1.001"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
