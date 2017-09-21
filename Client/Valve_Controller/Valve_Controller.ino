#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <SPI.h>
#include <PubSubClient.h>

//______________________________________
//Declaration of global variables

const char* ssid      = "RKT";
const char* password  = "Unikornis";

String webPage = "";

int Irrigator1 = 0;
int Irrigator2 = 2;
int Pump       = 14;

char* serverIP;// = "192.168.1.7";
char topic[]    = "IRRIGATION";
char clientID[] = "IRRIGATION"; 

MDNSResponder mdns;
ESP8266WebServer  server(80);

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//_____________________________________________________________
//WiFi and MQTT Init

void callback(char* topic, byte* payload, unsigned int length)
{
  //char* msg = (char *)malloc(length + 1);
  char msg[6];
  //msg = (char*)payload;
  
  Serial.print("Message arrived with topic: [");
  Serial.print(topic);
  Serial.print("]; Message: '");

//  strncpy(msg, (char*)payload, length);
//
//  msg[length+1] = '/0';

  for (int i=0; i<6; i++)
  {
    msg[i] = (char)payload[i];
  }
  Serial.print(msg);
  Serial.println("'");
  Serial.print("length is: ");
  Serial.println(length);

  if(strcmp(msg,"Start1") == 0)
  {
    digitalWrite(Irrigator1, LOW);
    digitalWrite(Pump, LOW);
    Serial.println("Started 01");
    mqttClient.publish("IRRIGATION/report", "LED1 is ON");
  }
  else if (strcmp(msg,"Start2") == 0)
  {
    digitalWrite(Irrigator2, LOW);
    digitalWrite(Pump, LOW);
    Serial.println("Started 02");
    mqttClient.publish("IRRIGATION/report", "LED2 is ON");
  }
  else if (strcmp(msg,"Stop01") == 0)
  {
    digitalWrite(Irrigator1, HIGH);
    digitalWrite(Pump, HIGH);
    Serial.println("Stopped 01");
    mqttClient.publish("IRRIGATION/report", "LED1 is OFF");
  }
  else if (strcmp(msg,"Stop02") == 0)
  {
    digitalWrite(Irrigator2, HIGH);
    digitalWrite(Pump, HIGH);
    Serial.println("Stopped 02");
    mqttClient.publish("IRRIGATION/report", "LED2 is OFF");
  }
  //digitalWrite(gpio2_pin, HIGH);
}

//_____________________________________________________________
//WiFi and MQTT Init

void connectToWiFi()
{
   //Connect to the WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  int cnt = 0;

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
    cnt++;
    if(cnt == 20){
      Serial.println("");
      cnt = 0;
    }
  }
 
  Serial.println(" ");
  Serial.println(" Connected ");

  // Start the server
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void startWebServer()
{
  webPage += "<h1>ESP8266 Web Server</h1>";
  webPage += "<fieldset> <legend>Connect to broker:</legend>";
  webPage += "<form>IP Address: <br> <input type=\"text\" name=\"mqttIPAddress\"><br>";
  webPage += "<form>Port number: <br> <input type=\"text\" name=\"mqttPort\" value=\"1883\"><br><br>";
  webPage += "<input type=\"submit\" value=\"Connect\"></form>";
  webPage += "</fieldset><br>";
  webPage += "<fieldset> <legend>Controller:</legend>";
  webPage += "<p>Irrigator #1 <a href=\"socket1On\"><button>ON</button></a>&nbsp;<a href=\"socket1Off\"><button>OFF</button></a></p>";
  webPage += "<p>Irrigator #2 <a href=\"socket2On\"><button>ON</button></a>&nbsp;<a href=\"socket2Off\"><button>OFF</button></a></p>";
  webPage += "</fieldset>";

  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

    server.on("/", [](){
      server.send(200, "text/html", webPage);
    });
    server.on("/socket1On", [](){
      server.send(200, "text/html", webPage);
      digitalWrite(Irrigator1, LOW);
      delay(1000);
    });
    server.on("/socket1Off", [](){
      server.send(200, "text/html", webPage);
      digitalWrite(Irrigator1, HIGH);
      delay(1000);
    });
    server.on("/socket2On", [](){
      server.send(200, "text/html", webPage);
      digitalWrite(Irrigator2, LOW);
      delay(1000); 
    });
    server.on("/socket2Off", [](){
      server.send(200, "text/html", webPage);
      digitalWrite(Irrigator2, HIGH);
      delay(1000);
    });

  server.begin();
  Serial.println("HTTP server started");  
}

void getMosquittoIP()
{  
  Serial.println("Waiting for connection to MQTT broker...");
  
  while(!(server.hasArg("mqttIPAddress") && server.hasArg("mqttPort") ))
  {
      delay(500);
      server.handleClient();
  }
  
  String tmp = server.arg("mqttIPAddress");
 
  serverIP = new char[tmp.length()+1];
  tmp.toCharArray(serverIP, tmp.length()+1);

  tmp = server.arg("mqttPort");
  int port = tmp.toInt();

  Serial.println("Submitted: ");
  Serial.print("IP: ");
  Serial.println(serverIP);
  Serial.print("Port: ");
  Serial.println(port);
  Serial.println("");

  mqttClient.setServer(serverIP, port);
  mqttClient.setCallback(callback);  
}

void mqttReconnect()
{
  // Do this untill it is connected
  while(!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection to ");
    Serial.println(serverIP);
    //Attempt to connect
    if(mqttClient.connect(clientID))
    {
      Serial.println("MQTT connection established.");
      mqttClient.publish("IRRIGATION/report", "connected");
      mqttClient.subscribe(topic);
    }
    else
    {
      Serial.println("Failed to connect, will try to connect in 5 seconds.");
      delay(5000);
    }
  }
}

void setup() {

  // preparing GPIOs
  pinMode(Irrigator1, OUTPUT);
  digitalWrite(Irrigator1, HIGH);
  pinMode(Irrigator2, OUTPUT);
  digitalWrite(Irrigator2, HIGH);

  Serial.begin(9600);
  delay(10);

  connectToWiFi();
  startWebServer();
  getMosquittoIP();

  mqttClient.setCallback(callback);  
}

void loop() {

  if (!mqttClient.connected()) {
    mqttReconnect();
  }
  
  mqttClient.loop();
  server.handleClient();
}








