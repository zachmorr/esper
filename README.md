![Imgur](https://i.imgur.com/srEjdoel.jpg)

# About Esper

Heavily inspired by the Pi-hole, this project aims to bring network wide ad-blocking to another popular device popular among makers and hobbiests, the ESP32.

## What is DNS?

Commonly refered to as the "phonebook for the internet", the DNS protocol is how devices map human readable urls like google.com or facebook.com into IP addresses that the device can use.

## How does Esper block ads?

Just like the [Pi-Hole](https://github.com/pi-hole/pi-hole), Esper acts as a DNS server for devices. When sent a DNS query, Esper will read the url the device is requesting, then decide to block the request by returning 0.0.0.0 or forward the request on to a real DNS server.

## Differences between Pi-hole and Esper

The biggest, and most obvious difference is the hardware these projects run on. Pi-hole aims to be available on any Linux hardware that you own, while Esper is an ESP32 specific project. Due to being run on a microcontroller, Esper is much more limited in it's capabilities and is therefore much more minimal in it's design.

## Why choose Esper instead of Pi-hole?

Choose whatever one suits you! If like tinkering with hardware, and do not have much experience with the Raspberry Pi/Linux ecosystem, setting up the Esper software on an ESP32 dev board might be an interesting project. And while Esper may not be nearly as powerful as a Raspberry Pi, DNS protocol is very lightweight in nature. Unless you are in a household with 10+ people that are always online you will likely never going to need more computing power than a microcontroller can provice.

At the same time, if you have no problem using a shell and Linux environment, the Pi-hole is a much more feature rich and powerful piece of software that is a joy to use.

## How can I get an Esper?

The easiest way, is to purchase a pre-made one from the Tindie store. (Link incoming...) 

If you have some ESP32 devopment boards collecting dust, it will work on those too! It will take some slight configuration [see instructions here](https://github.com/zachmorr/esper/blob/main/software/README.md), but you can have it running in no time.

DISCLAIMER: It is **heavily** recommended to use a device with ethernet. Even the best wifi will significantly increase latency and decrease reliability compared to a good copper connection.

If you want to try you hand an making an Esper from scratch, the hardware directory contains all the schematic, pcb, and enclosure files needed to build one yourself.

# Esper Development

## Tools Used

* KiCAD
* FreeCAD
* Hugo
* ESP-IDF
* Python 3.7

## Directory Structure

    ├── hardware                    # Hardware root directory
    │   ├── custom components           # Custom KiCad component files
    │   ├── enclosure                   # FreeCAD project and STL files for enclosure
    │   ├── product                     # KiCad project for Esper
    │   ├── programmer                  # KiCad project for programmer/debugger
    └── software                    # Software root directory
        ├── default blacklist           # List of URLs that will be blocked by default
        ├── firmware                    # ESP-IDF project that contains all code for Esper
        ├── scripts                     # Scripts used in build process and testing
        ├── tests                       # Scripts used in testing (not started yet)
        └── website                     # Hugo project used to generate website

