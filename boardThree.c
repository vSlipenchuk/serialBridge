
/*
 serialBridge for nexx3020 (its based on mt76xx so tolerant to 5v serial)
  serial: 57600 on /dev/ttyS0
*/

#define boardName    "boardTree"

//[!nxt13][!nxt13] [!off13]
//[!now] [!dt=20051112 051223]
//[!lcd00--first line ---] [!lcd51--secondXXX_____]

#define bridgeSpeed 57600 /* RT5350 normal speed is 57600, but need setup 57600/2 for 8Mhz internal proc */

//#define dht11 10 /* dht on 10 pin [!dht] temp humidity */

//#define thermo_pulldown 1
#define  termo         A6,A7 /* 14,15 /* = A0,A1 - termoresistors [!term0] */

#define rtc_DS1307 /* if we need RTC (now,dt=)  functions */

//#define lcd_HD44780 4,5,6,7,8,9 /* where LiquidCrystal pins RS,E,D4,D5,D6,D7 connected */
#define lcd_HD44780 A2,A1,A0,13,12,11 /* where LiquidCrystal pins RS,E,D4,D5,D6,D7 connected */

#define lcd_size 16,2 /* 16,2 - default */

// size 13.808 with lcd

#define ir_pin A3   /* need IRRemote.h from https://github.com/z3t0/Arduino-IRremote/releases/tag/stable and remove std RobotIRemote lib */
// size 20435   [!hello] for start recv
