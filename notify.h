//
// Created by zhengdongtian on 19-3-30.
//

#ifndef FTPUPLOAD_NOTIFY_H
#define FTPUPLOAD_NOTIFY_H

#include <sys/inotify.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include "def.h"

int add_dir_to_watch_list(notification *ntf_ptr, const char *path_ptr);
void watch(const conf *cf_ptr);

#endif //FTPUPLOAD_NOTIFY_H
