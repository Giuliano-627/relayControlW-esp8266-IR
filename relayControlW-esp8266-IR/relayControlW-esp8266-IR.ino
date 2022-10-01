#include <ESP8266WiFi.h>

const char* ssid = "DADAH";
const char* password = "17281722";

#include <IRremote.h>
#define receptor 14
#define salida 4
bool estado = LOW;
IRrecv irrecv(receptor);
decode_results codigo;
WiFiServer server(80);
IPAddress ip(192, 168, 1, 197);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
void setup()
{
  Serial.begin(115200);
  delay(10);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  digitalWrite(12, HIGH);
  digitalWrite(13, LOW);
  irrecv.enableIRIn();
  pinMode(salida, OUTPUT);
  digitalWrite(salida, LOW);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1);
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

}

void loop()
{
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
    }
    if (irrecv.decode(&codigo)) {
      Serial.println(codigo.value, HEX);
      if (codigo.value == 0xFFC23D) {
        Serial.println("Tecla correcta");
        estado = !estado;
        digitalWrite(salida, estado);
      }
      irrecv.resume();
    }
  }
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client)
  {
    if (irrecv.decode(&codigo)) {
      Serial.println(codigo.value, HEX);
      if (codigo.value == 0xFFC23D) {
        Serial.println("Tecla correcta");
        estado = !estado;
        digitalWrite(salida, estado);
      }
      irrecv.resume();
    }
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available())
  {
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Match the request

  int value = LOW;
  if (request.indexOf("/LED=ON") != -1)
  {
    digitalWrite(salida, HIGH);
    value = HIGH;
  }
  if (request.indexOf("/LED=OFF") != -1)
  {
    digitalWrite(salida, LOW);
    value = LOW;
  }

  // Set salida according to the request
  //digitalWrite(salida, value);

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

  client.print("Led pin is now: ");

  if (value == HIGH)
  {
    client.print("On");
  }
  else
  {
    client.print("Off");
  }
  client.println("<br><br>");
  client.println("<a href=\"/LED=ON\"\"><button>Turn On </button></a>");
  client.println("<a href=\"/LED=OFF\"\"><button>Turn Off </button></a><br />");
  client.println("</html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");

}
