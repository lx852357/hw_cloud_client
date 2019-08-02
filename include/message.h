#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <string.h>
#include "cJSON.h"
enum DIRECT
{
	MOVE_UP = 0,
	MOVE_DOWN = 1,
	MOVE_LEFT = 2,
	MOVE_RIGHT = 3,
	NO_MOVE = 4
};

struct SubAction
{
	DIRECT moveDirect;
};

class ActMsg
{
public:
    ActMsg(int roundId);
    ~ActMsg(){cJSON_Delete(root);}
    void AddSubAction(int team, int player, SubAction& act);

    void PackActMsg(char* actMsg, int maxMsgLenth);
public:
    cJSON* root;
    cJSON* msg_data;
    cJSON* actions;
    cJSON* subAct;
    cJSON* move;
};

class LegStartMsg
{
public:
    LegStartMsg(cJSON* msg){root = msg;};
    ~LegStartMsg(){cJSON_Delete(root);};

    void DecodeMessge(int& myTeamId,int myPlayerId[4]);
    void GetMyTeamInfo(int& myTeamId,int myPlayerId[4]);
    void GetMeteor(int& myTeamId);
    void GetWormhole(int& myTeamId);
    void GetTunnel(int& myTeamId);
    void GetTeams(int& myTeamId);
    void GetCloud(int& myTeamId);
private:

public:
    cJSON* root;
};

class RoundMsg
{
public:
    RoundMsg(cJSON* msg){root = msg;}
    ~RoundMsg(){cJSON_Delete(root);}
    void DecodeMessge();
    int GetRoundId(){return round_id;}
private:
    void DecodePlayers(cJSON *players);
    void DecodeTeams(cJSON *teams);   
    void DecodePower(cJSON *coins);

public:
    cJSON* root;
    int round_id;
};

class LegEndMsg
{
public:
    LegEndMsg(cJSON* msg){root = msg;}
    ~LegEndMsg(){cJSON_Delete(root);}
    void DecodeMessge(){};
public:
    cJSON* root;
};

class GameEndMsg
{
public:
    GameEndMsg(cJSON* msg){root = msg;}
    ~GameEndMsg(){cJSON_Delete(root);}
    void DecodeMessge(){}
public:
    cJSON* root;
};

#endif



