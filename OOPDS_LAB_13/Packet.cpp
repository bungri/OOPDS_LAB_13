#include "Packet.hpp"

Packet::Packet(UINT_32 srcAddr, UINT_32 dstAddr, UINT_32 sN)
{
	this->srcAddr = srcAddr;
	this->dstAddr = dstAddr;
	this->seqNo = sN;
	this->protocol = 0;
	this->payloadLength = 0;
	this->hop_count = 0;
}

ostream & Packet::printSaddrSeqNo(ostream * pFout)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *pFout;
}

ostream & Packet::printPacket(ostream * pFout)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *pFout;
}
