//
// Created by zhengdongtian on 19-3-30.
//

#include "notify.h"

extern s_notify *s_notify_p;
extern int s_notify_p_len;

static char dirs[DIR_MAX][PATH_MAX];
static int dirs_len = 0;
static size_t flags = IN_CLOSE_WRITE | IN_MOVED_TO;

static void list_dir(char* path, int depth)
{
    DIR *d;
    struct dirent *file;
    struct stat st;
    char full_path[PATH_MAX] = {0};

    if(!(d = opendir(path)))
    {
        printf("opendir failed[%s]\n", path);
        return;
    }

    while((file = readdir(d)) != NULL)
    {
        if(strncmp(file->d_name, ".", 1) == 0)
        {
            continue;
        }

        memset(full_path, 0, sizeof(full_path) / sizeof(char));
        sprintf(full_path, "%s/%s", path, file->d_name);

        if(stat(full_path, &st) >= 0 && S_ISDIR(st.st_mode) && depth <= DEPTH)
        {
            strcpy(dirs[dirs_len++], full_path);
            list_dir(full_path, depth + 1);
        }
    }
    closedir(d);
}

void handle_notify(s_notify *ntf, struct inotify_event *event)
{
    if(event->mask & IN_ISDIR)
    {
        if(event->len > 0)
        {
            //ignore hidden file
            if(event->name[0] != '.')
            {
                char new_path[PATH_MAX] = {0};
                int parent_wd = event->wd;
                for(int i = 0;i < ntf->s_watch_p_len;i++)
                {
                    if(parent_wd == ntf->s_watch_p[i].wd)
                    {
                        sprintf(new_path, "%s/%s", ntf->s_watch_p[i].wpath, event->name);
                        break;
                    }
                }
                notify_dir(ntf, new_path);
            }
        }
    }
    else
    {
        if(event->mask & flags)
        {
            if(event->len > 0)
            {
                //ignore hidden file
                if(event->name[0] != '.')
                {
                    printf("%s\n", event->name);
                    ntf->time = time(NULL);
                }
            }
        }
    }
}

int notify_dir(s_notify *ntf, const char *path_ptr)
{
    int wd = inotify_add_watch(ntf->notify_fd, path_ptr, flags);
    if(wd == -1)
    {
        printf("inotify_add_watch failed[%s]\n", path_ptr);
    }
    else
    {
        ntf->s_watch_p_len += 1;
        s_watch *s_w = (s_watch *)realloc(ntf->s_watch_p, sizeof(s_watch) * ntf->s_watch_p_len);
        s_w[ntf->s_watch_p_len - 1].wd = wd;
        strcpy(s_w[ntf->s_watch_p_len - 1].wpath, path_ptr);
        ntf->s_watch_p = s_w;
    }

    return wd;
}


void notify()
{
    for(int i = 0;i < s_notify_p_len;i++)
    {
        int nd = inotify_init();
        if(nd == -1)
        {
            printf("inotify_init failed\n");
            s_notify_p[i].notify_fd = -1;
            s_notify_p[i].s_watch_p = NULL;
            s_notify_p[i].s_watch_p_len = 0;
            continue;
        }

        s_notify_p[i].notify_fd = nd;

        int wd = notify_dir(&(s_notify_p[i]), s_notify_p[i].conf.path);
        if(wd != -1)
        {
            memset(dirs, 0, sizeof(dirs) / sizeof(char));
            dirs_len = 0;
            list_dir(s_notify_p[i].conf.path, 1);
            for(int j = 0; j < dirs_len;j++)
            {
                notify_dir(&(s_notify_p[i]), dirs[j]);
            }
        }
    }
}


void handle_watch(s_notify *ntf)
{
    char buf[BUF_LEN];
    size_t read_len;
    char *p;
    struct inotify_event *event;

    read_len = read(ntf->notify_fd, buf, BUF_LEN);
    if(read_len == -1)
    {
        printf("read failed\n");
    }

    for(p=buf;p < buf+read_len;)
    {
        event = (struct inotify_event *)p;
        handle_notify(ntf, event);
//        printf("%d %d %s\n", read_len, event->len, event->name);
        p += sizeof(struct inotify_event) + event->len;
    }
}

void watch()
{
    struct timeval tv;
    while(1)
    {
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        fd_set rd;
        int max_fd = 0;
        FD_ZERO(&rd);
        for(int i = 0;i < s_notify_p_len;i++)
        {
            FD_SET(s_notify_p[i].notify_fd, &rd);
            if(max_fd < s_notify_p[i].notify_fd)
            {
                max_fd = s_notify_p[i].notify_fd;
            }
        }

        int ret = select(max_fd + 1, &rd, NULL, NULL, &tv);

//        for(int i = 0; i < s_notify_p_len;i++)
//        {
//            time_t cmd_time = s_notify_p[i].time;
//            time_t now_time = time(NULL);
//            if(cmd_time == 0)
//            {
//                continue;
//            }
//            else
//            {
//                if(now_time - cmd_time >= s_notify_p[i].conf.delay)
//                {
//                    system(s_notify_p[i].conf.cmd);
//                    s_notify_p[i].time = 0;
//                }
//            }
//        }

        if(ret == -1)
        {
            continue;
        }
        else if(ret == 0)
        {
            continue;
        }
        else
        {
            for(int i = 0; i < s_notify_p_len;i++)
            {
                if(FD_ISSET(s_notify_p[i].notify_fd, &rd))
                {
                    handle_watch(&(s_notify_p[i]));
                }
            }
        }
    }
}
