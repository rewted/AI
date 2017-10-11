#include <iostream>
#include <list>
#include <algorithm>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>


using namespace std;

typedef struct Point {
  int id;
  double x, y;
} Point;

typedef struct Chromo {
  int rank;
  list<int> tour;
  double fitness;
} Chromo;

typedef struct Population {
  int generation;
  double averageFitness, bestFitness, worstFitness;
  vector<Chromo> citizens;
} Population;

// Global Variables
Point points[100];
int totalCount = 100;

void insert (Point *point, int i, double x, double y) {
  point->id = i;
  point->x = x;
  point->y = y;
}

int readFile (char * fileName, Point *points) {
  string buffer;
  string tmp;
  int count = 0;
  int id;
  double x, y;
  ifstream tspFile (fileName);
  if (!tspFile.is_open()) {
    cout << "File failed to load" << endl;
  }

  for (int i = 0; i < 7; i++) {
    getline (tspFile, buffer);
  }

  while (getline(tspFile, buffer)) {
    istringstream iss (buffer);
    iss >> tmp;
    id = stoi (tmp);
    iss >> tmp;
    x = stod (tmp);
    iss >> tmp;
    y = stod (tmp);
    iss >> tmp;
    if (count == id -1) {
      insert ((points+count), id, x, y);
    }
    count++;
  }
  tspFile.close();
  return count;
}

double distanceCalc (Point firstPoint, Point secondPoint) {
  return sqrt(pow((secondPoint.x - firstPoint.x),2) + pow((secondPoint.y - firstPoint.y),2));
}

double routeDistance (list<int> route, Point *points, int numPoints) {
  double distance = 0;
  list<int>::iterator it = route.begin();
  list<int>::iterator it2 = route.begin();
  advance(it2, numPoints-1);
  distance = distanceCalc (points[*it-1], points[*it2-1]);

  it2 = route.begin();
  it2++;
  while (it2 != route.end()) {
    distance = distance + distanceCalc (points[*it-1], points[*it2-1]);
    it++;
    it2++;
  }
  return distance;
}

int random (int i) { return std::rand()%i; }

void generatePopulation (Population *a) {
  vector<int> b;
  list<int> path;
  Chromo tmpChromo;
  for (int i = 0; i < 100; i++) {
    b.push_back(i);
  }
  for (int i = 0; i < 100; i++) {
    random_shuffle (b.begin(), b.end());
    for (vector<int>::iterator it = b.begin(); it != b.end(); it++) {
      path.push_back(*it);
      //      cout << *it << " ";
    }
    //cout << endl << "-------------------" << endl;
    tmpChromo.tour = path;
    path.clear();
    tmpChromo.rank = i;
    tmpChromo.fitness = routeDistance (tmpChromo.tour, points, totalCount);
    a->citizens.push_back(tmpChromo);
  }
}

void generationSort (Population *a) {
  vector<Chromo> b;
  vector<Chromo> c = a->citizens;
  vector<Chromo>::iterator it;
  vector<Chromo>::iterator tmpIt;
  int index;
  int i = 0;
  double lowest;
  while (c.size() != 0) {
    lowest = 999999999999;
    it = c.begin();
    i = 0;
    while (it != c.end()) {
      if (c[i].fitness < lowest) {
	lowest = c[i].fitness;
	index = i;
	tmpIt = it;
      }
      it++;
      i++;
    }
    b.push_back(c[index]);
    c.erase(tmpIt);
  }
  a->citizens = b;
  for (int j = 0; j < 100; j++) {
    a->citizens[j].rank = i;
  }
}

void generationStats (Population *a) {
  double average = 0;
  for (int i = 0; i < 100; i++) {
    average = average + a->citizens[i].fitness;
  }
  average = average / 100;
  a->averageFitness = average;
  a->bestFitness = a->citizens[0].fitness;
  a->worstFitness = a->citizens[99].fitness;
}

void generationMutate (Population *a, int mutationRate) {
  int nodeA = 0;
  int nodeB = 0;
  int tmpInt;
  list<int>::iterator it;
  list<int>::iterator it2;
  //list<int>::iterator it3;
  for (int i = 0; i < 100; i++) {
    //cout << mutationRate << endl;
    if (random (mutationRate) == 0) {
      while (nodeA == nodeB) {
	nodeA = random (100);
	nodeB = random (100);
      }
      it = a->citizens[i].tour.begin();
      it2 = a->citizens[i].tour.begin();
      advance (it, nodeA);
      advance (it2, nodeB);
      //cout << *it << " " << *it2;
      /*
      cout << endl;
      it3 = a->citizens[i].tour.begin();
      cout << "prior to swap" << endl;
      while (it3 != a->citizens[i].tour.end()) {
	cout << *it3 << " ";
	it3++;
      }
      cout << endl;
      */      
      a->citizens[i].tour.insert(it, *it2);
      a->citizens[i].tour.insert(it2, *it);
      it = a->citizens[i].tour.erase(it);
      it2 = a->citizens[i].tour.erase(it2);
      a->citizens[i].fitness = routeDistance (a->citizens[i].tour, points, totalCount);
      /*
      it3 = a->citizens[i].tour.begin();
      cout << endl;
      cout << "post swap" << endl;
      while (it3 != a->citizens[i].tour.end()) {
	cout << *it3 << " ";
	it3++;
      }
      cout << endl;
      */
      cout << "Mutate: " << i << "\tSwapping " << nodeA << " and " << nodeB << endl;
    }
    nodeA = 0;
    nodeB = 0;
  }
}

void printWorld (Population *a) {
  cout << "World stats \tBest: " << a->bestFitness << "\tAverage: " << a->averageFitness << "\tWorst: " << a->worstFitness << endl; 
  for (int i = 0; i < 100; i++) {
    cout << "--[RANK: " << i << " FITNESS: " << a->citizens[i].fitness << "]--" << endl;
    for (list<int>::iterator it = a->citizens[i].tour.begin(); it != a->citizens[i].tour.end(); it++) {
      cout << *it << " ";
    }
    cout << endl << endl;
  }
}

int main (int argc, char *argv[]) {
  if (argc < 2) {
    cout << "Include file name when executing." << endl;
    return 0;
  }
  srand (unsigned (time (0)));
  int mutationRate = 5;
  totalCount = readFile (argv[1], points);
  Population world;
  generatePopulation (&world);
  generationMutate (&world, mutationRate);
  generationSort (&world);
  generationStats (&world);
  printWorld (&world);
  

  /*
  srand (unsigned (time (0)));
  vector<vector<int>> b (100, vector<int>(0));;
  list<int> a;
  list<int>::iterator it = a.begin();
  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 100; j++) {
      b[j].push_back(i);
    }
    a.push_back(i);
    //    cout << *it << endl;
  }
  for (int i = 0; i < 100; i++) {
    random_shuffle (b[i].begin(), b[i].end());
    for (vector<int>::iterator it2 = b[i].begin(); it2 != b[i].end(); it2++) {
      cout << *it2 << " ";
    }
    cout << endl;
  }
  */
  return 0;
}
