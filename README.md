# RingDong

A simple application to send a MQTT message when a pin input state changes, e.g. when someone pressed the doorbell you connected to your raspberry pi.

## Getting Started

Simply fill in your MQTT server details into the configuration file, change the topic and message if needed and adapt the debounce time if needed.

### Prerequisites

You will need the libmosquitto, libconfuse and wiringPI libraries.
Oh and you have to run it as root because of the wiringPI pin setup.
Privilege dropping doesn't work yet.

### Notes

This application is in a early stage and 

## License

This project is licensed under the Zero clause BSD License - see the [LICENSE.md](LICENSE.md) file for details