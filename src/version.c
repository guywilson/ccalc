#include "version.h"

#define __BDATE__      "2024-04-22 09:53:52"
#define __BVERSION__   "2.0.002"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
