#pragma once
#include <utility>
#include <math.h>
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
	Room(float x, float y, float z = -5);
	Room(std::pair<float, float> pos, float z = -5);
	Room(float x1, float y1, float width, float height);
	~Room();

	//Getters&Setters
	void setPosition(float x, float y) { vPosition[0] = x; vPosition[1] = y; };
	void setEdgeSizes(float height, float width) { nHeight = height; nWidth = width; setHalfEdgeSizes(); };
	void setHalfEdgeSizes() { nHalfWidth = nWidth / 2.f; nHalfHeight = nHeight / 2.f; };
	void setAABB();		//calculates and sets the AABB based on position and edge sizes
	void setConnected(bool Bool);
	const float* getPosition() const { return vPosition; };
	const float& getHeight() const { return nHeight; };
	const float& getWidth() const { return nWidth; };
	const AABB& getAABB() const { return sAABB; };
	const bool isTrueRoom()const { return bTrueRoom; };
	const bool isFillerCell()const { return bFillerCell; };
	const bool isCorridorCell()const { return bCorridorCell; };
	const bool isConnected()const { return bConnected; };

	//Get Boundaries, single x or y value for calculations.
	const float getTop(float padding = 0) const;
	const float getBottom(float padding = 0) const;
	const float getLeft(float padding = 0) const;
	const float getRight(float padding = 0) const;

	void shift(float dx, float dy);	//move room by a delta x and delta y amount
	void expand(float by);	//expand size of room evenly on each side by the amount specified.
	bool overlaps(Room& B, float padding = 0);	//True if this room (A) overlaps with Room B
	bool markIfTrueRoom(float minWidth, float minHeight);	//marks this object as a True Room if it fits the size requirements

	bool cmpf(float A, float B, float epsilon = 0.005f)
	{
		return ((fabs(A - B) < epsilon) && (A < B));
	};

private:
	float vPosition[3];	//x,y, and z but z not used
	float nHeight;
	float nWidth;
	float nHalfWidth = 0;
	float nHalfHeight = 0;
	float RoomSizeRatio;
	bool bTrueRoom;
	bool bFillerCell;
	bool bCorridorCell;
	bool bConnected;	//only rooms with true will be used in the end
	AABB sAABB;
};

