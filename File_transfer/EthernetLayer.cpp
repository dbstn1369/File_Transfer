// EthernetLayer.cpp: implementation of the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "File_transfer.h"
#include "EthernetLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEthernetLayer::CEthernetLayer(char* pName)
	: CBaseLayer(pName)
{
	ResetHeader();
}

CEthernetLayer::~CEthernetLayer()
{
}

void CEthernetLayer::ResetHeader()
{
	memset(m_sHeader.enet_dstaddr, 0, 6);
	memset(m_sHeader.enet_srcaddr, 0, 6);
	memset(m_sHeader.enet_data, ETHER_MAX_DATA_SIZE, 6);
	m_sHeader.enet_type = 0;
}

unsigned char* CEthernetLayer::GetSourceAddress()
{
	return m_sHeader.enet_srcaddr;
}

unsigned char* CEthernetLayer::GetDestinAddress()
{

	return m_sHeader.enet_dstaddr; // <---- 이부분은 수정해야함
}

void CEthernetLayer::SetSourceAddress(unsigned char *pAddress)
{
	//////////////////////// fill the blank ///////////////////////////////
	memcpy(m_sHeader.enet_srcaddr, pAddress, 6);
	///////////////////////////////////////////////////////////////////////
}

void CEthernetLayer::SetDestinAddress(unsigned char *pAddress)
{
	memcpy(m_sHeader.enet_dstaddr, pAddress, 6);
}

BOOL CEthernetLayer::Send(unsigned char *ppayload, int nlength)
{
	memcpy(m_sHeader.enet_data, ppayload, nlength);

	BOOL bSuccess = FALSE;
	//////////////////////// fill the blank ///////////////////////////////
	m_sHeader.enet_type = 0x1254;
	bSuccess = mp_UnderLayer->Send((unsigned char*)&m_sHeader, nlength + ETHER_HEADER_SIZE);

	///////////////////////////////////////////////////////////////////////

	return bSuccess;
}

BOOL CEthernetLayer::Receive(unsigned char* ppayload)
{
	PETHERNET pFrame = (PETHERNET)ppayload;
	BOOL bSuccess = FALSE;

	unsigned char broadcast[6];
	memset(broadcast, 0xff, 6);

	//////////////////////// fill the blank ///////////////////////////////
	if (((memcmp(pFrame->enet_dstaddr, m_sHeader.enet_srcaddr, 6) == 0 && 
		memcmp(pFrame->enet_srcaddr, m_sHeader.enet_srcaddr, 6) != 0) || memcmp(pFrame->enet_dstaddr,(char *)broadcast,6) == 0) && pFrame->enet_type == ntohs(0x5412)) {
		bSuccess = mp_aUpperLayer[0]->Receive((unsigned char*)pFrame->enet_data);
	}

	
	///////////////////////////////////////////////////////////////////////

	return bSuccess;
}
