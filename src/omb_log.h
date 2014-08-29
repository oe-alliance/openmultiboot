#ifndef _OMB_LOG_H_
#define _OMB_LOG_H_

#define LOG_ERROR 0
#define LOG_WARNING 1
#define LOG_DEBUG 2

void omb_log(int level, const char* format, ...);

#endif // _OMB_LOG_H_