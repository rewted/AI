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

int adjM5[10][11] = { {0,0,0,0,0,1,0,0,0,1,0}, // 1
                      {0,0,0,0,0,0,0,0,0,0,1},
                      {0,1,0,0,0,0,0,0,0,0,0}, // 3
                      {0,0,1,0,0,0,0,0,0,0,0},
                      {0,0,0,1,0,0,0,0,0,0,0}, // 5
                      {0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,1,0,0,0,0,0,0,0}, // 7
                      {0,0,0,0,0,0,1,0,0,0,0},
                      {0,0,0,0,0,0,0,1,0,0,0}, // 9
                      {0,0,0,0,0,0,0,0,1,0,0}};

int adjM6[10][11] = { {0,1,1,1,1,1,1,1,1,1,0}, // 1
                      {0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0}, // 3
                      {0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0}, // 5
                      {0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0}, // 7
                      {0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0}, // 9
                      {0,0,0,0,0,0,0,0,0,0,0}};

int adjM7[10][11] = { {0,1,0,0,0,0,0,0,0,1,0}, // 1
                      {0,0,0,0,0,0,0,0,0,0,1},
                      {0,0,0,0,0,0,0,0,0,0,0}, // 3
                      {0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,1,0,0,0,0,0,0,0}, // 5
                      {0,0,0,0,1,0,0,0,0,0,0},
                      {0,0,0,0,0,1,0,0,0,0,0}, // 7
                      {0,0,0,0,0,0,1,0,0,0,0},
                      {0,0,0,0,0,0,0,1,0,0,0}, // 9
                      {0,0,0,0,0,0,0,0,1,0,0}};

bool dfs (vector<int> , int, int [][11], int *, int, int []);
vector<int> findEdges( int, int [][11], int *);

int main () {
  // array to keep track of nodes that have been visited
  // also keeps track of the parent of the node
  int visited[11] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  int nodeLevel[11] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  // keeps track of the path taken
  vector<int> path;
  // add the starting point
  vector<int> start;
  start.push_back(0);
  // set the goal node
  int goal = 10;
  // keeps track of number of steps taken
  int level = 1;
  int steps = 0;
  // dfs 
  if(dfs(start, goal, adjM7, visited, level, nodeLevel)) {
    // displays the node and parent relationship
    for (int i = 0; i < 11; i++) {
      cout << "Node: " << i << " Parent: " << visited[i] << endl;
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
    cout << endl << "------------------" << endl;
    cout << "Path taken: " << endl;
    while (pathIt != path.begin()) {
      pathIt--;
      steps++;
      cout << *pathIt << " -> " ;
    }
    cout << goal << endl;
    cout << steps << " steps taken" << endl;
  }
  else {
    cout << "Path not found" << endl;
  }
  return 0;
}

// depth first traversal of a graph
bool dfs (vector<int> queue, int goal, int matrix[][11], int visited[], int level, int nodeLevel[]) {
  bool pathFound;
  // base case, if we find the goal
  if (queue[0] == goal) {
    cout << "---------------------" << endl;
    cout << "A path has been found " << endl;
    cout << "Moving back up a level" << endl;
    cout << "---------------------" << endl << endl;
    // assign nodeLevel the current level depth that we are at
    // if it is unassigned (-1) or if we're at a smaller depth
    if (nodeLevel[0] == -1 || nodeLevel[0] > level) {
      nodeLevel[goal] = queue[0];
    }
    pathFound = true;
    return pathFound;
  }
  // initializing some variables
  // children keeps track of the child edges of the parent node
  // next node is what gets loaded into the recursive function
  // it iterates through the child nodes once recursive functions complete
  vector<int> children = findEdges((int)queue[0], matrix, visited);
  vector<int> nextNode;
  vector<int>::iterator it = children.begin();
  // no children for this parent node
  if (children.size() == 0) {
    cout << "---------------" << endl;
    cout << "Dead end node: " << queue[0]  << endl;
    cout << "Moving back up a level" << endl;
    cout << "---------------" << endl;
    pathFound = false;
    return pathFound;
  }
  // loop to keep track of levels and parent child relationship
  for (int i = 0; i < (int)children.size(); i++) {
    // set the node level for the children if not set yet (-1) or found at a lower level
    if (nodeLevel[children[i]] == -1 || level < nodeLevel[children[i]]) {
      nodeLevel[children[i]] = level;
      // set the children node
      visited[children[i]] = queue[0];
      cout << "Assigning child: " << children[i] << " level: " << level << endl;  
    }
  }
  cout << "Current Parent: " << queue[0] << endl;
  // loop to iterate through the children and recursively go deeper
  while (it != children.end()) {
    // if the level we are at is lower than previously visited nodes
    if (level <= nodeLevel[*it] || nodeLevel[*it] == -1) {
      nextNode.push_back(*it);
      cout << "Moving deeper to node: " << nextNode[0] << endl;
      if (dfs(nextNode, goal, matrix, visited, level+1, nodeLevel)) {
	pathFound = true;
      }
      nextNode.pop_back();
    }
    // iterate the pointer that looks at child nodes
    it++;
  }
  return pathFound;
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
    }
  }
  // returns a vector with all edges of current node
  return edgesRet;
}
