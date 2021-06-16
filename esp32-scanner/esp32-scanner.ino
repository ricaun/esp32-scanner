/*
  esp32-scanner
  created 16 06 2021
  by Luiz H. Cassettari - ricaun@gmail.com
*/

#include <WiFi.h>
#include <WebServer.h>

/* GOOGLE URL */

//#define GOOGLE_URL "https://script.google.com/macros/s/#######################################################/exec"
#define GOOGLE_URL "https://script.google.com/macros/s/AKfycbyddAxEfARJLdXOD6pLBL410Dzg5SRU0XM0MlzddXO0mj19NAZtgMtV/exec"
#define GOOGLE_TIME 30

/* ANALOG TIME */
#define ANALOG_TIME 10

/* TaskHandle_t */

TaskHandle_t Task0;

/* WIFI */

const char *ssid = "";
const char *password = "";

// ----------------- google ------------------- //

String post_google(String path, String body);
String get_google(String path);

// ----------------- server ------------------- //

WebServer server(80);

const char *script = "<script>function loop() {var resp = GET_NOW('values'); document.getElementById('values').innerHTML = resp; setTimeout('loop()', 1000);} function GET_NOW(get) { var xmlhttp; if (window.XMLHttpRequest) xmlhttp = new XMLHttpRequest(); else xmlhttp = new ActiveXObject('Microsoft.XMLHTTP'); xmlhttp.open('GET', get, false); xmlhttp.send(); return xmlhttp.responseText; }</script>";

void handleRoot()
{
  String str = "";
  str += "<html>";
  str += "<head>";
  str += "<title>ESP32</title>";
  str += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  str += script;
  str += "</head>";
  str += "<body onload='loop()'>";
  str += "<center>";
  str += "<div id='values'></div>";
  str += "</center>";
  str += "</body>";
  str += "</html>";
  server.send(200, "text/html", str);
}

void handleGetValues()
{
  String str = "";
  server.send(200, "text/plain", str);
}

// ----------------- setup ------------------- //

void setup(void)
{
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  if (ssid != "")
    WiFi.begin(ssid, password);
  WiFi.begin();
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/values", handleGetValues);
  server.begin();
  Serial.println("HTTP server started");
  get_body();

  TFMini_setup();
  servo_setup();

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    TaskCode0,   /* Task function. */
    "IDLE0",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task0,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */
}

void TaskCode0( void * pvParameters ) {
  while (1)
  {
    google_loop();
    vTaskDelay(10);
  }
}

void loop(void)
{
  if (analog_runEvery(ANALOG_TIME * 1000))
  {
    //add_body();
  }
  servo_loop();
  server.handleClient();
}

// ----------------- body ------------------- //

static String _body = "";

String add_body()
{
  String body = "";

  body += ";";
  body += get_servo_x(0);
  body += "|";
  body += get_servo_x(1);
  /*
  body += readAnalogPin();
  body += "|";
  body += hx711_read();
  body += "|";
  body += dht_read_temperature();
  body += "|";
  body += dht_read_humidity();
  body += "|";
  body += tsl_read();
  */
  _body += body;
}

String add_body_now(String body)
{
  _body += body;
}

String get_body()
{
  String payload = _body;
  String mac = WiFi.macAddress();
  _body = mac;
  return payload;
}

// ----------------- google ------------------- //

void google_loop()
{
  if (google_runEvery(GOOGLE_TIME * 1000))
  {
    String post = post_google(GOOGLE_URL, get_body());
    Serial.println(post);
    //servo_start();
  }
}

boolean google_runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}

// 

boolean analog_runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}