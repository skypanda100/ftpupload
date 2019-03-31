#include "def.h"
#include "config.h"
#include "notify.h"

int main(int argc,char **argv)
{
//    daemon(0, 0);

    if(argc == 2)
    {
        // get infomation from conf file
        conf cf;
        config(&cf, argv[1]);

        // watch the path and upload
        watch(&cf);
    }

    return 0;
}