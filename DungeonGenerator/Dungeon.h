#pragma once

#include <vector>
#include <map>
#include "Room.h"
#include <random>
#include <time.h>
#include <math.h>
#include <iostream>
#include <algorithm>

extern bool debug_flag;

class Dungeon
{
public:
	Dungeon();
	~Dungeon();
	void GenerateDungeon();

	const int getNumCells()const { return nCells; };
	const std::vector<Room*>& getRooms()const { return vRooms; };
	void seperateCellRectangles();

private:
	/*Initial properties and constraints of our desired dungeon required for its generation*/
	int nCells;
	int vOrigin[2];	//Not really sure i'll even need this. Or might rename to vCenter.
	int nDomainMax;
	int nDomainMin;
	int nRangeMax;
	int nRangeMin;
	int nMaxRoomEdgeSize;
	float fMinRoomSizeRatio;
	float fMaxRoomSizeRatio;
	int nMinRoomEdgeLength;
	int nMinRoomEdgeWidth;

	int nDungeonSize;//Length/Width(in 'tiles') of the Dungeon. Area will be a square for simplicity.

	/*tileMap used to indicate which 'tiles' of the Dungeon contain a tile. Use bool or int. Int may be useful to indicate the type of tile. 0 for empty. */
	std::map<int[2], bool> tileMap;
	std::vector<Room*> vRooms;	//keeps track of the rooms.
};

