/*
 * colored fputs support, ported from ffmpeg avutil/log.c
 *
 * @author rui.sun 2012-8-16 2:53 Thu
 */
#ifndef COLORED_FPUTS_H
#define COLORED_FPUTS_H

#define level_info 3
#define level_debug 2
#define level_warn 1
#define level_error 0

void colored_fputs(int level, const char *str);

#endif