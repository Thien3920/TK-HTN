
#define BLYNK_TEMPLATE_ID "TMPL09LZQicT"
#define BLYNK_DEVICE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN  "CjzvGoxzkvw0x1F7bExWN3w2wTwLgol7"


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include<SoftwareSerial.h> //Included SoftwareSerial Library

#include <Wire.h>
#include <string.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "AndroidAP";
char pass[] = "akvr5219";

static const uint8_t D1 = 5;
static const uint8_t D2 = 4;
int value_pin_led1 = 0;
int value_pin_led2 = 0;
int value_pin_fun = 0;
char data_pin[5] = {'\0'};
String data_str = String(data_pin);
const char *data_send = data_str.c_str();

void setup(){  
  Serial.begin(9600); /* begin serial for debug */
  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
  Blynk.begin(auth, ssid, pass);

}

// get pin value of led and fun
BLYNK_WRITE(V4) // led 1
{
  value_pin_led1 = param.asInt(); // assigning incoming value from pin V4 to a variable
  if(value_pin_led1 == 0)
  {
    value_pin_led1 = 1;
  }
  else
  {
    value_pin_led1 = 0;
  }

  // process received value
}

BLYNK_WRITE(V8) // led 2
{
  value_pin_led2 = param.asInt(); // assigning incoming value from pin V8 to a variable
  if(value_pin_led2 == 0)
  {
    value_pin_led2 = 1;
  }
  else
  {
    value_pin_led2 = 0;
  }
  // process received value
}

BLYNK_WRITE(V7) // fun
{
  value_pin_fun = param.asInt(); // assigning incoming value from pin V7 to a variable
  if(value_pin_fun == 0)
  {
    value_pin_fun = 1;
  }
  else
  {
    value_pin_fun = 0;
  }
  // process received value
}

void loop() {
  Wire.beginTransmission(8); /* begin with device address 8 */
  sprintf(data_pin, "%d,%d,%d", value_pin_led1, value_pin_led2, value_pin_fun);
  data_str = String(data_pin);
  data_send = data_str.c_str(); 
  Serial.print(data_send);
  Wire.write(data_send);  /* sends hello string */
  Wire.endTransmission();    /* stop transmitting */

  Wire.requestFrom(8, 7); /* request & read data of size 13 from slave */
  String data_receiver;
  while(Wire.available()){
    char c = Wire.read();
    data_receiver.concat(c);
  }
  String str_obj(data_receiver);
  char *data = &str_obj[0];
  char *token = strtok(data, ",");
  String humi, temp;
  humi = String(token);
  // loop through the string to extract all other tokens
  while(token != NULL) {
     token = strtok(NULL, " ");
     temp = String(token);
     break;
  }
  String value_temp = String(temp.toInt());
  Blynk.virtualWrite(V5, humi);
  Blynk.virtualWrite(V6, value_temp);

  Serial.println();
  delay(1000);
}
