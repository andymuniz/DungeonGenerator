#include "Dungeon.h"

static std::map<int, int> heightDist;
static std::map<int, int> widthDist;

Dungeon::Dungeon()
{
	this->nCells = 150;
	this->vOrigin[0] = vOrigin[1] = 0;
	this->nDomainMin = nRangeMin = 0;
	this->nDomainMax = nDomainMin = 100;
	this->fMinRoomSizeRatio = 0.5f;
	this->fMinRoomSizeRatio = 1.5f;
	this->nMaxRoomEdgeSize = 15;
	//Change these constraints to get more or less TRUE rooms!
	this->nMinRoomEdgeHeight = 8;
	this->nMinRoomEdgeWidth = 8;

	this->nDungeonSize = 100;
}

Dungeon::~Dungeon()
{
}

void Dungeon::GenerateDungeon()
{
	///----Construct and lay down rooms...
	//-----Determine Cell Properties
	generateCellCoordinates();
	generateCellRectangles();

	////Steer the Cells away from each other to remove overlap.
	seperateCellRectangles();

	markAllTileMap();
	markTrueRooms();

	//fillSmallCellGaps();

	/////Graphs
	//ConstructRoomDelaunayGraph();
	//ConstructRoomMST();
	//AddLoopsToMST();

	//DetermineCorridors();
}

//Generates (val of nCells) Cells as Room objects and gives them a location.
void Dungeon::generateCellCoordinates()
{
	srand((unsigned int)time(NULL));
	//Create and push a room into our vector holding our Rooms.
	for (int i = 0; i < nCells; i++) {
		this->vRooms.push_back(new Room(rand() % 100, rand() % 100));
	}
}

//Randomly Generates Rectangle data for each cell
void Dungeon::generateCellRectangles()
{
	std::default_random_engine generator;
	std::normal_distribution<double> distribution(4.0f, 3.0f);
	//Generate width and length of each Room. Use Parker-Normal Distribution. We want the edges to be even whole numbers.
	for (auto& it : vRooms) {
		int height = ((int)abs(distribution(generator)) % this->nMaxRoomEdgeSize) + 1;
		if (height % 2 != 0)
			height++;
		int width = rand() % (int)(height * 1.5 - height * 0.5) + (int)(height * 0.5) + 1;
		if (width % 2 != 0)
			width++;

		it->setEdgeSizes(height, width);
		// Debugging
		if (debug_flag) {
			std::cout << "X: " << it->getPosition()[0] << "      Y: " << it->getPosition()[1] //<< std::endl;
				<< "      Z: " << it->getPosition()[2] << std::endl;
			std::cout << "Height: " << it->getHeight() << "      Width: " << it->getWidth() << std::endl;
			heightDist[height]++;
			widthDist[width]++;
		}
	}
	//Debugging
	if (debug_flag) {
		std::cout << "-Height distribution-" << std::endl;
		for (auto& it : heightDist) {
			std::cout << it.first << ": "; for (int i = 0; i < it.second; i++) { std::cout << "*"; } std::cout << std::endl;
		}
		std::cout << "-Width distribution-" << std::endl;
		for (auto& it : widthDist) {
			std::cout << it.first << ": "; for (int i = 0; i < it.second; i++) { std::cout << "*"; } std::cout << std::endl;
		}
	}

	//Generate "Rectangles" (AABB data) for each room
	for (auto& it : vRooms) {
		it->setAABB();
	}
}

//Steers Cells away from each other, to remove overlap !!TODO: Check the rounding for bugs!!!!! IMPORTANT
void Dungeon::seperateCellRectangles()
{
	int iterations = 0; //for debugging, remove later
	int padding = 0;
	Room* a;
	Room* b; // to hold any two rooms that are over lapping
	int dx, dxa, dxb, dy, dya, dyb; // holds delta values of the overlap
	bool touching; // a boolean flag to keep track of touching rooms
	do {
		if (debug_flag) {
			std::cout << "Starting " << "# of iteration: " << iterations << std::endl;
			iterations++;
		}
		touching = false;
		for (int i = 0; i < (int)vRooms.size(); i++) {
			a = vRooms[i];
			for (int j = i + 1; j < (int)vRooms.size(); j++) { // for each pair of rooms (notice i+1)
				b = vRooms[j];
				if ((*a).overlaps(*b, padding)) { // if the two rooms touch (allowed to overlap by 1)
					touching = true; // update the touching flag so the loop iterates again
					// find the two smallest deltas required to stop the overlap
					dx = std::min(abs((*a).getRight() - (*b).getLeft() + padding), abs((*a).getLeft() - (*b).getRight() - padding));
					dy = std::min(abs((*a).getBottom() - (*b).getTop() + padding), abs((*a).getTop() - (*b).getBottom() - padding));
					// only keep the smalled delta
					if (abs(dx) < abs(dy)) dy = 0;
					else dx = 0;
					// create a delta for each rectangle as half the whole delta.
					dxa = -dx / 2;
					dxb = dx + dxa;
					// same for y
					dya = -dy / 2;
					dyb = dy + dya;
					// shift both rectangles
					(*a).shift(dxa, dya);
					(*b).shift(dxb, dyb);
				}
			}
		}
	} while (touching); // loop until no rectangles are touching
	if (debug_flag) {
		std::cout << "Out of Loop." << std::endl << "# of iterations: " << iterations << std::endl;
	}
}

//Marks every (integer)coodinate residing in a Room in the tileMap
void Dungeon::markTileMap(Room& a)
{
	for (int x = a.getLeft(); x <= a.getRight(); x++) {
		for (int y = a.getBottom(); y <= a.getTop(); y++) {
			tileMap[std::make_pair(x, y)] = true;
		}
	}
}

//Marks all the occupied tiles in the tileMap
void Dungeon::markAllTileMap()
{
	for (auto& it : vRooms) {
		markTileMap(*it);
	}
}

//Marks room as a True Room if their width and height meet criteria
void Dungeon::markTrueRooms()
{
	for (auto& it : vRooms) {
		it->markIfTrueRoom(nMinRoomEdgeWidth, nMinRoomEdgeHeight);
	}
}

/*
Another algorithm:
Get a set of 4 points to define a square and traverse through the grid. If each point is in the tilemap but there is no
*/

//checks each existing coordinate for any 1x1 gaps around it, then creates a 2x2 room at those spots...not perfect
//!!Checked 2 cells away for existing, but forgot to check 1 cell away for empty...
//So..check 1 cell away for existing, if empty check 2 cells away for existing
void Dungeon::fillSmallCellGaps()
{
	std::pair<int, int> point;	//original point we will center on
	for (auto& it : this->tileMap) {
		point = it.first;
		std::pair<int, int> point2, point3;	//will hold a point to check if a cell exists two cells away
		//Check for gap above
		point2 = std::make_pair(point.first, point.second + 1);
		point3 = std::make_pair(point2.first, point2.second + 1);
		if (this->tileMap.find(point2) == this->tileMap.end()) {
			//point doesn't exist so check for a point 1 further to veryify a gap.
			if (this->tileMap.find(point3) != this->tileMap.end()) {
				this->vRooms.push_back(new Room(point2));	//create a 1x1 cell\room at that gap
				markTileMap(*vRooms.back());
			}
		}
		else {
			//point exists, so no gap exists
		}
		//Check for gap below
		point2 = std::make_pair(point.first, point.second - 1);
		point3 = std::make_pair(point2.first, point2.second - 1);
		if (this->tileMap.find(point2) == this->tileMap.end()) {
			//point doesn't exist so check for a point 1 further to veryify a gap.
			if (this->tileMap.find(point3) != this->tileMap.end()) {
				this->vRooms.push_back(new Room(point2));	//create a 1x1 cell\room at that gap
				markTileMap(*vRooms.back());
			}
		}
		else {
			//point exists, so no gap exists
		}
		//Check for gap to left
		point2 = std::make_pair(point.first - 1, point.second);
		point3 = std::make_pair(point2.first - 1, point2.second);
		if (this->tileMap.find(point2) == this->tileMap.end()) {
			//point doesn't exist so check for a point 1 further to veryify a gap.
			if (this->tileMap.find(point3) != this->tileMap.end()) {
				this->vRooms.push_back(new Room(point2));	//create a 1x1 cell\room at that gap
				markTileMap(*vRooms.back());
			}
		}
		else {
			//point exists, so no gap exists
		}
		//Check for gap to right
		point2 = std::make_pair(point.first + 1, point.second);
		point3 = std::make_pair(point2.first + 1, point2.second);
		if (this->tileMap.find(point2) == this->tileMap.end()) {
			//point doesn't exist so check for a point 1 further to veryify a gap.
			if (this->tileMap.find(point3) != this->tileMap.end()) {
				this->vRooms.push_back(new Room(point2));	//create a 1x1 cell\room at that gap
				markTileMap(*vRooms.back());
			}
		}
		else {
			//point exists, so no gap exists
		}

		/*Reverse process creates rounded cavelike/islandlike structure*/
		//point = it.first;
		//std::pair<int, int> point2;	//will hold a point to check if a cell exists two cells away
		////Check for gap above
		//point2 = std::make_pair(point.first, point.second + 2);
		//if (this->tileMap.find(point2) == this->tileMap.end()) {
		//	//point doesn't exist so no gap. Do nothing.
		//}
		//else {
		//	//point exists, so gap exists
		//	point2.second--;
		//	this->vRooms.push_back(new Room(point2));	//create a 1x1 cell\room at that gap
		//}
		////Check for gap below
		//point2 = std::make_pair(point.first, point.second - 2);
		//if (this->tileMap.find(point2) == this->tileMap.end()) {
		//	//point doesn't exist so no gap. Do nothing.
		//}
		//else {
		//	//point exists, so gap exists
		//	point2.second++;;
		//	this->vRooms.push_back(new Room(point2));	//create a 1x1 cell\room at that gap
		//}
		////Check for gap to left
		//point2 = std::make_pair(point.first - 2, point.second);
		//if (this->tileMap.find(point2) == this->tileMap.end()) {
		//	//point doesn't exist so no gap. Do nothing.
		//}
		//else {
		//	//point exists, so gap exists
		//	point2.first++;;
		//	this->vRooms.push_back(new Room(point2));	//create a 1x1 cell\room at that gap
		//}
		////Check for gap to right
		//point2 = std::make_pair(point.first + 2, point.second);
		//if (this->tileMap.find(point2) == this->tileMap.end()) {
		//	//point doesn't exist so no gap. Do nothing.
		//}
		//else {
		//	//point exists, so gap exists
		//	point2.first--;;
		//	this->vRooms.push_back(new Room(point2));	//create a 1x1 cell\room at that gap
		//}
	}
}
