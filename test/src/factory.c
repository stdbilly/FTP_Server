#include "../include/factory.h"

void cleanUp(void *p) {
    pthread_mutex_unlock((pthread_mutex_t*)p);
    //printf("clean success\n");
}
void* threadFunc(void *p) {
    pFactory_t pthreadInfo=(pFactory_t)p;
    pQue_t pq=&pthreadInfo->que;
    pNode_t pGet;
    int getTaskSuccess;
    while(1){
        pthread_mutex_lock(&pq->mutex);
        pthread_cleanup_push(cleanUp,&pq->mutex);
        if(!pq->queSize){
            pthread_cond_wait(&pthreadInfo->cond,&pq->mutex);
        }
        getTaskSuccess=queGet(pq,&pGet);//拿任务
        pthread_cleanup_pop(1);
        if(!getTaskSuccess){
           transFile(pGet->clientFd);
           close(pGet->clientFd);
           free(pGet);
        }
        pGet=NULL;
    }
}

int factoryStart(pFactory_t p) {
    if(!p->startFlag){
        int i;
        for(i=0;i<p->threadNum;i++) {
            pthread_create(p->pthid+i,NULL,threadFunc,p);
        }
        p->startFlag=1;
    }
    return 0;
}

int epollAdd(int epfd,int fd) {
    struct epoll_event event;
    event.events=EPOLLIN;
    event.data.fd=fd;
    int ret;
    ret=epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&event);
    ERROR_CHECK(ret,-1,"epoll_ctl");
    return 0;
}

int factoryInit(int *sfd,pFactory_t p) {
    int port, threadNum, capacity,socketFd;
    char ip[20]={0};
    FILE *config;
    config=fopen(SERVER_CONF,"r");
    fscanf(config,"%s %d %d %d",ip,&port,&threadNum,&capacity);

    queInit(&p->que,capacity);
    pthread_cond_init(&p->cond,NULL);
    p->pthid=(pthread_t*)calloc(threadNum,sizeof(pthread_t));
    p->threadNum=threadNum;
    p->startFlag=0;

    socketFd=socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(socketFd,-1,"socket");
    struct sockaddr_in serAddr;
    bzero(&serAddr,sizeof(serAddr));
    serAddr.sin_family=AF_INET;
    serAddr.sin_port=htons(port);
    serAddr.sin_addr.s_addr=inet_addr(ip);
    int ret,reuse=1;
    ret=setsockopt(socketFd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));
    ERROR_CHECK(ret,-1,"setsockopt");
    ret=bind(socketFd,(struct sockaddr*)&serAddr,sizeof(serAddr));
    ERROR_CHECK(ret,-1,"bind");
    listen(socketFd,10);
    *sfd=socketFd;
    return 0;
}

