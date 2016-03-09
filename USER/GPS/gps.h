#ifndef __GPS_H
#define __GPS_H

#include "stm32f10x.h"

#define GPS_BUF_LEN	100
#define COMMA_MAX_POSITION	255

typedef enum _GPS_DATA_TYPE{
	GPGGA = 0,		// GPS ��λ��Ϣ
	GPRMC,				// �Ƽ���С��λ��Ϣ
	GPVTG,				// �����ٶ���Ϣ
	GPGLL,				// ����λ��Ϣ
	GPGSA,				// ��ǰ������Ϣ
	GPGSV				// �ɼ�������Ϣ
} GPS_DATA_TYPE;

// ��γ�ȱ�ʾ��ʽ
typedef enum _LATI_LONG_FORMAT{
	DEFAULT = 0,
	DMS_F,			// �ȷ���
	DMM_F,			// �ȷ�
	DDD_F				// ��
} LATI_LONG_FORMAT;

typedef struct _GPS_DATA_BUF{
	char buf[GPS_BUF_LEN+1];
	uint8_t index;
} GPS_DATA_BUF;

typedef struct _GPS_FLAG{
	uint8_t gpsAnalysis;		// GPS ���ݷ�����
	uint8_t gpsReceiver;		// GPS ���ݽ�����
	uint8_t gpsStop;			// GPS ���ݽ��ս���
} GPS_FLAG, *pGPS_GLAG;

typedef struct _DATA_TIME{
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} DATE_TIME;

// �ȷ����ʽ��ʾ��γ��, ddd.mm.ss
typedef struct _DMS{
	double latitude;
	double longitude;
	
	uint8_t 	latitudeDegree;		// ��			
	uint8_t		latitudeCent;		// ��
	uint8_t		latitudeSecond;		// ��
	uint8_t		longitudeDegree;
	uint8_t		longitudeCent;
	uint8_t 	longitudeSecond; 
} DMS;

// �Ⱥ�ʮ���Ʒ�, ddd.mm.mmm
typedef struct _DMM{
	char* latitude;
	char* longitude;
} DMM;

// С������, ddd.ddddd
typedef struct _DDD{
	double latitude;
	double longitude;
	
	uint8_t latitudeIntegers;			// �ȵ�����λ
	float 	latitudeDecimals;			// �ȵ�С��λ
	uint8_t longitudeIntegers;
	float		longitudeDecimals;
} DDD;

typedef struct _GPS_INFO{
	char			latitude[11];			// �ַ�����ʾ�ľ���
	char			longitude[12];			// �ַ�����ʾ��γ��							
	float			speed;					// �ٶ�
	float			direction;				// ����
	float			heightGround;			// ˮƽ��߶�
	float			heightSea;				// ���θ߶�
	char			NS[2];					// γ�Ȱ��򣬱�γ����γ
	char			EW[2];					// ���Ȱ��򣬶���������
	DATE_TIME	DT;							// ���ں�ʱ��
} GPS_INFO;


uint8_t parseGpsData(char* buffer, GPS_INFO* gps, GPS_DATA_TYPE type);
uint8_t outputLatitudeLongitude(GPS_INFO* gps, LATI_LONG_FORMAT format);
uint8_t getComma(uint8_t position, char* str);

static double str2Double(char* string);
static void getStringNumber(char* str, char* retval);
static double getDoubleNumber(char* str);
static void UTC2BTC(DATE_TIME* gpsDT);
static char *gcvt(double value, int ndigit, char *buf);
static char* itoa(int value,char*string,int radix);

uint8_t isGpsAnalysising(void);
uint8_t isGpsReceivering(void);
uint8_t isGpsStoping(void);
void cleanGpsFlag(GPS_FLAG* gpsFlag);
void cleanGpsDataBuf(char* gpsDataBuf);

#endif
