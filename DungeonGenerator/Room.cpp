#include "Room.h"

Room::Room(float x, float y, float z)
{
	this->vPosition[2] = z;
	this->setPosition(x, y);
	this->bTrueRoom = bFillerCell = bConnected = bCorridorCell = false;
}

//This constructor is only used when filling gaps in Dungeon Generation..keep in mind
Room::Room(std::pair<float, float> pos, float z)
{
	this->vPosition[2] = -5;
	//this->setPosition(pos.first, pos.second);
	vPosition[0] = pos.first;
	vPosition[1] = pos.second;
	this->bTrueRoom = false;
	this->setEdgeSizes(1, 1);

	this->setAABB();
	bTrueRoom = bConnected = bCorridorCell = false;
	bFillerCell = true;
	//std::cout << "Filler Position: (" << vPosition[0] << ", " << vPosition[1] << ")" << std::endl;
	//std::cout << "LL: (" << sAABB.LL[0] << ", " << sAABB.LL[1] << ")" << std::endl;
	//std::cout << "LR: (" << sAABB.LR[0] << ", " << sAABB.LR[1] << ")" << std::endl;
	//std::cout << "UR: (" << sAABB.UR[0] << ", " << sAABB.UR[1] << ")" << std::endl;
	//std::cout << "UL: (" << sAABB.UL[0] << ", " << sAABB.UL[1] << ")" << std::endl << std::endl;
}

//Constructor used in creating corridors
Room::Room(float x1, float y1, float width, float height) {

	float _left = x1;
	float _right = x1;
	if (width < 0)
		_left += width;
	else
		_right += width;

	float _bottom = y1;
	float _top = y1;
	if (height < 0)
		_bottom += height;
	else
		_top += height;

	this->vPosition[2] = -5;

	vPosition[0] = (_left + _right) / 2;
	vPosition[1] = (_top + _bottom) / 2;
	this->bTrueRoom = false;
	this->setEdgeSizes(_top - _bottom, _right - _left);

	this->setAABB();
	bTrueRoom = false;
	bCorridorCell = bConnected = true;
	bFillerCell = false;
}

Room::~Room()
{
}

void Room::shift(float dx, float dy)
{
	this->vPosition[0] += dx; this->vPosition[1] += dy;
	this->setAABB();
}

void Room::expand(float by)
{
	this->setEdgeSizes(nHeight + (by * 2), nWidth + (by * 2));
	this->setAABB();
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
	nHalfWidth = nWidth / 2.f;
	nHalfHeight = nHeight / 2.f;
	this->sAABB.LL[0] = 0 - nHalfWidth;
	this->sAABB.LL[1] = 0 - nHalfHeight;
	this->sAABB.LR[0] = 0 + nHalfWidth;
	this->sAABB.LR[1] = 0 - nHalfHeight;
	this->sAABB.UR[0] = 0 + nHalfWidth;
	this->sAABB.UR[1] = 0 + nHalfHeight;
	this->sAABB.UL[0] = 0 - nHalfWidth;
	this->sAABB.UL[1] = 0 + nHalfHeight;
}

void Room::setConnected(bool Bool)
{
	this->bConnected = Bool;
}

const float  Room::getTop(float padding) const
{
	return vPosition[1] + nHalfHeight + padding;
}

const float Room::getBottom(float padding) const
{
	return vPosition[1] - nHalfHeight - padding;
}

const float Room::getLeft(float padding) const
{
	return vPosition[0] - nHalfWidth - padding;
}

const float Room::getRight(float padding) const
{
	return vPosition[0] + nHalfWidth + padding;
}

bool Room::overlaps(Room& b, float padding)
{
	auto a = this;
	return !((b.getLeft(padding)) >= a->getRight(padding) ||
		b.getRight(padding) <= (a->getLeft(padding)) ||
		b.getTop(padding) <= (a->getBottom(padding)) ||
		(b.getBottom(padding)) >= a->getTop(padding));

	//return
	//	(
	//		this->cmpf(a->getRight(padding), b.getLeft(padding)) ||
	//		this->cmpf(b.getRight(padding), a->getLeft(padding)) ||
	//		this->cmpf(b.getTop(padding), a->getBottom(padding)) ||
	//		this->cmpf(a->getTop(), b.getBottom(padding))
	//		);
}

bool Room::markIfTrueRoom(float minWidth, float minHeight)
{
	if (nWidth >= minWidth && nHeight >= minHeight) {
		this->bConnected = true;
		return (this->bTrueRoom = true);
		//return true;
	}
	return false;
}
