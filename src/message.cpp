
#include "message.h"
#include "string"


const char* direct[] = {"up","down","left","right",""};

ActMsg::ActMsg(int roundId)
{
    root=cJSON_CreateObject();
    cJSON_AddItemToObject(root, "msg_name", cJSON_CreateString("action"));
    cJSON_AddItemToObject(root, "msg_data", msg_data=cJSON_CreateObject());
    cJSON_AddNumberToObject(msg_data,"round_id",roundId);
    cJSON_AddItemToObject(msg_data, "actions", actions=cJSON_CreateArray());
}

void ActMsg::AddSubAction(int team, int player, SubAction& act)
{
    cJSON_AddItemToArray(actions,subAct=cJSON_CreateObject());
    cJSON_AddNumberToObject(subAct,"team",team);
    cJSON_AddNumberToObject(subAct,"player_id",player);
    cJSON_AddItemToObject(subAct, "move", move=cJSON_CreateArray());
    if (act.moveDirect < NO_MOVE)
    {
        cJSON_AddItemToArray(move,cJSON_CreateString(direct[act.moveDirect]));
    }
}

void ActMsg::PackActMsg(char* actMsg, int maxMsgLenth)
{
    char* msg = cJSON_PrintUnformatted(root);
    printf("\r\nSendActMsg:%s\r\n",msg);

    snprintf(actMsg, maxMsgLenth, "%05d%s", (int)strlen(msg), msg);
    free(msg);
}



void LegStartMsg::DecodeMessge(int& myTeamId,int myPlayerId[4])
{
    cJSON* msg_data = cJSON_GetObjectItem(root,"msg_data");
    if(NULL == msg_data) return;

    cJSON* map = cJSON_GetObjectItem(msg_data,"map");
    if(NULL == map) return;

    cJSON* width = cJSON_GetObjectItem(map,"width");
    if(NULL == width) return;
    
    cJSON* height = cJSON_GetObjectItem(map,"height");
    if(NULL == height) return;
    
    cJSON* vision = cJSON_GetObjectItem(map,"vision");
    if(NULL == vision) return;

     GetMeteor(myTeamId);
     GetWormhole(myTeamId);
     GetTunnel(myTeamId);
	 GetCloud(myTeamId);
     GetMyTeamInfo(myTeamId,myPlayerId);
     

}

void LegStartMsg::GetMyTeamInfo(int& myTeamId,int myPlayerId[4])
{
    cJSON* msg_data = cJSON_GetObjectItem(root,"msg_data");

    cJSON* teams = cJSON_GetObjectItem(msg_data,"teams");
    if(NULL == teams) return;

    int len = cJSON_GetArraySize(teams);
    if(len > 4)  return;

    for (int i = 0; i< len; ++i)
    {
        cJSON* teamInfo = cJSON_GetArrayItem(teams,i);
        if (NULL != teamInfo)
        {
            cJSON* id = cJSON_GetObjectItem(teamInfo,"id");
            if (NULL == id) return;
			if (myTeamId != id->valueint) continue;

            cJSON* players  = cJSON_GetObjectItem(teamInfo,"players");
            if(NULL == players) return;

            int playerCount = cJSON_GetArraySize(players);
            for (int j = 0; j < playerCount; ++j)
            {
                cJSON* playerInfo = cJSON_GetArrayItem(players,j);
                if(NULL == playerInfo) return;

              
				if (myTeamId == id->valueint)
				{
					myPlayerId[j] = playerInfo->valueint;
				}
            }
        }
    }
}
void LegStartMsg::GetMeteor(int& myTeamId)
{
    cJSON* msg_data = cJSON_GetObjectItem(root,"msg_data");
    if(NULL == msg_data) return;

    cJSON* map = cJSON_GetObjectItem(msg_data,"map");
    if(NULL == map) return;

    cJSON* meteor = cJSON_GetObjectItem(map,"meteor");
    if(NULL == meteor) return;

    int len = cJSON_GetArraySize(meteor);
    
    for (int i = 0; i< len; ++i)
    {
        cJSON* sub_meteor = cJSON_GetArrayItem(meteor,i);
        if (NULL != sub_meteor)
        {
            cJSON* x = cJSON_GetObjectItem(sub_meteor,"x");
            if (NULL == x) return;

            cJSON* y  = cJSON_GetObjectItem(sub_meteor,"y");
            if(NULL == y) return;
        }
    }
}

void LegStartMsg::GetWormhole(int& myTeamId)
{
    cJSON* msg_data = cJSON_GetObjectItem(root,"msg_data");
    if(NULL == msg_data) return;

    cJSON* map = cJSON_GetObjectItem(msg_data,"map");
    if(NULL == map) return;

    cJSON* wormhole = cJSON_GetObjectItem(map,"wormhole");
    if(NULL == wormhole) return;
    
    int wormhole_len = cJSON_GetArraySize(wormhole);
    
    for (int i = 0; i< wormhole_len; ++i)
    {
        cJSON* sub_wormhole = cJSON_GetArrayItem(wormhole,i);
        if (NULL != sub_wormhole)
        {
            cJSON* x = cJSON_GetObjectItem(sub_wormhole,"x");
            if (NULL == x) return;

            cJSON* y  = cJSON_GetObjectItem(sub_wormhole,"y");
            if(NULL == y) return;
            
            cJSON* name = cJSON_GetObjectItem(sub_wormhole,"name");
            if(NULL == name) return;
        }
    }
}

void LegStartMsg::GetTunnel(int& myTeamId)
{
    cJSON* msg_data = cJSON_GetObjectItem(root,"msg_data");
    if(NULL == msg_data) return;

    cJSON* map = cJSON_GetObjectItem(msg_data,"map");
    if(NULL == map) return;

    cJSON* tunnel = cJSON_GetObjectItem(map,"tunnel");
    if(NULL == tunnel) return;

    int tunnel_len = cJSON_GetArraySize(tunnel);
    
    for (int i = 0; i< tunnel_len; ++i)
    {
        cJSON* sub_tunnel = cJSON_GetArrayItem(tunnel,i);
        if (NULL != sub_tunnel)
        {
            cJSON* x = cJSON_GetObjectItem(sub_tunnel,"x");
            if (NULL == x) return;

            cJSON* y  = cJSON_GetObjectItem(sub_tunnel,"y");
            if(NULL == y) return;

            cJSON* direction = cJSON_GetObjectItem(sub_tunnel,"direction");
            if(NULL == direction) return;
        }
    }
    
}

void LegStartMsg::GetCloud(int& myTeamId)
{
    cJSON* msg_data = cJSON_GetObjectItem(root,"msg_data");
    if(NULL == msg_data) return;

    cJSON* map = cJSON_GetObjectItem(msg_data,"map");
    if(NULL == map) return;

    cJSON* cloud = cJSON_GetObjectItem(map,"cloud");
    if(NULL == cloud) return;

    int cloud_len = cJSON_GetArraySize(cloud);
    
    for (int i = 0; i< cloud_len; ++i)
    {
        cJSON* sub_cloud = cJSON_GetArrayItem(cloud,i);
        if (NULL != sub_cloud)
        {
            cJSON* x = cJSON_GetObjectItem(sub_cloud,"x");
            if (NULL == x) return;

            cJSON* y  = cJSON_GetObjectItem(sub_cloud,"y");
            if(NULL == y) return;
        }
    }
    
}

void RoundMsg::DecodePower(cJSON *power)
{
    int len = cJSON_GetArraySize(power);
    for (int i = 0; i < len; ++i)
    {
        cJSON* powerInfo = cJSON_GetArrayItem(power,i);
        if (NULL != powerInfo)
        {
            cJSON* x     = cJSON_GetObjectItem(powerInfo,"x");
            cJSON* y     = cJSON_GetObjectItem(powerInfo,"y");
            cJSON* point = cJSON_GetObjectItem(powerInfo,"point");
            if((NULL == x)||(NULL == y)||(NULL == powerInfo)) return;

            //使用字段(x->valueint,y->valueint,point->valueint)
        }
    }
}

void RoundMsg::DecodePlayers(cJSON *players)
{
    int len = cJSON_GetArraySize(players);
    for (int i = 0; i < len; ++i)
    {
        cJSON* playerInfo = cJSON_GetArrayItem(players,i);
        if (NULL != playerInfo)
        {
            cJSON* id= cJSON_GetObjectItem(playerInfo,"id");
            cJSON* score = cJSON_GetObjectItem(playerInfo,"score");
            cJSON* sleep= cJSON_GetObjectItem(playerInfo,"sleep");
            cJSON* team = cJSON_GetObjectItem(playerInfo,"team");
            cJSON* x = cJSON_GetObjectItem(playerInfo,"x");
            cJSON* y = cJSON_GetObjectItem(playerInfo,"y");
            if((NULL == id)||(NULL == score)) return;

            //使用字段(x->valueint,y->valueint,0)
        }
    }
}


void RoundMsg::DecodeTeams(cJSON *teams)
{
    int len = cJSON_GetArraySize(teams);
    for (int i = 0; i < len; ++i)
    {
        cJSON* teamInfo = cJSON_GetArrayItem(teams,i);
        if (NULL != teamInfo)
        {
            cJSON* id    = cJSON_GetObjectItem(teamInfo,"id");
            cJSON* point = cJSON_GetObjectItem(teamInfo,"point");
			cJSON* remain_life = cJSON_GetObjectItem(teamInfo,"remain_life");
            if((NULL == id)||(NULL == point)) return;

            //使用字段(id->valueint,point->valueint,remain_life->valueint)
        }
    }
}

void RoundMsg::DecodeMessge()
{
    cJSON* msg_data = cJSON_GetObjectItem(root,"msg_data");
    if(NULL == msg_data) return;

    cJSON* round_id = cJSON_GetObjectItem(msg_data,"round_id");
    if(NULL == round_id) return;
	this->round_id = round_id->valueint;

	printf("\r\n ============ roundId: %d ============ \r\n ", round_id->valueint);

    cJSON* players = cJSON_GetObjectItem(msg_data,"players");
    if(NULL == players) return;
    DecodePlayers(players);

    cJSON* teams = cJSON_GetObjectItem(msg_data,"teams");
    if(NULL == teams) return;
    DecodeTeams(teams);  


    cJSON* power = cJSON_GetObjectItem(msg_data,"power");
    if(NULL == power) return;
    DecodePower(power);
}




