#include "Packet.hpp"

Packet::Packet(UINT_32 srcAddr, UINT_32 dstAddr, UINT_32 sN)
{
	this->srcAddr = srcAddr;
	this->dstAddr = dstAddr;
	this->seqNo = sN;
}

ostream & Packet::printSaddrSeqNo(ostream * pFout)
{
	// TODO: ���⿡ ��ȯ ������ �����մϴ�.
	return *pFout;
}

ostream & Packet::printPacket(ostream * pFout)
{
	// TODO: ���⿡ ��ȯ ������ �����մϴ�.
	return *pFout;
}
