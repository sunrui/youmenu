/*
 * public log provider
 *
 * author rui.sun 2012-7-3 Tue 5:03
 *
 * update
 *    * for colored fputs rui.sun 2012-8-16 Thu 4:56
 */
#ifndef LOG_H
#define LOG_H

#ifdef  __cplusplus
# define __LOG_BEGIN_DECLS  extern "C" {
# define __LOG_END_DECLS    }
#else
# define __LOG_BEGIN_DECLS
# define __LOG_END_DECLS
#endif

__LOG_BEGIN_DECLS

#ifndef LOG_DISABLE

#define LOGD(...)	log_output(log_level_debug, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LOGI(...)	log_output(log_level_info, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LOGW(...)	log_output(log_level_warn, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LOGE(...)	log_output(log_level_error, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG(...)	log_output2(__VA_ARGS__)

typedef enum log_level
{
	log_level_debug		= 3,
	log_level_info		= 2,
	log_level_warn		= 1,
	log_level_error		= 0
} log_level;

/*
 * log init module
 *
 * return 1 is success
 */
int log_init(const char * localfile);

/*
 * set log only show level lower in
 */
void log_set_level(log_level level);
void log_set_console(int enable);

void log_output(log_level level, const char * c_file, const char * function, int line, const char * format, ...);
void log_output2(const char * format, ...);

void log_fini();

#else

#define LOGD(...)
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)
#define LOG(...)

#endif

__LOG_END_DECLS

#endif