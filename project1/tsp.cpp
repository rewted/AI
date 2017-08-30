#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>
#include <algorithm>
#include <array>
#include <sstream>
#include <cmath>

using namespace std;

typedef struct Point_t {
  int id;
  double p1, p2;
} Point_t;

void insert(Point_t *point, int i, double pt1, double pt2) {
  point->id = i;
  point->p1 = pt1;
  point->p2 = pt2;
  /*  cout << point->id << " " << i << endl;
  cout << point->p1 << " " << pt1 << endl;
  cout << point->p2 << " " << pt2 << endl;*/
}

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
  for (int i = 0; i < 7; i++) {
    getline(tspFile, buffer);
  }
  while (getline(tspFile, buffer)) {
    istringstream iss(buffer);
    iss >> tmp;
    id = stoi(tmp);
    iss >> tmp;
    pt1 = stod(tmp);
    iss >> tmp;
    pt2 = stod(tmp);
    iss >> tmp;
    if (elements == id-1) {
      cout << id << " " << pt1 << " " << pt2 << endl;
      insert((points+elements), id, pt1, pt2);
    }
    elements++;
    
  }
  tspFile.close();
  return elements;
}

double distanceCalc(Point_t firstPoint, Point_t secondPoint) {
  double distance;
  double dx = secondPoint.p1 - firstPoint.p1;
  double dy = secondPoint.p2 - firstPoint.p2;
  distance = sqrt((dx*dx)+(dy*dy));
  return distance;
}

void printValues(Point_t *points, int total) {
  for (int i = 0; i<total; i++) {
    cout << (points+i)->id << " " << points[i].p1 << " " << points[i].p2 << endl;
  }
}

double routeDistance(int *route, Point_t *points, int numPoints){
  double distance = 0;
  int pos1, pos2;
  distance = distanceCalc(points[route[0]], points[route[numPoints-1]]);
  for (int i = 0; i < numPoints-1; i++) {
    pos1 = route[i];
    pos2 = route[i+1];
    distance = distance + distanceCalc(points[pos1], points[pos2]);
    //cout << i << ". From: " << pos1 << " to " << pos2 << " " << distanceCalc(points[pos1], points[pos2]) << endl;
  }
  pos1 = route[numPoints-1];
  pos2 = route[0];
  //  cout << numPoints-1 << ". From: " << route[numPoints-1] << " to " << route[0] << " " << distanceCalc(points[pos1], points[pos2]) << endl;;
  //  cout << distance << endl;;
  return distance;
}
  
/*
bool uniqueChecker(int perm[], int n) {
  int *count = new int[n];
  for (int i = 0; i < n+1; i++) {
    count[i] = 0;
    //    cout << perm[i] << endl;
  }
  for (int i = 0; i < n; i++) {
    count[perm[i]]++;
    cout << perm[i] << " count: " << count[perm[i]] << endl;
    if (count[perm[i]] > 1) {
      delete count;
      return false;
    }
  }
  delete count;
  return true;
}
*/


int main (int argc, char *argv[]) {
  if ( argc < 2 ) {
    cout << "Include filename when executing." << endl;
    return 0;
  }
  Point_t points[20];
  int totalElements = readFile(argv[1], points);
  double distance, shortest, longest;
  //cout << "Total Elements: " << totalElements << endl;
  int *i = new int[totalElements];
  int *shortRoute = new int[totalElements];
  int *longestRoute = new int[totalElements];
  for (int j = 0; j < totalElements; j++) {
    i[j] = j;
    shortRoute[j] = j;
    longestRoute[j] = j;
  }
  //distance = routeDistance(i, points, totalElements);
  //cout << "Distance traveled: " << distance << endl;
  //cout << endl << endl << endl;
  cout << "--------------" << endl;
  int counter = 1;
  shortest = routeDistance(i, points, totalElements);
  longest = shortest;
  while (next_permutation(i,i+totalElements)) {
    counter++;
    distance = routeDistance(i, points, totalElements);
    if (distance < shortest) {
      for (int j = 0; j < totalElements; j++) {
	shortRoute[j] = i[j];
	}
      shortest = distance;
    }
    if (distance > longest) {
      for (int j = 0; j < totalElements; j++) {
	longestRoute[j] = i[j];
      }
      longest = distance;
    }
    for (int a = 0; a < totalElements; a++) {
      cout << i[a] << ' ';
    }
    cout << distance << endl;
  }
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
  cout << counter << endl;
  delete [] i;
  delete [] shortRoute;
  delete [] longestRoute;
  return 0;
}
