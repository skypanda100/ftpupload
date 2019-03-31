//
// Created by zhengdongtian on 19-3-30.
//

#include "notify.h"

static char dirs[DIR_MAX][PATH_MAX];
static int dirs_len = 0;
static size_t flags = IN_CREATE | IN_CLOSE_WRITE | IN_MOVED_TO;

static void list_dir(const char *path_ptr)
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
            strcpy(dirs[dirs_len++], full_path);
            list_dir(full_path);
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
    char dst_file_path[PATH_MAX] = {0};
    absolute_path(ntf_ptr, event_ptr, src_file_path);
    if(strlen(src_file_path) > 0)
    {
        sprintf(dst_file_path, "%s/%s", cf_ptr->dst_dir, src_file_path + strlen(cf_ptr->src_dir));
        upload(src_file_path, dst_file_path, cf_ptr->user_pwd);
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
        memset(dirs, 0, sizeof(dirs) / sizeof(char));
        dirs_len = 0;
        list_dir(cf_ptr->src_dir);
        for(int j = 0; j < dirs_len;j++)
        {
            add_dir_to_watch_list(&ntf, dirs[j]);
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
