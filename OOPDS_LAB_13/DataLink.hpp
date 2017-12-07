/* DataLink.h */
#ifndef DATALINK_HPP
#define DATALINK_HPP
#include <queue>
#include <windows.h>
#include "Packet.hpp"
using namespace std;
class DataLink {
private:
	CRITICAL_SECTION queue_mutex; // Mutex for queue input & output operations
	queue<Packet> pktQ; // Packet Queue
public:
	DataLink(void) { // Constructor
		InitializeCriticalSection(&queue_mutex);
	}
	~DataLink(void) { // Destructor
					  // Nothing to do here
	}
	// Try to put a single message into the mailbox
	void enqueue(Packet pkt) {
		EnterCriticalSection(&queue_mutex); // Try to lock down queue
											// int currentSize = pktQ.size(); // for test only
		pktQ.push(pkt); // Push message into mailbox
		LeaveCriticalSection(&queue_mutex); // Unlock queue
	}
	void dequeue() {
		EnterCriticalSection(&queue_mutex);
		// Indicate if mailbox is empty
		if (pktQ.empty()) {
			LeaveCriticalSection(&queue_mutex);
			return;
		}
		else { // Otherwise, grab the message
			pktQ.pop(); // Pop of least recent message
		}
		LeaveCriticalSection(&queue_mutex); // Unlock queue
	}
	// Peek at single message from a mailbox
	void front(Packet *pPkt) {
		EnterCriticalSection(&queue_mutex); // Lock down queue
		if (pktQ.empty()) { // Wait for a message to come in
			pPkt = NULL;
			LeaveCriticalSection(&queue_mutex);
			return;
		}
		*pPkt = pktQ.front(); // Peek at most recent message
		LeaveCriticalSection(&queue_mutex); // Unlock queue
	}
	bool empty()
	{
		int size = pktQ.size();
		if (size == 0)
			return true;
		else
			return false;
	}
};
#endif