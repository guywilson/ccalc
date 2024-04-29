#include "version.h"

#define __BDATE__      "2024-04-29 22:36:32"
#define __BVERSION__   "2.1.005"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
