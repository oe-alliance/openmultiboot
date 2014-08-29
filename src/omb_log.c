#include <stdio.h>
#include <stdarg.h>

#include "omb_log.h"
#include "omb_common.h"

void omb_log(int level, const char* format, ...)
{
	va_list arglist;
	
	switch(level)
	{
		case LOG_ERROR:
			printf("%s - error: ", OMB_APP_NAME);
			break;
		case LOG_WARNING:
			printf("%s - warning: ", OMB_APP_NAME);
			break;
		case LOG_DEBUG:
			printf("%s - debug: ", OMB_APP_NAME);
			break;
	}
	
	va_start(arglist, format);
	vprintf(format, arglist);
	va_end(arglist);
	printf("\n");
}
