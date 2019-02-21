// EthernetLayer.h: interface for the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ETHERNETLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
#define AFX_ETHERNETLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"

class CEthernetLayer
	: public CBaseLayer
{
private:
	inline void		ResetHeader();

public:
	BOOL			Receive(unsigned char* ppayload);
	BOOL			Send(unsigned char* ppayload, int nlength);
	void			SetDestinAddress(unsigned char* pAddress);
	void			SetSourceAddress(unsigned char* pAddress);
	unsigned char*	GetDestinAddress();
	unsigned char*	GetSourceAddress();

	CEthernetLayer(char* pName);
	virtual ~CEthernetLayer();



	typedef struct _ETHERNET {

		unsigned char	enet_dstaddr[6];		// ���� �ּ�(6����Ʈ)
		unsigned char	enet_srcaddr[6];		// �ڱ� �ּ�(6����Ʈ)
		unsigned short	enet_type;				// frame data type , 2
		unsigned char	enet_data[ETHER_MAX_DATA_SIZE]; // data �κ��� 46-1500�� �Ǿ�� �ϰ� �ִ�� 1500�̴�.
	} _ETHERNET, *PETHERNET;

	_ETHERNET	m_sHeader;
};

#endif // !defined(AFX_ETHERNETLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
