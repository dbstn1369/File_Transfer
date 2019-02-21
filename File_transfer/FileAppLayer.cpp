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
	path = (CString)ppayload; // ���� ��� ����.
	return bSuccess;

}

BOOL CFileAppLayer::StartFileThread()
{
	BOOL bSuccess = FALSE;
	bThread = TRUE;//���� ���� ������ ����ġ ��.
	AfxBeginThread(FileThread, this);//���� ���� ������ ����.
	return bSuccess;
}

BOOL CFileAppLayer::Receive(unsigned char* ppayload)
{//buffer 
	LPFILE_APP pFrame = (LPFILE_APP)ppayload;
	BOOL bSuccess = FALSE;
	unsigned long dataSize ;
	
	int frag_count;
	
	if (pFrame->fapp_type == 0 && pFrame->fapp_msg_type == 0 ) {//fapp_type == 0 ���ϸ� , totlen 
		dataSize = pFrame->fapp_tollen;
		frag_count = (dataSize / FILE_APP_DATA_SIZE);
		u_char* fileName= pFrame->fapp_data;
		fp = fopen((char*)fileName, "wb");
		if (fp == NULL) {
			AfxMessageBox("FILE OPEN ERROR");
		}
		//2���� �迭 �����-> fapp_tollen ��ŭ�� �迭 : ������ �ӽ������� �����ϱ� ���ؼ�
		file_buff = (u_char**)malloc(sizeof(u_char*)*(frag_count+1));
		for (int i = 0; i < (frag_count + 1); i++) {
			file_buff[i] = (u_char*)malloc(sizeof(u_char)*FILE_APP_DATA_SIZE);
			memset(file_buff[i], 0, FILE_APP_DATA_SIZE);
		}
		// sender���� ACK ������.
		bThread = TRUE;// �޴� ���α׷����� ������ ����.
		bR_ack=TRUE;
	
		return bSuccess;
	}
	if (pFrame->fapp_type == 1 && pFrame->fapp_msg_type == 0) {

	/*	if (pFrame->fapp_seq_num-iNAK>1) {
			//������
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
			if (i < frag_count - 1) { //�߰����� ������
				size = FILE_APP_DATA_SIZE;
			}
			else {//������ �����͸� ��Ÿ���� type ����
				size = dataSize % FILE_APP_DATA_SIZE;
			}
			fwrite(file_buff[i], size, 1, fp);//fp�� file_buff �־��ֱ�.
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

	CFileAppLayer *cFile = (CFileAppLayer*)pParam;//���Ͼ۷��̾� ��ü ��������.

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
				dwFILENSZ = fileName.GetLength(); // ���ϸ� ����
				fseek(file_pointer, 0L, SEEK_END);
				sz = ftell(file_pointer);//���� ��ü ������
				rewind(file_pointer);
				dataleng = (sz / FILE_APP_DATA_SIZE) + 1;// ����ȭ ������ ����.
			}
			//cFile->bS_first = FALSE;
			// data_type == 0
			//ó�� ������� ���ϸ� ������
			

			FILE_APP_HEADER f_sHeader;
			f_sHeader.fapp_tollen = sz;
			f_sHeader.fapp_type = 0;
			f_sHeader.fapp_msg_type = 0;
			f_sHeader.fapp_seq_num = 0;
			
			memset(f_sHeader.fapp_data, 0, FILE_APP_DATA_SIZE);
			memcpy(f_sHeader.fapp_data, fileName, dwFILENSZ); // ���ϸ� ����
			cFile->mp_UnderLayer->Send((unsigned char*)&f_sHeader, FILE_APP_HEADER_SIZE + dwFILENSZ);
			
		
		}
		if (cFile->bS_second) {
			cFile->bS_second = FALSE;

			FILE_APP_HEADER f_sHeader;

			if (sz > FILE_APP_DATA_SIZE) {
				//MTU ���� tollen�� Ŭ ��
				f_sHeader.fapp_tollen = FILE_APP_DATA_SIZE;
				f_sHeader.fapp_type = 1;
				f_sHeader.fapp_msg_type = 0;
				for (int i = 0; i < dataleng; i++) {
					int size;
					if (i < dataleng-1) { //�߰����� ������
						size = FILE_APP_DATA_SIZE;
					}
					else {//������ �����͸� ��Ÿ���� type ����
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
			else {// MTU ���� tollen�� ���� ��
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