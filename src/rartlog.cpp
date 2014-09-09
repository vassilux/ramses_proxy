
#include <stdio.h>
#include <stdlib.h>
#include "zlog.h"
#include "rartlog.h"

int rc =0;
zlog_category_t *rart_category = 0;

void log_init(const char *conf)
{
	rc = zlog_init(conf);
	if (rc) {
		printf("init failed\n");
		exit(-1);
	}

	rart_category = zlog_get_category("rart_cat");
	if (!rart_category) {
		printf("get cat fail\n");
		zlog_fini();
		exit( -2);
	}
}

void log_fini(){
	zlog_fini();
}

void log_reload(const char *conf){
	rc =  zlog_reload(conf);
}


/*void log_debug(const char *format, const char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	zlog_debug(rart_category, "format, ap");
	va_end(ap);

}

void
log_info(const char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	zlog_info(rart_category, msg, ap);
	va_end(ap);
}

void log_notice(const char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	zlog_notice(rart_category, msg, ap);
	va_end(ap);
}

void log_warn(const char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	zlog_warn(rart_category, msg, ap);
	va_end(ap);
}

void log_error(const char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	zlog_error(rart_category, msg, ap);
	va_end(ap);
}

void log_fatal(const char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	zlog_fatal(rart_category, msg, ap);
	va_end(ap);
}*/

