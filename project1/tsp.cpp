#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
#include <cmath>

using namespace std;

// data structure to hold point information
typedef struct Point_t {
  int id;
  double p1, p2;
} Point_t;

// used to insert data into the point structure
void insert(Point_t *point, int i, double pt1, double pt2) {
  point->id = i;
  point->p1 = pt1;
  point->p2 = pt2;
}

// reads the specified file and loads values into the data structure
int readFile(char *fileName, Point_t *points) {
  string buffer;
  string tmp;
  int elements = 0;
  int id;
  double pt1, pt2;
  ifstream tspFile(fileName);
  if (!tspFile.is_open()) {
    cout << "File failed to load" << endl;
    return -1;
  }

  // cycles through first 7 lines to get to the values
  for (int i = 0; i < 7; i++) {
    getline(tspFile, buffer);
  }

  // used to split strings and typecast to appropriate types
  while (getline(tspFile, buffer)) {
    istringstream iss(buffer);
    iss >> tmp;
    id = stoi(tmp);
    iss >> tmp;
    pt1 = stod(tmp);
    iss >> tmp;
    pt2 = stod(tmp);
    iss >> tmp;

    // error check to make sure right values are being inserted
    if (elements == id-1) {
      cout << id << " " << pt1 << " " << pt2 << endl;
      insert((points+elements), id, pt1, pt2);
    }
    elements++;
    
  }
  tspFile.close();
  return elements;
}

// calculates the distance between two points
double distanceCalc(Point_t firstPoint, Point_t secondPoint) {
  double distance;
  double dx = secondPoint.p1 - firstPoint.p1;
  double dy = secondPoint.p2 - firstPoint.p2;
  distance = sqrt((dx*dx)+(dy*dy));
  return distance;
}

// prints values of the Point_t data structure
// primarily used for debugging
void printValues(Point_t *points, int total) {
  for (int i = 0; i<total; i++) {
    cout << (points+i)->id << " " << points[i].p1 << " " << points[i].p2 << endl;
  }
}

// given a route as an int array, an array of points, and number of points
// calculate the distance traveled traversing specified route
double routeDistance(int *route, Point_t *points, int numPoints){
  double distance = 0;
  int pos1, pos2;
  distance = distanceCalc(points[route[0]], points[route[numPoints-1]]);
  for (int i = 0; i < numPoints-1; i++) {
    pos1 = route[i];
    pos2 = route[i+1];
    distance = distance + distanceCalc(points[pos1], points[pos2]);
  }
  pos1 = route[numPoints-1];
  pos2 = route[0];
  return distance;
}
  
int main (int argc, char *argv[]) {
  // no file specified during execution
  if ( argc < 2 ) {
    cout << "Include filename when executing." << endl;
    return 0;
  }
  // create an array for the points
  Point_t points[20];

  // number of points to visit
  int totalElements = readFile(argv[1], points);

  // holds specified values to compare against
  double distance, shortest, longest;

  // represents route we are checking
  int *i = new int[totalElements];

  // stores shortest route
  int *shortRoute = new int[totalElements];

  // stores longest route
  int *longestRoute = new int[totalElements];

  // initializes to the first route
  for (int j = 0; j < totalElements; j++) {
    i[j] = j;
    shortRoute[j] = j;
    longestRoute[j] = j;
  }
  cout << "--------------" << endl;

  // keeps track of the number of permutations
  int counter = 1;

  // since only one route has been considered so far
  // it is both the shortest and longest
  shortest = routeDistance(i, points, totalElements);
  longest = shortest;

  // used to iterate to the next permutation
  while (next_permutation(i,i+totalElements)) {

    // increments the permutation counter
    counter++;
    distance = routeDistance(i, points, totalElements);

    // if true current route is now the shortest
    if (distance < shortest) {
      for (int j = 0; j < totalElements; j++) {
	shortRoute[j] = i[j];
	}
      shortest = distance;
    }

    // if true current route is now the longest
    if (distance > longest) {
      for (int j = 0; j < totalElements; j++) {
	longestRoute[j] = i[j];
      }
      longest = distance;
    }

    // used to output the route and distance calculated
    for (int a = 0; a < totalElements; a++) {
      cout << i[a] << ' ';
    }
    cout << distance << endl;
  }

  // output the results found
  cout << "--------------" << endl;
  cout << "Shortest Route: ";
  for (int j = 0; j < totalElements; j++) {
    cout << shortRoute[j] << " "; 
  }
  cout << "Distance: " << shortest << endl;;
  cout << "Longest Route: ";
  for (int j = 0; j < totalElements; j++) {
    cout << longestRoute[j] << " ";
  }
  cout << "DIstance: " << longest << endl;
  cout << "Routes considered: " << counter << endl;

  // deallocate
  delete [] i;
  delete [] shortRoute;
  delete [] longestRoute;
  return 0;
}
