#include<ESP8266WiFi.h>
#include <DHT11.h>

DHT11 dht11(16);

#ifndef ssid
#define SSID "B406"
#define PASS "12345678-a"
#define PORT 8888
#endif

WiFiServer server(PORT);

const char*ssid=SSID;
const char*passwd=PASS;

void setup(){
  Serial.begin(115200);
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,passwd);

  Serial.println("Connecting to WiFi");
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
    delay(500);
  }
  Serial.print("IP Address:");
  Serial.println(WiFi.localIP());

  server.begin();
}
void loop(){
  WiFiClient client=server.available();

  //int test1=65;
  //Serial.println(char(test1));//ascii码转字符
  //char test2='A';
  //Serial.println(int(test2));//字符转ascii码

  float humidity;
  float temperature;
  char hu[5];
  char temp[5];

  char receive_buff[10];

  if(client){
    if(client.connected()){
      Serial.println("client connected");
    }
    while(client.connected())
    {
      ESP.wdtFeed();//长时间执行以下读取函数时会导致看门狗复位，及时喂狗
      if(client.available()>0)
      {
        //这个位置还需要补充一个信息接收Buffer
        String temp_buff=client.readString();
        Serial.println(temp_buff);
        if(temp_buff==String("TH")){
          Serial.println("receive client message TH");
          humidity = dht11.readHumidity();
          temperature=dht11.readTemperature();
          if (temperature != -1 &&humidity != -1)
          {
            strcpy(temp,String(temperature).c_str());
            //client.write("temperature:");
            client.write(temp);

            client.write("-");

            strcpy(hu,String(humidity).c_str());
            //client.write("humidity:");
            client.write(hu);
          }
        }
        break;
      }
      Serial.println("client read nothing...");
      delay(100*2);
    }
        //dtostrf(temperature,2,1,temp);
        //client.write(temp);
        //dtostrf(humidity,2，1,hu);
        //client.write(hu);
        //client.write("?");
        //应该不是看门狗的问题，推测是write的次数太多
    client.stop();
    Serial.println("Client disconnected");
  }
}
