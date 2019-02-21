

#if !defined(AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)
#define AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"

class CFileAppLayer
	: public CBaseLayer
{
private:
	inline void		ResetHeader();
	CObject* mp_Dlg;
	

public:
	BOOL			Receive(unsigned char* ppayload);
	BOOL			Send(unsigned char* ppayload, int nlength);
	static UINT		FileThread(LPVOID pParam);	// FileApp ������
	CString			path;
	BOOL			bThread;
	BOOL			bS_first;
	BOOL			bS_second;
	BOOL			bS_third;
	BOOL			bS_forth;
	BOOL			bR_ack;
	int				iNAK;
	int				empty_c;

	CFileAppLayer(char* pName);
	virtual ~CFileAppLayer();

	typedef struct _FILE_APP_HEADER {
		unsigned long fapp_tollen;// ���� �� ����(4����Ʈ)
		unsigned short fapp_type;//����ȭ�Ǵ� ������ Ÿ��(2����Ʈ)
		unsigned char fapp_msg_type; //�޽��� ����(������ or ����)(1����Ʈ)
		unsigned char ed; //������(1����Ʈ)
		unsigned long fapp_seq_num; //����ȭ�Ǵ� �Ϸù�ȣ(4����Ʈ)
		unsigned char fapp_data[FILE_APP_DATA_SIZE]; //������(���� 1448����Ʈ)

	} FILE_APP_HEADER, *LPFILE_APP;

	
	FILE *fp;
	u_char** file_buff;
	BOOL	StartFileThread();

protected:
	FILE_APP_HEADER		f_sHeader;

	enum {
		DATA_MSG_frag = 0x00,
		DATA_MSG_ack = 0x02,
		DATA_MSG_err = 0x03,


		DATA_TYPE_inf = 0x00,
		DATA_TYPE_org = 0x02,
		DATA_TYPE_end = 0x03


	};
};

#endif // !defined(AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)





