/*
 *
 */

#ifndef _LOG_H_
#define _LOG_H_

#include <stdarg.h>
#include <stdio.h>
#include "zlog.h"

#ifdef __GNUC__
#define CHECK_FMT(a,b) __attribute__((format(printf, a, b)))
#else
#define CHECK_FMT(a,b)
#endif

void log_init(const char *conf);
void log_fini();
void log_reload(const char *conf);

/*void log_debug(const char *format, const char *msg, ...) CHECK_FMT(1,2);
void log_info(const char *msg, ...) CHECK_FMT(1,2);
void log_notice(const char *msg, ...) CHECK_FMT(1,2);
void log_warn(const char *msg, ...) CHECK_FMT(1,2);
void log_error(const char *msg, ...) CHECK_FMT(1,2);
void log_fatal(const char *msg, ...) CHECK_FMT(1,2);*/

#define log_fatal(cat, ...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, __VA_ARGS__)
#define log_error(cat, ...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, __VA_ARGS__)
#define log_warn(cat, ...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, __VA_ARGS__)
#define log_notice(cat, ...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, __VA_ARGS__)
#define log_info(cat, ...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, __VA_ARGS__)
#define log_debug(cat, ...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, __VA_ARGS__)

extern zlog_category_t *rart_category;

#endif