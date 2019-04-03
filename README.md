# ftpupload
一开始用shell去做文件传输，可是脚本终归是脚本，好多状态监测不到，比如没法判断（或者说判断起来很麻烦）要传输的文件是否正在生成，是否生成完毕等等，所以决定还是用c去实现：
* inotify
* libcurl

大致流程如下：
1. 利用inotify监控文件是否创建，是否生成完毕，是否有子目录生成，是否有文件被移入。
2. 一旦有文件处于待传输状态，等待3秒后，再传输，此处是为了避免某些中间文件的传输。
3. 如果传输过程中网络异常，做5次重传尝试。

*注意：*
*由于inotify监控的是本地文件系统的状态，所以该程序具有以下限制：*
<table>
    <tbody>
        <tr>
            <th align="center">前置条件</th>
            <th align="center">是否可用</th>
        </tr>
    </tbody>
    <tbody>
        <tr>
            <td align="left">监控目录是本地目录并且该目录中的文件在本地生成</td>
            <td align="center">√</td>
        </tr>
    </tbody>
    <tbody>
        <tr>
            <td align="left">监控目录是本地目录也是共享目录（nfs，ftp等），该目录中的文件在本地生成</td>
            <td align="center">√</td>
        </tr>
    </tbody>
    <tbody>
        <tr>
            <td align="left">监控目录是本地目录也是共享目录（nfs，ftp等），该目录中的文件在其他机器上生成</td>
            <td align="center">√</td>
        </tr>
    </tbody>
    <tbody>
        <tr>
            <td align="left">监控目录是挂载的其他机器上的目录，该目录中的文件在本地生成</td>
            <td align="center">√</td>
        </tr>
    </tbody>
    <tbody>
        <tr>
            <td align="left">监控目录是挂载的其他机器上的目录，该目录中的文件在其他机器上生成</td>
            <td align="center">×</td>
        </tr>
    </tbody>
</table>

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
