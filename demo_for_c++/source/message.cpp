
#include "message.h"


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


/**
Leg Start Msg
*/

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
	 //GetCloud(myTeamId);
     GetMyTeamInfo(myTeamId,myPlayerId);
	 GenerateMap(height->valueint, width->valueint);


     

}

void LegStartMsg::GetMyTeamInfo(int& myTeamId,int myPlayerId[4])
{
    cJSON* msg_data = cJSON_GetObjectItem(root,"msg_data");
	mTeamInfo.id = myTeamId;

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
					mTeamInfo.players[j] = playerInfo->valueint;
					myPlayerId[j] = playerInfo->valueint;
				}
            }
			cJSON* force = cJSON_GetObjectItem(teamInfo, "force");
			if (NULL == force) return;
			mTeamInfo.force = force->valuestring;
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
		Point p;
        cJSON* sub_meteor = cJSON_GetArrayItem(meteor,i);
        if (NULL != sub_meteor)
        {
            cJSON* x = cJSON_GetObjectItem(sub_meteor,"x");
            if (NULL == x) return;

            cJSON* y  = cJSON_GetObjectItem(sub_meteor,"y");
            if(NULL == y) return;
			p.x = x->valueint;
			p.y = y->valueint;
			mMeteors.push_back(p);
        }
    }
	// test print
	/*for (int i = 0; i < mMeteors.size(); i++) {
		printf("metros x:%d, y:%d", mMeteors[i].x, mMeteors[i].y);
	}*/
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

	int temp[26][3] = {0};
	int point_temp[27] = { 0 };//表示哪一个temp里有值，0表示没有，1表示有，最后一位为计数位，判断有值的个数
	std::cout << "wormlen: " << wormhole_len << std::endl;
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
			char c = *(name->valuestring);
			int ic = c;
			if (c < 'a') {//upper  to lower
				ic = c + 'a' - 'A';
			}
			
			if (point_temp[26] == 0) {
				temp[0][0] = ic;
				temp[0][1] = y->valueint;
				temp[0][2] = x->valueint;
				point_temp[0] = 1;
				point_temp[26] += 1;
			}
			else {
				bool isAdd = true;
				for (int i = 0; i < 26; i++) {
					if (point_temp[i] == 1 && temp[i][0] == ic) {//delete same name hole
						//std::cout << "delete : " << ic << " in " << i << std::endl;
						WormholePair w;
						w.name1 = temp[i][0];
						w.name2 = ic;

						w.point1.y = temp[i][1];
						w.point1.x = temp[i][2];

						w.point2.y = y->valueint;
						w.point2.x = x->valueint;

						mWormholePairs.push_back(w);

						//置标志位
						temp[i][0] = 0;
						point_temp[i] = 0;
						point_temp[26] -= 1;
						isAdd = false;

						break;
					}
				}
				if (isAdd) {// add 
					for (int i = 0; i < 26; i++) {
						if (point_temp[i] == 0) {
							//std::cout << "you need add : " << ic << " in " << i << std::endl;
							temp[i][0] = ic;
							point_temp[i] = 1;
							point_temp[26] += 1;
							isAdd = false;

							temp[i][1] = y->valueint;
							temp[i][2] = x->valueint;
							break;
						}
					}
				}
			}
			for (int i = 0; i < 26; i++) {
				std::cout << temp[i][0] << "  ";
			}
			std::cout << "===========" << point_temp[26] << std::endl;
			
        }
    }//for
	for (auto wormholePairs : mWormholePairs) {
		std::cout << "point1: x: " << wormholePairs.point1.x << ",y: " << wormholePairs.point1.y
			<< " name: " << wormholePairs.name1
			<< "\t point2:x " << wormholePairs.point2.x << ",y: " << wormholePairs.point2.y 
			<< " name: " << wormholePairs.name2
			<< std::endl;
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
		Tunnel t;
        cJSON* sub_tunnel = cJSON_GetArrayItem(tunnel,i);
        if (NULL != sub_tunnel)
        {
            cJSON* x = cJSON_GetObjectItem(sub_tunnel,"x");
            if (NULL == x) return;

            cJSON* y  = cJSON_GetObjectItem(sub_tunnel,"y");
            if(NULL == y) return;

            cJSON* direction = cJSON_GetObjectItem(sub_tunnel,"direction");
            if(NULL == direction) return;

			t.point.x = x->valueint;
			t.point.y = y->valueint;
			char* d = direction->valuestring;
			if (strcmp(d, "up") == 0) {
				t.direct = UP;
			}
			if (strcmp(d, "down") == 0) {
				t.direct = DOWN;
			}
			if (strcmp(d, "left") == 0) {
				t.direct = LEFT;
			}
			if (strcmp(d, "right") == 0) {
				t.direct = RIGHT;
			}			
        }
		mTunnels.push_back(t);
    }
	/*for (auto tunnel : mTunnels) {
		std::cout << "x: " << tunnel.point.x << ",y: " << tunnel.point.y
			<< "\tdirect: " << DIRECTLOG[tunnel.direct-20] << std::endl;
	}*/
    
}


/*************************
0：地图可行区域
1 - 5：分数奖励
10 - 17：player信息
8：障碍物
虫洞id的ascii码：表示虫洞对
20 - 23表示滑梯的上下左右
h->y
w->x
**************************/
void LegStartMsg::GenerateMap(int h, int w) {

	//获取宽高
	mGameMap.h = h;
	mGameMap.w = w;

	//生成陨石
	for (auto metoer : mMeteors) {
		mGameMap.map[metoer.y][metoer.x] = 8;
	}

	//生成通道
	for (auto tuunel : mTunnels) {
		mGameMap.map[tuunel.point.y][tuunel.point.x] = tuunel.direct;
	}

	//生成虫洞
	std::cout << "====generated wormhole====" << std::endl;

	for (auto wormhole : mWormholePairs) {
		mGameMap.map[wormhole.point1.y][wormhole.point1.x] = wormhole.name1;
		mGameMap.map[wormhole.point2.y][wormhole.point2.x] = wormhole.name2;
		std::cout << "point1: x: " << wormhole.point1.x << ",y: " << wormhole.point1.y
			<< " name: " << wormhole.name1
			<< "\t point2:x " << wormhole.point2.x << ",y: " << wormhole.point2.y
			<< " name: " << wormhole.name2
			<< std::endl;
	}

	std::cout << "====generated map====" << std::endl;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			std::cout << mGameMap.map[i][j] << "  ";
		}
		std::cout << std::endl;
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

/**
Round Msg
*/

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




