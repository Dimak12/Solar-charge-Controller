#include <LiquidCrystal.h>
//Define pins
const int pwmOutputPin = 11;
const int solarPanelPin = A0; 
const int batt_input_pin = A1;
const int curr_ctrl_output = A2;


int dutyCycle = 0;
const int update_interval = 15;
unsigned long LCD_previous_update = 0;
//LCD setup
LiquidCrystal lcd(13,12,5,4,3,2);

const double solarCellVoltsPerDigit = 0.02357; //23.57mV per bit 
const double batteryVoltPerBit = 0.01526; // 15.26mV per bit
const double maxBatteryVolt = 14.4; //maximum charge voltage
double vSolar;
double vbattery1;
double vBattery2;
double load_voltage;
double current;
//Status of the battery
char batteryState1[] = "Battery Charged";
char batteryState2[] = "Charging......";
char batteryState3[] = "Low Solar Volt";


void setup() {

  pinMode(pwmOutputPin, OUTPUT);
  pinMode(solarPanelPin, INPUT);
  pinMode(batt_input_pin, INPUT);
  Serial.begin(9600);
  //Initialize LCD
  lcd.begin(16,2);
  lcd.print("MAKALI 220344817");

}

void loop() {
    
    voltage_calibration();
    charge_control();

    //print load voltage to serial monitor
    Serial.print("Load Voltage: ");
    Serial.print(load_voltage);
    Serial.print("V");
    Serial.println();
    
}

void voltage_calibration(){
    //Voltage calibrated     
    vSolar = analogRead(solarPanelPin) * solarCellVoltsPerDigit;
    vbattery1 = analogRead(curr_ctrl_output) * batteryVoltPerBit;
    vBattery2 = analogRead(batt_input_pin) * batteryVoltPerBit;
    /*calculate the voltage drop across 1 ohm resistor which is 
    the current going into the battery, then converts it to mA*/
    current = (vbattery1 - vBattery2)*1000;
    //measure the load voltage
    load_voltage = analogRead(A3)*0.00489;
    
}
//start and stop charging automatically
void charge_control(){
    
    //charge kept at approximately 14.4V 
    //charge stable under different conditions
    if(vBattery2 > maxBatteryVolt){
        //Stop charging automatically
        dutyCycle = 0;
        analogWrite(pwmOutputPin, dutyCycle);
        check_interval(batteryState1);

    }
    else{
        //stop charging automatically when voltage low across solar panel
        if(vBattery2 >= vSolar){
            dutyCycle = 0;
            analogWrite(pwmOutputPin, dutyCycle);
            check_interval(batteryState3);
        }
        //start charging automatically
        else{
            dutyCycle = 60;
            analogWrite(pwmOutputPin, dutyCycle);
            check_interval(batteryState2);
        }
    }

}

//LCD is displaying
void lcd_output(char batteryState []){

    lcd.clear();
    lcd.setCursor(0,0);
    //show voltage charged
    lcd.print(batteryState);
    //LCD is displaying voltage and current
    lcd.setCursor(0,1);
    lcd.print("B:");
    lcd.print(vBattery2,1);
    lcd.print("V");
    lcd.print(" I:");
    lcd.print(current,1);
    lcd.print("mA");

}
//Track previous update time
void check_interval(char batteryState []){

    if(update_interval <= millis() - LCD_previous_update) {
    //print to LCD
    lcd_output(batteryState);
    LCD_previous_update = millis();

  	}
}