/* FreeRTOS.org includes. */

#include "Arduino_FreeRTOS.h"
//#include "task.h"
#include "queue.h"

/* Demo includes. */
//#include "basic_io_avr.h"
#include <Wire.h>
#include "DHT.h"
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0X27,16,2); //SCL A5 SDA A4


//define
#define DHTTYPE DHT11
#define DHTPIN 48
#define LED1 49
#define LED2 50
#define FUN 51





DHT dht(DHTPIN, DHTTYPE);

char data_DHT11[10] = {'\0'};
String data_str = String(data_DHT11);
const char *data = data_str.c_str();
char value_pin_led1 = '0', value_pin_led2='0', value_pin_fun='0';
float hum = 0;
float temp = 0;
int per = 0;
/* The task function. */
void vTaskControl( void *pvParameters );
void vTask_dht( void *pvParameters );

void vTaskLCD(void *pvParameters);



TaskHandle_t HandleBuzzer;
TaskHandle_t Handledht;
TaskHandle_t HandleLCD;


QueueHandle_t xQueuetemp;
QueueHandle_t xQueuehum;


void setup() {
  Wire.begin(8);  
   /* join i2c bus with address 8 */
  Wire.onReceive(receiveEvent); /* register receive event */
  Wire.onRequest(requestEvent);
  Serial.begin(9600);
  Wire.begin();
  dht.begin();
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(FUN, OUTPUT);
  //lcd
  lcd.init();                    
  lcd.backlight();
  lcd.setCursor(2,0);
  lcd.print("start"); 


  xQueuetemp = xQueueCreate( 10, sizeof( long) );
  xQueuehum = xQueueCreate( 10, sizeof( long ) );
  
  if( xQueuetemp != NULL && xQueuehum != NULL )
  {

  xTaskCreate( vTaskDht, "Task DHT", 500, NULL, 1,&Handledht );
  xTaskCreate( vTaskLCD, "Task LCD", 500, NULL, 2,&HandleLCD );

  }

 
  xTaskCreate( vTaskControl, "Task Control", 500, NULL, 3, &HandleBuzzer );
  /* Start the scheduler so our tasks start executing. */
  vTaskStartScheduler();
}


void vTaskControl(void *pvParameters)
{ 
   
  for(;;)
  {
    Serial.print("buzzer\n");
  // Buzzer
  if (per >= 25 || value_pin_fun == '1')
  {
    digitalWrite(FUN, HIGH);
  }
  else
  {
    digitalWrite(FUN, LOW);
  }
  // LED 1
  if (value_pin_led1 == '1')
  {
    digitalWrite(LED1, HIGH);
  }
  else
  {
    digitalWrite(LED1, LOW);
  }
  // LED 2
    if (value_pin_led2 == '1')
  {
    digitalWrite(LED2, HIGH);
  }
  else
  {
    digitalWrite(LED2, LOW);
  }
  
  vTaskDelay(250/ portTICK_PERIOD_MS );
  }
}


void vTaskDht(void *pvParameters)
{
//  portBASE_TYPE xStatustemp;
//  portBASE_TYPE xStatushum;
  long send_temp;
  long send_hum;
  
  for(;;)
  {
    Serial.print("dht\n");
   hum = dht.readHumidity();
  /*Read temperature*/
   temp = dht.readTemperature();
   int value = analogRead(A0);   //đọc giá trị điện áp ở chân A0 - chân cảm biến
                            //(value luôn nằm trong khoảng 0-1023)
   per = map(value,0,1023,0,100);

   if(isnan(hum))
   {
      temp = 0;
      hum = 0;
   }
   send_temp = long(temp);
   send_hum = long(hum);
   xQueueSendToBack( xQueuetemp,&send_temp, 0 );
   xQueueSendToBack( xQueuehum,&send_hum, 0 );
   sprintf(data_DHT11, "%d,%d", int(hum), int(temp));
   data_str = String(data_DHT11);
   data = data_str.c_str();
   Serial.print(data);
   Serial.println(); 
   vTaskDelay(250/portTICK_PERIOD_MS );
   taskYIELD();
  }
}

void vTaskLCD(void *pvParameters)
{
//  portBASE_TYPE xStatustemp;
//  portBASE_TYPE xStatushum;
  
  long queue_recei_temp;
  long queue_recei_hum;
  
  const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS;
  for(;;)
  {
    xQueueReceive( xQueuetemp, &queue_recei_temp, xTicksToWait );
    xQueueReceive( xQueuehum, &queue_recei_hum, xTicksToWait );
    Serial.print("LCD\n");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("hum:");
    lcd.print(int(queue_recei_hum));
    lcd.print("%");
    lcd.setCursor(0,1);
    lcd.print("temp:");
    lcd.print(int(queue_recei_temp));
    lcd.print(char(223));
    lcd.print("C");
    vTaskDelay(500/portTICK_PERIOD_MS );
  }
}

  

void receiveEvent(int howMany) {
 int count = 0;
 while (0 < Wire.available()) {
    char value_pin = Wire.read();      /* receive byte as a character */
    if (value_pin == ',')
    {
      continue;
    }
    count += 1;
    if (count == 1)
    {
      value_pin_led1 = value_pin;
    }
    else if (count == 2)
    {
      value_pin_led2 = value_pin;
    }
    else
    {
      value_pin_fun = value_pin;
    }
  }

} 

// function that executes whenever data is requested from master
void requestEvent() {
 Serial.print("receicei---------");
 Wire.write(data);  /*send string on request */
}
  


void loop()
{
}
