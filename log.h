//
// Created by zhengdongtian on 4/1/19.
//

#ifndef FTPUPLOAD_LOG_H
#define FTPUPLOAD_LOG_H

#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>
#include <limits.h>
#include <sys/stat.h>

#define LOG(dir, fmt, args...) logger(dir, __FILE__, __LINE__, fmt, ##args);

void logger(const char *dir_ptr, const char *source_ptr, size_t line, const char *format_ptr, ...);

#endif //FTPUPLOAD_LOG_H
