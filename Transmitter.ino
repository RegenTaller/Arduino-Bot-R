#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(8, 10);
int data[4];  

const int xPin = A0;
const int yPin = A1;
const int zPin = 4; 

int xPosition = 0;
int yPosition = 0;
int PushButton = 0; // Переменная мгновенного значения статуса кнопки орудия

int switcher = -1; // Переменная длительного хранения статуса орудия

                                
void setup(){
    radio.begin();
    radio.setChannel(10);                                  // Указываем канал передачи данных (от 0 до 127), 5 - значит передача данных осуществляется на частоте 2,405 ГГц (на одном канале может быть только 1 приёмник и до 6 передатчиков)
    radio.setDataRate     (RF24_1MBPS);                   // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
    radio.setPALevel      (RF24_PA_HIGH);                 // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
    radio.openWritingPipe (0x1234567DABLL);               // Открываем трубу с идентификатором 0x1234567890 для передачи данных (на одном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)
    //radio.stoptListening(); // try to uncomment this if radio doesn't work
}
void loop(){

    xPosition = analogRead(yPin);
    yPosition = analogRead(xPin);
    PushButton = digitalRead(zPin);

    if (PushButton == 1) {switcher = -1*switcher; delay(100);} // При нажатии кнопки переписываем значение статуса орудия
    data[3] = switcher;

    Serial.print("Push ");
    Serial.println(data[3]);
    
    data[0] = 0; // dummy value. Instead you can send some meaningful value - button state, for example

    data[1] = xPosition;
    data[2] = yPosition;

    Serial.print("X1 ");
    Serial.println(data[1]);
    Serial.print("Y1 ");
    Serial.println(data[2]);

    if (xPosition >= 650) {data[1] = map(xPosition, 650, 1023, 0, 1000);}
    else if (xPosition <= 350) {data[1] = map(xPosition, 0, 350, -1000, 0);}
    else {data[1] = 0;}

    Serial.print("X ");
    Serial.println(data[1]);

    if (yPosition >= 600) {data[2] = map(yPosition, 600, 1023, 0, -1000);}
    else if (yPosition <= 400) {data[2] = map(yPosition, 400, 0, 0, 1000);}
    else {data[2] = 0;}

    Serial.print("Y ");
    Serial.println(data[2]);

    delay(10);
   
    radio.write(&data, sizeof(data));  // указывая сколько байт массива мы хотим отправить. с проверкой их доставки: if( radio.write(&data, sizeof(data)) ){ приняты;}else{не приняты приёмником;}

}

