int dac_sdi = 7;
int dac_sdo = 8;
int dac_sync = 9;
int csbi = 10;
int sdi = 11;
int sdo = 12;
int scki = 14;
int charger_detect = 16;
int load_switch = 17;
int charge_switch = 18;
int power_switch = 19;
int power_led = 21;
int precharge_switch = 22;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  pinMode(dac_sdi, OUTPUT);
  pinMode(dac_sdo, INPUT);
  pinMode(dac_sync, OUTPUT);
  pinMode(csbi, OUTPUT);
  pinMode(sdi, OUTPUT);
  pinMode(sdo, INPUT);
  pinMode(scki, OUTPUT);
  pinMode(charger_detect, INPUT);
  pinMode(load_switch, OUTPUT);
  pinMode(charge_switch, OUTPUT);
  pinMode(power_switch, INPUT);
  pinMode(power_led, OUTPUT);
  pinMode(precharge_switch, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
}
