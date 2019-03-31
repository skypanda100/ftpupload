//
// Created by zhengdongtian on 19-3-30.
//

#include "notify.h"

static size_t flags = IN_CREATE | IN_CLOSE_WRITE | IN_MOVED_TO;

static void list_dir(const char *path_ptr, char ***dir_ptr_ptr_ptr, size_t *dirs_len_ptr)
{
    DIR *d_ptr;
    struct dirent *file_ptr;
    struct stat st;
    char full_path[PATH_MAX] = {0};

    if(!(d_ptr = opendir(path_ptr)))
    {
        printf("opendir failed[%s]\n", path_ptr);
        return;
    }

    while((file_ptr = readdir(d_ptr)) != NULL)
    {
        if(strncmp(file_ptr->d_name, ".", 1) == 0)
        {
            continue;
        }

        memset(full_path, 0, sizeof(full_path) / sizeof(char));
        sprintf(full_path, "%s/%s", path_ptr, file_ptr->d_name);

        if(stat(full_path, &st) >= 0 && S_ISDIR(st.st_mode))
        {
            *dirs_len_ptr += 1;
            *dir_ptr_ptr_ptr = realloc(*dir_ptr_ptr_ptr, sizeof(char *) * (*dirs_len_ptr));
            (*dir_ptr_ptr_ptr)[*dirs_len_ptr - 1] = strdup(full_path);
            list_dir(full_path, dir_ptr_ptr_ptr, dirs_len_ptr);
        }
    }
    closedir(d_ptr);
}

static void absolute_path(const notification *ntf_ptr, const struct inotify_event *event_ptr, char *new_path_ptr)
{
    for(int i = 0;i < ntf_ptr->dir_watch_ptr_len;i++)
    {
        if(event_ptr->wd == ntf_ptr->dir_watch_ptr[i].wd)
        {
            sprintf(new_path_ptr, "%s/%s", ntf_ptr->dir_watch_ptr[i].wpath, event_ptr->name);
            break;
        }
    }
}

static void handle_notify(const conf *cf_ptr, const notification *ntf_ptr, const struct inotify_event *event_ptr)
{
    char src_file_path[PATH_MAX] = {0};
    char relative_dst_file_path[PATH_MAX] = {0};
    absolute_path(ntf_ptr, event_ptr, src_file_path);
    if(strlen(src_file_path) > 0)
    {
        strcpy(relative_dst_file_path, src_file_path + strlen(cf_ptr->src_dir));
        printf("after 3 seconds upload %s!\n", src_file_path);
        sleep(3);   // 待文件稳定后再上传
        int code = upload(cf_ptr->is_sftp, src_file_path, cf_ptr->dst_dir, relative_dst_file_path, cf_ptr->user_pwd);
        if(code == UPLOAD_FAILED)
        {
            for(int try_no = 0;try_no < RETRY_MAX;try_no++)
            {
                printf("%d retry: after 10 seconds upload file again, max retry number is %d!\n", try_no, RETRY_MAX);
                sleep(10);
                code = upload(cf_ptr->is_sftp, src_file_path, cf_ptr->dst_dir, relative_dst_file_path, cf_ptr->user_pwd);
                if(code == UPLOAD_OK || code == FILE_NOT_EXISTS)
                {
                    break;
                }
            }
        }
        else if(code == UPLOAD_OK)
        {
            printf("upload file successfully!\n");
        }
        else if(code == FILE_NOT_EXISTS)
        {
            printf("file is not exist!\n");
        }
    }
}


int add_dir_to_watch_list(notification *ntf_ptr, const char *path_ptr)
{
    int wd = inotify_add_watch(ntf_ptr->notify_fd, path_ptr, flags);
    if(wd == -1)
    {
        printf("inotify_add_watch failed[%s]\n", path_ptr);
    }
    else
    {
        ntf_ptr->dir_watch_ptr_len += 1;
        dir_watch *s_w = (dir_watch *)realloc(ntf_ptr->dir_watch_ptr, sizeof(dir_watch) * ntf_ptr->dir_watch_ptr_len);
        s_w[ntf_ptr->dir_watch_ptr_len - 1].wd = wd;
        strcpy(s_w[ntf_ptr->dir_watch_ptr_len - 1].wpath, path_ptr);
        ntf_ptr->dir_watch_ptr = s_w;
    }

    return wd;
}

void watch(const conf *cf_ptr)
{
    notification ntf;
    memset(&ntf, 0, sizeof(notification));

    int nd = inotify_init();
    if(nd == -1)
    {
        printf("inotify_init failed\n");
        ntf.notify_fd = -1;
        ntf.dir_watch_ptr = NULL;
        ntf.dir_watch_ptr_len = 0;
        return;
    }

    ntf.notify_fd = nd;

    int wd = add_dir_to_watch_list(&ntf, cf_ptr->src_dir);
    if(wd != -1)
    {
        size_t dirs_len = 0;
        char **dir_ptr_ptr = NULL;
        list_dir(cf_ptr->src_dir, &dir_ptr_ptr, &dirs_len);
        for(int i = 0; i < dirs_len;i++)
        {
            add_dir_to_watch_list(&ntf, dir_ptr_ptr[i]);
            printf("sub dir is %s\n", dir_ptr_ptr[i]);
        }
        // free
        if(dirs_len > 0)
        {
            for(int i = 0; i < dirs_len;i++)
            {
                free(dir_ptr_ptr[i]);
            }
            free(dir_ptr_ptr);
        }
    }

    for(;;)
    {
        char *temp_buf_ptr = NULL;
        char buf[BUF_LEN] = {0};
        size_t read_len = 0;
        struct inotify_event *event_ptr;

        read_len = read(ntf.notify_fd, buf, BUF_LEN);
        if(read_len == -1)
        {
            printf("read failed\n");
        }

        for(temp_buf_ptr = buf;temp_buf_ptr < buf + read_len;)
        {
            event_ptr = (struct inotify_event *)temp_buf_ptr;
            // handle watch
            if(event_ptr->mask & IN_ISDIR)
            {
                if(event_ptr->len > 0)
                {
                    //ignore hidden file
                    if(event_ptr->name[0] != '.')
                    {
                        char new_path[PATH_MAX] = {0};
                        absolute_path(&ntf, event_ptr, new_path);
                        if(strlen(new_path) > 0)
                        {
                            add_dir_to_watch_list(&ntf, new_path);
                        }
                    }
                }
            }
            else
            {
                if(event_ptr->mask & (IN_CLOSE_WRITE | IN_MOVED_TO))
                {
                    if(event_ptr->len > 0)
                    {
                        //ignore hidden file
                        if(event_ptr->name[0] != '.')
                        {
                            handle_notify(cf_ptr, &ntf, event_ptr);
                        }
                    }
                }
            }

//        printf("%d %d %s\n", read_len, event->len, event->name);
            temp_buf_ptr += sizeof(struct inotify_event) + event_ptr->len;
        }
    }
}
