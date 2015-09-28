/*
 * 
 *  serialBridge board definition file
 *  
 *  
    boardOne connects to RT5350 openwrt and has:
     rele[0,1] on pins [9,8] [!on9] [!on13]
     10K termoresistors on pins - [!termo] [!termo1] --- termo reports bad temp...
     rf_recv - receiver 433/315 SET TO 0 for pin#2
     rf_send - transmitter - no, manly set it to 10

     serial_debug (RX,TX)  - no   - for software serial debugging ports

     13.508K - RF+TERMO

*/

#define boardName    "boardOne"

#define bridgeSpeed 57600/2 /* RT5350 normal speed is 57600, but need setup 57600/2 for 8Mhz internal proc */

#define  relay_pins    9,8
#define  rf_recv       0 /* 0 is interupt number for pin2 */ 
#define  termo         A0,A1 /* 14,15 /* = A0,A1 - termoresistors [!term0] */



