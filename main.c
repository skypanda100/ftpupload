#include "def.h"
#include "config.h"
#include "notify.h"

s_notify *s_notify_p = NULL;
int s_notify_p_len = 0;

int main(int argc,char **argv)
{
//    daemon(0, 0);

    if(argc == 2)
    {
        // get infomation from conf file
        config(argv[1]);
        if(s_notify_p_len == 0)
        {
            return -1;
        }

        // notify the path
        notify();
        int i = 0;
        for(;i < s_notify_p_len;i++)
        {
            if(s_notify_p[i].notify_fd != -1
               && s_notify_p[i].s_watch_p_len > 0)
            {
                break;
            }
        }
        if(i == s_notify_p_len)
        {
            return -1;
        }

        // watch the path and do shell command
        watch();
    }

    return 0;
}