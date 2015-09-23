#ifndef _INC_RCVDROIDSENSORS
#define _INC_RCVDROIDSENSORS

#include <Windows.h>

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

	// 方位角系
	bool	isGetAttitude;
	float	mAzimuth, mPitch, mRoll;	// 姿勢

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

	// データを取得
	void	getGPSData(float retArray[3]);
	void	getAttitudeData(float retArray[3]);
	
};

#endif