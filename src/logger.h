#include <stdbool.h>

#ifndef __INCL_LOGGER
#define __INCL_LOGGER

#define MAX_LOG_LENGTH          512

/*
** Supported log levels...
*/
#define LOG_LEVEL_INFO          0x01
#define LOG_LEVEL_STATUS        0x02
#define LOG_LEVEL_DEBUG         0x04
#define LOG_LEVEL_ERROR         0x08
#define LOG_LEVEL_FATAL         0x10

#define LOG_LEVEL_ALL           (LOG_LEVEL_INFO | LOG_LEVEL_STATUS | LOG_LEVEL_DEBUG | LOG_LEVEL_ERROR | LOG_LEVEL_FATAL)

#ifdef __cplusplus
extern "C" {
#endif

struct _log_handle_t;
typedef struct _log_handle_t        log_handle_t;

int             lgOpen(const char * pszLogFile, const char * pszLogFlags);
int             lgOpenStdout(const char * pszLogFlags);
void            lgClose(void);
void            lgSetLogLevel(int logLevel);
int             lgGetLogLevel(void);
bool            lgCheckLogLevel(int logLevel);
void            lgNewline(void);
int             lgLogInfo(const char * fmt, ...);
int             lgLogStatus(const char * fmt, ...);
int             lgLogDebug(const char * fmt, ...);
int             lgLogDebugNoCR(const char * fmt, ...);
int             lgLogError(const char * fmt, ...);
int             lgLogFatal(const char * fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
