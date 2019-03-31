//
// Created by zhengdongtian on 19-3-31.
//

#include "curlftp.h"

int upload(const char *src_path_ptr, const char *dst_path_ptr, const char *user_pwd_ptr)
{
    CURLcode curl_code;
    long file_size = 0L;
    FILE *fp = NULL;

    fp = fopen(src_path_ptr, "r");
    if(NULL == fp)
    {
        return FILE_NOT_EXISTS;
    }
    fseek(fp, 0L, SEEK_END);
    file_size = ftell(fp);
    printf("file name is %s, file size is %ld\n", src_path_ptr, file_size);
    fseek(fp, 0L, SEEK_SET);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();
    if(NULL == curl)
    {
        fclose(fp);
        return UPLOAD_FAILED;
    }

    curl_easy_setopt(curl, CURLOPT_USERPWD, user_pwd_ptr);
    curl_easy_setopt(curl, CURLOPT_URL, dst_path_ptr);
    curl_easy_setopt(curl, CURLOPT_READDATA, fp);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE, file_size);
    curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 10);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 30);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

    curl_code = curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    fclose(fp);
    curl_global_cleanup();

    if(CURLE_OK != curl_code)
    {
        return UPLOAD_FAILED;
    }

    return UPLOAD_OK;
}