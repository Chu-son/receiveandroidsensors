#include <stdio.h>					// �W���w�b�_�[
#include <windows.h>				// Windows API�p�w�b�_�[
#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <string>

#include "receiveAndroidSensors.h"

#define COM_PORT		"COM12"		// �ʐM�|�[�g�̎w��
#pragma warning(disable : 4996)

using namespace std;


/*----------------------------------------------------------------------------*/
/*
*	�T�v�F�ʐM�|�[�g�����
*
*	�֐��FHANDLE CommClose( HANDLE hComm )
*	�����F
*		HANDLE			hComm		�ʐM�|�[�g�̃n���h��
*	�߂�l�F
*		1				����
*
*/
int CommClose(HANDLE hComm)
{
	if (hComm){
		CloseHandle(hComm);
	}

	return 1;
}

/*----------------------------------------------------------------------------*/
/*
*	�T�v�F�ʐM�|�[�g���J��
*
*	�֐��FHANDLE CommOpen( char *pport )
*	�����F
*		char			*pport		�ʐM�|�[�g��
*									�T�[�{�ƒʐM�\�ȃ|�[�g��
*	�߂�l�F
*		0				�ʐM�n���h���G���[
*		0�łȂ��l		����(�ʐM�p�n���h��)
*
*	�ʐM���x�́A115200bps�Œ�ł�
*
*/
HANDLE CommOpen(char *pport)
{
	HANDLE			hComm;		// �ʐM�p�n���h��
	DCB				cDcb;		// �ʐM�ݒ�p
	COMMTIMEOUTS	cTimeouts;	// �ʐM�|�[�g�^�C���A�E�g�p


	// �ʐM�|�[�g���J��
	string com = "\\\\.\\COM" + to_string(12);
	hComm = CreateFile(com.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	// �n���h���̊m�F
	if (hComm == INVALID_HANDLE_VALUE){
		hComm = NULL;
		LPVOID lpMsgBuf;
		FormatMessage(				//�G���[�\��������쐬
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf, 0, NULL);

		MessageBox(NULL, (const char*)lpMsgBuf, NULL, MB_OK);	//���b�Z�[�W�\��
		goto FuncEnd;
	}


	// �ʐM�ݒ�
	if (!GetCommState(hComm, &cDcb)){
		// �ʐM�ݒ�G���[�̏ꍇ
		printf("ERROR:GetCommState error\n");
		CommClose(hComm);
		hComm = NULL;
		goto FuncEnd;
	}
	cDcb.BaudRate = 115200;				// �ʐM���x
	cDcb.ByteSize = 8;					// �f�[�^�r�b�g��
	cDcb.fParity = TRUE;				// �p���e�B�`�F�b�N
	cDcb.Parity = NOPARITY;			// �m�[�p���e�B
	cDcb.StopBits = ONESTOPBIT;			// 1�X�g�b�v�r�b�g

	if (!SetCommState(hComm, &cDcb)){
		// �ʐM�ݒ�G���[�̏ꍇ
		printf("ERROR:GetCommState error\n");
		CommClose(hComm);
		hComm = NULL;
		goto FuncEnd;
	}


	// �ʐM�ݒ�(�ʐM�^�C���A�E�g�ݒ�)
	// �����̓ǂݍ��ݑ҂�����(ms)
	cTimeouts.ReadIntervalTimeout = 50;
	// �ǂݍ��݂̂P����������̎���(ms)
	cTimeouts.ReadTotalTimeoutMultiplier = 50;
	// �ǂݍ��݂̒萔����(ms)
	cTimeouts.ReadTotalTimeoutConstant = 50;
	// �������݂̂P����������̎���(ms)
	cTimeouts.WriteTotalTimeoutMultiplier = 0;

	if (!SetCommTimeouts(hComm, &cTimeouts)){
		// �ʐM�ݒ�G���[�̏ꍇ
		printf("ERROR:SetCommTimeouts error\n");
		CommClose(hComm);
		hComm = NULL;
		goto FuncEnd;
	}


	// �ʐM�o�b�t�@�N���A
	PurgeComm(hComm, PURGE_RXCLEAR);


FuncEnd:
	return hComm;
}

int getAndroidSensors(HANDLE hComm , float& latitude , float& longitude)
{
	unsigned char	readbuf[128];
	unsigned char	sum;
	int				i;
	int				ret;
	unsigned long	len, readlen;
	short			angle;
	float			azimuth, pitch, roll;
	float			accuracy ;
	float			dLatitude, dLongitude;


	// �n���h���`�F�b�N
	if (!hComm){
		return -1;
	}

	// �ǂݍ���
	memset(readbuf, 0x00, sizeof(readbuf));
	readlen = 11;
	len = 0;

	ret = ReadFile(hComm, readbuf, readlen, &len, NULL);

	//printf("\t< %d > datas! \n",len);

	if (readbuf[0] == 1){
		//�f�[�^����
		dLatitude = (readbuf[1] << 16) + (readbuf[2] << 8) + readbuf[3];
		dLongitude = (readbuf[4] << 16) + (readbuf[5] << 8) + readbuf[6];
		accuracy = (readbuf[7] << 8) + readbuf[8];

		//�������킹
		if ((readbuf[9] & 1) > 0) dLatitude = -dLatitude;
		if ((readbuf[9] & 2) > 0) dLongitude = -dLongitude;

		dLatitude /=  100000;
		dLongitude /= 100000;
		accuracy /= 10;

		latitude += dLatitude;
		longitude += dLongitude;

		printf("--GPS�ψ�--\n %.6f , %.6f \n", dLatitude, dLongitude);
		printf("--GPS��Βl--\n %.6f , %.6f , %.1f \n\n", latitude, longitude, accuracy);
	}
	else if (readbuf[0] == 2){
		azimuth = (readbuf[1] << 8 ) + readbuf[2];
		pitch = (readbuf[3] << 8) + readbuf[4];
		roll = (readbuf[5] << 8) + readbuf[6];

		azimuth = azimuth / 100 - 180;
		pitch = pitch / 100 - 180;
		roll = roll / 100 - 180;

		printf("--orientation--\n %.2f , %.2f , %.2f \n", azimuth, pitch, roll);
	}
	else if (readbuf[0] == 3){
		latitude = (readbuf[1] << 24) + (readbuf[2] << 16) + (readbuf[3] << 8) + readbuf[4];
		longitude = (readbuf[5] << 24) + (readbuf[6] << 16) + (readbuf[7] << 8) + readbuf[8];
		accuracy = (readbuf[9] << 8) + readbuf[10];

		latitude = latitude / 1000000;
		longitude = longitude / 1000000;
		accuracy = accuracy / 10;

		printf("--firstGPS--\n %.6f , %.6f , %.1f \n\n", latitude, longitude, accuracy);
	}


	return 0;
}

void unkomain()
{
	HANDLE		hComm = NULL;		// �ʐM�p�n���h��
	int			ret = 0;			// ���^�[���p

	float latitude = 0, longitude = 0;

	char z;

	// �ʐM�|�[�g���J��
	printf("COM PORT OPEN [%s]\n", COM_PORT);
	hComm = CommOpen(COM_PORT);

	if (!hComm){
		printf("ERROR:Com port open error\n");
	}

	while (true)
		ret = getAndroidSensors(hComm,latitude,longitude);

	CommClose(hComm);

	z = getchar();
}

void main()
{
	float dummy[3] = { 0 };

	rcvAndroidSensors rcvDroid(14);

	while (true)
	{
		//rcvDroid.getSensorData();
		rcvDroid.getAttitudeData(dummy);
		Sleep(2000);
	}

}