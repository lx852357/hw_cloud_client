//Defines the entry point for the console application.

#include <iostream>
#include <string>
#include <ctime>
#include <unistd.h>
#include <string.h>
//for socket
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include<unistd.h>
#include <sys/termios.h>
#include <fstream>
#include "message.h"

int sockClient;


int main(int argc, char * argv[])
{
    if(argc < 4)
    {
        printf("Usage: %s [player_id] [serverIp] [serverPort]\n", argv[0]);
        return -1;
    }

    /* 提取命令行参数 */

    unsigned long serverIp = inet_addr(argv[2]);
    unsigned short serverPort = atoi(argv[3]);


    /* 创建socket */
    sockClient = socket(AF_INET,SOCK_STREAM,0);

    /* 连接server */
    struct sockaddr_in addrSrv;
    bzero( &addrSrv, sizeof(addrSrv) );
    addrSrv.sin_addr.s_addr = serverIp;
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(serverPort);

    printf("try to connect server(%s:%u)\n", inet_ntoa(addrSrv.sin_addr), ntohs(addrSrv.sin_port));

    while(0 != connect(sockClient,(sockaddr*)&addrSrv,sizeof(sockaddr)))
    {
        sleep(10);
    };

    printf("connect server success\n", inet_ntoa(addrSrv.sin_addr), ntohs(addrSrv.sin_port));    

    int myTeamId = atoi(argv[1]);
    int myPlayerId[4] = {0};

    /* 向server注册 */
    char regMsg[200]={'\0'};
    snprintf(regMsg, sizeof(regMsg), "{\"msg_name\":\"registration\",\"msg_data\":{\"team_id\":%d,\"team_name\":\"test_demo\"}}", myTeamId);
    char regMsgWithLength[200]={'\0'};
    snprintf(regMsgWithLength, sizeof(regMsgWithLength), "%05d%s", (int)strlen(regMsg), regMsg);
    send(sockClient, regMsgWithLength, (int)strlen(regMsgWithLength), 0);    

    printf("register my info to server success\n");

    /* 进入游戏 */
    while(1)
    {
        char buffer[99999] = {'\0'};  
        int size = recv(sockClient, buffer, sizeof(buffer)-1, 0);
        if (size > 0)
        {
            //printf("\r\n Round Server Msg: %s\r\n", buffer);
            
            cJSON *msgBuf = cJSON_Parse(buffer+5);
            if(NULL == msgBuf) continue;

            printf("\r\n OnMssageRecv: %d\r\n ",clock());            
            cJSON* msgNamePtr = cJSON_GetObjectItem(msgBuf,"msg_name");
            if(NULL == msgNamePtr) continue;

            char* msgName = msgNamePtr->valuestring;

            if (0 == strcmp(msgName,"round"))
            {                
                RoundMsg roundMsg(msgBuf);
                roundMsg.DecodeMessge();

                //根据策略和寻路决定下一步的动作，向服务器发送action消息
                //Demo程序直接发送随机动作
                ActMsg actMsg(roundMsg.GetRoundId());
	            for (int index = 0; index < 4; ++index)
	            {
		            SubAction action;
		            action.moveDirect  = (DIRECT)(rand() % 4);
		            actMsg.AddSubAction(myTeamId, myPlayerId[index], action);
	            }
                const int maxActMsgLenth = 9999;
                char msgToSend[maxActMsgLenth] = {0};
                actMsg.PackActMsg(msgToSend,maxActMsgLenth);
                send(sockClient, msgToSend, (int)strlen(msgToSend), 0);
            }
            else if (0 == strcmp(msgName,"leg_start"))
            {
                LegStartMsg legMsg(msgBuf);
                legMsg.DecodeMessge(myTeamId,myPlayerId);
            }
            else if(0 == strcmp(msgName,"leg_end"))
            {
                LegEndMsg legMsg(msgBuf);
                legMsg.DecodeMessge();
            }
            else if(0 == strcmp(msgName,"game_over"))
            {
                break;
            }
        }
        /* 如果收到game_over消息, 请跳出循环，进入释放资源程序退出阶段 */
    }

    close(sockClient);


    return 0;
}



