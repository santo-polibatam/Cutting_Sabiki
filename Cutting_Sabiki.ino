#include <EEPROMex.h>
#include <EEPROMVar.h>
#include <Encoder.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display
#define speed 1000
#define Mtr_DC 9
#define Mtr_Stepper 13
#define Dir_Stepper 12
#define SW_Knop A2
#define SW_Start 11
#define SW_Stop 10
#define Time_To_Press 30

bool Start_Machine = 0;

long Enc_Data = 0;
int Press_Wait = 0;
bool Status_mesin = 0;
long Setting_Lenght = 0;

//Encoder
Encoder Knop(A0, A1);
void setup()
{
    //Serial
    Serial.begin(115200);
    //Motor dc
    pinMode(Mtr_DC, OUTPUT);
    pinMode(Mtr_Stepper, OUTPUT);

    //Knop encoder
    pinMode(SW_Knop, INPUT);
    digitalWrite(SW_Knop, HIGH);

    //Button
    pinMode(SW_Start, INPUT);
    pinMode(SW_Stop, INPUT);
    digitalWrite(SW_Start, HIGH);
    digitalWrite(SW_Stop, HIGH);

    //LCD
    lcd.init(); // initialize the lcd
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Cutting Tinsel");
    lcd.setCursor(2, 1);
    lcd.print("Sanindotekno");
    delay(1500);
    lcd.clear();

    Serial.println("Program Start");
    Setting_Lenght = EEPROM.readLong(0);

    // for (int i = 0; i < 11; i++)
    // {
    //     EEPROM.writeLong(i,0);
    // }
}

void loop()
{
    char buffer[18];

    lcd.setCursor(0, 0);
    lcd.print("Status:");
    if (Status_mesin)
    {
        lcd.setCursor(7, 0);
        lcd.print("Running... ");
        for (int i = 0; i < Setting_Lenght*10; i++)
        {
            digitalWrite(Dir_Stepper, HIGH);
            digitalWrite(Mtr_Stepper, HIGH);
            delayMicroseconds(600);
            digitalWrite(Mtr_Stepper, LOW);
            delayMicroseconds(600);
            if(digitalRead(SW_Stop) == 0)break;
        }
        delay(1000);
        digitalWrite(Mtr_DC, HIGH);
        delay(1000);
        digitalWrite(Mtr_DC, LOW);
        
    }
    else
    {
        lcd.setCursor(7, 0);
        lcd.print("Stoped   ");
    }
    lcd.setCursor(0, 1);
    sprintf(buffer, "Seting:%d mm  ", Setting_Lenght);
    lcd.print(buffer);

    //cek Start Stop
    if (digitalRead(SW_Knop) == 0)
    {
        Press_Wait = 0;
        Status_mesin = 0;
        Menu();
    }
    else if (Press_Count(digitalRead(SW_Start) == 0) >= Time_To_Press)
    {
        Status_mesin = 1;
        Press_Wait = 0;
    }
    else if (digitalRead(SW_Stop) == 0)
    {
        Status_mesin = 0;
    }
}

void Run(long Setting_Lenght)
{
}

void Menu()
{

    int menu_no = 1;
    long Set_data = EEPROM.readLong(menu_no * 10);
    bool flag = 0;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Entering Menu......");
    delay(1500);
    lcd.clear();
    display_menu(menu_no, Set_data, flag);
    while (1)
    {
        if (digitalRead(SW_Start) == 0)
        {
            if (menu_no < 10 && flag == 0)
            {
                menu_no++;
                Set_data = EEPROM.readLong(menu_no * 10);
            }
            else if (flag == 1)
            {
                Set_data++;
            }
            while (digitalRead(SW_Start) == 0)
            {
            }
            display_menu(menu_no, Set_data, flag);
            delay(300);
        }
        else if (digitalRead(SW_Stop) == 0)
        {
            if (menu_no > 1 && flag == 0)
            {
                menu_no--;
                Set_data = EEPROM.readLong(menu_no * 10);
            }
            else if (flag == 1)
            {
                Set_data--;
            }
            while (digitalRead(SW_Stop) == 0)
            {
            }
            display_menu(menu_no, Set_data, flag);
            delay(300);
        }
        else if (digitalRead(SW_Knop) == 0)
        {
            long count = 0;
            while (digitalRead(SW_Knop) == 0)
            {
                count++;
                if (count >= 500000)
                {
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Saving Data...");
                    EEPROM.writeLong(menu_no * 10, Set_data);
                    EEPROM.writeLong(0, EEPROM.readLong(menu_no * 10));
                    Setting_Lenght = EEPROM.readLong(0);
                    delay(1000);
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Exit...");
                    delay(1000);
                    lcd.clear();
                    return;
                }
            }

            flag = !flag;
            display_menu(menu_no, Set_data, flag);
        }
    }
}

void display_menu(int menu_no, long Set_data, bool flag)
{
    char buffer[18];
    lcd.setCursor(0, 0);
    sprintf(buffer, "Menu No:%d ", menu_no);
    lcd.print(buffer);
    lcd.setCursor(0, 1);
    sprintf(buffer, "Data   :%d mm    ", Set_data);
    lcd.print(buffer);
    lcd.setCursor(8, flag);
    lcd.blink();
}

int Press_Count(int pin)
{
    if (pin)
        Press_Wait++;
    return Press_Wait;
}
