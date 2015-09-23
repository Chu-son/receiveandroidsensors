#ifndef _INC_RCVDROIDSENSORS
#define _INC_RCVDROIDSENSORS

#include <Windows.h>

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

	// ���ʊp�n
	bool	isGetAttitude;
	float	mAzimuth, mPitch, mRoll;	// �p��

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

	// �f�[�^���擾
	void	getGPSData(float retArray[3]);
	void	getAttitudeData(float retArray[3]);
	
};

#endif