/* -------------------------------------------
 *
 * Project 6: Open street maps
 * @author Rahin Jain
 * CS 251 Spring '23
 * Project provided by: Prof. Adam Kohler
 * @file application.cpp
 *
  ------------------------------------------- */
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <stack>
#include <set>
#include <limits>
#include <algorithm>
#include <cstring>
#include <cassert>

#include "tinyxml2.h"
#include "dist.h"
#include "graph.h"
#include "osm.h"

using namespace std;
using namespace tinyxml2;

typedef graph<long long, double> graphy;
typedef map<long long, Coordinates> coordMap;

const double INF = numeric_limits<double>::max();

class prioritize {
public:
    bool operator()(const pair<long long, double>& p1, const pair<long long, double>& p2) const {

        return p1.second > p2.second;
    }
};

// ----------------------- Helper functions ----

bool searchBuilding(string buildingName, vector<BuildingInfo>& buildingList, BuildingInfo& buildingInfo) {

    for (int i = 0; i < buildingList.size(); i++) {
        if (buildingList[i].Fullname.find(buildingName) != string::npos) {
            buildingInfo = buildingList[i];
            return true;
        }
    }
    return false;
}

BuildingInfo nearestBuilding(Coordinates midpoint, vector<BuildingInfo>& Buildings, set<string>& unreachable) {

    double min = INF;
    BuildingInfo nearest;

    for (auto& b : Buildings) {

        if (unreachable.count(b.Fullname) != 0) {
            continue;
        }
        double distance = distBetween2Points(midpoint.Lat, midpoint.Lon, b.Coords.Lat, b.Coords.Lon);

        if (distance < min) {

            min = distance;
            nearest = b;
        }
    }
    return nearest;
}

long long nearestNode(BuildingInfo building, vector<FootwayInfo>& footway, map<long long, Coordinates>& coords) {

    double min = INF;
    long long nearest;

    for (auto& foot : footway) {
        for (int i = 0; i < foot.Nodes.size(); i++) {

            double distance = distBetween2Points(building.Coords.Lat, building.Coords.Lon,coords[foot.Nodes[i]].Lat, coords[foot.Nodes[i]].Lon);
            if (distance < min) {

                min = distance;
                nearest = foot.Nodes[i];
            }
        }
    }
    return nearest;
}

vector<long long> getPath(long long startVertex, map<long long, long long>& predStorage, long long destination) {

    vector<long long> pathVec;
    stack<long long> pathStack;

    pathStack.push(destination);
    long long curr = destination;

    while (startVertex != curr) {

        curr = predStorage[curr];
        pathStack.push(curr);
    }

    while (!pathStack.empty()) {

        curr = pathStack.top();
        pathVec.push_back(curr);
        pathStack.pop();
    }

    return pathVec;
}

vector<long long> Dijkstra(graphy G, long long startV, map<long long, double>& distance, map<long long, long long>& predecessors) {

    vector<long long> visited;
    priority_queue <pair<long long, double>, vector<pair<long long, double>>,prioritize> unvisitedQueue;

    for (auto a : G.getVertices()) {

        distance[a] = INF;
        predecessors[a] = 0;
        unvisitedQueue.push(make_pair(a, INF));
    }

    distance[startV] = 0;
    unvisitedQueue.push(make_pair(startV, 0));

    while (!unvisitedQueue.empty()) {

        long long topV = unvisitedQueue.top().first;
        unvisitedQueue.pop();

        if (distance[topV] == INF) {

            break;
        }
        else if (count(visited.begin(), visited.end(), topV) > 0) {

            continue;
        }
        else {

            visited.push_back(topV);
        }

        for (auto neighbor : G.neighbors(topV)) {

            double edgeWeight;
            G.getWeight(topV, neighbor, edgeWeight);
            double alternative = distance[topV] + edgeWeight;

            if (alternative < distance[neighbor]) {

                distance[neighbor] = alternative;
                unvisitedQueue.push(make_pair(neighbor, alternative));
                predecessors[neighbor] = topV;
            }
        }
    }
    return visited;
}

// ----------------------- Print functions ----

void printPath(vector<long long>& p) {

    cout << "Path: ";
    for (int i = 0; i < p.size() - 1; i++) {
        cout << p[i] << "->";
    }

    cout << p[p.size() - 1] << endl;
}

void printBuilding(BuildingInfo b1, BuildingInfo b2, BuildingInfo dest) {

    cout << "Person 1's point:" << endl;
    cout << " " << b1.Fullname << endl;
    cout << " (" << b1.Coords.Lat << ", " << b1.Coords.Lon << ")" << endl;
    cout << "Person 2's point:" << endl;
    cout << " " << b2.Fullname << endl;
    cout << " (" << b2.Coords.Lat << ", " << b2.Coords.Lon << ")" << endl;
    cout << "Destination Building:" << endl;
    cout << " " << dest.Fullname << endl;
    cout << " (" << dest.Coords.Lat << ", " << dest.Coords.Lon << ")" << endl;
    cout << endl;
}

void printNodes(long long& n1, long long& n2, long long& cent, map<long long, Coordinates>& N) {

    cout << "Nearest P1 node:" << endl;
    cout << " " << n1 << endl;
    cout << " (" << N[n1].Lat << ", " << N[n1].Lon << ")" << endl;
    cout << "Nearest P2 node:" << endl;
    cout << " " << n2 << endl;
    cout << " (" << N[n2].Lat << ", " << N[n2].Lon << ")" << endl;
    cout << "Nearest destination node:" << endl;
    cout << " " << cent << endl;
    cout << " (" << N[cent].Lat << ", " << N[cent].Lon << ")" << endl << endl;
}

void printNewDestInfo(BuildingInfo& dest, long long& destN, Coordinates& N) {

    cout << "New destination building:" << endl;
    cout << " " << dest.Fullname << endl;
    cout << " (" << dest.Coords.Lat << ", " << dest.Coords.Lon << ")" << endl;
    cout << "Nearest destination node:" << endl;
    cout << " " << destN << endl;
    cout << " (" << N.Lat << ", " << N.Lon << ")" << endl << endl;
}

// ----------------------- Logic functions ----

void application(coordMap& Nodes, vector<FootwayInfo>& Footways, vector<BuildingInfo>& Buildings, graphy& G) {

    string person1, person2;
    BuildingInfo buildA, buildB;

    cout << endl;
    cout << "Enter person 1's building (partial name or abbreviation), or #> ";
    getline(cin, person1);

    while (person1 != "#") {
        cout << "Enter person 2's building (partial name or abbreviation)> ";
        getline(cin, person2);

        if (!searchBuilding(person1, Buildings, buildA)) {
            cout << "Person 1's building not found" << endl;
            cout << "Enter person 1's building (partial name or abbreviation), or #> ";
            getline(cin, person1);
            continue;
        }
        if (!searchBuilding(person2, Buildings, buildB)) {
            cout << "Person 2's building not found" << endl;
            cout << "Enter person 1's building (partial name or abbreviation), or #> ";
            getline(cin, person1);
            continue;
        }

        Coordinates midpoint = centerBetween2Points(buildA.Coords.Lat,buildA.Coords.Lon, buildB.Coords.Lat, buildB.Coords.Lon);
        set <string> unreachable;
        bool pathCondition = true;

        while (pathCondition) {

            BuildingInfo midBuilding = nearestBuilding(midpoint, Buildings, unreachable);

            long long nearNodeID1 = nearestNode(buildA, Footways, Nodes);
            long long nearNodeID2 = nearestNode(buildB, Footways, Nodes);
            long long nearNodeIDCen = nearestNode(midBuilding, Footways, Nodes);

            if (unreachable.size() == 0) {

                cout << endl;
                printBuilding(buildA, buildB, midBuilding);
                printNodes(nearNodeID1, nearNodeID2, nearNodeIDCen, Nodes);
            }
            else {
                printNewDestInfo(midBuilding, nearNodeIDCen, Nodes[nearNodeIDCen]);
            }

            map<long long, double> distances1, distances2;
            map<long long, long long> predecessor1, predecessor2;
            vector<long long> p1 = Dijkstra(G, nearNodeID1, distances1, predecessor1);

            if (distances1[nearNodeID2] >= INF) {
                cout << "Sorry, destination unreachable." << endl;
                break;
            }

            vector<long long> p2 = Dijkstra(G, nearNodeID2, distances2, predecessor2);

            if (distances1[nearNodeIDCen] >= INF || distances2[nearNodeIDCen] >= INF) {

                cout << "At least one person was unable to reach the destination building.";
                cout << " Finding next closest building..." << endl << endl;
                unreachable.insert(midBuilding.Fullname);
            }
            else {

                pathCondition = false;
                vector<long long> path1 = getPath(nearNodeID1, predecessor1, nearNodeIDCen);
                vector<long long> path2 = getPath(nearNodeID2, predecessor2, nearNodeIDCen);

                cout << "Person 1's distance to dest: " << distances1[nearNodeIDCen];
                cout << " miles" << endl;
                printPath(path1);

                cout << endl;
                cout << "Person 2's distance to dest: " << distances2[nearNodeIDCen];
                cout << " miles" << endl;
                printPath(path2);
            }
        }

        cout << endl;
        cout << "Enter person 1's building (partial name or abbreviation), or #> ";
        getline(cin, person1);
    }
}

int main() {

  graphy G;
  coordMap Nodes;
  vector<FootwayInfo> Footways;
  vector<BuildingInfo> Buildings;
  XMLDocument xmldoc;

  cout << "** Navigating UIC open street map **" << endl;
  cout << endl;
  cout << std::setprecision(8);

  string def_filename = "map.osm";
  string filename;

  cout << "Enter map filename> ";
  getline(cin, filename);

  if (filename == "") {
    filename = def_filename;
  }

  // Load XML-based map file
  if (!LoadOpenStreetMap(filename, xmldoc)) {
    cout << "**Error: unable to load open street map." << endl;
    cout << endl;
    return 0;
  }

  // Read the nodes, which are the various known positions on the map:
  int nodeCount = ReadMapNodes(xmldoc, Nodes);

  // Read the footways, which are the walking paths:
  int footwayCount = ReadFootways(xmldoc, Footways);

  // Read the university buildings:
  int buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);

  // Stats
  assert(nodeCount == (int)Nodes.size());
  assert(footwayCount == (int)Footways.size());
  assert(buildingCount == (int)Buildings.size());

  cout << endl;
  cout << "# of nodes: " << Nodes.size() << endl;
  cout << "# of footways: " << Footways.size() << endl;
  cout << "# of buildings: " << Buildings.size() << endl;

  // adding nodes to the graph
  for (auto& a : Nodes) {
      G.addVertex(a.first);
  }

  // adding edges to the graph
  for (auto& b: Footways) {
      for (int i = 0; i < ((int)b.Nodes.size())-1; i++) {
          double weight = distBetween2Points(Nodes[b.Nodes[i]].Lat, Nodes[b.Nodes[i]].Lon, Nodes[b.Nodes[i+1]].Lat, Nodes[b.Nodes[i+1]].Lon);
          G.addEdge(b.Nodes[i], b.Nodes[i+1], weight);
          G.addEdge(b.Nodes[i + 1], b.Nodes[i], weight);
      }
  }

   cout << "# of vertices: " << G.NumVertices() << endl;
   cout << "# of edges: " << G.NumEdges() << endl;
   cout << endl;

  // Execute Application
  application(Nodes, Footways, Buildings, G);

  cout << "** Done **" << endl;
  return 0;
}
