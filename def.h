//
// Created by zhengdongtian on 19-3-30.
//

#ifndef FTPUPLOAD_DEF_H
#define FTPUPLOAD_DEF_H

#include <sys/time.h>

#define BUF_LEN 1024
#define DEPTH 64
#define DIR_MAX 256
#define PATH_MAX 4096
typedef struct st_dir_watch
{
    int wd;
    char wpath[PATH_MAX];
}dir_watch;

typedef struct st_conf
{
    char src_dir[1024];
    char dst_dir[1024];
    char user_pwd[128];
}conf;

typedef struct st_notification
{
    int notify_fd;
    dir_watch *dir_watch_ptr;
    int dir_watch_ptr_len;
}notification;

#endif //FTPUPLOAD_DEF_H
