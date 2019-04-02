# ftpupload
一开始用shell去做文件传输，可是脚本终归是脚本，好多状态监测不到，比如没法判断要传输的文件是否正在生成，是否生成完毕等等，所以决定还是用c去实现：
* inotify
* libcurl

大致流程如下：
1. 利用inotify监控文件是否创建，是否生成完毕，是否有子目录生成，是否有文件被移入。
2. 一旦有文件处于待传输状态，等待3秒后，再传输，此处是为了避免某些中间文件的传输。
3. 如果传输过程中网络异常，做5次重传尝试。

# Example Usage
*project path is `/root/ftpupload`*
* build  
    ```bash
    $ cd /root/ftpupload
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    ```
* conf

    ```bash
    $ cat /root/ftpupload/conf/example.conf
    src_dir=[dir]
    dst_dir=ftp://[ip]/[dir]/
    user_pwd=[user]:[password]
    log=[dir]
    ```
* execute  
    ```bash
    $ cd /root/ftpupload/build
    $ ./ftpupload /root/ftpupload/conf/example.conf
    ```
