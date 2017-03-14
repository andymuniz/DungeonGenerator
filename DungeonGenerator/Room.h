#pragma once
#include <utility>

extern bool debug_flag;

//Axis-Aligned Bounding Box
typedef struct AABB {
	int LL[2];	//Lower-left corner
	int LR[2];	//Lower-right corner
	int UR[2];	//Upper-right corner
	int UL[2];	//Upper-left corner
}AABB;

class Room
{
public:
	Room(int x, int y, int z = -5);
	Room(std::pair<int, int> pos, int z = -5);
	~Room();

	//Getters&Setters
	void setPosition(int x, int y) { vPosition[0] = x; vPosition[1] = y; };
	void setEdgeSizes(int height, int width) { nHeight = height; nWidth = width; setHalfEdgeSizes(); };
	void setHalfEdgeSizes() { nHalfWidth = nWidth / 2; nHalfHeight = nHeight / 2; };
	void setAABB();		//calculates and sets the AABB based on position and edge sizes
	const int* getPosition() const { return vPosition; };
	const int& getHeight() const { return nHeight; };
	const int& getWidth() const { return nWidth; };
	const AABB& getAABB() const { return sAABB; };
	bool isTrueRoom() { return bTrueRoom; };
	bool isFillerCell() { return bFillerCell; };

	//Get Boundaries, single x or y value for calculations.
	const int getTop() const;
	const int getBottom() const;
	const int getLeft() const;
	const int getRight() const;

	void shift(int dx, int dy);	//move room by a delta x and delta y amount
	bool overlaps(const Room& B, int padding = 0) const;	//True if this room (A) overlaps with Room B
	void markIfTrueRoom(int minWidth, int minHeight);	//marks this object as a True Room if it fits the size requirements

private:
	int vPosition[3];	//x,y, and z but z not used
	int nHeight;
	int nWidth;
	float nHalfWidth = 0;
	float nHalfHeight = 0;
	float RoomSizeRatio;
	bool bTrueRoom;
	bool bFillerCell;
	AABB sAABB;
};

