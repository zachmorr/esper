EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 3
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text GLabel 4950 4150 2    50   Input ~ 0
MDC
Text GLabel 4950 3750 2    50   Input ~ 0
MDIO
Text GLabel 4950 4450 2    50   Input ~ 0
CRS_DV
Text GLabel 4950 4350 2    50   Input ~ 0
RXD1
Text GLabel 4950 4250 2    50   Input ~ 0
RXD0
Text GLabel 5950 3950 2    50   Input ~ 0
TXEN
Text GLabel 5950 4150 2    50   Input ~ 0
TXD1
Text GLabel 5950 3750 2    50   Input ~ 0
TXD0
Text GLabel 4950 2650 2    50   Input ~ 0
RX
Text GLabel 4950 2850 2    50   Input ~ 0
TX
Text GLabel 4950 2550 2    50   Input ~ 0
IO0
Text GLabel 4950 3650 2    50   Input ~ 0
CLK50MHZ
$Comp
L Device:R R22
U 1 1 5DF2BBCB
P 5700 3950
F 0 "R22" V 5800 4050 50  0000 C CNN
F 1 "49.9" V 5800 3850 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 5630 3950 50  0001 C CNN
F 3 "~" H 5700 3950 50  0001 C CNN
F 4 "" H 5700 3950 50  0001 C CNN "LCSC Part Number"
	1    5700 3950
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R23
U 1 1 5DF2BD63
P 5700 4150
F 0 "R23" V 5800 4250 50  0000 C CNN
F 1 "49.9" V 5800 4050 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 5630 4150 50  0001 C CNN
F 3 "~" H 5700 4150 50  0001 C CNN
F 4 "" H 5700 4150 50  0001 C CNN "LCSC Part Number"
	1    5700 4150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4950 2850 4600 2850
Wire Wire Line
	4950 3750 4600 3750
Wire Wire Line
	4600 4150 4950 4150
Wire Wire Line
	4950 4250 4600 4250
Wire Wire Line
	4600 4350 4950 4350
Wire Wire Line
	4950 4450 4600 4450
Wire Wire Line
	4600 3850 5300 3850
Wire Wire Line
	5300 3850 5300 3750
Wire Wire Line
	4600 4050 5300 4050
Wire Wire Line
	5300 4050 5300 4150
Wire Wire Line
	5850 4150 5950 4150
Wire Wire Line
	5850 3950 5950 3950
Wire Wire Line
	5950 3750 5850 3750
NoConn ~ 3400 3750
NoConn ~ 3400 3850
NoConn ~ 3400 3950
NoConn ~ 3400 4050
NoConn ~ 3400 4150
NoConn ~ 3400 4250
$Comp
L power:GND #PWR037
U 1 1 5DF342C0
P 4000 5300
F 0 "#PWR037" H 4000 5050 50  0001 C CNN
F 1 "GND" H 4005 5127 50  0000 C CNN
F 2 "" H 4000 5300 50  0001 C CNN
F 3 "" H 4000 5300 50  0001 C CNN
	1    4000 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 5300 4000 5150
Wire Wire Line
	3400 2550 3250 2550
$Comp
L power:+3.3V #PWR034
U 1 1 5DF362C5
P 4000 2050
F 0 "#PWR034" H 4000 1900 50  0001 C CNN
F 1 "+3.3V" H 4015 2223 50  0000 C CNN
F 2 "" H 4000 2050 50  0001 C CNN
F 3 "" H 4000 2050 50  0001 C CNN
	1    4000 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3250 2550 3250 2450
Wire Wire Line
	3250 2150 3250 2050
$Comp
L power:+3.3V #PWR033
U 1 1 5DF35B8F
P 3250 2050
F 0 "#PWR033" H 3250 1900 50  0001 C CNN
F 1 "+3.3V" H 3265 2223 50  0000 C CNN
F 2 "" H 3250 2050 50  0001 C CNN
F 3 "" H 3250 2050 50  0001 C CNN
	1    3250 2050
	1    0    0    -1  
$EndComp
$Comp
L Device:R R17
U 1 1 5DF33A2D
P 3250 2300
F 0 "R17" H 3400 2250 50  0000 C CNN
F 1 "10K" H 3400 2350 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 3180 2300 50  0001 C CNN
F 3 "~" H 3250 2300 50  0001 C CNN
F 4 "" H 3250 2300 50  0001 C CNN "LCSC Part Number"
	1    3250 2300
	-1   0    0    1   
$EndComp
Text GLabel 3050 2550 0    50   Input ~ 0
EN
Wire Wire Line
	3250 2550 3050 2550
Connection ~ 3250 2550
$Comp
L Device:R R21
U 1 1 5DF2A186
P 5700 3750
F 0 "R21" V 5800 3850 50  0000 C CNN
F 1 "49.9" V 5800 3650 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 5630 3750 50  0001 C CNN
F 3 "~" H 5700 3750 50  0001 C CNN
F 4 "" H 5700 3750 50  0001 C CNN "LCSC Part Number"
	1    5700 3750
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4600 2650 4950 2650
NoConn ~ 4600 2750
NoConn ~ 4600 3050
Wire Wire Line
	4600 2550 4950 2550
Wire Wire Line
	4600 3650 4950 3650
Wire Wire Line
	4600 3950 5550 3950
Wire Wire Line
	5300 3750 5550 3750
Wire Wire Line
	5300 4150 5550 4150
Text Label 4650 3850 0    50   ~ 0
TXD0_R
Text Label 4650 3950 0    50   ~ 0
TXEN_R
Text Label 4650 4050 0    50   ~ 0
TXD1_R
Text Label 4650 4250 0    50   ~ 0
RXD0
Text Label 4650 4350 0    50   ~ 0
RXD1
$Comp
L Mechanical:MountingHole_Pad H1
U 1 1 5F4EAB37
P 3450 6900
F 0 "H1" H 3550 6946 50  0000 L CNN
F 1 "MountingHole" H 3550 6855 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2_Pad" H 3450 6900 50  0001 C CNN
F 3 "~" H 3450 6900 50  0001 C CNN
	1    3450 6900
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H2
U 1 1 5F4EB661
P 4300 6900
F 0 "H2" H 4400 6946 50  0000 L CNN
F 1 "MountingHole" H 4400 6855 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2_Pad" H 4300 6900 50  0001 C CNN
F 3 "~" H 4300 6900 50  0001 C CNN
	1    4300 6900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR035
U 1 1 5F4A3DFF
P 9850 3650
F 0 "#PWR035" H 9850 3400 50  0001 C CNN
F 1 "GND" H 9855 3477 50  0000 C CNN
F 2 "" H 9850 3650 50  0001 C CNN
F 3 "" H 9850 3650 50  0001 C CNN
	1    9850 3650
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_GSD Q3
U 1 1 5F4A7E31
P 8950 3350
F 0 "Q3" H 9156 3396 50  0000 L CNN
F 1 "Q_NMOS_GSD" H 9156 3305 50  0001 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 9150 3450 50  0001 C CNN
F 3 "~" H 8950 3350 50  0001 C CNN
	1    8950 3350
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_GSD Q2
U 1 1 5F4A98E8
P 9350 3100
F 0 "Q2" H 9556 3100 50  0000 L CNN
F 1 "Q_NMOS_GSD" H 9556 3055 50  0001 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 9550 3200 50  0001 C CNN
F 3 "~" H 9350 3100 50  0001 C CNN
	1    9350 3100
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_GSD Q1
U 1 1 5F4AC46E
P 9750 2850
F 0 "Q1" H 9956 2850 50  0000 L CNN
F 1 "Q_NMOS_GSD" H 9956 2805 50  0001 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 9950 2950 50  0001 C CNN
F 3 "~" H 9750 2850 50  0001 C CNN
	1    9750 2850
	1    0    0    -1  
$EndComp
$Comp
L Device:R R19
U 1 1 5F4B4C35
P 9450 2450
F 0 "R19" H 9520 2496 50  0000 L CNN
F 1 "47" H 9520 2405 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9380 2450 50  0001 C CNN
F 3 "~" H 9450 2450 50  0001 C CNN
F 4 "1/10W" H 9650 2320 50  0000 C CNN "Rating"
	1    9450 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	8750 3350 8500 3350
Wire Wire Line
	9150 3100 8500 3100
Wire Wire Line
	9550 2850 8500 2850
Text GLabel 8500 3350 0    50   Input ~ 0
Blue
Text GLabel 8500 3100 0    50   Input ~ 0
Green
Text GLabel 8500 2850 0    50   Input ~ 0
Red
$Comp
L power:VBUS #PWR?
U 1 1 5F4D638B
P 8550 4550
AR Path="/5F4D638B" Ref="#PWR?"  Part="1" 
AR Path="/5DED2F9B/5F4D638B" Ref="#PWR?"  Part="1" 
AR Path="/5DED3446/5F4D638B" Ref="#PWR036"  Part="1" 
F 0 "#PWR036" H 8550 4400 50  0001 C CNN
F 1 "VBUS" H 8565 4723 50  0000 C CNN
F 2 "" H 8550 4550 50  0001 C CNN
F 3 "" H 8550 4550 50  0001 C CNN
	1    8550 4550
	1    0    0    -1  
$EndComp
$Comp
L Device:R R24
U 1 1 5F4D639E
P 8550 4800
F 0 "R24" H 8620 4846 50  0000 L CNN
F 1 "10K" H 8620 4755 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 8480 4800 50  0001 C CNN
F 3 "~" H 8550 4800 50  0001 C CNN
	1    8550 4800
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR038
U 1 1 5F4DC54A
P 8550 5600
F 0 "#PWR038" H 8550 5350 50  0001 C CNN
F 1 "GND" H 8555 5427 50  0000 C CNN
F 2 "" H 8550 5600 50  0001 C CNN
F 3 "" H 8550 5600 50  0001 C CNN
	1    8550 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 5500 8550 5600
Wire Wire Line
	8550 4650 8550 4550
Wire Wire Line
	8550 5000 8250 5000
Connection ~ 8550 5000
Wire Wire Line
	8550 5000 8550 4950
Text GLabel 8250 5000 0    50   Input ~ 0
Block
Wire Wire Line
	9050 3550 9050 3600
Wire Wire Line
	9050 3600 9450 3600
Wire Wire Line
	9850 3600 9850 3650
Wire Wire Line
	9850 3050 9850 3600
Connection ~ 9850 3600
Wire Wire Line
	9450 3300 9450 3600
Connection ~ 9450 3600
Wire Wire Line
	9450 3600 9850 3600
Wire Wire Line
	8550 5000 8550 5100
Text GLabel 4950 4750 2    50   Input ~ 0
Block
Wire Wire Line
	4600 4750 4950 4750
Wire Wire Line
	4000 2050 4000 2350
Text Notes 4150 2300 0    50   ~ 0
No decoupling needed\nWROOM module already has 10, 1, 0.1 caps inside\n 
$Comp
L LED:ASMT-YTB7-0AA02 D2
U 1 1 5FCC646A
P 8650 2050
F 0 "D2" H 8650 2417 50  0000 C CNN
F 1 "RS-3535MWAP" H 8650 2326 50  0000 C CNN
F 2 "LED_SMD:LED_RGB_PLCC-6" H 8450 1730 50  0001 L CNN
F 3 "https://datasheet.lcsc.com/szlcsc/1811181548_Foshan-NationStar-Optoelectronics-FM-Z3535RGBA-SH_C110903.pdf" H 8800 2050 50  0001 L CNN
	1    8650 2050
	1    0    0    -1  
$EndComp
$Comp
L power:VBUS #PWR?
U 1 1 5FEEF4AA
P 8150 1750
AR Path="/5FEEF4AA" Ref="#PWR?"  Part="1" 
AR Path="/5DED2F9B/5FEEF4AA" Ref="#PWR?"  Part="1" 
AR Path="/5DED3446/5FEEF4AA" Ref="#PWR032"  Part="1" 
F 0 "#PWR032" H 8150 1600 50  0001 C CNN
F 1 "VBUS" H 8165 1923 50  0000 C CNN
F 2 "" H 8150 1750 50  0001 C CNN
F 3 "" H 8150 1750 50  0001 C CNN
	1    8150 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	8150 1750 8150 1950
Wire Wire Line
	8150 2150 8350 2150
Wire Wire Line
	8350 2050 8150 2050
Connection ~ 8150 2050
Wire Wire Line
	8150 2050 8150 2150
Wire Wire Line
	8350 1950 8150 1950
Connection ~ 8150 1950
Wire Wire Line
	8150 1950 8150 2050
Wire Wire Line
	9850 2600 9850 2650
Wire Wire Line
	9450 2600 9450 2900
Wire Wire Line
	9050 2600 9050 3150
$Comp
L Device:R R18
U 1 1 5F4B5722
P 9050 2450
F 0 "R18" H 9120 2496 50  0000 L CNN
F 1 "47" H 9120 2405 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 8980 2450 50  0001 C CNN
F 3 "~" H 9050 2450 50  0001 C CNN
F 4 "1/10W" H 9250 2320 50  0000 C CNN "Rating"
	1    9050 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	8950 2150 9050 2150
Wire Wire Line
	9050 2150 9050 2300
Wire Wire Line
	8950 2050 9450 2050
Wire Wire Line
	9450 2050 9450 2300
Wire Wire Line
	8950 1950 9850 1950
Wire Wire Line
	9850 1950 9850 2300
$Comp
L Switch:SW_SPST SW1
U 1 1 5FF060B9
P 8550 5300
F 0 "SW1" V 8596 5212 50  0000 R CNN
F 1 "PTS645VH83-2 LFS" V 8505 5212 50  0000 R CNN
F 2 "Button_Switch_THT:SW_Tactile_SPST_Angled_PTS645Vx83-2LFS" H 8550 5300 50  0001 C CNN
F 3 "https://www.ckswitches.com/media/1471/pts645.pdf" H 8550 5300 50  0001 C CNN
	1    8550 5300
	0    -1   -1   0   
$EndComp
Text Notes 8650 1500 0    50   ~ 0
RED: 2.2V @ 40mA\nGREEN: 3.5V @ 40mA\nBLUE: 3.5V @ 40mA\n
$Comp
L Device:R R20
U 1 1 5F4B4082
P 9850 2450
F 0 "R20" H 9920 2496 50  0000 L CNN
F 1 "82" H 9920 2405 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric" V 9780 2450 50  0001 C CNN
F 3 "~" H 9850 2450 50  0001 C CNN
F 4 "1/4W" H 10030 2320 50  0000 C CNN "Rating"
F 5 "C247183" H 9850 2450 50  0001 C CNN "LCSC#"
	1    9850 2450
	1    0    0    -1  
$EndComp
$Comp
L power:Earth #PWR0102
U 1 1 5FF73B5F
P 3450 7050
F 0 "#PWR0102" H 3450 6800 50  0001 C CNN
F 1 "Earth" H 3450 6900 50  0001 C CNN
F 2 "" H 3450 7050 50  0001 C CNN
F 3 "~" H 3450 7050 50  0001 C CNN
	1    3450 7050
	1    0    0    -1  
$EndComp
$Comp
L power:Earth #PWR0103
U 1 1 5FF749E4
P 4300 7050
F 0 "#PWR0103" H 4300 6800 50  0001 C CNN
F 1 "Earth" H 4300 6900 50  0001 C CNN
F 2 "" H 4300 7050 50  0001 C CNN
F 3 "~" H 4300 7050 50  0001 C CNN
	1    4300 7050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 7050 4300 7000
Wire Wire Line
	3450 7050 3450 7000
Wire Wire Line
	7000 5850 7100 5850
$Comp
L Connector_Generic:Conn_02x05_Odd_Even J?
U 1 1 6008BCDD
P 6700 5650
AR Path="/5DED3446/6008BCDD" Ref="J?"  Part="1" 
AR Path="/5DED2F9B/6008BCDD" Ref="J?"  Part="1" 
F 0 "J?" H 6750 5975 50  0000 C CNN
F 1 "Conn_02x05_Odd_Even" H 6750 5976 50  0001 C CNN
F 2 "Esper_Footprints:CNC_3220-10-0100-00" H 6700 5650 50  0001 C CNN
F 3 "~" H 6700 5650 50  0001 C CNN
	1    6700 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	7100 5450 7000 5450
Wire Wire Line
	7000 5550 7100 5550
Wire Wire Line
	7100 5650 7000 5650
Wire Wire Line
	7000 5750 7100 5750
Wire Wire Line
	7100 5850 7100 5950
$Comp
L power:GND #PWR?
U 1 1 6008BCE8
P 7100 5950
AR Path="/5DED3446/6008BCE8" Ref="#PWR?"  Part="1" 
AR Path="/5DED2F9B/6008BCE8" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 7100 5700 50  0001 C CNN
F 1 "GND" H 7105 5777 50  0000 C CNN
F 2 "" H 7100 5950 50  0001 C CNN
F 3 "" H 7100 5950 50  0001 C CNN
	1    7100 5950
	1    0    0    -1  
$EndComp
Text GLabel 7100 5750 2    50   Input ~ 0
IO0
Text GLabel 7100 5650 2    50   Input ~ 0
EN
Text GLabel 7100 5450 2    50   Input ~ 0
TX
Text GLabel 7100 5550 2    50   Input ~ 0
RX
Wire Wire Line
	6500 5450 6400 5450
Text GLabel 6400 5450 0    50   Input ~ 0
MTMS
Wire Wire Line
	6500 5550 6400 5550
Text GLabel 6400 5550 0    50   Input ~ 0
MTDI
Wire Wire Line
	6500 5650 6400 5650
Text GLabel 6400 5650 0    50   Input ~ 0
MTCK
Wire Wire Line
	6500 5750 6400 5750
NoConn ~ 6500 5850
$Comp
L EsperDNS-rescue:ESP32-WROOM-32D-RF_Module U?
U 1 1 5DF25F0A
P 4000 3750
AR Path="/5DF25F0A" Ref="U?"  Part="1" 
AR Path="/5DED3446/5DF25F0A" Ref="U3"  Part="1" 
F 0 "U3" H 3600 5100 50  0000 C CNN
F 1 "ESP32-WROOM-32D" V 4000 3850 50  0000 C CNN
F 2 "RF_Module:ESP32-WROOM-32" H 4000 2250 50  0001 C CNN
F 3 "https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf" H 3700 3800 50  0001 C CNN
F 4 "" H 4000 3750 50  0001 C CNN "LCSC Part Number"
	1    4000 3750
	1    0    0    -1  
$EndComp
Text GLabel 6400 5750 0    50   Input ~ 0
MTDO
Wire Wire Line
	4600 3150 4950 3150
Wire Wire Line
	4600 3250 4950 3250
Wire Wire Line
	4600 3350 4950 3350
Wire Wire Line
	4600 3450 4950 3450
Text GLabel 4950 3150 2    50   Input ~ 0
MTDI
Text GLabel 4950 3350 2    50   Input ~ 0
MTMS
Text GLabel 4950 3250 2    50   Input ~ 0
MTCK
Text GLabel 4950 3450 2    50   Input ~ 0
MTDO
Wire Wire Line
	4600 3550 4950 3550
Wire Wire Line
	4600 4550 4950 4550
Wire Wire Line
	4600 4650 4950 4650
Text GLabel 4950 3550 2    50   Input ~ 0
Blue
Text GLabel 4950 4550 2    50   Input ~ 0
Red
Text GLabel 4950 4650 2    50   Input ~ 0
Green
Text Notes 750  2900 0    50   ~ 0
Strapping Pins:\n• GPIO0: internal pull-up\n• GPIO2: internal pull-down\n• GPIO4: internal pull-down\n• GPIO5: internal pull-up\n• MTDI/GPIO12: internal pull-down\n• MTDO/GPIO15: internal pull-up
NoConn ~ 4600 2950
NoConn ~ 4600 4850
NoConn ~ 3400 2850
NoConn ~ 3400 2750
Text Notes 750  3300 0    50   ~ 0
Can't use for LEDs,\npulled up during reset:\n1, 3, 6, 7, 8, \n9, 10, 11, 14,
Text Notes 750  3550 0    50   ~ 0
Input Only:\n34, 35, SENSOR_VP, SENSOR_VN
$EndSCHEMATC
