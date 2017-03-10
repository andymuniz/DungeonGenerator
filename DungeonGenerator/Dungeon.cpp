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
	this->nMinRoomEdgeLength = 7;
	this->nMinRoomEdgeWidth = 7;

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
	//SeperateCellRectangles();
	////FillSmallCellGaps();
	//DetermineRoomCells();

	/////Graphs
	//ConstructRoomDelaunayGraph();
	//ConstructRoomMST();
	//AddLoopsToMST();

	//DetermineCorridors();
}
