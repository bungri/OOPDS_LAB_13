/* BreadthFirstSearch.h */

#ifndef BREADTH_FIRST_SEARCH_HPP
#define BREADTH_FIRST_SEARCH_HPP

#include "Graph.hpp"
using namespace std;
class BreadthFirstSearch
{
protected:
	typedef Graph::Vertex Vertex;
	typedef Graph::Edge Edge;
	typedef std::list<Graph::Vertex> VertexList;
	typedef std::list<Graph::Edge> EdgeList;
private:
	Graph& graph;
	Vertex start;
	bool done; // flag of search done
	int **ppDistMtrx; // distance matrix
	int *pLeastCost; //least cost to the destination from this (start) node
	int *pPrev; // previous vertex in shortest path tree
protected:
	void bfsTraversal(Vertex& v, Vertex& target, VertexList& path);
	void dijkstraBFS(Vertex& s, int* pPrev);
	virtual bool isDone() const { return done; }
	// marking utilities
	void visit(Vertex& v);
	void visit(Edge& e);
	void unvisit(Vertex& v);
	void unvisit(Edge& e);
	bool isVisited(Vertex& v);
	bool isVisited(Edge& e);
	void setEdgeStatus(Edge& e, EdgeStatus es);
	EdgeStatus getEdgeStatus(Edge& e);
public:
	BreadthFirstSearch(Graph& g) :graph(g) { /*. . . .*/ }
	void initialize();
	void initDistMtrx();
	void printDistMtrx();
	void findShortestPath(Vertex& s, Vertex& t, VertexList& path);
	void ShortestPathsTree(Vertex& s);
	Graph& getGraph() { return graph; }
	int** getppDistMtrx() { return ppDistMtrx; }
	int* getpPrev() { return pPrev; }
	int* getpLeastCost() { return pLeastCost; }
};
#endif