#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>

using namespace std;

// hard coded set of edges
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


typedef struct vertex {
  int id;
  double p1, p2;
  int * edges;
} vert;

bool bfs (vector<int> , int , int [][11], int *);
vector<int> findEdges( int, int [][11], int *);

int main () {
  int visited[11] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  vector<int> path;
  /*  
  while (j < 10) {
    vector<int> testVector = findEdges(j, adjM, visited);
    vector<int>::iterator it = testVector.begin();
    while (it != testVector.end()) {
      //cout << *it << ", ";
      it++;
    }
    //cout << endl;
    i++;
    j++;
    testVector.clear();
  }/*
  cout << endl;
  for (int i = 0; i < 11; i++) {
    cout << "Parent: " << i+1 << " " << visited[i]+1 << endl;;
    }*/
  vector<int> start;
  start.push_back(0);
  int goal = 10;
  int steps = 0;

  if (bfs(start, goal, adjM4, visited)) {
    cout << "Path found " << endl;
    for (int k = 0; k < 11; k++) {
      cout << "Node: " << k << " Parent: " << visited[k] << endl;
      }
    int i = goal;
    while (i != 0) {
      path.push_back(visited[i]);
      i = visited[i];
    }
    vector<int>::iterator pathIt = path.end();
    cout << "------------" << endl;
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
  if (queue[0] == goal && queue.size() == 1) {
    cout << "Goal found" << endl;
    visited[goal] == queue[0];
    return true;
  }
  vector<int>::iterator it = queue.begin();
  vector<int> newQueue;
  while (it != queue.end()) {
    cout << *it << " current node" << endl;
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
  return bfs(newQueue, goal, matrix, visited);
  
}

// hard coded for size 11
vector<int> findEdges (int node, int matrix[][11], int visited[]) {
  //  int count = 0;
  vector<int> edgesRet;
  for (int i = 0; i < 11; i++) {
    if (matrix[node][i] == 1) {
      edgesRet.push_back(i);
      if (visited[i] == -1) {
	visited[i] = node;
	//cout << "Parent: " << i+1 << " " << visited[i]+1 << endl;
      }
      //count++;
    }
  }
  return edgesRet;
}
