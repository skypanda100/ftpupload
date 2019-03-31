//
// Created by zhengdongtian on 19-3-31.
//

#include "curlftp.h"

static void current_dir(const char *dst_dir_path_ptr, char *location_ptr)
{
    sscanf(dst_dir_path_ptr, "%*[^:]://%*[^/]%s", location_ptr);
}

int upload(int is_sftp, const char *src_file_path_ptr, const char *dst_dir_path_ptr, const char *relative_dst_file_path_ptr, const char *user_pwd_ptr)
{
    CURLcode curl_code;
    long file_size = 0L;
    FILE *fp = NULL;
    char cwd[PATH_MAX] = {0};
    char remote_url[PATH_MAX] = {0};
    char buf_1[PATH_MAX] = {0};
    char buf_2[PATH_MAX] = {0};
    struct curl_slist *header_list_ptr = NULL;

    sprintf(remote_url, "%s/%s.zdt", dst_dir_path_ptr, relative_dst_file_path_ptr);

    fp = fopen(src_file_path_ptr, "rb");
    if(NULL == fp)
    {
        return FILE_NOT_EXISTS;
    }
    fseek(fp, 0L, SEEK_END);
    file_size = ftell(fp);
    printf("file name is %s, file size is %ld\n", src_file_path_ptr, file_size);
    fseek(fp, 0L, SEEK_SET);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();
    if(NULL == curl)
    {
        fclose(fp);
        return UPLOAD_FAILED;
    }

    current_dir(dst_dir_path_ptr, cwd);
    if(is_sftp)
    {
        sprintf(buf_1, "rm %s/%s", cwd, relative_dst_file_path_ptr);
        sprintf(buf_2, "rename %s/%s.zdt %s/%s", cwd, relative_dst_file_path_ptr, cwd, relative_dst_file_path_ptr);
        header_list_ptr = curl_slist_append(header_list_ptr, buf_1);
        header_list_ptr = curl_slist_append(header_list_ptr, buf_2);
    }
    else
    {
        sprintf(buf_1, "RNFR %s/%s.zdt", cwd, relative_dst_file_path_ptr);
        sprintf(buf_2, "RNTO %s/%s", cwd, relative_dst_file_path_ptr);
        header_list_ptr = curl_slist_append(header_list_ptr, buf_1);
        header_list_ptr = curl_slist_append(header_list_ptr, buf_2);
    }

    curl_easy_setopt(curl, CURLOPT_USERPWD, user_pwd_ptr);
    curl_easy_setopt(curl, CURLOPT_URL, remote_url);
    curl_easy_setopt(curl, CURLOPT_POSTQUOTE, header_list_ptr);
    curl_easy_setopt(curl, CURLOPT_READDATA, fp);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_APPEND, 0L);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE, file_size);
    curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 10L);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 30L);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    curl_code = curl_easy_perform(curl);

    curl_slist_free_all(header_list_ptr);
    curl_easy_cleanup(curl);
    fclose(fp);
    curl_global_cleanup();

    if(CURLE_OK != curl_code && CURLE_QUOTE_ERROR != curl_code)
    {
        return UPLOAD_FAILED;
    }

    return UPLOAD_OK;
}