#include <SPI.h>
#include <Servo.h>

Servo myservo;
Servo myservo2;

// steping mortor
#define PIN_SPI_MOSI 11
#define PIN_SPI_MISO 12
#define PIN_SPI_SCK 13
#define PIN_SPI_SS 10
#define PIN_BUSY 9

// button
#define START 4  //[起]
#define ROTOR 5  //[廻]
#define END 6    //[終]

void setup()
{
  delay(1000);
  pinMode(PIN_SPI_MOSI, OUTPUT);
  pinMode(PIN_SPI_MISO, INPUT);
  pinMode(PIN_SPI_SCK, OUTPUT);
  pinMode(PIN_SPI_SS, OUTPUT);
  pinMode(PIN_BUSY, INPUT);
  SPI.begin();
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
  digitalWrite(PIN_SPI_SS, HIGH);

  L6470_resetdevice();
  L6470_setup();

  myservo.attach(3);
  myservo2.attach(2);

  pinMode(START, INPUT_PULLUP);
  pinMode(ROTOR, INPUT_PULLUP);
  pinMode(END, INPUT_PULLUP);
}

void loop() {
  deactive();

  //Boot button[起]
  if (digitalRead(START) == HIGH) {
    boot();

    //start flapping loop
    while (1) {
      flapping();

      // start rotate button[廻]
      if ( digitalRead(ROTOR) == HIGH) {
        rotate();

        // end button[終]
      } else if (digitalRead(END) == HIGH) {
        end_motion();
        break;
      }
    }

    //posing button[廻]
  } else if (digitalRead(ROTOR) == HIGH) {
    while (1) {
      posing();

      //end button[終]
      if (digitalRead(END) == HIGH) {
        break;
      }
    }
  }
}

void deactive() {
  myservo.write( 180 );
  myservo2.write( 180 );
  delay(2000);
}

void boot() {
  myservo.write( 90 );
  myservo2.write( 90 );
  delay(500);

  L6470_move(1, 20);
  myservo2.write( 180 );
  delay(300);
  L6470_move(0, 470);
  L6470_move(1, 50);
}

void flapping() {
  myservo.write( 90 );
  myservo2.write( 90 );
  delay(400);
  myservo.write( 0 );
  myservo2.write( 0 );
  delay(400);
}

void rotate() {
  delay(1000);
  myservo.write( 180 );
  myservo2.write( 150 );
  delay(400);
  for (int cw = 0; cw < 16; cw++) {
    L6470_move(0, 25);
    myservo.write( 90 );
    delay(300);
    L6470_move(0, 25);
    myservo.write( 0 );
    delay(300);
  }
  for (int ccw = 0; ccw < 16; ccw++) {
    L6470_move(1, 25);
    myservo.write( 90 );
    delay(300);
    L6470_move(1, 25);
    myservo.write( 0 );
    delay(300);
  }
}

void end_motion () {
  myservo.write( 90 );
  myservo2.write( 120 );
  delay(400);
  L6470_move(0, 20);
  L6470_move(1, 470);
  L6470_move(0, 50);
  delay(400);
  myservo.write( 0 );
  delay(400);
  myservo.write( 180 );
  delay(400);
}

void posing () {
  myservo.write( 120 );
  myservo2.write( 120 );
  delay(400);
}

void L6470_setup() {
  L6470_setparam_acc(0x20);
  L6470_setparam_dec(0x20);
  L6470_setparam_maxspeed(0x20);
  L6470_setparam_minspeed(0x0);
  L6470_setparam_fsspd(0x3ff);
  L6470_setparam_kvalhold(0xff);
  L6470_setparam_kvalrun(0xff);
  L6470_setparam_kvalacc(0xff);
  L6470_setparam_kvaldec(0xff);

  L6470_setparam_stepmood(0x01);
}
