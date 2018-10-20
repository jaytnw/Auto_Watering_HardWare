F8hf#include <ESP8266WiFi.h>
const char* ssid     = "JayThanawut";
//const char* ssid     = "Mordang_FL4#1";
const char* password = "123456788";
String ResponseControl(void);

WiFiServer server(80);

int pinList[8] = {2, 4, 5, 12, 13, 14, 15, 16};
String responseHeader = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nAccess-Control-Allow-Origin: *\r\nConnection: close\r\n\r\n";

void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  delay(10);

  // prepare
  for (int i = 0; i < 8; i++) {
    pinMode(pinList[i], OUTPUT);
    digitalWrite(pinList[i], LOW);
  }

  //  WiFi.begin("Pixel","123456788");




  Serial.println();
  Serial.println();
  Serial.print("Connecting to "); //แสดงข้อความ  “Connecting to”
  Serial.println(ssid);       //แสดงข้อความ ชื่อ SSID
  WiFi.begin(ssid, password); // เชื่อมต่อไปยัง AP

  while (WiFi.status() != WL_CONNECTED)   //รอจนกว่าจะเชื่อมต่อสำเร็จ
  {
    delay(500);
    Serial.print(".");
  }

  IPAddress local_ip = {172, 20, 10, 9}; //ตั้งค่า IP
  IPAddress gateway = {172, 20, 10, 1}; //ตั้งค่า IP Gateway
  IPAddress subnet = {255, 255, 255, 0}; //ตั้งค่า Subnet
  WiFi.config(local_ip, gateway, subnet); //setค่าไปยังโมดูล
    // Start the server
  server.begin();
  Serial.println("\nServer started");

  Serial.println("");
  Serial.println("WiFi connected");   //แสดงข้อความเชื่อมต่อสำเร็จ
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); //แสดงหมายเลข IP ของ ESP8266
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (client.connected() && !client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  if (req.indexOf("/api") >= 0) { // Is API request
    for (int i = 0; i < 8; i++) {
      if (req.indexOf("/api/gpio" + String(pinList[i])) >= 0) {
        if (req.indexOf("/api/gpio" + String(pinList[i]) + "/1") >= 0) {
          digitalWrite(pinList[i], HIGH);
          client.flush();
          client.print(responseHeader + "OK");
        } else if (req.indexOf("/api/gpio" + String(pinList[i]) + "/0") >= 0) {
          digitalWrite(pinList[i], LOW);
          client.flush();
          client.print(responseHeader + "OK");
        } else if (req.indexOf("/api/gpio" + String(pinList[i]))) {
          client.flush();
          client.print(responseHeader + "Status:" + (digitalRead(pinList[i]) ? "1" : "0")); // Send the response to the client
        }
        break;
      }
    }
  } else {
    // Match the request
    for (int i = 0; i < 8; i++) {
      if (req.indexOf("/gpio" + String(pinList[i]) + "/1") >= 0)
        digitalWrite(pinList[i], HIGH);
      else if (req.indexOf("/gpio" + String(pinList[i]) + "/0") >= 0)
        digitalWrite(pinList[i], LOW);
    }

    client.flush();

    // Prepare the response
    String s = ResponseControl();

    // Send the response to the client
    client.print(s);
  }

  Serial.println("Client disonnected");
  delay(1);
  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}

String ResponseControl() {
  String ControlHTML = responseHeader;
  ControlHTML += "<title>Drop</title>\r\n";
  ControlHTML += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n";
  ControlHTML += "<h1 style=\"margin-bottom:0\">Drop</h1>\r\n";
  ControlHTML += "<h3 style=\"color:#999;margin-top:5px;\"></h3>\r\n";
  ControlHTML += "<table width=\"180\" border=\"0\" cellpadding=\"10\" cellspacing=\"0\">\r\n";
  for (int i = 0; i < 8; i++) {
    ControlHTML += "<tr>\r\n";
    ControlHTML += "<td width=\"100\">GPIO" + String(pinList[i]) + "</td>\r\n";
    ControlHTML += "<td width=\"60\" align=\"center\"><button onClick=\"location='/gpio" + String(pinList[i]) + "/" + (digitalRead(pinList[i]) ? "0" : "1") + "'\">" + (digitalRead(pinList[i]) ? "OFF" : "ON") + "</button></td>\r\n";
    ControlHTML += "</tr>\r\n";
  }
  ControlHTML += "</table>\r\n";
  return ControlHTML;
}
