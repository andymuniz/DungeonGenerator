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
	this->nMinRoomEdgeHeight = 9;
	this->nMinRoomEdgeWidth = 9;

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
	markTrueRooms();
	//expand true room sizes by 1
	for (auto room : vTrueRooms)
		room->expand(1);

	////Steer the Cells away from each other to remove overlap. Steer True Rooms away first, so we can space them away from each other a bit.
	{
		bool tooClose = false;
		do {
			seperateTrueRooms();
			seperateCellRectangles();
		} while (roomsTooClose(5));
	}

	markAllTileMap(vRooms);
	fillSmallCellGaps();

	/////Graph
	constructGraph(); //construct a relative neighborhood graph 

	//constructCorridors();
	{
		srand((unsigned int)time(NULL));
		int dx, dy, x, y;
		Room *a, *b;
		for (auto& outer : vTrueRooms) {
			std::vector<Room*>& Edges = graph[outer];

			for (auto& inner : Edges) {
				//We want L shaped corridors if the rooms aren't aligned on an axis at all. Straightish Lines otherwise.
				//Check the tileRoomMap for existing corridor tiles?
				if (outer->getPosition()[0] < inner->getPosition()[0]) {
					a = outer;
					b = inner;
				}
				else {
					a = inner;
					b = outer;
				}

				x = a->getPosition()[0];
				y = a->getPosition()[1];
				dx = b->getPosition()[0] - x;
				dy = b->getPosition()[1] - y;

				if (rand() % 2 == 1) {
					vRooms.push_back(new Room(x, y, dx + 1, 1));
					vCorridorRooms.push_back(vRooms.back());
					vRooms.push_back(new Room(x + dx, y, 1, dy));
					vCorridorRooms.push_back(vRooms.back());
				}
				else {
					vRooms.push_back(new Room(x, y + dy, dx + 1, 1));
					vCorridorRooms.push_back(vRooms.back());
					vRooms.push_back(new Room(x, y, 1, dy));
					vCorridorRooms.push_back(vRooms.back());
				}

			}
		}
	}

	//Expand corridor sizes
	for (auto room : vCorridorRooms)
		room->expand(1);

	removeUntouchedCells();

	//Remove disconnected cells from vRooms;
	for (std::vector<Room*>::iterator it = vRooms.begin(); it != vRooms.end();) {
		if (!(*it)->isConnected()) {
			delete *it;
			it = vRooms.erase(it);
		}
		else
			++it;
	}

	//clear tileMap and remap it all?
	//markAllTileMap(vCorridorRooms);
	tileRoomMap.clear();
	markAllTileMap(vRooms);

	//mark all True Room Walls

}

//Generates (val of nCells) Cells as Room objects and gives them a location.
void Dungeon::generateCellCoordinates()
{
	srand((unsigned int)time(NULL));
	//Create and push a room into our vector holding our Rooms.
	for (int i = 0; i < nCells; i++) {
		this->vRooms.push_back(new Room((float)(rand() % 100), (float)(rand() % 100)));
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

		it->setEdgeSizes((float)height, (float)width);
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

void Dungeon::seperateTrueRooms()
{
	int iterations = 0; //for debugging, remove later
	float padding = 6;
	Room* a;
	Room* b; // to hold any two rooms that are over lapping
	float dx, dxa, dxb, dy, dya, dyb; // holds delta values of the overlap
	bool touching = false; // a boolean flag to keep track of touching rooms
	do {
		if (debug_flag) {
			std::cout << "Starting " << "# of iteration: " << iterations << std::endl;
			iterations++;
		}
		touching = false;
		for (int i = 0; i < (int)vTrueRooms.size(); i++) {
			a = vTrueRooms[i];
			for (int j = i + 1; j < (int)vTrueRooms.size(); j++) { // for each pair of rooms (notice i+1)
				b = vTrueRooms[j];
				if (a->overlaps(*b, padding)) { // if the two rooms touch (allowed to overlap by 1)
					touching = true; // update the touching flag so the loop iterates again
									 // find the two smallest deltas required to stop the overlap
					dx = std::min(a->getRight(padding) - b->getLeft(padding), a->getLeft(padding) - b->getRight(padding));
					dy = std::min(a->getBottom(padding) - b->getTop(padding), a->getTop(padding) - b->getBottom(padding));
					// only keep the smalled delta
					if (abs(dx) < abs(dy)) dy = 0;
					else dx = 0;
					// create a delta for each rectangle as half the whole delta.
					dxa = ceil(-dx / 2.f);
					dxb = ceil(dx + dxa);
					// same for y
					dya = ceil(-dy / 2.f);
					dyb = ceil(dy + dya);
					// shift both rectangles
					a->shift(dxa, dya);
					b->shift(dxb, dyb);
				}
			}
		}
	} while (touching == true); // loop until no rectangles are touching
	if (debug_flag) {
		std::cout << "Out of Loop." << std::endl << "# of iterations: " << iterations << std::endl;
	}
}

//Steers Cells away from each other, to remove overlap !!TODO: Check the rounding for bugs!!!!! IMPORTANT
void Dungeon::seperateCellRectangles()
{
	int iterations = 0; //for debugging, remove later
	float padding = 0;
	Room* a;
	Room* b; // to hold any two rooms that are over lapping
	float dx, dxa, dxb, dy, dya, dyb; // holds delta values of the overlap
	bool touching = false; // a boolean flag to keep track of touching rooms
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
				if (a->overlaps(*b, padding)) { // if the two rooms touch (allowed to overlap by 1)
					touching = true; // update the touching flag so the loop iterates again
					// find the two smallest deltas required to stop the overlap
					dx = std::min(a->getRight(padding) - b->getLeft(padding), a->getLeft(padding) - b->getRight(padding));
					dy = std::min(a->getBottom(padding) - b->getTop(padding), a->getTop(padding) - b->getBottom(padding));
					// only keep the smalled delta
					if (abs(dx) < abs(dy)) dy = 0;
					else dx = 0;
					// create a delta for each rectangle as half the whole delta.
					dxa = ceil(-dx / 2.f);
					dxb = ceil(dx + dxa);
					// same for y
					dya = ceil(-dy / 2.f);
					dyb = ceil(dy + dya);
					// shift both rectangles
					a->shift(dxa, dya);
					b->shift(dxb, dyb);
				}
			}
		}
	} while (touching == true); // loop until no rectangles are touching
	if (debug_flag) {
		std::cout << "Out of Loop." << std::endl << "# of iterations: " << iterations << std::endl;
	}
}

//Returns true if rooms edges are closer than padding * 2 distance apart
bool Dungeon::roomsTooClose(float padding)
{
	Room* a;
	Room* b; // to hold any two rooms that are over lapping
	for (int i = 0; i < (int)vTrueRooms.size(); i++) {
		a = vTrueRooms[i];
		for (int j = i + 1; j < (int)vTrueRooms.size(); j++) { // for each pair of rooms (notice i+1)
			b = vTrueRooms[j];
			if (a->overlaps(*b, padding)) { // if the two rooms touch
				return true;
			}
		}
	}
	return false;
}

//Marks every (integer)coodinate residing in a Room in the tileMap
//Stores the coordinate of the "center" of each 1x1 "pixel" in every room.
void Dungeon::markTileMap(Room& a)
{
	for (float x = a.getLeft(); x < a.getRight(); x++) {
		for (float y = a.getBottom(); y < a.getTop(); y++) {
			std::pair<float, float> Pos = std::make_pair(x + 0.5f, y + 0.5f);
			if (tileRoomMap.find(Pos) == tileRoomMap.end()) {//doesn't exist
				tileRoomMap[Pos] = &a;	//add a small offset to get the center and not the corner of pixel
			}
			else {
				if (tileRoomMap[Pos]->isTrueRoom()) {
					continue;
				}
				if (tileRoomMap[Pos]->isCorridorCell()) {
					continue;
				}
				tileRoomMap[Pos] = &a;	//add a small offset to get the center and not the corner of pixel
			}

		}
	}
}

//Marks all the occupied tiles in the tileMap
void Dungeon::markAllTileMap(std::vector<Room*>& vectorOfRooms)
{
	for (auto& it : vectorOfRooms) {
		markTileMap(*it);
	}
}

//Marks room as a True Room if their width and height meet criteria and inserts reference into vTrueRooms
void Dungeon::markTrueRooms()
{
	for (auto& it : vRooms) {
		if (it->markIfTrueRoom((float)nMinRoomEdgeWidth, (float)nMinRoomEdgeHeight)) {
			vTrueRooms.push_back(it);
		}
	}
}

//checks each existing coordinate for any 1x1 gaps around it, then creates a 1x1 room at that location
void Dungeon::fillSmallCellGaps()
{
	std::pair<float, float> point;	//original point we will center on
	for (auto& it : this->tileRoomMap) {
		point = it.first;
		std::pair<float, float> point2, point3;	//will hold a point to check if a cell exists two cells away
		//Check for gap above
		point2 = std::make_pair(point.first, point.second + 1);
		point3 = std::make_pair(point2.first, point2.second + 1);
		if (this->tileRoomMap.find(point2) == this->tileRoomMap.end()) {
			//point doesn't exist so check for a point 1 further to veryify a gap.
			if (this->tileRoomMap.find(point3) != this->tileRoomMap.end()) {
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
		if (this->tileRoomMap.find(point2) == this->tileRoomMap.end()) {
			//point doesn't exist so check for a point 1 further to veryify a gap.
			if (this->tileRoomMap.find(point3) != this->tileRoomMap.end()) {
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
		if (this->tileRoomMap.find(point2) == this->tileRoomMap.end()) {
			//point doesn't exist so check for a point 1 further to veryify a gap.
			if (this->tileRoomMap.find(point3) != this->tileRoomMap.end()) {
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
		if (this->tileRoomMap.find(point2) == this->tileRoomMap.end()) {
			//point doesn't exist so check for a point 1 further to veryify a gap.
			if (this->tileRoomMap.find(point3) != this->tileRoomMap.end()) {
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

//constructs a relative neighborhood graph
void Dungeon::constructGraph()
{
	Room *a, *b, *c;
	double abDist, acDist, bcDist;
	bool skip;
	for (int i = 0; i < (int)vTrueRooms.size(); i++) {
		a = vTrueRooms[i];
		for (int j = i + 1; j < (int)vTrueRooms.size(); j++) { // for each pair of rooms
			skip = false;
			b = vTrueRooms[j];
			// get the sqrd distance between a and b
			abDist = pow(a->getPosition()[0] - b->getPosition()[0], 2) + pow(a->getPosition()[1] - b->getPosition()[1], 2);
			for (int k = 0; k < (int)vTrueRooms.size(); k++) { // loop through all other rooms
				if (k == i || k == j) // that are not a or b
					continue;
				c = vTrueRooms[k];
				// get the other two applicable distances
				acDist = pow(a->getPosition()[0] - c->getPosition()[0], 2) + pow(a->getPosition()[1] - c->getPosition()[1], 2);
				bcDist = pow(b->getPosition()[0] - c->getPosition()[0], 2) + pow(b->getPosition()[1] - c->getPosition()[1], 2);
				// if the distance between a and c or b and c are smaller than a, the pairing of
				// a and b is not a graph edge
				if (acDist < abDist && bcDist < abDist)
					skip = true;
				if (skip) // so we break the loop and go to the next a and b paring
					break;
			}
			if (!skip) { // if this a and b pairing was never skipped, it should be an edge
				if (graph.find(a) == graph.end())
					graph[a] = std::vector<Room*>();
				graph[a].push_back(b);
			}
		}
	}

}

void Dungeon::removeUntouchedCells()
{
	for (auto& corridor : vCorridorRooms) {
		for (auto& room : vRooms) {
			if (room->isConnected()) continue;

			if (corridor->overlaps(*room, .5f)) {
				room->setConnected(true);
			}
		}
	}
}
