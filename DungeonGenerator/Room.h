#pragma once
#include <utility>
#include <iostream>

extern bool debug_flag;

//Axis-Aligned Bounding Box
typedef struct AABB {
	float LL[2];	//Lower-left corner
	float LR[2];	//Lower-right corner
	float UR[2];	//Upper-right corner
	float UL[2];	//Upper-left corner
}AABB;

class Room
{
public:
	Room(int x, int y, int z = -5);
	Room(std::pair<float, float> pos, int z = -5);
	Room(int x1, int y1, int width, int height);
	~Room();

	//Getters&Setters
	void setPosition(int x, int y) { vPosition[0] = x; vPosition[1] = y; };
	void setEdgeSizes(int height, int width) { nHeight = height; nWidth = width; setHalfEdgeSizes(); };
	void setHalfEdgeSizes() { nHalfWidth = nWidth / 2.f; nHalfHeight = nHeight / 2.f; };
	void setAABB();		//calculates and sets the AABB based on position and edge sizes
	const float* getPosition() const { return vPosition; };
	const int& getHeight() const { return nHeight; };
	const int& getWidth() const { return nWidth; };
	const AABB& getAABB() const { return sAABB; };
	const bool isTrueRoom()const { return bTrueRoom; };
	const bool isFillerCell()const { return bFillerCell; };
	const bool isConnected()const { return bConnected; };

	//Get Boundaries, single x or y value for calculations.
	const float getTop(int padding = 0) const;
	const float getBottom(int padding = 0) const;
	const float getLeft(int padding = 0) const;
	const float getRight(int padding = 0) const;

	void shift(int dx, int dy);	//move room by a delta x and delta y amount
	bool overlaps(Room& B, int padding = 0);	//True if this room (A) overlaps with Room B
	bool markIfTrueRoom(int minWidth, int minHeight);	//marks this object as a True Room if it fits the size requirements

private:
	float vPosition[3];	//x,y, and z but z not used
	int nHeight;
	int nWidth;
	float nHalfWidth = 0;
	float nHalfHeight = 0;
	float RoomSizeRatio;
	bool bTrueRoom;
	bool bFillerCell;
	bool bCorridorCell;
	bool bConnected;	//only rooms with true will be used in the end
	AABB sAABB;
};

