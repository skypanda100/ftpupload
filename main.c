#include <sys/wait.h>
#include <sys/prctl.h>
#include "def.h"
#include "notify.h"

void set_process_title(int argc, char **argv, const char *fmt, ...);
void sub_quit_signal_handle(int sig);
int sub_process(int index);

conf *cf_ptr = NULL;
int conf_len = 0;

int main(int argc,char **argv)
{
    daemon(0, 1);

    int *pid_ptr = NULL;
    int pid_len = 0;

    if(argc == 2)
    {
        // get infomation from conf file
        config(argv[1]);

        // alloc memory to process_id_ptr
        pid_len = conf_len;
        pid_ptr = (int *)malloc(pid_len * sizeof(int));
        memset(pid_ptr, 0, pid_len * sizeof(int));

        // watch the path and upload
        for(int i = 0;i < conf_len;i++)
        {
            int process_id = fork();
            *(pid_ptr + i) = process_id;
            if(process_id == 0)
            {
                set_process_title(argc, argv, "%s@%d", argv[0], i + 1);
                return sub_process(i);
            }
        }
    }
    else
    {
        fprintf(stderr, "argument error, please input absolute path of conf file!\n");
        exit(-1);
    }

    signal(SIGCHLD, sub_quit_signal_handle);

    for(;;)
    {
        sleep(30);

        for(int i = 0;i < pid_len;i++)
        {
            int pid = *(pid_ptr + i);
            if(kill(pid, 0) != 0)
            {
                printf("pid[%d] is dead, it will be restarted in a few seconds!\n", pid);
                int process_id = fork();
                *(pid_ptr + i) = process_id;
                if(process_id == 0)
                {
                    set_process_title(argc, argv, "%s@%d", argv[0], i + 1);
                    return sub_process(i);
                }
            }
            else
            {
                printf("pid[%d] is alive!\n", pid);
            }
        }
    }

    return 0;
}

void set_process_title(int argc, char **argv, const char *fmt, ...)
{
    int arg_len = 0;
    char buf[2048];
    int buf_len = 0;
    va_list ap;

    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    for(int i = 0;i < argc;i++)
    {
        arg_len += strlen(argv[i]) + 1;
    }

    buf_len = strlen(buf);

    strcpy(argv[0], buf);
    for(int i = buf_len;i < arg_len;i++)
    {
        argv[0][i] = '\0';
    }
    argv[1] = NULL;

    // 调用prctl
    prctl(PR_SET_NAME, buf);
}

void sub_quit_signal_handle(int sig)
{
    int status;
    int quit_pid = wait(&status);
    printf("sub process %d quit, exit status %d\n", quit_pid, status);
}

int sub_process(int index)
{
    watch(&(cf_ptr[index]));
}
