

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
	static UINT		FileThread(LPVOID pParam);	// FileApp 쓰레드
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
		unsigned long fapp_tollen;// 파일 총 길이(4바이트)
		unsigned short fapp_type;//단편화되는 데이터 타입(2바이트)
		unsigned char fapp_msg_type; //메시지 종류(데이터 or 응답)(1바이트)
		unsigned char ed; //사용안함(1바이트)
		unsigned long fapp_seq_num; //단편화되는 일련번호(4바이트)
		unsigned char fapp_data[FILE_APP_DATA_SIZE]; //데이터(권장 1448바이트)

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





