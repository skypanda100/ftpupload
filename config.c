//
// Created by zhengdongtian on 19-3-30.
//

#include "config.h"

extern conf *cf_ptr;
extern int conf_len;

static char *node_ptr = "mtt";
static char *key_src_dir_ptr = "src_dir";
static char *key_dst_dir_ptr = "dst_dir";
static char *key_user_pwd_ptr = "user_pwd";
static char *key_log_ptr = "log";
static char *key_cmd_ptr = "cmd";
static char *key_retry_ptr = "retry";

static char *l_trim(char *output_ptr, const char *input_ptr)
{
    assert(input_ptr != NULL);
    assert(output_ptr != NULL);
    assert(output_ptr != input_ptr);
    for(;*input_ptr != '\0' && isspace(*input_ptr);++input_ptr)
    {
        ;
    }
    return strcpy(output_ptr, input_ptr);
}

static char *a_trim(char *output_ptr, const char *input_ptr)
{
    char *p = NULL;
    assert(input_ptr != NULL);
    assert(output_ptr != NULL);
    l_trim(output_ptr, input_ptr);
    for(p = output_ptr + strlen(output_ptr) - 1;p >= output_ptr && isspace(*p);--p)
    {
        ;
    }
    *(++p) = '\0';
    return output_ptr;
}


void config(const char *conf_path_ptr)
{
    char key[32] = {0};
    char val_src_dir[1024] = {0};
    char val_dst_dir[1024] = {0};
    char val_user_pwd[128] = {0};
    char val_log[1024] = {0};
    char val_cmd[256] = {0};
    char val_retry[4] = {0};
    int is_sftp = 0;
    int has_found_node = 0;
    char *buf, *c;
    char buf_i[1024], buf_o[1024];
    FILE *fp;
    if((fp=fopen(conf_path_ptr, "r")) == NULL)
    {
        fprintf(stderr, "openfile [%s] error [%s]\n", conf_path_ptr, strerror(errno));
        exit(1);
    }
    fseek(fp, 0, SEEK_SET);

    while(!feof(fp) && fgets(buf_i, 1024, fp) != NULL)
    {
        l_trim(buf_o, buf_i);
        if(strlen(buf_o) <= 0)
            continue;
        buf = buf_o;

        if(buf[0] == '#')
        {
            continue;
        }
        else
        {
            memset(key, 0, sizeof(key) / sizeof(char));
            sprintf(key, "[%s]", node_ptr);
            if(strncmp(buf, key, strlen(key)) == 0)
            {
                if(has_found_node != 0)
                {
                    if(strlen(val_src_dir) == 0)
                    {
                        fprintf(stderr, "src_dir can not be empty!\n");
                        exit(-1);
                    }

                    if(strlen(val_dst_dir) == 0)
                    {
                        fprintf(stderr, "dst_dir can not be empty!\n");
                        exit(-1);
                    }

                    conf_len++;
                    cf_ptr = (conf *)realloc(cf_ptr, sizeof(conf) * conf_len);

                    strcpy(cf_ptr[conf_len - 1].src_dir, val_src_dir);
                    memset(val_src_dir, 0, sizeof(val_src_dir));
                    strcpy(cf_ptr[conf_len - 1].dst_dir, val_dst_dir);
                    memset(val_dst_dir, 0, sizeof(val_dst_dir));
                    strcpy(cf_ptr[conf_len - 1].user_pwd, val_user_pwd);
                    memset(val_user_pwd, 0, sizeof(val_user_pwd));
                    strcpy(cf_ptr[conf_len - 1].log, val_log);
                    memset(val_log, 0, sizeof(val_log));
                    strcpy(cf_ptr[conf_len - 1].cmd, val_cmd);
                    memset(val_cmd, 0, sizeof(val_cmd));
                    cf_ptr[conf_len - 1].retry = strlen(val_retry) == 0 ? 0 : atoi(val_retry);
                    memset(val_retry, 0, sizeof(val_retry));
                    cf_ptr[conf_len - 1].is_sftp = is_sftp;
                    is_sftp = 0;
                }
                has_found_node = 1;
                continue;
            }

            if((c = strchr(buf, '=')) == NULL)
                continue;
            memset(key, 0, sizeof(key));
            sscanf(buf, "%[^=|^ |^\t]", key);
            if(strcmp(key, key_src_dir_ptr) == 0)
            {
                sscanf(++c, "%[^\n]", val_src_dir);
                char *val_o = (char *)malloc(strlen(val_src_dir) + 1);
                if(val_o != NULL)
                {
                    memset(val_o, 0, strlen(val_src_dir) + 1);
                    a_trim(val_o, val_src_dir);
                    if(val_o && strlen(val_o) > 0)
                        strcpy(val_src_dir, val_o);
                    free(val_o);
                    val_o = NULL;
                }
            }
            else if(strcmp(key, key_dst_dir_ptr) == 0)
            {
                sscanf(++c, "%[^\n]", val_dst_dir);
                char *val_o = (char *)malloc(strlen(val_dst_dir) + 1);
                if(val_o != NULL)
                {
                    memset(val_o, 0, strlen(val_dst_dir) + 1);
                    a_trim(val_o, val_dst_dir);
                    if(val_o && strlen(val_o) > 0)
                        strcpy(val_dst_dir, val_o);
                    free(val_o);
                    val_o = NULL;

                    // check protocol
                    if(strncmp("sftp:", val_dst_dir, 5) == 0)
                    {
                        is_sftp = 1;
                    }
                    else if(strncmp("ftp:", val_dst_dir, 4) == 0)
                    {
                        is_sftp = 0;
                    } else{
                        fprintf(stderr, "only ftp or sftp available!\n");
                        exit(-1);
                    }
                }
            }
            else if(strcmp(key, key_user_pwd_ptr) == 0)
            {
                sscanf(++c, "%[^\n]", val_user_pwd);
                char *val_o = (char *)malloc(strlen(val_user_pwd) + 1);
                if(val_o != NULL)
                {
                    memset(val_o, 0, strlen(val_user_pwd) + 1);
                    a_trim(val_o, val_user_pwd);
                    if(val_o && strlen(val_o) > 0)
                        strcpy(val_user_pwd, val_o);
                    free(val_o);
                    val_o = NULL;
                }
            }
            else if(strcmp(key, key_log_ptr) == 0)
            {
                sscanf(++c, "%[^\n]", val_log);
                char *val_o = (char *)malloc(strlen(val_log) + 1);
                if(val_o != NULL)
                {
                    memset(val_o, 0, strlen(val_log) + 1);
                    a_trim(val_o, val_log);
                    if(val_o && strlen(val_o) > 0)
                        strcpy(val_log, val_o);
                    free(val_o);
                    val_o = NULL;
                }
            }
            else if(strcmp(key, key_cmd_ptr) == 0)
            {
                sscanf(++c, "%[^\n]", val_cmd);
                char *val_o = (char *)malloc(strlen(val_cmd) + 1);
                if(val_o != NULL)
                {
                    memset(val_o, 0, strlen(val_cmd) + 1);
                    a_trim(val_o, val_cmd);
                    if(val_o && strlen(val_o) > 0)
                        strcpy(val_cmd, val_o);
                    free(val_o);
                    val_o = NULL;
                }
            }
            else if(strcmp(key, key_retry_ptr) == 0)
            {
                sscanf(++c, "%[^\n]", val_retry);
                char *val_o = (char *)malloc(strlen(val_retry) + 1);
                if(val_o != NULL)
                {
                    memset(val_o, 0, strlen(val_retry) + 1);
                    a_trim(val_o, val_retry);
                    if(val_o && strlen(val_o) > 0)
                        strcpy(val_retry, val_o);
                    free(val_o);
                    val_o = NULL;
                }
            }
        }
    }

    fclose(fp);

    if(has_found_node != 0)
    {
        if(strlen(val_src_dir) == 0)
        {
            fprintf(stderr, "src_dir can not be empty!\n");
            exit(-1);
        }

        if(strlen(val_dst_dir) == 0)
        {
            fprintf(stderr, "dst_dir can not be empty!\n");
            exit(-1);
        }

        conf_len++;
        cf_ptr = (conf *)realloc(cf_ptr, sizeof(conf) * conf_len);

        strcpy(cf_ptr[conf_len - 1].src_dir, val_src_dir);
        strcpy(cf_ptr[conf_len - 1].dst_dir, val_dst_dir);
        strcpy(cf_ptr[conf_len - 1].user_pwd, val_user_pwd);
        strcpy(cf_ptr[conf_len - 1].log, val_log);
        strcpy(cf_ptr[conf_len - 1].cmd, val_cmd);
        cf_ptr[conf_len - 1].retry = strlen(val_retry) == 0 ? 0 : atoi(val_retry);
        cf_ptr[conf_len - 1].is_sftp = is_sftp;
    }

    // final check
    if(conf_len == 0)
    {
        fprintf(stderr, "conf file is not correct, please fix it like this:\n"
                "[mtt]\n"
                "src_dir={dir}\n"
                "dst_dir=ftp://{ip}/{dir}/\n"
                "user_pwd={user}:{password}\n"
                "retry={retryNumber}\n"
                "log={dir}\n\n"
                "[mtt]\n"
                "src_dir={dir}\n"
                "dst_dir=ftp://{ip}/{dir}/\n"
                "user_pwd={user}:{password}\n"
                "retry={retryNumber}\n"
                "log={dir}\n");
        exit(-1);
    }
}
