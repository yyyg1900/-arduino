const int Trig = 10;                                        // 设定SR04连接的Arduino引脚
const int Echo = 11; 
double distance,time ; 
int n=0,m=0,p;
int n5=0,n2=0,n3=0,n4=0;
int p1,p2;


//引入ESP8266.h头文件，建议使用教程中修改后的文件
#include "ESP8266.h"
#include "SoftwareSerial.h"

//配置ESP8266WIFI设置
#define SSID "yyyg1900"    //填写2.4GHz的WIFI名称，不要使用校园网
#define PASSWORD "yyyg1900"//填写自己的WIFI密码
#define HOST_NAME "api.heclouds.com"  //API主机名称，连接到OneNET平台，无需修改
#define DEVICE_ID "643141553"       //填写自己的OneNet设备ID
#define HOST_PORT (80)                //API端口，连接到OneNET平台，无需修改
String APIKey = "j5EWjW8BpEaR=jS32xXKYDSME28="; //与设备绑定的APIKey

#define INTERVAL_SENSOR 5000 //定义传感器采样及发送时间间隔


//定义ESP8266所连接的软串口
/*********************
 * 该实验需要使用软串口
 * Arduino上的软串口RX定义为D3,
 * 接ESP8266上的TX口,
 * Arduino上的软串口TX定义为D2,
 * 接ESP8266上的RX口.
 * D3和D2可以自定义,
 * 但接ESP8266时必须恰好相反
 *********************/
SoftwareSerial mySerial(3, 2);
ESP8266 wifi(mySerial);

void setup()
{
  Serial.begin(9600);     //初始化串口
  pinMode(Trig, OUTPUT); 
  
        pinMode(Echo, INPUT);                                  //要检测引脚上输入的脉冲宽度，需要先设置为输入状态
    Serial.println("The distance is :");
  mySerial.begin(115200); //初始化软串口
  Serial.begin(9600); 
  Serial.print("setup begin\r\n");


  //以下为ESP8266初始化的代码
  Serial.print("FW Version: ");
  Serial.println(wifi.getVersion().c_str());

  if (wifi.setOprToStation()) {
    Serial.print("to station ok\r\n");
  } else {
    Serial.print("to station err\r\n");
  }

  //ESP8266接入WIFI
  if (wifi.joinAP(SSID, PASSWORD)) {
    Serial.print("Join AP success\r\n");
    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());
  } else {
    Serial.print("Join AP failure\r\n");
  }

Serial.println("");


  mySerial.println("AT+UART_CUR=9600,8,1,0,0");
  mySerial.begin(9600);
  Serial.println("setup end\r\n");
}

unsigned long net_time1 = millis(); //数据上传服务器时间
void loop(){
   digitalWrite(Trig, LOW);                                 
        delayMicroseconds(2);                                   
        digitalWrite(Trig, HIGH);                               
        delayMicroseconds(10);                                  //产生一个10us的高脉冲去触发SR04
        digitalWrite(Trig, LOW);                                
            
        time = pulseIn(Echo, HIGH);                              // 检测脉冲宽度，注意返回值是微秒us
        distance = time /58 ;                                  //计算出距离,输出的距离的单位是厘米cm
        Serial.print(distance);                                //把得到的距离值通过串口通信返回给电脑，通过串口监视器显示出来
        Serial.println("cm"); 
        if (distance<100)
        n=n+1;
        delay(1000);
        
  if (net_time1 > millis())
    net_time1 = millis();

  if (millis() - net_time1 > INTERVAL_SENSOR) //发送数据时间间隔
  {

    int chk = distance;
    p=n-m;
p1=n2-n3;
p2=n4-n5;
    
    Serial.print("Read sensor: ");
    

    float sensor_hum = distance;
    Serial.println("");

    if (wifi.createTCP(HOST_NAME, HOST_PORT)) { //建立TCP连接，如果失败，不能发送该数据
      Serial.print("create tcp ok\r\n");
      char buf[10];
      //拼接发送data字段字符串
      String jsonToSend = "{\"times\":";
dtostrf(n, 1, 0, buf);
      jsonToSend += "\"" + String(buf) + "\"";
      jsonToSend += "}";

      //拼接POST请求字符串
      String postString = "POST /devices/";
      postString += DEVICE_ID;
      postString += "/datapoints?type=3 HTTP/1.1";
      postString += "\r\n";
      postString += "api-key:";
      postString += APIKey;
      postString += "\r\n";
      postString += "Host:api.heclouds.com\r\n";
      postString += "Connection:close\r\n";
      postString += "Content-Length:";
      postString += jsonToSend.length();
      postString += "\r\n";
      postString += "\r\n";
      postString += jsonToSend;
      postString += "\r\n";
      postString += "\r\n";
      postString += "\r\n";

      const char *postArray = postString.c_str(); //将str转化为char数组

      Serial.println(postArray);
      wifi.send((const uint8_t *)postArray, strlen(postArray)); //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
      Serial.println("send success");
      if (wifi.releaseTCP()) { //释放TCP连接
        Serial.print("release tcp ok\r\n");
      } else {
        Serial.print("release tcp err\r\n");
      }
      postArray = NULL; //清空数组，等待下次传输数据
    } else {
      Serial.print("create tcp err\r\n");
    }

    Serial.println("");

    net_time1 = millis();
  }
}
