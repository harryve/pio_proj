#include <ESP8266WebServer.h>   // Include the WebServer library

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

static float temperature, humidity, pressure;
static int32_t signalStrength;

void WebServerPublish(float temp, float hum, float pres, int32_t sigStrength)
{
    temperature = temp;
    humidity = hum;
    pressure = pres;
    signalStrength = sigStrength;

}

static void handleRoot()
{
    //String content = "HTTP/1.1 200 OK\n";
    //content += "Content-type:text/html\n";
    //content += "Connection: close\n";

    String content = "<!DOCTYPE html><html>";
    content += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
    content += "<link rel=\"icon\" href=\"data:,\">";

    //client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
    //client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
    //client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
    //client.println(".button2 {background-color: #77878A;}</style></head>");
    content += "<body><h1>Wemos 4 Web Server</h1>";
    content += "<p>Temperatuur: ";
    content += temperature;
    content += "</p>";

    content += "<p>Vochtigheid: ";
    content += humidity;
    content += "</p>";

    content += "<p>Luchtdruk: ";
    content += pressure;
    content += "</p>";

    content += "<p>Signaal sterkte: ";
    content += signalStrength;
    content += "</p>";

    content += "<p>Uptime: ";
    content += millis() / 1000;
    content += "</p>";

    content += "</body></html>";



    server.send(200, "text/html", content);
}

void handleNotFound()
{
    server.send(404, "text/plain", "404: Not found");
}

void WebServerInit()
{
    server.on("/", handleRoot);               // Call the 'handleRoot' function when a client requests URI "/"
    server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

    server.begin();                           // Actually start the server
    Serial.println("HTTP server started");
}

void WebServerLoop()
{
    server.handleClient();                    // Listen for HTTP requests from clients
}
