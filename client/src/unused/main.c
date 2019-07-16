#include "head.h"
#include "transmission.h"
#include "client.h"

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        printf("wrong arg num\n");
        return -1;
    }

    int ret;

    DataPackage data;
    char user_name[USER_LEN + 1];

    TransInfo trans_info;
    strcpy(trans_info.ip_address, argv[1]);
    strcpy(trans_info.port, argv[2]);
    pthread_t tran_thread;

    int socketFd;

    system("clear");
    int regi_flag = 0;
    while (1)
    {
        ret = login_page(regi_flag);
        if (ret == '1')
        {
            //user authentication
            ret = tran_authen(&socketFd, argv[1], argv[2], user_name, &data, &trans_info);
            if (ret == -1)
            {
                continue;
            }
            strcpy(trans_info.user_name, user_name);
            break;
        }
        else if (ret == '2')
        {
            ret = user_signup(&socketFd, argv[1], argv[2], user_name, &data);
            if (ret == -1)
            {
                //delete key file
                char pk_path[FILE_NAME_LEN];
                sprintf(pk_path, "%s_rsa.key", user_name);
                ret = access(pk_path, F_OK);
                if (ret == 0)
                {
                    remove(pk_path);
                }
                sprintf(pk_path, "%s_rsa_pub.key", user_name);
                ret = access(pk_path, F_OK);
                if (ret == 0)
                {
                    remove(pk_path);
                }
                regi_flag = -1;
                continue;
            }
            regi_flag = 1;
        }
        else if (ret == '0')
        {
            return 0;
        }
    }

    system("clear");
    print_help();
    while (1)
    {
        data.data_len = read(STDIN_FILENO, data.buf, sizeof(data.buf));
        data.buf[data.data_len - 1] = '\0';

        ret = cmd_interpret(&data);
        if (ret == 1 || ret == -1)      //0 for normal, 1 for help page, -1 for error
        {
            continue;
        }
        else if (ret == 2)          // 2 for gets
        {
            char file_name[CMD_LEN];
            int i = strlen(data.buf);
            while (data.buf[i] != ' ' && data.buf[i] != '/')
            {
                i--;
            }
            i++;
            int j = 0;
            while (data.buf[i - 1] != '\0')
            {
                file_name[j++] = data.buf[i++];
            }
            char file_path[CMD_LEN];
            sprintf(file_path, "downloads/%s/%s", user_name, file_name);
            if (access(file_path, F_OK) == 0)
            {
                system("clear");
                printf("-----$ %s\n", data.buf);
                printf("file with the same name already exists\n");
                continue;
            }
            strcpy(trans_info.cmd, data.buf);
            pthread_create(&tran_thread, NULL, get_files, &trans_info);
        }
        else if (ret == 3)          // 3 for puts
        {
            strcpy(trans_info.cmd, data.buf);
            pthread_create(&tran_thread, NULL, put_files, &trans_info);
        }
        else
        {
            tran_cmd(socketFd, &data);
        }
    }
    return 0;
}

