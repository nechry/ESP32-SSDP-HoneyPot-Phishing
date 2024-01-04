# ESP32-SSDP-HoneyPot-Phishing

This code runs a web server on the ESP32 that responds to `SSDP` multicast discover requests, posing as a generic UPNP device. The spoofed device will magically appear in Windows Explorer on machines in your local network.

Users who are tempted to open the device are shown a configurable phishing page.

## Requirements

An [ESP32 lilyGO T-Display S3 board](https://s.click.aliexpress.com/e/_DFoYDZR) (affiliate link)

`Visual Studio Code` with the `PlatformIO` extension

## Installation

Clone this repository and open it in Visual Studio Code.

You have to rename the file `credentials.h.example` to `credentials.h` from `include` folder and fill in your own SSID and password.

Recommended: Before you upload the code to the ESP32, upload the data folder to the ESP32 using the `Upload Filesystem image` tool in the `PlatformIO` menu.

## Usage

When you power on the ESP32 it will start a web server and it will be discoverable by SSDP.

When someone browses/open the device, they get a web page that has a log in. When they log in the username/password are displayed on the ESP32 screen.

The default page is a fake corporate scanner, you can easily change the page to look like a backup controller, by starting the ESP32 with the button 2 pressed.

## Credits

Based on the project https://github.com/bdash9/ESP32-SSDP-HoneyPot-Phishing-webserver
and devices templates from https://github.com/initstring/evil-ssdp

## License

MIT License

## Disclaimer

This is a security research tool for demonstrations purpose. Use only where granted explicit permission from the network owner.
