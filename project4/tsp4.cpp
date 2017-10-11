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
  list<int>::iterator it;
  list<int>::iterator it2;
  for (int i = 0; i < 100; i++) {
    if (random (mutationRate) == 0) {
      while (nodeA == nodeB) {
	nodeA = random (100);
	nodeB = random (100);
      }
      it = a->citizens[i].tour.begin();
      it2 = a->citizens[i].tour.begin();
      advance (it, nodeA);
      advance (it2, nodeB);
      a->citizens[i].tour.insert(it, *it2);
      a->citizens[i].tour.insert(it2, *it);
      it = a->citizens[i].tour.erase(it);
      it2 = a->citizens[i].tour.erase(it2);
      a->citizens[i].fitness = routeDistance (a->citizens[i].tour, points, totalCount);
      //cout << "Mutate: " << i << "\tSwapping " << nodeA << " and " << nodeB << endl;
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

void pmxCrossover (Population *a) {
  // kill 30 least fit in the population
  for (int i = 0; i < 40; i++) {
    a->citizens.pop_back();
    //cout << "Pop citizen" << endl;
  }
  int count = 0;
  int parentA = 0;
  int parentB = 0;
  int crossoverSite;
  list<int> childA;
  list<int> childB;
  vector<int> aCross;
  vector<int> bCross;
  list<int>::iterator itParentA1;
  list<int>::iterator itParentA2;
  list<int>::iterator itParentB1;
  list<int>::iterator itParentB2;
  list<int>::iterator itChild1;
  list<int>::iterator itChild2;
  list<int>::iterator ptr;
  double percentA = 0;
  double percentB = 0;
  double chance = 0;
  // need to replace 30 children
  // two created at a time
  while (count != 30) {
    // select two different parents
    while (parentA == parentB) {
      parentA = random (60);
      parentB = random (60);
    }
    // percent gradent dependent on fitness
    // for chance to mate between parentA and parent B
    if (parentA < 15) {
      percentA = 0.80;
    }
    if (parentA >= 15 && parentA < 30) {
      percentA = 0.70;
    }
    if (parentA >= 30 && parentA < 45) {
      percentA = 0.60;
    }
    if (parentA >= 45 && parentA < 60) {
      percentA = 0.50;
    }
    if (parentB < 15) {
      percentA = 0.80;
    }
    if (parentB >= 15 && parentB < 30) {
      percentB = 0.70;
    }
    if (parentB >= 30 && parentB < 45) {
      percentB = 0.60;
    }
    if (parentB >= 45 && parentB < 60) {
      percentB = 0.50;
    }
    chance = percentA * percentB;
    //cout << "Parent A: " << parentA << " Parent B: " << parentB << " Chance: " << chance << endl;
    // mating successful initiate PMX Crossover
    if (random((1/chance)) == 0) {
      crossoverSite = random (96);
      cout << "----------------" << endl;
      cout << "Do cross over: " << count << " at site: " << crossoverSite << endl;
      itParentA1 = a->citizens[parentA].tour.begin();
      itParentA2 = a->citizens[parentA].tour.begin();
      itParentB1 = a->citizens[parentB].tour.begin();
      itParentB2 = a->citizens[parentB].tour.begin();
      advance (itParentA1, crossoverSite);
      advance (itParentA2, crossoverSite + 5);
      advance (itParentB1, crossoverSite);
      advance (itParentB2, crossoverSite + 5);
      // getting values for deduping
      ptr = itParentA1;
      //cout << "[ ";
      while (ptr != itParentA2) {
	//cout << *ptr << " ";
	aCross.push_back(*ptr);
	ptr++;
      }
      // getting values for deduping
      ptr = itParentB1;
      //cout <<"]----[ ";
      while (ptr != itParentB2) {
	bCross.push_back(*ptr);
	//cout << *ptr << " ";
	ptr++;
      }
      // deduping crossing values
      cout << "Deduping" <<endl;
      vector<int>::iterator itCrossA = aCross.begin();
      vector<int>::iterator itCrossB = bCross.begin();
      while (itCrossA != aCross.end()) {
	itCrossB = bCross.begin();
	while (itCrossB != bCross.end()) {
	  if (*itCrossA == *itCrossB) {
	    cout << "Erasing: " << *itCrossA << endl;
	    itCrossA = aCross.erase(itCrossA);
	    itCrossB = bCross.erase(itCrossB);
	    if (itCrossB == bCross.end()) {
	      cout << "B: Deleting an end" << endl;
	      break;
	    }
	  }
	  itCrossB++;
	}
	if (itCrossA == aCross.end()) {
	  cout << "A: deleting an end" << endl;
	  break;
	}
	itCrossA++;
      }
      itCrossA = aCross.begin();
      itCrossB = bCross.begin();
      cout << "[ ";
      while (itCrossA != aCross.end()) {
	cout << *itCrossA << " ";
	itCrossA++;
      }
      cout << "] [ ";
      while (itCrossB != bCross.end()) {
	cout << *itCrossB << " ";
	itCrossB++;
      }
      cout << "]" << endl;
      //cout << "]" << endl;
      // Starting to create childA
      ptr = a->citizens[parentA].tour.begin();
      while (ptr != a->citizens[parentA].tour.end()) {
	childA.push_back(*ptr);
	ptr++;
	if (ptr == itParentA1) {
	  ptr = itParentB1;
	}
	if (ptr == itParentB2) {
	  ptr = itParentA2;
	}
      }
      // Starting to create childB
      ptr = a->citizens[parentB].tour.begin();
      while (ptr != a->citizens[parentB].tour.end()) {
	childB.push_back(*ptr);
	ptr++;
	if (ptr == itParentB1) {
	  ptr = itParentA1;
	}
	if (ptr == itParentA2) {
	  ptr = itParentB2;
	}
      }
      // removes duplicate nodes in the child
      ptr = childA.begin();
      itChild1 = childA.begin();
      itChild2 = childA.begin();
      advance (itChild1, crossoverSite);
      advance (itChild2, crossoverSite + 5);
      itCrossA = aCross.begin();
      itCrossB = bCross.begin();
      while (ptr != childA.end()) {
	if (ptr == itChild1) {
	  ptr = itChild2;
	}
	itCrossB = bCross.begin();
	while (itCrossB != bCross.end()) {
	  if (*ptr == *itCrossB) {
	    cout << "Equal " << *ptr << endl;
	  }
	  itCrossB++;
	}
	ptr++;
      }
      
      /*
      for (list<int>::iterator a = childA.begin(); a != childA.end(); a++) {
	cout << *a << " ";
      }
      cout << endl << endl;
      for (list<int>::iterator a = childB.begin(); a != childB.end(); a++) {
	cout << *a << " ";
      }
      */
      cout << endl << endl;
      count++;
    }
    aCross.clear();
    bCross.clear();
    childA.clear();
    childB.clear();
    chance = 0;
    parentA = 0;
    parentB = 0;
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
  pmxCrossover (&world);
  //printWorld (&world);
  return 0;
}
