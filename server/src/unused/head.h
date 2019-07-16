#ifndef __FUN_H__
#define __FUN_H__

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <mysql/mysql.h>
#include <assert.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <openssl/err.h>

#define _DEBUG

#define RESULT_LEN 1000
#define INT_LEN 12
#define MD5_LEN 150
#define FILE_NAME_LEN 100
#define RSA_EN_LEN 256
#define RSA_DE_LEN 245
#define SER_EN_LEN 384
#define SER_DE_LEN 373

#endif

