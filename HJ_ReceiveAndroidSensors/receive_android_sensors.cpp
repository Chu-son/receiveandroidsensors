#include <stdio.h>					// 標準ヘッダー
#include <windows.h>				// Windows API用ヘッダー
#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <string>

#include "receiveAndroidSensors.h"

#define COM_PORT		"COM12"		// 通信ポートの指定
#pragma warning(disable : 4996)

using namespace std;


/*----------------------------------------------------------------------------*/
/*
*	概要：通信ポートを閉じる
*
*	関数：HANDLE CommClose( HANDLE hComm )
*	引数：
*		HANDLE			hComm		通信ポートのハンドル
*	戻り値：
*		1				成功
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
*	概要：通信ポートを開く
*
*	関数：HANDLE CommOpen( char *pport )
*	引数：
*		char			*pport		通信ポート名
*									サーボと通信可能なポート名
*	戻り値：
*		0				通信ハンドルエラー
*		0でない値		成功(通信用ハンドル)
*
*	通信速度は、115200bps固定です
*
*/
HANDLE CommOpen(char *pport)
{
	HANDLE			hComm;		// 通信用ハンドル
	DCB				cDcb;		// 通信設定用
	COMMTIMEOUTS	cTimeouts;	// 通信ポートタイムアウト用


	// 通信ポートを開く
	string com = "\\\\.\\COM" + to_string(12);
	hComm = CreateFile(com.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	// ハンドルの確認
	if (hComm == INVALID_HANDLE_VALUE){
		hComm = NULL;
		LPVOID lpMsgBuf;
		FormatMessage(				//エラー表示文字列作成
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf, 0, NULL);

		MessageBox(NULL, (const char*)lpMsgBuf, NULL, MB_OK);	//メッセージ表示
		goto FuncEnd;
	}


	// 通信設定
	if (!GetCommState(hComm, &cDcb)){
		// 通信設定エラーの場合
		printf("ERROR:GetCommState error\n");
		CommClose(hComm);
		hComm = NULL;
		goto FuncEnd;
	}
	cDcb.BaudRate = 115200;				// 通信速度
	cDcb.ByteSize = 8;					// データビット長
	cDcb.fParity = TRUE;				// パリティチェック
	cDcb.Parity = NOPARITY;			// ノーパリティ
	cDcb.StopBits = ONESTOPBIT;			// 1ストップビット

	if (!SetCommState(hComm, &cDcb)){
		// 通信設定エラーの場合
		printf("ERROR:GetCommState error\n");
		CommClose(hComm);
		hComm = NULL;
		goto FuncEnd;
	}


	// 通信設定(通信タイムアウト設定)
	// 文字の読み込み待ち時間(ms)
	cTimeouts.ReadIntervalTimeout = 50;
	// 読み込みの１文字あたりの時間(ms)
	cTimeouts.ReadTotalTimeoutMultiplier = 50;
	// 読み込みの定数時間(ms)
	cTimeouts.ReadTotalTimeoutConstant = 50;
	// 書き込みの１文字あたりの時間(ms)
	cTimeouts.WriteTotalTimeoutMultiplier = 0;

	if (!SetCommTimeouts(hComm, &cTimeouts)){
		// 通信設定エラーの場合
		printf("ERROR:SetCommTimeouts error\n");
		CommClose(hComm);
		hComm = NULL;
		goto FuncEnd;
	}


	// 通信バッファクリア
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


	// ハンドルチェック
	if (!hComm){
		return -1;
	}

	// 読み込み
	memset(readbuf, 0x00, sizeof(readbuf));
	readlen = 11;
	len = 0;

	ret = ReadFile(hComm, readbuf, readlen, &len, NULL);

	//printf("\t< %d > datas! \n",len);

	if (readbuf[0] == 1){
		//データ復元
		dLatitude = (readbuf[1] << 16) + (readbuf[2] << 8) + readbuf[3];
		dLongitude = (readbuf[4] << 16) + (readbuf[5] << 8) + readbuf[6];
		accuracy = (readbuf[7] << 8) + readbuf[8];

		//符号合わせ
		if ((readbuf[9] & 1) > 0) dLatitude = -dLatitude;
		if ((readbuf[9] & 2) > 0) dLongitude = -dLongitude;

		dLatitude /=  100000;
		dLongitude /= 100000;
		accuracy /= 10;

		latitude += dLatitude;
		longitude += dLongitude;

		printf("--GPS変位--\n %.6f , %.6f \n", dLatitude, dLongitude);
		printf("--GPS絶対値--\n %.6f , %.6f , %.1f \n\n", latitude, longitude, accuracy);
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
	HANDLE		hComm = NULL;		// 通信用ハンドル
	int			ret = 0;			// リターン用

	float latitude = 0, longitude = 0;

	char z;

	// 通信ポートを開く
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