#pragma once
#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <string.h>
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

struct Point {
	int x;
	int y;
};

struct TeamInfo {
	int id;
	int players[4];
	char* force;
};

struct WormholePair {
	int name1;
	int name2;
	Point point1;
	Point point2;
};

struct Tunnel {
	Point point;
	int direct;//上下左右 20 21 22 23
};

struct Power {
	Point point;
	int value;
};

/*************************
0：地图可行区域
1 - 5：分数奖励
10 - 17：player信息
8：障碍物
虫洞id的ascii码：表示虫洞对
20 - 23表示滑梯的上下左右

map能装25x25个数，用w和h对map做约束
*/
struct GameMap {
	int h;
	int w;
	//      Y   X
	int map[25][25] = {0};
};


enum DIRECT
{
	MOVE_UP = 0,
	MOVE_DOWN = 1,
	MOVE_LEFT = 2,
	MOVE_RIGHT = 3,
	NO_MOVE = 4,
	UP = 20,
	DOWN = 21,
	LEFT = 22,
	RIGHT = 23
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
    void GetMyTeamInfo(int& myTeamId,int myPlayerId[4]); //更新了mTeamInfo信息
    void GetMeteor(int& myTeamId);                       //更新了mMeteors信息
    void GetWormhole(int& myTeamId);                     //更新了mWormholes信息
    void GetTunnel(int& myTeamId);                       //更新了mTunnels信息
    void GetCloud(int& myTeamId);                        //协议里应该是没有这个的
	void GenerateMap(int h, int w);                      //当所有信息都生成后，用这个生成map信息
private:

public:
	char* DIRECTLOG[4] = { "UP", "DOWN", "LEFT", "RIGHT" };

    cJSON* root;
	int h;
	int w;
	TeamInfo mTeamInfo;
	std::vector<Point> mMeteors;
	std::vector<WormholePair> mWormholePairs;
	std::vector<Tunnel> mTunnels;
	GameMap mGameMap;
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

/// 

#endif



