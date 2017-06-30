#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <random>
#include <time.h>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <utility>
#include "Room.h"

extern bool debug_flag;

class Dungeon
{
public:
	Dungeon();
	~Dungeon();
	void GenerateDungeon();

	const int getNumCells()const { return nCells; };
	const std::vector<Room*>& getRooms()const { return vRooms; };
	const std::map<Room*, std::vector<Room*>>& getGraph()const { return graph; };
	const std::map < std::pair<float, float >, Room* > & getTileMap()const { return tileRoomMap; };

	void generateCellCoordinates();
	void generateCellRectangles();
	void seperateTrueRooms();
	void seperateCellRectangles();
	bool roomsTooClose(float padding);
	void markTileMap(Room& a);
	void markAllTileMap(std::vector<Room*>&);
	void markTrueRooms();
	void fillSmallCellGaps();
	void constructGraph();	//construct relative neighbor graph between true rooms
	void removeUntouchedCells();

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
	int nMinRoomEdgeHeight;
	int nMinRoomEdgeWidth;

	int nDungeonSize;//Length/Width(in 'tiles') of the Dungeon. Area will be a square for simplicity.

	/*tileMap used to indicate which 'tiles' of the Dungeon contain a tile. Use bool or int. Int may be useful to indicate the type of tile. 0 for empty. */
	/*Maybe just remove tileMap...lol*/
	//std::map < std::pair < float, float >, bool > tileMap;	//maps if a point contains part of a cell of some kind.
	std::map < std::pair < float, float >, Room* > tileRoomMap;	//maps out every drawn "pixel" to it's respective Room
	std::vector<Room*> vRooms;	//keeps track of the rooms.
	std::vector<Room*> vTrueRooms;	//keeps track of true rooms.
	std::vector<Room*> vCorridorRooms;	//keeps track of rooms made to create corridors.
	std::map<Room*, std::vector<Room*>> graph;	//our graph used to form the dungeon paths
};

