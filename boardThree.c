
/*
 serialBridge for nexx3020 (its based on mt76xx so tolerant to 5v serial)
  serial: 57600 on /dev/ttyS0
*/

#define boardName    "boardTree"

//[!nxt13][!nxt13] [!off13]

#define bridgeSpeed 57600 /* RT5350 normal speed is 57600, but need setup 57600/2 for 8Mhz internal proc */

//#define dht11 10 /* dht on 10 pin [!dht] temp humidity */

//#define thermo_pulldown 1
#define  termo         A6,A7 /* 14,15 /* = A0,A1 - termoresistors [!term0] */

