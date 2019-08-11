#ifndef _CORE_H_
#define _CORE_H_
#include "message.h"
struct Point {
	int x;
	int y;
};

struct TeamInfo {
	int id;
	int players[4];
	char* force;
};

struct wormhole {
	char* name1;
	char* name2;
	Point p1;
	Point p2;
};

struct Tunnel {
	Point p;
	int direct;//иообвСср 1234
};

#endif // !_CORE_H_


