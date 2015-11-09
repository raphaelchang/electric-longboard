

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

IntervalTimer timer;
boolean main_power = false;
boolean precharging = false;
int precharge_timer = 0;
int precharge_time = 0;

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
  *portConfigRegister(power_switch) |= PORT_PCR_PE; //pull enable
  *portConfigRegister(power_switch) &= ~PORT_PCR_PS; //pull down
  pinMode(power_led, OUTPUT);
  pinMode(precharge_switch, OUTPUT);
  analogReadResolution(16);
  attachInterrupt(power_switch, power, RISING);
  analogReadAveraging(1000);
  timer.begin(main_loop, 1000);
}

void power()
{
  delay(10);
  if (digitalRead(power_switch) == LOW)
    return;
  main_power = !main_power;
  if (main_power)
  {
    precharging = true;
    precharge_timer = 0;
  }
}

void main_loop()
{
  Serial.println((analogRead(A1) - 32768) / 32768.0 * 1.65 / 20 / 0.0003);
    
  if (main_power)
  {
  digitalWrite(power_led, HIGH);
    if (precharging)
    {
      digitalWrite(precharge_switch, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
      if (precharge_timer++ > precharge_time)
      {
        precharging = false;
        digitalWrite(precharge_switch, LOW);
        digitalWrite(load_switch, HIGH);
      }
    }
    else
    {
      digitalWrite(precharge_switch, LOW);
      digitalWrite(load_switch, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
    }
  }
  else
  {
    digitalWrite(power_led, LOW);
    digitalWrite(load_switch, LOW);
    digitalWrite(precharge_switch, LOW);
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void loop() {
}

