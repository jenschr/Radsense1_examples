# Radsense1 example files

Various example files for use with [RadSense1](https://maketronics.no/radsense-1/) based on the LD2410 millimeter radar sensor. These examples are made for use with either the PlatformIO IDE or Arduino. Pick up a device from [here](https://maketronics.no/radsense-1/) or build your own.

## Radar and LEDs only

This example shows basic use of the Radar to detect if anyone is present (up to 5m/15ft range) or within a specific distance (3m/9ft range) by changing the color of the builtin programmable RGB LEDs.

## Radar and relays

This example shows how to use the builtin relays with the radar sensor.

## Pushmessage

This example extends the first example and adds the option to send a Push message whenever anyone triggers the radar. Use this example when you want to be notified if anyone enters your server room, Halloween candy/cookie jar or anything else you want a notification for when someone is present.

We are using the PushSafer service to send push notifications in realtime to any registered iPhone, iPad, Android, Windows mobile or Windows desktop device, Telegram and even to your browser (Chrome, Firefox, Edge, Opera & Yandex).

## Animated LEDs

This simple project will just animate the programmable LEDs in a circle motion. 