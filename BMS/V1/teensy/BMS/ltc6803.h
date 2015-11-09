#include "SPI.h"

#define LT_SPI_START digitalWrite(10,LOW)
#define LT_SPI_END digitalWrite(10,HIGH)
byte config[6] = { 0xE9,0x00,0x00,0xFC,0x71,0xC8 };  //Default config
float current_voltages[12];


#define WRCFG 0x01  // Write config registers
#define RDCFG 0x02  // Read config registers
#define RDCV 0x04   // Read all cell voltages
#define RDCVA 0x06  // Read cells 1-4
#define RDCVB 0x08  // Read cells 5-8
#define LTCADDR 0x80 // LTC's Address - 0000

void setup(){
 pinMode(10, OUTPUT);
 Serial.begin(9600);
 SPI.begin();
 SPI.setBitOrder(MSBFIRST);
 SPI.setDataMode(SPI_MODE3);
 SPI.setClockDivider(SPI_CLOCK_DIV64);
 // Write LTC configs
 LT_SPI_START;
 sendToSPI(LTCADDR);  // Non-broadcast command
 sendToSPI(WRCFG);  // Write config
 sendMultipleToSPI(config, 6);
 LT_SPI_END; 
 // Finish LTC config
}

void loop(){
 // STUFF!
 beginCellVolt(); // Start cell 1-6 conversion
 readCellVolt(current_voltages,LTCADDR);
 for (int i; i<6; i++){
   Serial.print(current_voltages[i]);
   Serial.print(" | ");
 }
Serial.println(" ");
delay(1000);
 
}







// ---- FUNCTIONS ----

void beginCellVolt() {
 //for (int i=17; i<=23; i++){ // Read cells 1-6 to save time
 LT_SPI_START;
 sendToSPI(RDCV);
 delay(15); //Time for conversions, 1.4ms max per cell
 LT_SPI_END;
}

// Reads cell voltage registers  
void readCellVolt(float* cell_voltages, byte board) {
 LT_SPI_START;
 sendToSPI(board);  // Board address is selected
 sendToSPI(RDCV);  // Cell voltages to be read
 byte cvr[8];  // Buffer to store unconverted values
 getMultipleFromSPI(cvr, RDCV, 8);
 LT_SPI_END; 

 // Converting cell voltage registers to cell voltages
 cell_voltages[0] = (cvr[0] & 0xFF) | (cvr[1] & 0x0F) << 8;
 cell_voltages[1] = (cvr[1] & 0xF0) >> 4 | (cvr[2] & 0xFF) << 4;
 cell_voltages[2] = (cvr[3] & 0xFF) | (cvr[4] & 0x0F) << 8;
 cell_voltages[3] = (cvr[4] & 0xF0) >> 4 | (cvr[5] & 0xFF) << 4;
 cell_voltages[4] = (cvr[6] & 0xFF) | (cvr[7] & 0x0F) << 8;
 cell_voltages[5] = (cvr[7] & 0xF0) >> 4 | (cvr[8] & 0xFF) << 4;
 
 for(int i=0;i<6;i++) {
   cell_voltages[i] = cell_voltages[i]*1.5*0.001;
 }
}  

// Send data to LTC
void sendMultipleToSPI(byte * data, int n) {
 for(int i=0; i<n; i++) {
   SPI.transfer(data[i]);
 }
 SPI.transfer(getPEC(data, n));
}

// Get data from LTC
byte * getMultipleFromSPI(byte * data, byte info, int n) {
 for(int i=0; i<n; i++) {
   data[i] = SPI.transfer(info);
 }
 byte pec = SPI.transfer(info);
}

void sendToSPI(byte data) {
 sendMultipleToSPI(&data, 1);
}

void readConfig(byte*config,byte board) {
 LT_SPI_START;   
 sendToSPI(board);  // Board address is selected
 sendToSPI(RDCFG);  // Configuration is read
 getMultipleFromSPI(config, RDCFG, 6);
 LT_SPI_END;
}

// Calculate PEC, n is  size of DIN
byte getPEC(byte * din, int n) {
 byte pec, in0, in1, in2;
 pec = 0x41;
 for(int j=0; j<n; j++) {
   for(int i=0; i<8; i++) {
     in0 = ((din[j] >> (7 - i)) & 0x01) ^ ((pec >> 7) & 0x01);
     in1 = in0 ^ ((pec >> 0) & 0x01);
     in2 = in0 ^ ((pec >> 1) & 0x01);
     pec = in0 | (in1 << 1) | (in2 << 2) | ((pec << 1) & ~0x07);
   }
 }
 return pec;
}

