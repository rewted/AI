/* 
   James Culver
   CECS 545 - Project 5
   Some notes to keep it from breaking:
   populationKill has to be even, having an odd will cause a segmentation fault
   This is due to child creation being done in groups of 2. 
 */

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
#include <cairo.h>
#include <gtk/gtk.h>
#include <iomanip>

using namespace std;

// keeps track of point data for cities
typedef struct Point {
  int id;
  double x, y;
} Point;

// keeps track of route data
typedef struct Chromo {
  int rank;
  list<int> tour;
  double fitness;
} Chromo;

// keeps track of the population
typedef struct Population {
  int generation;
  double averageFitness, bestFitness, worstFitness;
  vector<Chromo> citizens;
} Population;

// Global Variables
Point points[222];
int totalCount = 222;
list<int> bestRoute;
int populationSize = 50;
int populationKill = 10;
int crossoverSize = 3;
int mutationRate = 10;
int generations = 5000;
int numOfBest = 30;
int matrix [222][222];

// function to check if a path is valid or not
bool validPath (list<int> a) {
  vector<int> b;
  for (int i = 0; i < (int)a.size(); i++) {
    b.push_back(0);
  }
  list<int>::iterator it = a.begin();
  while (it != a.end()) {
    b[*it]++;
    it++;
  }
  vector<int>::iterator it2 = b.begin();
  while (it2 != b.end()) {
    if (*it2 != 1) {
      cout << "******************Not a valid path********************" << endl;
      return false;
    }
    it2++;
  }
  return true;
}

// function used for visualization
static void do_drawing(cairo_t *cr) {
  cairo_set_source_rgb(cr, 0, 0, 255);
  cairo_set_line_width(cr, 1);

  for (int i = 0; i < totalCount; i++) {
    cairo_arc(cr, 9*points[i].x, 9*points[i].y, 5, 0, 2*M_PI);
    cairo_stroke_preserve(cr);
    cairo_fill(cr);
  }
  cairo_set_source_rgb(cr, 0, 0, 0);
  list<int>::iterator it = bestRoute.begin();
  list<int>::iterator it2 = bestRoute.end();
  it2--;
  while (it != it2) {
    cairo_move_to(cr, 9*points[*it].x, 9*points[*it].y);
    it++;
    cairo_line_to(cr, 9*points[*it].x, 9*points[*it].y);
  }
  cairo_stroke(cr);
  cairo_set_source_rgb(cr, 0, 255, 0);
  cairo_arc(cr, 9*points[*bestRoute.begin()].x, 9*points[*bestRoute.begin()].y, 8, 0, 2*M_PI);
  cairo_stroke_preserve(cr);
  cairo_fill(cr);
  cairo_set_source_rgb(cr, 255, 0, 0);
  it = bestRoute.end();
  it--;
  cairo_arc(cr, 9*points[*it].x, 9*points[*it].y, 8, 0, 2*M_PI);
  cairo_stroke_preserve(cr);
  cairo_fill(cr);
  /*
  it = bestRoute.begin();
  cairo_move_to(cr, 9*points[*it-1].x, 9*points[*it-1].y);
  advance(it,totalCount-1);
  cairo_line_to(cr, 9*points[*it-1].x, 9*points[*it-1].y);
  */
  cairo_stroke(cr);

}

// graphics function
static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
  do_drawing(cr);
  return false;
}

// used to insert data into a point structure
void insert (Point *point, int i, double x, double y) {
  point->id = i;
  point->x = x;
  point->y = y;
}

// reads a file and inserts points and creates an array of cities
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

  for (int i = 0; i
	 < 7; i++) {
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

// function to calculate distance between two points
double distanceCalc (Point firstPoint, Point secondPoint) {
  return sqrt(pow((secondPoint.x - firstPoint.x),2) + pow((secondPoint.y - firstPoint.y),2));
}

// function to calculate the total distance for a route
double routeDistance (list<int> route, Point *points, int numPoints) {
  double distance = 0;
  list<int>::iterator it = route.begin();
  list<int>::iterator it2 = route.begin();
  advance(it2, numPoints-1);
  distance = distanceCalc (points[*it], points[*it2]);

  it2 = route.begin();
  it2++;
  while (it2 != route.end()) {
    distance = distance + distanceCalc (points[*it], points[*it2]);
    it++;
    it2++;
  }
  return distance;
}

// function to generate random numbers
int random (int i) { return std::rand()%i; }

// function that generates a population
// each citizen has a random path
// path fitness is calculated
void generatePopulation (Population *a) {
  a->generation = 0;
  a->citizens.clear();
  vector<int> b;
  list<int> path;
  Chromo tmpChromo;
  // create a path using vector<int>
  for (int i = 0; i < totalCount; i++) {
    b.push_back(i);
  }
  // randomize path for each citizen
  for (int i = 0; i < populationSize; i++) {
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

// the population is sorted by fitness
// fittest will have lower index values
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
  for (int j = 0; j < populationSize; j++) {
    a->citizens[j].rank = i;
  }
}

// generates stats such as fittest, average, and least fit
void generationStats (Population *a) {
  //cout << "Gen stats" << endl;
  double average = 0;
  for (int i = 0; i < populationSize; i++) {
    average = average + a->citizens[i].fitness;
  }
  average = (average / (double)populationSize);
  a->averageFitness = average;
  a->bestFitness = a->citizens[0].fitness;
  a->worstFitness = a->citizens[populationSize-1].fitness;
}

// has a chance to mutate a citizen based on mutation rate
// generates a new random path
void generationMutate (Population *a, int mutationRate) {
  int nodeA = 0;
  int nodeB = 0;
  list<int>::iterator it;
  list<int>::iterator it2;
  for (int i = 0; i < populationSize; i++) {
    if (random (mutationRate) == 0) {
      while (nodeA == nodeB) {
	nodeA = random (totalCount);
	nodeB = random (totalCount);
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
    }
    nodeA = 0;
    nodeB = 0;
  }
}
/* // mutates entire path by generating a random new one
void generationMutate (Population *a, int mutationRate) {

  for (int i = 0; i < populationSize; i++) {
    if (random (mutationRate) == 0) {
      a->citizens[i].tour.clear();
      vector<int> b;
      for (int i = 0; i < totalCount; i++) {
	b.push_back(i);
      }
      random_shuffle (b.begin(), b.end());
      for (vector<int>::iterator it = b.begin(); it != b.end(); it++) {
	a->citizens[i].tour.push_back(*it);
      }      
    }

  }
}
*/


// print function used to print data about the population
void printWorld (Population *a) {
  
  cout << a->generation << "," << a->bestFitness << "," << a->averageFitness << "," << a->worstFitness << endl;
  
  //cout << "World stats \tGeneration: " << a->generation << "\tBest: " << a->bestFitness << "\tAverage: " << a->averageFitness << "   " << "\tWorst: " << a->worstFitness << endl; 
  
  /*
  for (int i = 0; i < 10; i++) {
    cout << "--[RANK: " << i << " FITNESS: " << a->citizens[i].fitness << "]--" << endl;
    for (list<int>::iterator it = a->citizens[i].tour.begin(); it != a->citizens[i].tour.end(); it++) {
      cout << *it << " ";
    }
    cout << endl << endl;
  }
  */
}

// function used to kill unfit citizens
void generationKill (Population *a) {
  for (int i = 0; i < populationKill; i++) {
    a->citizens.pop_back();
  }
}

// used to get a percent gradient for mating
double getPercent (int a) {
  if (a < 15) {
    return 0.8;
  }
  if (a >= 15 && a < 30) {
    return 0.7;
  }
  if (a >= 30 && a < 45) {
    return 0.6;
  }
  if (a >= 45 && a < 60) {
    return 0.5;
  }
  else {
    return 0.5;
  }
}

// parents mating using partially mapped crossover
list<int> mate (list<int> parent, list<int> cross, int site) {
  list<int> child;
  list<int>::iterator ptr1 = parent.begin();
  list<int>::iterator ptr2 = cross.begin();
  for (int i = 0; i < site; i++) {
    child.push_back(*ptr1);
    ptr1++;
  }
  for (int i = 0; i < crossoverSize; i++) {
    child.push_back(*ptr2);
    ptr2++;
  }
  ptr1 = parent.begin();
  advance (ptr1, site+crossoverSize);
  while (ptr1 != parent.end()) {
    child.push_back(*ptr1);
    ptr1++;
  }
  return child;
}

// the crossovers used is deduped
void dedupeCross (list<int> *cross1, list<int> *cross2) {
  bool matched = false;
  list <int> tmp1;
  list <int> tmp2;
  list <int> c1;
  list <int> c2;
  list <int>::iterator ptr1 = cross1->begin();
  list <int>::iterator ptr2;
  while (ptr1 != cross1->end()) {
    tmp1.push_back(*ptr1);
    ptr1++;
  }
  ptr1 = cross2->begin();
  while (ptr1 != cross2->end()) {
    tmp2.push_back(*ptr1);
    ptr1++;
  }
  ptr1 = tmp1.begin();
  ptr2 = tmp2.begin();
  while (ptr1 != tmp1.end()) {
    matched = false;
    ptr2 = tmp2.begin();
    while (ptr2 != tmp2.end()) {
      if (*ptr1 == *ptr2) {
	matched = true;
      }
      ptr2++;
    }
    if (!matched) {
      c1.push_back(*ptr1);
    }
    ptr1++;
  }

  ptr1 = tmp1.begin();
  ptr2 = tmp2.begin();
  while (ptr2 != tmp2.end()) {
    matched = false;
    ptr1 = tmp1.begin();
    while (ptr1 != tmp1.end()) {
      if (*ptr2 == *ptr1) {
	matched = true;
      }
      ptr1++;
    }
    if (!matched) {
      c2.push_back(*ptr2);
    }
    ptr2++;
  }
  *cross1 = c1;
  *cross2 = c2;
}

// dedupe the offspring
// so it will be a valid route
void dedupeChild (list<int> *child, list<int> cross1, list<int> cross2, int site) {
  bool matched = false;
  list<int> tmpc1 = cross1;
  list<int> tmpc2 = cross2;
  list<int> tmpChild;
  list<int> buffer;
  list<int>::iterator stop;
  list<int>::iterator start;
  list<int>::iterator ptr;
  list<int>::iterator c1ptr;
  ptr = child->begin();
  while (ptr != child->end()) {
    tmpChild.push_back(*ptr);
    ptr++;
  }
  ptr = tmpChild.begin();
  stop = tmpChild.begin();
  advance (stop, site);
  while (ptr != stop) {
    matched = false;
    c1ptr = tmpc1.begin();
    while (c1ptr != tmpc1.end()) {
      if (*ptr == *c1ptr) {
	matched = true;
      }
      c1ptr++;
    }
    if (!matched) {
      buffer.push_back(*ptr);
    }
    if (matched) {
      buffer.push_back(*tmpc2.begin());
      if (tmpc2.size() > 0) {
	tmpc2.pop_front();
      }
    }
    ptr++;
  }
  start = tmpChild.begin();
  advance (start, site+crossoverSize);
  while (ptr != start) {
    buffer.push_back(*ptr);
    ptr++;
  }
  
  ptr = start;
  while (ptr != tmpChild.end()) {
    matched = false;
    c1ptr = tmpc1.begin();
    while (c1ptr != tmpc1.end()) {
      if (*ptr == *c1ptr) {
	matched = true;
      }
      c1ptr++;
    }
    if (!matched) {
      buffer.push_back(*ptr);
    }
   
    if (matched) {
      buffer.push_back(*tmpc2.begin());
      if (tmpc2.size() > 0) {
	tmpc2.pop_front();
      }
    }
    ptr++;
  }
  validPath (buffer);
  *child = buffer;
}

// partially mapped crossover
// basically picks a location
// two parents are picked
// childA has parentB crossover mapped onto it
// childB has parentA crossover mapped onto it
// offspring are deduped so they will be valid routes
// the offspring are then added to the population
void pmxCrossover (Population *a) {
  generationKill(a);
  int count = 0;
  int parentA = 0;
  int parentB = 0;
  int crossoverSite;
  list<int> crossA;
  list<int> crossB;
  list<int> childA;
  list<int> childB;
  list<int>::iterator ptr;
  double chance = 0;
  // need to replace 40 children
  // two created at a time
  while (count != populationKill) {
    // select two different parents
    while (parentA == parentB) {
      parentA = random (populationSize - populationKill);
      parentB = random (populationSize - populationKill);
    }

    
    // percent gradent dependent on fitness
    // for chance to mate between parentA and parent B
    chance = getPercent(parentA) * getPercent(parentB); 
    //cout << "Parent A: " << parentA << " Parent B: " << parentB << " Chance: " << chance << endl;
    // mating successful initiate PMX Crossover
    if (random((1/chance)) == 0) {
      // select where crossover site will occur
      crossoverSite = random (totalCount-crossoverSize-1);

      ptr = a->citizens[parentB].tour.begin();
      advance(ptr, crossoverSite);
      for (int i = 0; i < crossoverSize; i++) {
	crossB.push_back(*ptr);
	ptr++;
      }
      childA = mate (a->citizens[parentA].tour, crossB, crossoverSite);
 
      ptr = a->citizens[parentA].tour.begin();
      advance(ptr, crossoverSite);
      for (int i = 0; i < crossoverSize; i++) {
 	crossA.push_back(*ptr);
	ptr++;
      }
      childB = mate (a->citizens[parentB].tour, crossA, crossoverSite);
      ptr = childB.begin();

      dedupeCross (&crossA, &crossB);
      ptr = crossA.begin();
      /*
      while (ptr != crossA.end()) {
	cout << *ptr << " ";
	ptr++;
      }
      cout << endl;
      ptr = crossB.begin();
      while (ptr != crossB.end()) {
	cout << *ptr << " ";
	ptr++;
      }
      
      cout << endl;
      */
      dedupeChild (&childA, crossB, crossA, crossoverSite);
      dedupeChild (&childB, crossA, crossB, crossoverSite);
      
      crossA.clear();
      crossB.clear();

      Chromo temp;

      temp.tour = childA;
      temp.fitness = routeDistance (childA, points, totalCount);
      temp.rank = 60+count;
      a->citizens.push_back(temp);
      temp.tour = childB;
      temp.rank = 61+count;
      temp.fitness = routeDistance (childB, points, totalCount);
      a->citizens.push_back(temp);        
      count = count+2;
    }
    childA.clear();
    childB.clear();
    chance = 0;
    parentA = 0;
    parentB = 0;
  }

}

//template <size_t s>
list<int> WOCPath () {
  list<int> path;
  vector<int> validSearch;

  // generate a vector with valid search parameters
  for (int i = 0; i < totalCount; i++) {
    validSearch.push_back(1);
  }
  int highest = -99999;
  int x, y;
  // find the most agreed on edge
  for (int i = 0; i < totalCount; i++) {
    for (int j = 0; j < totalCount; j++) {
      if (matrix[i][j] > highest && i != j) {
	highest = matrix[i][j];
	x = i;
	y = j;
	cout << "Found: " << x << " " << y << endl;
      }
    }
  }
  path.push_back(x);
  path.push_back(y);
  validSearch[x] = 0;
  while (path.size() != (size_t)totalCount) {
    x = y;
    highest = -99999;
    for (int j = 0; j < totalCount; j++) {
      if (validSearch[j] != 0 && x != j) {
	if (matrix[x][j] > highest) {
	  highest = matrix[x][y];
	  y = j;
	}
      }
    }
    cout << "Pushing: " << y << endl;
    path.push_back(y);
    validSearch[x] = 0;
  }

  validPath (path);
  list<int>::iterator it = path.begin();
  while (it != path.end()) {
    cout << *it << " ";
    it++;
  }
  cout << endl;
  vector<int>::iterator it2 = validSearch.begin();
  while ( it2 != validSearch.end()) {
    cout << *it2 << " ";
    it2++;
  }
  cout << endl;
  return path;
}

void generateAdjMatrix (Population *a) {
  //int matrix[totalCount][totalCount];
  // initialize adj matrix to all 0s
  for (int i = 0; i < totalCount; i++) {
    for (int j = 0; j < totalCount; j++) {
      matrix[i][j] = 0;
    }
  }

  // generates adj matrix
  for (int i = 0; i < populationSize; i++) {
    list<int>::iterator itF = a->citizens[i].tour.begin();
    list<int>::iterator itB = a->citizens[i].tour.end();
    itB--;
    // start and finish nodes connected together
    matrix[*itF][*itB]++;
    list<int>::iterator itBuff;
    for (int j = 0; j < totalCount; j++) {
      if ( itF != a->citizens[i].tour.end()) {
	itBuff = itF;
	itBuff++;
	matrix[*itF][*itBuff]++;
	itF++;
      }
      /*
      if (itB != a->citizens[i].tour.begin()) {
	itBuff = itB;
	itBuff--;
	matrix[*itB][*itBuff]++;
	itB--;
      }
      */
    }
  }

  for (int i = 0; i < totalCount; i++) {
    for (int j = 0; j < totalCount; j++) {
      cout << setw(3) << matrix[i][j];
    }
    cout << endl;
  }
  bestRoute = WOCPath ();
}

int main (int argc, char *argv[]) {
  if (argc < 2) {
    cout << "Include file name when executing." << endl;
    return 0;
  }
  srand (unsigned (time (0)));
  totalCount = readFile (argv[1], points);
  Population world;
  Chromo tmpChromo;
  vector<Chromo> tmpCitizens;

  for (int j = 0; j < numOfBest; j++) {
    generatePopulation (&world);
    for (int i = 0; i<generations; i++) {
      generationMutate (&world, mutationRate);
      pmxCrossover (&world);
      generationSort (&world);
      generationStats (&world);
      if (i%50 == 0) {
	printWorld (&world);
      }
      world.generation++;
    }
    bestRoute = world.citizens[0].tour;
    validPath(bestRoute);
    tmpChromo.tour = bestRoute;
    tmpChromo.fitness = world.citizens[0].fitness;
    tmpCitizens.push_back(tmpChromo);
  }
  world.citizens = tmpCitizens;
  // population has changed from initial pop for GA
  // to population for number selected for WOC
  populationSize = numOfBest;
  generationSort (&world);
  generateAdjMatrix (&world);
  
  /*
  for (int i = 0; i < populationSize; i++) {
    generatePopulation (&world);
    for (int i = 0; i<generations; i++) {
      generationMutate (&world, mutationRate);
      pmxCrossover (&world);
      generationSort (&world);
      generationStats (&world);
      	if (i%50 == 0) {
	printWorld (&world);
	}
	world.generation++;
    }
    bestRoute = world.citizens[0].tour;
    validPath(bestRoute);

    tmpChromo.tour = bestRoute;
    tmpChromo.fitness = world.citizens[0].fitness;
    tmpCitizens.push_back(tmpChromo);
    list<int>::iterator iter = bestRoute.begin();
    cout << world.citizens[0].fitness << " [ ";
      while (iter != bestRoute.end()) {
	cout << *iter << " ";
	iter++;
      }

    cout << "]" << endl;
  }
  world.citizens = tmpCitizens;
  generationSort(&world);
  for (int i = 0; i<generations; i++) {
    generationMutate (&world, mutationRate);
    pmxCrossover (&world);
    generationSort (&world);
    generationStats (&world);
      if (i%50 == 0) {
      printWorld (&world);
      }
    world.generation++;
  }
  list<int>::iterator iter = bestRoute.begin();
  cout << world.citizens[0].fitness << " [ ";
  while (iter != bestRoute.end()) {
    cout << *iter << " ";
    iter++;
  }

  cout << "]" << endl;
  */
  // stuff to create graphics
  GtkWidget *window;
  GtkWidget *darea;
  gtk_init(&argc,&argv);
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  darea = gtk_drawing_area_new();
  gtk_container_add(GTK_CONTAINER(window), darea);
  g_signal_connect(G_OBJECT(darea), "draw", G_CALLBACK(on_draw_event), NULL);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 1000, 1000);
  gtk_window_set_title(GTK_WINDOW(window), "TSP - Wisdom of Crowds");
  gtk_widget_show_all(window);
  gtk_main();
  return 0;
}
