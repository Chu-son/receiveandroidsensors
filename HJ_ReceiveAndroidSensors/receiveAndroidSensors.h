#ifndef _INC_RCVDROIDSENSORS
#define _INC_RCVDROIDSENSORS

// 2015/09/25

#include <fstream>
#include <Windows.h>
#include "Timer.h"

class rcvAndroidSensors
{
private:
	// �����o�ϐ�
	// �V���A���ʐM�n
	int		COM;	//COM�|�[�g�ԍ�
	HANDLE	hComm;	//�V���A���|�[�g�̃n���h��

	// GPS�n
	bool	isGetGPS;
	float	mLatitude , mLongitude , mAccuracy;	//	GPS����擾����ܓx�o�x���x
	Timer	timerGPS;
	ofstream	ofsGPS;
	int		timeCountGPS;

	// ���ʊp�n
	bool	isGetAttitude;
	float	mAzimuth, mPitch, mRoll;	// �p��
	Timer	timerAttitude;
	ofstream	ofsAttitude;
	int		timeCountAttitude;

	// ���̑�
	int		minSaveInterval;


	// private���\�b�h
	// �V���A���|�[�g���J��
	void	comOpen();
	// �|�[�g�����
	void	comClose();
public:
	//public���\�b�h
	rcvAndroidSensors( int comport );
	~rcvAndroidSensors();

	// �f�[�^�̍X�V
	void	getSensorData();

	// �f�[�^���擾���邩�ݒ�
	void	setGPSData(bool isGetdata);
	void	setAttitudeData(bool isGetdata);

	// �ۑ��̍Œ�Ԋu[msec]
	void	setSaveMinInterval(int interval);

	// �f�[�^���擾
	void	getGPSData(float retArray[3]);
	void	getAttitudeData(float retArray[3]);
	
};

#endif