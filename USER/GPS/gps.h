#ifndef __GPS_H
#define __GPS_H

#include "stm32f10x.h"

#define GPS_BUF_LEN	100
#define COMMA_MAX_POSITION	255

typedef enum _GPS_DATA_TYPE{
	GPGGA = 0,		// GPS 定位信息
	GPRMC,				// 推荐最小定位信息
	GPVTG,				// 地面速度信息
	GPGLL,				// 地理定位信息
	GPGSA,				// 当前卫星信息
	GPGSV				// 可见卫星信息
} GPS_DATA_TYPE;

// 经纬度表示格式
typedef enum _LATI_LONG_FORMAT{
	DEFAULT = 0,
	DMS_F,			// 度分秒
	DMM_F,			// 度分
	DDD_F				// 度
} LATI_LONG_FORMAT;

typedef struct _GPS_DATA_BUF{
	char buf[GPS_BUF_LEN+1];
	uint8_t index;
} GPS_DATA_BUF;

typedef struct _GPS_FLAG{
	uint8_t gpsAnalysis;		// GPS 数据分析中
	uint8_t gpsReceiver;		// GPS 数据接收中
	uint8_t gpsStop;			// GPS 数据接收结束
} GPS_FLAG, *pGPS_GLAG;

typedef struct _DATA_TIME{
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} DATE_TIME;

// 度分秒格式表示经纬度, ddd.mm.ss
typedef struct _DMS{
	double latitude;
	double longitude;
	
	uint8_t 	latitudeDegree;		// 度			
	uint8_t		latitudeCent;		// 分
	uint8_t		latitudeSecond;		// 秒
	uint8_t		longitudeDegree;
	uint8_t		longitudeCent;
	uint8_t 	longitudeSecond; 
} DMS;

// 度和十进制分, ddd.mm.mmm
typedef struct _DMM{
	char* latitude;
	char* longitude;
} DMM;

// 小数度数, ddd.ddddd
typedef struct _DDD{
	double latitude;
	double longitude;
	
	uint8_t latitudeIntegers;			// 度的整数位
	float 	latitudeDecimals;			// 度的小数位
	uint8_t longitudeIntegers;
	float		longitudeDecimals;
} DDD;

typedef struct _GPS_INFO{
	char			latitude[11];			// 字符串表示的经度
	char			longitude[12];			// 字符串表示的纬度							
	float			speed;					// 速度
	float			direction;				// 航向
	float			heightGround;			// 水平面高度
	float			heightSea;				// 海拔高度
	char			NS[2];					// 纬度半球，北纬、南纬
	char			EW[2];					// 经度半球，东经、西经
	DATE_TIME	DT;							// 日期和时间
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
