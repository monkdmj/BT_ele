/*****************************************************
//用于GPRS向服务器发送数据时建立结构体
//结构体再转成Json，Json再转成字符串进行发送
******************************************************/

#ifndef SENSOR_H
#define SENSOR_H

//class Sensor
typedef struct Sensor Sensor;
struct Sensor
{
	
	char* date;           //采集的时间
	float temperature;  //采集的温度
	float humidity;     //采集的湿度
	float light;        //光强
	float distance;     //平层误差a
	float Amper[3];		//三相电流
	float voltage[3];   //三相电压
	
	
};

typedef struct {
	float temperature;  //采集的温度
	float humidity;     //采集的湿度
	float light;        //光强
	float distance;     //平层误差a
	float voltage[3];   //三相电压
	float Amper[3];		//三相电流
	

	float voice;		//噪声
	float acc[3];		//加速度
	float pal[3];		//角速度
	float angle[3];		//角度
	float dis[4];		//以主控为中心，距电梯四壁距离

	char* date;           //采集的时间

} Sensor_master;

//用于初始化一个Sensor结构体
Sensor* Sensor_Create(void);
void Sensor_Clean(Sensor* const me);

Sensor_master* Sensor_master_Create(void);
void Sensor_master_Clean(Sensor_master* const me);

//用于清空一个结构体
void Sensor_Destroy(Sensor* const me);
void Sensor_master_Destroy(Sensor_master* const me);

//用于在采集过程中设置值

void Sensor_setTemperature(Sensor* const me,float temp);
void Sensor_setHumidity(Sensor*const me, float humidity);
void Sensor_setLight(Sensor*const me, float light);
void Sensor_setDistance(Sensor* const me, float distance);
void Sensor_setDate(Sensor* const me, char *date);
void Sensor_setVoltage(Sensor* const me, float voltage[]);
void Sensor_setAmper(Sensor* const me, float AMper[]);


//用于在查询过程中获取值

float Sensor_getTemperature(Sensor* const me);
float Sensor_getHumidity(Sensor* const me);
float Sensor_getLight(Sensor* const me);
float  Sensor_getDistance(Sensor* const me);
char*  Sensor_getDate(Sensor* const me);

char* Sensor_master_to_String(Sensor_master* const me);
char* Sensor_to_String(Sensor* const me);
// void save_Sensor(Sensor* const me,const char* key);
// Sensor* get_Sensor(const char*key);

Sensor_master* get_master_Struct(char* char_value);
Sensor* get_Struct(char* char_value);
#endif /*SENSOR_H*/
