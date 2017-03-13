#pragma once

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
	~Room();

	void shift(int dx, int dy);

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

	//Get Boundaries, single x or y value for calculations.
	const int getTop() const;
	const int getBottom() const;
	const int getLeft() const;
	const int getRight() const;

	bool overlaps(const Room& B, int padding = 0) const;	//True if this room (A) overlaps with Room B
	void markIfTrueRoom(int minWidth, int minHeight);	//marks this object as a True Room if it fits the size requirements


private:
	int vPosition[3];	//x,y, and z but z not used
	int nHeight;
	int nWidth;
	int nHalfWidth = 0;
	int nHalfHeight = 0;
	float RoomSizeRatio;
	bool bTrueRoom;
	AABB sAABB;
};

