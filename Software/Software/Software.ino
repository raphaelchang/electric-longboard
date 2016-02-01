#include <Wire.h>

/* Debug definitions */
#define PRINT_NUNCHUK_DATA
#define WIRELESS_KAMA_NUNCHUK    /* Comment out this line if using a Wired Nunchuk */

#define READ_DELAY        5      /* (milliseconds) - Increase this number to not read the nunchuk data so fast */
#define SERIAL_BAUD_RATE  115200

unsigned int joy_x = 0;
unsigned int joy_y = 0;
unsigned int acc_x = 0;
unsigned int acc_y = 0;
unsigned int acc_z = 0;
unsigned int btn_c = 0;
unsigned int btn_z = 0;
double speed = 0;
double acceleration = 0.02;
double decay = 0.98;

unsigned long previous_read_time = 0;

void setup()
{ 
   Serial.begin(SERIAL_BAUD_RATE);
   Serial1.begin(SERIAL_BAUD_RATE);
   Wire.begin();

   delay(10000);
   initialize_nunchuk();

   previous_read_time = millis();
}

void loop()
{
   if (millis() - previous_read_time > READ_DELAY)
   {
       read_nunchuk_data();
       if (btn_c == 1 && btn_z == 1)
       {
         double accel = ((int)joy_y - 128) / 128.0;
         Serial.println(accel);
         speed *= decay;
         speed += accel * acceleration;
         if (speed > 1.0)
          speed = 1.0;
         else if (speed < -1.0)
          speed = -1.0;
         Serial.println(speed);
         int out = (int)((speed + 1.0) * 500);
         Serial.println(out);
         Serial1.print(out);
         Serial1.print("\n");
       } else
       {
        speed = 0;
         Serial1.print("c\n");
       }
       previous_read_time = millis();
   }
}

void read_nunchuk_data()
{
   unsigned int buffer[6];
   byte buffer_index = 0;
 
   Wire.beginTransmission(0x52);
   Wire.send(0x00);
   Wire.endTransmission();
   
#ifndef WIRELESS_KAMA_NUNCHUK    
   delay(1); /* This delay is required for a wired nunchuk otherwise the data will appear maxed out */
#endif
 
   Wire.requestFrom(0x52, 6);
   while(Wire.available())    
   {
       buffer[buffer_index] = Wire.receive();
       buffer_index++;
   }
   
   joy_x = buffer[0];
   joy_y = buffer[1];
   acc_x = ((buffer[2] << 2) | ((buffer[5] & 0x0C) >> 2) & 0x03FF);
   acc_y = ((buffer[3] << 2) | ((buffer[5] & 0x30) >> 4) & 0x03FF);
   acc_z = ((buffer[4] << 2) | ((buffer[5] & 0xC0) >> 6) & 0x03FF);
   btn_c = !((buffer[5] & 0x02) >> 1);
   btn_z = !(buffer[5] & 0x01);
   
#ifdef PRINT_NUNCHUK_DATA
   Serial.print("    "); Serial.print(joy_x);
   Serial.print("      "); Serial.print(joy_y);
   Serial.print("      "); Serial.print(acc_x);
   Serial.print("       "); Serial.print(acc_y);
   Serial.print("       "); Serial.print(acc_z);
   Serial.print("        "); Serial.print(btn_c);
   Serial.print("          "); Serial.print(btn_z); 
   Serial.println("");
#endif
}

void initialize_nunchuk()
{
#ifdef WIRELESS_KAMA_NUNCHUK
   Wire.beginTransmission(0x52);
   Wire.send (0xF0);
   Wire.send (0x55);
   Wire.endTransmission();
   delay(30);
 
   Wire.beginTransmission (0x52);
   Wire.send (0xFB);
   Wire.send (0x00);
   Wire.endTransmission();
   delay(30);
   
   Wire.beginTransmission(0x52);
   Wire.send (0xFA);
   Wire.endTransmission();
   delay(30);
 
   Wire.requestFrom(0x52, 6);
   Serial.print("Device ID is: ");
   while(Wire.available())  
   {
       byte c = Wire.receive(); 
       Serial.print(c, HEX);      
       Serial.print(" ");
   }
   delay(30);
   
#else
   Wire.beginTransmission(0x52);
   Wire.send (0x40);      
   Wire.send (0x00);      
   Wire.endTransmission();
   delay(30);
#endif

#ifdef PRINT_NUNCHUK_DATA    
   Serial.println("");  
   Serial.println("  X-axis   Y-axis   X-accel   Y-accel   Z-accel   C-button   Z-button");
#endif
}

