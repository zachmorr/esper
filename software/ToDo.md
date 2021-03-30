## TODO:

### Version 0.1
- Document Code
- Clean up build scripts?
- Run through tests
- Set up Tindie
    - Packaging?
- How to use guide
- Hardware w/ on board programmer
- Basic site at openesper.com
- Push to official repo

### Tests:
- Start writing test doc
### Build
- Add Arduino support
### Settings
- advanced settings fold
### Debugging page
- remaining memory
    - total and for each task
- Error logs
- clear logs
### OTA Updates
- progress bar
### DATETIME
- Add timezone select
- Connect to SNTP in background, non-blocking
### CONFIGURATION
- Look into wifi_provisioning library
### DNS
- Support for non A/AAAA types
- better client handling
    - print error if client never got response
- Reverse DNS support
- Benchmark DNS response times
### LEDs
- LED Error Codes
### WEBSITE
- Password protection!
- Analytics
    - Total queries and total blocked queries
    - most blocked/queried websites
- show currently connected network & ip
- show connected clients
- Add log page select to homepage 
- Upload/download blacklist files
### BLACKLIST
- more error handling errors
- Benchmark speed improvement from keeping blacklist in RAM instead of flash
### FLASH
- figure out way to emulate flash wear out & error handling
- Handle flash errors
- Document what is being held in flash

## Maybe Some Day:
- DHCP support
    - Individual block lists for devices
    - User accounts??
- DNS Over HTTPS (early tests dont look hopeful for ESP32, will likely need MPU)

## Known Bugs:
- Have to reset device if network ssid/pass change
- black/whitelist buttons on homepage do not update after being added 
