#define SERIAL_COMMAND
#define USE_RTC
#define _PWM_PIN_15
#define _useTimer2

#include <avr/pgmspace.h>
#include <SD.h>
#include <OneWire.h>
#include <LedControl.h>
#include <Wire.h>
#include "AnyPCPWM.h"

#ifdef USE_RTC
#include <RTClib.h>
#endif

#ifdef SERIAL_COMMAND
#include <SerialCommand.h>
#endif

#define TS_ON 0x80
#define TS_ERROR 0x40
#define TS_EDIT 0x20
#define EEPROM_CFG 0x50
#define TSCFG_START_ADDR 10
#define INTENSITY_ADDR 0
#define ANTIFREEZE_TEMP_ADDR 1
#define PUMP_OFF_DELAY_ADDR 2
#define MAX_OUT_TEMP_ADDR 3

#define TEMP_ERROR -100
#define PUMP_OFF_DELAY 60

#define EXT_OUT_1 0
#define EXT_OUT_2 1
#define EXT_OUT_3 2
#define EXT_OUT_4 3
#define EXT_OUT_5 4
#define EXT_OUT_6 5
#define EXT_OUT_7 6
#define EXT_OUT_8 7

// занятые ноги
#define A0P 15
#define EXT_CS_IN 14
// 13
// 12
// 11
#define SD_CS_PIN 10
#define TEMP_SENSOR_PIN 9
#define BOILER_RELAY EXT_OUT_1
#define PUMP_RELAY EXT_OUT_2
#define EXT_CS 8
#define EXT_CLOCK 7
#define EXT_DATA_OUT 6
#define EXT_DATA_IN 5
#define LED_DATA_PIN 4 //MOSI
#define LED_CS_PIN 3   //CS
#define LED_CLOCK_PIN 2 //CLK


#define TX 1
#define RX 0

#define BOILER_ON 0x01
#define PUMP_ON 0x02
#define TUBE_POWER_ON 0x04

#define BOILER_TIMER_ON 0x04

#define INPUT_PS_MODE 0x01
#define BOILER_BUTTON_ON 0x02
#define INPUT_EDIT_MODE 0x04
#define INPUT_PLUS 0x08
#define INPUT_MINUS 0x20
#define BOILER_TERMOSTAT_ON 0x40

#define NO_VALUE 0
#define FORCE 1
#define FROM_BTN 2
#define FROM_TRMS 3
#define FROM_ANTIFREEZE 4
#define WATER_OVERTEMP 5
#define BOILER_TIMER 6

#define MAX_OUT_TEMP 75




#define temp9bit

#ifdef temp12bit
#define temp_accuracy 625 / 1000
#define temp_config 0x60
#define temp_delay 800
#undef temp11bit
#undef temp10bit
#undef temp9bit
#endif

#ifdef temp11bit
#define temp_accuracy 125 / 1000
#define temp_config 0x40
#define temp_delay 376
#undef temp12bit
#undef temp10bit
#undef temp9bit
#endif

#ifdef temp10bit
#define temp_accuracy 25 / 100
#define temp_config 0x20
#define temp_delay 188
#undef temp12bit
#undef temp11bit
#undef temp9bit
#endif

#ifdef temp9bit
#define temp_accuracy 5 / 10
#define temp_config 0x10
#define temp_delay 94
#undef temp12bit
#undef temp11bit
#undef temp10bit
#endif


#define MAX_TEMP_SENSOR 4
#define WATER_OUT_DISP 0
#define WATER_IN_DISP 1
#define ANTIFREEZE_DISP 3
#define OUT_DOOR_DISP 2


#define cstrFreeMemory				0
#define cstrLoadAndInit				1
#define cstrLedInitAndTest			2
#define cstrRTCIsNotRunning			3
#define cstrSearchTemperatureSensor 4
#define cstrNoDSChip				5
#define cstrROM						6
#define cstrFounded					7
#define cstrError					8
#define cstrOK						9
#define cstrFAILED					10
#define cstrLoadTSConfig			11
#define cstrBOILER					12
#define cstrPUMP					13
#define cstrON						14
#define cstrOFF						15
#define cstrInitSD					16
#define cstrBOILER_TIMER			17

prog_char str_FreeMemory[] PROGMEM = "F:";
prog_char str_LoadAndInit[] PROGMEM = "INIT";
prog_char str_LedInitAndTest[] PROGMEM = "INIT_LED";
prog_char str_RTCIsNotRunning[] PROGMEM = "RTC_F";
prog_char str_SearchTemperatureSensor[] PROGMEM = "INIT_TS_SENSOR";
prog_char str_NoDSChip[] PROGMEM = "UNKDEV";
prog_char str_ROM[] PROGMEM = "ROM=";
prog_char str_Founded[] PROGMEM = "FOUNDED:";
prog_char str_Error[] PROGMEM = "ERROR\n.";
prog_char str_OK[] PROGMEM = "OK";
prog_char str_FAILED[] PROGMEM = "FAILED";
prog_char str_LoadTSConfig[] PROGMEM = "LD_TS_CFG";
prog_char str_BOILER[] PROGMEM = "BLR";
prog_char str_PUMP[] PROGMEM = "PUMP";
prog_char str_ON[] PROGMEM = "ON";
prog_char str_OFF[] PROGMEM = "OFF";
prog_char str_InitSD[] PROGMEM = "INIT_SD";
prog_char str_BOILER_TIMER[] PROGMEM = "BL_TMR";

PROGMEM const char *string_table[] = {
	str_FreeMemory,
	str_LoadAndInit,
	str_LedInitAndTest,
	str_RTCIsNotRunning,
	str_SearchTemperatureSensor,
	str_NoDSChip,
	str_ROM,
	str_Founded,
	str_Error,
	str_OK,
	str_FAILED,
	str_LoadTSConfig,
	str_BOILER,
	str_PUMP,
	str_ON,
	str_OFF,
	str_InitSD,
	str_BOILER_TIMER
};

const char cpoint = '.';
const char ccomma = ',';
const char csemicolon = ';';
const char cspace = ' ';
const char ccolon = ':';
const char cE = 'E';
const char cminus = '-';
const char cstar = '*';

OneWire  ds(TEMP_SENSOR_PIN);  // on pin 10 (a 4.7K resistor is necessary)
LedControl lc=LedControl(LED_DATA_PIN, LED_CLOCK_PIN, LED_CS_PIN, 3);

#ifdef USE_RTC
RTC_DS1307 rtc;
#endif

#ifdef SERIAL_COMMAND
SerialCommand SCmd;
#endif

#if(MAX_TEMP_SENSOR == 4)
const byte cTempDisplay[MAX_TEMP_SENSOR] = {0x08, 0x04, 0x18, 0x14};
#else
const byte cTempDisplay[16] = {0x08, 0x04, 0x18, 0x14, 0x28, 0x24, 0x38, 0x34, 0x48, 0x44, 0x58, 0x54, 0x68, 0x64, 0x78, 0x74};
#endif

typedef struct _temp_sensor {
	uint8_t addr[8];
	byte  disp;
	byte  flags;
	char temp[3];
};

_temp_sensor temp_sensor[MAX_TEMP_SENSOR];
byte ts_count = 0;
byte relay_state;
byte boiler_state = 0;
byte in_state = 0;
byte antifreeze_temp;
byte max_water_temp = MAX_OUT_TEMP;
byte boiler_on_mode = NO_VALUE;
byte boiler_off_mode = NO_VALUE;
byte edit_mode = 0;

boolean boiler_force_on = false;

boolean power_save_mode = false;
unsigned long boiler_timer;
short pump_off_delay = 0;

#ifdef USE_RTC
DateTime now;
#endif

void SerialPrint(int cStr){
	char buffer[30];
	strcpy_P(buffer, (char*)pgm_read_word(&(string_table[cStr])));
	Serial.print(buffer);
}

void SerialPrintln(int cStr){
	char buffer[30];
	strcpy_P(buffer, (char*)pgm_read_word(&(string_table[cStr])));
	Serial.println(buffer);
}


void clear_indicator(byte disp, byte ind) {
	for (byte i=0; i<8; i++) {
		lc.setLed(disp, ind, i, false);
	}
}

void set_led_point(boolean on) {
	for (byte i=0; i < ts_count; i++) {
		lc.setLed(temp_sensor[i].disp >> 4, (temp_sensor[i].disp) - 4 & 0x0F, 0, on);
	}
}

void i2c_eeprom_write_byte( int deviceaddress, unsigned int eeaddress, byte data ) {
	int rdata = data;
	Wire.beginTransmission(deviceaddress);
	Wire.write((int)(eeaddress >> 8)); // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.write(rdata);
	Wire.endTransmission();
	delay(10);
}

// WARNING: address is a page address, 6-bit end will wrap around
// also, data can be maximum of about 30 bytes, because the Wire library has a buffer of 32 bytes
void i2c_eeprom_write_buffer( int deviceaddress, unsigned int eeaddresspage, byte* data, byte length ) {
	Wire.beginTransmission(deviceaddress);
	Wire.write((int)(eeaddresspage >> 8)); // MSB
	Wire.write((int)(eeaddresspage & 0xFF)); // LSB
	byte c;
	for ( c = 0; c < length; c++) {
		Wire.write(data[c]);
		delay(10);
	}
	Wire.endTransmission();
	delay(10);
}

byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress ) {
	byte rdata = 0xFF;
	Wire.beginTransmission(deviceaddress);
	Wire.write((int)(eeaddress >> 8)); // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.endTransmission();
	Wire.requestFrom(deviceaddress,1);
	if (Wire.available()) rdata = Wire.read();
	return rdata;
}

// maybe let's not read more than 30 or 32 bytes at a time!
void i2c_eeprom_read_buffer( int deviceaddress, unsigned int eeaddress, byte *buffer, int length ) {
	Wire.beginTransmission(deviceaddress);
	Wire.write((int)(eeaddress >> 8)); // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.endTransmission();
	Wire.requestFrom(deviceaddress,length);
	int c = 0;
	for ( c = 0; c < length; c++ )
	if (Wire.available()) buffer[c] = Wire.read();
}

void clear_led() {
	int devices=lc.getDeviceCount();
	for(int address=0;address<devices;address++) {
		lc.clearDisplay(address);
	}
}

void Serial_print_buffer(byte *buf, int sz) {
	for (int i=0; i < sz; i++) {
		Serial.print(buf[i], HEX);
	}
	Serial.println();
}

void save_config(unsigned int startaddr, byte *buffer, int sz) {
	for (int i=0; i < sz; i++) {
		i2c_eeprom_write_byte(EEPROM_CFG, startaddr+i, buffer[i]);
	}
}

void load_config(unsigned int startaddr, byte *buffer, int sz) {
	for (int i=0; i <sz; i++) {
		buffer[i] = i2c_eeprom_read_byte(EEPROM_CFG, startaddr+i);
	}
}


void init_and_test_led(byte intensity) {
	int devices=lc.getDeviceCount();
	//we have to init all devices in a loop
	for(int address=0;address<devices;address++) {
		/*The MAX72XX is in power-saving mode on startup*/
		lc.shutdown(address,false);
		/* Set the brightness to a medium values */
		lc.setIntensity(address, intensity);
		/* and clear the display */
		lc.clearDisplay(address);
	}
	
	for(int row=0;row<8;row++) {
		for(int col=0;col<8;col++) {
			for(int address=0;address<devices;address++) {
				lc.setLed(address,row,col,true);
			}
		}
	}
	clear_led();
}



void search_ds() {
	uint8_t addr[8];
	byte type_s;
	byte k = 0;

	SerialPrintln(cstrSearchTemperatureSensor);
	while (ds.search(addr)) {
		
		if (OneWire::crc8(addr, 7) == addr[7]) {
			switch (addr[0]) {
				case 0x10:
				//				SerialPrint(cstrChip);  // or old DS1820
				//				SerialPrintln(cstrChip18S20);
				type_s = 2;
				break;
				case 0x28:
				//				SerialPrint(cstrChip);  // or old DS1820
				//				SerialPrintln(cstrChip18B20);
				type_s = 1;
				break;
				case 0x22:
				//				SerialPrint(cstrChip);  // or old DS1820
				//				SerialPrintln(cstrChip1822);
				type_s = 1;
				break;
				default:
				type_s = 0;
				SerialPrintln(cstrNoDSChip);
				return;
			}
			if (type_s) {
				SerialPrint(cstrROM);
				for( int i = 0; i < 8; i++) {
					Serial.write(ccolon);
					Serial.print(addr[i], HEX);
				}
				Serial.println();
				memcpy(temp_sensor[k].addr, addr, sizeof(addr));
				temp_sensor[k].disp = cTempDisplay[k];
				temp_sensor[k].flags = TS_ON | type_s;
				temp_sensor[k].temp[2] = 0;
				ds.select(temp_sensor[k].addr);
				ds.write(0x4E);
				ds.write(temp_config);
				ds.write(0x48);
				k++;
				if (k >= MAX_TEMP_SENSOR) {
					break;
				}
			}
		}
	}
	ds.reset_search();
	SerialPrint(cstrFounded);
	Serial.println(k);
	ts_count = k;
	Serial.println(cpoint);
}


void display_temp(byte disp, char temp1, char temp2, byte flag) {
	byte displayNo = disp >> 4;
	byte indicatorNo = disp & 0x0F;
	if (flag & TS_ERROR) {
		lc.setChar(displayNo, indicatorNo-2, cE, false);
		lc.setChar(displayNo, indicatorNo-3, cE, false);
		lc.setChar(displayNo, indicatorNo-4, cE, false);
	} else
	{
		if (temp1 < 0) {
			lc.setChar(displayNo, indicatorNo-1, cminus, false);
		}
		else
		{
			clear_indicator(displayNo, indicatorNo-1);
		}
		
		lc.setDigit(displayNo, indicatorNo-2, abs(temp1)/10, false);
		lc.setDigit(displayNo, indicatorNo-3, abs(temp1)%10, true);
		lc.setDigit(displayNo, indicatorNo-4, temp2, false);
	}
	if ((flag & TS_EDIT) !=0) {
	  lc.setLed(displayNo, indicatorNo-1, 1, true);
	}
}


void get_and_display_temp() {

	byte i;
	byte present = 0;
	uint8_t addr[8];
	uint8_t data[9];
	char sign;
	set_led_point(true);
	ds.reset();
	// всем измерить температуру
	ds.write(0xCC, 1);
	ds.write(0x44, 1);
	delay(temp_delay);
	byte temp_int;
	byte temp_flt;
	
	for (int r=0; r<ts_count; r++) {
		
		present = ds.reset();
		memcpy(addr, temp_sensor[r].addr, sizeof(addr));
		
		ds.select(addr);
		ds.write(0xBE);         // Read Scratchpad
		for ( i = 0; i < 9; i++) {           // we need 9 bytes
			data[i] = ds.read();
		}
		uint8_t bcrc = OneWire::crc8(data, 8);
		if (bcrc == data[8]) {
			int16_t raw = (data[1] << 8) | data[0];
			if ((temp_sensor[r].flags & 0x0F)  == 2) {
				raw = raw << 3; // 9 bit resolution default
				if (data[7] == 0x10) {
					raw = (raw & 0xFFF0) + 12 - data[6];
				}
			} else
			if ((temp_sensor[r].flags & 0x0F) == 1) {
				byte cfg = (data[4] & 0x60);
				if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
				else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
				else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
			}
			
			
			if (raw < 0) {
				raw = ~raw + 1;
				sign = -1;
			} else
			{
				sign = 1;
			}
			temp_int = raw >> 4;
			temp_flt = (raw & 0xF) * temp_accuracy; //625 / 1000;
			temp_sensor[r].flags &= ~TS_ERROR;
		} else
		{
			temp_int = 0;
			temp_flt = 0;
			temp_sensor[r].flags |= TS_ERROR;
		}
		
		temp_sensor[r].temp[0] = (temp_int * sign) + temp_sensor[r].temp[2];
		temp_sensor[r].temp[1] = temp_flt;
		if (temp_sensor[r].temp[0] < 0) {
			temp_sensor[r].temp[1] = 9 - temp_flt;
		}
		
		if (edit_mode == 0) {			
			display_temp(temp_sensor[r].disp, temp_sensor[r].temp[0], temp_sensor[r].temp[1], temp_sensor[r].flags);
			} else {
			if (edit_mode>0 && edit_mode < 5) {
				if ((edit_mode-1) == r) {
					display_temp(temp_sensor[r].disp, temp_sensor[r].temp[2], 0, TS_EDIT);
					} else {
					display_temp(temp_sensor[r].disp, temp_sensor[r].temp[0], temp_sensor[r].temp[1], temp_sensor[r].flags);
				}
				
			} else
			switch (edit_mode) {
				case 5:display_temp(temp_sensor[ANTIFREEZE_DISP].disp, antifreeze_temp, 0, TS_EDIT);
				break;
				case 6:display_temp(temp_sensor[WATER_OUT_DISP].disp, max_water_temp, 0, TS_EDIT);
				break;
			}
		}
		
	}
	set_led_point(false);
}



char ts_by_display(byte disp) {
	for (byte i=0; i < ts_count; i++) {
		if (temp_sensor[i].disp == disp) return i;
	}
	return -1;
}

char ts_by_addr(byte *addr) {
	for (byte i=0; i<ts_count; i++) {
		if (memcmp(addr, temp_sensor[i].addr, sizeof(temp_sensor[i].addr)) == 0) return i;
	}
	return -1;
}

void save_ts_config() {
	byte ts_mem_item[10];
	for (byte i=0; i < 15; i++) {
		unsigned int memaddr = (i * sizeof(ts_mem_item)) + TSCFG_START_ADDR;
		char ts_idx = ts_by_display(cTempDisplay[i]);
		if (ts_idx > -1) {
			memcpy(ts_mem_item, temp_sensor[ts_idx].addr, sizeof(temp_sensor[ts_idx].addr));
			ts_mem_item[8] = temp_sensor[ts_idx].flags;
			ts_mem_item[9] = temp_sensor[ts_idx].temp[2];
		} else
		{
			memset(ts_mem_item, 0, sizeof(ts_mem_item));
		}
		Serial.write(cstar);
		save_config(memaddr, ts_mem_item, sizeof(ts_mem_item));
		//    i2c_eeprom_write_buffer(EEPROM_CFG, memaddr, ts_mem_item, sizeof(ts_mem_item));
	}
	Serial.println(cpoint);
}

void load_ts_config() {
	SerialPrintln(cstrLoadTSConfig);
	byte ts_mem_item[10];
	for (byte i=0; i < 15; i++) {
		unsigned int memaddr = (i * sizeof(ts_mem_item)) + TSCFG_START_ADDR;
		//    i2c_eeprom_read_buffer(EEPROM_CFG, memaddr, ts_mem_item, sizeof(ts_mem_item));
		load_config(memaddr, ts_mem_item, sizeof(ts_mem_item));
		char ts_idx = ts_by_addr(ts_mem_item);
		if (ts_idx > -1) {
			temp_sensor[ts_idx].flags = ts_mem_item[8];
			temp_sensor[ts_idx].temp[2] = ts_mem_item[9];
			char old_ts = ts_by_display(cTempDisplay[i]);
			if ((old_ts > -1) & (old_ts != ts_idx)) {
				temp_sensor[old_ts].disp = temp_sensor[ts_idx].disp;
			}
			temp_sensor[ts_idx].disp = cTempDisplay[i];
		}
		Serial.write(cstar);
	}
	Serial.println(cpoint);
}

#ifdef SERIAL_COMMAND
void set_display_light() {
	char *arg = SCmd.next();
	if (arg != NULL) {
		byte intensity = atoi(arg);
		i2c_eeprom_write_byte(EEPROM_CFG, INTENSITY_ADDR, intensity);
		int devices=lc.getDeviceCount();
		for(int address=0;address<devices;address++) {
			lc.setIntensity(address, intensity);
		}
	}
}
#endif


void save_antifreeze_temp(byte temp) {
	
	i2c_eeprom_write_byte(EEPROM_CFG, ANTIFREEZE_TEMP_ADDR, temp);
	
}

void save_max_water_temp(byte temp) {
	i2c_eeprom_write_byte(EEPROM_CFG, MAX_OUT_TEMP_ADDR, temp);
}

#ifdef SERIAL_COMMAND
void cmd_boiler() {
	char *arg = SCmd.next();
	
	if (arg != NULL) {
		if (strcmp(arg, "on") == 0) {
			arg = SCmd.next();
			byte mode = FORCE;
			if ((arg != NULL) & (!is_external_boiler_on(0))) {
				boiler_timer = atoi(arg);
				boiler_state |= BOILER_TIMER_ON;
				mode = BOILER_TIMER;
				} else {
				boiler_timer = 0;
				boiler_state &= ~BOILER_TIMER_ON;
			}
			boiler_on(mode);
		} else
		if (strcmp(arg, "maxtemp") == 0) {
			arg = SCmd.next();
			byte maxtemp;
			if (arg != NULL) {
				maxtemp = atoi(arg);
				if ((maxtemp == 0) | (maxtemp > MAX_OUT_TEMP)) maxtemp = MAX_OUT_TEMP;
				save_max_water_temp(maxtemp);
				max_water_temp = maxtemp;
			}
			maxtemp = i2c_eeprom_read_byte(EEPROM_CFG, MAX_OUT_TEMP_ADDR);
			Serial.println(maxtemp, DEC);
			Serial.println(cpoint);
			
		} else
		if (strcmp(arg, "pd") == 0) {
			arg = SCmd.next();
			byte pd;
			if (arg != NULL) {
				pd = atoi(arg);
				if (pd == 0) {
					pd = 60;
				}
				i2c_eeprom_write_byte(EEPROM_CFG, PUMP_OFF_DELAY_ADDR, pd);
			}
			pd = i2c_eeprom_read_byte(EEPROM_CFG, PUMP_OFF_DELAY_ADDR);
			Serial.println(pd, DEC);
			Serial.println(cpoint);
		} else
		if (strcmp(arg, "off") == 0) {
			//boiler_off(true);
			boiler_force_on = false;
			boiler_timer = 0;
		}
		else
		if (strcmp(arg, "at") == 0) {
			arg = SCmd.next();
			if (arg != NULL) {
				antifreeze_temp = atoi(arg);
				if (antifreeze_temp > max_water_temp) {
					antifreeze_temp = i2c_eeprom_read_byte(EEPROM_CFG, ANTIFREEZE_TEMP_ADDR);
				} else
				{
					save_antifreeze_temp(antifreeze_temp);					
				}
			}
			else
			{
				Serial.println(antifreeze_temp);
				Serial.println(cpoint);
			}
		} else
		{
			SerialPrintln(cstrError);
		}
		} else {
		printState();
	}
}
#endif

#ifdef SERIAL_COMMAND
#ifdef USE_RTC
void cmd_datetime () {
	char *arg;
	DateTime now = rtc.now();
	if (!rtc.isrunning()) {
		SerialPrintln(cstrRTCIsNotRunning);
	}
	arg = SCmd.next();
	if (strcmp(arg, "t")==0) {
		arg = SCmd.next();
		if (arg != NULL) {
			byte h = atoi(strtok_r(arg, ":", &arg));
			byte m = atoi(strtok_r(arg, ":", &arg));
			byte s = atoi(arg);
			DateTime newdate(now.year(), now.month(), now.day(), h, m, s);
			rtc.adjust(newdate);
		}
	}
	else
	if (strcmp(arg, "d") == 0) {
		arg = SCmd.next();
		if (arg != NULL) {
			byte d = atoi(strtok_r(arg, ".", &arg));
			byte m = atoi(strtok_r(arg, ".", &arg));
			uint16_t y = atoi(arg);
			DateTime newdate(y, m, d, now.hour(), now.minute(), now.second());
			rtc.adjust(newdate);
		}
	}
	
	now = rtc.now();
	Serial.print(now.hour());
	Serial.write(ccolon);
	Serial.print(now.minute());
	Serial.write(ccolon);
	Serial.print(now.second());
	Serial.write(cspace);
	Serial.print(now.day());
	Serial.print(cpoint);
	Serial.print(now.month());
	Serial.print(cpoint);
	Serial.println(now.year());
	Serial.println(cpoint);
	
}
#endif
#endif

#ifdef SERIAL_COMMAND
void list_temp_sensor(){
	byte argNum;
	char *arg;
	char *sensor;
	char *disp;
	boolean refresh_screen = false;
	
	arg = SCmd.next();
	if (arg == NULL) {
		arg = "ls";
	}
	sensor = SCmd.next();
	
	if (strcmp(arg, "ls") == 0) {
		for (byte i = 0; i < ts_count; i++) {
			Serial.print("ts");
			Serial.print(i);
			Serial.write('=');
			for (byte k = 0; k < 8; k++) {
				Serial.print(temp_sensor[i].addr[k], HEX);
			}
			Serial.write(ccomma);
			Serial.print(temp_sensor[i].temp[0], DEC);
			Serial.write(cpoint);
			Serial.print(temp_sensor[i].temp[1], DEC);
			Serial.write(ccomma);
			Serial.print(temp_sensor[i].temp[2], DEC);
			Serial.write(ccomma);
			Serial.print(temp_sensor[i].disp, HEX);
			Serial.write(ccomma);
			Serial.print(temp_sensor[i].flags, HEX);
			Serial.println(csemicolon);
		}
	} else
	if (strcmp(arg, "corr") == 0) {
		char *c = SCmd.next();
		if (c != NULL) {
			char corr = atoi(c);
			if (sensor == NULL) {
				SerialPrintln(cstrError);
				return;
			}
			byte s = atoi(sensor);
			if (s >= ts_count) {
				SerialPrintln(cstrError);
				return;
			}
			temp_sensor[s].temp[2] = corr;
			refresh_screen = true;
		}
	} else
	if (strcmp(arg, "init") == 0) {
		search_ds();
		refresh_screen = true;
	}
	if (strcmp(arg, "load") == 0) {
		load_ts_config();
		refresh_screen = true;
	} else
	if (strcmp(arg, "save") == 0){
		save_ts_config();
	} else
	if (strcmp(arg, "on") == 0) {
		if (sensor == NULL) {
			SerialPrintln(cstrError);
			return;
		}
		byte s = atoi(sensor);
		if (s >= ts_count) {
			SerialPrintln(cstrError);
			return;
		}
		temp_sensor[s].flags = temp_sensor[s].flags | TS_ON;
		refresh_screen = true;
	} else
	if (strcmp(arg, "off") == 0) {
		if (sensor == NULL) {
			SerialPrintln(cstrError);
			return;
		}
		byte s = atoi(sensor);
		if (s >= ts_count) {
			SerialPrintln(cstrError);
			return;
		}
		temp_sensor[s].flags = temp_sensor[s].flags & ~TS_ON;
		refresh_screen = true;
	}
	if (strcmp(arg, "set") == 0) {
		if (sensor == NULL) {
			SerialPrintln(cstrError);
			return;
		}
		disp = SCmd.next();
		if (disp == NULL) {
			SerialPrintln(cstrError);
			return;
		}
		byte s = atoi(sensor);
		byte d = atoi(disp);
		if (s >= ts_count) {
			SerialPrintln(cstrError);
			return;
		}
		if (d >= MAX_TEMP_SENSOR) {
			SerialPrintln(cstrError);
			return;
		}
		char oldDisp = -1;
		
		for (byte i=0; i<ts_count; i++) {
			if (temp_sensor[i].disp == cTempDisplay[d]) {
				oldDisp = i;
				break;
			}
		}
		if (oldDisp > -1) {
			temp_sensor[oldDisp].disp = temp_sensor[s].disp;
		}
		temp_sensor[s].disp = cTempDisplay[d];
		refresh_screen = true;
		//display_temp(temp_sensor[s].disp, temp_sensor[s].temp[0], temp_sensor[s].temp[1]);
	}
	if (refresh_screen) {
		clear_led();
		get_and_display_temp();
	}
}
#endif

void unrecognized()
{
	SerialPrintln(cstrError);
}

unsigned long curr_time;
unsigned long loop_time;

char get_temp_at_display(byte disp) {
	char ts = ts_by_display(cTempDisplay[disp]);
	if (ts > -1) {
		if (!(temp_sensor[ts].flags & TS_ERROR)) {
			return temp_sensor[ts].temp[0];
		}
	}
	return TEMP_ERROR;
}

void out_relay_state() {
	digitalWrite(EXT_CLOCK, LOW);
	digitalWrite(EXT_CS, LOW);
	shiftOut(EXT_DATA_OUT, EXT_CLOCK, MSBFIRST, ~relay_state);
	digitalWrite(EXT_CS, HIGH);
}

boolean set_boiler_on () {
	
	if (!is_boiler_on()) {
		relay_state |= BOILER_ON | PUMP_ON;
		boiler_state |= BOILER_ON | PUMP_ON;
	}
	out_relay_state();
}


boolean boiler_on(byte mode) {
	switch (mode) {
		case NO_VALUE: break;
		case FORCE: boiler_force_on = true;
		case BOILER_TIMER:
		case FROM_ANTIFREEZE:
		case FROM_BTN:
		case FROM_TRMS: set_boiler_on();
		break;
	}
	printState();
	boiler_on_mode = mode;
	return is_boiler_on();
}

void set_boiler_off(byte mode) {
	boiler_on_mode = NO_VALUE;
	boiler_off_mode = mode;
	boiler_force_on = false;
	relay_state &= ~BOILER_ON;
	boiler_state &= ~(BOILER_ON | BOILER_TIMER_ON);
	out_relay_state();
	boiler_timer = 0;
	pump_off_delay = i2c_eeprom_read_byte(EEPROM_CFG, PUMP_OFF_DELAY_ADDR);;
	if (pump_off_delay == 0) pump_off_delay = PUMP_OFF_DELAY;
	printState();
}

void check_boiler() {
	char ts;
	ts = get_temp_at_display(WATER_OUT_DISP);
	byte data = get_input_data();
	if (is_boiler_on()) {
		if (ts > max_water_temp) {
			set_boiler_off(WATER_OVERTEMP);
			} else {
			switch(boiler_on_mode) {
				case NO_VALUE: break;
				case FROM_BTN:
				case FROM_TRMS: if (!is_external_boiler_on(data)) {
					set_boiler_off(boiler_on_mode);
				}
				break;
				case BOILER_TIMER:
				if (boiler_timer == 0) {
					if (is_external_boiler_on(data)) {
						if (is_button_boiler_on(data)) {
							boiler_on_mode = FROM_BTN;
						} else
						boiler_on_mode = FROM_TRMS;
						
					} else   set_boiler_off(boiler_on_mode);
				}
				break;
				case FORCE: if (!boiler_force_on) {
					set_boiler_off(boiler_on_mode);
				}
				break;
				case FROM_ANTIFREEZE: ts = get_temp_at_display(ANTIFREEZE_DISP);
				if (ts > antifreeze_temp) {
					set_boiler_off(boiler_on_mode);
				}
				break;
			}
		}
		
		
		} else {
		
		if (ts > (max_water_temp - 15)) return;
		
		ts = get_temp_at_display(ANTIFREEZE_DISP);
		if ((ts > -100) & (ts < -2)) {
			boiler_on(FROM_ANTIFREEZE);
		} else
		if (is_button_boiler_on(data)) {
			boiler_on(FROM_BTN);
		} else
		if (is_termostat_boiler_on(data)) {
			boiler_on(FROM_TRMS);
		}
	}
}

void pump_off() {
	relay_state &= ~PUMP_ON;
	boiler_state &= ~PUMP_ON;
	out_relay_state();
	printState();
}

boolean is_boiler_on(){
	return (relay_state & BOILER_ON);
}

boolean is_pump_on() {
	return (relay_state & PUMP_ON) >> 1;
}

boolean is_boiler_timer_on() {
	if ((boiler_state & BOILER_TIMER_ON) != 0) {
		return true;
	} else
	return false;
}

int freeRam () {
	extern int __heap_start, *__brkval;
	int v;
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void printState() {
	SerialPrint(cstrBOILER);
	Serial.write(cspace);
	SerialPrintln(is_boiler_on() ? cstrON : cstrOFF);
	SerialPrint(cstrPUMP);
	Serial.write(cspace);
	SerialPrintln(is_pump_on() ? cstrON : cstrOFF);
	if (is_boiler_timer_on()) {
		SerialPrint(cstrBOILER_TIMER);
		Serial.write(ccolon);
		Serial.println(boiler_timer);
	}
	Serial.println(cpoint);
}


byte get_input_data() {
	digitalWrite(EXT_CS_IN, LOW);
	digitalWrite(EXT_CLOCK, LOW);
	digitalWrite(EXT_CLOCK, HIGH);
	digitalWrite(EXT_CS_IN, HIGH);
	byte data = shiftIn(EXT_DATA_IN, EXT_CLOCK, LSBFIRST);
	return data;
}

boolean is_button_boiler_on(byte state) {
	if ((state & BOILER_BUTTON_ON) != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

boolean is_termostat_boiler_on(byte state) {
	if ((state & BOILER_TERMOSTAT_ON) != 0) {
		return true;
	}  else
	{
		return false;
	}
}

boolean is_external_boiler_on(byte state) {
	if (state == 0) {
		state = get_input_data();
	}
	return is_button_boiler_on(state) | is_termostat_boiler_on(state);
}

void input_data () {
	byte new_in_state = get_input_data();
	if (new_in_state != in_state) {
		Serial.println();
		Serial.print("Old state:");
		Serial.println(in_state, DEC);
		Serial.print("New state:");
		Serial.println(new_in_state, DEC);
		
		if ((new_in_state & INPUT_PS_MODE) != 0) {
			power_save_mode = true;
		} else
		{
			power_save_mode = false;
		}
		
		if ((new_in_state & INPUT_EDIT_MODE) !=0) {
			if (edit_mode == 0) {
				edit_mode = 1;
				} else {
				edit_mode++;
			}
			if (edit_mode > 6) {
				edit_mode = 0;
				save_ts_config();
				save_antifreeze_temp(antifreeze_temp);
				save_max_water_temp(max_water_temp);
			}
		}
		if ((new_in_state & INPUT_PLUS) != 0 && (in_state & INPUT_MINUS) == 0 )   {
			switch (edit_mode) {
				case 1:
				case 2:
				case 3:
				case 4: temp_sensor[edit_mode-1].temp[2]++;
						break;
				case 5: antifreeze_temp++;
						if (antifreeze_temp > max_water_temp) antifreeze_temp = max_water_temp;
						break;
				case 6: max_water_temp++;		
						if (max_water_temp > MAX_OUT_TEMP) max_water_temp = MAX_OUT_TEMP;
						break;
			}
		  if (edit_mode)	get_and_display_temp();
		} else
		if ((new_in_state & INPUT_MINUS) != 0  && (in_state & INPUT_PLUS) == 0) {
			switch (edit_mode)	{
				case 1:
				case 2:
				case 3:
				case 4: temp_sensor[edit_mode - 1].temp[2]--;				
						break;
				case 5: antifreeze_temp--;
						if (antifreeze_temp < 10) antifreeze_temp = 10;
						break;
				case 6: max_water_temp--;
						if (max_water_temp < 10) max_water_temp = MAX_OUT_TEMP;
						if (max_water_temp < antifreeze_temp) antifreeze_temp = max_water_temp;
						break;
			}
			if (edit_mode) get_and_display_temp();
		}
		
	}
	in_state = new_in_state;
}



void log_data() {
	
	File log_file = SD.open("boiler.log", FILE_WRITE);
	if (log_file) {
		#ifdef USE_RTC
		log_file.print(now.year());
		log_file.print(cpoint);
		log_file.print(now.month());
		log_file.print(cpoint);
		log_file.print(now.day());
		log_file.print(cspace);
		log_file.print(now.hour());
		log_file.print(ccolon);
		log_file.print(now.minute());
		log_file.print(ccolon);
		log_file.print(now.second());
		log_file.print(csemicolon);
		#endif
		log_file.print(relay_state, DEC);
		log_file.print(csemicolon);
		log_file.print(boiler_state, DEC);
		log_file.print(csemicolon);
		log_file.print(boiler_on_mode, DEC);
		log_file.print(csemicolon);
		log_file.print(boiler_off_mode, DEC);
		log_file.print(csemicolon);
		log_file.print(power_save_mode, DEC);
		log_file.print(csemicolon);
		for (byte k = 0; k < MAX_TEMP_SENSOR; k++) {
			char idx = ts_by_display(cTempDisplay[k]);
			if (idx > -1) {
				log_file.print(temp_sensor[idx].temp[0], DEC);
				log_file.print(cpoint);
				log_file.print(temp_sensor[idx].temp[1], DEC);
				log_file.print(csemicolon);
				log_file.print(temp_sensor[idx].temp[2], DEC);
				log_file.print(csemicolon);
				log_file.print(temp_sensor[idx].flags, DEC);
				log_file.print(csemicolon);
			} else
			{
				log_file.print(cminus);
				log_file.print(cpoint);
				log_file.print(cminus);
				log_file.print(csemicolon);
				log_file.print(cminus);
				log_file.print(csemicolon);
				log_file.print(cminus);
				log_file.print(csemicolon);
			}
		}
		log_file.println();
		
		log_file.close();
	}
	
}

void tube_power() {
	char ts = get_temp_at_display(OUT_DOOR_DISP);
	byte state = relay_state;
	if ((ts < -5) & (!power_save_mode)) {
		state |= TUBE_POWER_ON;
	} else
	{
		state &= ~TUBE_POWER_ON;
	}
	if (state != relay_state) {
		relay_state = state;
		out_relay_state();
	}
}

void setup(void) {
	setup_pc_pwm_any();
	pinMode(EXT_CS_IN, OUTPUT);
	digitalWrite(EXT_CS_IN, HIGH);
	pinMode(EXT_DATA_IN, INPUT);
	pinMode(EXT_CS, OUTPUT);
//	pinMode(A2, OUTPUT);
	digitalWrite(EXT_CS, HIGH);
	pinMode(EXT_CLOCK, OUTPUT);
	pinMode(EXT_DATA_OUT, OUTPUT);
	set_boiler_off(FORCE);
	pump_off();
	Serial.begin(9600);
	
	while (!Serial) {
		;
	}
	SerialPrint(cstrFreeMemory);
	Serial.println(freeRam(), DEC);
	SerialPrintln(cstrLoadAndInit);
	Wire.begin();
	#ifdef USE_RTC
	rtc.begin();
	rtc.writeSqwPinMode(SquareWave1HZ);
	
	if (!rtc.isrunning()) {
		SerialPrintln(cstrRTCIsNotRunning);
		//rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
	}
	Serial.println(cpoint);
	#endif

	#ifdef SERIAL_COMMAND
	SCmd.addCommand("ts", list_temp_sensor);
	SCmd.addCommand("lt", set_display_light);
	SCmd.addCommand("st", printState);
	SCmd.addCommand("bl", cmd_boiler);
	#ifdef USE_RTC
	SCmd.addCommand("dt", cmd_datetime);
	#endif
	SCmd.addDefaultHandler(unrecognized);
	#endif
	
	search_ds();
	load_ts_config();
	
	SerialPrintln(cstrLedInitAndTest);
	byte intensity = i2c_eeprom_read_byte(EEPROM_CFG, INTENSITY_ADDR);
	init_and_test_led(intensity);
	Serial.println(cpoint);
	
	SerialPrintln(cstrInitSD);
	
	if (!SD.begin(SD_CS_PIN)) {
		SerialPrintln(cstrFAILED);
	} else
	{
		SerialPrintln(cstrOK);
	}
	Serial.println(cpoint);
	max_water_temp = i2c_eeprom_read_byte(EEPROM_CFG, MAX_OUT_TEMP_ADDR);
	if ((max_water_temp == 0) | (max_water_temp > MAX_OUT_TEMP)) max_water_temp = MAX_OUT_TEMP;
	
	antifreeze_temp = i2c_eeprom_read_byte(EEPROM_CFG, ANTIFREEZE_TEMP_ADDR);
	
	if ((antifreeze_temp == 0) | (antifreeze_temp > max_water_temp)) antifreeze_temp = 40;
	
}


int test_val = 255;

void loop(void) {
	byte h = 0;
	byte m = 0;
	byte s = 0;
	byte ts_display = 0;
	
	#ifdef SERIAL_COMMAND
	SCmd.readSerial();
	#endif
	
	#ifdef USE_RTC
	now = rtc.now();
	h = now.hour();
	m = now.minute();
	s = now.second();
	#endif;
	lc.setDigit(2, 3, h/10, false);
	lc.setDigit(2, 2, h%10, s%2 == 0 ? false : true);
	lc.setDigit(2, 1, m/10, false);
	lc.setDigit(2, 0, m%10, false);
	
	curr_time = millis();
	
	if (curr_time >= (loop_time + 1000)) {
		if (((boiler_state & BOILER_TIMER_ON) > 0) & (boiler_timer > 0)) {
			boiler_timer--;
		}
		if (pump_off_delay > 0) pump_off_delay--;
		get_and_display_temp();
		loop_time = millis();
		log_data();
	}
	
		analogWriteAny(15, test_val);
		//		Serial.println(test_val);
		test_val--;
		if (test_val == 0) test_val = 1023;
	
	if ((boiler_state & BOILER_TIMER_ON) > 0) {
		lc.setChar(2, 7, 'b', true);
		lc.setDigit(2, 6, boiler_timer/100, false);
		lc.setDigit(2, 5, (boiler_timer/10)%10, false);
		lc.setDigit(2, 4, boiler_timer%10, false);
	} else
	
	if (!(is_boiler_on()) & (is_pump_on())) {
		lc.setChar(2, 7, 'P', true);
		lc.setDigit(2, 6, pump_off_delay/100, false);
		lc.setDigit(2, 5, (pump_off_delay/10)%10, false);
		lc.setDigit(2, 4, pump_off_delay%10, true);
		if (pump_off_delay == 0) {
			pump_off();
			clear_indicator(2, 7);
			clear_indicator(2, 6);
			clear_indicator(2, 5);
			clear_indicator(2, 4);
		}
	}
	input_data();
	tube_power();
	check_boiler();
	
}

