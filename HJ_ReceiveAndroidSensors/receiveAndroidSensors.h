#ifndef _INC_RCVDROIDSENSORS
#define _INC_RCVDROIDSENSORS

// 2015/09/25

#include <fstream>
#include <Windows.h>
#include "Timer.h"

class rcvAndroidSensors
{
private:
	// メンバ変数
	// シリアル通信系
	int		COM;	//COMポート番号
	HANDLE	hComm;	//シリアルポートのハンドル

	// GPS系
	bool	isGetGPS;
	float	mLatitude , mLongitude , mAccuracy;	//	GPSから取得する緯度経度精度
	Timer	timerGPS;
	ofstream	ofsGPS;
	int		timeCountGPS;

	// 方位角系
	bool	isGetAttitude;
	float	mAzimuth, mPitch, mRoll;	// 姿勢
	Timer	timerAttitude;
	ofstream	ofsAttitude;
	int		timeCountAttitude;

	// その他
	int		minSaveInterval;


	// privateメソッド
	// シリアルポートを開く
	void	comOpen();
	// ポートを閉じる
	void	comClose();
public:
	//publicメソッド
	rcvAndroidSensors( int comport );
	~rcvAndroidSensors();

	// データの更新
	void	getSensorData();

	// データを取得するか設定
	void	setGPSData(bool isGetdata);
	void	setAttitudeData(bool isGetdata);

	// 保存の最低間隔[msec]
	void	setSaveMinInterval(int interval);

	// データを取得
	void	getGPSData(float retArray[3]);
	void	getAttitudeData(float retArray[3]);
	
};

#endif