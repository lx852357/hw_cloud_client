//Defines the entry point for the console application.

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <time.h>
//user defined
#include "message.h"
#include "socketClient.h"



int main(int argc, char * argv[])
{
    if(argc < 4)
    {
        printf("Usage: %s [player_id] [serverIp] [serverPort]\n", argv[0]);
        return -1;
    }

    /* 提取命令行参数 */
	clock_t start, ends;
    unsigned long serverIp = inet_addr(argv[2]);
    unsigned short serverPort = atoi(argv[3]);

#if defined(_MSC_VER)//win
    WORD wVersionRequested = MAKEWORD(1,1);
    WSADATA wsAdata;

    if(0 != WSAStartup(wVersionRequested,&wsAdata))
    {
        return -1;
    }

    if((LOBYTE(wsAdata.wVersion)!=1) || (HIBYTE(wsAdata.wVersion)!=1))
    {
        WSACleanup();
        return -1;
    }
#endif


    /* 创建socket */
    SocketClient client(serverIp, serverPort);


    printf("try to connect server(%s:%u)\n", inet_ntoa(client.addrSrv_.sin_addr), ntohs(client.addrSrv_.sin_port));

    while(0 != client.Connect())
    {
        sleep(10);
    };

    printf("connect server success\n", inet_ntoa(client.addrSrv_.sin_addr), ntohs(client.addrSrv_.sin_port));

	/* 将输出打印到log.txt中 */
	//std::ofstream log("log.txt");

    int myTeamId = atoi(argv[1]);
    int myPlayerId[4] = {0};

    /* 向server注册 */
    char regMsg[200]={'\0'};
    snprintf(regMsg, sizeof(regMsg), "{\"msg_name\":\"registration\",\"msg_data\":{\"team_id\":%d,\"team_name\":\"test_demo\"}}", myTeamId);
    char regMsgWithLength[200]={'\0'};
    snprintf(regMsgWithLength, sizeof(regMsgWithLength), "%05d%s", (int)strlen(regMsg), regMsg);
    client.Send(regMsgWithLength, (int)strlen(regMsgWithLength), 0);
//    send(sockClient, regMsgWithLength, (int)strlen(regMsgWithLength), 0);

    printf("register my info to server success\n");

    /* 进入游戏 */
    while(1)
    {
        char buffer[99999] = {'\0'};  
//        int size = recv(sockClient, buffer, sizeof(buffer)-1, 0);
        int size = client.Recv(buffer, sizeof(buffer)-1, 0);
		start = clock();
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
                client.Send(msgToSend, (int)strlen(msgToSend), 0);
				printf("send msg\n");
//                send(sockClient, msgToSend, (int)strlen(msgToSend), 0);
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
		ends = clock();

        /* 如果收到game_over消息, 请跳出循环，进入释放资源程序退出阶段 */
		std::cout << "delta T: " << ends - start << std::endl;
    }

    //close(sockClient);
	client.Close();

    return 0;
}



