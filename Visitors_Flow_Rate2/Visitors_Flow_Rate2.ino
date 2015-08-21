#include <Wire.h>
#include <AT24Cxx.h>
//#include <RTClib.h>
#include "DS3231.h"
AT24Cxx AT24C32(0x57);
DS3231 RTC; //Create the DS3231 object
//RTC_DS1307 RTC;
DateTime dt(2015, 8, 21, 15, 18, 0, 5);
char Director1 = 0;//flag
int VisitorsIn = 0;
int VisitorsOut = 0;
int count = 0;
void setup (void) {
  //初始化串口
  Serial.begin(9600);
  //初始化I2C
  Wire.begin();
  //初始化RTC
  RTC.begin();
  //  RTC.adjust(dt); //Adjust date-time as defined 'dt' above
  // clear_Mem();
  attachInterrupt(0, Detect1, RISING  );//下降沿说明有
  attachInterrupt(1, Detect2, RISING );//下降沿说明人
}

void clear_Mem()
{
  int i = 0;
  for (i = 0; i < 400; i++)
    AT24C32.WriteMem(i, 0x00);
}

void Detect1()
{
  Serial.println("Detect1()");
  if (Director1 == 0) //means people is go in and not yet pass
  {
    Director1 = 1;
  }
  if (Director1 == 2) //means people is going out
  {
    Director1 = 0;
    VisitorsOut ++;
    count++;
    SaveData();
  }
}
void Detect2()
{
  Serial.println("Detect2()");
  if (Director1 == 0) //means people is go out and not yet pass
  {
    Director1 = 2;
  }
  if (Director1 == 1)
  {
    Director1 = 0;
    VisitorsIn ++;
    count++;
    SaveData();
  }
}

void SaveData()
{
  //获取当前时间信息
  DateTime now = RTC.now();
  char buffer[10];

  buffer[0] = now.year() - 2000;
  buffer[1] = now.month();
  buffer[2] = now.date();
  buffer[3] = now.hour();
  buffer[4] = now.minute();
  buffer[5] = now.second();

  buffer[6] = VisitorsIn % 255;
  buffer[7] = (VisitorsIn >> 8) % 255; //

  buffer[8] = VisitorsOut % 255;
  buffer[9] = (VisitorsOut >> 8) % 255;
  //写入时间信息到eeprom
  AT24C32.WriteMem(0, (char)count%255);
  AT24C32.WriteMem(1, (char)(count>>8)%255);
  AT24C32.WriteMem(2+count*10, buffer, 10);
}

void ReadData()
{
  int all;
  char buffer[10];
  char buffer1[2];
  AT24C32.ReadMem(0, buffer1, 2);
  all = buffer1[0] + buffer1[1] * 255;
  Serial.println( buffer1[0],DEC);
   Serial.println( buffer1[1],DEC);
  Serial.println(all);
  int i;
  for (i = 0; i < all; i++)
  {
    AT24C32.ReadMem(2+i*10, buffer, 10);
    Serial.print(2000 + buffer[0], DEC);  //year
    Serial.print('-');
    Serial.print(buffer[1], DEC);//month
    Serial.print('-');
    Serial.print(buffer[2], DEC);//day
    Serial.print('/');

    Serial.print(buffer[3], DEC);//hour
    Serial.print(':');
    Serial.print(buffer[4], DEC);//min
    Serial.print(':');
    Serial.print(buffer[5], DEC);//sec
    Serial.print('#');

    int temp = buffer[6] + buffer[7] * 255;
    Serial.print(temp, DEC);
    Serial.print('/');
    int temp2 = buffer[8] + buffer[9] * 255;
    Serial.print(temp2, DEC);
    Serial.println();
  }
}

void loop() {
  //判断是否有新串口数据
  if (Serial.available() > 0) {
    //读取数据
    int instruct = Serial.read();
    switch (instruct) {
      case 'P':
        {
          //先eeprom 指定地址写入数据
          AT24C32.WriteMem(0, 0x04);
          break;
        }
      case 'G':
        {
          ReadData();
          break;
        }
      case 'F':
        {
          //获取当前时间信息
          DateTime now = RTC.now();
          char buffer[10];

          buffer[0] = now.year() - 2000;
          buffer[1] = now.month();
          buffer[2] = now.date();
          buffer[3] = now.hour();
          buffer[4] = now.minute();
          buffer[5] = now.second();

          buffer[6] = VisitorsIn % 255;
          buffer[7] = (VisitorsIn >> 8) % 255; //

          buffer[8] = VisitorsOut % 255;
          buffer[9] = (VisitorsOut >> 8) % 255;
          //写入时间信息到eeprom
          AT24C32.WriteMem(0, buffer, 10);
          break;
        }
    }
  }
}


