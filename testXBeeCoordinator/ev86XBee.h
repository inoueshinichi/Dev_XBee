//
//  ev86XBee.h
//  
//
//  Created by 井上真一 on 2015/08/04.
//
//  このクラスはe-carのEV86車両情報可視化プロジェクトでXBeeをより簡単に使えるようにXBee_AndrewRappのラッパープログラムとして作成しました。
//  present by Digi International http://www.digi.com
//  An up-to-date datasheet is browsed on http://www.digi.com/support/productdatail?pid=4549

/*
 * One package frame is below.
 *
 * | Start Delimiter | Length | Frame-specific Data | Checksum |
 *
 *  Start Delimiter = 0x7E : 1 byte
 *  Length = | MSB | LSB | : 2 or 3 byte
 *  Frame-specific Data = | Frame Type | Specific-Data | : bytes depend on Frame Type
 *  Frame-specific Data is so called "API Frame".
 *
 *
 * Frame-specific Data is varied by each Frame ID as following below.
 * 
 * This lists are so called "API Frame Names"
 *
 * Case 1 : Frame Type is "AT Command(0x08)"
 *  Frame-specific Data = | Frame Type (0x08) | Frame ID (0x52) | AT Command | Parameter Value (# bytes) |
 *
 * Case 2 : Frame Type is "AT Command-Queue Parameter Value(0x09)"
 *  Frame-specific Data = | Frame Type (0x09) | Frame ID (0x01) | AT Command | Parameter Value (# bytes) |
 *
 * Case 3 : Frame Type is "ZigBee Transmit Request(0x11)"
 *  Frame-specific Data = | Frame Type (0x10) | Frame ID (0x01) | 64-bit Destination Address | 16-bit Destination Network Address |
 *                          BroadCast Radius (1 byte) | Options (1 byte) | RF Data (# bytes) |
 *
 * Case 4 : Frame Type is "Explicit Addressing ZigBee Command frame(0x11)"
 *  Frame-specific Data = | Frame Type (0x11) | Frame ID (0x01) | 64-bit Destination Address | 16-bit Destination Network Address |
 *                          Source Endpoint | Destination Endpoint | Cluster ID | Profile ID | BroadCast Radius (1 byte) | Transmit Options (1 byte) |
 *                          Data Payload (# bytes) |
 *
 * Case 5 : Frame Type is "Remote AT Command Request(0x17)"
 *  Frame-specific Data = | Frame Type (0x17) | Frame ID (0x01) | 64-bit Destination Address | 16-bit Destination Network Address |
 *                          Remote Command Options | AT Command | Command Parameter |
 *
 * Case 6 : Frame Type is "Create Source Route(0x21)"
 *  Frame-specific Data = | Frame Type (0x21) | Frame ID (0x00) | 64-bit Destination Address | 16-bit Destination Network Address |
 *                          Route Command Options | Number of Addresses | Address 1 (2 bytes) | Address 2 (closer hop) (2 bytes) | Address 3 (2 bytes) |
 *
 * Case 7 : Frame Type is "AT Command Response(0x88)"
 *  Frame-specific Data = | Frame Type (0x88) | Frame ID (0x01) | AT Command (2 bytes) | Command Status | Command Data |
 *
 * Case 8 : Frame Type is "Modem Status(0x8A)"
 *  Frame-specific Data = | Frame Type (0x8A) | Status (1 byte) |
 *
 * Case 9 : Frame Type is "ZigBee Transmit Status(0x8B)"
 *  Frame-specific Data = | Frame Type (0x8B) | Frame ID (0x01) | 16-bit address of destination | Transmit Retry Count (1 byte) | Delivery Status (1 byte) |
 *                          Discovery Status |
 *
 * Case 10 : Frame Type is "ZigBee Receive Packet(0x90)"
 *  Frame-specific Data = | Frame Type (0x90) | 64-bit Source Address | 16-bit Network Address | Receive Options (1 byte) | Received Data |
 *
 * Case 11 : Frame Type is "ZigBee Explicit Rx Indicator(0x91)"
 *  Frame-specific Data = | Frame Type (0x91) | 64-bit Source Address | 16-bit Network Address | Source Endpoint | Destination Endpoint | Cluster ID |
 *                          Profile ID | Receive Options | Received Data |
 *
 * Case 12 : Frame Type is "ZigBee IO Data Sample Rx Indicator(0x92)"
 *  Frame-specific Data = | Frame Type (0x92) | 64-bit Source Address | 16-bit Source Network Address | Receive Options | Number of Samples |
 *                          Digital Channel Mask* | Analog Channel Mask** | Digital Samples (if included) | Analog Sample
 *
 * Case 13 : Frame Type is "XBee Sensor Read Indicator(0x94)"
 *  Frame-specific Data = | Frame Type (0x94) | 64-bit Source Address | 16-bit Source Network Address | Receive Options (1 byte) | 1-Wire Sensors |
 *                          A/D Values | Temparature Read |
 *
 * Case 14 : Frame Type is "Node Identification Indicator(0x95)"
 *  Frame-specific Data = | 64-bit Source Address | 16-bit Source Network Address | Receive Options | Source 16-bit Address | 64-bit Network Address |
 *                          NI String (2 bytes) | Parent 16-bit Address | Device Type (1 byte) | Source Event (1 byte) | Digi Profile ID (2 bytes) |
 *                          Manufacture ID (2 bytes) |
 *
 * Case 15 : Frame Type is "Remote Command Response(0x97)"
 *  Frame-specific Data = | Frame Type (0x97) | Frame ID (0x55) | 64-bit Source (remote)Address | 16-bit Source (remote)Address | AT Commands |
 *                          Command Status (1 byte) | Command Data |
 *
 * Case 16 : Frame Type is "Over-the-Air Firmware Update Status(0xA0)"
 *  Frame-specific Data = | Frame Type (0xA0) | 64-bit Source (remote)Address | 16-bit Destination Address | Receive Options (1 byte) |
 *                          Bootloader Message Type (1 byte) | Block Number (1 byte) | 64-bit Target Address |
 *
 * Case 17 : Frame Type is "Route Recode Indicator(0xA1)"
 *  Frame-specific Data = | Frame Type (0xA1) | 64-bit Source Address | Source (updater) 16-bit Address | Receive Address | Number of Addresses |
 *                          Address 1 (2 bytes) | Address 2 (closer hop) (2 bytes) | Address n (neighbor of source) |
 *
 * Case 18 : Frame Type is "Many-to-One Route Request Indicator(0xA3)"
 *  Frame-specific Data = | 64-bit Source Address | Source 16-bit Address | Reserved (1 byte) |
 *
 *
 *************************************************************************************************************************
 * These are some extended API Frames about Profile configurations as following. 
 * Note that you don't have to use these commands if you usually use XBee.
 *
 *------------------------------------------------------------------------------------------------------------------------
 * Sending ZigBee Device Objects (ZDO) commands with the API
 *
 * ZDO Command                      Cluster ID
 * Network Address Request          0x0000
 * IEEE Address Request             0x0001
 * Node Descriptor Request          0x0002
 * Simple Descriptor Request        0x0004
 * Active Endpoints Request         0x0005
 * Match Descriptor Request         0x0006
 * Mgmt LQI Request                 0x0031
 * Mgmt Leave Request               0x0034
 * Mgmt Permit Joining Request      0x0036
 * Mgmt Network Update Request      0x0038
 *
 *  @The Explicit Transmit API frame(0x11) is used to send ZigBee Devices Objects commands to devices in the network.
 *  Frame-specific Data = | Frame Type (0x11) | Frame ID | 64-bit Destination Address | 16-bit Destination Network Address | 
 *                          Source Endpoint | Destination Endpoint | Cluster ID | Profile ID| Broadcast Radius | Transmit Options |
 *                          Data Payload ("Transaction Sequence Number" and "ZDO Payload") | ??? |
 *  @ I'm sorry. I'm patially not sure this command index.
 *
 * 
 *------------------------------------------------------------------------------------------------------------------------
 * Sending ZigBee Cluster Library (ZCL) commands with the API
 *
 * Cluster ID            Attributes ID                      Command ID
 * Basic(0x0000)         Application Version(0x0001)        -reset to defaults(0x00)
 *                       Hardware Version(0x0003)           
 *                       Model Identifier(0x0005)
 * Identify(0x0003)      Identify Time(0x000)               Identify(0x00)
 *                                                          Identify Query(0x01)
 * Time(0x000A)          Time(0x0000)
 *                       Time Status(0x0001)
 *                       Time Zone(0x0002)
 * Thermostat(0x0201)    Local Temperature(0x0000)          -Setpoint raise / lower(0x00)
 *                       Occupancy(0x0002)
 *
 * Command(Command ID)                  Description
 * Read Attributes (0x00)               Used to read one or more attributes on a remote device
 * Read Attributes Response(0x01)       Generated in response to a read attributes command
 * Write Attributes (0x02)              Used to change one or more attributes on a remote device
 * Write Attributes (0x04)              Sent in response to a write attributes command
 * Configure Repoting (0x06)            Used to configure a device to automatically report on the values of one or more of its attributes
 * Report Attributes (0x0A)             Used to report attributes when report conditions have been satisfied
 * Discover Attributes (0x0C)           Used to discover the attribute identifiers on a remote device
 * Discover Attributes Response (0x0D)  Sent in response to a discover attributes command
 *
 *
 * @The Explicit Transmit API frame(0x11) is used to send ZigBee Devices Objects commands to devices in the network.
 * Frame-specific Data = | Frame Type (0x11) | Frame ID (0x01) | 64-bit Destination Address | 16-bit Destination Network Address |
 *                         Source Endopoint | Destination Endpoint | Cluster ID | Profile ID | Broadcast Radius | Transmit Options |
 *                         ZCL Frame Header (Frame Control, Transaction Sequence Number, Command ID) | ZCL Payload (Attribute ID) |
 *
 *------------------------------------------------------------------------------------------------------------------------
 *Sending public profile commands with the API
 *
 * Frame-specific Data = | Frame Type (0x11) | Frame ID (0x01) | 64-bit Destination Address | 16-bit Destination Network Address |
 *                         Source Endpoint | Destination Endpoint | Cluster ID | Profile ID | Broadcast Radius | Transmit Options |
 *                         ZCL Frame Header (Frame Control, Transaction Sequence Number, Command ID) | ZCL Payload-Load control Event Data
 *                         (Issuer Event ID, Device Class, Utility Enrollment Group, Start Time, Duration in Minutes, Criticality Level, Cooling Temperature,
 *                          Heating Temperature Offset, Cooling Temperature Set Point, Heating Temperature Set Point, Average Load Adjustment Percentage, Duty Cycle,
 *                          Duty Cycle Event Control |
 *
 *************************************************************************************************************************
 * Other Frames
 *
 * Frame Type is "Device Authenticated Indicator (0xa2)"
 * Frame-specific Data = | Frame Type | 64-bit Source Address | Source 16-bit Address | Status |
 *
 * Frame Type is "Register Joining Device Status (0xa4)"
 * Frame-specific Data = | Frame Type | 64-bit Source Address | Source 16-bit Address | Status |
 *
 * Frame Type is "Register Joining Device (0x24)"
 * Frame-specific Data = | Frame Type | 64-bit Source Address | Source 16-bit Address | Options |
 */

/*
 *  XBee command reference tables
 * <Addressing>
 * DH : Destination Address High
 * DL : Destination Address Low
 * MY : 16-bit Network Address
 * MP : 16-bit Parent Network Address
 * NC : Number of Remaining Children
 * SH : Serial Number High
 * SL : Serial Number Low
 * NI : Node Identifer
 * SE : Source Endpoint
 * DE : Destination Endpoint
 * CI : Cluster Identifier
 * TO : Transmit Options
 * NP : Maximum RF Payload Bytes
 * DD : Device Type Identifer
 * CR : Conflict Report
 *
 * <Networking>
 * CH : Operating Channel
 * DA : Force Disassociation
 * ID : Extended PAN ID
 * OP : Operating Extended PAN ID
 * NH : Maximum Unicast Hops
 * BH : Broadcast Hops
 * OI : Operating 16-bit PAN ID
 * NT : Node Discovery Timeout
 * NO : Network Discovery Options
 * SC : Scan Channels
 * SD : Scan Duration
 * ZS : ZigBee Stack Profile
 * NJ : Node Join Time
 * JV : Channel Verification
 * NW : Network Watchdog Timeout
 * JN : Join Notification
 * AR : Aggregate Routing Notification
 * DJ : Disable Joining
 * II : Initial ID
 *
 * <Security>
 * EE : Encryption Enable
 * EO : Encryption Options
 * NK : Network Encryption Key
 * KY : Link Key
 *
 * <RF interfacing>
 * PL : Power Level
 * PM : Power Mode
 * DB : Received Signal Strength
 * PP : Peak Power
 *
 * <Serial interfacing I/O>
 * AP : API Enable
 * AO : API Options
 * BD : Interface Data Rate
 * NB : Serial Parity
 * SB : Stop Bits
 * RO : Packetization Timeout
 * D7 : DIO7 Configuration
 * D6 : DIO6 configuration
 *
 * <I/O>
 * IR : IO Sample Rate
 * IC : IO Digital Change Detection
 * P0 : PWM0 Configuration
 * P1 : DIO11 Configuration
 * P2 : DIO12 Configuration
 * P3 : DIO13 Configuration
 * D0 : AD0/DIO0 Configuration
 * D1 : AD1/DIO1 Configuration
 * D2 : AD2/DIO2 Configuration
 * D3 : AD3/DIO3 Configuration
 * D4 : DIO4 Configuration
 * D5 : DIO5 Configuration
 * D8 : DIO8 Configuration
 * LT : Assoc LED Blink Time
 * PR : Pull-up Resistor
 * RP : RSSI PWM Timer
 * %V : Supply Voltage
 * V+ : Voltage Supply Monitoring
 * TP : Temparature
 *
 * <Diagnostics>
 * VR : Firmware Version
 * HV : Hardware Version
 * AI : Association Indication
 *
 * <AT command options>
 * CT : Command Mode Timeout
 * CN : Exit Command Mode
 * GT : Guard Times
 * CC : Command Sequence Character
 *
 * <Sleep>
 * SM : Sleep Mode
 * SN : Number of Sleep Periods
 * SP : Sleep Period
 * ST : Time Before Sleep
 * SO : Sleep Options
 * WH : Wake Host
 * SI : Sleep Immediately
 * PO : Polling Rate
 *
 * <Execution>
 * AC : Apply Changes
 * WR : Write
 * RE : Restore Defaults
 * FR : Software Reset
 * NR : Network Reset
 * SI : Sleep Immediately
 * CB : Commissioning Pushbutton
 * ND : Node Discover
 * DN : Destination Node
 * IS : Force Sample
 * 1S : XBee Sensor Sample
 * AS : Active Scan
 */
/*
ID  PAN ID                        
Range: 0x0 - 0xFFFFFFFFFFFFFFFF (Default: 0)  
Set the PAN (Personal Area Network) ID the device should join (ZigBee extended PAN ID). 
Valid range is 0 - 0xFFFFFFFFFFFFFFFF. Alternatively, set ID=0 (default) for the device to join any PAN ID.

SC  Scan Channels                 
Range: 0x1 - 0x7FFF (Default: 7FFFF)          
Set/read list of channels to scan (active and energy scans) when forming a PAN as bitfield. 
Scans are initiated during coordinator startup: Bit 15 = Chan 0x1A . . . Bit 0 = Chan 0x0B

SD  Scan Duration                 
Range: 0x0 - 0x07 (Default: 3)                
Set/read the Scan Duration exponent. 
The exponent configures the duration of the active scan (PAN scan) on each channel in the SC channel mask when attempting to join a PAN. 
Scan Time = SC * (2 ^ SD) * 15.36ms. (SC=# channels)

ZS  ZigBee Stack Profile          
Range: 0x0 - 0x2 (Default: 0)                 
Set/read the ZigBee stack profile setting. 0=Network Specific, 1=ZigBee-2006, 2=ZigBee-PRO

NJ  Node Join Time                
Range: 0x0 - 0xFF (Default: FF) 
Set/read the network watchdog timeout.
If set to a non-zero value, the network watchdog timer is enabled on a router.
The router will leave the network if is does not receive valid communication within (3 * NW) minutes.
The timer is reset each time data is received from or sent to a coordinator, or if a many-to-one broadcast is received.

NW  Network Watchdog Timeout
Range: 0x0 - 0x64FF (Dafault: 0) 
Set/read the channel verification setting.
If enabled, a router will verify a coordinator exists on the same channel after joining or power cycling to ensure it is operating on a valid channel, 
and will leave if a coordinator cannot be found (if NJ=0xFF). If disabled, the router will remain on the same channel through power cycles.

JV  Channel Verification          
Disabled 0  Enabled 1  (Default: 0)
Set/read the join notification setting.
If enabled, the module will transmit a broadcast node identification frame on power up and when joining.
This action blinks the Assoc LED rapidly on all devices that receive the data, and sends an API frame out the UART of API devices.
This function should be disabled for large networks.

JN  Join Notification
Disabled 0  Enabled 1  (Default: 0)
Read the operating PAN ID (ZigBee extended PAN ID).

OP  Operating PAN ID
Read-only parameter
Read the operating PAN ID(ZigBee extended PAN ID)

OI  Operating 16-bit PAN ID
Read-only parameter
Read the 16-bit operating PAN ID

CH  Operating Channel
Read-only parameter    
Read the operating channel number (Uses 802.15.4 channel numbers).

NC  Number of Remaining Children  
Read-only parameter
Read the number of remaining end device children that can join this device.
If NC=0, the device cannot allow any more end device children to join.

SH  Serial Number High           
Read-only parameter                          
Read high 32 bits of modems unique IEEE 64-bit Extended Address.

SL  Serial Number Low             
Read-only parameter                           
Read low 32 bits of modems unique IEEE 64-bit Extended Address.

MY  16-bit Network Address        
Read-only parameter 
Read the 16 bit Network Address for the modem. 
0xFFFE means the device has not joined a PAN.

DH  Destination Address High      
Range: 0x0 - 0xFFFFFFFF (Default: 0)
Set/read the upper 32 bits of the 64 bit destination extended address.
0x000000000000FFFF is the broadcast address for the PAN.
0x0000000000000000 can be used to address the Pan Coordinator.

HL  Destination Address Low       
Range: 0x0 - oxFFFFFFFF (Default: 0)
Set/read the lower 32 bits of the 64 bit destination extended address.
0x000000000000FFFF is the broadcast address for the PAN.
0x0000000000000000 can be used to address the Pan Coordinator.

NI  Node Identifier
0 - 20 ASCII characters (Default: '')
Set/read Node Identifier string

NH  Maximum Hops
Range: 0x0 - 0xFF (Default: 1e)
Set/read the maximum hops limit.
This limit sets the maximum number of broadcast hops (BH) and determines the unicast timeout (unicast timeout = (50 * NH) + 100).
A unicast timeout of 1.6 seconds (NH=30) is enough time for the data and acknowledgment to traverse about 8 hops.

BH  Broadcast Radius             
Range: 0x0 - 0x1E (Default: 0)                
Set/Read the transmission radius for broadcast data transmissions. Set to 0 for maximum radius.

AR  Many-to-One Route Broadcast Time
Range: 0x0 - 0xFF (Default: FF)
Set/read the time between aggregation route broadcast times.
An aggregation route broadcast creates a route on all devices in the network back to the device that sends the aggregation broadcast. 
Setting AR to 0xFF disables aggregation route broadcasting. Setting AR to 0 sends one broadcast.

DD Device Type Identifier
Range: 0x - 0xFFFFFFFF (Default: 30000)
Set/read the device type identifier value. This can be used to differentiate multiple XBee-based products.
*/


#ifndef ____ev86XBee__
#define ____ev86XBee__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "XBee.h"

// new API id constants
#define CREATE_SOURCE_ROUTE 0x21
#define XBEE_SENSOR_READ_INDICATOR 0x94
#define OVER_THE_AIR_FIRMWARE_UPDATE_STATUS 0xa0
#define ROUTE_RECORD_INDICATOR 0xa1
#define ZB_MANY_TO_ONE_ROUTE_REQUEST_INDICATOR 0xA3

// new TX Status constants
#define MAC_ACK_FAILURE 0x01
#define BROADCAST_SOURCE_FAILED_TO_HEAR_A_NEIGHBOR_RELAY_THE_MESSAGE 0x26
#define INVALID_BINDING_TABLE_INDEX 0x2b
#define RESOURCE_ERROR_LACK_OF_FREE_BUFFERS_TIMERS_AND_SO_FORTH 0x2c
#define ATTEMPTED_BROADCAST_WITH_APS_TRANSMISSION 0x2d
#define ATTEMPTED_UNICAST_WITH_APS_TRANSMISSION_BUT_EE 0x2e
#define RESOURCE_EEROR_LACK_OF_FREE_BUFFERS_TIMERS_AND_SO_FORTH 0x32

// new modem Status constants
#define NETWORK_SECURITY_KEY_WAS_UPDATED 7
#define VALTAGE_SUPPLY_LIMIT_EXCEEDED 0x0D
#define MODEM_CONFIGURATION_CHANGED_WHILE_JOIN_IN_PROGRESS 0x11
#define STACK_ERROR 0x80 // 0x80+ ???

// EV86XBeeの基底クラス
class EV86XBee {       
protected:
    // Lists of AT commands.
    // Addressing
    uint8_t dhCmd[2] = {'D','H'};            // Destination Address High (16-bit)
    uint8_t dlCmd[2] = {'D','L'};            // Destination Address Low (16-bit)
    uint8_t myCmd[2] = {'M','Y'};            // 16-bit Network Address
    uint8_t mpCmd[2] = {'M','P'};            // 16-bit Parent Network Address
    uint8_t ncCmd[2] = {'N','C'};            // Number of Remaining Children
    uint8_t shCmd[2] = {'S','H'};            // Serial Number High on (unique 64-bit) such as Mac Address
    uint8_t slCmd[2] = {'S','L'};            // Serial Number Low on (unique 64-bit) such as Mac Address
    uint8_t niCmd[2] = {'N','I'};            // Node Identifer
    uint8_t seCmd[2] = {'S','E'};            // Source Endpoint
    uint8_t deCmd[2] = {'D','E'};            // Destination Endpoint
    uint8_t ciCmd[2] = {'C','I'};            // Cluster Identifier
    uint8_t toCmd[2] = {'T','O'};            // Transmit Options
    uint8_t npCmd[2] = {'N','P'};            // Maximum RF Payload Bytes
    uint8_t ddCmd[2] = {'D','D'};            // Device Type Identifer
    uint8_t crCmd[2] = {'C','R'};            // Conflict Report
    
    // Networking
    uint8_t chCmd[2] = {'C','H'};            // Operating Channel
    uint8_t daCmd[2] = {'D','A'};            // Force Disassociation
    uint8_t idCmd[2] = {'I','D'};            // Extended PAN ID
    uint8_t opCmd[2] = {'O','P'};            // Operating Extended PAN ID
    uint8_t nhCmd[2] = {'N','H'};            // Maximum Unicast Hops
    uint8_t boCmd[2] = {'B','O'};            // Broadcast Hops
    uint8_t oiCmd[2] = {'O','I'};            // Operating 16-bit PAN ID
    uint8_t ntCmd[2] = {'N','T'};            // Node Discovery Timeout
    uint8_t noCmd[2] = {'N','O'};            // Network Discovery Options
    uint8_t scCmd[2] = {'S','C'};            // Scan Channels
    uint8_t sdCmd[2] = {'S','D'};            // Scan Duration
    uint8_t zsCmd[2] = {'Z','S'};            // ZigBee Stack Profile
    uint8_t njCmd[2] = {'N','J'};            // Node Join Time
    uint8_t jvCmd[2] = {'J','V'};            // Channel Verification
    uint8_t nwCmd[2] = {'N','W'};            // Network Watchdog Timeout
    uint8_t jnCmd[2] = {'J','N'};            // Join Notification
    uint8_t arCmd[2] = {'A','R'};            // Aggregate Routing Notification
    uint8_t djCmd[2] = {'D','J'};            // Disable Joining
    uint8_t iiCmd[2] = {'I','I'};            // Initial ID
   
    // Security
    uint8_t eeCmd[2] = {'E','E'};            // Encryption Enable
    uint8_t eoCmd[2] = {'E','O'};            // Encryption Options
    uint8_t nkCmd[2] = {'N','K'};            // Network Encryption Key
    uint8_t kyCmd[2] = {'K','Y'};            // Link Key
    
    // RF Interfacing
    uint8_t plCmd[2] = {'P','L'};            // Power Level
    uint8_t pmCmd[2] = {'P','M'};            // Power Mode
    uint8_t dbCmd[2] = {'D','B'};            // Received Signal Strength (RSSI)
    uint8_t ppCmd[2] = {'P','P'};            // Peak Power
    
    // Serial interfacing I/O
    uint8_t apCmd[2] = {'A','P'};            // API Enable
    uint8_t aoCmd[2] = {'A','O'};            // API Options
    uint8_t bdCmd[2] = {'B','D'};            // Interface Data Rate
    uint8_t nbCmd[2] = {'N','B'};            // Serial Parity
    uint8_t sbCmd[2] = {'S','B'};            // Stop Bits
    uint8_t roCmd[2] = {'R','O'};            // Packetization Timeout
    uint8_t d7Cmd[2] = {'D','7'};            // DIO7 Configuration
    uint8_t d6Cmd[2] = {'D','6'};            // DIO6 configuration
    
    // I/O
    uint8_t irCmd[2] = {'I','R'};            // IO Sample Rate
    uint8_t icCmd[2] = {'I','C'};            // IO Digital Change Detection
    uint8_t p0Cmd[2] = {'P','0'};            // PWM0 Configuration
    uint8_t p1Cmd[2] = {'P','1'};            // DIO11 Configuration
    uint8_t p2Cmd[2] = {'P','2'};            // DIO12 Configuration
    uint8_t p3Cmd[2] = {'P','3'};            // DIO13 Configuration
    uint8_t d0Cmd[2] = {'D','0'};            // AD0/DIO0 Configuration
    uint8_t d1Cmd[2] = {'D','1'};            // AD1/DIO1 Configuration
    uint8_t d2Cmd[2] = {'D','2'};            // AD2/DIO2 Configuration
    uint8_t d3Cmd[2] = {'D','3'};            // AD3/DIO3 Configuration
    uint8_t d4Cmd[2] = {'D','4'};            // DIO4 Configuration
    uint8_t d5Cmd[2] = {'D','5'};            // DIO5 Configuration
    uint8_t d8Cmd[2] = {'D','8'};            // DIO8 Configuration
    uint8_t ltCmd[2] = {'L','T'};            // Assoc LED Blink Time
    uint8_t prCmd[2] = {'P','R'};            // Pull-up Resistor
    uint8_t rpCmd[2] = {'R','P'};            // RSSI PWM Timer
    uint8_t svCmd[2] = {'%','V'};            // Supply Voltage
    uint8_t vmCmd[2] = {'V','+'};            // Voltage Supply Monitoring
    uint8_t tpCmd[2] = {'T','P'};            // Temparature
    
    // Diagnostics
    uint8_t vrCmd[2] = {'V','R'};            // Firmware Version
    uint8_t hvCmd[2] = {'H','V'};            // Hardware Version
    uint8_t aiCmd[2] = {'A','I'};            // Association Indication
    
    // AT command options
    uint8_t ctCmd[2] = {'C','T'};            // Command Mode Timeout
    uint8_t cnCmd[2] = {'C','N'};            // Exit Command Mode
    uint8_t gtCmd[2] = {'G','T'};            // Guard Times
    uint8_t ccCmd[2] = {'C','C'};            // Command Sequence Character
    
    // Sleep
    uint8_t smCmd[2] = {'S','M'};            // Sleep Mode
    uint8_t snCmd[2] = {'S','N'};            // Number of Sleep Periods
    uint8_t spCmd[2] = {'S','P'};            // Sleep Period
    uint8_t stCmd[2] = {'S','T'};            // Time Before Sleep
    uint8_t soCmd[2] = {'S','O'};            // Sleep Options
    uint8_t whCmd[2] = {'W','H'};            // Wake Host
    uint8_t siCmd[2] = {'S','I'};            // Sleep Immediately
    uint8_t poCmd[2] = {'P','O'};            // Polling Rate
    
    // Execution
    uint8_t acCmd[2] = {'A','C'};            // Apply Changes
    uint8_t wrCmd[2] = {'W','R'};            // WR : Write
    uint8_t reCmd[2] = {'R','E'};            // Restore Defaults
    uint8_t frCmd[2] = {'F','R'};            // Software Reset
    uint8_t nrCmd[2] = {'N','R'};            // Network Reset
    uint8_t cbCmd[2] = {'C','B'};            // Commissioning Pushbutton
    uint8_t ndCmd[2] = {'N','D'};            // Node Discover
    uint8_t dnCmd[2] = {'D','N'};            // Destination Node
    uint8_t isCmd[2] = {'I','S'};            // Force Sample
    uint8_t xsCmd[2] = {'1','S'};            // XBee Sensor  
  
protected:
    XBee _xbee;                               // XBeeインスタンス宣言
    XBeeAddress64 _dstAdd64;                  // 宛先アドレスのインスタンス宣言
    XBeeAddress64 _srcAdd64;                  // 宛名アドレスのインスタンス宣言
    ZBTxRequest _zbTxRequest;                 // データ送信用のインスタンス宣言
    ZBRxResponse _zbRxResponse;               // データ受信用のインスタンス宣言
    ZBTxStatusResponse _zbTxStatusResponse;   // データ送信完了チェックのインスタンス宣言
    AtCommandRequest _atRequest;              // ホストXBeeへのATコマンドリクエストのインスタンス宣言
    AtCommandResponse _atResponse;            // ホストXBeeからのATコマンドレスポンスのインスタンス宣言
    RemoteAtCommandRequest _rmAtRequest;      // リモートXBeeへのATコマンドリクエストのインスタンス宣言
    RemoteAtCommandResponse _rmAtResponse;    // リモートXBeeからのATコマンドレスポンスのインスタンス宣言
    ModemStatusResponse _modemStatus;         // ホストXBeeの状態を表すインスタンス宣言
           
protected:
    String get_data; // 受信データ
             
protected:
    EV86XBee();                                                      // コンストラクタ
    void getATCommand(uint8_t* sendCmd, uint8_t* cmdValue);          // host向けATコマンド発行メソッド
    void getRemoteATCommand(uint8_t* sendCmd, uint8_t* cmdValue);    // remote向けATコマンド発行メソッド
    
    
public:
    void begin(Stream &serial);                            // ArduinoとXBee間のSerialを使って初期化
    void bufFlush();                                       // ホストXBeeの内部受信バッファをフラッシュする
    void setDstAdd64(uint32_t msbAdd, uint32_t lsbAdd);    // 宛先ノード(アドレス)の指定
    void hsXBeeInit();                                     // ホストXBeeの初期設定用メソッド
    void hsXBeeStatus();                                   // ホストXBeeの設定確認用メソッド
    void rmXBeeInit();                                     // リモートXBeeの初期設定用メソッド
    void rmXBeeStatus();                                   // リモートXBeeの設定確認用メソッド
    boolean getPacket();                                   // データ受信用の基底メソッド Coordinator flag:0 Router,EndDevice flag:1
    void sendData(String str);                             // データ送信用の基底メソッド
    String getData();                                      // 受信したデータの確認
    void clearData();                                      // 受信データを初期化する
    void stopRequest(String _stop);                        // リモートXBeeへ送信停止要求メソッドを出すメソッド
    boolean checkData(String request);  // 受信データがリクエストかチェック
};



/*                 Coordinator                    */

// EV86XBeeのCoordinatorクラス
class EV86XBeeC : public EV86XBee {
public:
    EV86XBeeC();
    void sendRequest(String request);                // センサーデータのリクエスト用メソッド
};



/*                 Router                    */

// EV86XBeeのRouterクラス
class EV86XBeeR : public EV86XBee {  
public:
    EV86XBeeR();
    
    //void sendSensorData(String send_buf);              // レスポンスデータのチェック
};


#endif /* defined(____ev86XBee__) */
