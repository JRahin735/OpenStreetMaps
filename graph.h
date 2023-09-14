/* -------------------------------------------
 *
 * Project 6: Open street maps
 * @author Rahin Jain
 * CS 251 Spring '23
 * Project provided by: Prof. Adam Kohler
 * @file graph.h
 *
  ------------------------------------------- */

#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>
#include <map>

using namespace std;
template <typename VertexT, typename WeightT>

class graph {

 private:

    // Data structure
    map <VertexT, WeightT> toValue;
    map <VertexT, map<VertexT, WeightT>> fromValue;
    vector <VertexT> allVertices;

 public:

    // creates an empty graph
    graph() {

    }

    // Returns the # of vertices currently in the graph.
    int NumVertices() const {
        return fromValue.size();
    }

    // Returns the # of edges currently in the graph.
    int NumEdges() const {

        int count = 0;
        for (auto a : fromValue) {
            count += a.second.size();
        }
        return count;
    }

    //Adds the vertex v to the graph if there's room, and if so
    //returns true.  If the graph is full, or the vertex already
    //exists in the graph, then false is returned.
    bool addVertex(VertexT v) {

        if (fromValue.count(v) == 1) {
            return false;
        }
        else {
            map<VertexT,WeightT> emptyTemp;
            fromValue.emplace(v,emptyTemp);
            allVertices.push_back(v);
            return true;
        }
    }

    // Adds the edge (from, to, weight) to the graph, and returns
    // true.  If the vertices do not exist false is returned.
    bool addEdge(VertexT from, VertexT to, WeightT weight) {

        if(fromValue.count(from) == 1 && fromValue.count(to) == 1) {

            if (fromValue.at(from).count(to) == 1) {
                fromValue.at(from).at(to) = weight;
            }
            else {
                fromValue.at(from).emplace(to, weight);
            }
            return true;
        }
        else {
            return false;
        }

    }

    // Returns the weight associated with a given edge.  If
    // the edge exists, the weight is returned via the reference
    // parameter and true is returned.  If the edge does not
    // exist, the weight parameter is unchanged and false is
    // returned.
    bool getWeight(VertexT from, VertexT to, WeightT& weight) const {

        if (fromValue.count(from) == 1) {
            if (fromValue.at(from).count(to) == 1) {
                weight = fromValue.at(from).at(to);
                return true;
            }
        }
        return false;
    }

    // Returns a set containing the neighbors of v, i.e. all
    // vertices that can be reached from v along one edge.
    // Since a set is returned, the neighbors are returned in
    // sorted order; use foreach to iterate through the set.
    set<VertexT> neighbors(VertexT v) const {

        set<VertexT> allNeighbours;
        if (fromValue.count(v) == 1) {
            map<VertexT, WeightT> out = fromValue.at(v);
            for (auto a: out) {
                allNeighbours.insert(a.first);
            }
        }
        return allNeighbours;
    }

    // Returns a vector containing all the vertices currently in
    // the graph.
    vector<VertexT> getVertices() const {

        return allVertices;
    }

    void dump(ostream& output) const {

    }
};
