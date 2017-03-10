#include "Room.h"

Room::Room(int x, int y, int z)
{
	vPosition[2] = z;
	setPosition(x, y);
}

Room::~Room()
{
}

/*
Since we're using a grid system, all edges are even for room, and all positions are integers, we are working
with even whole numbers, making this really easy to do. IDK if this will need to be changed in the future,
but this is an easy way to save hassle.
*/
//This AABB is centered at an "origin" making it simpler to draw in opengl and then translate.
//The true origin should be the Position
void Room::setAABB()
{
	nHalfWidth = nWidth / 2;
	nHalfHeight = nHeight / 2;
	this->sAABB.LL[0] = 0 - nHalfWidth;
	this->sAABB.LL[1] = 0 - nHalfHeight;
	this->sAABB.LR[0] = 0 + nHalfWidth;
	this->sAABB.LR[1] = 0 - nHalfHeight;
	this->sAABB.UR[0] = 0 + nHalfWidth;
	this->sAABB.UR[1] = 0 + nHalfHeight;
	this->sAABB.UL[0] = 0 - nHalfWidth;
	this->sAABB.UL[1] = 0 + nHalfHeight;
}

const int  Room::getTop() const
{
	return vPosition[1] + nHalfHeight;
}

const int Room::getBottom() const
{
	return vPosition[1] - nHalfHeight;
}

const int Room::getLeft() const
{
	return vPosition[0] - nHalfWidth;
}

const int Room::getRight() const
{
	return vPosition[0] + nHalfWidth;
}

bool Room::overlaps(const Room& b, int padding) const
{
	auto a = this;
	return !(b.getLeft() - padding > a->getRight() ||
		b.getRight() < a->getLeft() - padding ||
		b.getTop() < a->getBottom() - padding ||
		b.getBottom() - padding > a->getTop());
}
