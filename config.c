//
// Created by zhengdongtian on 19-3-30.
//

#include "config.h"

extern s_notify *s_notify_p;
extern int s_notify_p_len;

static char *node = "fnotify";
static char *key_path = "path";
static char *key_cmd = "cmd";
static char *key_delay = "delay";

static char *l_trim(char *output, const char *input)
{
    assert(input != NULL);
    assert(output != NULL);
    assert(output != input);
    for(;*input != '\0' && isspace(*input);++input)
    {
        ;
    }
    return strcpy(output, input);
}

/*static char *r_trim(char *output, const char *input)
{
    char *p = NULL;
    assert(input != NULL);
    assert(output != NULL);
    assert(output != input);
    strcpy(output, input);
    for(p = output + strlen(output) - 1; p >= output && isspace(*p); --p)
    {
        ;
    }
    *(++p) = '\0';
    return output;
}*/

static char * a_trim(char * output, const char * input)
{
    char *p = NULL;
    assert(input != NULL);
    assert(output != NULL);
    l_trim(output, input);
    for(p = output + strlen(output) - 1;p >= output && isspace(*p); --p)
    {
        ;
    }
    *(++p) = '\0';
    return output;
}


void config(char *conf)
{
    char key[32] = {0};
    char val_path[PATH_MAX] = {0};
    char val_cmd[PATH_MAX] = {0};
    int val_delay = 0;
    char *buf, *c;
    char buf_i[KEY_VALUE_LEN], buf_o[KEY_VALUE_LEN];
    FILE *fp;
    int found = 0;
    if((fp=fopen( conf,"r" )) == NULL)
    {
        printf("openfile [%s] error [%s]\n", conf, strerror(errno));
        return;
    }
    fseek(fp, 0, SEEK_SET);


    while(!feof(fp) && fgets(buf_i, KEY_VALUE_LEN, fp) != NULL)
    {
        l_trim(buf_o, buf_i);
        if(strlen(buf_o) <= 0)
            continue;
        buf = buf_o;

        if(found == 0)
        {
            memset(key, 0, sizeof(key) / sizeof(char));
            sprintf(key, "[%s]", node);
            if(buf[0] != '[')
            {
                continue;
            }
            else if(strncmp(buf, key, strlen(key)) == 0)
            {
                found = 1;
                continue;
            }

        }
        else if(found == 1)
        {
            if(buf[0] == '#')
            {
                continue;
            }
            else if(buf[0] == '[')
            {
                break;
            }
            else
            {
                if((c = strchr(buf, '=')) == NULL)
                    continue;
                memset(key, 0, sizeof(key));
                sscanf(buf, "%[^=|^ |^\t]", key);
                if(strcmp(key, key_path) == 0)
                {
                    sscanf(++c, "%[^\n]", val_path);
                    char *val_o = (char *)malloc(strlen(val_path) + 1);
                    if(val_o != NULL){
                        memset(val_o, 0, strlen(val_path) + 1);
                        a_trim(val_o, val_path);
                        if(val_o && strlen(val_o) > 0)
                            strcpy(val_path, val_o);
                        free(val_o);
                        val_o = NULL;
                    }
                    found = 2;
                } else {
                    break;
                }
            }
        }
        else if(found == 2)
        {
            if(buf[0] == '#')
            {
                continue;
            }
            else if(buf[0] == '[')
            {
                break;
            }
            else
            {
                if((c = strchr(buf, '=')) == NULL)
                    continue;
                memset(key, 0, sizeof(key));
                sscanf(buf, "%[^=|^ |^\t]", key);
                if(strcmp(key, key_cmd) == 0)
                {
                    sscanf(++c, "%[^\n]", val_cmd);
                    char *val_o = (char *)malloc(strlen(val_cmd) + 1);
                    if(val_o != NULL){
                        memset(val_o, 0, strlen(val_cmd) + 1);
                        a_trim(val_o, val_cmd);
                        if(val_o && strlen(val_o) > 0)
                            strcpy(val_cmd, val_o);
                        free(val_o);
                        val_o = NULL;
                    }
                    found = 3;
                } else {
                    break;
                }
            }
        }
        else if(found == 3)
        {
            if(buf[0] == '#')
            {
                continue;
            }
            else if(buf[0] == '[')
            {
                break;
            }
            else
            {
                if((c = strchr(buf, '=')) == NULL)
                    continue;
                memset(key, 0, sizeof(key));
                sscanf(buf, "%[^=|^ |^\t]", key);
                if(strcmp(key, key_delay) == 0)
                {
                    char val[20] = {0};
                    sscanf(++c, "%[^\n]", val);
                    char *val_o = (char *)malloc(strlen(val) + 1);
                    if(val_o != NULL){
                        memset(val_o, 0, strlen(val) + 1);
                        a_trim(val_o, val);
                        if(val_o && strlen(val_o) > 0)
                            strcpy(val, val_o);
                        free(val_o);
                        val_o = NULL;
                        val_delay = atoi(val);

                        s_notify_p_len += 1;
                        s_notify_p = (s_notify *)realloc(s_notify_p, sizeof(s_notify) * s_notify_p_len);
                        strcpy(s_notify_p[s_notify_p_len - 1].conf.path, val_path);
                        strcpy(s_notify_p[s_notify_p_len - 1].conf.cmd, val_cmd);
                        s_notify_p[s_notify_p_len - 1].conf.delay = val_delay;
                        s_notify_p[s_notify_p_len - 1].notify_fd = -1;
                        s_notify_p[s_notify_p_len - 1].s_watch_p = NULL;
                        s_notify_p[s_notify_p_len - 1].s_watch_p_len = 0;
                        s_notify_p[s_notify_p_len - 1].time = 0;
                    }
                    found = 0;
                } else {
                    break;
                }
            }
        }
    }
    fclose(fp);
}
