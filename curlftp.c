//
// Created by zhengdongtian on 19-3-31.
//

#include "curlftp.h"

static void work_dir(const char *dst_dir_path_ptr, char *location_ptr)
{
    sscanf(dst_dir_path_ptr, "%*[^:]://%*[^/]%s", location_ptr);
}

static int rename_by_cmd(CURL *curl, int is_sftp, const char *dst_dir_path_ptr, const char *relative_dst_file_path_ptr, size_t suffix)
{
    CURLcode curl_code;
    struct curl_slist *header_list_1_ptr = NULL;
    struct curl_slist *header_list_2_ptr = NULL;
    char cwd[PATH_MAX] = {0};
    char buf_1[PATH_MAX] = {0};
    char buf_2[PATH_MAX] = {0};

    work_dir(dst_dir_path_ptr, cwd);
    if(is_sftp)
    {
        sprintf(buf_1, "rm %s/%s", cwd, relative_dst_file_path_ptr);
        header_list_1_ptr = curl_slist_append(header_list_1_ptr, buf_1);
    }
    else
    {
        sprintf(buf_1, "RNFR %s/%s.%ld", cwd, relative_dst_file_path_ptr, suffix);
        sprintf(buf_2, "RNTO %s/%s", cwd, relative_dst_file_path_ptr);
        header_list_1_ptr = curl_slist_append(header_list_1_ptr, buf_1);
        header_list_1_ptr = curl_slist_append(header_list_1_ptr, buf_2);
    }

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 0L);
        curl_easy_setopt(curl, CURLOPT_POSTQUOTE, header_list_1_ptr);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1);

        curl_code = curl_easy_perform(curl);
        curl_slist_free_all(header_list_1_ptr);

        if(curl_code == CURLE_QUOTE_ERROR || curl_code == CURLE_OK)
        {
            if(is_sftp)
            {
                sprintf(buf_2, "rename %s/%s.%ld %s/%s", cwd, relative_dst_file_path_ptr, suffix, cwd, relative_dst_file_path_ptr);
                header_list_2_ptr = curl_slist_append(header_list_2_ptr, buf_2);
                curl_easy_setopt(curl, CURLOPT_POSTQUOTE, header_list_2_ptr);

                curl_code = curl_easy_perform(curl);
                curl_slist_free_all(header_list_2_ptr);
            }
        }
    }

    return curl_code;
}

int upload(int is_sftp, const char *src_file_path_ptr, const char *dst_dir_path_ptr, const char *relative_dst_file_path_ptr, const char *user_pwd_ptr)
{
    CURLcode curl_code;
    long local_file_size;
    FILE *fp = NULL;
    char remote_url[PATH_MAX] = {0};
    size_t suffix = (size_t)&remote_url;

    sprintf(remote_url, "%s/%s.%ld", dst_dir_path_ptr, relative_dst_file_path_ptr, suffix);

    fp = fopen(src_file_path_ptr, "rb");
    if(NULL == fp)
    {
        return FILE_NOT_EXISTS;
    }
    fseek(fp, 0L, SEEK_END);
    local_file_size = ftell(fp);
    printf("file name is %s, file size is %ld\n", src_file_path_ptr, local_file_size);
    fseek(fp, 0L, SEEK_SET);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();
    if(NULL == curl)
    {
        fclose(fp);
        return UPLOAD_FAILED;
    }

    curl_easy_setopt(curl, CURLOPT_USERPWD, user_pwd_ptr);
    curl_easy_setopt(curl, CURLOPT_URL, remote_url);
    curl_easy_setopt(curl, CURLOPT_READDATA, fp);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_APPEND, 0L);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE, local_file_size);
    curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 10L);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 30L);
//    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    curl_code = curl_easy_perform(curl);
    if(CURLE_OK == curl_code)
    {
        curl_code = rename_by_cmd(curl, is_sftp, dst_dir_path_ptr, relative_dst_file_path_ptr, suffix);
    }
    curl_easy_cleanup(curl);
    fclose(fp);
    curl_global_cleanup();

    if(CURLE_OK != curl_code)
    {
        fprintf(stderr, "%s.%ld rename error\n", relative_dst_file_path_ptr, suffix);
        return UPLOAD_FAILED;
    }

    return UPLOAD_OK;
}