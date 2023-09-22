#include "version.h"

#define __BDATE__      "2023-09-22 08:34:45"
#define __BVERSION__   "1.0.014"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
