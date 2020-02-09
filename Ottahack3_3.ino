
  #include <Servo.h> 
  #include <Wire.h> 
  
  //Servo stuff
  Servo myServo;
  
  //Serial input stuff
  const byte s7sAddress = 0x71;
  unsigned int counter = 9900;  // This variable will count up to 65k
  char tempString[10];  // Will be used with sprintf to create strings

  //Global Variables
    //b<Number> = Button pin number
  const int bOne = 3;
  const int bTwo = 4;
  const int bThree = 5;
  const int bFour = 6;
  const int bFive = 7;
  const int bSix = 8;
  
  const int SIZE = 4;
  const int password[4] = {1,2,3,4};
  
void setup() {
  Wire.begin();
  
  //Pin setup
  pinMode(bOne, INPUT);
  pinMode(bTwo, INPUT);
  pinMode(bThree, INPUT);
  pinMode(bFour, INPUT);
  pinMode(bFive, INPUT);
  pinMode(bSix, INPUT);
  
  myServo.attach(9);

  Serial.begin(9600);
  
  clearDisplayI2C();
  s7sSendStringI2C("6969");
  setDecimalsI2C(0b111111);  
  setBrightnessI2C(0);  // Lowest brightness
  delay(1500);
  setBrightnessI2C(255);  // High brightness
  delay(1500);
  clearDisplayI2C(); 

  myServo.write(0);
  
}

void loop() {

  waitInput();
  clearDisplayI2C(); 
  delay(1000);

}

void s7sSendStringI2C(String toSend)
{
  Wire.beginTransmission(s7sAddress);
  for (int i=0; i<4; i++)
  {
    Wire.write(toSend[i]);
  }
  Wire.endTransmission();
}

// Send the clear display command (0x76)
//  This will clear the display and reset the cursor
void clearDisplayI2C()
{
  Wire.beginTransmission(s7sAddress);
  Wire.write(0x76);  // Clear display command
  Wire.endTransmission();
}

// Set the displays brightness. Should receive byte with the value
//  to set the brightness to
//  dimmest------------->brightest
//     0--------127--------255
void setBrightnessI2C(byte value)
{
  Wire.beginTransmission(s7sAddress);
  Wire.write(0x7A);  // Set brightness command byte
  Wire.write(value);  // brightness data byte
  Wire.endTransmission();
}

// Turn on any, none, or all of the decimals.
//  The six lowest bits in the decimals parameter sets a decimal 
//  (or colon, or apostrophe) on or off. A 1 indicates on, 0 off.
//  [MSB] (X)(X)(Apos)(Colon)(Digit 4)(Digit 3)(Digit2)(Digit1)
void setDecimalsI2C(byte decimals)
{
  Wire.beginTransmission(s7sAddress);
  Wire.write(0x77);
  Wire.write(decimals);
  Wire.endTransmission();
}

int getInput()
{
  int number = 0;
  bool one = false;
  bool two = false;
  bool three = false;
  bool four = false;
  bool five = false;
  bool six = false;
  
  while(number == 0){
    one = digitalRead(bOne);
    two = digitalRead(bTwo);
    three = digitalRead(bThree);
    four = digitalRead(bFour);
    five = digitalRead(bFive);
    six = digitalRead(bSix);
    
    if(one){
       number = 1;
    }
    if(two){
      number = 2;
    }
    if(three){
      number = 3;
    }
    if(four){
      number = 4;
    }
    if(five){
      number = 5;
    }
    if(six){
      number = 6;
    }
    delay(100);
    Serial.println("Number not found");
  }

   Serial.println(number);
   return number;
}

void waitInput()
{ 
  int toSend[4] = {0,0,0,0};
  int digit = 0;
  displayUpdate(toSend);
  for(int j = 0; j < 4; j++)
  {
    digit = getInput();
    toSend[j] = digit;
    displayUpdate(toSend);
    delay(150);
  }
  bool equal = true;
  for(int k = 0; k < 4 && equal; k++)
  {
    if(toSend[k] != password[k]){
      equal = false;
    }
  }
  if(equal){
    clearDisplayI2C();
    s7sSendStringI2C("OPEN");
    myServo.write(90);
    getInput();
    myServo.write(0);
  }
  else{
    clearDisplayI2C();
    s7sSendStringI2C("FAIL");
    myServo.write(0);

  }
  delay(1000);
}

void displayUpdate(int toSend[SIZE])
{
  Wire.beginTransmission(s7sAddress);
  for (int i=0; i<4; i++)
  {
    Wire.write(toSend[i]);
  }
  Wire.endTransmission();
}
