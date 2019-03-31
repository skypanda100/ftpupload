//
// Created by zhengdongtian on 19-3-31.
//

#ifndef FTPUPLOAD_CURLFTP_H
#define FTPUPLOAD_CURLFTP_H

#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include "def.h"

int upload(const char *src_path_ptr, const char *dst_path_ptr, const char *user_pwd_ptr);

#endif //FTPUPLOAD_CURLFTP_H
