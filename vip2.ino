#include <DHT.h>
#include "HX711.h"
#include "FirebaseESP8266.h"
#include "ESP8266WiFi.h"
#include <ThingSpeak.h>
#define DHTPIN 12
#define DHTTYPE DHT11
#define FIREBASE_HOST "https://esp8266-9438b-default-rtdb.firebaseio.com/"                          // the project name address from firebase id
#define FIREBASE_AUTH "j0w8rUJorOqfcS3wOKSv6oaGqJTMwWt723YyGdc2"
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Servo.h>
Servo myservo;
WiFiUDP u;
NTPClient n(u, "1.asia.pool.ntp.org", 7 * 3600); // khai báo truy cập vào NTP sever khu vực ASIA
DHT dht(DHTPIN, DHTTYPE);
//HX711 cell(16, 5);
const char *ssid =  "NOBICODE";
const char *pass =  "0929777727";
const char *server = "api.thingspeak.com";
unsigned long myChannelNumber = 1322368;
const char * myWriteAPIKey = "SU3VQ40XJN0XAH5H";
int lastHumi , lastTemp;
float h = 0;
float t = 0;
WiFiClient client;
FirebaseData firebaseData;
FirebaseData ledData;
FirebaseData gioData;
FirebaseData phutData;
FirebaseData svData;
FirebaseData autoData;
FirebaseData taData;

FirebaseJson json;
int led = 2;     // Connect LED to D5
void setup() {
  Serial.begin(9600);
  Serial.println("Do Minh Tuan");
  myservo.attach(4);
  dht.begin();
  setupWiFi();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  n.begin();// khởi động truy cập sever thời gian

}

void loop() {
  delay(1000);
  
    readDHT11();
    

    client_Sendata();
 
  n.update();//cap nhap tg
  //controlled();
  hengio2();
  //sv();
  updatetime();
  //can();
}
void client_Sendata() {
  static boolean data_state = false;
  if ( data_state )
  {
    ThingSpeak.writeField(myChannelNumber, 1, lastTemp, myWriteAPIKey);
    data_state = false;

  }
  else
  {
    ThingSpeak.writeField(myChannelNumber, 2, lastHumi, myWriteAPIKey);
    data_state = true;
  }
  ThingSpeak.writeField(myChannelNumber, 3, lastTemp, myWriteAPIKey);
  Serial.println("Data send to ThingSpeak");
}
void setupWiFi() {
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  ThingSpeak.begin(client);
  pinMode(led, OUTPUT);
}
void readDHT11() {
  h = dht.readHumidity();
  t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  } else {
    lastHumi = h;
    lastTemp = t;
  }
  if (Firebase.setFloat(firebaseData, "nhietdo", lastTemp))
  {
    Serial.println("PASSED");
  }
  else
  {
    Serial.println("FAILED");
  }

  if (Firebase.setFloat(firebaseData, "doam", lastHumi))
  {
    Serial.println("PASSED");
  }
  else
  {
    Serial.println("FAILED");;
  }
  Serial.print("Humidity: ");
  Serial.print(lastHumi);
  Serial.print("%t");
  Serial.print("Temperature: ");
  Serial.print(lastTemp);
  Serial.print("°C");
  Serial.println();
}
void controlled() {
  if (Firebase.getString(ledData, "led")) {
    Serial.println(ledData.stringData());
    if (ledData.stringData() == "1") {
      digitalWrite(led, LOW);
    }
    else if (ledData.stringData() == "0") {
      digitalWrite(led, HIGH);
    }
  }
}

void hengio() {
  if (Firebase.getString(gioData, "gio")) {
    String a = String(n.getHours());
    if (gioData.stringData() == ("\\\"" + a + "\\\"") ) {
      if (Firebase.getString(phutData, "phut")) {
        String b = String(n.getMinutes());
        if (phutData.stringData() == ("\\\"" + b + "\\\"")) {
          digitalWrite(led, LOW);
        }
        if (phutData.stringData() == "1") {
          digitalWrite(led, LOW);
        }
        if (phutData.stringData() == "0") {
          digitalWrite(led, HIGH);
        }
      }
    }
  }
}
void sv() {
  if (Firebase.getString(svData, "sv")) {
    Serial.println(svData.stringData());
    if (svData.stringData() == "1") {
      if (myservo.read() == 180) {
        return;
      } else {
        myservo.write(180);
      }
    }
    else if (svData.stringData() == "0") {
      if (myservo.read() == 0) {
        return;
      } else {
        myservo.write(0);
      }
    }
  }
}
void updatetime() {
  n.update();
  Firebase.setInt(firebaseData, "h", n.getHours());
  Serial.println(firebaseData.stringData());
  Firebase.setInt(firebaseData, "p", n.getMinutes());
  Serial.println(firebaseData.stringData());
}

/*void can() {
  long val = 0;
  val = cell.read(); // most recent reading
  float gt = (val - 57209) / 104.f;
  if (Firebase.getString(autoData, "auto")) {
    if (ledData.stringData() == "1") {
      if (gt <= 200) {
        Serial.println(gt);
        myservo.write(180);
      } else {
        Serial.println(gt);
        myservo.write(0);
      }
    }

  }
  }*/
void hengio2() {
  if (Firebase.getString(gioData, "gio")) {
    String a = String(n.getHours());
    if (gioData.stringData() == ("\\\"" + a + "\\\"") ) {
      if (Firebase.getString(phutData, "phut")) {
        String b = String(n.getMinutes());
        if (phutData.stringData() == ("\\\"" + b + "\\\"")) {
          long val = 0;
          val = cell.read(); // most recent reading
          //float gt = (val - 57209) / 104.f;

          Firebase.getString(taData, "ta") ;
          int gt2 = taData.stringData().toInt();
          while (true) {
            val = cell.read(); // most recent reading
            float gt3 = (val - 57209) / 104.f;
            Serial.print(".");
            myservo.write(180);
            if(gt3>=gt2){
                break;
            }
          delay(500);
          }
          Serial.println("Cho an xong");
          myservo.write(0);

        }
      }
    }
  }
}
