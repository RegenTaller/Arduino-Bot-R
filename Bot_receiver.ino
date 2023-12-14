#include <SPI.h>                                         
#include <nRF24L01.h>                                    
#include <RF24.h>        

#define SPEED_1      5 
#define DIR_1        4
 
#define SPEED_2      6
#define DIR_2        7

#define RELAY_PIN 3

double koef = 1.35;


RF24 radio(8, 9);   // nRF24L01+ (CE, CSN)
int data[4];        // создаем массив для приема данных по радиоканалу

int VoltMAX = 140;  // Значение, при котором MotorShield выдаёт 12В
int VoltMIN = 40;   // Минимальное значение старта движения моторов

// Создаем переменные для хранения значений с джойстика
int x = 0; 
int y = 0;

int weapon = -1;    // Переменная остояния орудия

double mod = 0;     // Величина радиус-вектора скорости. Абсолютная скорость машинки

int SP1 = 0;        // Переменная скорости мотора 1
int SP2 = 0;        // Переменная скорости мотора 2 

void setup(){
  Serial.begin(9600);
    
  radio.begin();                                        
  radio.setChannel(10);                                 // Указываем канал приёма данных (от 0 до 127), 5 - значит приём данных осуществляется на частоте 2,405 ГГц (на одном канале может быть только 1 приёмник и до 6 передатчиков)
  radio.setDataRate     (RF24_1MBPS);                   // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
  radio.setPALevel      (RF24_PA_HIGH);                 // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
  radio.openReadingPipe (3, 0x1234567DABLL);            // Открываем 1 трубу с идентификатором 0x1234567890 для приема данных (на одном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)
  radio.startListening  ();                             // Включаем приемник, начинаем прослушивать открытую трубу

  // Устанавливаем режим работы пинов 
  for (int i = 4; i < 8; i++) {     
    pinMode(i, OUTPUT);
  }
  pinMode(RELAY_PIN, OUTPUT);
}

void loop(){
    // Если есть соединение с джойстиком, то ...

    if(radio.available()){   

        // ..ю то считываем из радиоканала значения с джойстика - они хранятся в массиве data                             
        radio.read(&data, sizeof(data));             

        // Сохраняем значения из массива в переменные (чтобы не запутаться с индексами массива)
        x = data[1];
        y = data[2];

        mod = sqrt(pow(x, 2) + pow(y, 2))/1000; // Вычисление значения абсолютной скорости. Модуль радиус-вектора.


        if (x > 0 && y > 0) {

          Serial.println("VVE PRAVO");  // Движение робота вправо-вверх

          SP1 = mod*VoltMAX;            // Скорость Левого (1-го) мотора пропорциональна модулю радиус-вектора
          SP2 = map(y, 0, 1000, VoltMIN, VoltMAX)/koef; // Скорость правого 

          digitalWrite(DIR_1, HIGH);    // направление мотора 1 - вперед (Если увидите, что машинка едет не в ту сторону или происходит что-то не то - меняйте направления, пока она не начнёт ехать предсказуемо)
          analogWrite(SPEED_1, SP1); 

          digitalWrite(DIR_2, HIGH);    // направление мотора 2 - вперед
          analogWrite(SPEED_2, SP2);

          Serial.println("Vtoroy");
          Serial.print(SP2);

        }

        else if (x > 0 && y < 0) {

          SP1 = mod*VoltMAX;
          SP2 = map(y, 0, -1000, VoltMIN, VoltMAX)/koef;

          digitalWrite(DIR_1, LOW); // направление мотора 1 - вперед
          analogWrite(SPEED_1, SP1); 

          digitalWrite(DIR_2, LOW); // направление мотора 2 - вперед
          analogWrite(SPEED_2, SP2);

          Serial.println("VNI PRAVO");
          
        }

        else if (x < 0 && y > 0) {

          SP1 = map(y, 0, 1000, VoltMIN, VoltMAX)/koef;
          SP2 = mod*VoltMAX;
          
          digitalWrite(DIR_1, HIGH); // направление мотора 1 - вперед
          analogWrite(SPEED_1, SP1);

          digitalWrite(DIR_2, HIGH); // направление мотора 2 - вперед
          analogWrite(SPEED_2, SP2);

          Serial.println("VVE LEVO");

        }

        else if (x < 0 && y < 0) {

          SP1 = map(y, 0, -1000, VoltMIN, VoltMAX)/koef;
          SP2 = mod*VoltMAX;
          
          digitalWrite(DIR_1, LOW); // направление мотора 1 - вперед
          analogWrite(SPEED_1, SP1);

          digitalWrite(DIR_2, LOW); // направление мотора 2 - вперед
          analogWrite(SPEED_2, SP2);

          Serial.println("VNI LEVO");

        }
        
        else if (x == 0 && y > 0) {
          Serial.println("VPeriod");

          SP1 = mod*VoltMAX;
          SP2 = mod*VoltMAX;

          digitalWrite(DIR_1, HIGH); // направление мотора 1 - вперед
          analogWrite(SPEED_1, SP1); 

          digitalWrite(DIR_2, HIGH); // направление мотора 2 - вперед
          analogWrite(SPEED_2, SP2);

        }

        else if (x == 0 && y < 0) {

          Serial.println("Nazad");

          SP1 = mod*VoltMAX;
          SP2 = mod*VoltMAX;

          digitalWrite(DIR_1, LOW); // направление мотора 1 - вперед
          analogWrite(SPEED_1, SP1);

          digitalWrite(DIR_2, LOW); // направление мотора 2 - вперед
          analogWrite(SPEED_2, SP2);

        }

        else if (x > 0 && y == 0) {

          Serial.println("PRAVO");

          SP1 = mod*VoltMAX;
          SP2 = mod*VoltMAX;

          digitalWrite(DIR_1, HIGH); // направление мотора 1 - вперед
          analogWrite(SPEED_1, SP1);

          digitalWrite(DIR_2, LOW); // направление мотора 2 - вперед
          analogWrite(SPEED_2, SP2);

        }

        else if (x < 0 && y == 0) {

          Serial.println("LEVO");

          SP1 = mod*VoltMAX;
          SP2 = mod*VoltMAX;

          digitalWrite(DIR_1, LOW); // направление мотора 1 - вперед
          analogWrite(SPEED_1, SP1);

          digitalWrite(DIR_2, HIGH); // направление мотора 2 - вперед
          analogWrite(SPEED_2, SP2);

        }

        else if (x == 0 && y == 0) {

          Serial.println("ZERO");

          SP1 = 0;
          SP2 = 0;

          //digitalWrite(DIR_1, LOW); // направление мотора 1 - вперед
          analogWrite(SPEED_1, SP1);

          //digitalWrite(DIR_2, LOW); // направление мотора 2 - вперед
          analogWrite(SPEED_2, SP2);

        }

        weapon = data[3];

        // Если значение переменной орудия 1, то включаем реле и запускаем мотор орудия
        if (weapon == 1) {
          digitalWrite(RELAY_PIN, HIGH);
          Serial.println("On");
        }
        else { // Если значение переменной орудия -1, то выключаем реле и мотор
          digitalWrite(RELAY_PIN, LOW);
          //Serial.println("Off");
        }

      delay(10);
        
       
    }
}