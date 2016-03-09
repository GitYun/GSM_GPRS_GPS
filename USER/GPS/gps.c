#include "gps.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

extern GPS_FLAG gps_flag;


/**
  * @brief  �������յ��� GPS ��Ϣ
  * @param  buffer: GPS ��Ϣ������
	* @param	gps: �洢������� GPS ��Ϣ�Ľṹ��
	* @param	type: ��Ҫ������ GPS ��Ϣ������
  * @retval 1 - �����ɹ���0 - ����ʧ��
  */
uint8_t parseGpsData(char* buffer, GPS_INFO* gps, GPS_DATA_TYPE type)
{
	uint8_t ch, status, tmp;
	float speed_tmp;
	char* buf = buffer;
	
	ch = buf[5];
	if ( type == (GPS_DATA_TYPE)GPRMC )
		status = buf[getComma(2, buf)];			// �ж� GPRMC �����Ƿ�Ϊ��Ч��λ��A=��Ч��V=��Ч
	
	if ( ch == 'C' )				// ���������ַ���C��($GPRMC)
	{
		if ( status == 'A' )	// ���������Ч��������Ѿ���λ
		{
			gps->NS[0] = buf[getComma(4, buf)];
			gps->NS[1] = '\0';
			gps->EW[0] = buf[getComma(6, buf)];
			gps->EW[1] = '\0';
			
			getStringNumber(&buf[getComma(3, buf)], gps->latitude);
			getStringNumber(&buf[getComma(5, buf)], gps->longitude);			
/*			
			if ( format == (LATI_LONG_FORMAT)DMS_F )
			{
				float lati_cent_tmp, lati_second_tmp;
				float long_cent_tmp, long_second_tmp;
				char s[4] = {0};
				uint8_t strlen = 0;
				
				gps->dms.latitude = getDoubleNumber(&buf[getComma(3, buf)]);
				gps->dms.longitude = getDoubleNumber(&buf[getComma(5, buf)]);
				
				gps->dms.latitudeDegree = (uint16_t)gps->dms.latitude / 100;								// ����γ��
				lati_cent_tmp 					= (gps->dms.latitude - gps->dms.latitudeDegree * 100);
				gps->dms.latitudeCent 	= (uint8_t)lati_cent_tmp;
				lati_second_tmp					= (lati_cent_tmp - gps->dms.latitudeCent) * 60;
				gps->dms.latitudeSecond = (uint8_t)lati_second_tmp;
				
				gps->dms.longitudeDegree 	= (uint16_t)gps->dms.longitude / 100;							// ���뾭��
				long_cent_tmp							= (gps->dms.longitude - gps->dms.longitudeDegree * 100);
				gps->dms.longitudeCent 		= (uint8_t)long_cent_tmp;
				long_second_tmp						= (long_cent_tmp - gps->dms.longitudeCent) * 60;
				gps->dms.longitudeSecond 	= (uint8_t)long_second_tmp;
				
				strlen = sprintf(s, "%d", gps->dms.latitudeDegree);
				s[strlen] = '.';
				strncpy(gps->latitude, s, strlen+1);
				strlen = sprintf(s, "%d", gps->dms.latitudeCent);
				s[strlen] = '.';
				strncat(gps->latitude, s, strlen+1);
				strlen = sprintf(s, "%d", gps->dms.latitudeSecond);
				strncat(gps->latitude, s, strlen);
				
				strlen = sprintf(s, "%d", gps->dms.longitudeDegree);
				s[strlen] = '.';
				strncpy(gps->longitude, s, strlen+1);
				strlen = sprintf(s, "%d", gps->dms.longitudeCent);
				s[strlen] = '.';
				strncat(gps->longitude, s, strlen+1);
				strlen = sprintf(s, "%d", gps->dms.longitudeSecond);
				strncat(gps->longitude, s, strlen);
			}
			else if ( format == (LATI_LONG_FORMAT)DMM_F || format == (LATI_LONG_FORMAT)DEFAULT )
			{
				getStringNumber(&buf[getComma(3, buf)], gps->latitude);
				getStringNumber(&buf[getComma(5, buf)], gps->longitude);
			}
			else if ( format == (LATI_LONG_FORMAT)DDD_F )
			{
				float	lati_decimals, long_decimals;
				
				gps->ddd.latitude 	= getDoubleNumber(&buf[getComma(3, buf)]);
				gps->ddd.longitude 	= getDoubleNumber(&buf[getComma(5, buf)]);
				
				gps->ddd.latitudeIntegers = (uint16_t)gps->ddd.latitude / 100;
				lati_decimals = (gps->ddd.latitude - gps->ddd.latitudeIntegers * 100);
				gps->ddd.latitudeDecimals = lati_decimals / 60.0;
				gps->ddd.latitude = (float)gps->ddd.latitudeIntegers + gps->ddd.latitudeDecimals;
				
				gps->ddd.longitudeIntegers = (uint16_t)gps->ddd.longitude / 100;
				long_decimals = (gps->ddd.longitude - gps->ddd.longitudeIntegers * 100);
				gps->ddd.longitudeDecimals = long_decimals / 60.0;
				gps->ddd.longitude = (float)gps->ddd.longitudeIntegers + gps->ddd.longitudeDecimals;
				
				sprintf(gps->latitude, "%f", gps->ddd.latitude);
				sprintf(gps->longitude, "%f", gps->ddd.longitude);
			}
*/			
			speed_tmp				= (float)getDoubleNumber(&buf[getComma(7, buf)]);	// �ٶ�(��λ������/ʱ)
			gps->speed			= speed_tmp * 1.85;																// 1����=1.85����
			gps->direction 	= (float)getDoubleNumber(&buf[getComma(8, buf)]);	// �Ƕ�
			
			gps->DT.hour		= (buf[7] - '0') * 10 + (buf[8] - '0');
			gps->DT.minute	= (buf[9] - '0') * 10 + (buf[10] - '0');
			gps->DT.second	= (buf[11] - '0') * 10 + (buf[12] - '0');
			tmp = getComma(9, buf);
			gps->DT.day		= (buf[tmp + 0] - '0') * 10 + (buf[tmp + 1] - '0');
			gps->DT.month	= (buf[tmp + 2] - '0') * 10 + (buf[tmp + 3] - '0');
			gps->DT.year	= (buf[tmp + 4] - '0') * 10 + (buf[tmp + 5] - '0') + 2000;
			
			UTC2BTC(&gps->DT);
			
			return 1;
		}
	}
	
	return 0;
}

/**
  * @brief  ����ʽ�����γ��
	* @param  gps: �洢������� GPS ��Ϣ�Ľṹ��
	* @param	format: �����ʽ
  * @retval 1 - ָʾ��������ʽ����ɹ���0 - ���ʧ��
  */
uint8_t outputLatitudeLongitude(GPS_INFO* gps, LATI_LONG_FORMAT format)
{
	if ( format == (LATI_LONG_FORMAT)DMM_F || format == (LATI_LONG_FORMAT)DEFAULT )
	{
		return 1;
	}
	else if ( format == (LATI_LONG_FORMAT)DMS_F )
	{
		DMS dms;
		float lati_cent_tmp, lati_second_tmp;
		float long_cent_tmp, long_second_tmp;
		char s[10] = {0};
		uint8_t strlen = 0;
		
		dms.latitude 	= str2Double(gps->latitude);
		dms.longitude = str2Double(gps->longitude);
		
		dms.latitudeDegree 	= (uint16_t)dms.latitude / 100;								// ����γ��
		lati_cent_tmp 			= (dms.latitude - dms.latitudeDegree * 100);
		dms.latitudeCent 		= (uint8_t)lati_cent_tmp;
		lati_second_tmp			= (lati_cent_tmp - dms.latitudeCent) * 60;
		dms.latitudeSecond 	= (uint8_t)lati_second_tmp;
		
		dms.longitudeDegree = (uint16_t)dms.longitude / 100;							// ���뾭��
		long_cent_tmp				= (dms.longitude - dms.longitudeDegree * 100);
		dms.longitudeCent 	= (uint8_t)long_cent_tmp;
		long_second_tmp			= (long_cent_tmp - dms.longitudeCent) * 60;
		dms.longitudeSecond = (uint8_t)long_second_tmp;

		memset(gps->latitude, 0, 11);
		memset(gps->longitude, 0, 12);
		
		strlen = sprintf(s, "%d", dms.latitudeDegree);
		s[strlen++] = '.';
		strlen += sprintf(s+strlen, "%02d", dms.latitudeCent);
		s[strlen++] = '.';
		strlen += sprintf(s+strlen, "%02d", dms.latitudeSecond);
		strncpy(gps->latitude, s, strlen+1);

		strlen = sprintf(s, "%d", dms.longitudeDegree);
		s[strlen++] = '.';
		strlen += sprintf(s+strlen, "%02d", dms.longitudeCent);
		s[strlen++] = '.';
		strlen += sprintf(s+strlen, "%02d", dms.longitudeSecond);
		s[strlen] = '\0';
		strncpy(gps->longitude, s, strlen+1);
		
		return 1;
	}
	else if ( format == (LATI_LONG_FORMAT)DDD_F )
	{
		DDD ddd;
		float	lati_decimals, long_decimals;
		
		ddd.latitude 	= str2Double(gps->latitude);
		ddd.longitude = str2Double(gps->longitude);
		
		ddd.latitudeIntegers 	= (uint16_t)ddd.latitude / 100;
		lati_decimals 				= (ddd.latitude - ddd.latitudeIntegers * 100);
		ddd.latitudeDecimals 	= lati_decimals / 60.0;
		ddd.latitude 					= (float)ddd.latitudeIntegers + ddd.latitudeDecimals;
		
		ddd.longitudeIntegers = (uint16_t)ddd.longitude / 100;
		long_decimals 				= (ddd.longitude - ddd.longitudeIntegers * 100);
		ddd.longitudeDecimals = long_decimals / 60.0;
		ddd.longitude 				= (float)ddd.longitudeIntegers + ddd.longitudeDecimals;
		
		memset(gps->latitude, 0, 11);
		memset(gps->longitude, 0, 12);		

		sprintf(gps->latitude, "%f", ddd.latitude);
		sprintf(gps->longitude, "%f", ddd.longitude);
		return 1;
	}
	
	return 0;
}

/**
  * @brief  �����ַ����и������ŵ�λ��
  * @param  position - ���ҵĶ����ǵڼ�������
	* @param	str	- ��Ҫ���ҵ��ַ���
  * @retval ���ҳɹ������ض�������λ�ã���1��ʼ�ƣ����򣬷���0
*/
uint8_t getComma(uint8_t position, char* str)
{
	uint8_t i, j =0;
	uint8_t len = (uint8_t)strlen(str);
	
	for ( i = 0; i < len; i++ )
	{
		if ( str[i] == ',' )
			j++;
		if ( j == position )
			return i+1;
		if ( j > COMMA_MAX_POSITION )
			return 0;
	}
	
	return 0;
}

/**
  * @brief  ��һ���ַ���ת���ɸ�����
  * @param  string - �ַ���
  * @retval ת����˫����ֵ
  */
static double str2Double(char* string)
{

	double retval = 0;
	double dat;
	uint8_t integerFlag = 1;	// �ж����������ֻ���С������
	uint16_t i = 0;						// С���� x��λ��С�����һλ i=1������λ i=10������λ i=100��... 
	
	while ( *string != '\0' )
	{
		switch ( *string )
		{
			case '0':
				dat = 0;
				break;
			case '1':
				dat = 1;
				break;
			case '2':
				dat = 2;
				break;
			case '3':
				dat = 3;
				break;
			case '4':
				dat = 4;
				break;
			case '5':
				dat = 5;
				break;
			case '6':
				dat = 6;
				break;
			case '7':
				dat = 7;
				break;
			case '8':
				dat = 8;
				break;
			case '9':
				dat = 9;
				break;
			case '.':
				dat = '.';
				break;
		}
		
		if ( dat == '.' )
		{
			integerFlag = 0;
			i = 1;
			string++;
			continue;
		}
		
		if ( integerFlag == 1 )
		{
			retval = retval * 10 + dat;
		}
		else
		{
			retval = retval + dat / (10 * i);
			i *= 10;
		}
		string++;
	}
	
	return retval;
}

/**
  * @brief  ��ȡ�����ַ�����һ������֮ǰ���ַ�
  * @param  str - �ַ���
	* @param	retval - ��ȡ���ַ���
  * @retval ��
  */
static void getStringNumber(char* str, char* retval)
{
	uint8_t i; 
	i = getComma(1, str);
	i -= 1;
	strncpy(retval, str, i);
	retval[i] = 0;		// '\0'
}

/**
  * @brief  �Ѹ����ַ�����һ������֮ǰ���ַ�ת����˫������
  * @param  str - �ַ���
  * @retval ת�����˫����ֵ
  */
static double getDoubleNumber(char* str)
{
	char* buf = NULL;
	uint8_t i;
	double retval;
	i = getComma(1, str);
	buf = (char*)calloc(i, sizeof(char));
	if ( buf )
	{
		i -= 1;
		strncpy(buf, str, i);
		buf[i] = 0;		// '\0'
		retval = str2Double(buf);
		memset(buf, 0, i);
		free(buf);
		buf = NULL;
	}
	return retval;
}

/**
  * @brief  ת��ʱ��Ϊ����ʱ����ʱ��
  * @param  gpsDT	- ���ʱ��Ľṹ��
  * @retval ��
  */
static void UTC2BTC(DATE_TIME* gpsDT)
{
	gpsDT->second++;
	if ( gpsDT->second > 59 )
	{
		gpsDT->second = 0;
		gpsDT->minute++;
		if ( gpsDT->minute > 59 )
		{
			gpsDT->minute = 0;
			gpsDT->hour++;
		}
	}
	
	gpsDT->hour = gpsDT->hour + 8;
	if ( gpsDT->hour > 23 )
	{
		gpsDT->hour -= 24;
		gpsDT->day ++;
		if ( gpsDT->month == 4 || gpsDT->month == 6 ||
				 gpsDT->month == 9 || gpsDT->month == 11 )
		{
			if ( gpsDT->day > 30 )
			{
				gpsDT->day = 1;
				gpsDT->month++;
			}
		}
		else
		{
			if (gpsDT->day > 31)
			{
				gpsDT->day = 1;
				gpsDT->month++;
			}
		}
		
		if ( gpsDT->year % 4 == 0 )
		{
			if ( gpsDT->day > 29 && gpsDT->month == 2)
			{
				gpsDT->day = 1;
				gpsDT->month++;
			}
		}
		else
		{
			if ( gpsDT->day > 28 && gpsDT->month == 2)
			{
				gpsDT->day = 1;
				gpsDT->month++;
			}
		}
		
		if ( gpsDT->month > 12 )
		{
			gpsDT->month -= 12;
			gpsDT->year++;
		}
	}
}

/**
  * @brief  ��������ת��Ϊ�ַ���
	* @param  value: ������
	* @param	ndigit: ת����λ��
	* @param	buf: �ַ�������
  * @retval ָ��ת������ַ������׵�ַ
  */
static char *gcvt(double value, int ndigit, char *buf)
{
	int len;
	char* string = (char*)calloc(ndigit, sizeof(char));
	if ( string == NULL )
		return NULL;
	sprintf(buf, "%f", value);
	strncpy(string, buf, ndigit);
	len = strlen(buf) + 1;
	memset(buf, 0, len);
	strncpy(buf, string, ndigit);
	buf[ndigit] = '\0';
	memset(string, 0, ndigit);
	free(string);
	string = NULL;
	return buf;
}

static char* itoa(int value, char* string, int radix)
{
	return (void*)0;
}

uint8_t isGpsAnalysising(void)
{
	return gps_flag.gpsAnalysis;
}

uint8_t isGpsReceivering(void)
{
	return gps_flag.gpsReceiver;
}
uint8_t isGpsStoping(void)
{
	return gps_flag.gpsStop;
}

void cleanGpsFlag(GPS_FLAG* gpsFlag)
{
	gpsFlag->gpsAnalysis = 0;
	gpsFlag->gpsReceiver = 0;
	gpsFlag->gpsStop = 0;
}

void cleanGpsDataBuf(char* gpsDataBuf)
{
	memset(gpsDataBuf, 0, GPS_BUF_LEN);
}

