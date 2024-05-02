#include "version.h"

#define __BDATE__      "2024-05-02 12:09:02"
#define __BVERSION__   "2.1.008"

const char * getVersion(void)
{
    return __BVERSION__;
}

const char * getBuildDate(void)
{
    return __BDATE__;
}
