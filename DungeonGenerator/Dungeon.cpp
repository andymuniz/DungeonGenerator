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
	//GenerateCellCoordinates();
	{
		srand((unsigned int)time(NULL));
		//Create and push a room into our vector holding our Rooms.
		for (int i = 0; i < nCells; i++) {
			this->vRooms.push_back(new Room(rand() % 100, rand() % 100));
		}
	}

	//GenerateCellRectangles();
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
	////Steer the Cells away from each other to remove overlap.
	seperateCellRectangles();

	markAllTileMap();
	markTrueRooms();

	////FillSmallCellGaps();
	//DetermineRoomCells();

	/////Graphs
	//ConstructRoomDelaunayGraph();
	//ConstructRoomMST();
	//AddLoopsToMST();

	//DetermineCorridors();
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
	int point[2] = { 0,0 };
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

void Dungeon::markTrueRooms()
{
	for (auto& it : vRooms) {
		it->markIfTrueRoom(nMinRoomEdgeWidth, nMinRoomEdgeHeight);
	}
}
