#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <list>
#include <cairo.h>
#include <gtk/gtk.h>

using namespace std;

// data structure to hold point information
typedef struct Point {
  int id;
  double x, y;
} Point;

// global variables to help with gfx
Point points[100];
int totalCount;
list<int> globalRoute;

// functions to implement program
void insert (Point *, int, double, double);
int readFile (char *, Point *);
double distanceCalc (Point, Point);
void printValues (Point *, int);
double routeDistance (list<int>, Point *, int);
Point findStart (Point *, int);
Point insertNextPoint (Point *, list<int> *, list<int> *, int);
void initializeList (Point *,list<int> *, list<int> *, int);
int findPoint(Point , Point , Point , double *);

// some gfx stuff
static void do_drawing(cairo_t *);
static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
  do_drawing(cr);
  return false;
}





int main (int argc, char *argv[]) {
  // used to check if the file has been supplied as an argument
  if (argc < 2) {
    cout << "Include file name when executing." << endl;
    return 0;
  }
  //Point points[40];
  totalCount = readFile(argv[1], points);
  list<int> path;
  list<int> notInPath;
  list<int> *pathPt = &path;
  Point start = findStart(points, totalCount);
  initializeList(points, pathPt, &notInPath, totalCount);
  //path.push_back(start.id);
  // comment above and uncomment below
  // can be used to edit start point
  path.push_back(4); 
  while(notInPath.size() > 0) {
    insertNextPoint(points, &path, &notInPath, totalCount);
  }
  globalRoute = path;
  routeDistance(path, points, totalCount);
  // below is graphics stuff
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






// used to insert data into the point structure
void insert(Point *point, int i, double x, double y) {
  point->id = i;
  point->x = x;
  point->y = y;
}


// opens the file and reads the data from the file
// stores the values in a Point struct
int readFile(char *fileName, Point *points) {
  string buffer;
  string tmp;
  int count = 0; // number of elements
  int id;
  double x, y;
  ifstream tspFile(fileName);
  if (!tspFile.is_open()) {
    cout << "File failed to loadd" << endl;
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
    x = stod(tmp);
    iss >> tmp;
    y = stod(tmp);
    iss >> tmp;

    // error check to make sure right values are being inserted
    if (count == id-1) {
      //      cout << "Inserting id: " << id << " x: "  << x << " y: " << y << endl;
      insert((points+count), id, x, y);
    }
    count++;
  }
  tspFile.close();
  return count;
}

// formula to calculate distance between two vertices
double distanceCalc(Point firstPoint, Point secondPoint) {
  return sqrt(pow((secondPoint.x - firstPoint.x),2) + pow((secondPoint.y - firstPoint.y),2));
}

// prints the points obtained from the data file
// used for debugging to ensure data is correct
void printValues (Point *points, int total) {
  for (int i = 0; i < total; i++) {
    cout << (points+i)->id << " " << points[i].x << " " << points[i].y << endl;
  }
}

// used to calculate the total distance of the path
// also connects the end of the path back to the start
double routeDistance (list<int> route, Point *points, int numPoints) {
  double distance = 0;
  list<int>::iterator it = route.begin();
  list<int>::iterator it2 = route.begin();
  advance(it2,numPoints-1);
  distance = distanceCalc(points[*it-1], points[*it2-1]);

  it2 = route.begin();
  it2++;
  while (it2 != route.end()) {
    distance = distance + distanceCalc(points[*it-1], points[*it2-1]);
    it++;
    it2++;
  }
  cout << endl;
  cout << "-------------" << endl;
  cout << "Total distance of route: " << distance << endl;
  return distance;
}

// used to get a start point, in this case searches for
// a node that is closet to origin (0,0)
Point findStart(Point *points, int total) {
  Point origin;
  int index;
  insert(&origin, -1, 0, 0);
  double tmpDistance, tmpDistance2;
  tmpDistance = -1;
  // find the point closest to origin (0,0) as starting point
  // tie will take 
  for (int i = 0; i<total; i++) {
    tmpDistance2 = distanceCalc(origin,points[i]);
    if (tmpDistance < 0 || tmpDistance > tmpDistance2) {
      tmpDistance = tmpDistance2;
      index = i;
    }
  }
  return points[index];
}

// mega function to insert the next node into the path
// evaluates two conditions, one when the start is the only point in the path
// then after that compares nodes to the edge and finds the closest edge node pair
// and inserts that into the path
Point insertNextPoint(Point *points, list<int> *path, list<int> *notInPath, int total) {
  if (path->size() == 1) {
    list<int>::iterator remove = notInPath->begin();
    while (remove != notInPath->end()) {
      if (*remove == *path->begin()) {
	cout << "Start has been chosen. Removing: " << *remove << endl;
	cout << points[*remove-1].id << " " << points[*remove-1].x << " " << points[*remove-1].y << endl;
	remove = notInPath->erase(remove);
      }
      remove++;
    }
    double tmpDistance, tmpDistance2;
    int index;
    tmpDistance = -1;
    list<int>::iterator it = notInPath->begin();
    while(it != notInPath->end()) {
      tmpDistance2 = distanceCalc(points[*path->begin()-1], points[*it-1]);
      cout << "Distance from: " << *path->begin() << " to " << points[*it-1].id << " is " << tmpDistance2 << endl;
      if (tmpDistance2 == 0) {
	cout << "Dang" << endl; 
      }
      if (tmpDistance < 0 || tmpDistance > tmpDistance2) {
	tmpDistance = tmpDistance2;
	index = *it-1;
      }
      it++;
    }
    path->push_back(points[index].id);
    list<int>::iterator it2 = path->begin();
    advance(it2,1);
    cout << "Adding to path: " << index+1 << " Distance: " << tmpDistance << endl;
    it = notInPath->begin();
    while (it != notInPath->end()) {
      if (*it == points[index].id) {
	cout << "Removing node: " << *it << " from unconnected node list " << endl;
	it = notInPath->erase(it);
	it = notInPath->end()--;
      }
      it++;
    }
    return points[index];
  }
  else {
    cout << "\n----------------------------\n";
    int edges = path->size()-1;
    int k = 0;
    int * closestToEdge = new int[edges];
    double * distances = new double[edges];
    list<int>::iterator vertex1 = path->begin();
    list<int>::iterator vertex2 = vertex1;
    vertex2++;
    while(vertex2 != path->end()) {
      cout << "-------------Currently looking at edge: " << k << endl;
      list<int>::iterator it = notInPath->begin();
      double tmpDistance = -1;
      double tmpDistance2;
      while(it != notInPath->end()) {
	findPoint(points[*vertex1-1],points[*vertex2-1],points[*it-1], &tmpDistance2);
	if(tmpDistance == -1 || tmpDistance > tmpDistance2) {
	  closestToEdge[k] = *it;
	  tmpDistance = tmpDistance2;
	  cout << "New closer is now: " << *it << " at distance: " << tmpDistance << endl;
	}
	it++;
      }
      distances[k] = tmpDistance;
      vertex1++;
      vertex2++;
      k++;
    }
    double shortest = -1;
    double tmpEst;
    int whichEdge;
    for (int i = 0; i < edges; i++) {
      tmpEst = distances[i];
      if (shortest == -1 || shortest > tmpEst) {
	shortest = tmpEst;
	whichEdge = i;
      }
    }
    cout << closestToEdge[whichEdge] << " " << distances[whichEdge] << endl;
    list<int>::iterator it = path->begin();
    list<int>::iterator remove = notInPath->begin();
    advance(it,whichEdge+1);
    path->insert(it,closestToEdge[whichEdge]);
    cout << "Inserting node: " << closestToEdge[whichEdge] << " into path on edge: " << whichEdge << endl;
    while (remove != notInPath->end()) {
      if (*remove == closestToEdge[whichEdge]) {
	cout << "Removing node: " << *remove << " from unconnected node list " << endl;
	remove = notInPath->erase(remove);
      }
      remove++;
    }
    it = path->begin();
    cout << "----------------" << endl;
    cout << "Current Path: " << endl;
    while (it != path->end()){
      cout << *it;
      it++;
      if (it != path->end()) {
	cout << " -> ";
      }
    }
    cout << endl;
    return points[0];
  }
  return points[0];
}

// this initializes both lists path and notInPath which keep track of
// the current path and the nodes currently not in the path.
// also can be used to reset
void initializeList(Point *points, list<int> *path, list<int> *notInPath, int total) {
  path->clear();
  notInPath->clear();
  for (int i = 0; i < total; i++) {
    notInPath->push_back(points[i].id);
  }
}

// this function is used to find the distance a vertex is away from an edge
// also finds the closest point that vertex is to an edge
int findPoint(Point p1, Point p2, Point p0, double * cost) {
  double x0, x1, x2, y0, y1, y2;
  x0 = p0.x;
  x1 = p1.x;
  x2 = p2.x;
  y0 = p0.y;
  y1 = p1.y;
  y2 = p2.y;

  double distance = abs((y2-y1)*x0-(x2-x1)*y0+x2*y1-y2*x1)/sqrt((y2-y1)*(y2-y1));
  double dx = x2-x1;
  double dy = y2-y1;
  double m = dy/dx;
  double x = 0;
  double b = m*(x-x1)+y1;
  // standard form
  double A = -m;
  double B = 1;
  double C = -b;
  double xp = (B*(B*x0-A*y0)-A*C)/(A*A+B*B);
  double yp = (A*(-B*x0+A*y0)-B*C)/(A*A+B*B);
  Point tmpPoint;
  tmpPoint.id = -1;
  tmpPoint.x = xp;
  tmpPoint.y = yp;
  //cout << "Distance: " << distance << endl;
  //cout << "Point x: " << xp << " y: " << yp << endl;
  double ptDistance1 = distanceCalc(p1,p2);
  double ptDistance2 = distanceCalc(p1,tmpPoint);
  double ptDistance3 = distanceCalc(p2,tmpPoint);
  if (ptDistance1 < ptDistance2 || ptDistance1 < ptDistance3) {
    // use distance to nearest vertex
    //cout << "Use a vertex " << endl;
    if (ptDistance2 < ptDistance3) {
      *cost = distanceCalc(p1, p0);
      cout << "Distance from: " << p0.id << " to " << p1.id << " is " << *cost << endl; 
      return -1;
    }
    else {
      *cost = distanceCalc(p2, p0);
      cout << "Distance from: " << p0.id << " to " << p2.id << " is " << *cost << endl;
      return 1;
    }
  }
  *cost = distance;
  cout << p0.id << " Projects onto an edge with distance:  " << *cost << endl;
  return 0;
}


// function used to draw the vertices and edges
static void do_drawing(cairo_t *cr) {
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_set_line_width(cr, 1);

  for (int i = 0; i < totalCount; i++) {
    cairo_arc(cr, 9*points[i].x, 9*points[i].y, 5, 0, 2*M_PI);
    cairo_stroke_preserve(cr);
    cairo_fill(cr);
  }
  list<int>::iterator it = globalRoute.begin();
  for (int i = 0; i < totalCount-1; i++) {
    cairo_move_to(cr, 9*points[*it-1].x, 9*points[*it-1].y);
    it++;
    cairo_line_to(cr, 9*points[*it-1].x, 9*points[*it-1].y);
  }
  it = globalRoute.begin();
  cairo_move_to(cr, 9*points[*it-1].x, 9*points[*it-1].y);
  advance(it,totalCount-1);
  cairo_line_to(cr, 9*points[*it-1].x, 9*points[*it-1].y);
  cairo_stroke(cr);
}
