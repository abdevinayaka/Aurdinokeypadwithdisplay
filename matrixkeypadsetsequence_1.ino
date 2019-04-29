#include <Keypad.h>
#include <EEPROM.h>
const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns
// Define the Keymap
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
//byte rowPins[ROWS] = { 9, 8, 7, 6 };
byte rowPins[ROWS] = { 2, 3, 4, 5 };
// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
//byte colPins[COLS] = { 12, 11, 10 }; 
byte colPins[COLS] = { 6, 7, 8 }; 

// Create the Keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

#define ledpin 13
#define lockpin 11
int inpt[8];
short ep1=0;
int curnt=0;
long num=0;
long code=0;
long saved_code=0;
unsigned long last_key_time;
long setSequence=0;

void setup()
{
  pinMode(ledpin,OUTPUT);
  pinMode(lockpin,OUTPUT);
  digitalWrite(ledpin, HIGH);
  digitalWrite(lockpin, LOW);
  Serial.begin(9600);
  int val = EEPROM.read(0);
  Serial.print("Starting; current EEPROM value is ");
  Serial.println(val);
  if(val != 1){
    Serial.println("EEPROM byte not set yet; Writing...");
    EEPROM.write(0, 1);
    EEPROMWritelong(1, 10000);
    saved_code = 10000;
  } else if (val == 1000){
    Serial.println("EEPROM byte was set!");
  }
  saved_code = EEPROMReadlong(1);
  Serial.println("Done.");
  last_key_time = millis();
  Serial.println(saved_code);
 
}

void loop()
{
  char key = kpd.getKey();
  if(key)  // Check for a valid key.
  {
    switch (key)
    {
      case '*':
        //digitalWrite(ledpin, LOW);
        curnt=0;
        num=0;
        break;
      case '#':
        //digitalWrite(ledpin, HIGH);
        curnt=0;
        num=0;
        break;
      default:
      if((millis()-last_key_time)>10000){
        curnt=0;
        num=0;
        Serial.println("old key");
        }
        last_key_time = millis();
      
      num = num * 10 + (key - '0');
        inpt[curnt]=key-'0';
        curnt++;
        for(int i=0;i<8;i++){
          Serial.println(inpt[i]);
          }
          if(curnt==8){
            code = ((num/1000) ^ (num%1000));
            curnt=0;
            Serial.println("decrypting");
            if(num == 22200071){
              Serial.println("correct master code!!");
              digitalWrite(ledpin, LOW);
              digitalWrite(lockpin, HIGH);
              delay(2000);
              digitalWrite(ledpin, HIGH);
              digitalWrite(lockpin, LOW);
              }
             if(num == 92200071){
              Serial.println("correct master code!!");
              EEPROMWritelong(1, 10000);
              saved_code = 10000;
             }

             if(setSequence == 98798760){
              EEPROMWritelong(1, code);
              saved_code = code;
              Serial.println("Sequence has been set!!");
              digitalWrite(ledpin, LOW);
              digitalWrite(lockpin, HIGH);
              delay(2000);
              digitalWrite(ledpin, HIGH);
              digitalWrite(lockpin, LOW);
              setSequence = 0;
            }

            if(num == 98798760){
              setSequence = num;
            }
             
            num=0;
            if( ( (code > saved_code) &&
                  (code < (saved_code+20))
                ) || 
                ( (code < saved_code) &&
                  (code > (saved_code-3))
                )
               ){
              Serial.println("correct code!!");
              EEPROMWritelong(1, code);
              saved_code = code;
              digitalWrite(ledpin, LOW);
              digitalWrite(lockpin, HIGH);
              delay(2000);
              digitalWrite(ledpin, HIGH);
              digitalWrite(lockpin, LOW);
              }else{
                Serial.println("incorrect code!!");
                }
            Serial.println(code);
            Serial.println(saved_code);
            }
        Serial.println(num);
    }
  }
}
void EEPROMWritelong(int address, long value)
      {
      //Decomposition from a long to 4 bytes by using bitshift.
      //One = Most significant -> Four = Least significant byte
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);

      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address, four);
      EEPROM.write(address + 1, three);
      EEPROM.write(address + 2, two);
      EEPROM.write(address + 3, one);
      }
long EEPROMReadlong(long address)
      {
      //Read the 4 bytes from the eeprom memory.
      long four = EEPROM.read(address);
      long three = EEPROM.read(address + 1);
      long two = EEPROM.read(address + 2);
      long one = EEPROM.read(address + 3);

      //Return the recomposed long by using bitshift.
      return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
      }
