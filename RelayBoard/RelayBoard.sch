EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	2100 1750 2000 1750
Wire Wire Line
	2400 1950 2400 2000
$Comp
L power:GND #PWR02
U 1 1 62E8051A
P 2400 2000
F 0 "#PWR02" H 2400 1750 50  0001 C CNN
F 1 "GND" H 2405 1827 50  0000 C CNN
F 2 "" H 2400 2000 50  0001 C CNN
F 3 "" H 2400 2000 50  0001 C CNN
	1    2400 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	3700 1200 3700 1650
Wire Wire Line
	2750 1200 3700 1200
Wire Wire Line
	2400 1500 2400 1550
Wire Wire Line
	2750 1500 2400 1500
Wire Wire Line
	2750 1650 2750 1500
Wire Wire Line
	2950 1650 2750 1650
Wire Wire Line
	3700 1650 3550 1650
Connection ~ 3700 1200
Wire Wire Line
	3700 1100 3700 1200
Connection ~ 2750 1500
$Comp
L Device:D D1
U 1 1 62E8052A
P 2750 1350
F 0 "D1" V 2704 1429 50  0000 L CNN
F 1 "D" V 2795 1429 50  0000 L CNN
F 2 "Diode_THT:D_T-1_P10.16mm_Horizontal" H 2750 1350 50  0001 C CNN
F 3 "~" H 2750 1350 50  0001 C CNN
	1    2750 1350
	0    1    1    0   
$EndComp
$Comp
L power:+12V #PWR06
U 1 1 62E80530
P 3700 1100
F 0 "#PWR06" H 3700 950 50  0001 C CNN
F 1 "+12V" H 3715 1273 50  0000 C CNN
F 2 "" H 3700 1100 50  0001 C CNN
F 3 "" H 3700 1100 50  0001 C CNN
	1    3700 1100
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R1
U 1 1 62E80536
P 1850 1750
F 0 "R1" V 1645 1750 50  0000 C CNN
F 1 "2k" V 1736 1750 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 1890 1740 50  0001 C CNN
F 3 "~" H 1850 1750 50  0001 C CNN
	1    1850 1750
	0    1    1    0   
$EndComp
$Comp
L Transistor_BJT:2SC1815 Q1
U 1 1 62E8053C
P 2300 1750
F 0 "Q1" H 2490 1796 50  0000 L CNN
F 1 "2SC1815" H 2490 1705 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline_Wide" H 2500 1675 50  0001 L CIN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Toshiba%20PDFs/2SC1815.pdf" H 2300 1750 50  0001 L CNN
	1    2300 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	2150 3150 2050 3150
Wire Wire Line
	2450 3350 2450 3400
$Comp
L power:GND #PWR04
U 1 1 62E83470
P 2450 3400
F 0 "#PWR04" H 2450 3150 50  0001 C CNN
F 1 "GND" H 2455 3227 50  0000 C CNN
F 2 "" H 2450 3400 50  0001 C CNN
F 3 "" H 2450 3400 50  0001 C CNN
	1    2450 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	3750 2600 3750 3050
Wire Wire Line
	2800 2600 3750 2600
Wire Wire Line
	2450 2900 2450 2950
Wire Wire Line
	2800 2900 2450 2900
Wire Wire Line
	2800 3050 2800 2900
Wire Wire Line
	3000 3050 2800 3050
Wire Wire Line
	3750 3050 3600 3050
Connection ~ 3750 2600
Wire Wire Line
	3750 2500 3750 2600
Connection ~ 2800 2900
$Comp
L Device:D D2
U 1 1 62E83480
P 2800 2750
F 0 "D2" V 2754 2829 50  0000 L CNN
F 1 "D" V 2845 2829 50  0000 L CNN
F 2 "Diode_THT:D_T-1_P10.16mm_Horizontal" H 2800 2750 50  0001 C CNN
F 3 "~" H 2800 2750 50  0001 C CNN
	1    2800 2750
	0    1    1    0   
$EndComp
$Comp
L power:+12V #PWR08
U 1 1 62E83486
P 3750 2500
F 0 "#PWR08" H 3750 2350 50  0001 C CNN
F 1 "+12V" H 3765 2673 50  0000 C CNN
F 2 "" H 3750 2500 50  0001 C CNN
F 3 "" H 3750 2500 50  0001 C CNN
	1    3750 2500
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R2
U 1 1 62E8348C
P 1900 3150
F 0 "R2" V 1695 3150 50  0000 C CNN
F 1 "2k" V 1786 3150 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 1940 3140 50  0001 C CNN
F 3 "~" H 1900 3150 50  0001 C CNN
	1    1900 3150
	0    1    1    0   
$EndComp
$Comp
L Transistor_BJT:2SC1815 Q2
U 1 1 62E83492
P 2350 3150
F 0 "Q2" H 2540 3196 50  0000 L CNN
F 1 "2SC1815" H 2540 3105 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline_Wide" H 2550 3075 50  0001 L CIN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Toshiba%20PDFs/2SC1815.pdf" H 2350 3150 50  0001 L CNN
	1    2350 3150
	1    0    0    -1  
$EndComp
$Comp
L Mylib:G5LE-1A_SPST K2
U 1 1 62E83498
P 3300 3250
F 0 "K2" V 2733 3250 50  0000 C CNN
F 1 "G5LE-1" V 2824 3250 50  0000 C CNN
F 2 "Myfootprint:Relay_SPST_Omron-G5LE" H 3750 3200 50  0001 L CNN
F 3 "http://www.omron.com/ecb/products/pdf/en-g5le.pdf" H 3300 3250 50  0001 C CNN
	1    3300 3250
	0    1    1    0   
$EndComp
Wire Wire Line
	2200 4450 2100 4450
Wire Wire Line
	2500 4650 2500 4700
$Comp
L power:GND #PWR05
U 1 1 62E85127
P 2500 4700
F 0 "#PWR05" H 2500 4450 50  0001 C CNN
F 1 "GND" H 2505 4527 50  0000 C CNN
F 2 "" H 2500 4700 50  0001 C CNN
F 3 "" H 2500 4700 50  0001 C CNN
	1    2500 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 3900 3800 4350
Wire Wire Line
	2850 3900 3800 3900
Wire Wire Line
	2500 4200 2500 4250
Wire Wire Line
	2850 4200 2500 4200
Wire Wire Line
	2850 4350 2850 4200
Wire Wire Line
	3050 4350 2850 4350
Wire Wire Line
	3800 4350 3650 4350
Connection ~ 3800 3900
Wire Wire Line
	3800 3800 3800 3900
Connection ~ 2850 4200
$Comp
L Device:D D3
U 1 1 62E85137
P 2850 4050
F 0 "D3" V 2804 4129 50  0000 L CNN
F 1 "D" V 2895 4129 50  0000 L CNN
F 2 "Diode_THT:D_T-1_P10.16mm_Horizontal" H 2850 4050 50  0001 C CNN
F 3 "~" H 2850 4050 50  0001 C CNN
	1    2850 4050
	0    1    1    0   
$EndComp
$Comp
L power:+12V #PWR09
U 1 1 62E8513D
P 3800 3800
F 0 "#PWR09" H 3800 3650 50  0001 C CNN
F 1 "+12V" H 3815 3973 50  0000 C CNN
F 2 "" H 3800 3800 50  0001 C CNN
F 3 "" H 3800 3800 50  0001 C CNN
	1    3800 3800
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R3
U 1 1 62E85143
P 1950 4450
F 0 "R3" V 1745 4450 50  0000 C CNN
F 1 "2k" V 1836 4450 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 1990 4440 50  0001 C CNN
F 3 "~" H 1950 4450 50  0001 C CNN
	1    1950 4450
	0    1    1    0   
$EndComp
$Comp
L Transistor_BJT:2SC1815 Q3
U 1 1 62E85149
P 2400 4450
F 0 "Q3" H 2590 4496 50  0000 L CNN
F 1 "2SC1815" H 2590 4405 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline_Wide" H 2600 4375 50  0001 L CIN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Toshiba%20PDFs/2SC1815.pdf" H 2400 4450 50  0001 L CNN
	1    2400 4450
	1    0    0    -1  
$EndComp
$Comp
L Mylib:G5LE-1A_SPST K3
U 1 1 62E8514F
P 3350 4550
F 0 "K3" V 2783 4550 50  0000 C CNN
F 1 "G5LE-1" V 2874 4550 50  0000 C CNN
F 2 "Myfootprint:Relay_SPST_Omron-G5LE" H 3800 4500 50  0001 L CNN
F 3 "http://www.omron.com/ecb/products/pdf/en-g5le.pdf" H 3350 4550 50  0001 C CNN
	1    3350 4550
	0    1    1    0   
$EndComp
Wire Wire Line
	2100 5700 2000 5700
Wire Wire Line
	2400 5900 2400 5950
$Comp
L power:GND #PWR03
U 1 1 62E87CB8
P 2400 5950
F 0 "#PWR03" H 2400 5700 50  0001 C CNN
F 1 "GND" H 2405 5777 50  0000 C CNN
F 2 "" H 2400 5950 50  0001 C CNN
F 3 "" H 2400 5950 50  0001 C CNN
	1    2400 5950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3700 5150 3700 5600
Wire Wire Line
	2750 5150 3700 5150
Wire Wire Line
	2400 5450 2400 5500
Wire Wire Line
	2750 5450 2400 5450
Wire Wire Line
	2750 5600 2750 5450
Wire Wire Line
	2950 5600 2750 5600
Wire Wire Line
	3700 5600 3550 5600
Connection ~ 3700 5150
Wire Wire Line
	3700 5050 3700 5150
Connection ~ 2750 5450
$Comp
L Device:D D4
U 1 1 62E87CC8
P 2750 5300
F 0 "D4" V 2704 5379 50  0000 L CNN
F 1 "D" V 2795 5379 50  0000 L CNN
F 2 "Diode_THT:D_T-1_P10.16mm_Horizontal" H 2750 5300 50  0001 C CNN
F 3 "~" H 2750 5300 50  0001 C CNN
	1    2750 5300
	0    1    1    0   
$EndComp
$Comp
L power:+12V #PWR07
U 1 1 62E87CCE
P 3700 5050
F 0 "#PWR07" H 3700 4900 50  0001 C CNN
F 1 "+12V" H 3715 5223 50  0000 C CNN
F 2 "" H 3700 5050 50  0001 C CNN
F 3 "" H 3700 5050 50  0001 C CNN
	1    3700 5050
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R4
U 1 1 62E87CD4
P 1850 5700
F 0 "R4" V 1645 5700 50  0000 C CNN
F 1 "2k" V 1736 5700 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 1890 5690 50  0001 C CNN
F 3 "~" H 1850 5700 50  0001 C CNN
	1    1850 5700
	0    1    1    0   
$EndComp
$Comp
L Transistor_BJT:2SC1815 Q4
U 1 1 62E87CDA
P 2300 5700
F 0 "Q4" H 2490 5746 50  0000 L CNN
F 1 "2SC1815" H 2490 5655 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline_Wide" H 2500 5625 50  0001 L CIN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Toshiba%20PDFs/2SC1815.pdf" H 2300 5700 50  0001 L CNN
	1    2300 5700
	1    0    0    -1  
$EndComp
$Comp
L Mylib:G5LE-1A_SPST K4
U 1 1 62E87CE0
P 3250 5800
F 0 "K4" V 2683 5800 50  0000 C CNN
F 1 "G5LE-1" V 2774 5800 50  0000 C CNN
F 2 "Myfootprint:Relay_SPST_Omron-G5LE" H 3700 5750 50  0001 L CNN
F 3 "http://www.omron.com/ecb/products/pdf/en-g5le.pdf" H 3250 5800 50  0001 C CNN
	1    3250 5800
	0    1    1    0   
$EndComp
$Comp
L Mylib:G5LE-1A_SPST K5
U 1 1 62E8C862
P 5700 1850
F 0 "K5" V 5133 1850 50  0000 C CNN
F 1 "G5LE-1" V 5224 1850 50  0000 C CNN
F 2 "Myfootprint:Relay_SPST_Omron-G5LE" H 6150 1800 50  0001 L CNN
F 3 "http://www.omron.com/ecb/products/pdf/en-g5le.pdf" H 5700 1850 50  0001 C CNN
	1    5700 1850
	0    1    1    0   
$EndComp
Wire Wire Line
	2750 1650 2750 1850
Connection ~ 2750 1650
$Comp
L power:+12V #PWR010
U 1 1 62E905D8
P 5250 1300
F 0 "#PWR010" H 5250 1150 50  0001 C CNN
F 1 "+12V" H 5265 1473 50  0000 C CNN
F 2 "" H 5250 1300 50  0001 C CNN
F 3 "" H 5250 1300 50  0001 C CNN
	1    5250 1300
	1    0    0    -1  
$EndComp
$Comp
L Mylib:G5LE-1A_SPST K6
U 1 1 62E930D5
P 5700 3200
F 0 "K6" V 5133 3200 50  0000 C CNN
F 1 "G5LE-1" V 5224 3200 50  0000 C CNN
F 2 "Myfootprint:Relay_SPST_Omron-G5LE" H 6150 3150 50  0001 L CNN
F 3 "http://www.omron.com/ecb/products/pdf/en-g5le.pdf" H 5700 3200 50  0001 C CNN
	1    5700 3200
	0    1    1    0   
$EndComp
$Comp
L Mylib:G5LE-1A_SPST K7
U 1 1 62E95284
P 5700 4550
F 0 "K7" V 5133 4550 50  0000 C CNN
F 1 "G5LE-1" V 5224 4550 50  0000 C CNN
F 2 "Myfootprint:Relay_SPST_Omron-G5LE" H 6150 4500 50  0001 L CNN
F 3 "http://www.omron.com/ecb/products/pdf/en-g5le.pdf" H 5700 4550 50  0001 C CNN
	1    5700 4550
	0    1    1    0   
$EndComp
$Comp
L Mylib:G5LE-1A_SPST K8
U 1 1 62E9789A
P 5700 5900
F 0 "K8" V 5133 5900 50  0000 C CNN
F 1 "G5LE-1" V 5224 5900 50  0000 C CNN
F 2 "Myfootprint:Relay_SPST_Omron-G5LE" H 6150 5850 50  0001 L CNN
F 3 "http://www.omron.com/ecb/products/pdf/en-g5le.pdf" H 5700 5900 50  0001 C CNN
	1    5700 5900
	0    1    1    0   
$EndComp
Wire Wire Line
	2800 3050 2800 3200
Connection ~ 2800 3050
Wire Wire Line
	2850 4500 2850 4350
Connection ~ 2850 4350
Wire Wire Line
	2750 5600 2750 5750
Wire Wire Line
	2750 5750 4850 5750
Connection ~ 2750 5600
$Comp
L Connector:Conn_Coaxial J1
U 1 1 62EA0852
P 1050 3650
F 0 "J1" H 978 3888 50  0000 C CNN
F 1 "Conn_Coaxial" H 978 3797 50  0000 C CNN
F 2 "Connector_Coaxial:SMA_Amphenol_132289_EdgeMount" H 1050 3650 50  0001 C CNN
F 3 " ~" H 1050 3650 50  0001 C CNN
	1    1050 3650
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_Coaxial J2
U 1 1 62EA2169
P 7850 3600
F 0 "J2" H 7950 3575 50  0000 L CNN
F 1 "Conn_Coaxial" H 7950 3484 50  0000 L CNN
F 2 "Connector_Coaxial:SMA_Amphenol_132289_EdgeMount" H 7850 3600 50  0001 C CNN
F 3 " ~" H 7850 3600 50  0001 C CNN
	1    7850 3600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 62EA684D
P 1050 3900
F 0 "#PWR01" H 1050 3650 50  0001 C CNN
F 1 "GND" H 1055 3727 50  0000 C CNN
F 2 "" H 1050 3900 50  0001 C CNN
F 3 "" H 1050 3900 50  0001 C CNN
	1    1050 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 2350 2950 2050
Wire Wire Line
	1250 2350 1250 3650
Wire Wire Line
	1250 2350 2950 2350
Wire Wire Line
	1250 3650 3000 3650
Wire Wire Line
	3000 3650 3000 3450
Connection ~ 1250 3650
Wire Wire Line
	1250 5000 3000 5000
Wire Wire Line
	3000 5000 3000 4750
Wire Wire Line
	3000 4750 3050 4750
Wire Wire Line
	1250 6350 2950 6350
Wire Wire Line
	2950 6350 2950 6000
Wire Wire Line
	1250 3650 1250 5000
Connection ~ 1250 5000
Wire Wire Line
	1250 5000 1250 6350
Connection ~ 7650 3600
$Comp
L power:GND #PWR011
U 1 1 62EB845F
P 7850 3900
F 0 "#PWR011" H 7850 3650 50  0001 C CNN
F 1 "GND" H 7855 3727 50  0000 C CNN
F 2 "" H 7850 3900 50  0001 C CNN
F 3 "" H 7850 3900 50  0001 C CNN
	1    7850 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	7850 3900 7850 3800
Wire Wire Line
	1050 3900 1050 3850
$Comp
L Connector:Conn_01x06_Female J3
U 1 1 62EBFBE8
P 8550 1250
F 0 "J3" H 8578 1226 50  0000 L CNN
F 1 "Conn_01x06_Female" H 8578 1135 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 8550 1250 50  0001 C CNN
F 3 "~" H 8550 1250 50  0001 C CNN
	1    8550 1250
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR012
U 1 1 62EC1F12
P 7550 1550
F 0 "#PWR012" H 7550 1400 50  0001 C CNN
F 1 "+12V" H 7565 1723 50  0000 C CNN
F 2 "" H 7550 1550 50  0001 C CNN
F 3 "" H 7550 1550 50  0001 C CNN
	1    7550 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	8350 1150 8050 1150
Wire Wire Line
	8350 1250 8050 1250
Wire Wire Line
	8350 1350 8050 1350
Wire Wire Line
	8350 1450 8050 1450
$Comp
L power:GND #PWR013
U 1 1 62ECD8C3
P 7550 1050
F 0 "#PWR013" H 7550 800 50  0001 C CNN
F 1 "GND" H 7555 877 50  0000 C CNN
F 2 "" H 7550 1050 50  0001 C CNN
F 3 "" H 7550 1050 50  0001 C CNN
	1    7550 1050
	1    0    0    -1  
$EndComp
Text GLabel 8050 1250 0    50   Input ~ 0
sw0
Text GLabel 8050 1150 0    50   Input ~ 0
sw1
Text GLabel 8050 1450 0    50   Input ~ 0
sw2
Text GLabel 8050 1350 0    50   Input ~ 0
sw3
Text GLabel 1700 1750 0    50   Input ~ 0
sw0
Text GLabel 1700 3150 0    50   Input ~ 0
sw1
Text GLabel 1700 4450 0    50   Input ~ 0
sw2
Text GLabel 1600 5700 0    50   Input ~ 0
sw3
Wire Wire Line
	1700 5700 1600 5700
Wire Wire Line
	1800 4450 1700 4450
Wire Wire Line
	1750 3150 1700 3150
Wire Wire Line
	4850 5750 4850 5850
Wire Wire Line
	4850 5850 6000 5850
Wire Wire Line
	6000 5850 6000 5700
Wire Wire Line
	6000 4500 6000 4350
Wire Wire Line
	2850 4500 6000 4500
Wire Wire Line
	6000 3200 6000 3000
Wire Wire Line
	2800 3200 6000 3200
Wire Wire Line
	6000 1850 6000 1650
Wire Wire Line
	2750 1850 6000 1850
Wire Wire Line
	5250 1300 5250 1650
Wire Wire Line
	5250 5700 5400 5700
Wire Wire Line
	5400 4350 5250 4350
Connection ~ 5250 4350
Wire Wire Line
	5250 4350 5250 5700
Wire Wire Line
	5400 3000 5250 3000
Connection ~ 5250 3000
Wire Wire Line
	5250 3000 5250 4350
Wire Wire Line
	5400 1650 5250 1650
Connection ~ 5250 1650
Wire Wire Line
	5250 1650 5250 3000
Wire Wire Line
	7650 3600 7650 3850
Wire Wire Line
	5400 2050 5400 2400
Wire Wire Line
	5400 2400 7650 2400
Wire Wire Line
	7650 2400 7650 3600
Wire Wire Line
	5400 3400 5400 3850
Wire Wire Line
	5400 3850 7650 3850
Connection ~ 7650 3850
Wire Wire Line
	7650 3850 7650 5200
Wire Wire Line
	5400 4750 5400 5200
Wire Wire Line
	5400 5200 7650 5200
Connection ~ 7650 5200
Wire Wire Line
	5400 6100 5400 6500
Wire Wire Line
	5400 6500 7650 6500
Wire Wire Line
	7650 5200 7650 6500
Wire Wire Line
	8350 1050 7550 1050
Wire Wire Line
	8350 1550 7550 1550
Wire Wire Line
	3600 3550 3600 3350
Wire Wire Line
	3650 4650 3650 4850
Wire Wire Line
	3550 6100 3550 5900
Wire Wire Line
	6000 6000 6000 6200
Wire Wire Line
	6000 4850 6000 4650
Wire Wire Line
	6000 3300 6000 3500
Wire Wire Line
	6000 1950 6000 2150
Wire Wire Line
	3550 2150 3550 1950
$Comp
L Mylib:G5LE-1A_SPST K1
U 1 1 62E80542
P 3250 1850
F 0 "K1" V 2683 1850 50  0000 C CNN
F 1 "G5LE-1" V 2774 1850 50  0000 C CNN
F 2 "Myfootprint:Relay_SPST_Omron-G5LE" H 3700 1800 50  0001 L CNN
F 3 "http://www.omron.com/ecb/products/pdf/en-g5le.pdf" H 3250 1850 50  0001 C CNN
	1    3250 1850
	0    1    1    0   
$EndComp
$EndSCHEMATC