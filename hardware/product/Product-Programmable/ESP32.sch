EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 3
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L RF_Module:ESP32-WROOM-32D U?
U 1 1 5DF25F0A
P 3600 4550
AR Path="/5DF25F0A" Ref="U?"  Part="1" 
AR Path="/5DED3446/5DF25F0A" Ref="U3"  Part="1" 
AR Path="/5DED2F9B/5DF25F0A" Ref="U2"  Part="1" 
F 0 "U2" H 3200 5900 50  0000 C CNN
F 1 "ESP32-WROOM-32E" V 3600 4650 50  0000 C CNN
F 2 "RF_Module:ESP32-WROOM-32" H 3600 3050 50  0001 C CNN
F 3 "https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32e_esp32-wroom-32ue_datasheet_en.pdf" H 3300 4600 50  0001 C CNN
F 4 "C701341" H 3600 4550 50  0001 C CNN "LCSC Part Number"
F 5 "X" H 3600 4550 50  0001 C CNN "Extended Part"
F 6 "C701341" H 3600 4550 50  0001 C CNN "LCSC"
	1    3600 4550
	1    0    0    -1  
$EndComp
Text GLabel 4550 4950 2    50   Input ~ 0
MDC
Text GLabel 4550 4550 2    50   Input ~ 0
MDIO
Text GLabel 4550 5250 2    50   Input ~ 0
CRS_DV
Text GLabel 4550 5150 2    50   Input ~ 0
RXD1
Text GLabel 4550 5050 2    50   Input ~ 0
RXD0
Text GLabel 5550 4750 2    50   Input ~ 0
TXEN
Text GLabel 5550 4950 2    50   Input ~ 0
TXD1
Text GLabel 5550 4550 2    50   Input ~ 0
TXD0
Text GLabel 4550 3450 2    50   Input ~ 0
RX
Text GLabel 4550 3650 2    50   Input ~ 0
TX
Text GLabel 4550 3350 2    50   Input ~ 0
IO0
Text GLabel 5550 4350 2    50   Input ~ 0
CLK50MHZ
$Comp
L Device:R R7
U 1 1 5DF2BBCB
P 5300 4750
F 0 "R7" V 5400 4850 50  0000 C CNN
F 1 "49.9" V 5400 4650 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 5230 4750 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/szlcsc/Uniroyal-Elec-0402WGF499JTCE_C25120.pdf" H 5300 4750 50  0001 C CNN
F 4 "C25120" H 5300 4750 50  0001 C CNN "LCSC Part Number"
F 5 "C25120" H 5300 4750 50  0001 C CNN "LCSC"
	1    5300 4750
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R9
U 1 1 5DF2BD63
P 5300 4950
F 0 "R9" V 5400 5050 50  0000 C CNN
F 1 "49.9" V 5400 4850 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 5230 4950 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/szlcsc/Uniroyal-Elec-0402WGF499JTCE_C25120.pdf" H 5300 4950 50  0001 C CNN
F 4 "C25120" H 5300 4950 50  0001 C CNN "LCSC Part Number"
F 5 "C25120" H 5300 4950 50  0001 C CNN "LCSC"
	1    5300 4950
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4550 3650 4200 3650
Wire Wire Line
	4550 4550 4200 4550
Wire Wire Line
	4200 4950 4550 4950
Wire Wire Line
	4550 5050 4200 5050
Wire Wire Line
	4200 5150 4550 5150
Wire Wire Line
	4550 5250 4200 5250
Wire Wire Line
	4200 4650 4900 4650
Wire Wire Line
	4900 4650 4900 4550
Wire Wire Line
	4200 4850 4900 4850
Wire Wire Line
	4900 4850 4900 4950
Wire Wire Line
	5450 4950 5550 4950
Wire Wire Line
	5450 4750 5550 4750
Wire Wire Line
	5550 4550 5450 4550
NoConn ~ 3000 4550
NoConn ~ 3000 4650
NoConn ~ 3000 4750
NoConn ~ 3000 4850
NoConn ~ 3000 4950
NoConn ~ 3000 5050
NoConn ~ 3000 3650
NoConn ~ 3000 3550
$Comp
L power:GND #PWR019
U 1 1 5DF342C0
P 3600 6100
F 0 "#PWR019" H 3600 5850 50  0001 C CNN
F 1 "GND" H 3605 5927 50  0000 C CNN
F 2 "" H 3600 6100 50  0001 C CNN
F 3 "" H 3600 6100 50  0001 C CNN
	1    3600 6100
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 6100 3600 5950
Wire Wire Line
	3000 3350 2850 3350
$Comp
L power:+3.3V #PWR013
U 1 1 5DF362C5
P 3600 2850
F 0 "#PWR013" H 3600 2700 50  0001 C CNN
F 1 "+3.3V" H 3615 3023 50  0000 C CNN
F 2 "" H 3600 2850 50  0001 C CNN
F 3 "" H 3600 2850 50  0001 C CNN
	1    3600 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	2850 3350 2850 3250
Wire Wire Line
	2850 2950 2850 2850
$Comp
L power:+3.3V #PWR012
U 1 1 5DF35B8F
P 2850 2850
F 0 "#PWR012" H 2850 2700 50  0001 C CNN
F 1 "+3.3V" H 2865 3023 50  0000 C CNN
F 2 "" H 2850 2850 50  0001 C CNN
F 3 "" H 2850 2850 50  0001 C CNN
	1    2850 2850
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 5DF33A2D
P 2850 3100
F 0 "R2" H 3000 3050 50  0000 C CNN
F 1 "10K" H 3000 3150 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 2780 3100 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/szlcsc/Uniroyal-Elec-0402WGF1002TCE_C25744.pdf" H 2850 3100 50  0001 C CNN
F 4 "C25744" H 2850 3100 50  0001 C CNN "LCSC Part Number"
F 5 "C25744" H 2850 3100 50  0001 C CNN "LCSC"
	1    2850 3100
	-1   0    0    1   
$EndComp
Text GLabel 2650 3350 0    50   Input ~ 0
EN
Wire Wire Line
	2850 3350 2650 3350
Connection ~ 2850 3350
$Comp
L Device:R R6
U 1 1 5DF2A186
P 5300 4550
F 0 "R6" V 5400 4650 50  0000 C CNN
F 1 "49.9" V 5400 4450 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 5230 4550 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/szlcsc/Uniroyal-Elec-0402WGF499JTCE_C25120.pdf" H 5300 4550 50  0001 C CNN
F 4 "C25120" H 5300 4550 50  0001 C CNN "LCSC Part Number"
F 5 "C25120" H 5300 4550 50  0001 C CNN "LCSC"
	1    5300 4550
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4200 3450 4550 3450
NoConn ~ 4200 3550
NoConn ~ 4200 3850
Wire Wire Line
	4200 3350 4550 3350
Wire Wire Line
	4200 4750 5150 4750
Wire Wire Line
	4900 4550 5150 4550
Wire Wire Line
	4900 4950 5150 4950
Text Label 4250 4650 0    50   ~ 0
TXD0_R
Text Label 4250 4750 0    50   ~ 0
TXEN_R
Text Label 4250 4850 0    50   ~ 0
TXD1_R
Text Label 4250 5050 0    50   ~ 0
RXD0
Text Label 4250 5150 0    50   ~ 0
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
L power:GND #PWR017
U 1 1 5F4A3DFF
P 10550 6100
F 0 "#PWR017" H 10550 5850 50  0001 C CNN
F 1 "GND" H 10555 5927 50  0000 C CNN
F 2 "" H 10550 6100 50  0001 C CNN
F 3 "" H 10550 6100 50  0001 C CNN
	1    10550 6100
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_GSD Q3
U 1 1 5F4A7E31
P 9650 5800
F 0 "Q3" H 9856 5846 50  0000 L CNN
F 1 "2N7002" H 9856 5755 50  0001 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 9850 5900 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/szlcsc/Changjiang-Electronics-Tech-CJ-2N7002_C8545.pdf" H 9650 5800 50  0001 C CNN
F 4 "C8545" H 9650 5800 50  0001 C CNN "LCSC Part Number"
F 5 "C8545" H 9650 5800 50  0001 C CNN "LCSC"
	1    9650 5800
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_GSD Q2
U 1 1 5F4A98E8
P 10050 5550
F 0 "Q2" H 10256 5550 50  0000 L CNN
F 1 "2N7002" H 10256 5505 50  0001 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 10250 5650 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/szlcsc/Changjiang-Electronics-Tech-CJ-2N7002_C8545.pdf" H 10050 5550 50  0001 C CNN
F 4 "C8545" H 10050 5550 50  0001 C CNN "LCSC Part Number"
F 5 "C8545" H 10050 5550 50  0001 C CNN "LCSC"
	1    10050 5550
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_GSD Q1
U 1 1 5F4AC46E
P 10450 5300
F 0 "Q1" H 10656 5300 50  0000 L CNN
F 1 "2N7002" H 10656 5255 50  0001 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 10650 5400 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/szlcsc/Changjiang-Electronics-Tech-CJ-2N7002_C8545.pdf" H 10450 5300 50  0001 C CNN
F 4 "C8545" H 10450 5300 50  0001 C CNN "LCSC Part Number"
F 5 "C8545" H 10450 5300 50  0001 C CNN "LCSC"
	1    10450 5300
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 5F4B4C35
P 10150 4900
F 0 "R4" H 10220 4946 50  0000 L CNN
F 1 "47" H 10220 4855 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 10080 4900 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/szlcsc/Uniroyal-Elec-0603WAF470JT5E_C23182.pdf" H 10150 4900 50  0001 C CNN
F 4 "1/10W" H 10350 4770 50  0000 C CNN "Rating"
F 5 "C23182" H 10150 4900 50  0001 C CNN "LCSC Part Number"
F 6 "C23182" H 10150 4900 50  0001 C CNN "LCSC"
	1    10150 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	9450 5800 9200 5800
Wire Wire Line
	9850 5550 9200 5550
Wire Wire Line
	10250 5300 9200 5300
Text GLabel 9200 5800 0    50   Input ~ 0
Blue
Text GLabel 9200 5550 0    50   Input ~ 0
Green
Text GLabel 9200 5300 0    50   Input ~ 0
Red
$Comp
L power:VBUS #PWR?
U 1 1 5F4D638B
P 7300 4200
AR Path="/5F4D638B" Ref="#PWR?"  Part="1" 
AR Path="/5DED2F9B/5F4D638B" Ref="#PWR016"  Part="1" 
AR Path="/5DED3446/5F4D638B" Ref="#PWR036"  Part="1" 
F 0 "#PWR016" H 7300 4050 50  0001 C CNN
F 1 "VBUS" H 7315 4373 50  0000 C CNN
F 2 "" H 7300 4200 50  0001 C CNN
F 3 "" H 7300 4200 50  0001 C CNN
	1    7300 4200
	1    0    0    -1  
$EndComp
$Comp
L Device:R R8
U 1 1 5F4D639E
P 7300 4450
F 0 "R8" H 7370 4496 50  0000 L CNN
F 1 "10K" H 7370 4405 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 7230 4450 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/szlcsc/Uniroyal-Elec-0402WGF1002TCE_C25744.pdf" H 7300 4450 50  0001 C CNN
F 4 "C25744" H 7300 4450 50  0001 C CNN "LCSC Part Number"
F 5 "C25744" H 7300 4450 50  0001 C CNN "LCSC"
	1    7300 4450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR018
U 1 1 5F4DC54A
P 7300 5250
F 0 "#PWR018" H 7300 5000 50  0001 C CNN
F 1 "GND" H 7305 5077 50  0000 C CNN
F 2 "" H 7300 5250 50  0001 C CNN
F 3 "" H 7300 5250 50  0001 C CNN
	1    7300 5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	7300 5150 7300 5250
Wire Wire Line
	7300 4300 7300 4200
Wire Wire Line
	7300 4650 7000 4650
Connection ~ 7300 4650
Wire Wire Line
	7300 4650 7300 4600
Text GLabel 7000 4650 0    50   Input ~ 0
Block
Wire Wire Line
	9750 6000 9750 6050
Wire Wire Line
	9750 6050 10150 6050
Wire Wire Line
	10550 6050 10550 6100
Wire Wire Line
	10550 5500 10550 6050
Connection ~ 10550 6050
Wire Wire Line
	10150 5750 10150 6050
Connection ~ 10150 6050
Wire Wire Line
	10150 6050 10550 6050
Wire Wire Line
	7300 4650 7300 4750
Wire Wire Line
	4200 4350 4550 4350
Wire Wire Line
	4200 5350 4550 5350
Wire Wire Line
	4200 5450 4550 5450
Text GLabel 4550 4350 2    50   Input ~ 0
Blue
Text GLabel 4550 5350 2    50   Input ~ 0
Red
Text GLabel 4550 5450 2    50   Input ~ 0
Green
Text GLabel 4550 5550 2    50   Input ~ 0
Block
Wire Wire Line
	4200 5550 4550 5550
Wire Wire Line
	3600 2850 3600 3150
Text Notes 3750 3150 0    50   ~ 0
No decoupling needed\n10uf, 1uf, 0.1uf caps inside\n 
$Comp
L LED:ASMT-YTB7-0AA02 D2
U 1 1 5FCC646A
P 9350 4500
F 0 "D2" H 9350 4867 50  0000 C CNN
F 1 "RS-3535MWAR" H 9350 4776 50  0000 C CNN
F 2 "LED_SMD:LED_RGB_PLCC-6" H 9150 4180 50  0001 L CNN
F 3 "https://datasheet.lcsc.com/szlcsc/Foshan-NationStar-Optoelectronics-RS-3535MWAR-2JYJY-J_C110402.pdf" H 9500 4500 50  0001 L CNN
F 4 " C110402" H 9350 4500 50  0001 C CNN "LCSC Part Number"
F 5 " C110402" H 9350 4500 50  0001 C CNN "LCSC"
	1    9350 4500
	1    0    0    -1  
$EndComp
$Comp
L power:VBUS #PWR?
U 1 1 5FEEF4AA
P 8850 4200
AR Path="/5FEEF4AA" Ref="#PWR?"  Part="1" 
AR Path="/5DED2F9B/5FEEF4AA" Ref="#PWR015"  Part="1" 
AR Path="/5DED3446/5FEEF4AA" Ref="#PWR032"  Part="1" 
F 0 "#PWR015" H 8850 4050 50  0001 C CNN
F 1 "VBUS" H 8865 4373 50  0000 C CNN
F 2 "" H 8850 4200 50  0001 C CNN
F 3 "" H 8850 4200 50  0001 C CNN
	1    8850 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	8850 4200 8850 4400
Wire Wire Line
	8850 4600 9050 4600
Wire Wire Line
	9050 4500 8850 4500
Connection ~ 8850 4500
Wire Wire Line
	8850 4500 8850 4600
Wire Wire Line
	9050 4400 8850 4400
Connection ~ 8850 4400
Wire Wire Line
	8850 4400 8850 4500
Wire Wire Line
	10550 5050 10550 5100
Wire Wire Line
	10150 5050 10150 5350
Wire Wire Line
	9750 5050 9750 5600
$Comp
L Device:R R3
U 1 1 5F4B5722
P 9750 4900
F 0 "R3" H 9820 4946 50  0000 L CNN
F 1 "47" H 9820 4855 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9680 4900 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/szlcsc/Uniroyal-Elec-0603WAF470JT5E_C23182.pdf" H 9750 4900 50  0001 C CNN
F 4 "1/10W" H 9950 4770 50  0000 C CNN "Rating"
F 5 "C23182" H 9750 4900 50  0001 C CNN "LCSC Part Number"
F 6 "C23182" H 9750 4900 50  0001 C CNN "LCSC"
	1    9750 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	9650 4600 9750 4600
Wire Wire Line
	9750 4600 9750 4750
Wire Wire Line
	9650 4500 10150 4500
Wire Wire Line
	10150 4500 10150 4750
Wire Wire Line
	9650 4400 10550 4400
Wire Wire Line
	10550 4400 10550 4750
$Comp
L Switch:SW_SPST SW1
U 1 1 5FF060B9
P 7300 4950
F 0 "SW1" V 7346 4862 50  0000 R CNN
F 1 "PTS645VH83-2 LFS" V 7255 4862 50  0000 R CNN
F 2 "Button_Switch_THT:SW_Tactile_SPST_Angled_PTS645Vx83-2LFS" H 7300 4950 50  0001 C CNN
F 3 "https://www.ckswitches.com/media/1471/pts645.pdf" H 7300 4950 50  0001 C CNN
	1    7300 4950
	0    -1   -1   0   
$EndComp
Text Notes 9350 3950 0    50   ~ 0
RED: 2.2V @ 40mA\nGREEN: 3.5V @ 40mA\nBLUE: 3.5V @ 40mA\n
$Comp
L Device:R R5
U 1 1 5F4B4082
P 10550 4900
F 0 "R5" H 10620 4946 50  0000 L CNN
F 1 "82" H 10620 4855 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric" V 10480 4900 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/szlcsc/Uniroyal-Elec-1206W4F825JT5E_C247183.pdf" H 10550 4900 50  0001 C CNN
F 4 "1/4W" H 10730 4770 50  0000 C CNN "Rating"
F 5 "C247183" H 10550 4900 50  0001 C CNN "LCSC Part Number"
F 6 "C247183" H 10550 4900 50  0001 C CNN "LCSC"
	1    10550 4900
	1    0    0    -1  
$EndComp
$Comp
L power:Earth #PWR020
U 1 1 5FF73B5F
P 3450 7050
F 0 "#PWR020" H 3450 6800 50  0001 C CNN
F 1 "Earth" H 3450 6900 50  0001 C CNN
F 2 "" H 3450 7050 50  0001 C CNN
F 3 "~" H 3450 7050 50  0001 C CNN
	1    3450 7050
	1    0    0    -1  
$EndComp
$Comp
L power:Earth #PWR021
U 1 1 5FF749E4
P 4300 7050
F 0 "#PWR021" H 4300 6800 50  0001 C CNN
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
	4200 4250 4550 4250
Text GLabel 4550 4250 2    50   Input ~ 0
MTDO
Wire Wire Line
	4200 4150 4550 4150
Text GLabel 4550 4150 2    50   Input ~ 0
MTMS
Wire Wire Line
	4200 4050 4550 4050
Text GLabel 4550 4050 2    50   Input ~ 0
MTCK
Wire Wire Line
	4200 3950 4550 3950
Text GLabel 4550 3950 2    50   Input ~ 0
MTDI
$Comp
L Device:C C?
U 1 1 601D1313
P 3350 1600
AR Path="/601D1313" Ref="C?"  Part="1" 
AR Path="/5DED2F9B/601D1313" Ref="C2"  Part="1" 
AR Path="/5DED3446/601D1313" Ref="C?"  Part="1" 
F 0 "C2" H 3465 1646 50  0000 L CNN
F 1 "0.1uF" H 3465 1555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 3388 1450 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/szlcsc/Samsung-Electro-Mechanics-CL05B104KO5NNNC_C1525.pdf" H 3350 1600 50  0001 C CNN
F 4 "C1525" H 3350 1600 50  0001 C CNN "LCSC Part Number"
F 5 "10V" H 3540 1470 50  0000 C CNN "Rating"
F 6 "C1525" H 3350 1600 50  0001 C CNN "LCSC"
	1    3350 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3350 1450 3350 1350
$Comp
L power:GND #PWR?
U 1 1 601D1320
P 3350 1900
AR Path="/601D1320" Ref="#PWR?"  Part="1" 
AR Path="/5DED2F9B/601D1320" Ref="#PWR08"  Part="1" 
AR Path="/5DED3446/601D1320" Ref="#PWR?"  Part="1" 
F 0 "#PWR08" H 3350 1650 50  0001 C CNN
F 1 "GND" H 3355 1727 50  0000 C CNN
F 2 "" H 3350 1900 50  0001 C CNN
F 3 "" H 3350 1900 50  0001 C CNN
	1    3350 1900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 601D1326
P 2050 2050
AR Path="/601D1326" Ref="#PWR?"  Part="1" 
AR Path="/5DED2F9B/601D1326" Ref="#PWR06"  Part="1" 
AR Path="/5DED3446/601D1326" Ref="#PWR?"  Part="1" 
F 0 "#PWR06" H 2050 1800 50  0001 C CNN
F 1 "GND" H 2055 1877 50  0000 C CNN
F 2 "" H 2050 2050 50  0001 C CNN
F 3 "" H 2050 2050 50  0001 C CNN
	1    2050 2050
	1    0    0    -1  
$EndComp
$Comp
L Regulator_Linear:AZ1117-3.3 U?
U 1 1 601D132D
P 3900 1350
AR Path="/601D132D" Ref="U?"  Part="1" 
AR Path="/5DED2F9B/601D132D" Ref="U1"  Part="1" 
AR Path="/5DED3446/601D132D" Ref="U?"  Part="1" 
F 0 "U1" H 3900 1592 50  0000 C CNN
F 1 "AZ1117EH-3.3TRG1" H 3900 1501 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223" H 3900 1600 50  0001 C CIN
F 3 "https://datasheet.lcsc.com/szlcsc/Diodes-Incorporated-AZ1117EH-3-3TRG1_C108494.pdf" H 3900 1350 50  0001 C CNN
F 4 "X" H 3900 1350 50  0001 C CNN "Extended Part"
F 5 "C108494" H 3900 1350 50  0001 C CNN "LCSC Part Number"
F 6 "C108494" H 3900 1350 50  0001 C CNN "LCSC"
	1    3900 1350
	1    0    0    -1  
$EndComp
$Comp
L power:VBUS #PWR?
U 1 1 601D1333
P 2950 1050
AR Path="/601D1333" Ref="#PWR?"  Part="1" 
AR Path="/5DED2F9B/601D1333" Ref="#PWR02"  Part="1" 
AR Path="/5DED3446/601D1333" Ref="#PWR?"  Part="1" 
F 0 "#PWR02" H 2950 900 50  0001 C CNN
F 1 "VBUS" H 2965 1223 50  0000 C CNN
F 2 "" H 2950 1050 50  0001 C CNN
F 3 "" H 2950 1050 50  0001 C CNN
	1    2950 1050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 601D1339
P 2950 1900
AR Path="/601D1339" Ref="#PWR?"  Part="1" 
AR Path="/5DED2F9B/601D1339" Ref="#PWR07"  Part="1" 
AR Path="/5DED3446/601D1339" Ref="#PWR?"  Part="1" 
F 0 "#PWR07" H 2950 1650 50  0001 C CNN
F 1 "GND" H 2955 1727 50  0000 C CNN
F 2 "" H 2950 1900 50  0001 C CNN
F 3 "" H 2950 1900 50  0001 C CNN
	1    2950 1900
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 601D1341
P 2950 1600
AR Path="/601D1341" Ref="C?"  Part="1" 
AR Path="/5DED2F9B/601D1341" Ref="C1"  Part="1" 
AR Path="/5DED3446/601D1341" Ref="C?"  Part="1" 
F 0 "C1" H 3065 1646 50  0000 L CNN
F 1 "1uF" H 3065 1555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 2988 1450 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/szlcsc/Samsung-Electro-Mechanics-CL05A105KA5NQNC_C52923.pdf" H 2950 1600 50  0001 C CNN
F 4 "C52923" H 2950 1600 50  0001 C CNN "LCSC Part Number"
F 5 "10V" H 3140 1470 50  0000 C CNN "Rating"
F 6 "C52923" H 2950 1600 50  0001 C CNN "LCSC"
	1    2950 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 1050 2950 1350
$Comp
L power:GND #PWR?
U 1 1 601D1348
P 3900 1900
AR Path="/601D1348" Ref="#PWR?"  Part="1" 
AR Path="/5DED2F9B/601D1348" Ref="#PWR09"  Part="1" 
AR Path="/5DED3446/601D1348" Ref="#PWR?"  Part="1" 
F 0 "#PWR09" H 3900 1650 50  0001 C CNN
F 1 "GND" H 3905 1727 50  0000 C CNN
F 2 "" H 3900 1900 50  0001 C CNN
F 3 "" H 3900 1900 50  0001 C CNN
	1    3900 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 1650 3900 1900
$Comp
L power:+3.3V #PWR?
U 1 1 601D134F
P 4550 1050
AR Path="/601D134F" Ref="#PWR?"  Part="1" 
AR Path="/5DED2F9B/601D134F" Ref="#PWR03"  Part="1" 
AR Path="/5DED3446/601D134F" Ref="#PWR?"  Part="1" 
F 0 "#PWR03" H 4550 900 50  0001 C CNN
F 1 "+3.3V" H 4565 1223 50  0000 C CNN
F 2 "" H 4550 1050 50  0001 C CNN
F 3 "" H 4550 1050 50  0001 C CNN
	1    4550 1050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 601D1355
P 4550 1900
AR Path="/601D1355" Ref="#PWR?"  Part="1" 
AR Path="/5DED2F9B/601D1355" Ref="#PWR010"  Part="1" 
AR Path="/5DED3446/601D1355" Ref="#PWR?"  Part="1" 
F 0 "#PWR010" H 4550 1650 50  0001 C CNN
F 1 "GND" H 4555 1727 50  0000 C CNN
F 2 "" H 4550 1900 50  0001 C CNN
F 3 "" H 4550 1900 50  0001 C CNN
	1    4550 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 1350 4550 1350
Connection ~ 4550 1350
$Comp
L Device:C C?
U 1 1 601D135F
P 4550 1600
AR Path="/601D135F" Ref="C?"  Part="1" 
AR Path="/5DED2F9B/601D135F" Ref="C3"  Part="1" 
AR Path="/5DED3446/601D135F" Ref="C?"  Part="1" 
F 0 "C3" H 4665 1646 50  0000 L CNN
F 1 "10uF" H 4665 1555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 4588 1450 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/szlcsc/Samsung-Electro-Mechanics-CL10A106KP8NNNC_C19702.pdf" H 4550 1600 50  0001 C CNN
F 4 "C15850" H 4550 1600 50  0001 C CNN "LCSC Part Number"
F 5 "10V" H 4740 1470 50  0000 C CNN "Rating"
F 6 "C15850" H 4550 1600 50  0001 C CNN "LCSC"
	1    4550 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4550 1450 4550 1350
Wire Wire Line
	2950 1450 2950 1350
Connection ~ 2950 1350
Wire Wire Line
	2950 1750 2950 1900
Wire Wire Line
	4550 1050 4550 1350
NoConn ~ 2250 1750
Wire Wire Line
	4550 1750 4550 1900
Wire Wire Line
	3350 1750 3350 1900
Connection ~ 3350 1350
Wire Wire Line
	3350 1350 2950 1350
Wire Wire Line
	3350 1350 3600 1350
Wire Wire Line
	1950 1950 1950 2000
Wire Wire Line
	1950 2000 2050 2000
Wire Wire Line
	2050 2000 2050 2050
Wire Wire Line
	1850 1950 1850 2000
Wire Wire Line
	1850 2000 1750 2000
Wire Wire Line
	1750 2000 1750 2050
$Comp
L power:Earth #PWR?
U 1 1 601D1383
P 1750 2050
AR Path="/5DED3446/601D1383" Ref="#PWR?"  Part="1" 
AR Path="/5DED2F9B/601D1383" Ref="#PWR05"  Part="1" 
F 0 "#PWR05" H 1750 1800 50  0001 C CNN
F 1 "Earth" H 1750 1900 50  0001 C CNN
F 2 "" H 1750 2050 50  0001 C CNN
F 3 "~" H 1750 2050 50  0001 C CNN
	1    1750 2050
	1    0    0    -1  
$EndComp
Text Notes 1150 4500 0    50   ~ 0
Strapping Pins:\n• GPIO0: internal pull-up\n• GPIO2: internal pull-down\n• GPIO4: internal pull-down\n• GPIO5: internal pull-up\n• MTDI/GPIO12: internal pull-down\n• MTDO/GPIO15: internal pull-up
Text Notes 1150 4950 0    50   ~ 0
Can't use for LEDs, \nPulled up during startup:\n1, 3, 6, 7, 8, \n9, 10, 11, 14
Text Notes 1150 5200 0    50   ~ 0
Input Only:\n34, 35, SENSOR_VP, SENSOR_VN
$Comp
L power:GND #PWR?
U 1 1 602095DF
P 6100 1900
AR Path="/602095DF" Ref="#PWR?"  Part="1" 
AR Path="/5DED2F9B/602095DF" Ref="#PWR011"  Part="1" 
AR Path="/5DED3446/602095DF" Ref="#PWR?"  Part="1" 
F 0 "#PWR011" H 6100 1650 50  0001 C CNN
F 1 "GND" H 6105 1727 50  0000 C CNN
F 2 "" H 6100 1900 50  0001 C CNN
F 3 "" H 6100 1900 50  0001 C CNN
	1    6100 1900
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 602095E6
P 6100 1300
AR Path="/602095E6" Ref="R?"  Part="1" 
AR Path="/5DED2F9B/602095E6" Ref="R1"  Part="1" 
AR Path="/5DED3446/602095E6" Ref="R?"  Part="1" 
F 0 "R1" H 6170 1346 50  0000 L CNN
F 1 "470" H 6170 1255 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 6030 1300 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/szlcsc/Uniroyal-Elec-0402WGD4700TCE_C270656.pdf" H 6100 1300 50  0001 C CNN
F 4 "1/16W" H 6300 1170 50  0000 C CNN "Rating"
F 5 " C25117" H 6100 1300 50  0001 C CNN "LCSC Part Number"
F 6 " C25117" H 6100 1300 50  0001 C CNN "LCSC"
	1    6100 1300
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D?
U 1 1 602095ED
P 6100 1650
AR Path="/602095ED" Ref="D?"  Part="1" 
AR Path="/5DED2F9B/602095ED" Ref="D1"  Part="1" 
AR Path="/5DED3446/602095ED" Ref="D?"  Part="1" 
F 0 "D1" V 6139 1533 50  0000 R CNN
F 1 "GRN" V 6048 1533 50  0000 R CNN
F 2 "Diode_SMD:D_0603_1608Metric" H 6100 1650 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/szlcsc/Everlight-Elec-19-217-GHC-YR1S2-3T_C72043.pdf" H 6100 1650 50  0001 C CNN
F 4 "C72043" H 6100 1650 50  0001 C CNN "LCSC Part Number"
F 5 "C72043" H 6100 1650 50  0001 C CNN "LCSC"
	1    6100 1650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6100 1050 6100 1150
Wire Wire Line
	6100 1450 6100 1500
Wire Wire Line
	6100 1800 6100 1900
$Comp
L power:+3.3V #PWR?
U 1 1 602095F6
P 6100 1050
AR Path="/602095F6" Ref="#PWR?"  Part="1" 
AR Path="/5DED2F9B/602095F6" Ref="#PWR04"  Part="1" 
AR Path="/5DED3446/602095F6" Ref="#PWR?"  Part="1" 
F 0 "#PWR04" H 6100 900 50  0001 C CNN
F 1 "+3.3V" H 6115 1223 50  0000 C CNN
F 2 "" H 6100 1050 50  0001 C CNN
F 3 "" H 6100 1050 50  0001 C CNN
	1    6100 1050
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 602C4488
P 5300 4350
AR Path="/602C4488" Ref="R?"  Part="1" 
AR Path="/5DED2F9B/602C4488" Ref="R22"  Part="1" 
AR Path="/5DED3446/602C4488" Ref="R?"  Part="1" 
F 0 "R22" V 5200 4200 50  0000 L CNN
F 1 "49.9" V 5200 4400 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 5230 4350 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/szlcsc/Uniroyal-Elec-0402WGF499JTCE_C25120.pdf" H 5300 4350 50  0001 C CNN
F 4 "C25120" H 5300 4350 50  0001 C CNN "LCSC Part Number"
F 5 "C25120" H 5300 4350 50  0001 C CNN "LCSC"
	1    5300 4350
	0    1    1    0   
$EndComp
Wire Wire Line
	4900 4450 4900 4350
Wire Wire Line
	4900 4350 5150 4350
Wire Wire Line
	4200 4450 4900 4450
Wire Wire Line
	5450 4350 5550 4350
Text Label 4250 4450 0    50   ~ 0
CLK_R
Wire Wire Line
	4200 3750 4550 3750
Text GLabel 4550 3750 2    50   Input ~ 0
nRST
Text Label 4250 3750 0    50   ~ 0
nRST
Text Label 4250 4950 0    50   ~ 0
MDC
Text Label 4250 4550 0    50   ~ 0
MDIO
Wire Wire Line
	4200 5650 4550 5650
Text GLabel 4550 5650 2    50   Input ~ 0
nINT
Text Label 4250 5650 0    50   ~ 0
nINT
Wire Wire Line
	2250 1650 2450 1650
Wire Wire Line
	2250 1550 2450 1550
Text Label 2350 1550 0    50   ~ 0
D+
Text Label 2350 1650 0    50   ~ 0
D-
$Comp
L dk_Interface-Controllers:CP2102-GMR_NRND U4
U 1 1 605ABDE8
P 8100 2050
F 0 "U4" H 7900 2950 60  0000 C CNN
F 1 "CP2102" H 7900 2850 60  0000 C CNN
F 2 "digikey-footprints:VFQFN-28-1EP_5x5mm" H 8300 2250 60  0001 L CNN
F 3 "https://www.silabs.com/documents/public/data-sheets/CP2102-9.pdf" H 8300 2350 60  0001 L CNN
F 4 "336-1160-1-ND" H 8300 2450 60  0001 L CNN "Digi-Key_PN"
F 5 "CP2102-GMR" H 8300 2550 60  0001 L CNN "MPN"
F 6 "Integrated Circuits (ICs)" H 8300 2650 60  0001 L CNN "Category"
F 7 "Interface - Controllers" H 8300 2750 60  0001 L CNN "Family"
F 8 "https://www.silabs.com/documents/public/data-sheets/CP2102-9.pdf" H 8300 2850 60  0001 L CNN "DK_Datasheet_Link"
F 9 "/product-detail/en/silicon-labs/CP2102-GMR/336-1160-1-ND/3672615" H 8300 2950 60  0001 L CNN "DK_Detail_Page"
F 10 "IC USB-TO-UART BRIDGE 28VQFN" H 8300 3050 60  0001 L CNN "Description"
F 11 "Silicon Labs" H 8300 3150 60  0001 L CNN "Manufacturer"
F 12 "Not For New Designs" H 8300 3250 60  0001 L CNN "Status"
F 13 "C6568" H 8100 2050 50  0001 C CNN "LCSC Part Number"
F 14 "C6568" H 8100 2050 50  0001 C CNN "LCSC"
	1    8100 2050
	1    0    0    -1  
$EndComp
NoConn ~ 8300 1250
$Comp
L power:+3.3V #PWR?
U 1 1 605B6204
P 8200 1150
AR Path="/605B6204" Ref="#PWR?"  Part="1" 
AR Path="/5DED2F9B/605B6204" Ref="#PWR0107"  Part="1" 
AR Path="/5DED3446/605B6204" Ref="#PWR?"  Part="1" 
F 0 "#PWR0107" H 8200 1000 50  0001 C CNN
F 1 "+3.3V" H 8150 1300 50  0000 C CNN
F 2 "" H 8200 1150 50  0001 C CNN
F 3 "" H 8200 1150 50  0001 C CNN
	1    8200 1150
	1    0    0    -1  
$EndComp
$Comp
L power:VBUS #PWR?
U 1 1 605B9CF8
P 8400 1150
AR Path="/605B9CF8" Ref="#PWR?"  Part="1" 
AR Path="/5DED2F9B/605B9CF8" Ref="#PWR0108"  Part="1" 
AR Path="/5DED3446/605B9CF8" Ref="#PWR?"  Part="1" 
F 0 "#PWR0108" H 8400 1000 50  0001 C CNN
F 1 "VBUS" H 8400 1300 50  0000 C CNN
F 2 "" H 8400 1150 50  0001 C CNN
F 3 "" H 8400 1150 50  0001 C CNN
	1    8400 1150
	1    0    0    -1  
$EndComp
Wire Wire Line
	8400 1250 8400 1150
Wire Wire Line
	8200 1250 8200 1150
Wire Wire Line
	7800 1650 7650 1650
Wire Wire Line
	7800 1750 7650 1750
Text Label 7650 1650 0    50   ~ 0
D+
Text Label 7650 1750 0    50   ~ 0
D-
$Comp
L power:VBUS #PWR?
U 1 1 605C8BA2
P 7450 1750
AR Path="/605C8BA2" Ref="#PWR?"  Part="1" 
AR Path="/5DED2F9B/605C8BA2" Ref="#PWR0109"  Part="1" 
AR Path="/5DED3446/605C8BA2" Ref="#PWR?"  Part="1" 
F 0 "#PWR0109" H 7450 1600 50  0001 C CNN
F 1 "VBUS" H 7450 1900 50  0000 C CNN
F 2 "" H 7450 1750 50  0001 C CNN
F 3 "" H 7450 1750 50  0001 C CNN
	1    7450 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	7800 1850 7650 1850
Wire Wire Line
	7450 1850 7450 1750
Wire Wire Line
	8600 1950 8800 1950
Wire Wire Line
	8600 1850 8800 1850
Wire Wire Line
	8600 2050 8800 2050
Wire Wire Line
	7800 2050 7650 2050
Wire Wire Line
	7800 1950 7650 1950
Wire Wire Line
	7650 1950 7650 1850
Connection ~ 7650 1850
Wire Wire Line
	7650 1850 7450 1850
Text GLabel 7650 2050 0    50   Input ~ 0
RX
Text GLabel 8800 2050 2    50   Input ~ 0
DTR
Text GLabel 8800 1950 2    50   Input ~ 0
TX
Text GLabel 8800 1850 2    50   Input ~ 0
RTS
NoConn ~ 8600 2150
NoConn ~ 8600 2250
NoConn ~ 8600 2350
NoConn ~ 8600 2450
NoConn ~ 8600 2550
NoConn ~ 8600 2650
NoConn ~ 7800 2750
NoConn ~ 7800 2650
NoConn ~ 7800 2550
NoConn ~ 7800 2450
NoConn ~ 7800 2350
NoConn ~ 7800 2250
NoConn ~ 7800 2150
NoConn ~ 8600 1750
NoConn ~ 8600 1650
NoConn ~ 7800 1550
NoConn ~ 7800 1450
$Comp
L power:GND #PWR?
U 1 1 60618015
P 8200 3150
AR Path="/60618015" Ref="#PWR?"  Part="1" 
AR Path="/5DED2F9B/60618015" Ref="#PWR0110"  Part="1" 
AR Path="/5DED3446/60618015" Ref="#PWR?"  Part="1" 
F 0 "#PWR0110" H 8200 2900 50  0001 C CNN
F 1 "GND" H 8205 2977 50  0000 C CNN
F 2 "" H 8200 3150 50  0001 C CNN
F 3 "" H 8200 3150 50  0001 C CNN
	1    8200 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	8200 3050 8200 3100
Wire Wire Line
	8200 3100 8300 3100
Wire Wire Line
	8300 3100 8300 3050
Connection ~ 8200 3100
Wire Wire Line
	8200 3100 8200 3150
$Comp
L Device:Q_NMOS_GSD Q5
U 1 1 606237C0
P 10150 1600
F 0 "Q5" H 10354 1646 50  0000 L CNN
F 1 "2N7002" H 10354 1555 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 10350 1700 50  0001 C CNN
F 3 "https://www.onsemi.com/pub/Collateral/2N7002E-D.PDF" H 10150 1600 50  0001 C CNN
F 4 "2N7002" H 10150 1600 50  0001 C CNN "LCSC Alternate"
F 5 "C8545" H 10150 1600 50  0001 C CNN "LCSC Part Number"
F 6 "https://datasheet.lcsc.com/szlcsc/Changjiang-Electronics-Tech-CJ-2N7002_C8545.pdf" H 10150 1600 50  0001 C CNN "Alternate Datasheet"
F 7 "C8545" H 10150 1600 50  0001 C CNN "LCSC"
	1    10150 1600
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_GSD Q4
U 1 1 6062D0F6
P 10150 2200
F 0 "Q4" H 10354 2154 50  0000 L CNN
F 1 "2N7002" H 10354 2245 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 10350 2300 50  0001 C CNN
F 3 "https://www.onsemi.com/pub/Collateral/2N7002E-D.PDF" H 10150 2200 50  0001 C CNN
F 4 "2N7002" H 10150 2200 50  0001 C CNN "LCSC Alternate"
F 5 "C8545" H 10150 2200 50  0001 C CNN "LCSC Part Number"
F 6 "https://datasheet.lcsc.com/szlcsc/Changjiang-Electronics-Tech-CJ-2N7002_C8545.pdf" H 10150 2200 50  0001 C CNN "Alternate Datasheet"
F 7 "C8545" H 10150 2200 50  0001 C CNN "LCSC"
	1    10150 2200
	1    0    0    1   
$EndComp
Wire Wire Line
	10250 2400 10250 2500
Wire Wire Line
	10250 2500 10450 2500
Wire Wire Line
	10250 1400 10250 1300
Wire Wire Line
	10800 1300 11000 1300
Wire Wire Line
	9950 1600 9900 1600
Wire Wire Line
	9950 2200 9850 2200
Text GLabel 9650 1600 0    50   Input ~ 0
DTR
Text GLabel 9650 2200 0    50   Input ~ 0
RTS
Text GLabel 11000 1300 2    50   Input ~ 0
EN
Text GLabel 10450 2500 2    50   Input ~ 0
IO0
Wire Wire Line
	10250 2000 10250 1950
Wire Wire Line
	10250 1950 9900 1950
Wire Wire Line
	9900 1950 9900 1600
Connection ~ 9900 1600
Wire Wire Line
	9900 1600 9650 1600
Wire Wire Line
	10250 1800 10250 1850
Wire Wire Line
	10250 1850 9850 1850
Wire Wire Line
	9850 1850 9850 2200
Connection ~ 9850 2200
Wire Wire Line
	9850 2200 9650 2200
Text Notes 7400 850  0    50   ~ 0
Programming Circuit:\nAutomatically puts ESP32 in programming mode when using ESP-IDF.\n\nhttps://github.com/espressif/esptool/wiki/ESP32-Boot-Mode-Selection#automatic-bootloader
Wire Wire Line
	2250 1350 2950 1350
$Comp
L Connector:USB_B_Micro J1
U 1 1 6071C560
P 1950 1550
F 0 "J1" H 2007 2017 50  0000 C CNN
F 1 "USB_B_Micro" H 2007 1926 50  0000 C CNN
F 2 "Connector_USB:USB_Micro-B_Molex-105017-0001" H 2100 1500 50  0001 C CNN
F 3 "~" H 2100 1500 50  0001 C CNN
	1    1950 1550
	1    0    0    -1  
$EndComp
Text Notes 9700 3100 0    50   ~ 0
Auto program\nDTR RTS-->EN IO0\n 1   1       1   1\n 0   0       1   1\n 1   0       0   1\n 0   1       1   0
$Comp
L Device:C C?
U 1 1 606C3113
P 10800 1600
AR Path="/606C3113" Ref="C?"  Part="1" 
AR Path="/5DED2F9B/606C3113" Ref="C12"  Part="1" 
AR Path="/5DED3446/606C3113" Ref="C?"  Part="1" 
F 0 "C12" H 10915 1646 50  0000 L CNN
F 1 "1uF" H 10915 1555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 10838 1450 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/szlcsc/Samsung-Electro-Mechanics-CL05A105KA5NQNC_C52923.pdf" H 10800 1600 50  0001 C CNN
F 4 "C52923" H 10800 1600 50  0001 C CNN "LCSC Part Number"
F 5 "10V" H 10990 1470 50  0000 C CNN "Rating"
F 6 "C52923" H 10800 1600 50  0001 C CNN "LCSC"
	1    10800 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	10800 1450 10800 1300
Wire Wire Line
	10800 1300 10250 1300
Connection ~ 10800 1300
Wire Wire Line
	10800 1750 10800 1800
$Comp
L power:GND #PWR?
U 1 1 606D57F4
P 10800 1800
AR Path="/606D57F4" Ref="#PWR?"  Part="1" 
AR Path="/5DED2F9B/606D57F4" Ref="#PWR01"  Part="1" 
AR Path="/5DED3446/606D57F4" Ref="#PWR?"  Part="1" 
F 0 "#PWR01" H 10800 1550 50  0001 C CNN
F 1 "GND" H 10805 1627 50  0000 C CNN
F 2 "" H 10800 1800 50  0001 C CNN
F 3 "" H 10800 1800 50  0001 C CNN
	1    10800 1800
	1    0    0    -1  
$EndComp
Text Notes 10450 1250 0    50   ~ 0
Need 1uF for \ncircuit to work
$EndSCHEMATC
