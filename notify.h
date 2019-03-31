//
// Created by zhengdongtian on 19-3-30.
//

#ifndef FNOTIFY_NOTIFY_H
#define FNOTIFY_NOTIFY_H

#include <sys/inotify.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include "def.h"

void handle_notify(s_notify *ntf, struct inotify_event *event);
int notify_dir(s_notify *ntf, const char *path_ptr);
void notify();
void handle_watch(s_notify *ntf);
void watch();

#endif //FNOTIFY_NOTIFY_H
