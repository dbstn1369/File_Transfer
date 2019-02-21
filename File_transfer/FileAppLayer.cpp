#include "stdafx.h"
#include "FileAppLayer.h"


CFileAppLayer::CFileAppLayer(char* pName)
	:CBaseLayer(pName),
	mp_Dlg(NULL)
{
	path = "";
	bThread = FALSE;
	bS_first = FALSE;
	bS_second = FALSE;
	bS_third = FALSE;
	
	bR_ack = FALSE;
	iNAK = 0;
	ResetHeader();
}


CFileAppLayer::~CFileAppLayer()
{
}

void CFileAppLayer::ResetHeader()
{
	f_sHeader.fapp_tollen = 0;
	f_sHeader.fapp_type = 0;
	f_sHeader.fapp_msg_type = 0;
	f_sHeader.ed = 0;
	f_sHeader.fapp_seq_num = 0;
	memset(f_sHeader.fapp_data, 0, FILE_APP_DATA_SIZE);
}

BOOL CFileAppLayer::Send(unsigned char* ppayload, int nlength)
{
	BOOL bSuccess = FALSE;
	bThread = TRUE;
	bS_first = TRUE;
	path = (CString)ppayload; // 파일 경로 저장.
	return bSuccess;

}

BOOL CFileAppLayer::StartFileThread()
{
	BOOL bSuccess = FALSE;
	bThread = TRUE;//파일 전송 스레드 스위치 온.
	AfxBeginThread(FileThread, this);//파일 전송 스레드 시작.
	return bSuccess;
}

BOOL CFileAppLayer::Receive(unsigned char* ppayload)
{//buffer 
	LPFILE_APP pFrame = (LPFILE_APP)ppayload;
	BOOL bSuccess = FALSE;
	unsigned long dataSize ;
	
	int frag_count;
	
	if (pFrame->fapp_type == 0 && pFrame->fapp_msg_type == 0 ) {//fapp_type == 0 파일명 , totlen 
		dataSize = pFrame->fapp_tollen;
		frag_count = (dataSize / FILE_APP_DATA_SIZE);
		u_char* fileName= pFrame->fapp_data;
		fp = fopen((char*)fileName, "wb");
		if (fp == NULL) {
			AfxMessageBox("FILE OPEN ERROR");
		}
		//2차원 배열 만들기-> fapp_tollen 만큼의 배열 : 파일을 임시적으로 저장하기 위해서
		file_buff = (u_char**)malloc(sizeof(u_char*)*(frag_count+1));
		for (int i = 0; i < (frag_count + 1); i++) {
			file_buff[i] = (u_char*)malloc(sizeof(u_char)*FILE_APP_DATA_SIZE);
			memset(file_buff[i], 0, FILE_APP_DATA_SIZE);
		}
		// sender에게 ACK 보내기.
		bThread = TRUE;// 받는 프로그램에서 쓰레드 실행.
		bR_ack=TRUE;
	
		return bSuccess;
	}
	if (pFrame->fapp_type == 1 && pFrame->fapp_msg_type == 0) {

	/*	if (pFrame->fapp_seq_num-iNAK>1) {
			//재전송
			empty_c = pFrame->fapp_seq_num - iNAK;
		}
		else {
			iNAK = pFrame->fapp_seq_num;
			
		}*/
		
		if (pFrame->fapp_tollen < FILE_APP_DATA_SIZE) {
			memcpy(file_buff[pFrame->fapp_seq_num], pFrame->fapp_data, pFrame->fapp_tollen);
		}
		else {
			memcpy(file_buff[pFrame->fapp_seq_num], pFrame->fapp_data, FILE_APP_DATA_SIZE);
		}
	}

	if (pFrame->fapp_type == 2 && pFrame->fapp_msg_type == 0) {
		bThread = FALSE;
		bS_first = FALSE;
		bS_second = FALSE;
		bS_third = FALSE;
		
		bR_ack = FALSE;
		dataSize = pFrame->fapp_tollen;
		frag_count = (dataSize / FILE_APP_DATA_SIZE) + 1;
		for (int i = 0; i < frag_count; i++) {
			int size;
			if (i < frag_count - 1) { //중간파일 보내기
				size = FILE_APP_DATA_SIZE;
			}
			else {//마지막 데이터를 나타내는 type 설정
				size = dataSize % FILE_APP_DATA_SIZE;
			}
			fwrite(file_buff[i], size, 1, fp);//fp에 file_buff 넣어주기.
		}
		fclose(fp);
	}
	
	if (pFrame->fapp_type == 0 && pFrame->fapp_msg_type == 1) {
		bS_first = FALSE;
		bS_second = TRUE;
		bS_third = FALSE;
		bR_ack = FALSE;
	}
	
}

UINT CFileAppLayer::FileThread(LPVOID pParam) {

	CFileAppLayer *cFile = (CFileAppLayer*)pParam;//파일앱레이어 객체 가져오기.

	u_char* myFile;
	CString path = cFile->path;
	CString fileName = path;
	int dwFILENSZ = 0;
	unsigned long sz = 0;
	int dataleng = 0;
	FILE* file_pointer = NULL;


	while (cFile->bThread) {
		if (cFile->bS_first) {
			cFile->bS_first = FALSE;
			if (cFile->path.GetLength() > 1)	
			{
				file_pointer = fopen(cFile->path, "rb");
				fileName = cFile->path.Right(cFile->path.GetLength() - cFile->path.ReverseFind('\\') - 1);
				dwFILENSZ = fileName.GetLength(); // 파일명 길이
				fseek(file_pointer, 0L, SEEK_END);
				sz = ftell(file_pointer);//파일 전체 사이즈
				rewind(file_pointer);
				dataleng = (sz / FILE_APP_DATA_SIZE) + 1;// 단편화 데이터 갯수.
			}
			//cFile->bS_first = FALSE;
			// data_type == 0
			//처음 헤더에는 파일명 보내기
			

			FILE_APP_HEADER f_sHeader;
			f_sHeader.fapp_tollen = sz;
			f_sHeader.fapp_type = 0;
			f_sHeader.fapp_msg_type = 0;
			f_sHeader.fapp_seq_num = 0;
			
			memset(f_sHeader.fapp_data, 0, FILE_APP_DATA_SIZE);
			memcpy(f_sHeader.fapp_data, fileName, dwFILENSZ); // 파일명 저장
			cFile->mp_UnderLayer->Send((unsigned char*)&f_sHeader, FILE_APP_HEADER_SIZE + dwFILENSZ);
			
		
		}
		if (cFile->bS_second) {
			cFile->bS_second = FALSE;

			FILE_APP_HEADER f_sHeader;

			if (sz > FILE_APP_DATA_SIZE) {
				//MTU 보다 tollen이 클 때
				f_sHeader.fapp_tollen = FILE_APP_DATA_SIZE;
				f_sHeader.fapp_type = 1;
				f_sHeader.fapp_msg_type = 0;
				for (int i = 0; i < dataleng; i++) {
					int size;
					if (i < dataleng-1) { //중간파일 보내기
						size = FILE_APP_DATA_SIZE;
					}
					else {//마지막 데이터를 나타내는 type 설정
						size = sz % FILE_APP_DATA_SIZE;
					}
					myFile = (u_char*)malloc(sizeof(u_char)*size);
					memset(myFile, 0, size);
					for (int j = 0; j < size; j++) {
						char ch = fgetc(file_pointer);
						f_sHeader.fapp_data[j] = ch;
					}
					f_sHeader.fapp_seq_num = i;
					f_sHeader.fapp_tollen = size;
					cFile->mp_UnderLayer->Send((unsigned char*)&f_sHeader, FILE_APP_HEADER_SIZE + size);
					memset(f_sHeader.fapp_data, 0, FILE_APP_DATA_SIZE);
				}
			}
			else {// MTU 보다 tollen이 작을 때
				int size = sz;
				myFile = (u_char*)malloc(sizeof(u_char)*size);
				fgets((char*)myFile, FILE_APP_DATA_SIZE, file_pointer);
				memcpy(&f_sHeader.fapp_data, myFile, size);
				cFile->mp_UnderLayer->Send((unsigned char*)&f_sHeader, FILE_APP_HEADER_SIZE + sz);
				memset(f_sHeader.fapp_data, 0, FILE_APP_DATA_SIZE);
			}
			fclose(file_pointer);
			cFile->bS_third = TRUE;
		}
		if (cFile->bS_third) {
			cFile->bS_third = FALSE;
			FILE_APP_HEADER f_sHeader;
			f_sHeader.fapp_tollen = sz;
			f_sHeader.fapp_type = 2;
			f_sHeader.fapp_msg_type = 0;
			f_sHeader.fapp_seq_num = 0;

			cFile->mp_UnderLayer->Send((unsigned char*)&f_sHeader, FILE_APP_HEADER_SIZE);
		}
	
		if (cFile->bR_ack) {
			cFile->bR_ack = FALSE;
			FILE_APP_HEADER f_sHeader;
			f_sHeader.fapp_tollen = 0;
			f_sHeader.fapp_type = 0;
			f_sHeader.fapp_msg_type = 1;
			f_sHeader.fapp_seq_num = 0;
			cFile->mp_UnderLayer->Send((unsigned char*)&f_sHeader, FILE_APP_HEADER_SIZE);
		}

		Sleep(10);
	}

	return 0;
}