#include <DS3231.h>
#include <Servo.h>
Servo myServo;
int pos = 0;
#define DS3231_I2C_ADDRESS 0x68
// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return ( (val / 10 * 16) + (val % 10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return ( (val / 16 * 10) + (val % 16) );
}
unsigned long init_time = 0;
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
struct ts {
  uint8_t sec;         /* seconds */
  uint8_t min;         /* minutes */
  uint8_t hour;        /* hours */
  uint8_t mday;        /* day of the month */
  uint8_t mon;         /* month */
  int16_t year;        /* year */
  uint8_t wday;        /* day of the week */
  uint8_t yday;        /* day in the year */
  uint8_t isdst;       /* daylight saving time */
  uint8_t year_s;      /* year in short notation*/
#ifdef CONFIG_UNIXTIME
  uint32_t unixtime;   /* seconds since 01.01.1970 00:00:00 UTC*/
#endif
};
struct ts t;

const byte ROWS = 4;
const byte COLS = 4;
const int reedPin = 11;
bool switchState = HIGH;
int total = 0;
int Open = 0;
int closed = 0;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3};

byte rowPins2[ROWS] = {31, 30, 29, 28};
byte colPins2[COLS] = {27, 26, 25, 24};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
Keypad customKeypad2 = Keypad(makeKeymap(hexaKeys), rowPins2, colPins2, ROWS, COLS);
#include <Wire.h>
//#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);
class systemTime{
  int timeElapsed = 0;
  public: 
  void incrementElapse(){timeElapsed +=1;}
  int getElapse(){return timeElapsed;}
  };

class Home
{
    String message = "Welcome to the iSecurity Security System";
    String password = " ";
    bool doorOpen = false;
    int inHouse = 3;
    //1: inHouse
    //2: Out ofHouse
    //3: Entering House
    //4: Exiting House
    bool newUser = true;
  public:
    String getMessage() {
      return message;
    }
    String getPassword() {
      return password;
    }
    void setPassword(String pass) {
      password = pass;
    }
    int getHouseStatus() {
      return inHouse;
    }
    void setHouseStatus(int val) {
      inHouse = val;
    }
    bool getUserStatus() {
      return newUser;
    }
    void setUserStatus(bool state) {
      newUser = state;
    }
    void append(char character) {
      password += character;
    }
};
Home homebaybee;
systemTime sysTime;
void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
                   dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}
void readDS3231time(byte *second,
                    byte *minute,
                    byte *hour,
                    byte *dayOfWeek,
                    byte *dayOfMonth,
                    byte *month,
                    byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}
void updateDelay(int milseconds){
    init_time = millis();
    while(millis() - init_time < milseconds){
      digitalWrite(15, digitalRead(16));
      }
    
    
  }
void displayTime()
{
  sysTime.incrementElapse();
  Serial.print("Time Elapsed: ");
  Serial.print(sysTime.getElapse());
  Serial.print(" ");
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
                 &year);
  // send it to the serial monitor
  Serial.print(hour, DEC);
  // convert the byte variable to a decimal number when displayed
  Serial.print(":");
  if (minute < 10)
  {
    Serial.print("0");
  }
  Serial.print(minute, DEC);
  Serial.print(":");
  if (second < 10)
  {
    Serial.print("0");
  }
  Serial.print(second, DEC);
  Serial.print(" ");
  Serial.print(dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(year, DEC);
  Serial.print(" Day of week: ");
  switch (dayOfWeek) {
    case 1:
      Serial.println("Sunday");
      break;
    case 2:
      Serial.println("Monday");
      break;
    case 3:
      Serial.println("Tuesday");
      break;
    case 4:
      Serial.println("Wednesday");
      break;
    case 5:
      Serial.println("Thursday");
      break;
    case 6:
      Serial.println("Friday");
      break;
    case 7:
      Serial.println("Saturday");
      break;
  }
}
void setup()
{
  Wire.begin();
  Serial.begin(9600);
  setDS3231time(30, 42, 21, 4, 26, 11, 14);
  //displayTime();
  /*while(true){
    displayTime(); // display the real-time clock data on the Serial Monitor,
    updateDelay(1000); // every second
    }*/
  
  myServo.attach(23);
  //clock.begin();
  //DS3231_init(DS3231_INTCN);
  // set the initial time here:
  // DS3231 seconds, minutes, hours, day, date, month, year

  pinMode(reedPin, INPUT);


  // initialize the LCD
  lcd.begin();
  lcd.setCursor(0, 0);
  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print(__TIME__);
  updateDelay(5000);
  lcd.clear();
  Serial.println(homebaybee.getPassword());
  if (homebaybee.getUserStatus()) {
    lcd.print(homebaybee.getMessage());
    updateDelay(2000);
    lcd.clear();
    lcd.print("Get ready to set up your password");
    updateDelay(2000);
    lcd.clear();
    lcd.print("Using the Keypad");
    lcd.print(", enter a number to");
    lcd.print("serve as your password");
    //Serial.println("Enter Password: ");
    //String name = Serial.readString();
    //Use 4x4 Keypad to enter string
    char customKey = customKeypad.getKey();
    String s = "";
    while (customKey != '#') {
      customKey = customKeypad.getKey();
      Serial.println(customKey);
      //customKey = customKeypad.getKey();
      if (customKey != '#' && customKey) {
        s += customKey;
      }
    }
    homebaybee.setPassword(s);
    Serial.println(s);
    lcd.clear();
    Serial.println("Password");
    Serial.println(homebaybee.getPassword());
    updateDelay(2000);
    if (homebaybee.getPassword() != " ") {
      lcd.print("Your password has been set");
      updateDelay(2000);
      lcd.clear();
      lcd.print(homebaybee.getPassword());
      updateDelay(2000);
      homebaybee.setUserStatus(false);
      lcd.clear();
    }
    else {
      lcd.print("Error");
      updateDelay(5000);
      lcd.clear();
    }
  }

}

void loop()
{
    digitalWrite(15, digitalRead(16));

  //Use the 4x4 Keypad to Tell the Security System if User is Entering Home, Exiting Home, etc.....
  int userState = homebaybee.getHouseStatus();
  if (userState == 3) {
    lcd.print("Please Enter Correct Password");
    updateDelay(2000);
    //sample_pass indicates where the security system would require input from the 4x4 Keypad
    //String sample_pass = "1233";
    bool correct = false;
    String s2 = "";
    char keypadKey = customKeypad.getKey();
    while (keypadKey != '#') {
      keypadKey = customKeypad.getKey();
      Serial.println(keypadKey);
      //customKey = customKeypad.getKey();
      if (keypadKey != '#' && keypadKey) {
        s2 += keypadKey;
      }
    }
    if (s2 == homebaybee.getPassword()) {
      lcd.clear();
      lcd.print("Congratulations you're now");
      lcd.print("entering your home");
      lcd.clear();
      lcd.print("The Time is....");
      updateDelay(1000);
      lcd.clear();
      lcd.print(__TIME__);
      updateDelay(2000);
      lcd.clear();
      int openCount2 = 0;
      int closedCount2 = 0;
      int totalCount2 = 0;
      //Serial.println("Prior to While lop");
      setDS3231time(30, 42, 21, 4, 26, 11, 14);
      while (true) {
        //Serial.println("Testing if door is open");
        switchState = digitalRead(reedPin);
        if (switchState == LOW)
        {
          Serial.println("Your Door is Closed");
          closedCount2 += 1;
        }
        else
        {
          Serial.println("Your Door is Open");
          openCount2 += 1;

        }
        totalCount2 += 1;
        if (totalCount2 == 50) {
          if (closedCount2 != 50) {
            displayTime();
            lcd.print("Your Door is Open");
            updateDelay(2000);
            lcd.clear();
            
            totalCount2 = 0;
            openCount2 = 0;
            closedCount2 = 0;
          }
          else {
            lcd.print("Your Door is Closed");
            updateDelay(2000);
            lcd.clear();
            totalCount2 = 0;
            openCount2 = 0;
            closedCount2 = 0;
            break;
          }
        }
      }
      homebaybee.setHouseStatus(1);
    }
    else {
      lcd.clear();
      lcd.print("The password you entered is: ");
      lcd.print(s2);
      updateDelay(2000);
      lcd.clear();
      lcd.print("Wrong password. Try again");
      updateDelay(2000);
      while (s2 != homebaybee.getPassword()) {
        //lcd.clear();
        //lcd.print("Please Enter Correct Password");
        s2 = "";
        keypadKey = customKeypad.getKey();
        while (keypadKey != '#') {
          keypadKey = customKeypad.getKey();
          Serial.println(keypadKey);
          //customKey = customKeypad.getKey();
          if (keypadKey != '#' && keypadKey) {
            s2 += keypadKey;
          }
        }
        if (s2 == homebaybee.getPassword()) {
          lcd.clear();
          lcd.print("Congratulations you're now");
          lcd.print("entering your home");
          updateDelay(2000);
          lcd.print("The Time is....");
          updateDelay(1000);
          lcd.clear();
          lcd.print(__TIME__);
          updateDelay(2000);
          int openCount = 0;
          int closedCount = 0;
          int totalCount = 0;
          //Serial.println("Prior to While lop");
          setDS3231time(30, 42, 21, 4, 26, 11, 14);
          while (true) {
            //Serial.println("Testing if door is open");
            switchState = digitalRead(reedPin);
            if (switchState == LOW)
            {
              Serial.println("Your Door is Closed");
              closedCount += 1;
            }
            else
            {
              Serial.println("Your Door is Open");
              openCount += 1;

            }
            totalCount += 1;
            if (totalCount == 50) {
              if (closedCount != 50) {
                displayTime();
                lcd.print("Your Door is Open");
                updateDelay(2000);
                lcd.clear();
                totalCount = 0;
                openCount = 0;
                closedCount = 0;
              }
              else {
                lcd.print("Your Door is Closed");
                updateDelay(2000);
                lcd.clear();
                totalCount = 0;
                openCount = 0;
                closedCount = 0;
                break;
              }
            }
          }

          homebaybee.setHouseStatus(1);
          break;
        }
      }
    }
  }
  else if (userState == 1) {
    lcd.clear();
    lcd.print("You are now in your home!");
    updateDelay(2000);
    lcd.print("The Time is....");
    updateDelay(1000);
    lcd.clear();
    lcd.print(__TIME__);
    updateDelay(2000);
    lcd.clear();

    //To exit Home, User will have to press a button on the 4x4 Keypad
    //
    //String s3 = "";
    char keypadKey22 = customKeypad2.getKey();
    while (keypadKey22 != '*') {
      keypadKey22 = customKeypad2.getKey();
      Serial.println(keypadKey22);
      //customKey = customKeypad.getKey();
      if (keypadKey22 != '*' && keypadKey22) {
        //do_nothing
        Serial.println(" ");
      }
      else if (keypadKey22 == '*') {
        lcd.clear();

        break;
      }
    }
    lcd.clear();
    updateDelay(2000);
    lcd.print("Please Enter the Correct Password");
    updateDelay(500);
    lcd.clear();
    String s33 = "";
    char keypadKey33 = customKeypad2.getKey();
    while (keypadKey33 != '#') {
      keypadKey33 = customKeypad2.getKey();
      Serial.println(keypadKey33);
      //customKey = customKeypad.getKey();
      if (keypadKey33 != '#' && keypadKey33) {
        s33 += keypadKey33;
      }
    }
    if (s33 == homebaybee.getPassword()) {
      lcd.print("Now preparing to leave home");

    }
    else {
      lcd.clear();
      lcd.print("The password you entered is: ");
      lcd.print(s33);
      updateDelay(2000);
      lcd.clear();
      lcd.print("Wrong password. Try again");
      updateDelay(2000);
      while (s33 != homebaybee.getPassword()) {
        s33 = "";
        char keypadValue44 = customKeypad2.getKey();
        while (keypadValue44 != '#') {
          keypadValue44 = customKeypad2.getKey();
          Serial.println(keypadValue44);
          //customKey = customKeypad.getKey();
          if (keypadValue44 != '#' && keypadValue44) {
            s33 += keypadValue44;
          }
        }
      }
      //Once it breaks out of the while loop, we know that the the password is correct
      if (s33 == homebaybee.getPassword()) {
        homebaybee.setHouseStatus(4);
      }


    }
    //bool buttonPressed = true;
    //Value is set to true just for testing


  }
  else if (userState == 4) {
    //Use Reed Switch to Detect if Door is still open. Once Door is closed Transition to State 2

    lcd.clear();
    lcd.print("Leaving the house..");
    int openC = 0;
    int closedC = 0;
    int totalC = 0;
    setDS3231time(30, 42, 21, 4, 26, 11, 14);
    while (true) {
      //Serial.println("Testing if door is open");
      bool reedValue = digitalRead(reedPin);
      if (reedValue == LOW)
      {
        Serial.println("Your Door is Closed");
        closedC += 1;
      }
      else
      {
        Serial.println("Your Door is Open");
        openC += 1;

      }
      totalC += 1;
      if (totalC == 50) {
        if (closedC != 50) {
          displayTime();
          lcd.print("Your Door is Open");
          updateDelay(2000);
          lcd.clear();
          totalC = 0;
          openC = 0;
          closedC = 0;
        }
        else {
          lcd.print("Your Door is Closed");
          updateDelay(2000);
          lcd.clear();
          totalC = 0;
          openC = 0;
          closedC = 0;
          homebaybee.setHouseStatus(2);
          break;
        }
      }
    }

  }
  else if (userState == 2) {
    //run Webcam App to Continously detect threats and make sure no visible/discernable threat is detected
    for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myServo.write(pos);              // tell servo to go to position in variable 'pos'
      updateDelay(15);                       // waits 15ms for the servo to reach the position
    }
    for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      myServo.write(pos);              // tell servo to go to position in variable 'pos'
      updateDelay(15);                       // waits 15ms for the servo to reach the position
    }

    //To Re-Enter the Home, the User will have to press the Start Button on the Outer Keypad
    char s100 = "";
    char keypadValue55 = customKeypad.getKey();
    while (keypadValue55 != '*') {
      keypadValue55 = customKeypad.getKey();
      Serial.println(keypadValue55);
      //customKey = customKeypad.getKey();
      if (keypadValue55 != '*' && keypadValue55) {
        s100 += keypadValue55;
      }
    }
    homebaybee.setHouseStatus(3);
  }
  //donothing baybee
  digitalWrite(15, digitalRead(16));
}
