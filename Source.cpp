#include <iostream>
#include <vector>
#include <queue>
#include <sstream>

#include <SDL.h>
#include <SDL_image.h>

using namespace std;

SDL_Window* window;
SDL_Renderer* renderer;

bool isRunning = true;

struct Location {
	float x;
	float y;
};

struct Node {
	string label = "";
	Location location;
	float g = INFINITY;	// Distance from start node
	float h = NULL;		// (heuristic) Estimated distance from current node to end node
	float f = NULL;		// Total cost of node
	bool visited = false;

	Node* parent = nullptr;
	vector<Node*> children;
};

const int NODE_SIZE = 20;

vector<Node*> openList;
vector<Node*> closedList;
vector<Node*> nodeMap;

// Create nodes
Node* n1 = new Node;
Node* n2 = new Node;
Node* n3 = new Node;
Node* n4 = new Node;
Node* n5 = new Node;
Node* n6 = new Node;
Node* n7 = new Node;
Node* n8 = new Node;
Node* n9 = new Node;

Node* activeNode = nullptr;

void PopulateNodes() {
	n1->label = "N1";
	n1->location.x = 40.f;
	n1->location.y = 40.f;
	n1->children.push_back(n2);
	n1->children.push_back(n3);

	n2->label = "N2";
	n2->location.x = 80.f;
	n2->location.y = 80.f;
	n2->children.push_back(n1);
	n2->children.push_back(n3);
	n2->children.push_back(n4);

	n3->label = "N3";
	n3->location.x = 130.f;
	n3->location.y = 50.f;
	n3->children.push_back(n1);
	n3->children.push_back(n2);
	n3->children.push_back(n4);
	n3->children.push_back(n5);
	n3->children.push_back(n6);

	n4->label = "N4";
	n4->location.x = 120.f;
	n4->location.y = 105.f;
	n4->children.push_back(n2);
	n4->children.push_back(n3);
	n4->children.push_back(n5);
	n4->children.push_back(n7);

	n5->label = "N5";
	n5->location.x = 200.f;
	n5->location.y = 85.f;
	n5->children.push_back(n3);
	n5->children.push_back(n4);
	n5->children.push_back(n6);
	n5->children.push_back(n7);
	n5->children.push_back(n8);

	n6->label = "N6";
	n6->location.x = 250.f;
	n6->location.y = 60.f;
	n6->children.push_back(n3);
	n6->children.push_back(n5);
	n6->children.push_back(n8);

	n7->label = "N7";
	n7->location.x = 260.f;
	n7->location.y = 150.f;
	n7->children.push_back(n4);
	n7->children.push_back(n5);
	n7->children.push_back(n8);
	n7->children.push_back(n9);

	n8->label = "N8";
	n8->location.x = 300.f;
	n8->location.y = 100.f;
	n8->children.push_back(n5);
	n8->children.push_back(n6);
	n8->children.push_back(n7);
	n8->children.push_back(n9);

	n9->label = "N9";
	n9->location.x = 400.f;
	n9->location.y = 150.f;
	n9->children.push_back(n7);
	n9->children.push_back(n8);

	nodeMap.push_back(n1);
	nodeMap.push_back(n2);
	nodeMap.push_back(n3);
	nodeMap.push_back(n4);
	nodeMap.push_back(n5);
	nodeMap.push_back(n6);
	nodeMap.push_back(n7);
	nodeMap.push_back(n8);
	nodeMap.push_back(n9);
}

float DistanceBetween(Location startLoc, Location endLoc) {
	return sqrt(pow(endLoc.x - startLoc.x, 2) + pow(endLoc.y - startLoc.y, 2) * 1.f);
}

void CalculateF(Node* currentNode) {
	currentNode->f = currentNode->g + currentNode->h;
}

void CalculateG(Node* currentNode) {
	if (currentNode->parent == nullptr) {
		currentNode->g = 0.f;
		return;
	}

	float distanceFromParent = DistanceBetween(currentNode->parent->location, currentNode->location);
	currentNode->g = distanceFromParent + currentNode->parent->g;
}

void CalculateH(Node* currentNode, Node* endNode) {
	currentNode->h = DistanceBetween(currentNode->location, endNode->location);
}

bool CompareFValues(Node* nodeOne, Node* nodeTwo) {
	return (nodeOne->f < nodeTwo->f);
}

void ResetVistedOnNodes() {
	for (int i = 0; i < nodeMap.size(); i++) {
		nodeMap[i]->visited = false;
	}
}

Node* MouseCollisionNode() {
	int x, y;
	SDL_GetMouseState(&x, &y);

	// Check collision with existing node
	for (int i = 0; i < nodeMap.size(); i++) {
		if (x >= nodeMap[i]->location.x - NODE_SIZE / 2 &&
			x <= nodeMap[i]->location.x + NODE_SIZE / 2 &&
			y >= nodeMap[i]->location.y - NODE_SIZE / 2 &&
			y <= nodeMap[i]->location.y + NODE_SIZE / 2) {
			return nodeMap[i];
		}
	}

	return nullptr;
}

// https://www.youtube.com/watch?v=eSOJ3ARN5FM&t=110s
void FindPath(Node* startNode, Node* endNode) {
	cout << "Finding path..." << endl;
	
	vector<Node*> openList;		// open list -- pending, they have been visited but not expanded
	vector<Node*> closedList;	// closed list -- have been visited and expanded
	Node* currentNode = startNode;

	CalculateH(currentNode, endNode);										// Set heuristic
	CalculateG(currentNode);												// Calc G value (distance from start to here)
	CalculateF(currentNode);
	openList.push_back(startNode);											// Add to open list

	while (currentNode != endNode) {
		cout << "CURRENT NODE: " << currentNode->label << endl;

		for (int i = 0; i < currentNode->children.size(); i++) {
			// Check if child is on closed list already
			if (currentNode->children[i]->visited) {
				continue;
			}			
			
			cout << "--CURRENT CHILD NODE: " << currentNode->children[i]->label << endl;

			currentNode->children[i]->parent = currentNode;
			CalculateH(currentNode->children[i], endNode);
			CalculateG(currentNode->children[i]);
			CalculateF(currentNode->children[i]);
			openList.push_back(currentNode->children[i]);
		}

		// After adding children, close the parent (remove from open and add to closed)
		// Note: Do we need to add a check after adding the children to see if one of them is the end node?

		currentNode->visited = true;
		closedList.push_back(currentNode);
		openList.erase(openList.begin());

		// Choose next node by selecting the one with the lowest F value
		sort(openList.begin(), openList.end(), CompareFValues);
		currentNode = openList[0];
	}

	// Just for output
	endNode->visited = true;
	Node* prevNode = endNode->parent;
	cout << "FINAL PATH: " << endNode->label;

	while (prevNode != nullptr) {
		cout << " -> " << prevNode->label;
		prevNode = prevNode->parent;
	}

	cout << endl << endl;
}

int main(int argc, char* argv[]) {

	SDL_Init(SDL_INIT_EVERYTHING);

	window = SDL_CreateWindow("Pathfinding", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	PopulateNodes();
	//FindPath(n1, n9);

	while (isRunning) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
				isRunning = false;
			}

			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_r) {
					ResetVistedOnNodes();
					FindPath(n1, nodeMap[nodeMap.size() - 1]);
				}

				if (event.key.keysym.sym == SDLK_s) {
					Node* tempStartNode = MouseCollisionNode();
					if (tempStartNode != nullptr) {

						// Set start node..
					}
				}

				if (event.key.keysym.sym == SDLK_e) {
					// Set end node
				}
			}

			if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					int x, y;
					SDL_GetMouseState(&x, &y);

					Node* newNode = new Node;
					ostringstream oss;
					oss << "N" << nodeMap.size() + 1;
					newNode->label = oss.str();
					newNode->location.x = x;
					newNode->location.y = y;
					nodeMap.push_back(newNode);
					activeNode = newNode;
				}

				if (event.button.button == SDL_BUTTON_RIGHT) {
					int x, y;
					SDL_GetMouseState(&x, &y);

					if (activeNode != nullptr) {
						// Check collision with existing node
						for (int i = 0; i < nodeMap.size(); i++) {
							if (x >= nodeMap[i]->location.x - NODE_SIZE / 2 &&
								x <= nodeMap[i]->location.x + NODE_SIZE / 2 &&
								y >= nodeMap[i]->location.y - NODE_SIZE / 2 &&
								y <= nodeMap[i]->location.y + NODE_SIZE / 2) {
								// Push active node and collision node onto each other's children 
								activeNode->children.push_back(nodeMap[i]);
								nodeMap[i]->children.push_back(activeNode);
							}
						}
					}
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
		SDL_RenderClear(renderer);

		SDL_Rect temp;

		for (int i = 0; i < nodeMap.size(); i++) {
			SDL_SetRenderDrawColor(renderer, 20, 20, 200, 255);

			// Draw lines for children
			for (int j = 0; j < nodeMap[i]->children.size(); j++) {
				SDL_RenderDrawLine(
					renderer,
					nodeMap[i]->location.x,
					nodeMap[i]->location.y,
					nodeMap[i]->children[j]->location.x,
					nodeMap[i]->children[j]->location.y
				);
			}

			if (nodeMap[i]->visited) {
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			}

			temp.x = (int)nodeMap[i]->location.x - (NODE_SIZE / 2);
			temp.y = (int)nodeMap[i]->location.y - (NODE_SIZE / 2);
			temp.w = NODE_SIZE;
			temp.h = NODE_SIZE;

			SDL_RenderFillRect(renderer, &temp);
		}

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}