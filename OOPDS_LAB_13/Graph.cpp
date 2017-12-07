#include "Graph.hpp"

using namespace std;

typedef std::vector<Graph::Vertex> VtxList;
typedef std::list<Graph::Edge> EdgeList;
typedef std::vector<Graph::Vertex>::iterator VtxItor;
typedef std::list<Graph::Edge>::iterator EdgeItor;

void Graph::insertVertex(Vertex& v)
{
	int vtx_ID;
	vtx_ID = v.getID();
	if (pVrtxArray[vtx_ID] == NULL) {
		pVrtxArray[vtx_ID] = v;
	}
}

void Graph::vertices(VtxList& vtxLst)
{
	vtxLst.clear();
	for (int i = 0; i<getNumVertices(); i++)
		vtxLst.push_back(pVrtxArray[i]);
}

void Graph::insertEdge(Edge& e) // Edge e(vtx_1 -> vtx_2)
{
	Vertex vtx_1, vtx_2;
	Vertex* pVtx;
	int vtx_1_ID, vtx_2_ID;
	vtx_1 = e.getVertex_1();
	vtx_2 = e.getVertex_2();
	//vtx_1 = *e.getpVrtx_1();
	//vtx_2 = *e.getpVrtx_2();
	vtx_1_ID = vtx_1.getID();
	vtx_2_ID = vtx_2.getID();
	if (pVrtxArray[vtx_1_ID] == NULL) {
		pVrtxArray[vtx_1_ID] = vtx_1;
	}
	if (pVrtxArray[vtx_2_ID] == NULL) {
		pVrtxArray[vtx_2_ID] = vtx_2;
	}
	e.setpVrtx_1(&pVrtxArray[vtx_1_ID]);
	e.setpVrtx_2(&pVrtxArray[vtx_2_ID]);
	//edgeList.push_back(e);
	pAdjLstArray[vtx_1_ID].push_back(e);
}

void Graph::edges(EdgeList& edges)
{
	EdgeItor eItor;
	Graph::Edge e;
	edges.clear();
	for (int i = 0; i<getNumVertices(); i++)
	{
		eItor = pAdjLstArray[i].begin();
		while (eItor != pAdjLstArray[i].end())
		{
			e = *eItor;
			edges.push_back(e);
			eItor++;
		}
	}
}

void Graph::incidentEdges(Vertex v, EdgeList& edgeLst)
{
	Graph::Edge e;
	EdgeItor eItor;
	int vtx_id = v.getID();
	eItor = pAdjLstArray[vtx_id].begin();
	while (eItor != pAdjLstArray[vtx_id].end())
	{
		e = *eItor;
		edgeLst.push_back(e);
		eItor++;
	}
}

void Graph::printGraph()
{
	int i, j;
	EdgeItor eItor;
	Graph::Edge e;
	int numOutgoingEdges;
	for (i = 0; i<num_vertices; i++)
	{
		cout << setw(2) << (char)(i + 'A') << " ";
		numOutgoingEdges = pAdjLstArray[i].size();
		eItor = pAdjLstArray[i].begin();
		while (eItor != pAdjLstArray[i].end())
		{
			e = *eItor;
			cout << e << " ";
			eItor++;
		}
		cout << endl;
	}
}