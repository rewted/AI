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
list<int> bestRoute;
int populationSize = 100;
int populationKill = 40;
int crossoverSize = 5;

bool validPath (list<int> a) {
  vector<int> b;
  for (int i = 0; i < a.size(); i++) {
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

static void do_drawing(cairo_t *cr) {
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_set_line_width(cr, 1);

  for (int i = 0; i < totalCount; i++) {
    cairo_arc(cr, 9*points[i].x, 9*points[i].y, 5, 0, 2*M_PI);
    cairo_stroke_preserve(cr);
    cairo_fill(cr);
  }
  list<int>::iterator it = bestRoute.begin();
  for (int i = 0; i < totalCount-1; i++) {
    cairo_move_to(cr, 9*points[*it-1].x, 9*points[*it-1].y);
    it++;
    cairo_line_to(cr, 9*points[*it-1].x, 9*points[*it-1].y);
  }
  it = bestRoute.begin();
  cairo_move_to(cr, 9*points[*it-1].x, 9*points[*it-1].y);
  advance(it,totalCount-1);
  cairo_line_to(cr, 9*points[*it-1].x, 9*points[*it-1].y);
  cairo_stroke(cr);
}

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
  do_drawing(cr);
  return false;
}

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
  a->generation = 0;
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
  for (int j = 0; j < totalCount; j++) {
    a->citizens[j].rank = i;
  }
}

void generationStats (Population *a) {
  //cout << "Gen stats" << endl;
  double average = 0;
  for (int i = 0; i < totalCount; i++) {
    average = average + a->citizens[i].fitness;
  }
  average = average / totalCount;
  a->averageFitness = average;
  a->bestFitness = a->citizens[0].fitness;
  a->worstFitness = a->citizens[populationSize-1].fitness;
}

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

void printWorld (Population *a) {
  
  cout << a->generation << "," << a->bestFitness << "," << a->averageFitness << "," << a->worstFitness << endl;
  
  cout << "World stats \tGeneration: " << a->generation << "\tBest: " << a->bestFitness << "\tAverage: " << a->averageFitness << "   " << "\tWorst: " << a->worstFitness << endl; 
  
  
  for (int i = 0; i < 10; i++) {
    cout << "--[RANK: " << i << " FITNESS: " << a->citizens[i].fitness << "]--" << endl;
    for (list<int>::iterator it = a->citizens[i].tour.begin(); it != a->citizens[i].tour.end(); it++) {
      cout << *it << " ";
    }
    cout << endl << endl;
  }
  
}

void generationKill (Population *a) {
  for (int i = 0; i < populationKill; i++) {
    a->citizens.pop_back();
  }
}

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

void pmxCrossover (Population *a) {
  generationKill(a);
  int count = 0;
  int parentA = 0;
  int parentB = 0;
  int crossoverSite;
  list<int> childA;
  list<int> childB;
  vector<int> aCross;
  vector<int> tmpA;
  vector<int> bCross;
  vector<int> tmpB;
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
  // need to replace 40 children
  // two created at a time
  while (count != 40) {
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
      crossoverSite = random (totalCount-5);
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
      cout << *itParentA1 << " " << *itParentA2 << " " << *itParentB1 << " " << *itParentB2 << endl;
      ptr = a->citizens[parentA].tour.begin();

      // some print functions for logs
      while (ptr != a->citizens[parentA].tour.end()) {
	cout << *ptr << " ";
	ptr++;
      }
      cout << endl;
      ptr = a->citizens[parentB].tour.begin();
      while (ptr != a->citizens[parentB].tour.end()) {
	cout << *ptr << " ";
	ptr++;
      }
      cout << endl;


      
      cout << "----- children ----" << endl;
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
      ptr = childA.begin();
      while (ptr != childA.end()) {
	cout << *ptr << " ";
	ptr++;
      }
      cout << endl;
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
      ptr = childB.begin();
      while (ptr != childB.end()) {
	cout << *ptr << " ";
	ptr++;
      }      
      cout << endl;

      
      // getting values for deduping
      ptr = itParentA1;
      cout << "[ ";
      while (ptr != itParentA2) {
	cout << *ptr << " ";
	aCross.push_back(*ptr);
	ptr++;
      }
      // getting values for deduping
      ptr = itParentB1;
      cout <<"]----[ ";
      while (ptr != itParentB2) {
	bCross.push_back(*ptr);
	cout << *ptr << " ";
	ptr++;
      }
      cout << "]" << endl;
      // deduping crossing values
      //cout << "Deduping" <<endl;
      vector<int>::iterator itCrossA = aCross.begin();
      vector<int>::iterator itCrossB = bCross.begin();
      while (itCrossA != aCross.end()) {
	itCrossB = bCross.begin();
	while (itCrossB != bCross.end()) {
	  if (*itCrossA == *itCrossB) {
	    //cout << "Erasing: " << *itCrossA << endl;
	    itCrossA = aCross.erase(itCrossA);
	    itCrossB = bCross.erase(itCrossB);
	    if (itCrossB == bCross.end() || itCrossA == aCross.end()) {
	      //cout << "B: Deleting an end" << endl;
	      break;
	    }
	  }
	  itCrossB++;
	}
	if (itCrossA == aCross.end()) {
	  //cout << "A: deleting an end" << endl;
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
      
      /*
      for (list<int>::iterator a = childB.begin(); a != childB.end(); a++) {
	cout << *a << " ";
      }
      */
      //cout << endl << endl;
      /*
      for (list<int>::iterator a = childB.begin(); a != childB.end(); a++) {
	cout << *a << " ";
      }
      */

      // removes duplicate nodes in the child
      ptr = childA.begin();
      itChild1 = childA.begin();
      itChild2 = childA.begin();
      advance (itChild1, crossoverSite);
      advance (itChild2, crossoverSite + 5);
		     cout << *itChild2 << endl;
      itCrossA = aCross.begin();
      itCrossB = bCross.begin();
      tmpA = aCross;
      tmpB = bCross;
      while (ptr != childA.end()) {
	if (ptr == itChild1) {
	  ptr = itChild2;
	}
	itCrossB = bCross.begin();
	while (itCrossB != bCross.end()) {
	  if (*ptr == *itCrossB) {
	    //cout << "Equal " << *ptr << " Inserting: " << *itCrossA <<  endl;
	    childA.insert(ptr, *itCrossA);
	    ptr = childA.erase(ptr);
	    itCrossB = bCross.erase(itCrossB);
	    itCrossA = aCross.erase(itCrossA);
	    if (itCrossB == bCross.end()) {
	      break;
	    }
	  }
	  itCrossB++;
	}
	if (ptr == childA.end()) {
	  break;
	}
	ptr++;
      }
      ptr = childB.begin();
      itChild1 = childB.begin();
      itChild2 = childB.begin();
      advance (itChild1, crossoverSite);
      advance (itChild2, crossoverSite + 5);
      itCrossA = aCross.begin();
      itCrossB = bCross.begin();
      aCross = tmpA;
      bCross = tmpB;
      while (ptr != childB.end()) {
	if (ptr == itChild1) {
	  ptr = itChild2;
	}
	itCrossA = aCross.begin();
	while (itCrossA != aCross.end()) {
	  if (*ptr == *itCrossA) {
	    //cout << "Equal " << *ptr << " Inserting: " << *itCrossB <<  endl;
	    childB.insert(ptr, *itCrossB);
	    ptr = childB.erase(ptr);
	    itCrossB = bCross.erase(itCrossB);
	    itCrossA = aCross.erase(itCrossA);
	    if (itCrossA == aCross.end()) {
	      break;
	    }
	  }
	  itCrossA++;
	}
	if (ptr == childB.end()) {
	  break;
	}
	ptr++;
      }
      for (list<int>::iterator a = childB.begin(); a != childB.end(); a++) {
	//cout << *a << " ";
      }
      //cout << endl << endl;
      Chromo temp;
      
      if (validPath (childA) == false) {
	ptr = childA.begin();
	cout << "A: ";
	while (ptr != childA.end()) {
	  cout << *ptr << " ";
	  ptr++;
	}
	cout << endl;
      }
      if (validPath (childB) == false) {
	ptr = childB.begin();
	cout << "B: ";
	while (ptr != childB.end()) {
	  cout << *ptr << " ";
	  ptr++;
	}
	cout << endl;
      }
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
  for (int i = 0; i<500; i++) {
    generationMutate (&world, mutationRate);
    pmxCrossover (&world);
    generationSort (&world);
    generationStats (&world);
    //printWorld (&world);
    world.generation++;
  }
  bestRoute = world.citizens[0].tour;
  validPath(bestRoute);
  list<int>::iterator iter = bestRoute.begin();
  while (iter != bestRoute.end()) {
    cout << *iter << " ";
    iter++;
  }
  cout << endl;
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
  gtk_window_set_title(GTK_WINDOW(window), "TSP - Greedy");
  gtk_widget_show_all(window);
  gtk_main();
  return 0;
}
