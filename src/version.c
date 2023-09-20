#include "version.h"

#define __BDATE__      "2023-09-20 13:38:15"
#define __BVERSION__   "1.0.003"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
