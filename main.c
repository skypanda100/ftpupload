#include "def.h"
#include "config.h"
#include "notify.h"

conf cf;

int main(int argc,char **argv)
{
//    daemon(0, 1);

    if(argc == 2)
    {
        // get infomation from conf file
        config(argv[1]);

        // watch the path and upload
        watch();
    }
    else
    {
        fprintf(stderr, "argument error, please input absolute path of conf file!\n");
    }

    return 0;
}