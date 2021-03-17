## TODO:
### Tests:
- Start writing test doc
### Settings
- advanced settings fold
- show if there is any available updates when changing update url
### Debugging page
- remaining memory
    - total and for each task
- Error logs
- clear logs
### OTA Updates
- Rollback to previous version
- progress bar
- error catching
- self-test on startup
Analytics
- Total queries and total blocked queries
- most blocked/queried websites
### DATETIME
- Add timezone select
### CONFIGURATION
- Look into wifi_provisioning library
### DNS
- Support for non A/AAAA types
- better client handling
- - print error if client never got response
- Reverse DNS support
- Benchmark DNS response times
### LEDs
- LED Error Codes
### WEBSITE
- show currently connected network & ip
- show connected clients
- Add log page select to homepage 
- Upload/download blacklist files
- Cancel buttons
### BLACKLIST
- Handling reload_blacklist() errors
- Benchmark speed improvement from keeping blacklist in RAM instead of flash
### FLASH
- Handle flash errors
- Document what is being held in flash
### OTHER
- Document/Refactor Code
- Prevent buffer overflows
- Move away from strcpy, strcmp, etc

## Maybe Some Day:
- DHCP support
    - Individual block lists for devices
    - User accounts??
- DNS Over HTTPS (early tests dont look hopeful for ESP32, will likely need MPU)

## Known Bugs:
- Have to reset device if network ssid/pass change
- Text on website overflows on skinny screens
- Auto joining bugs during configuration, regularly disconnects (on iphone)
- black/whitelist buttons on homepage do not update after being added 
