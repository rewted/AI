#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// hard coded set of edges
// first adjM is provided for the project
// rest are just various adjacency matrices
// for testing purposes.
int adjM[10][11] = { {0,1,1,1,0,0,0,0,0,0,0}, // 1
                     {0,0,1,0,0,0,0,0,0,0,0},
                     {0,0,0,1,1,0,0,0,0,0,0}, // 3
                     {0,0,0,0,1,1,1,0,0,0,0},
                     {0,0,0,0,0,0,1,1,0,0,0}, // 5
                     {0,0,0,0,0,0,0,1,0,0,0},
                     {0,0,0,0,0,0,0,0,1,1,0}, // 7
                     {0,0,0,0,0,0,0,0,1,1,1},
                     {0,0,0,0,0,0,0,0,0,0,1}, // 9
                     {0,0,0,0,0,0,0,0,0,0,1}};

int adjM2[10][11] = { {0,1,1,1,0,0,0,0,0,0,0}, // 1
                      {0,0,1,0,0,0,0,0,0,0,0},
                      {0,0,0,1,1,0,0,0,0,0,0}, // 3
                      {0,0,0,0,1,1,1,0,0,0,0},
                      {0,0,0,0,0,0,1,1,0,0,0}, // 5
                      {0,0,0,0,0,0,0,1,0,0,0},
                      {0,0,0,0,0,0,0,0,1,1,0}, // 7
                      {0,0,0,0,0,0,0,0,1,1,0},
                      {0,0,0,0,0,0,0,0,0,0,0}, // 9
                      {0,0,0,0,0,0,0,0,0,0,0}};

int adjM3[10][11] = { {0,1,1,1,0,0,0,0,0,0,1}, // 1
                      {0,0,1,0,0,0,0,0,0,0,0},
                      {0,0,0,1,1,0,0,0,0,0,0}, // 3
                      {0,0,0,0,1,1,1,0,0,0,0},
                      {0,0,0,0,0,0,1,1,0,0,0}, // 5
                      {0,0,0,0,0,0,0,1,0,0,0},
                      {0,0,0,0,0,0,0,0,1,1,0}, // 7
                      {0,0,0,0,0,0,0,0,1,1,0},
                      {0,0,0,0,0,0,0,0,0,0,0}, // 9
                      {0,0,0,0,0,0,0,0,0,0,0}};

int adjM4[10][11] = { {0,1,0,0,0,0,0,0,0,0,0}, // 1
                      {0,0,1,0,0,0,0,0,0,0,0},
                      {0,0,0,1,0,0,0,0,0,0,0}, // 3
                      {0,0,0,0,1,0,0,0,0,0,0},
                      {0,0,0,0,0,1,0,0,0,0,0}, // 5
                      {0,0,0,0,0,0,1,0,0,0,0},
                      {0,0,0,0,0,0,0,1,0,0,0}, // 7
                      {0,0,0,0,0,0,0,0,1,0,0},
                      {0,0,0,0,0,0,0,0,0,1,0}, // 9
                      {0,0,0,0,0,0,0,0,0,0,1}};

int adjM5[10][11] = { {0,0,0,0,0,0,0,0,0,1,0}, // 1
                      {0,0,0,0,0,0,0,0,0,0,1},
                      {0,1,0,0,0,0,0,0,0,0,0}, // 3
                      {0,0,1,0,0,0,0,0,0,0,0},
                      {0,0,0,1,0,0,0,0,0,0,0}, // 5
                      {0,0,0,0,1,0,0,0,0,0,0},
                      {0,0,0,0,0,1,0,0,0,0,0}, // 7
                      {0,0,0,0,0,0,1,0,0,0,0},
                      {0,0,0,0,0,0,0,1,0,0,0}, // 9
                      {0,0,0,0,0,0,0,0,1,0,0}};

int adjM6[10][11] = { {0,1,0,0,0,0,0,0,0,1,0}, // 1
                      {0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0}, // 3
                      {0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0}, // 5
                      {0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0}, // 7
                      {0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0}, // 9
                      {0,0,0,0,0,0,0,0,0,0,0}};


bool bfs (vector<int> , int , int [][11], int *);
vector<int> findEdges( int, int [][11], int *);

int main () {
  // array to keep track of nodes that have been visited
  // also keeps track of the parent of the node
  int visited[11] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  // keeps track of the path taken
  vector<int> path;
  // a queue using a vector, queue the starting point
  vector<int> start;
  start.push_back(0);
  // set the goal node
  int goal = 10;
  // keeps track of number of steps taken
  int steps = 0;
  // if bfs returns true a path has been found
  if (bfs(start, goal, adjM , visited)) {
    cout << "Path found " << endl;
    // displays the node and parent relationship
    for (int k = 0; k < 11; k++) {
      cout << "Node: " << k << " Parent: " << visited[k] << endl;
    }
    // iterates through the tree to generate the path taken
    int i = goal;
    while (i != start[0]) {
      path.push_back(visited[i]);
      i = visited[i];
    }
    // since the path is stored backwards iterate from the end
    // to the beginning and output the path along with steps.
    vector<int>::iterator pathIt = path.end();
    cout << "------------" << endl;
    cout << "Start: " << start[0] << " Goal: " << goal << endl;
    cout << "Path taken: " << endl;
    while (pathIt != path.begin()) {
      pathIt--;
      steps++;
      cout << *pathIt << " -> " ;
    }
    cout << goal << endl;
    cout << steps << " steps taken" << endl;
  }
  return 0;
}

bool bfs (vector<int> queue, int goal, int matrix[][11], int visited[]) {
  // check to see if start and goal are the same
  if (queue[0] == goal && queue.size() == 1) {
    cout << "Goal found, start and goal are the same" << endl;
    return true;
  }
  // initialize an iterator to cycle through the vector
  vector<int>::iterator it = queue.begin();
  // initialize a vector to act as the queue
  vector<int> newQueue;
  // goes through everything in the queue
  // generates the queue for the next level
  // dedup and remove elements from queue
  // that are currently visited
  while (it != queue.end()) {
    cout << *it << " current node" << endl;
    // finds all the edges of the node and stores them in edges
    vector<int> edges = findEdges(*it, matrix, visited);
    // goal is contained in one of the edges
    if (std::find(edges.begin(),edges.end(), goal) != edges.end()) {
      visited[goal] = *it;
      return true;
    }
    cout << "Number of edges: " << edges.size() << endl;
    for (int i = 0; i < (int)edges.size(); i++) {
      // only queues an edge if it is unique, this way duplicate edges will
      // not be visited
      if (std::find(newQueue.begin(), newQueue.end(), edges[i]) != newQueue.end()) {
	cout << "Duplicate element in queue " << endl;
      }
      else {
	newQueue.push_back(edges[i]);
	cout << "Push stuff onto queue " << edges[i] << endl;;
      }
    }
    // A node is queued that we are currently at
    if (std::find(newQueue.begin(), newQueue.end(), *it) != newQueue.end()) {
      newQueue.erase(std::find(newQueue.begin(), newQueue.end(), *it));
      cout << "Currently at node in queue, removing" << endl;
    }
    // increment the iterator and clear edges
    edges.clear();
    it++;
  }
  // this means we are at the bottom level and there are no remaining children
  // and that the goal hasn't been found
  if (newQueue.size() == 0) {
    cout << "Nothing left in queue, failed" << endl;
    return false;
  }
  cout << "------------------------" << endl;
  cout << "Going down one level" << endl;
  // recursively call bfs until a base condition is hit
  return bfs(newQueue, goal, matrix, visited);
}

// hard coded for size 11
vector<int> findEdges (int node, int matrix[][11], int visited[]) {
  //  int count = 0;
  vector<int> edgesRet;
  // checks the adj matrix for edges
  for (int i = 0; i < 11; i++) {
    // if edge is found push it to vector
    if (matrix[node][i] == 1) {
      edgesRet.push_back(i);
      // sets the parent if it hasn't been set already
      // will set in numerical order if there is a tie
      if (visited[i] == -1) {
	visited[i] = node;
      }
    }
  }
  // returns a vector with all edges of current node
  return edgesRet;
}
