#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>
#include <strutils.h>

#include "timeutils.h"
#include "logger.h"

#define LOG_BUFFER_LENGTH                   4096


struct _log_handle_t {
    FILE *          fptr;

    int             logLevel;
    bool            isInstantiated;
};

static log_handle_t         _log;
static pthread_mutex_t      _mutex = PTHREAD_MUTEX_INITIALIZER;
static char                 _logBuffer[LOG_BUFFER_LENGTH];


static log_handle_t * _lgGetHandle(void) {
    static log_handle_t *       pLog = NULL;

    if (pLog == NULL) {
        pLog = &_log;
        pLog->isInstantiated = false;
    }

    return pLog;
}

static int _logLevel_atoi(const char * pszLoggingLevel) {
    char *          pszLogLevel;
    char *          pszToken;
    char *          reference;
    int             logLevel = 0;

    pszLogLevel = strdup(pszLoggingLevel);

    reference = pszLogLevel;

    pszToken = str_trim(strtok_r(pszLogLevel, "|", &reference));

    while (pszToken != NULL) {
        if (strncmp(pszToken, "LOG_LEVEL_INFO", 14) == 0) {
            logLevel |= LOG_LEVEL_INFO;
        }
        else if (strncmp(pszToken, "LOG_LEVEL_STATUS", 16) == 0) {
            logLevel |= LOG_LEVEL_STATUS;
        }
        else if (strncmp(pszToken, "LOG_LEVEL_DEBUG", 15) == 0) {
            logLevel |= LOG_LEVEL_DEBUG;
        }
        else if (strncmp(pszToken, "LOG_LEVEL_ERROR", 15) == 0) {
            logLevel |= LOG_LEVEL_ERROR;
        }
        else if (strncmp(pszToken, "LOG_LEVEL_FATAL", 15) == 0) {
            logLevel |= LOG_LEVEL_FATAL;
        }

        free(pszToken);

        pszToken = str_trim(strtok_r(NULL, "|", &reference));
    }

    free(pszLogLevel);

    return logLevel;
}

int _log_message(int logLevel, bool addCR, const char * fmt, va_list args) {
    int                 bytesWritten = 0;
    char                szTimestamp[TIMESTAMP_STR_LEN];
    log_handle_t *      hlog = _lgGetHandle();

	pthread_mutex_lock(&_mutex);

    if (hlog->logLevel & logLevel) {
        if (strlen(fmt) > MAX_LOG_LENGTH) {
            fprintf(stderr, "Log line too long\n");
            return -1;
        }

        tmGetTimeStamp(szTimestamp, TIMESTAMP_STR_LEN, true);

        strncpy(_logBuffer, "[", 2);
        strncat(_logBuffer, szTimestamp, TIMESTAMP_STR_LEN);
        strncat(_logBuffer, "] ", 3);

        switch (logLevel) {
            case LOG_LEVEL_DEBUG:
                strncat(_logBuffer, "[DBG]", 6);
                break;

            case LOG_LEVEL_STATUS:
                strncat(_logBuffer, "[STA]", 6);
                break;

            case LOG_LEVEL_INFO:
                strncat(_logBuffer, "[INF]", 6);
                break;

            case LOG_LEVEL_ERROR:
                strncat(_logBuffer, "[ERR]", 6);
                break;

            case LOG_LEVEL_FATAL:
                strncat(_logBuffer, "[FTL]", 6);
                break;
        }

        if (addCR) {
            strncat(_logBuffer, fmt, (LOG_BUFFER_LENGTH >> 1));
            strncat(_logBuffer, "\n", 2);
        }
        else {
            strncpy(_logBuffer, fmt, (LOG_BUFFER_LENGTH >> 1));
        }

        bytesWritten = vfprintf(hlog->fptr, _logBuffer, args);
        fflush(hlog->fptr);

        _logBuffer[0] = 0;
    }

	pthread_mutex_unlock(&_mutex);

    return bytesWritten;
}

int lgOpen(const char * pszLogFile, const char * pszLogFlags) {
    log_handle_t * pLog = _lgGetHandle();

    if (!pLog->isInstantiated) {
        pLog->fptr = fopen(pszLogFile, "a");

        if (pLog->fptr == NULL) {
            fprintf(stderr, "Failed to open log file '%s': %s\n", pszLogFile, strerror(errno));
            return -1;
        }

        pLog->logLevel = _logLevel_atoi(pszLogFlags);

        pLog->isInstantiated = true;
    }
    else {
        fprintf(stderr, "Logger already initialised. You should only call lgOpen() once\n");
        return 0;
    }

    return 0;
}

int lgOpenStdout(const char * pszLogFlags) {
    log_handle_t * pLog = _lgGetHandle();

    if (!pLog->isInstantiated) {
        pLog->fptr = stdout;

        pLog->logLevel = _logLevel_atoi(pszLogFlags);

        pLog->isInstantiated = true;
    }
    else {
        fprintf(stderr, "Logger already initialised. You should only call lgOpen() once\n");
        return 0;
    }

    return 0;
}

void lgClose(void) {
    log_handle_t *      hlog = _lgGetHandle();

    fclose(hlog->fptr);

    hlog->logLevel = 0;
    hlog->isInstantiated = false;
}

void lgSetLogLevel(int logLevel) {
    log_handle_t *      hlog = _lgGetHandle();

    hlog->logLevel = logLevel;
}

int lgGetLogLevel(void) {
    log_handle_t *      hlog = _lgGetHandle();

    return hlog->logLevel;
}

bool lgCheckLogLevel(int logLevel) {
    log_handle_t *      hlog = _lgGetHandle();

    return ((hlog->logLevel & logLevel) == logLevel ? true : false);
}

void lgNewline(void) {
    log_handle_t *      hlog = _lgGetHandle();

    fprintf(hlog->fptr, "\n");
}

int lgLogInfo(const char * fmt, ...) {
    va_list             args;
    int                 bytesWritten;

    va_start (args, fmt);
    
    bytesWritten = _log_message(LOG_LEVEL_INFO, true, fmt, args);
    
    va_end(args);
    
    return bytesWritten;
}

int lgLogStatus(const char * fmt, ...) {
    va_list     args;
    int         bytesWritten;

    va_start (args, fmt);
    
    bytesWritten = _log_message(LOG_LEVEL_STATUS, true, fmt, args);
    
    va_end(args);
    
    return bytesWritten;
}

int lgLogDebug(const char * fmt, ...) {
    va_list     args;
    int         bytesWritten;

    va_start (args, fmt);
    
    bytesWritten = _log_message(LOG_LEVEL_DEBUG, true, fmt, args);
    
    va_end(args);
    
    return bytesWritten;
}

int lgLogDebugNoCR(const char * fmt, ...) {
    va_list     args;
    int         bytesWritten;

    va_start (args, fmt);
    
    bytesWritten = _log_message(LOG_LEVEL_DEBUG, false, fmt, args);
    
    va_end(args);
    
    return bytesWritten;
}

int lgLogError(const char * fmt, ...) {
    va_list     args;
    int         bytesWritten;

    va_start (args, fmt);
    
    bytesWritten = _log_message(LOG_LEVEL_ERROR, true, fmt, args);
    
    va_end(args);
    
    return bytesWritten;
}

int lgLogFatal(const char * fmt, ...) {
    va_list     args;
    int         bytesWritten;

    va_start (args, fmt);
    
    bytesWritten = _log_message(LOG_LEVEL_FATAL, true, fmt, args);
    
    va_end(args);
    
    return bytesWritten;
}
