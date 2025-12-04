# ioLogik R1200 Series User’s Manual
Edition 3.4, April 2021
www.moxa.com/product


# ioLogik R1200 Series User’s Manual
The software described in this manual is furnished under a license agreement and may be used only in accordance with the terms of that agreement.
Copyright Notice
© 2021 Moxa Inc. All rights reserved.
Trademarks
The MOXA logo is a registered trademark of Moxa Inc.
All other trademarks or registered marks in this manual belong to their respective manufacturers.
# Disclaimer
Information in this document is subject to change without notice and does not represent a commitment on the part of Moxa.
Moxa provides this document as is, without warranty of any kind, either expressed or implied, including, but not limited to, its particular purpose. Moxa reserves the right to make improvements and/or changes to this manual, or to the products and/or the programs described in this manual, at any time.
Information provided in this manual is intended to be accurate and reliable. However, Moxa assumes no responsibility for its use, or for any infringements on the rights of third parties that may result from its use.
This product might include unintentional technical or typographical errors. Changes are periodically made to the information herein to correct such errors, and these changes are incorporated into new editions of the publication.
# Technical Support Contact Information
## www.moxa.com/support
## Moxa Americas
Moxa China(Shanghai office)
Toll-free: 1-888-669-2872Toll-free: 800-820-5036Tel: +1-714-528-6777Tel: +86-21-5258-9955Fax: +1-714-528-6778Fax: +86-21-5258-5505Moxa EuropeMoxa Asia-PacificTel: +49-89-3 70 03 99-0Tel: +886-2-8919-1230Fax: +49-89-3 70 03 99-99Fax: +886-2-8919-1231
## Moxa India
Tel: +91-80-4172-9088
Fax: +91-80-4132-1045
# Table of Contents
Overview……………………………………………………………………………………………………………………. 1-1
Introduction………………………………………………………………………………………………………………… 1-2
Product Features…………………………………………………………………………………………………………… 1-2
Package Checklist…………………………………………………………………………………………………………. 1-2
Product Model Information……………………………………………………………………………………………….. 1-2
Ordering Information……………………………………………………………………………………………….. 1-2
Specifications………………………………………………………………………………………………………………. 1-3
Common Specifications…………………………………………………………………………………………….. 1-3
ioLogik R1210……………………………………………………………………………………………………….. 1-4
ioLogik R1212……………………………………………………………………………………………………….. 1-4
ioLogik R1214……………………………………………………………………………………………………….. 1-5
ioLogik R1240……………………………………………………………………………………………………….. 1-6
ioLogik R1241……………………………………………………………………………………………………….. 1-6
Physical Dimensions………………………………………………………………………………………………………. 1-7
Hardware Reference………………………………………………………………………………………………………. 1-7
Panel Guide………………………………………………………………………………………………………….. 1-7
LED Indicators……………………………………………………………………………………………………….. 1-8
Switch Settings……………………………………………………………………………………………………… 1-8
DI Circuit Diagram………………………………………………………………………………………………….. 1-8
DIO Circuit Diagram………………………………………………………………………………………………… 1-9
Relay Circuit Diagram……………………………………………………………………………………………… 1-10
AI Circuit Diagram…………………………………………………………………………………………………. 1-10
Initial Setup………………………………………………………………………………………………………………… 2-1
Hardware Installation…………………………………………………………………………………………………….. 2-2
Connecting the Power………………………………………………………………………………………………. 2-2
Grounding the ioLogik R1200……………………………………………………………………………………… 2-2
Connecting to Serial Interface…………………………………………………………………………………….. 2-2
Mounting the ioLogik R1200……………………………………………………………………………………….. 2-2
Connecting to Digital Sensors and Devices……………………………………………………………………… 2-3
RS-485 Networks……………………………………………………………………………………………………. 2-4
Modbus/RTU Devices……………………………………………………………………………………………….. 2-5
Dual RS-485 or Repeater Settings………………………………………………………………………………… 2-5
Jumper Settings(DIO and AI)…………………………………………………………………………………….. 2-6
Pull High/Low DIP Switch Settings for the RS-485 Port……………………………………………………….. 2-6
Software Installation……………………………………………………………………………………………………… 2-7
ioSearch Installation………………………………………………………………………………………………… 2-7
Initial Setup by USB………………………………………………………………………………………………… 2-9
Restore Factory Default Settings…………………………………………………………………………………. 2-10
Using ioSearch…………………………………………………………………………………………………………….. 3-1
Introduction to ioSearch………………………………………………………………………………………………….. 3-2
ioSearch Main Screen…………………………………………………………………………………………………….. 3-2
Main Screen Overview……………………………………………………………………………………………………. 3-2
ioSearch Setup…………………………………………………………………………………………………………….. 3-3
System……………………………………………………………………………………………………………….. 3-3
Sort……………………………………………………………………………………………………………………. 3-5
Help…………………………………………………………………………………………………………………… 3-6
Quick Links…………………………………………………………………………………………………………………. 3-6
Main Functions…………………………………………………………………………………………………………….. 3-6
Locate…………………………………………………………………………………………………………………. 3-6
Connect/Disconnect…………………………………………………………………………………………………. 3-6
Firmware Upgrade…………………………………………………………………………………………………… 3-7
Import………………………………………………………………………………………………………………… 3-7
Export…………………………………………………………………………………………………………………. 3-7
Change Server Name……………………………………………………………………………………………….. 3-7
Restart System………………………………………………………………………………………………………. 3-8
Delete ioLogik Device………………………………………………………………………………………………. 3-8
Reset to Default……………………………………………………………………………………………………… 3-8
Main Screen………………………………………………………………………………………………………………… 3-9
I/O Configuration Tab(General)………………………………………………………………………………….. 3-9
Configuring Digital Input Channels……………………………………………………………………………….. 3-9
Configuring Digital Output Channels…………………………………………………………………………….. 3-11
Configuring Analog Input Channels……………………………………………………………………………… 3-13
AI Input Range……………………………………………………………………………………………………… 3-13
Configuring Analog Output Channels……………………………………………………………………………. 3-14
Server Info Tab…………………………………………………………………………………………………………… 3-16
Server Settings Tab(General)……………………………………………………………………………………. 3-17
Watchdog……………………………………………………………………………………………………………. 3-18
A. Modbus Mapping………………………………………………………………………………………………………….. A-1
ioLogik R1200 System Modbus Address and Register Map………………………………………………………….. A-2
ioLogik R1210 Modbus Address and Register Map……………………………………………………………………. A-3
ioLogik R1212 Modbus Address and Register Map……………………………………………………………………A-11
ioLogik R1214 Modbus Address and Register Map……………………………………………………………………A-24
ioLogik R1240 Modbus Address and Register Map……………………………………………………………………A-31
ioLogik R1241 Modbus Address and Register Map……………………………………………………………………A-37
B. Factory Defaults…………………………………………………………………………………………………………… B-1
Pinouts………………………………………………………………………………………………………………………. C-1
D. FCC Interference Statement……………………………………………………………………………………………. D-1
European Community(CE)……………………………………………………………………………………………… E-1

1
1.Overview
The following topics are covered in this chapter:
口 Introduction
口 Product Features
🟥 Package Checklist
🟥 Product Model Information
Ordering Information
### 口 Specifications
Common Specifications
ioLogik R1210
ioLogik R1212
ioLogik R1214
ioLogik R1240
🟥 ioLogik R1241
🟥 Physical Dimensions
### 🟥 Hardware Reference
Panel Guide
LED Indicators
Switch Settings
DI Circuit Diagram
DIO Circuit Diagram
Relay Circuit Diagram
AI Circuit Diagram

# Introduction
The ioLogik R1200 is an industrial grade, wide-temperature serial remote I/O device equipped with dual RS-485 ports that allow users to select between two RS-485 serial ports or switch to a built-in repeater. Applications such as factory automation, security and surveillance systems, and tunnel monitoring can use the RS-485 serial line to set up multi-drop device configurations through serial cables. Furthermore, a technician with no serial background can upload device configurations and firmware via USB at the field site without bringing a PC to the field site. The ioLogik R1200 lets you easily build an industrial grade, long distance communication system with standard PC hardware, and extends the communication distance by4,000 ft.(1,200 m).
# Product Features
• Upload and install device configurations and firmware via USB
• Multi-drop support for device configuration and firmware upgrade via RS-485
• Remote firmware updates via RS-485
• Dual RS-485 ports with built-in repeater
• Wide temperature(-40 to 85°C), 1 kV surge protection, and 3 kV I/O isolation between I/O channels, networks and power circuits
• Multi-functional I/O support for DI, event counter, DO, and pulse output
• Modbus/RTU support for control by SCADA software, including Wonderware InTouch and GE Intellution iFix32
• Monitoring and configuration via ioSearch Windows utility
• Hardware detection over RS-485 via ioSearch
# Package Checklist
The ioLogik R1200 is shipped with the following items:
• 1 ioLogik R1200 remote I/O product
• Quick Installation Guide(printed)
NOTE Contact your sales representative if any of the above items are missing or damaged.
# Product Model Information
## Ordering Information
ioLogik R1210 RS-485 remote I/O, 16 DIs,-10 to 75°C operating temperature.
ioLogik R1210-T RS-485 remote I/O, 16 DIs,-40 to 85°C operating temperature.
ioLogik R1212 RS-485 remote I/O, 8 DIs, 8 DIOs,-10 to 75°C operating temperature.
ioLogik R1212-T RS-485 remote I/O, 8 DIs, 8 DIOs,-40 to 85°C operating temperature.
ioLogik R1214 RS-485 remote I/O, 6 DIs, 6 Relays,-10 to 75°C operating temperature.
ioLogik R1214-T RS-485 remote I/O, 6 DIs, 6 Relays,-40 to 85°C operating temperature.
ioLogik R1240 RS-485 remote I/O, 8 AIs,-10 to 75°C operating temperature.
ioLogik R1240-T RS-485 remote I/O, 8 AIs,-40 to 85°C operating temperature.
ioLogik R1241 RS-485 remote I/O, 4 AOs,-10 to 75°C operating temperature.
ioLogik R1241-T RS-485 remote I/O, 4 AOs,-40 to 85°C operating temperature.
# Specifications
# Common Specifications
## Serial Communication
Interface: RS-485-2w: Data+, Data-, GND(5-contact terminal block) Serial Line Protection: 15 kV ESD for all signals, Level 2 surge, EN 61000-4-5(1 kV)
## Serial Communication Parameters
Parity: None, Even, Odd(default= None)
Data Bits: 8
Stop Bits: 1, 2(default= 1)
Baudrate: 1200 to 921.6 kbps(default= 9600)
Pull High/Low Resistor for RS-485: 1 kΩ, 150 kΩ
Protocols: Modbus RTU
## Physical Characteristics
Wiring: I/O cable max. 16 AWG
Dimensions: 27.8 x 124 x 84 mm(1.09 x 4.88 x 3.31 in)
## Environmental Limits
### Operating Temperature:
Standard Models:-10 to 75°C(14 to 167°F)
Wide Temp. Models:-40 to 85°C(-40 to 185°F)
Storage Temperature:-40 to 85°C(-40 to 185°F)
Ambient Relative Humidity: 5 to 95%(non-condensing)
### Standards and Certifications
Safety: UL 508
## EMI:
EN 55032, EN 61000-3-2, EN 61000-3-3, FCC Part 15 Subpart B Class A
### EMS:
EN 55024, IEC 61000-4, IEC 61000-6
Shock: IEC 60068-2-27
Freefall: IEC 60068-2-32
Vibration: IEC 60068-2-6
## Warranty
Warranty Period: 5 years(excluding the ioLogik R1214)
Details: See www.moxa.com/warranty

## ioLogik R1210
Inputs and Outputs
Digital Inputs: 16 channels
Isolation: 3K VDC or 2K Vrms
### Digital Input
Sensor Type: Wet Contact(NPN or PNP), Dry Contact
I/O Mode: DI or Event Counter
### Dry Contact:
• On: short to GND
• Off: open
Wet Contact(DI to COM):
• On: 10 to 30 VDC
• Off: 0 to 3 VDC
Common Type: 8 points per COM
Counter Frequency: 2.5 kHz, power off storage
Digital Filtering Time Interval: Software selectable
Power Requirements
Power Input: 24 VDC nominal, 12 to 48 VDC
Power Consumption: 154 mA@ 24VDC
## ioLogik R1212
### Inputs and Outputs
Digital Inputs: 8 channels
Configurable DIOs: 8 channels
Isolation: 3K VDC or 2K Vrms
### Digital Input
Sensor Type: Wet Contact(NPN or PNP), Dry Contact
I/O Mode: DI or Event Counter
Dry Contact:
• On: short to GND
• Off: open
Wet Contact(DI to COM):
• On: 10 to 30 VDC
• Off: 0 to 3 VDC
Common Type: 8 points per COM
Counter Frequency: 2.5 kHz, power off storage
Digital Filtering Time Interval: Software selectable
Digital Output
Type: Sink
I/O Mode: DO or Pulse Output
Pulse Output Frequency: 5 kHz
Over-voltage Protection: 45 VDC
Over-current Protection: 2.6 A(4 channels@ 650 mA)
Over-temperature Shutdown: 175°C(typical), 150°C(min.)
Current Rating: 200 mA per channel
Power Requirements
Power Input: 24 VDC nominal, 12 to 48 VDC
Power Consumption: 187 mA@ 24VDC
## ioLogik R1214
### Inputs and Outputs
Digital Inputs: 6 channels
Relay Outputs: 6 channels
Isolation: 3K VDC or 2K Vrms
Digital Input
Sensor Type: Wet Contact(NPN or PNP), Dry Contact
I/O Mode: DI or Event Counter
Dry Contact:
• On: short to GND
• Off: open
Wet Contact(DI to COM):
• On: 10 to 30 VDC
• Off: 0 to 3 VDC
Common Type: 6 points per COM
Counter Frequency: 2.5 kHz, power off storage
Digital Filtering Time Interval: Software selectable
Relay Output
Type: Form A(N.O.) power relay
Contact Current Rating:
• Resistive Load: 5 A@ 30 VDC, 250 VAC, 110 VAC
Breakdown Voltage: 500 VAC
Relay On/Off Time: 1500 ms(Max.)
Initial Insulation Resistance: 1000 M ohms(min.)@ 500 VDC
Mechanical Endurance: 5,000,000 operations
Electrical Endurance: 100,000 operations@ 5 A resistive load
Contact Resistance: 100 m ohms(max.)
Pulse Output: 0.3 Hz at rated load
Note: Ambient humidity must be non-condensing and remain between 5 and 95%. The relays of the ioLogik R1214 may malfunction when operating in high condensation environments below 0°C.
### Power Requirements
Power Input: 24 VDC nominal, 12 to 48 VDC
Power Consumption: 207 mA@ 24VDC

# ioLogik R1240
## Inputs and Outputs
Analog Inputs: 8 channels
Isolation: 3K VDC or 2K Vrms
## Analog Input
Type: Differential input
Resolution: 16 bits
I/O Mode: Voltage/ Current
Input Range: 0 to 10 VDC, 0 to 20 mA, 4 to 20 mA(burn-out mode)
Accuracy:
±0.1% FSR@ 25°C
±0.3% FSR@-10 and 60°C
±0.5% FSR@-40 and 75°C
Sampling Rate(all channels):
12 Hz
Input Impedance: 10M ohms(min.)
Built-in Resistor for Current Input: 120 ohms
Power Requirements
Power Input: 24 VDC nominal, 12 to 48 VDC
Power Consumption: 216 mA@ 24VDC
# ioLogik R1241
Inputs and Outputs
Analog Outputs: 4 channels
Isolation: 3K VDC or 2K Vrms
Analog Output
Resolution: 12 bits
Output Range: 0 to 10 VDC, 4 to 20 mA
Voltage Output: 10 mA(max.)
Accuracy:
±0.1% FSR@ 25°C
±0.3% FSR@-40 and 75°C
Load Resistor:
• Internal power: 400 ohms
• External 24V power: 1000 ohms
Power Requirements
Power Input: 24 VDC nominal, 12 to 48 VDC
Power Consumption: 343 mA@ 24VDC

# Physical Dimensions
The dimensions of the ioLogik R1200 product are 27.8 x 124 x 84 mm. The connector for the two RS-485ports is a 5-pin 3.81 terminal block(2 RS-485 ports with 1 ground pin). The power connector is on the top and the reset button is on the bottom of the product. There are also two dials for Board ID setup, and a 2-pin DIP switch for “Initial/Run” mode and “Dual RS-485/Repeater” mode setup.
Unit: mm (inch)

# Hardware Reference
## Panel Guide

## LED Indicators
# Switch Settings
The R1200 series provides Dual/Rep and Run/Initial switch settings for setting the communication mode.

Dual(Default) Dual RS-485 mode
Rep Repeater mode Run User defined communication parameters Initial(Default) Initial RS-485 communication parameters
## DI Circuit Diagram

## DIO Circuit Diagram


# Relay Circuit Diagram

# AI Circuit Diagram
2


The following topics are covered in this chapter:
### 口 Hardware Installation
Connecting the Power
Grounding the ioLogik R1200
Connecting to Serial Interface
Mounting the ioLogik R1200
🟥 Connecting to Digital Sensors and Devices
RS-485 Networks
🟥 Modbus/RTU Devices
Dual RS-485 or Repeater Settings
🟥 Jumper Settings(DIO and AI)
Pull High/Low DIP Switch Settings for the RS-485 Port
口 Software Installation
ioSearch Installation
🟥 Initial Setup by USB
Restore Factory Default Settings

# Hardware Installation
# Connecting the Power
Connect the 12 to 48 VDC power line to the ioLogik R1200’s terminal block on the top panel. If power is properly supplied, the Power LED will glow a solid amber color.



# Grounding the ioLogik R1200
The ioLogik R1200 is equipped with a grounding point on the terminal block located on the top panel. Connect the ground pin if an earth ground is available.
# Connecting to Serial Interface
TB1 and TB2(two RS-485 2-wire connectors)
-8-


NOTE TB1 and TB2 share the same ground.

# Mounting the ioLogik R1200
The ioLogik R1200 can be used with both DIN rail and wall mounting applications.

ioLogik R1200 Series
# Connecting to Digital Sensors and Devices
Digital Input/Output(Sink Type)

A Dry Contact is a contact that works without a power source.
A Wet Contact is a contact that must work with a power source.
Analog Input/Output
Voltage/

## Relay Output(Form A)

Power
2-3
NOTE A “load” in a circuit schematic is a component or portion of the circuit that consumes electric power. For the diagrams shown in this document, “load” refers to the devices or systems connected to the remote I/O unit.
# RS-485 Networks
RS-485 permits a balanced transmission line to be shared in a party line or multi-drop configuration. As many as 32 driver/receiver pairs can share a multi-drop connection on a single two-wire bus. The length of the network is limited to 4,000 ft between the first node and the last node. You can use RS-485 in two-wire or four-wire multi-drop network applications.
In an RS-485 four- wire network, one node must be a master node and all others slave nodes. The master does not require tri-state output.
# Setting the Device ID for RS-485 Serial Communication
The RS-485 port is used to communicate with other RS-485 devices or to link to another ioLogik RS-485 I/O server. The RS-485 port can run Modbus/RTU or I/O command sets. The device ID for each ioLogik R1200device can be set to any number from 01 to 99 by turning the two rotary dials on the back of the device. The x1 dial(shown on the left in the figure below) represents the ones place and the x10 dial(shown on the right in the figure below) represents the tens place. Settings such as the baud rate, parity check, data bits, and stop bit are configured by software.

## Serial Communication Parameters(Initial State)
Communication Parameters(Initial mode)
Parity None, Even, Odd(default= None)
Data Bits 8
Stop Bits 1, 2(default= 1)
Baudrate 1200 to 921.6 kbps(default= 9600)
# Serial Communication Initial Setup
During your initial setup, set the switch to “Initial” mode to configure your device, after configuration is done flip the switch back to “Run” mode.

# Modbus/RTU Devices
The RS-485 port runs Modbus/RTU and can connect to any Modbus device. You may use different methods to connect different combinations of ioLogik R12000 servers and other Modbus devices. Some examples are shown below:
## Connecting One Modbus/RTU Device

## Connecting Multiple Modbus/RTU Devices

## Dual RS-485 or Repeater Settings
Dual RS-485: Switching the dial to “Dual” will divide the RS-485 port into two separate RS-485 ports for
users to run dual RS-485 lines.

Repeater: Switching the dial to “Rep” will set the ioLogik R1200 to act as a repeater.
NOTE In Repeater mode, when signals pass through one machine, the latency will increase by 1 byte with a

# Jumper Settings(DIO and AI)
The models with DIO or AI channels require configuring the jumpers inside the cover. Remove the screw located on the back panel and open the cover to configure the jumpers.

DIO mode configuration is shown to the right(default: DO Mode).

Analog mode configuration is shown to the right(default: Voltage Mode).


ATTENTION个 Remove the screw on the back panel and open the cover to configure the jumpers.

## Pull High/Low DIP Switch Settings for the RS-485 Port
In some critical environments, you may need to add termination resistors to prevent the reflection of serial signals. When using termination resistors, it is important to set the pull high/low resistors correctly so that the electrical signal is not corrupted. Since there is no resistor value that works for every environment, DIP switches are used to set the pull high/low resistor valve for each RS-485 port.
DIP switches inside the cover of the ioLogik R1200 are used to set the pull high/low resistor values for each serial port.

To set the pull high/low resistors to 150 kΩ, make sure both of the assigned DIP switches are in the OFF position(default setting).
To set the pull high/low resistors to 1 kΩ, make sure both of the assigned DIP switches are in the ON position.

Pull High/Low DIP switch settings for the RS-485 Port
# Software Installation
## ioSearch Installation
ioSearch™ is a search utility that helps the user locate ioLogik R1200 devices on the local RS-485 network. The latest version can be downloaded from Moxa’s website.
Install from the website: To download the software from Moxa’s website, first click on the following link to access the website’s search utility: http://www.moxa.com/support/search.aspx?type=soft. When the webpage opens, enter the model name of your product in the search box. Click the model name and navigate to the product page, and then click on ioSearch, located in the box titled Software. Download and then unzip the file, and then run SETUP.EXE from that location. The installation program will guide you through the installation process and install the software. You can also install the MXIO DLL library separately.
Open ioSearch: After installation is complete, run ioSearch from Start → Program Files → MOXA →IO Server > Utility 🟥 ioSearch
Search for the server: On the menu bar, select System → Auto Search Remote I/O Server. In the dialog window, select “RS-232/485 I/O server” and click “Start Search.”

• If multiple ioLogik R1200 units are installed on the same network, remember to assign a unique device ID to each unit to avoid conflicts.
• If ioSearch is unable to find the ioLogik R1200 device, there may also be a problem with your COM port settings. Click “Port Settings” to view or modify the settings.
Monitor I/O status: Once the unit is found by ioSearch, you can view the status of all connected I/O devices on the ioSearch main screen.

Ready
0:00:02:24 下午02:56:4191
# Initial Setup by USB

NOTE The USB format should be FAT or FAT32. The NTFS format is not supported.

# Import Configuration File
To import configuration files via USB, follow these steps:
Create a new “config” folder on the USB drive
Use the ioSearch utility to export the configuration file from the device and place the file into the“config”folder created in step 1(above):
X：12YY.txt
(X represents the USB drive, and R12YY represents the model type)
# Upgrade Firmware
To upgrade the firmware by USB, follow these steps:
Create a new”fwr” folder on the USB drive.
Place the firmware file under the folder created in step1:
X：12YY.1kp
(X represents the USB drive, and R12YY represent the model type)
# Configure the Device
After you set up the USB drive, follow the steps below to configure your ioLogik R1200 device.
Power off the ioLogik R1200.
Set the DIP switch to “Initial” mode.

Plug the USB drive into the ioLogik R1200 device.
Power on the ioLogik R1200 and the USB drive will automatically install the system files.(Process: Rdy LED Red blinking 3s, then Green blinking,
Result: RdyLED Green: pass, Rdy Red: Failed)
Power off the ioLogik R1200 device.
Unplug the USB drive.
Set the DIP switch to “Run” mode.

# Restore Factory Default Settings
There are two ways to restore the ioLogik R1200 device to the factory default settings.
Hold the reset button for 5 seconds.
Right-click on the ioLogik unit you want to restore in the ioSearch utility and change “Reset” to“Default.”

The following topics are covered in this chapter:
口 Introduction to ioSearch
1 ioSearch Main Screen
🟥 Main Screen Overview
### 口 ioSearch Setup
System
Sort
Help
### 🟥 Quick Links
### 口 Main Functions
Locate
Connect/Disconnect
🟥 Firmware Upgrade
Import
Export
Change Server Name
Restart System
Delete ioLogik Device
Reset to Default
口 Main Screen
I/O Configuration Tab(General)
Configuring Digital Input Channels
Configuring Digital Output Channels
Configuring Analog Input Channels
AI Input Range
Configuring Analog Output Channels
## 口 Server Info Tab
Server Settings Tab(General)
Watchdog

# Introduction to ioSearch
The ioSearch utility is used for locating or searching for an Logik R1200 unit on the physical network. The following functions are supported by the ioSearch utility.
• Search for and locate ioLogik R1200 units
• Configure communication address
• Upgrade firmware for multiple ioLogik R1200 units
• Export configuration files from multiple ioLogik R1200 units
• Import a configuration file to multiple ioLogik R1200 units
• Reset to default
# ioSearch Main Screen
The main screen of the ioSearch utility defaults to the I/O Configuration tab, which displays an image of the ioLogik R1200 and the status of each I/O channel. The other tabs on the main screen take you to server and network settings, and additional functions are available when you log on as an administrator. Note that configuration options are not available until you log on as an administrator.
# Main Screen Overview
1

# ioSearch Setup
## System
Several functions are available from the System menu.
Auto Search Remote Ethernet I/O Servers will search for ioLogik servers on the network. When connecting for the first time or recovering from a network disconnection, you can use this command to find I/O servers that are on the network.


Steps to Search for an ioLogik R1200 device:
Select the “RS-232/485 Remote I/O” checkbox.
Select the “R1000 Series” tab.
Select the ioLogik R1200 model(s) for which you are searching.
Click and Select “Enable search range” and enter the unit ID number, which ranges from 1 to 99.
Click the Port Settings button to configure, set, or verify the serial port settings. Select specific COM ports under “Only Search”, or select “Search All System Com Ports” to configure COM port settings, the Modbus protocol, and timeout interval.

Auto Search T imeout Interval: The timeout interval sets the preferred waiting time for an ioLogik R1200 device to respond to a search. After exceeding the preset time limit, ioSearch will proceed to the next device on the network.
Connection Timeout Interval: The timeout interval sets the preferred waiting time for an ioLogik R1200 device to respond before it disconnects. After exceeding the preset time limit, ioSearch will disconnect the ioLogik R1200 device.
When you click Start Search, ioSearch will begin searching up to 99 IDs for your ioLogik unit. The timeout interval is for RS-485 communication and defaults to 2,000 ms. As soon as your screen looks like the figure below, click Stop. Otherwise, ioSeach will continue to search all 99 ports.

0:00:02:24 下千02:5641
Once the ioLogik R1200 device has been discovered by the ioSearch utility, you will be able to monitor the I/O status from the first tab of the ioSearch utility. You will also be able to configure each DI and DO channel from this tab after first logging in under the Management tab.

# ATTENTION个
If ioSearch is unable to find your ioLogik R1200 device, confirm that the device ID matches the correct ioLogik device. See the previous section for setting or viewing the device ID.
Auto Search T imeout allows users to set the timeout value for TCP socket communication, but only for Ethernet remote I/O devices.

I/O Status Refresh Rate is used to adjust how often the I/O server is polled for device status. The current rate is displayed on the status bar at the bottom of the window. Note that higher sync rates result in higher loads on the network.

# Sort
The Sort menu allows the server list in the navigation panel to be sorted by ioLogik connection and server model.

# Help
In the Help menu, you can view vendor and version information.
# Quick Links
Quick links are provided to search for I/O servers on the RS-485 network and sort the server list.

Automatically searches the local network
Sorts by ioLogik R1200 device ID(“By Connection”)
Sorts by ioLogik R1200 model number(“By I/O Server”)
# Main Functions
Right click on a particular ioLogik R1200 device to view the ioSearch function menu.
## 白伞COM1
E…..四

## Locate
The locate function helps users find a specific ioLogik on the network. When this function is triggered, the ready LED on the selected unit will start to blink, indicating its location.
Locate Server
NOTE: The device LED will blink until the stop button is pressed. Stop
# Connect/Disconnect
The Connect/Disconnect function connects or disconnects the ioLogik device from the ioSearch network.
# Firmware Upgrade
The ioLogik R1200 supports a remote firmware upgrade function. Enter the path of the firmware file or click on the icon to browse for the file. The wizard will lead you through the process until the server restarts.
# Import
Select this command to reload a configuration that was exported to a text file. You will need to restart the ioLogik in order for the new configuration to take effect. This command may be used to restore a configuration after loading the factory defaults, or to duplicate a configuration to multiple ioLogik units.

# Export
Select this command to export the configuration of the ioLogik to a text file. You will need to log in as an administrator to use this function. It is strongly recommended you use this method to back up your configuration after you have finished configuring the ioLogik for your application.
# Change Server Name
The Change Server name function can be used to directly modify the Server Name, especially for first time installation.
Changing the Server Name for multiple ioLogik R1200 devices is allowed. Select the ioLogik R1200 and then right click to process this function. After entering the desired server name, click Submit to apply changes.


# Restart System
Select this command to restart the selected ioLogik R1200 device.
Restart multiple ioLogik R1200 units by right-clicking on an ioLogik R1200 and selecting this function.

# Delete ioLogik Device
Select this function to remove an ioLogik R1200 unit from the tree manually.
## Reset to Default
Select this function to reset all settings, including console passwords, to factory default values.
Reset multiple ioLogik R1200 units to default configurations by right-clicking the ioLogik R1200 and selecting this function.


# Main Screen
## I/O Configuration Tab(General)
The I/O Configuration tab shows the status of every I/O channel. This is the default tab when you first open ioSearch.

## Configuring Digital Input Channels

The ioLogik R1200’s digital channel can be separately set to “DI” or “Event Counter Mode.” In “DI” mode, the specifications are as follows:
In “Event Counter” mode, the ioLogik R1200’s DI channel accepts data from limit or proximity switches, and counts events according to the ON/OFF status. You may select from two modes, “Lo to Hi” or “Hi to Lo.”When “Lo to Hi” is selected, the counter value increases while the switch is pushed. When “Hi to Lo” is selected, the counter value increases when the switch is pushed and released. The sampling rate of the counter is 2.5 kHz. This function is designed for low speed switching, not for motor control.

To eliminate the problem of switch bouncing, the ioLogik R1200 provides software filtering that is configurable in multiples of 0.2 ms. For example, a setting of 100 would mean a 20 ms filter(100 × 0.2ms). The maximum value allowed by the software filter is 65535. Setting the filter to “0” causes the system to filter all signals.
Power On Settings: You may configure DI channels in Event Counter mode whether or not counting begins when powering up.
Safe Status Settings: For DI channels in Event Counter mode, you can configure whether or not counting starts or continues when Safe Status has been activated. When the network connection is lost as specified in the Host Connection Watchdog, the ioLogik R1200 will start or stop the counter according to the channel’s Safe Status settings. Note that the Host Connection Watchdog is disabled by default, and must be enabled for Safe Status settings to take effect.
Test I/O: You can test DI channels in the Test tab to see how the status or counter value responds when the attached input device is manipulated.

## Configuring Digital Output Channels

Each ioLogik R1200 digital output channel can be set to “DO” or “Pulse Output” mode. In DO mode, the specifications are as follows.

In “Pulse Output” mode, the selected digital output channel will generate a square wave as specified in the pulse mode parameters. The Low and High parameters are in multiples of 0.1 ms, with a maximum setting of 65535. To set the low level width for 5 ms, you would enter 50(because 50 × 0.1 ms= 5 ms). A setting of 100 for both Low and High would generate a square wave with a 20 ms cycle. The Output parameter specifies the number of pulses to send. When set to 0, the system will send pulses continuously.


Power On Settings: Use this field to set the initial status for the DO channel when the ioLogik is powered on.
Safe Status Settings: Use this field to specify how the DO channel responds to a break in network communication. When the network connection is lost as specified in the Host Connection Watchdog, the ioLogik R1200 will reset all channels according to their Safe Status settings. Note that the Host Connection Watchdog is disabled by default, and must be enabled for Safe Status settings to have effect.
Test I/O: You can test the DO channel in the Test tab.

# Configuring Analog Input Channels
The current status of each AI(analog input) channel can be viewed on the I/O Setting:There are two modes for the AI channels:
AI Input: Voltage Mode(V)(See Chapter 2, Jumper Settings(DIO and AI), for more information)

AI Input: Current Mode(mA)(See Chapter 2, Jumper Settings(DIO and AI), for more
information)

# AI Input Range
Set the AI input ranges for each mode, as follows:
AI Input: Voltage Mode(V)(See Jumper Settings(DIO and AI) in Chapter 2 for more information) There is only one default analog “voltage” input range:[0-10V]
AI Input: Current Mode(mA)(See Jumper Settings(DIO and AI) in Chapter 2 for more information)
There are two modes in the analog “current” input range:[0-20 mA],[4-20 mA(burnout)] Burnout mode indicates if the current analog input has burned out. For example, the 4–20 mA burnout mode is defined in the following diagram:

Users can define burnout values(BO, default 2 mA) for selected ranges. When input values are in the burnout range, raw data will register as 0000h to indicate analog input burnout. The definition of raw data is as follows:

# ATTENTION个
When configuring the jumpers to select voltage or current measurement for the AI channels, open the cover by first removing the screw on the back panel. Details on jumper settings can be found in the Jumper Settings(DIO and AI) section.

# Configuring Analog Output Channels
The current status of each AO(analog output) channel can be viewed on the I/O Setting:

Click on a specific channel to access the AO channel settings.

There are two modes for the AI channels, Voltage Mode(V) and Current Mode(mA). See Jumper Settings(DIO and AI) in Chapter 2 for more information.

Power On Settings: For AO channels in Event Counter mode, you may configure whether or not counting begins at power up.
Safe Status Settings: For AO channels in Event Counter mode, you can configure whether or not counting starts or continues when Safe Status has been activated. When the network connection is lost as specified in the Host Connection Watchdog, the ioLogik R1200 will start or stop the counter according to the channel’s Safe Status settings. Note that the Host Connection Watchdog is disabled by default, and must be enabled for Safe Status settings to have effect.

Test I/O: You can test AO channels in the Test tab. You may see how the status or counter value responds when the attached input device is manipulated.

# Server Info Tab
The Server Information tab provides the Modbus addresses for all system configurations. This helps you verify the access authority of each address. The screen also displays a clear explanation of each item.

# Server Settings Tab(General)
The Server Settings tab is where you log in as an administrator. This is required in order to gain access to the ioLogik R1200 configuration options. If no administrator password has been set up, simply click on Login and leave the Password for entry field blank. Additional information on ioSearch administrator functions is provided later in this chapter.

Click “Serial Port1 Setting” or “Serial Port2 Setting” to define and set the serial communication ports.

## Watchdog
The Watchdog tab is where you configure the Host Connection Watchdog, which is used with the Safe Status settings to define each channel’s response to a lost connection. When the ioLogik R1200 loses its connection as specified in the timeout, the Host Connection Watchdog will switch the ioLogik R1200 to Safe Status and all channels will reset to their Safe Status settings. By default, the Watchdog is disabled. To enable the Watchdog, make sure Enable Host Connection Watchdog is checked, set the Timeout value, then click the Update button.

After the Watchdog is enabled, the ioLogik R2110 will enter safe status if the RS-485 connection is lost. Once the connection has been restored, you will need to return to the Watchdog tab in order to exit Safe Status.
There will be a message saying “Host Connection Lost” to indicate that the server is in safe status. Click Clear Alarm to exit safe status and return to normal operation.


The following topics are covered in this appendix:
🟥 ioLogik R1200 System Modbus Address and Register Map
口 ioLogik R1210 Modbus Address and Register Map
🟥 ioLogik R1212 Modbus Address and Register Map
1 ioLogik R1214 Modbus Address and Register Map
口 ioLogik R1240 Modbus Address and Register Map
口 ioLogik R1241 Modbus Address and Register Map

# ioLogik R1200 System Modbus Address and Register Map

# ioLogik R1210 Modbus Address and Register Map








# ioLogik R1212 Modbus Address and Register Map













# ioLogik R1214 Modbus Address and Register Map







# ioLogik R1240 Modbus Address and Register Map



ioLogik R1200 Series
Address Register
R
R
31000 R
40705 R/W word
0x02C1 40706 R/W word
0x02C2 40707 R/W word
0x02C3 40708 R/W word
A-35
# ioLogik R1241 Modbus Address and Register Map

The ioLogik R1200 series comes configured with the following factory default settings:

C.Pinouts



D.FCC Interference Statement
## Federal Communication Commission Warning
This equipment has been tested and found to comply with the limits for a Class A digital device, pursuant to part 15 of the FCC Rules. Operation is subject to the following two conditions:(1) This device may not cause harmful interference, and(2) this device must accept any interference received, including interference that may cause undesired operation.
These limits are designed to provide reasonable protection against harmful interference when the equipment is operated in a commercial environment. This equipment generates, uses, and can radiate radio frequency energy and, if not installed and used in accordance with the instruction manual, may cause harmful interference to radio communications. Operation of this equipment in a residential area is likely to cause harmful interference in which case the user will be required to correct the interference at his own expense.

E.European Community(CE)
This is a Class A product. In a domestic environment, this product may cause radio interference in which case the user may be required to take adequate measures.
（注：本文档可能包含通义AI生产内容）

<!-- Table 1 -->

NOTE The RESET button restarts the server and resets all settings to factory defaults. Use a pointed object such as a straightened paper clip to hold down the reset button for 5 seconds. The factory defaults will load once the READY LED turns green again. You may then release the RESET button.


<!-- Table 2 -->

| LED | State | Description |
| --- | --- | --- |
| Power(PWR) | Amber | System power is ON |
| Power(PWR) | OFF | System power is OFF |
| Read(RDY) | Green | System is ready |
| Read(RDY) | Flashing | Flashes every 1 sec when the Locate function is triggered |
| Read(RDY) | Flashing | Flashes every 0.5 sec when the firmware is being upgraded |
| Read(RDY) | Flashing | Flashing USB upgrade is triggered |
| Read(RDY) | OFF | System is not ready. |
| Port 1(P1) | Green | Serial connection enabled |
| Port 1(P1) | Flashing | Transmitting or receiving data |
| Port 2(P2) | Green | Serial connection enabled |
| Port 2(P2) | Flashing | Transmitting or receiving data |


<!-- Table 3 -->

ATTENTION
个 Determine the maximum possible current for each power wire and common wire. Observe all electrical
codes dictating the maximum current allowable for each wire size. If the current exceeds the maximum
rating, the wiring could overheat, causing serious damage to your equipment. For safety reasons, we
recommend an average cable size of 22 AWG. However, depending on the current load, you may want to
adjust your cable size(the maximum wire size for power connectors is 2 mm).


<!-- Table 4 -->

| TB1(RS-485) | TB1(RS-485) | TB1(RS-485) | TB2(RS-485) | TB2(RS-485) | TB2(RS-485) |  |
| --- | --- | --- | --- | --- | --- | --- |
| Pin | 1 | 2 | 3 | 4 | 5 | 3 |
| Signal | D1+ | D1- | GND | D2+ | D2- | GND |


<!-- Table 5 -->

NOTE The transmission line is terminated on both ends of the line but not at drop points in the middle of the line.
Termination is only required with high data rates or long wire runs.


<!-- Table 6 -->

NOTE The initial communication setting is: baudrate= 9600, n, 8, 1.


<!-- Table 7 -->

| Switch | 1 | 2 | 3 |
| --- | --- | --- | --- |
| Switch | Pull High/Low | Pull High/Low | Terminator |
| ON | 1 kΩ | 1 kΩ | 120 Ω |
| OFF(default) | 150 kΩ | 150 kΩ |  |


<!-- Table 8 -->

When setting up your ioLogik R1200 for the first time, you need to import the initial configuration and firmware files onto a USB drive. But before you connect the USB drive to the ioLogik R1200’s USB port to install and upgrade configurations and firmware, you first need to place the configuration files under a designated folder.


<!-- Table 9 -->

| ioSearch Main Screen | ioSearch Main Screen |
| --- | --- |
| 1 | Title |
| 2 | Menu bar |
| 3 | Navigation panel |
| 4 | Quick link |
| 5 | Main window |
| 6 | Status Bar |


<!-- Table 10 -->

|  |  | Server Name | Model | IP AddressPort | MACAddress/Unit ID | FirmwareVer. | Status |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 1 |  | R1210 | COM1 | 1 | 1.0 | Unlock |  |
|  |  | R1210 | COM1 |  |  |  |  |


<!-- Table 11 -->

| Type | Logic 0(OFF) | Logic 1(ON) |
| --- | --- | --- |
| Dry contact | Open | Close to GND |
| Wet contact(DI to COM) | 0 to3 V | 10 to 30 V |


<!-- Table 12 -->

| Type | Logic 0(OFF) | Logic 1(ON) |
| --- | --- | --- |
| DO mode | Open | Short |


<!-- Table 13 -->

| Burnout Value(BO) | 0.0< BO< 4.0 | User defined(default 2 mA) |
| --- | --- | --- |
| Burnout State | 0 ≤ AI< BO mA | S/W output 0000h |
| Under Range | BO ≤ AI< 4 mA | S/W output raw data |
| Normal Range | 4 ≤AI ≤ 20.00 mA | S/W output raw data until FFFEh |
| Over Range | XX> 20.00 mA | S/W output FFFFh |


<!-- Table 14 -->

| Channe | Range | Yalue |
| --- | --- | --- |
|  | U-1UN | 0.000￥ |
|  | 0-10V | 0.000￥ |
|  | U-1UY | 0.000￥ |
|  | 0-10V | 0.000V |


<!-- Table 15 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x7530 | 330001 | R | word | SYS_modelID | 04:INPUTREGISTER |  |
| 0x7545 | 330022 | R | word | SYS_firmwareVersion | 04:INPUTREGISTER | 2 words |
| 0x7547 | 330024 | R | word | SYS_firmwareReleaseDate | 04:INPUTREGISTER | 2 words |
| 0x7549 | 330026 | R | word | SYS_vendorID | 04:INPUTREGISTER |  |
| 0x754A | 330027 | R | word | SYS_unitID | 04:INPUTREGISTER |  |
| 0x754B | 330028 | R | word | SYS_serialNumber | 04:INPUTREGISTER | 6 words |
| 0x7555 | 330038 | R | word | SYS_productName | 04:INPUTREGISTER | 10 words |
| 0x756A | 330059 | R | word | SYS_rs485PortMode | 04:INPUTREGISTER | 0: initial-repeater,1: initial-dual RS-485, 2: run-repeater, 3: run-dual RS-485 |
| 0x7531 | 430002 | R/W | word | SYS_serverName | 03:HOLDING REGISTER | 10 words |
| 0x753B | 430012 | R/W | word | SYS_serverLocation | 03:HOLDIN G REGISTER | 10 words |
| 0x755F | 430048 | R/W | word | SYS_password | 03:HOLDING REGISTER | 5 words |
| 0x7564 | 430053 | R/W | word | SYS_modbusWatchdogFuntion | 03:HOLDING REGISTER | 0: Disable, 1: Enable |
| 0x7565 | 430054 | R/W | word | SYS_modbusWatchdogTimeout | 03:HOLDING REGISTER | unit: sec(s) |
| 0x7566 | 430055 | R/W | word | SYS_modbusWatchdogStatus | 03:HOLDING REGISTER | 0: Normal, 1: Timeout |
| 0x7567 | 430056 | R/W | word | SYS_locateDevice | 03:HOLDING REGISTER |  |
| 0x7568 | 430057 | R/W | word | SYS_restartDevice | 03:HOLDING REGISTER | 404: Enable |
| 0x7569 | 430058 | R/W | word | SYS_loadFactoryDefault | 03:HOLDING REGISTER | 404: Enable |
| 0x756C | 430061 | R/W | word | SYS_port0Baudrate | 03:HOLDING REGISTER | 1: 1200, 2: 2400, 3: 4800, 4: 9600, 5: 19200, 6: 38400, 7: 57600, 8: 115200,9: 921600 |
| 0x756D | 430062 | R/W | word | SYS_port1Baudrate | 03:HOLDING REGISTER | 1: 1200, 2: 2400, 3: 4800, 4: 9600,5: 19200, 6: 38400,7: 57600, 8: 115200, 9: 921600 |


<!-- Table 16 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x7570 | 430065 | R/W | word | SYS_port0StopBit | 03:HOLDING REGISTER | 0: 1, 1: 2 |
| 0x7571 | 430066 | R/W | word | SYS_port1StopBit | 03:HOLDING REGISTER | 0: 1, 1: 2 |
| 0x7572 | 430067 | R/W | word | SYS_port0Parity | 03:HOLDING REGISTER | 0: None, 1: Even, 2: Odd |
| 0x7573 | 430068 | R/W | word | SYS_port1Parity | 03:HOLDING REGISTER | 0: None, 1: Even,2: Odd |
| 0x7594 | 430101 | R/W | word | SYS_vendorName | 03:HOLDING REGISTER | 20 words |


<!-- Table 17 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x0000 | 00001 | R/W | bit | DI-00_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0001 | 00002 | R/W | bit | DI-01_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0002 | 00003 | R/W | bit | DI-02_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0003 | 00004 | R/W | bit | DI-03_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0004 | 00005 | R/W | bit | DI-04_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0005 | 00006 | R/W | bit | DI-05_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0006 | 00007 | R/W | bit | DI-06_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0007 | 00008 | R/W | bit | DI-07_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0008 | 00009 | R/W | bit | DI-08_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0009 | 00010 | R/W | bit | DI-09_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x000A | 00011 | R/W | bit | DI-10_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x000B | 00012 | R/W | bit | DI-11_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x000C | 00013 | R/W | bit | DI-12_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x000D | 00014 | R/W | bit | DI-13_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x000E | 00015 | R/W | bit | DI-14_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x000F | 00016 | R/W | bit | DI-15_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0020 | 00033 | R/W | bit | DI-00_counterReset | 01:COILSTATUS | 1: reset to initialvalue |


<!-- Table 18 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x0021 | 00034 | R/W | bit | DI-01_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0022 | 00035 | R/W | bit | DI-02_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0023 | 00036 | R/W | bit | DI-03_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0024 | 00037 | R/W | bit | DI-04_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0025 | 00038 | R/W | bit | DI-05_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0026 | 00039 | R/W | bit | DI-06_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0027 | 00040 | R/W | bit | DI-07_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0028 | 00041 | R/W | bit | DI-08_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0029 | 00042 | R/W | bit | DI-09_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x002A | 00043 | R/W | bit | DI-10_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x002B | 00044 | R/W | bit | DI-11_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x002C | 00045 | R/W | bit | DI-12_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x002D | 00046 | R/W | bit | DI-13_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x002E | 00047 | R/W | bit | DI-14_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x002F | 00048 | R/W | bit | DI-15_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0040 | 00065 | R/W | bit | DI-00_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0041 | 00066 | R/W | bit | DI-01_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0042 | 00067 | R/W | bit | DI-02_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0043 | 00068 | R/W | bit | DI-03_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0044 | 00069 | R/W | bit | DI-04_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0045 | 00070 | R/W | bit | DI-05_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0046 | 00071 | R/W | bit | DI-06_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0047 | 00072 | R/W | bit | DI-07_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0048 | 00073 | R/W | bit | DI-08_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0049 | 00074 | R/W | bit | DI-09_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x004A | 00075 | R/W | bit | DI-10_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |


<!-- Table 19 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x004B | 00076 | R/W | bit | DI-11_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x004C | 00077 | R/W | bit | DI-12_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x004D | 00078 | R/W | bit | DI-13_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x004E | 00079 | R/W | bit | DI-14_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x004F | 00080 | R/W | bit | DI-15_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0060 | 00097 | R/W | bit | DI-00_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0061 | 00098 | R/W | bit | DI-01_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0062 | 00099 | R/W | bit | DI-02_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0063 | 00100 | R/W | bit | DI-03_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0064 | 00101 | R/W | bit | DI-04_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0065 | 00102 | R/W | bit | DI-05_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0066 | 00103 | R/W | bit | DI-06_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0067 | 00104 | R/W | bit | DI-07_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0068 | 00105 | R/W | bit | DI-08_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0069 | 00106 | R/W | bit | DI-09_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x006A | 00107 | R/W | bit | DI-10_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x006B | 00108 | R/W | bit | DI-11_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x006C | 00109 | R/W | bit | DI-12_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x006D | 00110 | R/W | bit | DI-13_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x006E | 00111 | R/W | bit | DI-14_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x006F | 00112 | R/W | bit | DI-15_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0080 | 00129 | R/W | bit | DI-00_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0081 | 00130 | R/W | bit | DI-01_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0082 | 00131 | R/W | bit | DI-02_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0083 | 00132 | R/W | bit | DI-03_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0084 | 00133 | R/W | bit | DI-04_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |


<!-- Table 20 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x0085 | 00134 | R/W | bit | DI-05_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0086 | 00135 | R/W | bit | DI-06_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0087 | 00136 | R/W | bit | DI-07_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0088 | 00137 | R/W | bit | DI-08_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0089 | 00138 | R/W | bit | DI-09_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x008A | 00139 | R/W | bit | DI-10_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x008B | 00140 | R/W | bit | DI-11_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x008C | 00141 | R/W | bit | DI-12_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x008D | 00142 | R/W | bit | DI-13_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x008E | 00143 | R/W | bit | DI-14_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x008F | 00144 | R/W | bit | DI-15_counterSafeModeStatus | 01:COIL STATUS | 0: STOP, 1: START |
| 0x00A0 | 00161 | R/W | bit | DI-00_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00A1 | 00162 | R/W | bit | DI-01_counterPowerOffStorage | 01:COIL STATUS | 0: Disable, 1:Enable |
| 0x00A2 | 00163 | R/W | bit | DI-02_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00A3 | 00164 | R/W | bit | DI-03_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00A4 | 00165 | R/W | bit | DI-04_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00A5 | 00166 | R/W | bit | DI-05_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00A6 | 00167 | R/W | bit | DI-06_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00A7 | 00168 | R/W | bit | DI-07_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00A8 | 00169 | R/W | bit | DI-08_counterPowerOffStorage | 01:COIL | 0: Disable, 1:Enable |
| 0x00A8 | 00169 | R/W | bit | DI-08_counterPowerOffStorage | STATUS | 0: Disable, 1:Enable |
| 0x00A9 | 00170 | R/W | bit | DI-09_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1: Enable |
| 0x00AA | 00171 | R/W | bit | DI-10_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00AB | 00172 | R/W | bit | DI-11_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00AC | 00173 | R/W | bit | DI-12_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00AD | 00174 | R/W | bit | DI-13_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1: Enable |
| 0x00AE | 00175 | R/W | bit | DI-14_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1: Enable |


<!-- Table 21 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x00AF | 00176 | R/W | bit | DI-15_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x0000 | 10001 | R | bit | DI-00_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0001 | 10002 | R | bit | DI-01_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0002 | 10003 | R | bit | DI-02_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0003 | 10004 | R | bit | DI-03_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0004 | 10005 | R | bit | DI-04_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0005 | 10006 | R | bit | DI-05_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0006 | 10007 | R | bit | DI-06_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0007 | 10008 | R | bit | DI-07_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0008 | 10009 | R | bit | DI-08_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0009 | 10010 | R | bit | DI-09_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x000A | 10011 | R | bit | DI-10_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x000B | 10012 | R | bit | DI-11_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x000C | 10013 | R | bit | DI-12_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x000D | 10014 | R | bit | DI-13_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x000E | 10015 | R | bit | DI-14_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x000F | 10016 | R | bit | DI-15_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0000 | 30001 | R | word | DI-00_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0001 | 30002 | R | word | DI-01_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0002 | 30003 | R | word | DI-02_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0003 | 30004 | R | word | DI-03_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0004 | 30005 | R | word | DI-04_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0005 | 30006 | R | word | DI-05_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0006 | 30007 | R | word | DI-06_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0007 | 30008 | R | word | DI-07_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0008 | 30009 | R | word | DI-08_status | 04:INPUTREGISTER | 0: OFF, 1: ON |


<!-- Table 22 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x0009 | 30010 | R | word | DI-09_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x000A | 30011 | R | word | DI-10_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x000B | 30012 | R | word | DI-11_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x000C | 30013 | R | word | DI-12_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x000D | 30014 | R | word | DI-13_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x000E | 30015 | R | word | DI-14_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x000F | 30016 | R | word | DI-15_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0020 | 30033 | R | word | DI-00_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0021 | 30034 | R | word | DI-00_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0022 | 30035 | R | word | DI-01_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0023 | 30036 | R | word | DI-01_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0024 | 30037 | R | word | DI-02_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0025 | 30038 | R | word | DI-02_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0026 | 30039 | R | word | DI-03_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0027 | 30040 | R | word | DI-03_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0028 | 30041 | R | word | DI-04_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0029 | 30042 | R | word | DI-04_counterValueLow | 04:INPUTREGISTER | low word |
| 0x002A | 30043 | R | word | DI-05_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x002B | 30044 | R | word | DI-05_counterValueLow | 04:INPUTREGISTER | low word |
| 0x002C | 30045 | R | word | DI-06_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x002D | 30046 | R | word | DI-06_counterValueLow | 04:INPUTREGISTER | low word |
| 0x002E | 30047 | R | word | DI-07_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x002F | 30048 | R | word | DI-07_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0030 | 30049 | R | word | DI-08_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0031 | 30050 | R | word | DI-08_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0032 | 30051 | R | word | DI-09_counterValueHigh | 04:INPUTREGISTER | high word |


<!-- Table 23 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x0033 | 30052 | R | word | DI-09_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0034 | 30053 | R | word | DI-10_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0035 | 30054 | R | word | DI-10_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0036 | 30055 | R | word | DI-11_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0037 | 30056 | R | word | DI-11_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0038 | 30057 | R | word | DI-12_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0039 | 30058 | R | word | DI-12_counterValueLow | 04:INPUTREGISTER | low word |
| 0x003A | 30059 | R | word | DI-13_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x003B | 30060 | R | word | DI-13_counterValueLow | 04:INPUTREGISTER | low word |
| 0x003C | 30061 | R | word | DI-14_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x003D | 30062 | R | word | DI-14_counterValueLow | 04:INPUTREGISTER | low word |
| 0x003E | 30063 | R | word | DI-15_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x003F | 30064 | R | word | DI-15_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0000 | 40001 | R/W | word | DI-00_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x0001 | 40002 | R/W | word | DI-01_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x0002 | 40003 | R/W | word | DI-02_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x0003 | 40004 | R/W | word | DI-03_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x0004 | 40005 | R/W | word | DI-04_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x0005 | 40006 | R/W | word | DI-05_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x0006 | 40007 | R/W | word | DI-06_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x0007 | 40008 | R/W | word | DI-07_mode | 03:HOLDIN G REGISTER | 0: DI, 1: Counter |
| 0x0008 | 40009 | R/W | word | DI-08_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x0009 | 40010 | R/W | word | DI-09_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x000A | 40011 | R/W | word | DI-10_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x000B | 40012 | R/W | word | DI-11_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x000C | 40013 | R/W | word | DI-12_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |


<!-- Table 24 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x000D | 40014 | R/W | word | DI-13_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x000E | 40015 | R/W | word | DI-14_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x000F | 40016 | R/W | word | DI-15_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x0020 | 40033 | R/W | word | DI-00_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x0021 | 40034 | R/W | word | DI-01_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x0022 | 40035 | R/W | word | DI-02_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x0023 | 40036 | R/W | word | DI-03_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x0024 | 40037 | R/W | word | DI-04_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x0025 | 40038 | R/W | word | DI-05_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x0026 | 40039 | R/W | word | DI-06_filter | 03:HOLDIN G REGISTER | unit: 100 us |
| 0x0027 | 40040 | R/W | word | DI-07_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x0028 | 40041 | R/W | word | DI-08_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x0029 | 40042 | R/W | word | DI-09_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x002A | 40043 | R/W | word | DI-10_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x002B | 40044 | R/W | word | DI-11_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x002C | 40045 | R/W | word | DI-12_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x002D | 40046 | R/W | word | DI-13_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x002E | 40047 | R/W | word | DI-14_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x002F | 40048 | R/W | word | DI-15_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x0040 | 40065 | R/W | word | DI-00_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge, 2: Both |
| 0x0041 | 40066 | R/W | word | DI-01_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x0042 | 40067 | R/W | word | DI-02_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x0043 | 40068 | R/W | word | DI-03_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge, 1: Falling Edge, 2: Both |


<!-- Table 25 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x0044 | 40069 | R/W | word | DI-04_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge, 1: Falling Edge,2: Both |
| 0x0045 | 40070 | R/W | word | DI-05_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge, 1: Falling Edge, 2: Both |
| 0x0046 | 40071 | R/W | word | DI-06_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x0047 | 40072 | R/W | word | DI-07_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x0048 | 40073 | R/W | word | DI-08_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x0049 | 40074 | R/W | word | DI-09_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x004A | 40075 | R/W | word | DI-10_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x004B | 40076 | R/W | word | DI-11_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x004C | 40077 | R/W | word | DI-12_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x004D | 40078 | R/W | word | DI-13_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge, 2: Both |
| 0x004E | 40079 | R/W | word | DI-14_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x004F | 40080 | R/W | word | DI-15_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge, 1: Falling Edge, 2: Both |


<!-- Table 26 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x0000 | 00001 | R/W | bit | DI-00_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0001 | 00002 | R/W | bit | DI-01_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0002 | 00003 | R/W | bit | DI-02_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0003 | 00004 | R/W | bit | DI-03_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |


<!-- Table 27 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x0004 | 00005 | R/W | bit | DI-04_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0005 | 00006 | R/W | bit | DI-05_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0006 | 00007 | R/W | bit | DI-06_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0007 | 00008 | R/W | bit | DI-07_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0008 | 00009 | R/W | bit | DI-08_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0009 | 00010 | R/W | bit | DI-09_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x000A | 00011 | R/W | bit | DI-10_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x000B | 00012 | R/W | bit | DI-11_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x000C | 00013 | R/W | bit | DI-12_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x000D | 00014 | R/W | bit | DI-13_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x000E | 00015 | R/W | bit | DI-14_counterStatus | 01:COIL STATUS | 0: STOP, 1: START |
| 0x000F | 00016 | R/W | bit | DI-15_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0020 | 00033 | R/W | bit | DI-00_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0021 | 00034 | R/W | bit | DI-01_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0022 | 00035 | R/W | bit | DI-02_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0023 | 00036 | R/W | bit | DI-03_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0024 | 00037 | R/W | bit | DI-04_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0025 | 00038 | R/W | bit | DI-05_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0026 | 00039 | R/W | bit | DI-06_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0027 | 00040 | R/W | bit | DI-07_counterReset | 01:COIL | 1: reset to initialvalue |
| 0x0027 | 00040 | R/W | bit | DI-07_counterReset | STATUS | 1: reset to initialvalue |
| 0x0028 | 00041 | R/W | bit | DI-08_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0029 | 00042 | R/W | bit | DI-09_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x002A | 00043 | R/W | bit | DI-10_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x002B | 00044 | R/W | bit | DI-11_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x002C | 00045 | R/W | bit | DI-12_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x002D | 00046 | R/W | bit | DI-13_counterReset | 01:COILSTATUS | 1: reset to initialvalue |


<!-- Table 28 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x002E | 00047 | R/W | bit | DI-14_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x002F | 00048 | R/W | bit | DI-15_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0040 | 00065 | R/W | bit | DI-00_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0041 | 00066 | R/W | bit | DI-01_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0042 | 00067 | R/W | bit | DI-02_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0043 | 00068 | R/W | bit | DI-03_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0044 | 00069 | R/W | bit | DI-04_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0045 | 00070 | R/W | bit | DI-05_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0046 | 00071 | R/W | bit | DI-06_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0047 | 00072 | R/W | bit | DI-07_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0048 | 00073 | R/W | bit | DI-08_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0049 | 00074 | R/W | bit | DI-09_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x004A | 00075 | R/W | bit | DI-10_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x004B | 00076 | R/W | bit | DI-11_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x004C | 00077 | R/W | bit | DI-12_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x004D | 00078 | R/W | bit | DI-13_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x004E | 00079 | R/W | bit | DI-14_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x004F | 00080 | R/W | bit | DI-15_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0060 | 00097 | R/W | bit | DI-00_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0061 | 00098 | R/W | bit | DI-01_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0062 | 00099 | R/W | bit | DI-02_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0063 | 00100 | R/W | bit | DI-03_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0064 | 00101 | R/W | bit | DI-04_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0065 | 00102 | R/W | bit | DI-05_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0066 | 00103 | R/W | bit | DI-06_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0067 | 00104 | R/W | bit | DI-07_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |


<!-- Table 29 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x0068 | 00105 | R/W | bit | DI-08_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0069 | 00106 | R/W | bit | DI-09_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x006A | 00107 | R/W | bit | DI-10_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x006B | 00108 | R/W | bit | DI-11_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x006C | 00109 | R/W | bit | DI-12_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x006D | 00110 | R/W | bit | DI-13_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x006E | 00111 | R/W | bit | DI-14_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x006F | 00112 | R/W | bit | DI-15_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0080 | 00129 | R/W | bit | DI-00_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0081 | 00130 | R/W | bit | DI-01_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0082 | 00131 | R/W | bit | DI-02_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0083 | 00132 | R/W | bit | DI-03_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0084 | 00133 | R/W | bit | DI-04_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0085 | 00134 | R/W | bit | DI-05_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0086 | 00135 | R/W | bit | DI-06_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0087 | 00136 | R/W | bit | DI-07_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0088 | 00137 | R/W | bit | DI-08_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0089 | 00138 | R/W | bit | DI-09_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x008A | 00139 | R/W | bit | DI-10_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x008B | 00140 | R/W | bit | DI-11_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x008C | 00141 | R/W | bit | DI-12_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x008D | 00142 | R/W | bit | DI-13_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x008E | 00143 | R/W | bit | DI-14_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x008F | 00144 | R/W | bit | DI-15_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x00A0 | 00161 | R/W | bit | DI-00_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1: Enable |
| 0x00A1 | 00162 | R/W | bit | DI-01_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1: Enable |


<!-- Table 30 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x00A2 | 00163 | R/W | bit | DI-02_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00A3 | 00164 | R/W | bit | DI-03_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00A4 | 00165 | R/W | bit | DI-04_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00A5 | 00166 | R/W | bit | DI-05_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1: Enable |
| 0x00A6 | 00167 | R/W | bit | DI-06_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00A7 | 00168 | R/W | bit | DI-07_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00A8 | 00169 | R/W | bit | DI-08_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00A9 | 00170 | R/W | bit | DI-09_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00AA | 00171 | R/W | bit | DI-10_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00AB | 00172 | R/W | bit | DI-11_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00AC | 00173 | R/W | bit | DI-12_counterPowerOffStorage | 01:COIL STATUS | 0: Disable, 1:Enable |
| 0x00AD | 00174 | R/W | bit | DI-13_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00AE | 00175 | R/W | bit | DI-14_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00AF | 00176 | R/W | bit | DI-15_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x0140 | 00321 | R/W | bit | DO-00_status | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0141 | 00322 | R/W | bit | DO-01_status | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0142 | 00323 | R/W | bit | DO-02_status | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0143 | 00324 | R/W | bit | DO-03_status | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0144 | 00325 | R/W | bit | DO-04_status | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0145 | 00326 | R/W | bit | DO-05_status | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0146 | 00327 | R/W | bit | DO-06_status | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0147 | 00328 | R/W | bit | DO-07_status | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0160 | 00353 | R/W | bit | DO-00_powerOnStatus | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0161 | 00354 | R/W | bit | DO-01_powerOnStatus | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0162 | 00355 | R/W | bit | DO-02_powerOnStatus | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0163 | 00356 | R/W | bit | DO-03_powerOnStatus | 01:COILSTATUS | 0: OFF, 1: ON |


<!-- Table 31 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x0164 | 00357 | R/W | bit | DO-04_powerOnStatus | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0165 | 00358 | R/W | bit | DO-05_powerOnStatus | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0166 | 00359 | R/W | bit | DO-06_powerOnStatus | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0167 | 00360 | R/W | bit | DO-07_powerOnStatus | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0180 | 00385 | R/W | bit | DO-00_pulseStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0181 | 00386 | R/W | bit | DO-01_pulseStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0182 | 00387 | R/W | bit | DO-02_pulseStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0183 | 00388 | R/W | bit | DO-03_pulseStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0184 | 00389 | R/W | bit | DO-04_pulseStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0185 | 00390 | R/W | bit | DO-05_pulseStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0186 | 00391 | R/W | bit | DO-06_pulseStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0187 | 00392 | R/W | bit | DO-07_pulseStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01A0 | 00417 | R/W | bit | DO-00_pulsePowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01A1 | 00418 | R/W | bit | DO-01_pulsePowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01A2 | 00419 | R/W | bit | DO-02_pulsePowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01A3 | 00420 | R/W | bit | DO-03_pulsePowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01A4 | 00421 | R/W | bit | DO-04_pulsePowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01A5 | 00422 | R/W | bit | DO-05_pulsePowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01A6 | 00423 | R/W | bit | DO-06_pulsePowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01A7 | 00424 | R/W | bit | DO-07_pulsePowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01C0 | 00449 | R/W | bit | DO-00_pulseSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01C1 | 00450 | R/W | bit | DO-01_pulseSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01C2 | 00451 | R/W | bit | DO-02_pulseSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01C3 | 00452 | R/W | bit | DO-03_pulseSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01C4 | 00453 | R/W | bit | DO-04_pulseSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01C5 | 00454 | R/W | bit | DO-05_pulseSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |


<!-- Table 32 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x01C6 | 00455 | R/W | bit | DO-06_pulseSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01C7 | 00456 | R/W | bit | DO-07_pulseSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0000 | 10001 | R | bit | DI-00_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0001 | 10002 | R | bit | DI-01_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0002 | 10003 | R | bit | DI-02_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0003 | 10004 | R | bit | DI-03_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0004 | 10005 | R | bit | DI-04_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0005 | 10006 | R | bit | DI-05_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0006 | 10007 | R | bit | DI-06_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0007 | 10008 | R | bit | DI-07_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0008 | 10009 | R | bit | DI-08_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0009 | 10010 | R | bit | DI-09_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x000A | 10011 | R | bit | DI-10_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x000B | 10012 | R | bit | DI-11_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x000C | 10013 | R | bit | DI-12_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x000D | 10014 | R | bit | DI-13_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x000E | 10015 | R | bit | DI-14_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x000F | 10016 | R | bit | DI-15_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x2830 | 10289 | R | bit | DIO-00_mode | 02:INPUTSTATUS | 0: DI, 1: DO |
| 0x2831 | 10290 | R | bit | DIO-01_mode | 02:INPUTSTATUS | 0: DI, 1: DO |
| 0x2832 | 10291 | R | bit | DIO-02_mode | 02:INPUTSTATUS | 0: DI, 1: DO |
| 0x2833 | 10292 | R | bit | DIO-03_mode | 02:INPUTSTATUS | 0: DI, 1: DO |
| 0x2834 | 10293 | R | bit | DIO-04_mode | 02:INPUTSTATUS | 0: DI, 1: DO |
| 0x2835 | 10294 | R | bit | DIO-05_mode | 02:INPUTSTATUS | 0: DI, 1: DO |
| 0x2836 | 10295 | R | bit | DIO-06_mode | 02:INPUTSTATUS | 0: DI, 1: DO |
| 0x2837 | 10296 | R | bit | DIO-07_mode | 02:INPUTSTATUS | 0: DI, 1: DO |


<!-- Table 33 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x0000 | 30001 | R | word | DI-00_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0001 | 30002 | R | word | DI-01_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0002 | 30003 | R | word | DI-02_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0003 | 30004 | R | word | DI-03_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0004 | 30005 | R | word | DI-04_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0005 | 30006 | R | word | DI-05_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0006 | 30007 | R | word | DI-06_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0007 | 30008 | R | word | DI-07_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0008 | 30009 | R | word | DI-08_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0009 | 30010 | R | word | DI-09_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x000A | 30011 | R | word | DI-10_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x000B | 30012 | R | word | DI-11_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x000C | 30013 | R | word | DI-12_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x000D | 30014 | R | word | DI-13_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x000E | 30015 | R | word | DI-14_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x000F | 30016 | R | word | DI-15_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0020 | 30033 | R | word | DI-00_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0021 | 30034 | R | word | DI-00_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0022 | 30035 | R | word | DI-01_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0023 | 30036 | R | word | DI-01_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0024 | 30037 | R | word | DI-02_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0025 | 30038 | R | word | DI-02_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0026 | 30039 | R | word | DI-03_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0027 | 30040 | R | word | DI-03_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0028 | 30041 | R | word | DI-04_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0029 | 30042 | R | word | DI-04_counterValueLow | 04:INPUTREGISTER | low word |


<!-- Table 34 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x002A | 30043 | R | word | DI-05_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x002B | 30044 | R | word | DI-05_counterValueLow | 04:INPUTREGISTER | low word |
| 0x002C | 30045 | R | word | DI-06_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x002D | 30046 | R | word | DI-06_counterValueLow | 04:INPUTREGISTER | low word |
| 0x002E | 30047 | R | word | DI-07_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x002F | 30048 | R | word | DI-07_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0030 | 30049 | R | word | DI-08_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0031 | 30050 | R | word | DI-08_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0032 | 30051 | R | word | DI-09_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0033 | 30052 | R | word | DI-09_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0034 | 30053 | R | word | DI-10_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0035 | 30054 | R | word | DI-10_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0036 | 30055 | R | word | DI-11_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0037 | 30056 | R | word | DI-11_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0038 | 30057 | R | word | DI-12_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0039 | 30058 | R | word | DI-12_counterValueLow | 04:INPUTREGISTER | low word |
| 0x003A | 30059 | R | word | DI-13_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x003B | 30060 | R | word | DI-13_counterValueLow | 04:INPUTREGISTER | low word |
| 0x003C | 30061 | R | word | DI-14_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x003D | 30062 | R | word | DI-14_counterValueLow | 04:INPUTREGISTER | low word |
| 0x003E | 30063 | R | word | DI-15_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x003F | 30064 | R | word | DI-15_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0120 | 30289 | R | word | DIO-00_mode | 04:INPUTREGISTER | 0: DI, 1: DO |
| 0x0121 | 30290 | R | word | DIO-01_mode | 04:INPUTREGISTER | 0: DI, 1: DO |
| 0x0122 | 30291 | R | word | DIO-02_mode | 04:INPUTREGISTER | 0: DI, 1: DO |
| 0x0123 | 30292 | R | word | DIO-03_mode | 04:INPUTREGISTER | 0: DI, 1: DO |


<!-- Table 35 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x0124 | 30293 | R | word | DIO-04_mode | 04:INPUTREGISTER | 0: DI, 1: DO |
| 0x0125 | 30294 | R | word | DIO-05_mode | 04:INPUTREGISTER | 0: DI, 1: DO |
| 0x0126 | 30295 | R | word | DIO-06_mode | 04:INPUTREGISTER | 0: DI, 1: DO |
| 0x0127 | 30296 | R | word | DIO-07_mode | 04:INPUTREGISTER | 0: DI, 1: DO |
| 0x0000 | 40001 | R/W | word | DI-00_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x0001 | 40002 | R/W | word | DI-01_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x0002 | 40003 | R/W | word | DI-02_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x0003 | 40004 | R/W | word | DI-03_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x0004 | 40005 | R/W | word | DI-04_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x0005 | 40006 | R/W | word | DI-05_mode | 03:HOLDIN G REGISTER | 0: DI, 1: Counter |
| 0x0006 | 40007 | R/W | word | DI-06_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x0007 | 40008 | R/W | word | DI-07_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x0008 | 40009 | R/W | word | DI-08_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x0009 | 40010 | R/W | word | DI-09_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x000A | 40011 | R/W | word | DI-10_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x000B | 40012 | R/W | word | DI-11_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x000C | 40013 | R/W | word | DI-12_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x000D | 40014 | R/W | word | DI-13_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x000E | 40015 | R/W | word | DI-14_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x000F | 40016 | R/W | word | DI-15_mode | 03:HOLDING REGISTER | 0: DI, 1: Counter |
| 0x0020 | 40033 | R/W | word | DI-00_filter | 03:HOLDIN G REGISTER | unit: 100 us |
| 0x0021 | 40034 | R/W | word | DI-01_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x0022 | 40035 | R/W | word | DI-02_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x0023 | 40036 | R/W | word | DI-03_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x0024 | 40037 | R/W | word | DI-04_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x0025 | 40038 | R/W | word | DI-05_filter | 03:HOLDING REGISTER | unit: 100 us |


<!-- Table 36 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x0026 | 40039 | R/W | word | DI-06_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x0027 | 40040 | R/W | word | DI-07_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x0028 | 40041 | R/W | word | DI-08_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x0029 | 40042 | R/W | word | DI-09_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x002A | 40043 | R/W | word | DI-10_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x002B | 40044 | R/W | word | DI-11_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x002C | 40045 | R/W | word | DI-12_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x002D | 40046 | R/W | word | DI-13_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x002E | 40047 | R/W | word | DI-14_filter | 03:HOLDING REGISTER | unit: 100 us |
| 0x002F | 40048 | R/W | word | DI-15_filter | 03:HOLDIN G REGISTER | unit: 100 us |
| 0x0040 | 40065 | R/W | word | DI-00_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x0041 | 40066 | R/W | word | DI-01_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x0042 | 40067 | R/W | word | DI-02_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x0043 | 40068 | R/W | word | DI-03_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge, 1: Falling Edge,2: Both |
| 0x0044 | 40069 | R/W | word | DI-04_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge, 1: Falling Edge,2: Both |
| 0x0045 | 40070 | R/W | word | DI-05_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge, 1: Falling Edge,2: Both |
| 0x0046 | 40071 | R/W | word | DI-06_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x0047 | 40072 | R/W | word | DI-07_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x0048 | 40073 | R/W | word | DI-08_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge, 2: Both |
| 0x0049 | 40074 | R/W | word | DI-09_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge, 1: Falling Edge, 2: Both |


<!-- Table 37 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x004A | 40075 | R/W | word | DI-10_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge, 1: Falling Edge,2: Both |
| 0x004B | 40076 | R/W | word | DI-11_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge, 1: Falling Edge, 2: Both |
| 0x004C | 40077 | R/W | word | DI-12_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x004D | 40078 | R/W | word | DI-13_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x004E | 40079 | R/W | word | DI-14_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x004F | 40080 | R/W | word | DI-15_counterEvent | 03:HOLDING REGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x0140 | 40321 | R/W | word | DO-00_status | 03:HOLDING REGISTER | 0: OFF, 1: ON |
| 0x0141 | 40322 | R/W | word | DO-01_status | 03:HOLDING REGISTER | 0: OFF, 1: ON |
| 0x0142 | 40323 | R/W | word | DO-02_status | 03:HOLDING REGISTER | 0: OFF, 1: ON |
| 0x0143 | 40324 | R/W | word | DO-03_status | 03:HOLDING REGISTER | 0: OFF, 1: ON |
| 0x0144 | 40325 | R/W | word | DO-04_status | 03:HOLDING REGISTER | 0: OFF, 1: ON |
| 0x0145 | 40326 | R/W | word | DO-05_status | 03:HOLDING REGISTER | 0: OFF, 1: ON |
| 0x0146 | 40327 | R/W | word | DO-06_status | 03:HOLDING REGISTER | 0: OFF, 1: ON |
| 0x0147 | 40328 | R/W | word | DO-07_status | 03:HOLDING REGISTER | 0: OFF, 1: ON |
| 0x0160 | 40353 | R/W | word | DO-00_pulseCountHigh | 03:HOLDING REGISTER | high word |
| 0x0161 | 40354 | R/W | word | DO-00_pulseCountLow | 03:HOLDING REGISTER | low word |
| 0x0162 | 40355 | R/W | word | DO-01_pulseCountHigh | 03:HOLDING REGISTER | high word |
| 0x0163 | 40356 | R/W | word | DO-01_pulseCountLow | 03:HOLDIN | low word |
| 0x0163 | 40356 | R/W | word | DO-01_pulseCountLow |  | low word |
| 0x0163 | 40356 | R/W | word | DO-01_pulseCountLow | G REGISTER | low word |
| 0x0164 | 40357 | R/W | word | DO-02_pulseCountHigh | 03:HOLDIN | high word |
| 0x0164 | 40357 | R/W | word | DO-02_pulseCountHigh |  | high word |
| 0x0164 | 40357 | R/W | word | DO-02_pulseCountHigh | G REGISTER | high word |
| 0x0165 | 40358 | R/W | word | DO-02_pulseCountLow | 03:HOLDING REGISTER | low word |
| 0x0166 | 40359 | R/W | word | DO-03_pulseCountHigh | 03:HOLDING REGISTER | high word |
| 0x0167 | 40360 | R/W | word | DO-03_pulseCountLow | 03:HOLDING REGISTER | low word |
| 0x0168 | 40361 | R/W | word | DO-04_pulseCountHigh | 03:HOLDING REGISTER | high word |


<!-- Table 38 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x0169 | 40362 | R/W | word | DO-04_pulseCountLow | 03:HOLDING REGISTER | low word |
| 0x016A | 40363 | R/W | word | DO-05_pulseCountHigh | 03:HOLDING REGISTER | high word |
| 0x016B | 40364 | R/W | word | DO-05_pulseCountLow | 03:HOLDING REGISTER | low word |
| 0x016C | 40365 | R/W | word | DO-06_pulseCountHigh | 03:HOLDING REGISTER | high word |
| 0x016D | 40366 | R/W | word | DO-06_pulseCountLow | 03:HOLDING REGISTER | low word |
| 0x016E | 40367 | R/W | word | DO-07_pulseCountHigh | 03:HOLDING REGISTER | high word |
| 0x016F | 40368 | R/W | word | DO-07_pulseCountLow | 03:HOLDING REGISTER | low word |
| 0x01A0 | 40417 | R/W | word | DO-00_pulseOnWidth | 03:HOLDING REGISTER | unit: 1 ms |
| 0x01A1 | 40418 | R/W | word | DO-01_pulseOnWidth | 03:HOLDING REGISTER | unit: 1 ms |
| 0x01A2 | 40419 | R/W | word | DO-02_pulseOnWidth | 03:HOLDIN G REGISTER | unit: 1 ms |
| 0x01A3 | 40420 | R/W | word | DO-03_pulseOnWidth | 03:HOLDING REGISTER | unit: 1 ms |
| 0x01A4 | 40421 | R/W | word | DO-04_pulseOnWidth | 03:HOLDING REGISTER | unit: 1 ms |
| 0x01A5 | 40422 | R/W | word | DO-05_pulseOnWidth | 03:HOLDING REGISTER | unit: 1 ms |
| 0x01A6 | 40423 | R/W | word | DO-06_pulseOnWidth | 03:HOLDING REGISTER | unit: 1 ms |
| 0x01A7 | 40424 | R/W | word | DO-07_pulseOnWidth | 03:HOLDING REGISTER | unit: 1 ms |
| 0x01C0 | 40449 | R/W | word | DO-00_pulseOffWidth | 03:HOLDING REGISTER | unit: 1 ms |
| 0x01C1 | 40450 | R/W | word | DO-01_pulseOffWidth | 03:HOLDING REGISTER | unit: 1 ms |
| 0x01C2 | 40451 | R/W | word | DO-02_pulseOffWidth | 03:HOLDING REGISTER | unit: 1 ms |
| 0x01C3 | 40452 | R/W | word | DO-03_pulseOffWidth | 03:HOLDING REGISTER | unit: 1 ms |
| 0x01C4 | 40453 | R/W | word | DO-04_pulseOffWidth | 03:HOLDING REGISTER | unit: 1 ms |
| 0x01C5 | 40454 | R/W | word | DO-05_pulseOffWidth | 03:HOLDING REGISTER | unit: 1 ms |
| 0x01C6 | 40455 | R/W | word | DO-06_pulseOffWidth | 03:HOLDING REGISTER | unit: 1 ms |
| 0x01C7 | 40456 | R/W | word | DO-07_pulseOffWidth | 03:HOLDING REGISTER | unit: 1 ms |
| 0x01E0 | 40481 | R/W | word | DO-00_safeModeStatus | 03:HOLDING REGISTER | 0: OFF, 1: ON, 2:Hold Last |
| 0x01E1 | 40482 | R/W | word | DO-01_safeModeStatus | 03:HOLDING REGISTER | 0: OFF, 1: ON, 2:Hold Last |
| 0x01E2 | 40483 | R/W | word | DO-02_safeModeStatus | 03:HOLDING REGISTER | 0: OFF, 1: ON, 2:Hold Last |


<!-- Table 39 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x01E3 | 40484 | R/W | word | DO-03_safeModeStatus | 03:HOLDING REGISTER | 0: OFF, 1: ON, 2:Hold Last |
| 0x01E4 | 40485 | R/W | word | DO-04_safeModeStatus | 03:HOLDING REGISTER | 0: OFF, 1: ON, 2:Hold Last |
| 0x01E5 | 40486 | R/W | word | DO-05_safeModeStatus | 03:HOLDING REGISTER | 0: OFF, 1: ON, 2:Hold Last |
| 0x01E6 | 40487 | R/W | word | DO-06_safeModeStatus | 03:HOLDING REGISTER | 0: OFF, 1: ON, 2:Hold Last |
| 0x01E7 | 40488 | R/W | word | DO-07_safeModeStatus | 03:HOLDING REGISTER | 0: OFF, 1: ON, 2:Hold Last |
| 0x0200 | 40513 | R/W | word | DO-00_mode | 03:HOLDING REGISTER | 0: DO, 1: Pulse |
| 0x0201 | 40514 | R/W | word | DO-01_mode | 03:HOLDING REGISTER | 0: DO, 1: Pulse |
| 0x0202 | 40515 | R/W | word | DO-02_mode | 03:HOLDING REGISTER | 0: DO, 1: Pulse |
| 0x0203 | 40516 | R/W | word | DO-03_mode | 03:HOLDING REGISTER | 0: DO, 1: Pulse |
| 0x0204 | 40517 | R/W | word | DO-04_mode | 03:HOLDIN G REGISTER | 0: DO, 1: Pulse |
| 0x0205 | 40518 | R/W | word | DO-05_mode | 03:HOLDING REGISTER | 0: DO, 1: Pulse |
| 0x0206 | 40519 | R/W | word | DO-06_mode | 03:HOLDING REGISTER | 0: DO, 1: Pulse |
| 0x0207 | 40520 | R/W | word | DO-07_mode | 03:HOLDING REGISTER | 0: DO, 1: Pulse |


<!-- Table 40 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x0000 | 00001 | R/W | bit | DI-00_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0001 | 00002 | R/W | bit | DI-01_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0002 | 00003 | R/W | bit | DI-02_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0003 | 00004 | R/W | bit | DI-03_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0004 | 00005 | R/W | bit | DI-04_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0005 | 00006 | R/W | bit | DI-05_counterStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0020 | 00033 | R/W | bit | DI-00_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0021 | 00034 | R/W | bit | DI-01_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0022 | 00035 | R/W | bit | DI-02_counterReset | 01:COILSTATUS | 1: reset to initialvalue |


<!-- Table 41 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x0023 | 00036 | R/W | bit | DI-03_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0024 | 00037 | R/W | bit | DI-04_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0025 | 00038 | R/W | bit | DI-05_counterReset | 01:COILSTATUS | 1: reset to initialvalue |
| 0x0040 | 00065 | R/W | bit | DI-00_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0041 | 00066 | R/W | bit | DI-01_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0042 | 00067 | R/W | bit | DI-02_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0043 | 00068 | R/W | bit | DI-03_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0044 | 00069 | R/W | bit | DI-04_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0045 | 00070 | R/W | bit | DI-05_counterOverflowFlag | 01:COILSTATUS | 1: clear overflowflag |
| 0x0060 | 00097 | R/W | bit | DI-00_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0061 | 00098 | R/W | bit | DI-01_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0062 | 00099 | R/W | bit | DI-02_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0063 | 00100 | R/W | bit | DI-03_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0064 | 00101 | R/W | bit | DI-04_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0065 | 00102 | R/W | bit | DI-05_counterPowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0080 | 00129 | R/W | bit | DI-00_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0081 | 00130 | R/W | bit | DI-01_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0082 | 00131 | R/W | bit | DI-02_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0083 | 00132 | R/W | bit | DI-03_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0084 | 00133 | R/W | bit | DI-04_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0085 | 00134 | R/W | bit | DI-05_counterSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x00A0 | 00161 | R/W | bit | DI-00_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00A1 | 00162 | R/W | bit | DI-01_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00A2 | 00163 | R/W | bit | DI-02_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x00A3 | 00164 | R/W | bit | DI-03_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1: Enable |
| 0x00A4 | 00165 | R/W | bit | DI-04_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1: Enable |


<!-- Table 42 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x00A5 | 00166 | R/W | bit | DI-05_counterPowerOffStorage | 01:COILSTATUS | 0: Disable, 1:Enable |
| 0x0140 | 00321 | R/W | bit | RLY-00_status | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0141 | 00322 | R/W | bit | RLY-01_status | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0142 | 00323 | R/W | bit | RLY-02_status | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0143 | 00324 | R/W | bit | RLY-03_status | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0144 | 00325 | R/W | bit | RLY-04_status | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0145 | 00326 | R/W | bit | RLY-05_status | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0160 | 00353 | R/W | bit | RLY-00_powerOnStatus | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0161 | 00354 | R/W | bit | RLY-01_powerOnStatus | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0162 | 00355 | R/W | bit | RLY-02_powerOnStatus | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0163 | 00356 | R/W | bit | RLY-03_powerOnStatus | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0164 | 00357 | R/W | bit | RLY-04_powerOnStatus | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0165 | 00358 | R/W | bit | RLY-05_powerOnStatus | 01:COILSTATUS | 0: OFF, 1: ON |
| 0x0180 | 00385 | R/W | bit | RLY-00_pulseStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0181 | 00386 | R/W | bit | RLY-01_pulseStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0182 | 00387 | R/W | bit | RLY-02_pulseStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0183 | 00388 | R/W | bit | RLY-03_pulseStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0184 | 00389 | R/W | bit | RLY-04_pulseStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x0185 | 00390 | R/W | bit | RLY-05_pulseStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01A0 | 00417 | R/W | bit | RLY-00_pulsePowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01A1 | 00418 | R/W | bit | RLY-01_pulsePowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01A2 | 00419 | R/W | bit | RLY-02_pulsePowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01A3 | 00420 | R/W | bit | RLY-03_pulsePowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01A4 | 00421 | R/W | bit | RLY-04_pulsePowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01A5 | 00422 | R/W | bit | RLY-05_pulsePowerOnStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01C0 | 00449 | R/W | bit | RLY-00_pulseSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |


<!-- Table 43 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x01C1 | 00450 | R/W | bit | RLY-01_pulseSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01C2 | 00451 | R/W | bit | RLY-02_pulseSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01C3 | 00452 | R/W | bit | RLY-03_pulseSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01C4 | 00453 | R/W | bit | RLY-04_pulseSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01C5 | 00454 | R/W | bit | RLY-05_pulseSafeModeStatus | 01:COILSTATUS | 0: STOP, 1: START |
| 0x01DF | 00480 | R/W | bit | RLY-00_pulseReset | 01:COILSTATUS | 1: reset pulse count |
| 0x01E0 | 00481 | R/W | bit | RLY-01_pulseReset | 01:COILSTATUS | 1: reset pulse count |
| 0x01E1 | 00482 | R/W | bit | RLY-02_pulseReset | 01:COILSTATUS | 1: reset pulse count |
| 0x01E2 | 00483 | R/W | bit | RLY-03_pulseReset | 01:COILSTATUS | 1: reset pulse count |
| 0x01E3 | 00484 | R/W | bit | RLY-04_pulseReset | 01:COILSTATUS | 1: reset pulse count |
| 0x01E4 | 00485 | R/W | bit | RLY-05_pulseReset | 01:COILSTATUS | 1: reset pulse count |
| 0x0000 | 10001 | R | bit | DI-00_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0001 | 10002 | R | bit | DI-01_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0002 | 10003 | R | bit | DI-02_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0003 | 10004 | R | bit | DI-03_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0004 | 10005 | R | bit | DI-04_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0005 | 10006 | R | bit | DI-05_status | 02:INPUTSTATUS | 0: OFF, 1: ON |
| 0x0000 | 30001 | R | word | DI-00_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0001 | 30002 | R | word | DI-01_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0002 | 30003 | R | word | DI-02_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0003 | 30004 | R | word | DI-03_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0004 | 30005 | R | word | DI-04_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0005 | 30006 | R | word | DI-05_status | 04:INPUTREGISTER | 0: OFF, 1: ON |
| 0x0020 | 30033 | R | word | DI-00_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0021 | 30034 | R | word | DI-00_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0022 | 30035 | R | word | DI-01_counterValueHigh | 04:INPUTREGISTER | high word |


<!-- Table 44 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x0023 | 30036 | R | word | DI-01_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0024 | 30037 | R | word | DI-02_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0025 | 30038 | R | word | DI-02_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0026 | 30039 | R | word | DI-03_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0027 | 30040 | R | word | DI-03_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0028 | 30041 | R | word | DI-04_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x0029 | 30042 | R | word | DI-04_counterValueLow | 04:INPUTREGISTER | low word |
| 0x002A | 30043 | R | word | DI-05_counterValueHigh | 04:INPUTREGISTER | high word |
| 0x002B | 30044 | R | word | DI-05_counterValueLow | 04:INPUTREGISTER | low word |
| 0x0140 | 30321 | R | word | RLY-00_totalCountHigh | 04:INPUTREGISTER | high word |
| 0x0141 | 30322 | R | word | RLY-00_totalCountLow | 04:INPUTREGISTER | low word |
| 0x0142 | 30323 | R | word | RLY-01_totalCountHigh | 04:INPUTREGISTER | high word |
| 0x0143 | 30324 | R | word | RLY-01_totalCountLow | 04:INPUTREGISTER | low word |
| 0x0144 | 30325 | R | word | RLY-02_totalCountHigh | 04:INPUTREGISTER | high word |
| 0x0145 | 30326 | R | word | RLY-02_totalCountLow | 04:INPUTREGISTER | low word |
| 0x0146 | 30327 | R | word | RLY-03_totalCountHigh | 04:INPUTREGISTER | high word |
| 0x0147 | 30328 | R | word | RLY-03_totalCountLow | 04:INPUTREGISTER | low word |
| 0x0148 | 30329 | R | word | RLY-04_totalCountHigh | 04:INPUTREGISTER | high word |
| 0x0149 | 30330 | R | word | RLY-04_totalCountLow | 04:INPUTREGISTER | low word |
| 0x014A | 30331 | R | word | RLY-05_totalCountHigh | 04:INPUTREGISTER | high word |
| 0x014B | 30332 | R | word | RLY-05_totalCountLow | 04:INPUTREGISTER | low word |
| 0x0180 | 30385 | R | word | RLY-00_currentCountHigh | 04:INPUTREGISTER | high word |
| 0x0181 | 30386 | R | word | RLY-00_currentCountLow | 04:INPUTREGISTER | low word |
| 0x0182 | 30387 | R | word | RLY-01_currentCountHigh | 04:INPUTREGISTER | high word |
| 0x0183 | 30388 | R | word | RLY-01_currentCountLow | 04:INPUTREGISTER | low word |
| 0x0184 | 30389 | R | word | RLY-02_currentCountHigh | 04:INPUTREGISTER | high word |


<!-- Table 45 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x0185 | 30390 | R | word | RLY-02_currentCountLow | 04:INPUTREGISTER | low word |
| 0x0186 | 30391 | R | word | RLY-03_currentCountHigh | 04:INPUTREGISTER | high word |
| 0x0187 | 30392 | R | word | RLY-03_currentCountLow | 04:INPUTREGISTER | low word |
| 0x0188 | 30393 | R | word | RLY-04_currentCountHigh | 04:INPUTREGISTER | high word |
| 0x0189 | 30394 | R | word | RLY-04_currentCountLow | 04:INPUTREGISTER | low word |
| 0x018A | 30395 | R | word | RLY-05_currentCountHigh | 04:INPUTREGISTER | high word |
| 0x018B | 30396 | R | word | RLY-05_currentCountLow | 04:INPUTREGISTER | low word |
| 0x0000 | 40001 | R/W | word | DI-00_mode | 03:HOLDINGREGISTER | 0: DI, 1: Counter |
| 0x0001 | 40002 | R/W | word | DI-01_mode | 03:HOLDINGREGISTER | 0: DI, 1: Counter |
| 0x0002 | 40003 | R/W | word | DI-02_mode | 03:HOLDINGREGISTER | 0: DI, 1: Counter |
| 0x0003 | 40004 | R/W | word | DI-03_mode | 03:HOLDINGREGISTER | 0: DI, 1: Counter |
| 0x0004 | 40005 | R/W | word | DI-04_mode | 03:HOLDINGREGISTER | 0: DI, 1: Counter |
| 0x0005 | 40006 | R/W | word | DI-05_mode | 03:HOLDINGREGISTER | 0: DI, 1: Counter |
| 0x0020 | 40033 | R/W | word | DI-00_filter | 03:HOLDINGREGISTER | unit: 100 us |
| 0x0021 | 40034 | R/W | word | DI-01_filter | 03:HOLDINGREGISTER | unit: 100 us |
| 0x0022 | 40035 | R/W | word | DI-02_filter | 03:HOLDINGREGISTER | unit: 100 us |
| 0x0023 | 40036 | R/W | word | DI-03_filter | 03:HOLDINGREGISTER | unit: 100 us |
| 0x0024 | 40037 | R/W | word | DI-04_filter | 03:HOLDINGREGISTER | unit: 100 us |
| 0x0025 | 40038 | R/W | word | DI-05_filter | 03:HOLDINGREGISTER | unit: 100 us |
| 0x0040 | 40065 | R/W | word | DI-00_counterEvent | 03:HOLDINGREGISTER | 0: Rising Edge,1: Falling Edge, 2: Both |
| 0x0041 | 40066 | R/W | word | DI-01_counterEvent | 03:HOLDINGREGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x0042 | 40067 | R/W | word | DI-02_counterEvent | 03:HOLDINGREGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x0043 | 40068 | R/W | word | DI-03_counterEvent | 03:HOLDINGREGISTER | 0: Rising Edge,1: Falling Edge, 2: Both |


<!-- Table 46 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x0044 | 40069 | R/W | word | DI-04_counterEvent | 03:HOLDINGREGISTER | 0: Rising Edge,1: Falling Edge,2: Both |
| 0x0045 | 40070 | R/W | word | DI-05_counterEvent | 03:HOLDINGREGISTER | 0: Rising Edge, 1: Falling Edge,2: Both |
| 0x0140 | 40321 | R/W | word | RLY-00_status | 03:HOLDINGREGISTER | 0: OFF, 1: ON |
| 0x0141 | 40322 | R/W | word | RLY-01_status | 03:HOLDINGREGISTER | 0: OFF, 1: ON |
| 0x0142 | 40323 | R/W | word | RLY-02_status | 03:HOLDINGREGISTER | 0: OFF, 1: ON |
| 0x0143 | 40324 | R/W | word | RLY-03_status | 03:HOLDINGREGISTER | 0: OFF, 1: ON |
| 0x0144 | 40325 | R/W | word | RLY-04_status | 03:HOLDINGREGISTER | 0: OFF, 1: ON |
| 0x0145 | 40326 | R/W | word | RLY-05_status | 03:HOLDINGREGISTER | 0: OFF, 1: ON |
| 0x0160 | 40353 | R/W | word | RLY-00_pulseCountHigh | 03:HOLDINGREGISTER | high word |
| 0x0161 | 40354 | R/W | word | RLY-00_pulseCountLow | 03:HOLDINGREGISTER | low word |
| 0x0162 | 40355 | R/W | word | RLY-01_pulseCountHigh | 03:HOLDINGREGISTER | high word |
| 0x0163 | 40356 | R/W | word | RLY-01_pulseCountLow | 03:HOLDINGREGISTER | low word |
| 0x0164 | 40357 | R/W | word | RLY-02_pulseCountHigh | 03:HOLDINGREGISTER | high word |
| 0x0165 | 40358 | R/W | word | RLY-02_pulseCountLow | 03:HOLDINGREGISTER | low word |
| 0x0166 | 40359 | R/W | word | RLY-03_pulseCountHigh | 03:HOLDINGREGISTER | high word |
| 0x0167 | 40360 | R/W | word | RLY-03_pulseCountLow | 03:HOLDINGREGISTER | low word |
| 0x0168 | 40361 | R/W | word | RLY-04_pulseCountHigh | 03:HOLDINGREGISTER | high word |
| 0x0169 | 40362 | R/W | word | RLY-04_pulseCountLow | 03:HOLDINGREGISTER | low word |
| 0x016A | 40363 | R/W | word | RLY-05_pulseCountHigh | 03:HOLDINGREGISTER | high word |
| 0x016B | 40364 | R/W | word | RLY-05_pulseCountLow | 03:HOLDINGREGISTER | low word |
| 0x01E0 | 40481 | R/W | word | RLY-00_safeModeStatus | 03:HOLDINGREGISTER | 0: OFF, 1: ON, 2:Hold Last |
| 0x01E1 | 40482 | R/W | word | RLY-01_safeModeStatus | 03:HOLDINGREGISTER | 0: OFF, 1: ON, 2:Hold Last |
| 0x01E2 | 40483 | R/W | word | RLY-02_safeModeStatus | 03:HOLDINGREGISTER | 0: OFF, 1: ON, 2:Hold Last |
| 0x01E3 | 40484 | R/W | word | RLY-03_safeModeStatus | 03:HOLDINGREGISTER | 0: OFF, 1: ON, 2:Hold Last |
| 0x01E4 | 40485 | R/W | word | RLY-04_safeModeStatus | 03:HOLDINGREGISTER | 0: OFF, 1: ON, 2:Hold Last |


<!-- Table 47 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x01E5 | 40486 | R/W | word | RLY-05_safeModeStatus | 03:HOLDINGREGISTER | 0: OFF, 1: ON, 2:Hold Last |
| 0x01E6 | 40487 | R/W | word | RLY-06_safeModeStatus | 03:HOLDINGREGISTER | 0: OFF, 1: ON, 2:Hold Last |
| 0x01E7 | 40488 | R/W | word | RLY-07_safeModeStatus | 03:HOLDINGREGISTER | 0: OFF, 1: ON, 2:Hold Last |
| 0x0220 | 40545 | R/W | word | RLY-00_PowerOnDelay | 03:HOLDINGREGISTER | unit: sec(s) |
| 0x0221 | 40546 | R/W | word | RLY-01_PowerOnDelay | 03:HOLDINGREGISTER | unit: sec(s) |
| 0x0222 | 40547 | R/W | word | RLY-02_PowerOnDelay | 03:HOLDINGREGISTER | unit: sec(s) |
| 0x0223 | 40548 | R/W | word | RLY-03_PowerOnDelay | 03:HOLDINGREGISTER | unit: sec(s) |
| 0x0224 | 40549 | R/W | word | RLY-04_PowerOnDelay | 03:HOLDINGREGISTER | unit: sec(s) |
| 0x0225 | 40550 | R/W | word | RLY-05_PowerOnDelay | 03:HOLDINGREGISTER | unit: sec(s) |


<!-- Table 48 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x02C0 | 00705 | R/W | bit | AI-00_resetMinValue | 01:COILSTATUS | 1: reset min. value |
| 0x02C1 | 00706 | R/W | bit | AI-01_resetMinValue | 01:COILSTATUS | 1: reset min. value |
| 0x02C2 | 00707 | R/W | bit | AI-02_resetMinValue | 01:COILSTATUS | 1: reset min. value |
| 0x02C3 | 00708 | R/W | bit | AI-03_resetMinValue | 01:COILSTATUS | 1: reset min. value |
| 0x02C4 | 00709 | R/W | bit | AI-04_resetMinValue | 01:COILSTATUS | 1: reset min. value |
| 0x02C5 | 00710 | R/W | bit | AI-05_resetMinValue | 01:COILSTATUS | 1: reset min. value |
| 0x02C6 | 00711 | R/W | bit | AI-06_resetMinValue | 01:COILSTATUS | 1: reset min. value |
| 0x02C7 | 00712 | R/W | bit | AI-07_resetMinValue | 01:COILSTATUS | 1: reset min. value |
| 0x02E0 | 00737 | R/W | bit | AI-00_resetMaxValue | 01:COILSTATUS | 1: reset max. value |
| 0x02E1 | 00738 | R/W | bit | AI-01_resetMaxValue | 01:COILSTATUS | 1: reset max. value |
| 0x02E2 | 00739 | R/W | bit | AI-02_resetMaxValue | 01:COILSTATUS | 1: reset max. value |
| 0x02E3 | 00740 | R/W | bit | AI-03_resetMaxValue | 01:COILSTATUS | 1: reset max. value |
| 0x02E4 | 00741 | R/W | bit | AI-04_resetMaxValue | 01:COILSTATUS | 1: reset max. value |


<!-- Table 49 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x02E5 | 00742 | R/W | bit | AI-05_resetMaxValue | 01:COILSTATUS | 1: reset max. value |
| 0x02E6 | 00743 | R/W | bit | AI-06_resetMaxValue | 01:COILSTATUS | 1: reset max. value |
| 0x02E7 | 00744 | R/W | bit | AI-07_resetMaxValue | 01:COILSTATUS | 1: reset max. value |
| 0x02C0 | 30705 | R | word | AI-00_rawValue | 04:INPUTREGISTER |  |
| 0x02C1 | 30706 | R | word | AI-01_rawValue | 04:INPUT REGISTER |  |
| 0x02C2 | 30707 | R | word | AI-02_rawValue | 04:INPUTREGISTER |  |
| 0x02C3 | 30708 | R | word | AI-03_rawValue | 04:INPUTREGISTER |  |
| 0x02C4 | 30709 | R | word | AI-04_rawValue | 04:INPUTREGISTER |  |
| 0x02C5 | 30710 | R | word | AI-05_rawValue | 04:INPUTREGISTER |  |
| 0x02C6 | 30711 | R | word | AI-06_rawValue | 04:INPUT REGISTER |  |
| 0x02C7 | 30712 | R | word | AI-07_rawValue | 04:INPUT REGISTER |  |
| 0x02E0 | 30737 | R | word | AI-00_rawValueMin | 04:INPUTREGISTER |  |
| 0x02E1 | 30738 | R | word | AI-01_rawValueMin | 04:INPUTREGISTER |  |
| 0x02E2 | 30739 | R | word | AI-02_rawValueMin | 04:INPUTREGISTER |  |
| 0x02E3 | 30740 | R | word | AI-03_rawValueMin | 04:INPUTREGISTER |  |
| 0x02E4 | 30741 | R | word | AI-04_rawValueMin | 04:INPUTREGISTER |  |
| 0x02E5 | 30742 | R | word | AI-05_rawValueMin | 04:INPUTREGISTER |  |
| 0x02E6 | 30743 | R | word | AI-06_rawValueMin | 04:INPUTREGISTER |  |
| 0x02E7 | 30744 | R | word | AI-07_rawValueMin | 04:INPUTREGISTER |  |
| 0x0300 | 30769 | R | word | AI-00_rawValueMax | 04:INPUTREGISTER |  |
| 0x0301 | 30770 | R | word | AI-01_rawValueMax | 04:INPUT REGISTER |  |
| 0x0302 | 30771 | R | word | AI-02_rawValueMax | 04:INPUTREGISTER |  |
| 0x0303 | 30772 | R | word | AI-03_rawValueMax | 04:INPUTREGISTER |  |
| 0x0304 | 30773 | R | word | AI-04_rawValueMax | 04:INPUTREGISTER |  |
| 0x0305 | 30774 | R | word | AI-05_rawValueMax | 04:INPUTREGISTER |  |
| 0x0306 | 30775 | R | word | AI-06_rawValueMax | 04:INPUT REGISTER |  |


<!-- Table 50 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x0307 | 30776 | R | word | AI-07_rawValueMax | 04:INPUTREGISTER |  |
| 0x0320 | 30801 | R | word | AI-00_engineeringValueHigh | 04:INPUTREGISTER | high word |
| 0x0321 | 30802 | R | word | AI-00_engineeringValueLow | 04:INPUTREGISTER | low word |
| 0x0322 | 30803 | R | word | AI-01_engineeringValueHigh | 04:INPUTREGISTER | high word |
| 0x0323 | 30804 | R | word | AI-01_engineeringValueLow | 04:INPUTREGISTER | low word |
| 0x0324 | 30805 | R | word | AI-02_engineeringValueHigh | 04:INPUTREGISTER | high word |
| 0x0325 | 30806 | R | word | AI-02_engineeringValueLow | 04:INPUTREGISTER | low word |
| 0x0326 | 30807 | R | word | AI-03_engineeringValueHigh | 04:INPUTREGISTER | high word |
| 0x0327 | 30808 | R | word | AI-03_engineeringValueLow | 04:INPUTREGISTER | low word |
| 0x0328 | 30809 | R | word | AI-04_engineeringValueHigh | 04:INPUT REGISTER | high word |
| 0x0329 | 30810 | R | word | AI-04_engineeringValueLow | 04:INPUT REGISTER | low word |
| 0x032A | 30811 | R | word | AI-05_engineeringValueHigh | 04:INPUTREGISTER | high word |
| 0x032B | 30812 | R | word | AI-05_engineeringValueLow | 04:INPUTREGISTER | low word |
| 0x032C | 30813 | R | word | AI-06_engineeringValueHigh | 04:INPUTREGISTER | high word |
| 0x032D | 30814 | R | word | AI-06_engineeringValueLow | 04:INPUTREGISTER | low word |
| 0x032E | 30815 | R | word | AI-07_engineeringValueHigh | 04:INPUTREGISTER | high word |
| 0x032F | 30816 | R | word | AI-07_engineeringValueLow | 04:INPUTREGISTER | low word |
| 0x0360 | 30865 | R | word | AI-00_engineeringValueMinHigh | 04:INPUTREGISTER | high word |
| 0x0361 | 30866 | R | word | AI-00_engineeringValueMinLow | 04:INPUTREGISTER | low word |
| 0x0362 | 30867 | R | word | AI-01_engineeringValueMinHigh | 04:INPUTREGISTER | high word |
| 0x0363 | 30868 | R | word | AI-01_engineeringValueMinLow | 04:INPUT REGISTER | low word |
| 0x0364 | 30869 | R | word | AI-02_engineeringValueMinHigh | 04:INPUTREGISTER | high word |
| 0x0365 | 30870 | R | word | AI-02_engineeringValueMinLow | 04:INPUTREGISTER | low word |
| 0x0366 | 30871 | R | word | AI-03_engineeringValueMinHigh | 04:INPUTREGISTER | high word |
| 0x0367 | 30872 | R | word | AI-03_engineeringValueMinLow | 04:INPUTREGISTER | low word |
| 0x0368 | 30873 | R | word | AI-04_engineeringValueMinHigh | 04:INPUTREGISTER | high word |


<!-- Table 51 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x0369 | 30874 | R | word | AI-04_engineeringValueMinLow | 04:INPUTREGISTER | low word |
| 0x036A | 30875 | R | word | AI-05_engineeringValueMinHigh | 04:INPUTREGISTER | high word |
| 0x036B | 30876 | R | word | AI-05_engineeringValueMinLow | 04:INPUTREGISTER | low word |
| 0x036C | 30877 | R | word | AI-06_engineeringValueMinHigh | 04:INPUTREGISTER | high word |
| 0x036D | 30878 | R | word | AI-06_engineeringValueMinLow | 04:INPUTREGISTER | low word |
| 0x036E | 30879 | R | word | AI-07_engineeringValueMinHigh | 04:INPUTREGISTER | high word |
| 0x036F | 30880 | R | word | AI-07_engineeringValueMinLow | 04:INPUTREGISTER | low word |
| 0x03A0 | 30929 | R | word | AI-00_engineeringValueMaxHigh | 04:INPUT REGISTER | high word |
| 0x03A1 | 30930 | R | word | AI-00_engineeringValueMaxLow | 04:INPUTREGISTER | low word |
| 0x03A2 | 30931 | R | word | AI-01_engineeringValueMaxHigh | 04:INPUT REGISTER | high word |
| 0x03A3 | 30932 | R | word | AI-01_engineeringValueMaxLow | 04:INPUTREGISTER | low word |
| 0x03A4 | 30933 | R | word | AI-02_engineeringValueMaxHigh | 04:INPUTREGISTER | high word |
| 0x03A5 | 30934 | R | word | AI-02_engineeringValueMaxLow | 04:INPUTREGISTER | low word |
| 0x03A6 | 30935 | R | word | AI-03_engineeringValueMaxHigh | 04:INPUTREGISTER | high word |
| 0x03A7 | 30936 | R | word | AI-03_engineeringValueMaxLow | 04:INPUTREGISTER | low word |
| 0x03A8 | 30937 | R | word | AI-04_engineeringValueMaxHigh | 04:INPUTREGISTER | high word |
| 0x03A9 | 30938 | R | word | AI-04_engineeringValueMaxLow | 04:INPUTREGISTER | low word |
| 0x03AA | 30939 | R | word | AI-05_engineeringValueMaxHigh | 04:INPUTREGISTER | high word |
| 0x03AB | 30940 | R | word | AI-05_engineeringValueMaxLow | 04:INPUTREGISTER | low word |
| 0x03AC | 30941 | R | word | AI-06_engineeringValueMaxHigh | 04:INPUTREGISTER | high word |
| 0x03AD | 30942 | R | word | AI-06_engineeringValueMaxLow | 04:INPUT REGISTER | low word |
| 0x03AE | 30943 | R | word | AI-07_engineeringValueMaxHigh | 04:INPUT REGISTER | high word |
| 0x03AF | 30944 | R | word | AI-07_engineeringValueMaxLow | 04:INPUTREGISTER | low word |
| 0x03E0 | 30993 | R | word | AI-00_status | 04:INPUTREGISTER | 0: normal, 1: burnout, 2: over range, 3: under range |


<!-- Table 52 -->

| (hex)0x03E1 | (decimal)30994 | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x03E2 | 30995 |  | word | AI-01_status | 04:INPUTREGISTER | 0: normal, 1: burnout, 2: over range, 3: under range |
| 0x03E3 | 30996 |  | word | AI-02_status | 04:INPUTREGISTER | 0: normal, 1: burnout, 2: over range, 3: under range |
| 0x03E4 | 30997 |  | word | AI-03_status | 04:INPUTREGISTER | 0: normal, 1: burnout, 2: over range, 3: under range |
| 0x03E5 | 30998 |  | word | AI-04_status | 04:INPUTREGISTER | 0: normal, 1: burnout, 2: over range, 3: under range |
| 0x03E6 | 30999 |  | word | AI-05_status | 04:INPUTREGISTER | 0: normal, 1: burnout, 2: over range, 3: under range |
| 0x03E7 |  |  | word | AI-06_status | 04:INPUTREGISTER | 0: normal, 1: burnout, 2: over range, 3: under range |
| 0x02C0 |  |  | word | AI-07_status | 04:INPUTREGISTER | 0: normal, 1: burnout, 2: over range, 3: under range |
|  |  |  |  | AI-00_mode | 03:HOLDINGREGISTER | 0: 0-10 V, 1: 0-20 mA, 2: 4-20 mA burnout, 3: 4-20 mA |
|  |  |  |  | AI-01_mode | 03:HOLDINGREGISTER | 0: 0-10 V, 1: 0-20 mA, 2: 4-20 mA burnout, 3: 4-20 mA |
|  |  |  |  | AI-02_mode | 03:HOLDINGREGISTER | 0: 0-10 V, 1: 0-20 mA,2: 4-20 mA burnout, 3: 4-20 mA |
|  |  |  |  | AI-03_mode | 03:HOLDINGREGISTER | 0: 0-10 V, 1: 0-20 mA,2: 4-20 mAburnout, 3: 4-20 mA |


<!-- Table 53 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x02C4 | 40709 | R/W | word | AI-04_mode | 03:HOLDINGREGISTER | 0: 0-10 V, 1: 0-20 mA,2: 4-20 mA burnout, 3: 4-20 mA |
| 0x02C5 | 40710 | R/W | word | AI-05_mode | 03:HOLDINGREGISTER | 0: 0-10 V, 1: 0-20 mA, 2: 4-20 mAburnout, 3: 4-20 mA |
| 0x02C6 | 40711 | R/W | word | AI-06_mode | 03:HOLDINGREGISTER | 0: 0-10 V,1: 0-20 mA,2: 4-20 mA burnout, 3: 4-20 mA |
| 0x02C7 | 40712 | R/W | word | AI-07_mode | 03:HOLDINGREGISTER | 0: 0-10 V, 1: 0-20 mA,2: 4-20 mAburnout, 3: 4-20 mA |
| 0x02E0 | 40737 | R/W | word | AI-00_burnoutValueHigh | 03:HOLDINGREGISTER | high word |
| 0x02E1 | 40738 | R/W | word | AI-00_burnoutValueLow | 03:HOLDINGREGISTER | low word |
| 0x02E2 | 40739 | R/W | word | AI-01_burnoutValueHigh | 03:HOLDINGREGISTER | high word |
| 0x02E3 | 40740 | R/W | word | AI-01_burnoutValueLow | 03:HOLDINGREGISTER | low word |
| 0x02E4 | 40741 | R/W | word | AI-02_burnoutValueHigh | 03:HOLDINGREGISTER | high word |
| 0x02E5 | 40742 | R/W | word | AI-02_burnoutValueLow | 03:HOLDINGREGISTER | low word |
| 0x02E6 | 40743 | R/W | word | AI-03_burnoutValueHigh | 03:HOLDINGREGISTER | high word |
| 0x02E7 | 40744 | R/W | word | AI-03_burnoutValueLow | 03:HOLDINGREGISTER | low word |
| 0x02E8 | 40745 | R/W | word | AI-04_burnoutValueHigh | 03:HOLDINGREGISTER | high word |
| 0x02E9 | 40746 | R/W | word | AI-04_burnoutValueLow | 03:HOLDINGREGISTER | low word |
| 0x02EA | 40747 | R/W | word | AI-05_burnoutValueHigh | 03:HOLDINGREGISTER | high word |
| 0x02EB | 40748 | R/W | word | AI-05_burnoutValueLow | 03:HOLDINGREGISTER | low word |
| 0x02EC | 40749 | R/W | word | AI-06_burnoutValueHigh | 03:HOLDINGREGISTER | high word |
| 0x02ED | 40750 | R/W | word | AI-06_burnoutValueLow | 03:HOLDINGREGISTER | low word |
| 0x02EE | 40751 | R/W | word | AI-07_burnoutValueHigh | 03:HOLDINGREGISTER | high word |
| 0x02EF | 40752 | R/W | word | AI-07_burnoutValueLow | 03:HOLDINGREGISTER | low word |


<!-- Table 54 -->

| Address(hex) | Register(decimal) | Access | Type | Parameter Name | FunctionCode | Description |
| --- | --- | --- | --- | --- | --- | --- |
| 0x04A0 | 41185 | R/W | word | AO-00_rawValue | 03:HOLDINGREGISTER |  |
| 0x04A1 | 41186 | R/W | word | AO-01_rawValue | 03:HOLDINGREGISTER |  |
| 0x04A2 | 41187 | R/W | word | AO-02_rawValue | 03:HOLDINGREGISTER |  |
| 0x04A3 | 41188 | R/W | word | AO-03_rawValue | 03:HOLDINGREGISTER |  |
| 0x04C0 | 41217 | R/W | word | AO-00_rawValuePowerOn | 03:HOLDINGREGISTER |  |
| 0x04C1 | 41218 | R/W | word | AO-01_rawValuePowerOn | 03:HOLDINGREGISTER |  |
| 0x04C2 | 41219 | R/W | word | AO-02_rawValuePowerOn | 03:HOLDINGREGISTER |  |
| 0x04C3 | 41220 | R/W | word | AO-03_rawValuePowerOn | 03:HOLDINGREGISTER |  |
| 0x04E0 | 41249 | R/W | word | AO-00_rawValueSafeMode | 03:HOLDINGREGISTER |  |
| 0x04E1 | 41250 | R/W | word | AO-01_rawValueSafeMode | 03:HOLDINGREGISTER |  |
| 0x04E2 | 41251 | R/W | word | AO-02_rawValueSafeMode | 03:HOLDINGREGISTER |  |
| 0x04E3 | 41252 | R/W | word | AO-03_rawValueSafeMode | 03:HOLDINGREGISTER |  |
| 0x0500 | 41281 | R/W | word | AO-00_mode | 03:HOLDINGREGISTER | 0: 0-10 V,1: 0-20 mA,2: 4-20 mA |
| 0x0501 | 41282 | R/W | word | AO-01_mode | 03:HOLDINGREGISTER | 0: 0-10 V,1: 0-20 mA, 2: 4-20 mA |
| 0x0502 | 41283 | R/W | word | AO-02_mode | 03:HOLDINGREGISTER | 0: 0-10 V,1: 0-20 mA, 2: 4-20 mA |
| 0x0503 | 41284 | R/W | word | AO-03_mode | 03:HOLDINGREGISTER | 0: 0-10 V, 1: 0-20 mA, 2: 4-20 mA |


<!-- Table 55 -->

| Baudrate | 9600 |
| --- | --- |
| Data Bits | 8 |
| Parity | None |
| Stop Bits | 1 |
| Watchdog | Disable |
| Watchdog Time Out | 10 seconds |
| Server Name | Blank |
| Server Location | Blank |
| DI Mode | DI |
| Filter Time | 100 ms |
| Trigger for Counter | Lo to Hi |
| Counter Status | Stop |
| DO Mode | DO |
| DO Safe Status | Disable |
| Power On Status | Disable |
| Low Width for Pulses | 1 ms(1.5 s for relay) |
| High Width for Pulses | 1 ms(1.5 s for relay) |
| Output Pulses | 0(continuous) |
| DIO Mode | DO |
| AI Mode | Voltage |
| AO Mode | Voltage |
