/** main.cpp */
#include <iostream> // for cin, cout
#include <fstream> // for file I/O
#include <iomanip>
#include "Packet.hpp"
#include "DataLink.hpp"
#include <windows.h> // for critical sections
#include <time.h>
#include <conio.h> // for getch()
#include <cstdlib> // for exit()
#include <string>
#include "Graph.hpp"
#include "BreadthFirstSearch.hpp"

typedef Graph::Vertex Vertex;
typedef Graph::Edge Edge;
typedef std::list<Graph::Vertex> VtxList;
typedef std::list<Graph::Edge> EdgeList;
typedef std::list<Graph::Vertex>::iterator VtxItor;
typedef std::list<Graph::Edge>::iterator EdgeItor;

using namespace std;

#define MAX_QUEUE_SIZE 10
enum ROLE { PACKET_ROUTER, PACKET_GEN, PACKET_RECV, LINK_TX, LINK_RX };

DWORD WINAPI Thread_PacketRouter(LPVOID pParam);

typedef struct ThreadParam
{
	int id;
	CRITICAL_SECTION *pCS;
	DataLink ***pppDL;
	BreadthFirstSearch* pNetTopology;
	int num_queue;
	int role; //
	UINT_32 addr;
	int max_queue;
	int* pNum_total_received;
	int *pTotal_packet_TX;
	ostream *pFout;
	ThreadParam() {}
} ThreadParam;

void main()
{
	ofstream fout;
	int total_packet_TX = 0;
	int num_nodes, numDataLinks;
	static int num_total_received = 0; // num of packets received by all routers

	fout.open("output.txt");

	if (fout.fail())
	{
		cout << "Fail in opening output.txt file.\n";
		exit(1);
	}
#define NUM_NODES 5
#define NUM_EDGES 8
	// Test simple network with five nodes
	Vertex v[NUM_NODES] =
	{ Vertex("A", 0, UN_VISITED),
		Vertex("B", 1, UN_VISITED),
		Vertex("C", 2, UN_VISITED),
		Vertex("D", 3, UN_VISITED),
		Vertex("E", 4, UN_VISITED)
	};

	Graph::Edge edges[NUM_EDGES] =
	{ Edge(v[0], v[1], 10), Edge(v[1], v[0], 10),
		Edge(v[1], v[2], 10), Edge(v[2], v[1], 10),
		Edge(v[2], v[3], 10), Edge(v[3], v[2], 10),
		Edge(v[3], v[4], 10), Edge(v[4], v[3], 10)
	};

	int start_node = 2;

	Graph simpleGraph(NUM_NODES);

	cout << "Inserting vertices .." << endl;
	for (int i = 0; i < NUM_NODES; i++) {
		simpleGraph.insertVertex(v[i]);
	}

	VtxList vtxLst;
	simpleGraph.vertices(vtxLst);

	int count = 0;
	cout << "Inserted vertices: ";
	for (VtxItor vItor = vtxLst.begin(); vItor != vtxLst.end(); ++vItor) {
		cout << *vItor << ", ";
	}
	cout << endl;

	cout << "Inserting edges .." << endl;
	for (int i = 0; i < NUM_EDGES; i++)
	{
		simpleGraph.insertEdge(edges[i]);
	}

	cout << "Inserted edges: " << endl;
	count = 0;
	EdgeList egLst;
	simpleGraph.edges(egLst);
	for (EdgeItor p = egLst.begin(); p != egLst.end(); ++p)
	{
		count++;
		cout << *p << ", ";
		if (count % 5 == 0)
			cout << endl;
	}
	cout << endl;

	cout << "Print out Graph based on Adjacency List .." << endl;
	simpleGraph.printGraph();

	num_nodes = simpleGraph.getNumVertices();
	numDataLinks = egLst.size();

	/* ==========================================*/
	BreadthFirstSearch networkTopology(simpleGraph);
	//VtxList path;
	//cout << "\nFinding shortest paths from node 0" << endl;
	//networkTopology.ShortestPathsTree(v[0]);
	cout << "\nFinding shortest paths from start node (" << start_node << ")" << endl;
	networkTopology.ShortestPathsTree(v[start_node]);
	/* ==========================================*/
	DataLink ***pppDL;
	int **ppDistMtrx;
	ThreadParam *pThrParam, threadParam;
	CRITICAL_SECTION crit;
	HANDLE hThreadPktRouter[NUM_NODES];

	networkTopology.initialize();
	networkTopology.initDistMtrx();
	pppDL = new DataLink**[num_nodes];
	for (int i = 0; i < num_nodes; i++)
	{
		pppDL[i] = new DataLink*[num_nodes];
	}
	ppDistMtrx = networkTopology.getppDistMtrx();

	for (int i = 0; i < num_nodes; i++) {
		for (int j = 0; j < num_nodes; j++)
		{
			if (i == j)
			{
				pppDL[i][j] = new DataLink();
			}
			else if (ppDistMtrx[i][j] == PLUS_INF) {
				pppDL[i][j] = NULL;
			}
			else {
				pppDL[i][j] = new DataLink();
			}
		}
	}
	/* ==========================================*/
	InitializeCriticalSection(&crit);

	EnterCriticalSection(&crit);
	cout << "==== Generating " << num_nodes << " threads ... ==== " << endl;
	for (int node_id = 0; node_id < num_nodes; node_id++)
	{
		pThrParam = new ThreadParam;
		pThrParam->id = node_id;
		pThrParam->addr = (UINT_32)node_id;
		pThrParam->pNetTopology = new BreadthFirstSearch(simpleGraph);
		pThrParam->role = PACKET_ROUTER;
		pThrParam->pCS = &crit;
		pThrParam->pppDL = pppDL;
		pThrParam->max_queue = MAX_QUEUE_SIZE;
		pThrParam->pFout = &cout;
		pThrParam->pNum_total_received = &num_total_received;
		hThreadPktRouter[node_id] =
			CreateThread(NULL, 0, Thread_PacketRouter, pThrParam, 0, NULL);
	}
	LeaveCriticalSection(&crit);

	EnterCriticalSection(&crit);
	cout << "\n======== main() is waiting threads to terminate === " << endl;
	LeaveCriticalSection(&crit);

	DWORD nExitCode = NULL;
	for (int i = 0; i < num_nodes; i++)
	{
		WaitForSingleObject(hThreadPktRouter[i], INFINITE);
		GetExitCodeThread(hThreadPktRouter[i], &nExitCode);
		TerminateThread(hThreadPktRouter[i], nExitCode);
		CloseHandle(hThreadPktRouter[i]);
	}
	DeleteCriticalSection(&crit);
	fout.close();
}

DWORD WINAPI Thread_PacketRouter(LPVOID pParam)
{
	ThreadParam *pThrParam;
	BreadthFirstSearch* pNetTopology;
	DataLink ***pppDL;
	Vertex *pVrtx, *pMyVrtx;
	int myNetAddr;
	Packet *pPkt;
	DataLink* pDL;
	UINT_32 srcAddr;
	ostream *pFout;
	int next_node;
	int dst;
	int *pPrev;
	int *pForwardTable;
	int num_nodes;
	int num_packets_generated = 0;
	int *pTotal_received;


	pThrParam = (ThreadParam *)pParam;
	myNetAddr = pThrParam->addr;
	pNetTopology = pThrParam->pNetTopology;
	pFout = pThrParam->pFout;
	pppDL = pThrParam->pppDL;
	pTotal_received = pThrParam->pNum_total_received;

	EnterCriticalSection(pThrParam->pCS);
	cout << " ******* Thread_PacketRouter [" << myNetAddr << "] *******" << endl;
	LeaveCriticalSection(pThrParam->pCS);

	srand(time(0) + pThrParam->addr);
	pNetTopology->initialize();
	pNetTopology->initDistMtrx();
	pVrtx = pNetTopology->getGraph().getpVrtxArray();
	pMyVrtx = &(pVrtx[myNetAddr]);
	EnterCriticalSection(pThrParam->pCS);
	pNetTopology->ShortestPathsTree(*pMyVrtx);
	pPrev = pNetTopology->getpParent();
	num_nodes = pNetTopology->getGraph().getNumVertices();

	// initialize packet forwarding table
	EnterCriticalSection(pThrParam->pCS);
	pForwardTable = new int[num_nodes];
	LeaveCriticalSection(pThrParam->pCS);
	for (dst = 0; dst < num_nodes; dst++)
	{
		int nxt;
		nxt = dst;
		while (pPrev[nxt] != myNetAddr) {
			nxt = pPrev[nxt];
		}

		if (pPrev[nxt] == myNetAddr)
		{
			pForwardTable[dst] = nxt;
		}
		else
		{
			cout << "Error in packet forwarding : destination (" << dst << ") is not reachable from this node (" << myNetAddr << "!!" << endl;
		}
	} // end for

	  /*
	  for (dst=0; dst<num_nodes; dst++)
	  {
	  cout << "node [" << myNetAddr << "] forwardTable [ dst: " << dst << "] : "
	  << pForwardTable[dst] << endl;
	  }
	  */
	LeaveCriticalSection(pThrParam->pCS);

	// packet generations as source node
	for (int i = 0; i < num_nodes; i++)
	{
		dst = i % num_nodes;
		pPkt = new Packet(myNetAddr, dst, i);

		pPkt->pushRouteNode(myNetAddr); // record route
		next_node = pForwardTable[dst];
		EnterCriticalSection(pThrParam->pCS);
		DataLink* pDL = pThrParam->pppDL[myNetAddr][next_node];
		if (pDL != NULL)
			pDL->enqueue(*pPkt);
		else
			cout << " Error : pppDL[][] is NULL" << endl;

		*pFout << "Router[" << myNetAddr << "]: (";
		*pFout << num_packets_generated << ")-th packet (";
		*pFout << pPkt->getSrcAddr() << " -> " << pPkt->getDstAddr();
		*pFout << ", seqNo: " << pPkt->getSeqNo() << ")\n";
		LeaveCriticalSection(pThrParam->pCS);

		num_packets_generated++;
		Sleep(1);
	} // end for( ; ; ) for generation of packets

	EnterCriticalSection(pThrParam->pCS);
	cout << "=== Router[" << myNetAddr << "] generated total " << num_packets_generated << " packets " << endl;

	/*
	for (int i = 0; i < num_nodes; i++)
	{
	cout << "pForwardTable [" << i << "] = " << pForwardTable[i] << endl;
	}
	*/
	LeaveCriticalSection(pThrParam->pCS);


	// packet forwarding as transit node and packet processing as destination node
	int source;
	int total_processed = 0;
	int received_by_this_node = 0;
	int hop_count;

	while ((*pTotal_received < (num_nodes * num_nodes))) //&& (received_by_this_node < num_nodes)
	{
		for (int src = 0; src < num_nodes; src++)
		{
			// checking data links and forwarding/handling packets
			pDL = pppDL[src][myNetAddr];
			if (pDL == NULL)
				continue;

			EnterCriticalSection(pThrParam->pCS);
			Packet newPkt = pDL->front(), *pNewPkt = &newPkt;
			if (pNewPkt->getSeqNo() == -1)
			{
				LeaveCriticalSection(pThrParam->pCS);
				continue;
			}
			pDL->dequeue();
			LeaveCriticalSection(pThrParam->pCS);
			pNewPkt->pushRouteNode(myNetAddr);

			if (pNewPkt->getDstAddr() == myNetAddr)
			{
				EnterCriticalSection(pThrParam->pCS);
				cout << "	Router[" << myNetAddr << "] received a packet from [" << pNewPkt->getSrcAddr() << "] to [" << pNewPkt->getDstAddr() << "], route : ";
				for (int i = 0; i < pNewPkt->getHopCount(); )
				{
					cout << pNewPkt->getRouteNodes(i);
					if (i++ < pNewPkt->getHopCount() - 1)
						cout << " -> ";
				}
				cout << endl;
				cout << "	Router[" << myNetAddr << "] currently received " << ++received_by_this_node << "packets." << endl;
				*pTotal_received = *pTotal_received + 1;
				LeaveCriticalSection(pThrParam->pCS);
			}
			else
			{
				next_node = pForwardTable[pNewPkt->getDstAddr()];
				DataLink* pSDL = pThrParam->pppDL[myNetAddr][next_node];

				EnterCriticalSection(pThrParam->pCS);
				if (pSDL != NULL)
					pSDL->enqueue(*pNewPkt);
				else
					cout << " Error : pppDL[][] is NULL" << endl;

				cout << "			#Router[" << myNetAddr << "] forwarded a packet from [" << pNewPkt->getSrcAddr() << "] to [" << pNewPkt->getDstAddr() << "], route : ";
				for (int i = 0; i < pNewPkt->getHopCount(); )
				{
					cout << pNewPkt->getRouteNodes(i);
					if (i++ < pNewPkt->getHopCount() - 1)
						cout << " -> ";
				}
				cout << endl;
				LeaveCriticalSection(pThrParam->pCS);
			}
			Sleep(100);
		} // end for
	} // end while

	EnterCriticalSection(pThrParam->pCS);
	*pFout << ">>> Router[" << myNetAddr << "] received total " << received_by_this_node << " packets ... terminating Thread...\n";
	LeaveCriticalSection(pThrParam->pCS);

	return 0;
}