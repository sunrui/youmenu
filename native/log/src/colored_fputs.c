#include "colored_fputs.h"

#include <stdlib.h>
#include <stdio.h>

/* color definitions from ffmpeg avutil/log.c */
#if defined(_WIN32) && !defined(__MINGW32CE__)
#include <windows.h>
#include <stdint.h>
//static const uint8_t color[] = {12,12,12,14,7,7,7};
static const uint8_t color[] = {
	FOREGROUND_RED | FOREGROUND_INTENSITY, /* error */
	FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY, /* warn */
	7, /* debug */
	FOREGROUND_GREEN | FOREGROUND_INTENSITY, /* info */
};
static int16_t background, attr_orig;
static HANDLE con;
#define set_color(x)  SetConsoleTextAttribute(con, background | color[x])
#define reset_color() SetConsoleTextAttribute(con, attr_orig)
#else
//static const uint8_t color[]={0x41,0x41,0x11,0x03,9,9,9};
static const uint8_t color[]={0x41,0x11,0x03,9};
#define set_color(x)  fprintf(stderr, "\033[%d;3%dm", color[x]>>4, color[x]&15)
#define reset_color() fprintf(stderr, "\033[0m")
#endif

static int use_color = -1;
#undef fprintf
void colored_fputs(int level, const char *str)
{
	if(use_color < 0) {
#if defined(_WIN32) && !defined(__MINGW32CE__)
		CONSOLE_SCREEN_BUFFER_INFO con_info;
		con = GetStdHandle(STD_ERROR_HANDLE);
		use_color = (con != INVALID_HANDLE_VALUE);
		if (use_color) {
			GetConsoleScreenBufferInfo(con, &con_info);
			attr_orig  = con_info.wAttributes;
			background = attr_orig & 0xF0;
		}
#elif defined(__APPLE__)
		use_color = 0;
#endif
	}
	
	if(use_color) {
		set_color(level);
	}
	
	fputs(str, stderr);
	
	if(use_color) {
		reset_color();
	}
}