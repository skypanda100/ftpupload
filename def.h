//
// Created by zhengdongtian on 19-3-30.
//

#ifndef FTPUPLOAD_DEF_H
#define FTPUPLOAD_DEF_H

#include <sys/time.h>
#include <limits.h>
#include <sys/inotify.h>
#include "log.h"

#define BUF_LEN 4096
#define RETRY_MAX 5

enum UPLOAD_CODE {
    UPLOAD_OK = 1,
    FILE_NOT_EXISTS,
    UPLOAD_FAILED
};

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
    char log[1024];
    char cmd[256];
    int is_sftp;
}conf;

typedef struct st_notification
{
    int notify_fd;
    dir_watch *dir_watch_ptr;
    int dir_watch_ptr_len;
}notification;

#endif //FTPUPLOAD_DEF_H
