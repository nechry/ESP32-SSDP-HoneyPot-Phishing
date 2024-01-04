# ESP32-SSDP-HoneyPot-Phishing

This code establishes a web server on the ESP32, which reacts to multicast discover requests of the `SSDP` protocol, presenting itself as a typical `UPNP` device. The simulated device will seamlessly appear within Windows Explorer on devices connected to your local network.

Users who are tempted to open the device are shown a configurable phishing page.

## Requirements

An [ESP32 lilyGO T-Display S3 board](https://s.click.aliexpress.com/e/_DFoYDZR) (affiliate link)

`Visual Studio Code` with the `PlatformIO` extension

## Installation

Clone this repository and open it in Visual Studio Code.

You have to rename the file `credentials.h.example` to `credentials.h` from `include` folder and fill in your own SSID and password.

Upload the data folder to the ESP32 using the `Upload Filesystem image` tool in the `PlatformIO` menu.

## Usage

When you power on the ESP32 it will start a web server and it will be discoverable by SSDP.

When someone browses/open the device, they get a web page that has a log in. When they log in the username/password are displayed on the ESP32 screen.

The default page mimics a fictitious corporate scanner. During ESP32 startup, if you hit the button 2, you can easily change the spoofed device to a office backup controller.

## Credits

Based on the project [https://github.com/bdash9/ESP32-SSDP-HoneyPot-Phishing-webserver](https://github.com/bdash9/ESP32-SSDP-HoneyPot-Phishing-webserver)
and devices templates from [https://github.com/initstring/evil-ssdp](https://github.com/initstring/evil-ssdp)

## License

MIT License

## Disclaimer

This tool has been specifically crafted as a security research instrument for illustrative purposes, intended to showcase and study potential vulnerabilities. It is crucial to exercise utmost caution and adhere to ethical standards by obtaining explicit permission from the network owner before deploying this tool. Responsible and authorized usage is imperative to foster a secure and ethical environment.
