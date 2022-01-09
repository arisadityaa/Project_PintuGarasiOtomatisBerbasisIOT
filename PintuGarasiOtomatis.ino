//deklarasi library yang akan digunakan
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <CTBot.h>
CTBot myBot;

String ssid  = "XXXXXXXXXXX"; // ssid wifi
String pass  = "XXXXXXXXXXX"; // password wifi
String token = "xxxxxxxxxx:xxxxxxxxxxxxxxxxxxxxxxxxx"; //Token API Telegram Bot
const int id = XXXXXXXXXXXX; //id telegram

Servo myservo;
const int pingPin = 16; //pin D0
const int echoPin = 5;  //pin D1  
const int limitSwitch = 14; //pin D5
int servo = 4;  //pin D2
const int irSensor = 2; //pin D4

boolean statusServo;   //mengecek servo sudah membuka atau menutup pintu
boolean ultrasonicOn = false; //membuat notifikasi pesan pintu garasi terbuka sekali saja
boolean ultrasonicOff =  false; //membuat notifikasi pesan pintu garasi tertutup sekali saja
boolean ActionMassage = false;  //membuat agar ketika memeberikan perintah buka pintu garasi pada telegram maka menutup pintu dengan sensor ultrasonik tidak dapat digunakan

void setup() {
  // put your setup code here, to run once:
   Serial.begin(115200);
   
   //deklarasi sensor ultrasonik
   pinMode(pingPin, OUTPUT);
   pinMode(irSensor, INPUT);

   //deklarasi servo
   myservo.attach(servo);

   //konfigurasi koneksi telegram bot
   myBot.wifiConnect(ssid, pass);
   myBot.setTelegramToken(token);
  if (myBot.testConnection())
    Serial.println("\ntestConnection OK");
  else
    Serial.println("\ntestConnection NOK");
}

void loop() {
  // put your main code here, to run repeatedly:
  //deklarasi variabel awal
  TBMessage msg;
  int switchStatus;
  long duration, cm;
  int irStatus = digitalRead(irSensor);

  //kode untuk sensor ultrasonik
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  cm = (duration * 0.034) / 2;

  //kode untuk limit switch
  pinMode(limitSwitch, INPUT);
  switchStatus = digitalRead(limitSwitch);

  //kode untuk monitoring pada serial monitor
  Serial.println(switchStatus);
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();

  //kode pada telegram bot
  if (myBot.getNewMessage(msg)) {
    Serial.println(msg.text);
    if (msg.text.equalsIgnoreCase("/on")){
      if(statusServo == true){
        myBot.sendMessage(msg.sender.id, "Pintu Masih Terbuka");
      }else{
        myservo.write(180);
        ActionMassage=true;
        statusServo=true;
        myBot.sendMessage(msg.sender.id, "Pintu Terbuka");
      }
    }
    else if (msg.text.equalsIgnoreCase("/off")){
      if(statusServo == false){
        myBot.sendMessage(msg.sender.id, "Pintu Masih Tertutup");
      }else{
        myservo.write(0);
        ActionMassage = false;
        statusServo=false;
        myBot.sendMessage(msg.sender.id, "Pintu Tertutup");
      }
    }else if (msg.text.equalsIgnoreCase("/status")){
      if(switchStatus==1){
        myBot.sendMessage(msg.sender.id, "Pintu Sedang Terbuka");
        if(irStatus == 0){// 0 = true 1 = false
          myBot.sendMessage(msg.sender.id, "Terdapat Kendaraan Didalam Garasi");
        }else if(irStatus == 1){// 0 = true 1 = false
          myBot.sendMessage(msg.sender.id, "Tidak Ada Kendaraan Didalam Garasi");
        }
      }else{
         myBot.sendMessage(msg.sender.id, "Pintu Sedang Tertutup");
         if(irStatus == 0){//cek ulang nanti
          myBot.sendMessage(msg.sender.id, "Terdapat Kendaraan Didalam Garasi");
        }else if(irStatus == 1){//cak ulang nanti
          myBot.sendMessage(msg.sender.id, "Tidak Ada Kendaraan Didalam Garasi");
        }
      }
    }
    else {
      String reply;
      reply = (String)"Welcome " + msg.sender.username + (String)". Ketik /on untuk membuka pintu, /off untuk menutup pintu dan /status untuk mengecek apakah pintu terbuka atau tertutup. Created By Aris Aditya Nugraha";
      myBot.sendMessage(msg.sender.id, reply);
     }
  }
  
    //kode servo berdasarkan sensor ultrasonik
    if(cm < 20){
        if(ActionMassage == false){
        myservo.write(180);
        statusServo=true;
      if (ultrasonicOn == false){
        myBot.sendMessage(msg.sender.id, "Pintu Ultrasonic Terbuka");
        ultrasonicOn=true;
        ultrasonicOff=false;
      }
    }
  }else{
    if(ActionMassage == false){
      myservo.write(0);
      statusServo=false;
      if(ultrasonicOff == false){
        myBot.sendMessage(msg.sender.id, "Pintu Ultrasonic Tertutup");
        ultrasonicOff=true;
        ultrasonicOn=false;
      }
    }
  }
    delay(500);
}
// Project Internet Of Thhings
//I Made Aris Aditya Nugraha
//1905551088
//Teknologi Informasi, Fakultas Teknik, Universitas Udayana
