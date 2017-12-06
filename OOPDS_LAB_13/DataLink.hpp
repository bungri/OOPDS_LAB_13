/* DataLink.h */
#ifndef DATALINK_HPP
#define DATALINK_HPP
#include <queue>
#include <windows.h>
#include "Packet.hpp"
using namespace std;
class DataLink {
private:
	CRITICAL_SECTION queue_mutex; // Mutex for queue control
	queue<Packet> pktQ; // Messages
public:
	// Constructor
	DataLink(void) { /*. . . .*/ }
	// Destructor
	~DataLink(void) {
		// Nothing to do here
	}
	// Try to put a single message into the mailbox/queue
	void enqueue(Packet pkt) { /*. . . .*/ }
	void dequeue() { /*. . . .*/ }
	// Peek at single message from a mailbox/queue
	void front(Packet *pPkt) { /*. . . .*/ }
	bool empty() { /*. . . .*/ }
};
#endif