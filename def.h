//
// Created by zhengdongtian on 19-3-30.
//

#ifndef FNOTIFY_DEF_H
#define FNOTIFY_DEF_H

#include <sys/time.h>

#define BUF_LEN 1024
#define DEPTH 64
#define DIR_MAX 256
#define PATH_MAX 4096
typedef struct watch
{
    int wd;
    char wpath[PATH_MAX];
}s_watch;

typedef struct conf
{
    char path[PATH_MAX];
    char cmd[PATH_MAX];
    int delay;
}s_conf;

typedef struct notify
{
    s_conf conf;
    int notify_fd;
    s_watch *s_watch_p;
    int s_watch_p_len;
    time_t time;
}s_notify;

#endif //FNOTIFY_DEF_H
