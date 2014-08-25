
// TearDrop
// Fan Controller 
// IC 1
// Version 99.0 
// ATmega328P
// Created by Nadav Matalon
// 24 August 2014

// (TDCR_IC1_FAN_99)


// FAN CONTROLLER: IC_1	(COM6)					

// PIN_D2  (PIN 4)  [PD2] - FAN_RPM_6		
// PIN_D3  (PIN 5)  [PD3] - VCC_DRIVE_2 (F_5-6) 
// PIN_D4  (PIN 6)  [PD4] - GND			
// PIN_D5  (PIN 11) [PD5] - GND			
// PIN_D6  (PIN 12) [PD6] - GND			
// PIN_D7  (PIN 13) [PD7] - SYSTEM_CONTROL_LED	
// PIN_D8  (PIN 14) [PB0] - SYSTEM_CONTROL_1 (OVD_ON/OVD_OFF)
// PIN_D9  (PIN 15) [PB1] - FAN_RPM_1
// PIN_D10 (PIN 16) [PB2] - VCC_DRIVE_1 (F_1-4)
// PIN_D11 (PIN 17) [PB3] - FAN_RPM_2
// PIN_D12 (PIN 18) [PB4] - FAN_RPM_3
// PIN_D13 (PIN 19) [PB5] - FAN_RPM_4
// PIN_A0  (PIN 23) [PC0] - GND	
// PIN_A1  (PIN 24) [PC1] - GND
// PIN_A2  (PIN 25) [PC2] - FAN_VCC_1
// PIN_A3  (PIN 26) [PC3] - FAN_VCC_2


// http://playground.arduino.cc/Main/PinChangeInt
// http://code.google.com/p/arduino-pinchangeint/
#include <PinChangeInt.h>

// https://github.com/steamfire/WSWireLib
#include <WSWire.h>


const boolean DEBUG_MODE = false;		// DEBUG_MODE
// const boolean DEBUG_MODE = true;		// DEBUG_MODE

// const boolean PRINT_FIRST_ROW = false;	// DEBUG_MODE
const boolean PRINT_FIRST_ROW = true;		// DEBUG_MODE

// const boolean PRINT_SECOND_ROW = false;	// DEBUG_MODE
const boolean PRINT_SECOND_ROW = true;		// DEBUG_MODE

const boolean PRINT_THIRD_ROW = false;		// DEBUG_MODE
// const boolean PRINT_THIRD_ROW = true;	// DEBUG_MODE

const boolean PRINT_FOURTH_ROW = false;		// DEBUG_MODE
// const boolean PRINT_FOURTH_ROW = true;	// DEBUG_MODE


const byte PIN_D2 = 2;				// FAN_RPM_6	
const byte PIN_D3 = 3;				// VCC_DRIVE_2 (F_5-6) 
const byte PIN_D4 = 4;				// GND
const byte PIN_D5 = 5;				// GND
const byte PIN_D6 = 6;				// GND
const byte PIN_D7 = 7;				// SYSTEM_CONTROL_LED
const byte PIN_D8 = 8;				// SYSTEM_CONTROL_1 (OVD_ON/OVD_OFF)
const byte PIN_D9 = 9;				// FAN_RPM_1
const byte PIN_D10 = 10;			// VCC_DRIVE_1 (F_1-4)
const byte PIN_D11 = 11;			// FAN_RPM_2
const byte PIN_D12 = 12;			// FAN_RPM_3
const byte PIN_D13 = 13;			// FAN_RPM_4
const byte PIN_A0 = A0;				// GND
const byte PIN_A1 = A1;				// GND
const byte PIN_A2 = A2;				// FAN_VCC_1
const byte PIN_A3 = A3;				// FAN_VCC_2

const byte ADC_A0 = 0;										// ADC input channel PIN_A0
const byte ADC_A1 = 1;										// ADC input channel PIN_A1
const byte ADC_A2 = 2;				// FAN_VCC_1					// ADC input channel PIN_A2
const byte ADC_A3 = 3;				// FAN_VCC_2					// ADC input channel PIN_A3
const byte ADC_A4 = 4;										// ADC input channel PIN_A4
const byte ADC_A5 = 5;										// ADC input channel PIN_A5
const byte ADC_TEMP = 8;			// M1_TEMP_IC					// ADC input channel IC_TEMP
const byte ADC_VCC = 14;			// M1_VCC_IC					// ADC input channel IC_VCC
const byte ADC_GND = 15;									// ADC input channel IC_GND

// const byte M2_ADDRESS = 40;			// FAN_CONTROLLER_IC2				// M2 I2C device address

const byte M1_ADDRESS = 50;			// FAN_CONTROLLER_IC1				// M1 I2C device address
const byte M1_DATA_OUT_COUNT = 8; 								// number of bytes sent (max: 32 bytes)
const byte M1_DATA_IN_COUNT = 30; 								// number of bytes sent (max: 32 bytes)

const byte PC_ADDRESS = 60;			// PC_I2C_ADDRESS				// PC I2C device address
const byte PC_DATA_OUT_COUNT = 30;
const byte PC_PING = 100;
const byte PC_PING_COUNT = 1;

const byte M2_PACKAGE_1 = 241;
const byte M2_PACKAGE_2 = 242;
const byte M2_PACKAGE_3 = 243;

const byte PC_PACKAGE_1 = 245;
const byte PC_PACKAGE_2 = 246;
const byte PC_PACKAGE_3 = 247;
const byte PC_PACKAGE_4 = 248;

const unsigned int FC_SYSTEM_CONTROL_1 = 2001;	// SYSTEM_CONTROL_1 (OVD_OFF/OVD_ON)		// FC_SYSTEM_CONTROL_1 processing update command
const unsigned int FC_SYSTEM_CONTROL_2 = 2002;	// SYSTEM_CONTROL_2 (MAN/AUTO)			// FC_SYSTEM_CONTROL_2 processing update command
const unsigned int FC_FAN_VCC_1 = 2003;		// FAN_VCC_1					// FC_FAN_VCC_1 processing update command
const unsigned int FC_FAN_VCC_2 = 2004;		// FAN_VCC_2					// FC_FAN_VCC_2 processing update command
const unsigned int FC_VCC_ICF = 2005;		// VCC_ICF (BOTTOM)				// FC_VCC_IC processing update command
const unsigned int FC_FAN_MODE_1 = 2006;	// FAN_MODE_1 (VCC_1/PWM_1)			// FC_FAN_MODE_1 processing update command
const unsigned int FC_FAN_RPM_1 = 2007;		// FAN_RPM_1					// FC_FAN_RPM_1 processing update command
const unsigned int FC_FAN_RPM_2 = 2008;		// FAN_RPM_2					// FC_FAN_RPM_2 processing update command
const unsigned int FC_FAN_RPM_3 = 2009;		// FAN_RPM_3					// FC_FAN_RPM_3 processing update command
const unsigned int FC_FAN_RPM_4 = 2010;		// FAN_RPM_4					// FC_FAN_RPM_4 processing update command
const unsigned int FC_DUTY_CYCLE_PER_1 = 2011;	// DUTY_CYCLE_PER_1				// FC_DUTY_CYCLE_PER_1 processing update command
const unsigned int FC_FAN_MODE_2 = 2012;	// FAN_MODE_2 (VCC_2/PWM_2)			// FC_FAN_MODE_2 processing update command
const unsigned int FC_FAN_RPM_5 = 2013;		// FAN_RPM_5					// FC_FAN_RPM_5 processing update command
const unsigned int FC_FAN_RPM_6 = 2014;		// FAN_RPM_6					// FC_FAN_RPM_6 processing update command
const unsigned int FC_DUTY_CYCLE_PER_2 = 2015;	// DUTY_CYCLE_PER_2				// FC_DUTY_CYCLE_PER_2 processing update command
const unsigned int FC_TEMP_AN1 = 2016;		// TEMP_AN1					// FC_TEMP_AN3 processing update command
const unsigned int FC_TEMP_AN2 = 2017;		// TEMP_AN2					// FC_TEMP_AN4 processing update command
const unsigned int FC_TEMP_DGF = 2018;		// TEMP_DGF (TEMP_OW_1)				// FC_TEMP_DG processing update command
const unsigned int FC_TEMP_ICF = 2019;		// TEMP_ICF (TOP)				// FC_TEMP_IC processing update command
const unsigned int FC_FREQ_VCC_1 = 2020;	// FREQ_VCC_1					// FC_FREQ_VCC_1 processing update command
const unsigned int FC_FREQ_PWM_1 = 2021;	// FREQ_PWM_1					// FC_FREQ_PWM_1 processing update command
const unsigned int FC_FREQ_VCC_2 = 2022;	// FREQ_VCC_2					// FC_FREQ_VCC_2 processing update command
const unsigned int FC_FREQ_PWM_2 = 2023;	// FREQ_PWM_2					// FC_FREQ_PWM_2 processing update command
const unsigned int FC_RAM_ICF = 2024;		// RAM_ICF (BOTTOM)				// FC_RAM_ICF processing update command

const unsigned int FC_TEMP_OW_3 = 2025;		// TEMP_OW_3					// FC_TEMP_OW_3 processing update command
const unsigned int FC_TEMP_OW_4 = 2026;		// TEMP_OW_4					// FC_TEMP_OW_4 processing update command

const unsigned int FC_TEMP_I2C_3 = 2027;	// TEMP_I2C_3					// FC_TEMP_I2C_3 processing update command
const unsigned int FC_TEMP_I2C_4 = 2028;	// TEMP_I2C_4					// FC_TEMP_I2C_4 processing update command

const unsigned int FC_PLACE_HOLDER = 2029;	// PLACE_HOLDER					// FC_PLACE_HOLDER processing update command

volatile boolean M2_got_data = false;
volatile byte M2_data_in[M1_DATA_IN_COUNT] = {0};
volatile byte M2_data_out[M1_DATA_OUT_COUNT] = {0};

volatile unsigned long ovf1 = 0;		// TIMER1_OVERFLOW				// stores M1_TIMER1 overflow count
volatile unsigned long ovf2 = 0;		// TIMER2_OVERFLOW				// stores M1_TIMER2 overflow count

volatile unsigned long currentBurp_D2 = 0;							// FAN_RPM_6 - stores rpm count for ISR on PIN_D2
volatile unsigned long currentBurp_D9 = 0;							// FAN_RPM_1 - stores rpm count for ISR on PIN_D9
volatile unsigned long currentBurp_D11 = 0;							// FAN_RPM_2 - stores rpm count for ISR on PIN_D11
volatile unsigned long currentBurp_D12 = 0;							// FAN_RPM_3 - stores rpm count for ISR on PIN_D12
volatile unsigned long currentBurp_D13 = 0;							// FAN_RPM_4 - stores rpm count for ISR on PIN_D13

boolean pc_connected = false;			// PC_CONNECTION
boolean check_print = true;
boolean start_print = false;
byte pc_data_flag = 0;

unsigned int freq_VCC_1 = 0;			// FREQ_VCC_1					// stores M1 TIMER1 frequency reading
unsigned int freq_VCC_2 = 0;			// FREQ_VCC_2					// stores M1 TIMER2 frequency reading

unsigned int freq_PWM_1 = 0;			// FREQ_PWM_1					// stores M2 TIMER1 frequency reading
unsigned int freq_PWM_2 = 0;			// FREQ_PWM_2					// stores M2 TIMER2 frequency reading

boolean system_control_1 = true;		// SYSTEM_CONTROL_1 (OVD_ON/OVD_OFF)		// stores switch state on PIN_D8 (0 => OVERDRIVE_ON; 1 => OVERDRIVE_OFF)
boolean system_control_2 = true;		// SYSTEM_CONTROL_2 (MAN/AUTO)			// SYSTEM_CONTROL (0 => MANUAL / 1 => AUTO)

boolean fan_mode_1 = true;			// FAN_MODE_1 (VCC_1/PWM_1)			// FAN_MODE_1 (0 => VCC_1; 1 => PWM_1)
boolean fan_mode_2 = true;			// FAN_MODE_2 (VCC_2/PWM_2)			// FAN_MODE_2 (0 => VCC_2; 1 => PWM_2)

unsigned int fan_control_1 = 880;		// FAN_CONTROL_1 (RAW_POT_1)			// FAN_CONTROL_1 - raw pot reading for CHANNEL1
unsigned int fan_control_2 = 880;		// FAN_CONTROL_2 (RAW_POT_2)			// FAN_CONTROL_2 - raw pot reading for CHANNEL2

unsigned int M1_duty_cycle_1 = 638;		// M1_DUTY_CYCLE_1 (VCC_1)
unsigned int M1_duty_cycle_2 = 78;		// M1_DUTY_CYCLE_2 (VCC_2)

unsigned int M2_duty_cycle_1 = 638;		// M2_DUTY_CYCLE_1 (PWM_1)
unsigned int M2_duty_cycle_2 = 78;		// M2_DUTY_CYCLE_2 (PWM_2)

unsigned int duty_cycle_per_1 = 100;		// DUTY_CYCLE_PER_1				// stores current duty_cycle value as percent (0-100%)
unsigned int duty_cycle_per_2 = 100;		// DUTY_CYCLE_PER_2				// stores current duty_cycle value as percent (0-100%)

unsigned int fan_rpm_1 = 0;			// FAN_RPM_1					// FAN_RPM_1 - stores rpm reading on PIN_D9
unsigned int fan_rpm_2 = 0;			// FAN_RPM_2					// FAN_RPM_2 - stores rpm reading on PIN_D11
unsigned int fan_rpm_3 = 0;			// FAN_RPM_3					// FAN_RPM_3 - stores rpm reading on PIN_D12
unsigned int fan_rpm_4 = 0;			// FAN_RPM_4					// FAN_RPM_4 - stores rpm reading on PIN_D13
unsigned int fan_rpm_5 = 0;			// FAN_RPM_5					// FAN_RPM_5 - stores rpm reading ON IC2
unsigned int fan_rpm_6 = 0;			// FAN_RPM_6					// FAN_RPM_6 - stores rpm reading on PIN_D2

unsigned int temp_AN1 = 0;			// TEMP_AN1					// stores TEMP_AN1 raw data
unsigned int temp_AN2 = 0;			// TEMP_AN2					// stores TEMP_AN2 raw data

unsigned int top_temp_AN = 550;			// TEMP_AN_TOP					// stores the current top temperature between temp_AN1 & temp_AN_2

unsigned int fan_vcc_1 = 0;			// FAN_VCC_1					// stores VCC reading of CHANNEL_1 supply
unsigned int fan_vcc_2 = 0;			// FAN_VCC_2					// stores VCC reading of CHANNEL_2 supply

boolean temp_OW_1_present = false;
boolean temp_OW_2_present = false;
boolean temp_OW_3_present = false;

unsigned int temp_OW_1 = 0;			// TEMP_OW_1					// stores temperature reading of OW_Device_1
unsigned int temp_OW_2 = 0;			// TEMP_OW_2					// stores temperature reading of OW_Device_2
unsigned int temp_OW_3 = 0;			// TEMP_OW_3					// stores temperature reading of OW_Device_3

boolean temp_I2C_1_present = false;
boolean temp_I2C_2_present = false;

unsigned int temp_I2C_1 = 0;			// TEMP_I2C_1					// stores current temperature reading of I2C_device_1
unsigned int temp_I2C_2 = 0;			// TEMP_I2C_2					// stores current temperature reading of I2C_device_2

unsigned int M1_vcc_IC = 0;			// M1_VCC_IC					// stores VCC reading of IC1 supply
unsigned int M1_temp_IC = 0;	                // M1_TEMP_IC					// stores temperature reading of IC1
unsigned int M1_ram_IC = 0;			// M1_FREE_RAM					// stores IC1 free ram 

unsigned int M2_vcc_IC = 0;			// M2_VCC_IC					// stores M2_VCC_IC (in mV)
unsigned int M2_temp_IC = 0;			// M2_TEMP_IC					// stores M2_TEMP_IC (in °C times 10)
unsigned int M2_ram_IC = 0;			// M2_FREE_RAM					// stores M2_FREE_RAM

unsigned int bottom_vcc_IC = 0;			// VCC_IC_BOTTOM				// stores the current bottom VCC_IC between M1_vcc_IC & M2_vcc_IC
unsigned int top_temp_IC = 0;			// TEMP_IC_TOP					// stores the current top temperature between M1_temp_IC & M2_temp_IC
unsigned int bottom_ram_IC = 0;			// RAM_IC_BOTTOM					// stores the current bottom free ram between M1_free_ram & M2_free_ram

boolean serial_open = false;									// stores state of serial port

const unsigned long LONG_UPDATE_INTERVAL = 1555556;  						// stores long time interval (in uS) for data updates
const float LONG_UPDATE_DIVISOR = 1.555556;        	       					// stores long update divisor (in Sec) for data updates
unsigned long long_update_tick = 0;								// stores current time reference for long update loop
unsigned long long_update_tock = 0;								// stores previous time reference for long update loop

const unsigned long SHORT_UPDATE_INTERVAL = 333334;  						// stores short time interval (in uS) for data updates
unsigned long short_update_tick = 0;								// stores current time reference for short update loop
unsigned long short_update_tock = 0;								// stores previous time reference for short update loop

const unsigned long PRINT_INTERVAL = 4000;
const unsigned long KICKSTART_INTERVAL = 8000;							// sets time interval for kickstart function (in mS)
const unsigned long STARTUP_INTERVAL = 10000;							// sets time interval for startup function (in mS)
unsigned long currentStartupCount = 0;								// stores time reference for KICKSTART function

const unsigned long PACKAGE_2_INTERVAL = 600000;  						// stores short time interval (in uS) for data updates
unsigned long package_2_tick = 0;								// stores current time reference for short update loop
unsigned long package_2_tock = 0;								// stores previous time reference for short update loop

const unsigned long PACKAGE_3_INTERVAL = 900000;  						// stores short time interval (in uS) for data updates
unsigned long package_3_tick = 0;								// stores current time reference for short update loop
unsigned long package_3_tock = 0;								// stores previous time reference for short update loop

const unsigned long PACKAGE_4_INTERVAL = 1200000;  						// stores short time interval (in uS) for data updates
unsigned long package_4_tick = 0;								// stores current time reference for short update loop
unsigned long package_4_tock = 0;								// stores previous time reference for short update loop


void setup() {

	DDRD &= ~(1 << DDD2); PORTD |= (1 << PORTD2);	// FAN_RPM_6 (PIN_D2)			// FAN_RPM_6 - pinMode(PIN_D2, INPUT_PULLUP)
	DDRD |= (1 << DDD3); 				// M1_TIMER2 (PIN_D3)			// VCC_DRIVE_2 (M1_TIMER2) - pinMode(PIN_D3, OUTPUT)
	DDRD &= ~(1 << DDD4);  									// GND - pinMode(PIN_D4, INPUT)
	DDRD &= ~(1 << DDD5);  									// GND - pinMode(PIN_D5, INPUT)
	DDRD &= ~(1 << DDD6);  									// GND - pinMode(PIN_D6, INPUT)
	DDRD |= (1 << DDD7); PORTD &= ~(1 << PORTD7); 	// SYSTEM_CONTROL_LED (PIN_D7)		// SYSTEM_CONTROL_LED - pinMode(PIN_D7, OUTPUT); digitalWrite(PIN_D7, LOW)
	DDRB &= ~(1 << DDB0); PORTB |= (1 << PORTB0);	// SYSTEM_CONTROL_1 (PIN_D8)		// SYSTEM_CONTROL (OVD) - pinMode(PIN_D8, INPUT_PULLUP)
	DDRB &= ~(1 << DDB1); PORTB |= (1 << PORTB1);	// FAN_RPM_1 (PIN_D9)			// FAN_RPM_1 - pinMode(PIN_D9, INPUT_PULLUP)
	DDRB |= (1 << DDB2); 				// M1_TIMER1 (PIN_D10)			// VCC_DRIVE_1 (M1_TIMER1) - pinMode(PIN_D10, OUTPUT)
	DDRB &= ~(1 << DDB3); PORTB |= (1 << PORTB3);	// FAN_RPM_2 (PIN_D11)			// FAN_RPM_2 - pinMode(PIN_D11, INPUT_PULLUP)
	DDRB &= ~(1 << DDB4); PORTB |= (1 << PORTB4);	// FAN_RPM_3 (PIN_D12)			// FAN_RPM_3 - pinMode(PIN_D12, INPUT_PULLUP)
	DDRB &= ~(1 << DDB5); PORTB |= (1 << PORTB5);	// FAN_RPM_4 (PIN_D13)			// FAN_RPM_4 - pinMode(PIN_D13, INPUT_PULLUP)
	DDRC &= ~(1 << DDC0);									// GND - pinMode(PIN_A0, INPUT)
	DDRC &= ~(1 << DDC1);									// GND - pinMode(PIN_A1, INPUT)
	DDRC &= ~(1 << DDC2);				// FAN_VCC_1 (PIN_A2)			// FAN_VCC_1 - pinMode(PIN_A2, INPUT)
	DDRC &= ~(1 << DDC3);				// FAN_VCC_2 (PIN_A3)			// FAN_VCC_2 - pinMode(PIN_A3, INPUT)


// read_ADC() SETUP //

	ADMUX = 0; 										// resets entire ADMUX register to zero (ADC OFF, auto-trigger disabled)
	ADMUX |= (1 << REFS0);									// configures AVCC as ADC voltage reference
	ADMUX |= ADC_GND;									// sets ADC Mux input channel to GND							
        ADCSRA = 0;										// resets entire ADCSRA register to zero
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);					// sets ADC with prescaler 128 (16MHz / 128 = 125KHz)
//	ADCSRA |= (1 << ADPS2) | (1 << ADPS1);							// sets ADC with prescaler 64 (16MHz / 64 = 250KHz)
//	ADCSRA |= (1 << ADPS2) | (1 << ADPS0);							// sets ADC with prescaler 32 (16MHz / 32 = 500KHz)
	ADCSRA |= (1 << ADEN) | (1 << ADSC);							// turns on ADC & preforms initial conversion


// M1_TIMER1 SETUP //

	cli();											// disables global interrupts
		TCCR1A = 0;									// resets entire Timer1 "Control Register A" to zero
												// (normal operation: OC1A & OC1B disconnected)
		TCCR1B = 0;									// resets entire Timer1 "Control Register B" to zero 
												// (Timer1 is OFF)										
		TIMSK1 |= (1 << TOIE1); 							// enables Timer1 "compare interrupt mask register A"
		TCCR1A |= (1 << COM1B1) | (1 << WGM11) | (1 << WGM10);				// set up Timer1 in FAST PWM mode (together with TCCR1B settings below)
												// (Timer1 prescaler = 1)
		TCCR1B |= (1 << WGM13) | (1 << WGM12);						// clears OC1B on Compare Match mode (OC1A as "TOP" & OC1B as "BOTTOM")
		OCR1A = 639;									// sets Timer1 "TOP" to generate 25KHz frequency (16Mhz / 1 / 640)
												// (Timer1 OVF ISR invoked every 40uS)
		OCR1B = 638;									// sets initial duty cycle on PIN_D10 (OC1B) to maximum value ("TOP"-1)
		TCCR1B |= (1 << CS10);								// turn on Timer1 (with prescaler 1)

// M1_TIMER2 SETUP //

		TCCR2A = 0;									// resets entire Timer2 "Control Register A" to zero
												// (normal operation: OC2A & OC2B disconnected)
		TCCR2B = 0;									// resets entire Timer2 "Control Register B" to zero
												// (Timer2 is OFF)
		TIMSK2 |= (1 << TOIE2);								// enables Timer2 "compare interrupt mask register A"
		TCCR2A |= (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);				// set ups Timer2 in FAST PWM mode (together with TCCR2B settings below)
		TCCR2B |= (1 << WGM22);								// clears OC2B on Compare Match mode (OC2A as "TOP" & OC2B as "BOTTOM")
		OCR2A = 79;									// sets Timer2 "TOP" to generate 25KHz frequency (16Mhz / 8 / 80 = 25KHz)
												// (Timer2 OVF ISR invoked every 40uS)
		OCR2B = 78;									// sets initial duty cycle on PIN_D3 (OC2B) to maximum value ("TOP"-1)
		TCCR2B |= (1 << CS21);								// turns on Timer2 with prescaler 8
	sei();											// enables global interrupts


  	Wire.begin(M1_ADDRESS);                	        					// joins I2C bus as master (defult no address sets as bus Master)
 
	TWBR = 12; 										// TWBR 12: 400KHz (max); 32: 200KHz; 72: 100KHz (default)

	Wire.onReceive(receive_Call);

	Wire.onRequest(request_Call);


	PCintPort::attachInterrupt(PIN_D2, burpCount_D2,FALLING);				// FAN_RPM_6 - attaches software interrupt on PIN_D2
   	PCintPort::attachInterrupt(PIN_D9, burpCount_D9,FALLING);				// FAN_RPM_1 - attaches software interrupt on PIN_D9
  	PCintPort::attachInterrupt(PIN_D11, burpCount_D11,FALLING); 				// FAN_RPM_2 - attaches software interrupt on PIN_D11
   	PCintPort::attachInterrupt(PIN_D12, burpCount_D12,FALLING);				// FAN_RPM_3 - attaches software interrupt on PIN_D12
   	PCintPort::attachInterrupt(PIN_D13, burpCount_D13,FALLING);				// FAN_RPM_4 - attaches software interrupt on PIN_D13


	if (!DEBUG_MODE) {

//		Serial.begin(115200);								// opens serial port & begins serial communication
		Serial.begin(230400);								// opens serial port & begins serial communication
//		Serial.begin(250000);								// opens serial port & begins serial communication
//		Serial.begin(345600);								// opens serial port & begins serial communication
//		Serial.begin(500000);								// opens serial port & begins serial communication

	} else {

		Serial.begin(115200);								// opens serial port & begins serial communication
	}

	check_Serial();										// checks if serial port is open


	while ((!pc_connected) && (micros() < 50000)) {

		pc_connected = FC_Discover_PC();
		delayMicroseconds(250);
	}
}


void loop() {


	if (M2_got_data) M1_read_Data_M2();


	short_update_tock = micros();								// current time reference for short update function (in uS)

	package_2_tock = short_update_tock;

	package_3_tock = short_update_tock;

	package_4_tock = short_update_tock;

	long_update_tock = micros();								// current time reference for long update function (in uS)

	currentStartupCount = millis();								// current time reference for startup function (in mS)


	if ((short_update_tock - short_update_tick) >= SHORT_UPDATE_INTERVAL) {			// update function preformed every 0.333334 secondS

		short_update_tick = short_update_tock;						// stores current time reference for next update

		if (check_print) check_Print();

		update_Duty_Cycle();								// updates current duty cycle of FAN_CHANNEL_1 & FAN_CHANNEL_2

		if (pc_connected) {					// PC_UPDATE_MODE

			M1_Send_Data_PC(PC_PACKAGE_1); 
       

		} else {						// FC_UPDATE_MODE

			if ((!DEBUG_MODE) && (start_print) && (serial_open)) M1_Short_Processing_Print();
		}

		if (pc_data_flag > 20) pc_connected = false;
  	}


	if ((package_2_tock - package_2_tick) >= PACKAGE_2_INTERVAL) {				// update function preformed every 1.555556 secondS

		package_2_tick = package_2_tock;						// stores current time reference for next update

		M1_Send_Data_PC(PC_PACKAGE_2); 
 	}

	if ((package_3_tock - package_3_tick) >= PACKAGE_3_INTERVAL) {				// update function preformed every 1.555556 secondS

		package_3_tick = package_3_tock;						// stores current time reference for next update

		M1_Send_Data_PC(PC_PACKAGE_3); 
 	}


	if ((package_4_tock - package_4_tick) >= PACKAGE_4_INTERVAL) {				// update function preformed every 1.555556 secondS

		package_4_tick = package_4_tock;						// stores current time reference for next update

		M1_Send_Data_PC(PC_PACKAGE_4); 
 	}

	if ((long_update_tock - long_update_tick) >= LONG_UPDATE_INTERVAL) {			// update function preformed every 1.555556 secondS

		long_update_tick = long_update_tock;						// stores current time reference for next update

		freq_VCC_1 = update_Frequency(PIN_D10);		// FREQ_VCC_1			// calculates VCC frequency on PIN_D10

		fan_vcc_1 = update_Fan_VCC(PIN_A2);		// FAN_VCC_1			// reads VCC output to CHANNEL1

		freq_VCC_2 = update_Frequency(PIN_D3);		// FREQ_VCC_2			// calculates VCC frequency on PIN_D3

		fan_vcc_2 = update_Fan_VCC(PIN_A3);		// FAN_VCC_2			// reads VCC output to CHANNEL2

		M1_vcc_IC = update_Vcc_IC(); 			// M1_VCC_IC			// reads vcc_IC1

		bottom_vcc_IC = update_Bottom_Vcc_IC();		// VCC_IC_BOTTOM

		fan_rpm_1 = update_Fan_RPM(PIN_D9);		// FAN_RPM_1			// FAN_RPM_1 - updates rpm reading on PIN_D9

		fan_rpm_2 = update_Fan_RPM(PIN_D11);		// FAN_RPM_2			// FAN_RPM_2 - updates rpm reading on PIN_D11

		fan_rpm_3 = update_Fan_RPM(PIN_D12);		// FAN_RPM_3			// FAN_RPM_3 - updates rpm reading on PIN_D12

		fan_rpm_4 = update_Fan_RPM(PIN_D13);		// FAN_RPN_4			// FAN_RPM_4 - updates rpm reading on PIN_D13

		M1_temp_IC = update_Temp_IC();			// M1_TEMP_IC			// reads temp_IC1

		top_temp_IC = update_Top_Temp_IC();		// TEMP_IC_TOP

		fan_rpm_6 = update_Fan_RPM(PIN_D2);		// FAN_RPM_6			// FAN_RPM_6 - updates rpm reading on PIN_D2

		M1_ram_IC = update_Ram_IC();			// M1_RAM_IC			// reads ram_IC1

		bottom_ram_IC = update_Bottom_Ram_IC();		// RAM_IC_BOTTOM
              
		if (!pc_connected) {

			pc_connected = FC_Discover_PC();							// checks if PUMP CONTROLLER was connected
                     
			if ((!DEBUG_MODE) && (start_print) && (serial_open)) M1_Long_Processing_Print();	// sends processing data to processing application
		}

		if ((DEBUG_MODE) && (start_print) && (serial_open)) print_Long_Update();			// preforms debugging serial print function		if (!serial_open) check_Serial();
	}
}



ISR(TIMER1_OVF_vect) {										// M1_TIMER1 ISR (pwm frequency counter)
  	ovf1++;											// counts the number of M1_TIMER1 overflows
}


ISR(TIMER2_OVF_vect) {										// M1_TIMER2 ISR (pwm frequency counter)
  	ovf2++;											// counts the number of M1_TIMER2 overflows
}


void burpCount_D2() {										// ISR for interrupt triggers on PIN_D2
  	currentBurp_D2++;
}

void burpCount_D9() {										// ISR for interrupt triggers on PIN_D9
  	currentBurp_D9++;
}

void burpCount_D11() {										// ISR for interrupt triggers on PIN_D11
  	currentBurp_D11++;
}

void burpCount_D12() {										// ISR for interrupt triggers on PIN_D12
  	currentBurp_D12++;
}

void burpCount_D13() {										// ISR for interrupt triggers on PIN_D13
  	currentBurp_D13++;
}


void receive_Call (int numBytes) {

	if (numBytes == M1_DATA_IN_COUNT) {

		for (int i = 0; i < numBytes; i++) {

			M2_data_in[i] = Wire.read();
		}

		M2_got_data = true;
	}
}


void request_Call() {	

	Wire.write(((byte*)M2_data_out), M1_DATA_OUT_COUNT);
}


boolean FC_Discover_PC() {

	static boolean pc_present;
	static byte ping_result;

	if ((Wire.requestFrom(PC_ADDRESS, PC_PING_COUNT)) == PC_PING_COUNT) {
		ping_result = Wire.read();
		if (ping_result == PC_PING) {
			pc_present = true;
			pc_data_flag = 0;
		} else {
			delayMicroseconds(250);
			if ((Wire.requestFrom(PC_ADDRESS, PC_PING_COUNT)) == PC_PING_COUNT) {
				ping_result = Wire.read();
				if (ping_result == PC_PING) {
					pc_present = true;
					pc_data_flag = 0;
				} else {
					pc_present = false;
				}
			} else {
				pc_present = false;
			} 
		}        
	}  else {
		pc_present = false;
	}    
	return pc_present;
}


void check_Print() {

	if (currentStartupCount > PRINT_INTERVAL) {
		check_print = false;
		start_print = true;
	}
}

	
unsigned int update_Frequency (const byte freq_pin) {

	static unsigned int current_freq;			      					// stores total overflow count of selected timer
//	static volatile unsigned int current_freq;		      					// stores total overflow count of selected timer
	static unsigned int previous_freq_1;
	static unsigned int previous_freq_2;
	static unsigned int delta_freq;

	switch (freq_pin) {

		case (PIN_D10):						// FREQ_VCC_1

			cli();
				current_freq = ovf1; 							// stores number of M1_TIMER1 overflow
				ovf1 = 0; 								// resets M1_TIMER1 overflow counter to zero
			sei();

		 	current_freq = round(current_freq / LONG_UPDATE_DIVISOR);
//		 	current_freq = round(float(current_freq) / LONG_UPDATE_DIVISOR);

			delta_freq = abs(current_freq - previous_freq_1);

			if ((delta_freq >= 10) || (current_freq < 24500)) previous_freq_1 = current_freq;
			else current_freq = previous_freq_1;

			break;

		case (PIN_D3):						// FREQ_VCC_2

			cli();
				current_freq = ovf2; 							// stores number of M1_TIMER2 overflow
				ovf2 = 0; 								// resets M1_TIMER2 overflow counter to zero
			sei();

		 	current_freq = round(current_freq / LONG_UPDATE_DIVISOR);
//		 	current_freq = round(float(current_freq) / LONG_UPDATE_DIVISOR);

			delta_freq = abs(current_freq - previous_freq_2);

			if ((delta_freq >= 10) || (current_freq < 24500)) previous_freq_2 = current_freq;
			else current_freq = previous_freq_2;

			break;
	}
	return current_freq;
}


void update_Duty_Cycle() {

	static unsigned int current_duty_cycle_1;								// stores current duty_cycle value of FAN_CHANNEL_1
	static unsigned int current_duty_cycle_2;								// stores current duty_cycle value of FAN_CHANNEL_2
	static union dataToSend {byte b[2]; unsigned int i;} M2_duty_out_1, M2_duty_out_2;

	system_control_1 = bitRead(PINB, 0);			// OVD_ON/OVD_OFF				// digitalRead(PIN_D8) (0 => OVD_OFF; 1 => OVD_ON)

	if ((currentStartupCount >= KICKSTART_INTERVAL) && (!system_control_1)) { 	// OVD_OFF		// SYSTEM_CONTROL_1 => OVD_OFF

		if (bitRead(PIND, 7)) PORTD &= ~(1 << PORTD7);				// OVD_LED_OFF		// turns off SYSTEM_CONTROL_LED (PIN_D7)

		if (!system_control_2) {			// MAN						// SYSTEM_CONTROL_2 => MAN

			current_duty_cycle_1 = map_Float(float(fan_control_1), 40.0, 880.0, 230.0, 638.0);  	// calculates duty cycle for M1_TIMER1 & M2_TIMER1 (minimum duty set to 36%)
			current_duty_cycle_2 = map_Float(float(fan_control_2), 40.0, 880.0, 28.0, 78.0);  	// calculates duty cycle for M1_TIMER2 & M2_TIMER2 (minimum duty set to 36%)
		
		} else {					// AUTO						// SYSTEM_CONTROL_2 => AUTO		

			if (top_temp_AN < 200) top_temp_AN = 200;
			if (top_temp_AN > 550) top_temp_AN = 550;

			current_duty_cycle_1 = map_Float(float(top_temp_AN), 200.0, 550.0, 230.0, 638.0);
			current_duty_cycle_2 = map_Float(float(top_temp_AN), 200.0, 550.0, 28.0, 78.0);
		}


		if (!fan_mode_1) {				// VCC_1						// FAN_MODE_1 => VCC_1 (M1_TIMER1)

			if (M2_duty_out_1.i != 638) M2_duty_out_1.i = 638;

			cli(); 												// disables global interrupts
				if (OCR1B != current_duty_cycle_1) OCR1B = current_duty_cycle_1;								// sets M1_TIMER1 (VCC_1) to new value
	
				if ((M2_duty_out_1.i != M2_duty_cycle_1) || (currentStartupCount < STARTUP_INTERVAL)) {

					M2_data_out[0] = M2_duty_out_1.b[0];
					M2_data_out[1] = M2_duty_out_1.b[1];
					M2_data_out[2] = M2_duty_out_1.b[0];
					M2_data_out[3] = M2_duty_out_1.b[1];
				}
			sei();												// enables global interrupts

			if (M1_duty_cycle_1 != current_duty_cycle_1) M1_duty_cycle_1 = current_duty_cycle_1;		// updates value of M1_OCR1B register
	
			if (M2_duty_cycle_1 != M2_duty_out_1.i) M2_duty_cycle_1 = M2_duty_out_1.i;

		} else {					// PWM_1						// FAN_MODE_1 => PWM_1 (M2_TIMER1)


			if (M2_duty_out_1.i != current_duty_cycle_1) M2_duty_out_1.i = current_duty_cycle_1;

			cli(); 												// disables global interrupts

				if (OCR1B != 638) OCR1B = 638;									// sets M1_TIMER1 (VCC_1) to maximum value

				if ((M2_duty_out_1.i != M2_duty_cycle_1) || (currentStartupCount < STARTUP_INTERVAL)) {

					M2_data_out[0] = M2_duty_out_1.b[0];
					M2_data_out[1] = M2_duty_out_1.b[1];
					M2_data_out[2] = M2_duty_out_1.b[0];
					M2_data_out[3] = M2_duty_out_1.b[1];
				}
			sei();												// enables global interrupts

			if (M1_duty_cycle_1 != 638) M1_duty_cycle_1 = 638;	

			if (M2_duty_cycle_1 != M2_duty_out_1.i) M2_duty_cycle_1 = M2_duty_out_1.i;
		}

		duty_cycle_per_1 = map_Float(float(current_duty_cycle_1), 230.0, 638.0, 36.0, 100.0);			// updates value of duty_cycle_per_1 (in percent)


		if (!fan_mode_2) {				// VCC_2						// FAN_MODE_2 => VCC_2 (M1_TIMER2)


			if (M2_duty_out_2.i != 78) M2_duty_out_2.i = 78;

			cli(); 												// disables global interrupts
				if (OCR2B != current_duty_cycle_2) OCR2B = current_duty_cycle_2;								// sets M1_TIMER1 (VCC_1) to new value

				if ((M2_duty_out_2.i != M2_duty_cycle_2) || (currentStartupCount < STARTUP_INTERVAL)) {

					M2_data_out[4] = M2_duty_out_2.b[0];
					M2_data_out[5] = M2_duty_out_2.b[1];
					M2_data_out[6] = M2_duty_out_2.b[0];
					M2_data_out[7] = M2_duty_out_2.b[1];
				}
			sei();												// enables global interrupts

			if (M1_duty_cycle_2 != current_duty_cycle_2) M1_duty_cycle_2 = current_duty_cycle_2;		// updates value of M1_OCR1B register

			if (M2_duty_cycle_2 != M2_duty_out_2.i) M2_duty_cycle_2 = M2_duty_out_2.i;


		} else {					// PWM_2						// FAN_MODE_2 => PWM_2 (M2_TIMER2)


			if (M2_duty_out_2.i != current_duty_cycle_2) M2_duty_out_2.i = current_duty_cycle_2;

			cli(); 												// disables global interrupts

				if (OCR2B != 78) OCR2B = 78;									// sets M1_TIMER1 (VCC_1) to maximum value

				if ((M2_duty_out_2.i != M2_duty_cycle_2) || (currentStartupCount < STARTUP_INTERVAL)) {

					M2_data_out[4] = M2_duty_out_2.b[0];
					M2_data_out[5] = M2_duty_out_2.b[1];
					M2_data_out[6] = M2_duty_out_2.b[0];
					M2_data_out[7] = M2_duty_out_2.b[1];
				}
			sei();												// enables global interrupts

			if (M1_duty_cycle_2 != 78) M1_duty_cycle_2 = 78;	

			if (M2_duty_cycle_2 != M2_duty_out_2.i) M2_duty_cycle_2 = M2_duty_out_2.i;			// sets M2_TIMER1 duty cycle to to new value

		}
	
		duty_cycle_per_2 = map_Float(float(current_duty_cycle_2), 28.0, 78.0, 36.0, 100.0);			// updates value of duty_cycle_per_2 (in percent)
		

	} else {								// OVD_ON				// SYSTEM_CONTROL_1 => OVD_ON

		if (!(bitRead(PIND, 7))) PORTD |= (1 << PORTD7);		// OVD_LED_ON				// turns on SYSTEM_CONTROL_LED (PIN_D7)

		system_control_1 = true;     			 							// changes system_control_1 to indicate OVD_ON

		if (M2_duty_out_1.i != 638) M2_duty_out_1.i = 638;

		if (M2_duty_out_2.i != 78) M2_duty_out_2.i = 78;

		cli(); 													// disables global interrupts
			if (OCR1B != 638) OCR1B = 638;									// sets M1_TIMER1 (VCC_1) to maximum value

			if (OCR2B != 78) OCR2B = 78;									// sets M1_TIMER2 (VCC_2) to maximum value

			if ((M2_duty_out_1.i != M2_duty_cycle_1) || (currentStartupCount < STARTUP_INTERVAL)) {

				M2_data_out[0] = M2_duty_out_1.b[0];
				M2_data_out[1] = M2_duty_out_1.b[1];
				M2_data_out[2] = M2_duty_out_1.b[0];
				M2_data_out[3] = M2_duty_out_1.b[1];
			}

			if ((M2_duty_out_2.i != M2_duty_cycle_2) || (currentStartupCount < STARTUP_INTERVAL)) {

				M2_data_out[4] = M2_duty_out_2.b[0];
				M2_data_out[5] = M2_duty_out_2.b[1];
				M2_data_out[6] = M2_duty_out_2.b[0];
				M2_data_out[7] = M2_duty_out_2.b[1];
			}
		sei();													// enables global interrupts

		if (M1_duty_cycle_1 != 638) M1_duty_cycle_1 = 638;

		if (M1_duty_cycle_2 != 78) M1_duty_cycle_2 = 78;

		if (M2_duty_cycle_1 != M2_duty_out_1.i) M2_duty_cycle_1 = M2_duty_out_1.i;				// sets M2_TIMER1 (PWM_1) to maximum value

		if (M2_duty_cycle_2 != M2_duty_out_2.i) M2_duty_cycle_2 = M2_duty_out_2.i;				// sets M2_TIMER2 (PWM_2) to maximum value

		if (duty_cycle_per_1 != 100) duty_cycle_per_1 = 100;							// sets value of duty_cycle_per_1 to max (in percent)

		if (duty_cycle_per_2 != 100) duty_cycle_per_2 = 100;							// sets value of duty_cycle_per_2 to max (in percent)
	}
}


unsigned int update_Fan_VCC (const byte fan_vcc_pin) {

	static unsigned int current_adc_reading;
	static unsigned int current_vcc_reading;									// stores curent ADc reading on selected pin
	static unsigned int previous_vcc_reading_1;									// stores previous ADc reading on FAN_CHANNEL_1
	static unsigned int previous_vcc_reading_2;									// stores previous ADc reading on FAN_CHANNEL_2
	static unsigned int delta_vcc_reading;										// stores delta between current & previous ADC readings

	switch (fan_vcc_pin) {						

		case (PIN_A2): 							// FAN_VCC_1				// read VCC on FAN_CHANNEL_1

			current_adc_reading = read_ADC(ADC_A2);

			current_vcc_reading = map_Float((float(current_adc_reading)), 200, 850, 6390, 12280);		// maps ADC reading to actual voltage (in mV)

			delta_vcc_reading = abs(current_vcc_reading - previous_vcc_reading_1);

			if ((delta_vcc_reading >= 100) || (current_vcc_reading < 5000)) previous_vcc_reading_1 = current_vcc_reading;
			else current_vcc_reading = previous_vcc_reading_1;
			
			break;

		case (PIN_A3): 							// FAN_VCC_2				// read VCC on FAN_CHANNEL_2

			current_adc_reading = read_ADC(ADC_A3);

			current_vcc_reading = map_Float((float(current_adc_reading)), 200, 850, 6390, 12280);		// maps ADC reading to actual voltage (in mV)

			delta_vcc_reading = abs(current_vcc_reading - previous_vcc_reading_1);

			if ((delta_vcc_reading >= 100) || (current_vcc_reading < 5000)) previous_vcc_reading_1 = current_vcc_reading;
			else current_vcc_reading = previous_vcc_reading_1;
			
			break;
	}

	return current_vcc_reading;
}


unsigned int read_ADC (const byte adc_channel) {			        					// analogRead() of selected input source

	static unsigned int adc_delay; 										// stoes adc delay length between first & second conversion
        ADMUX = 0;                                                              				// resets entire ADMUX reguster to zero
 	if (adc_channel != ADC_TEMP) ADMUX |= (1 << REFS0) | adc_channel;					// sets AVCC as ADC voltage reference and selected ADC input channel
	else ADMUX |= (1 << REFS1) | (1 << REFS0) | adc_channel;						// sets Internal 1V1 as ADC voltage reference and selected ADC input channel
	switch (adc_channel) {
		case (ADC_TEMP): adc_delay = 6500; break;							// waits 6500uS for ADC_TEMP channel reading 
		case (ADC_VCC): adc_delay = 500; break;			       					// waits 500uS for ADC_VCC channel reading 
		default: adc_delay = 20; break;	    		                				// waits 20uS for ADC_VCC channel reading 
	}
        delayMicroseconds(adc_delay);   
	ADCSRA |= (1 << ADSC);				  	                				// turns on ADC & preforms initial conversion
	while (bit_is_set(ADCSRA, ADSC));  									// waits until ADC conversion is done (ADSC bit is cleared)
        if ((adc_channel == ADC_A2) || (adc_channel == ADC_A3)) {						// preforms second reading for fan_control_1 & fan_control_2 channels
		ADCSRA |= (1 << ADSC);				                				// turns on ADC & preforms initial conversion
		delayMicroseconds(adc_delay);
		while (bit_is_set(ADCSRA, ADSC));  								// waits until ADC conversion is done (ADSC bit is cleared)
	} 
        return ADCW; 	                                      	    	    					// returns latest ADC reading
}


unsigned int map_Float (float x, float x1, float x2, float y1, float y2) {

	static unsigned int map_result;
	if ((x2 - x1) != 0) map_result = round((((x - x1) * (y2 - y1)) / (x2 - x1)) + y1);
        else map_result = 0;	
	return map_result;
}


unsigned int update_Fan_RPM (const byte fan_pin) {

	static unsigned int current_rpm;										// stores current rpm reading on selected pin
//	static volatile unsigned int current_rpm;									// stores current rpm reading on selected pin
	static unsigned int previous_rpm;										// stores previous rpm reading on selected pin
	static unsigned int delta_rpm;											// stores delta rpm between previous & current readings

	switch (fan_pin) {
		case (PIN_D2): 						// FAN_RPM_6
		     	previous_rpm = fan_rpm_6;    	
			cli();												// disables global interrupts
				current_rpm = currentBurp_D2;								// obtains current rpm count from interrupt routine
				currentBurp_D2 = 0;									// resets rpm count on PIN_D9 to zero
			sei();												// enables global interrupts
			break;
		case (PIN_D9): 						// FAN_RPM_1
		     	previous_rpm = fan_rpm_1;    	
			cli();												// disables global interrupts
				current_rpm = currentBurp_D9;								// obtains current rpm count from interrupt routine
				currentBurp_D9 = 0;									// resets rpm count on PIN_D9 to zero
			sei();												// enables global interrupts
			break;
		case (PIN_D11): 					// FAN_RPM_2
		     	previous_rpm = fan_rpm_2;    	
			cli();												// disables global interrupts
				current_rpm = currentBurp_D11;								// obtains current rpm count from interrupt routine
				currentBurp_D11 = 0;									// resets rpm count on PIN_D11 to zero
			sei();												// enables global interrupts
			break;
		case (PIN_D12): 					// FAN_RPM_3
		     	previous_rpm = fan_rpm_3;    	
			cli();												// disables global interrupts
				current_rpm = currentBurp_D12;								// obtains current rpm count from interrupt routine
				currentBurp_D12 = 0;									// resets rpm count on PIN_D11 to zero
			sei();												// enables global interrupts
			break;
		case (PIN_D13): 					// FAN_RPM_4
		     	previous_rpm = fan_rpm_4;    	
			cli();												// disables global interrupts
				current_rpm = currentBurp_D13;								// obtains current rpm count from interrupt routine
				currentBurp_D13 = 0;									// resets rpm count on PIN_D13 to zero
			sei();												// enables global interrupts
			break;
	}

	current_rpm = round((current_rpm * 30.0) / LONG_UPDATE_DIVISOR);						// calculates current rpm within the update time interval
	delta_rpm = abs(current_rpm - previous_rpm);									// calculates delta between previous & current rpm readings 
	if (current_rpm < 300) current_rpm = 0;
	if (current_rpm > 4000) current_rpm = previous_rpm;
	if (current_rpm > previous_rpm) current_rpm = round((current_rpm + previous_rpm) / 2.0);					// averages previous & current readings
	if (current_rpm < 300) current_rpm = 0;
	if ((delta_rpm < 15) && (current_rpm > 300)) current_rpm = previous_rpm;
	else previous_rpm = current_rpm;

	return current_rpm;
}


unsigned int update_Vcc_IC() {

	static unsigned int current_adc_vcc;
	static unsigned int previous_adc_vcc;

        current_adc_vcc = read_ADC(ADC_VCC);
	
	if (current_adc_vcc != 0) current_adc_vcc = round(1125300.0 / float(current_adc_vcc));

	if ((abs(current_adc_vcc - previous_adc_vcc) >= 50) || (current_adc_vcc < 2000)) previous_adc_vcc = current_adc_vcc;
	else current_adc_vcc = previous_adc_vcc;							

	return current_adc_vcc;
}



unsigned int update_Bottom_Vcc_IC() {

	static unsigned int current_bottom_vcc;
	static unsigned int previous_bottom_vcc;
	static unsigned int delta_bottom_vcc;

	if (M1_vcc_IC <= M2_vcc_IC) current_bottom_vcc = M1_vcc_IC;
//	else current_bottom_vcc = M2_vcc_IC;
	else current_bottom_vcc = M1_vcc_IC;					// alternative (ONLY M1_VCC_IC)

	delta_bottom_vcc = abs(current_bottom_vcc - previous_bottom_vcc);

	if ((delta_bottom_vcc > 200) || (current_bottom_vcc < 2000)) previous_bottom_vcc = current_bottom_vcc;
	else current_bottom_vcc = previous_bottom_vcc;

	return current_bottom_vcc;	
}



unsigned int update_Temp_IC() {
	
	static unsigned int current_adc_temp;
	static unsigned int previous_adc_temp;
	static unsigned int delta_adc_temp;

        current_adc_temp = read_ADC(ADC_TEMP);

  	current_adc_temp = round((((float(current_adc_temp)) - 324.31) / 1.22) * 10.0);

	delta_adc_temp = abs(current_adc_temp - previous_adc_temp);

	if ((delta_adc_temp >= 10) || (current_adc_temp < 50)) previous_adc_temp = current_adc_temp;
	else current_adc_temp = previous_adc_temp;

	return current_adc_temp;
}


unsigned int update_Top_Temp_IC() {

	static unsigned int current_top_temp;
	static unsigned int previous_top_temp;
	static unsigned int delta_top_temp;

	if (M1_temp_IC >+ M2_temp_IC) current_top_temp = M1_temp_IC;
//	else current_top_temp = M2_temp_IC;
	else current_top_temp = M1_temp_IC;						// alternative (ONLY M1_TEMP_IC)
	delta_top_temp = abs(current_top_temp - previous_top_temp);

	if ((delta_top_temp > 10) || (current_top_temp < 50)) previous_top_temp = current_top_temp;
	else current_top_temp = previous_top_temp;

	return current_top_temp;	
}


int update_Ram_IC() {

	extern int __heap_start, *__brkval; 
	int v; 
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}


unsigned int update_Bottom_Ram_IC() {

	static unsigned int current_bottom_ram;
	static unsigned int previous_bottom_ram;
	static unsigned int delta_bottom_ram;

	if (M1_ram_IC <= M2_ram_IC) current_bottom_ram = M1_ram_IC;
	else current_bottom_ram = M2_ram_IC;
//	else current_buttom_ram = M1_ram_IC;					// alternative (ONLY M1_RAM_IC)

	delta_bottom_ram = abs(current_bottom_ram - previous_bottom_ram);

	if ((delta_bottom_ram > 10) || (current_bottom_ram < 600)) previous_bottom_ram = current_bottom_ram;
	else current_bottom_ram = previous_bottom_ram;

	return current_bottom_ram;	
}


void check_Serial () {										// serial port communication function

	if (Serial) serial_open = true;								// checks if serial port is open
	else serial_open = false;
}


void M1_read_Data_M2() {

	static union M2_dataItem {byte b[2]; unsigned int i;} M2_data_item_1, M2_data_item_2, M2_data_item_3, M2_data_item_4, M2_data_item_5, M2_data_item_6, M2_data_item_7,
							   M2_crc_item_1, M2_crc_item_2, M2_crc_item_3, M2_crc_item_4, M2_crc_item_5, M2_crc_item_6, M2_crc_item_7;

	static boolean M2_data_ready;

	M2_data_ready = false;

	if (M2_data_in[0] == M2_data_in[1]) {		// PACKAGE IDENTIFIER VERIFICATION

		cli();								

			M2_data_item_1.b[0] = M2_data_in[2];
			M2_data_item_1.b[1] = M2_data_in[3];
			M2_crc_item_1.b[0] = M2_data_in[4];
			M2_crc_item_1.b[1] = M2_data_in[5];
			M2_data_item_2.b[0] = M2_data_in[6];
			M2_data_item_2.b[1] = M2_data_in[7];
			M2_crc_item_2.b[0] = M2_data_in[8];
			M2_crc_item_2.b[1] = M2_data_in[9];
			M2_data_item_3.b[0] = M2_data_in[10];
			M2_data_item_3.b[1] = M2_data_in[11];
			M2_crc_item_3.b[0] = M2_data_in[12];
			M2_crc_item_3.b[1] = M2_data_in[13];
			M2_data_item_4.b[0] = M2_data_in[14];
			M2_data_item_4.b[1] = M2_data_in[15];
			M2_crc_item_4.b[0] = M2_data_in[16];
			M2_crc_item_4.b[1] = M2_data_in[17];
			M2_data_item_5.b[0] = M2_data_in[18];
			M2_data_item_5.b[1] = M2_data_in[19];
			M2_crc_item_5.b[0] = M2_data_in[20];
			M2_crc_item_5.b[1] = M2_data_in[21];
			M2_data_item_6.b[0] = M2_data_in[22];
			M2_data_item_6.b[1] = M2_data_in[23];
			M2_crc_item_6.b[0] = M2_data_in[24];
			M2_crc_item_6.b[1] = M2_data_in[25];
			M2_data_item_7.b[0] = M2_data_in[26];
			M2_data_item_7.b[1] = M2_data_in[27];
			M2_crc_item_7.b[0] = M2_data_in[28];
			M2_crc_item_7.b[1] = M2_data_in[29];

		sei();

		M2_data_ready = true;								
	}

	M2_got_data = false;

	if (M2_data_ready) {

		switch (M2_data_in[0]) {		// PACKAGE IDENTIFIER			

			case (M2_PACKAGE_1):

				if (M2_data_item_1.i == M2_crc_item_1.i) {					// SYSTEM_CONTROL_2

					if (M2_data_item_1.i == 100) system_control_2 = false;
					if (M2_data_item_1.i == 200) system_control_2 = true;
				}

 				if (M2_data_item_2.i == M2_crc_item_2.i) {					// FAN_MODE_1

					if (M2_data_item_2.i == 300) fan_mode_1 = false;
					if (M2_data_item_2.i == 400) fan_mode_1 = true;
				}

 				if (M2_data_item_3.i == M2_crc_item_3.i) {					// FAN_MODE_2

					if (M2_data_item_3.i == 500) fan_mode_2 = false;
					if (M2_data_item_3.i == 600) fan_mode_2 = true;
				}

				if (M2_data_item_4.i == M2_crc_item_4.i) fan_control_1 = M2_data_item_4.i;	// FAN_CONTROL_1

				if (M2_data_item_5.i == M2_crc_item_5.i) fan_control_2 = M2_data_item_5.i;	// FAN_CONTROL_2

//				if (M2_data_item_6.i == M2_crc_item_6.i) = M2_data_item_6.i;			// PLACE HOLDER

//				if (M2_data_item_7.i == M2_crc_item_7.i) = M2_data_item_7.i;			// PLACE HOLDER
				
				break;


			case (M2_PACKAGE_2):

				if (M2_data_item_1.i == M2_crc_item_1.i) temp_AN1 = M2_data_item_1.i;		// TEMP_AN1

				if (M2_data_item_2.i == M2_crc_item_2.i) temp_AN2 = M2_data_item_2.i;		// TEMP_AN2

				if (M2_data_item_3.i == M2_crc_item_3.i) temp_OW_1 = M2_data_item_3.i;		// TEMP_OW_1

				if (M2_data_item_4.i == M2_crc_item_4.i) fan_rpm_5 = M2_data_item_4.i;		// FAN_RPM_5

				if (M2_data_item_5.i == M2_crc_item_5.i) freq_PWM_1 = M2_data_item_5.i;		// FREQ_PWM_1

				if (M2_data_item_6.i == M2_crc_item_6.i) freq_PWM_2 = M2_data_item_6.i;		// freq_PWM_2

				if (M2_data_item_7.i == M2_crc_item_7.i) temp_OW_2 = M2_data_item_7.i;		// TEMP_OW_2

				break;


			case (M2_PACKAGE_3):

				if (M2_data_item_1.i == M2_crc_item_1.i) M2_vcc_IC = M2_data_item_1.i;		// M2_VCC_IC

				if (M2_data_item_2.i == M2_crc_item_2.i) M2_temp_IC = M2_data_item_2.i;		// M2_TEMP_IC	

				if (M2_data_item_3.i == M2_crc_item_3.i) M2_ram_IC = M2_data_item_3.i;		// M2_RAM_IC

				if (M2_data_item_4.i == M2_crc_item_4.i) top_temp_AN = M2_data_item_4.i;	// TEMP_AN_TOP

				if (M2_data_item_5.i == M2_crc_item_5.i) temp_OW_3 = M2_data_item_5.i;		// TEMP_OW_3

				if (M2_data_item_6.i == M2_crc_item_6.i) temp_I2C_1 = M2_data_item_6.i;		// TEMP_I2C_1

				if (M2_data_item_7.i == M2_crc_item_7.i) temp_I2C_2 = M2_data_item_7.i;		// TEMP_I2C_2

				break;
		}
	}
}
		

void M1_Send_Data_PC (const byte pc_package_identifier) { 

	static byte pc_data_out[PC_DATA_OUT_COUNT];
	static union dataItem {byte b[2]; unsigned int i;} pc_data_item_1, pc_data_item_2, pc_data_item_3, pc_data_item_4, pc_data_item_5, pc_data_item_6, pc_data_item_7,
							   pc_crc_item_1, pc_crc_item_2, pc_crc_item_3, pc_crc_item_4, pc_crc_item_5, pc_crc_item_6, pc_crc_item_7;

	switch (pc_package_identifier) {

		case(PC_PACKAGE_1):

			pc_data_out[0] = PC_PACKAGE_1;
			pc_data_out[1] = PC_PACKAGE_1;

			if (!system_control_1) pc_data_item_1.i = 0;
			else pc_data_item_1.i = 1;
			pc_crc_item_1.i = (FC_SYSTEM_CONTROL_1 + pc_data_item_1.i);
			if (pc_crc_item_1.i <= 9999) pc_crc_item_1.i += 9999;

			if (!system_control_2) pc_data_item_2.i = 0;
			else pc_data_item_2.i = 1;
			pc_crc_item_2.i = (FC_SYSTEM_CONTROL_2 + pc_data_item_2.i);
			if (pc_crc_item_2.i <= 9999) pc_crc_item_2.i += 9999;

			if (!fan_mode_1) pc_data_item_3.i = 0;
			else pc_data_item_3.i = 1;
			pc_crc_item_3.i = (FC_FAN_MODE_1 + pc_data_item_3.i);
			if (pc_crc_item_3.i <= 9999) pc_crc_item_3.i += 9999;

			if (!fan_mode_2) pc_data_item_4.i = 0;
			else pc_data_item_4.i = 1;
			pc_crc_item_4.i = (FC_FAN_MODE_2 + pc_data_item_4.i);
			if (pc_crc_item_4.i <= 9999) pc_crc_item_4.i += 9999;

			pc_data_item_5.i = duty_cycle_per_1;
			pc_crc_item_5.i = (FC_DUTY_CYCLE_PER_1 + pc_data_item_5.i);
			if (pc_crc_item_5.i <= 9999) pc_crc_item_5.i += 9999;

			pc_data_item_6.i = duty_cycle_per_2;
			pc_crc_item_6.i = (FC_DUTY_CYCLE_PER_2 + pc_data_item_6.i);
			if (pc_crc_item_6.i <= 9999) pc_crc_item_6.i += 9999;

			pc_data_item_7.i = temp_OW_2;
			pc_crc_item_7.i = (FC_TEMP_OW_3 + pc_data_item_7.i);
			if (pc_crc_item_7.i <= 9999) pc_crc_item_7.i += 9999;

			break;
		

		case(PC_PACKAGE_2):

			pc_data_out[0] = PC_PACKAGE_2;
			pc_data_out[1] = PC_PACKAGE_2;

			pc_data_item_1.i = fan_vcc_1;
			pc_crc_item_1.i = (FC_FAN_VCC_1 + pc_data_item_1.i);
			if (pc_crc_item_1.i <= 9999) pc_crc_item_1.i += 9999;

			pc_data_item_2.i = fan_vcc_2;
			pc_crc_item_2.i = (FC_FAN_VCC_2 + pc_data_item_2.i);
			if (pc_crc_item_2.i <= 9999) pc_crc_item_2.i += 9999;

			pc_data_item_3.i = freq_VCC_1;
			pc_crc_item_3.i = (FC_FREQ_VCC_1 + pc_data_item_3.i);
			if (pc_crc_item_3.i <= 9999) pc_crc_item_3.i += 9999;

			pc_data_item_4.i = freq_VCC_2;
			pc_crc_item_4.i = (FC_FREQ_VCC_2 + pc_data_item_4.i);
			if (pc_crc_item_4.i <= 9999) pc_crc_item_4.i += 9999;

			pc_data_item_5.i = freq_PWM_1;
			pc_crc_item_5.i = (FC_FREQ_PWM_1 + pc_data_item_5.i);
			if (pc_crc_item_5.i <= 9999) pc_crc_item_5.i += 9999;

			pc_data_item_6.i = freq_PWM_2;
			pc_crc_item_6.i = (FC_FREQ_PWM_2 + pc_data_item_6.i);
			if (pc_crc_item_6.i <= 9999) pc_crc_item_6.i += 9999;

			pc_data_item_7.i = temp_OW_3;
			pc_crc_item_7.i = (FC_TEMP_OW_4 + pc_data_item_7.i);
			if (pc_crc_item_7.i <= 9999) pc_crc_item_7.i += 9999;

			break;

		
		case(PC_PACKAGE_3):

			pc_data_out[0] = PC_PACKAGE_3;
			pc_data_out[1] = PC_PACKAGE_3;

			pc_data_item_1.i = temp_AN1;
			pc_crc_item_1.i = (FC_TEMP_AN1 + pc_data_item_1.i);
			if (pc_crc_item_1.i <= 9999) pc_crc_item_1.i += 9999;

			pc_data_item_2.i = temp_AN2;
			pc_crc_item_2.i = (FC_TEMP_AN2 + pc_data_item_2.i);
			if (pc_crc_item_2.i <= 9999) pc_crc_item_2.i += 9999;

			pc_data_item_3.i = temp_OW_1;
			pc_crc_item_3.i = (FC_TEMP_DGF + pc_data_item_3.i);
			if (pc_crc_item_3.i <= 9999) pc_crc_item_3.i += 9999;

			pc_data_item_4.i = top_temp_IC;
			pc_crc_item_4.i = (FC_TEMP_ICF + pc_data_item_4.i);
			if (pc_crc_item_4.i <= 9999) pc_crc_item_4.i += 9999;

			pc_data_item_5.i = bottom_vcc_IC;
			pc_crc_item_5.i = (FC_VCC_ICF + pc_data_item_5.i);
			if (pc_crc_item_5.i <= 9999) pc_crc_item_5.i += 9999;

			pc_data_item_6.i = bottom_ram_IC;
			pc_crc_item_6.i = (FC_RAM_ICF + pc_data_item_6.i);
			if (pc_crc_item_6.i <= 9999) pc_crc_item_6.i += 9999;

			pc_data_item_7.i = temp_I2C_1;
			pc_crc_item_7.i = (FC_TEMP_I2C_3 + pc_data_item_7.i);
			if (pc_crc_item_7.i <= 9999) pc_crc_item_7.i += 9999;

			break;

		
		case(PC_PACKAGE_4):

			pc_data_out[0] = PC_PACKAGE_4;
			pc_data_out[1] = PC_PACKAGE_4;

			pc_data_item_1.i = fan_rpm_1;
			pc_crc_item_1.i = (FC_FAN_RPM_1 + pc_data_item_1.i);
			if (pc_crc_item_1.i <= 9999) pc_crc_item_1.i += 9999;

			pc_data_item_2.i = fan_rpm_2;
			pc_crc_item_2.i = (FC_FAN_RPM_2 + pc_data_item_2.i);
			if (pc_crc_item_2.i <= 9999) pc_crc_item_2.i += 9999;

			pc_data_item_3.i = fan_rpm_3;
			pc_crc_item_3.i = (FC_FAN_RPM_3 + pc_data_item_3.i);
			if (pc_crc_item_3.i <= 9999) pc_crc_item_3.i += 9999;

			pc_data_item_4.i = fan_rpm_4;
			pc_crc_item_4.i = (FC_FAN_RPM_4 + pc_data_item_4.i);
			if (pc_crc_item_4.i <= 9999) pc_crc_item_4.i += 9999;

			pc_data_item_5.i = fan_rpm_5;
			pc_crc_item_5.i = (FC_FAN_RPM_5 + pc_data_item_5.i);
			if (pc_crc_item_5.i <= 9999) pc_crc_item_5.i += 9999;

			pc_data_item_6.i = fan_rpm_6;
			pc_crc_item_6.i = (FC_FAN_RPM_6 + pc_data_item_6.i);
			if (pc_crc_item_6.i <= 9999) pc_crc_item_6.i += 9999;

			pc_data_item_7.i = temp_I2C_2;
			pc_crc_item_7.i = (FC_TEMP_I2C_4 + pc_data_item_7.i);
			if (pc_crc_item_7.i <= 9999) pc_crc_item_7.i += 9999;

			break;
	}

	pc_data_out[2] = pc_data_item_1.b[0];
	pc_data_out[3] = pc_data_item_1.b[1];

	pc_data_out[4] = pc_crc_item_1.b[0];
	pc_data_out[5] = pc_crc_item_1.b[1];

	pc_data_out[6] = pc_data_item_2.b[0];
	pc_data_out[7] = pc_data_item_2.b[1];

	pc_data_out[8] = pc_crc_item_2.b[0];
	pc_data_out[9] = pc_crc_item_2.b[1];

	pc_data_out[10] = pc_data_item_3.b[0];
	pc_data_out[11] = pc_data_item_3.b[1];

	pc_data_out[12] = pc_crc_item_3.b[0];
	pc_data_out[13] = pc_crc_item_3.b[1];

	pc_data_out[14] = pc_data_item_4.b[0];
	pc_data_out[15] = pc_data_item_4.b[1];

	pc_data_out[16] = pc_crc_item_4.b[0];
	pc_data_out[17] = pc_crc_item_4.b[1];

	pc_data_out[18] = pc_data_item_5.b[0];
	pc_data_out[19] = pc_data_item_5.b[1];

	pc_data_out[20] = pc_crc_item_5.b[0];
	pc_data_out[21] = pc_crc_item_5.b[1];

	pc_data_out[22] = pc_data_item_6.b[0];
	pc_data_out[23] = pc_data_item_6.b[1];

	pc_data_out[24] = pc_crc_item_6.b[0];
	pc_data_out[25] = pc_crc_item_6.b[1];

	pc_data_out[26] = pc_data_item_7.b[0];
	pc_data_out[27] = pc_data_item_7.b[1];

	pc_data_out[28] = pc_crc_item_7.b[0];
	pc_data_out[29] = pc_crc_item_7.b[1];

	Wire.beginTransmission (PC_ADDRESS);

	Wire.write(pc_data_out, PC_DATA_OUT_COUNT);

	if (Wire.endTransmission() == 0) pc_data_flag = 0;
	else pc_data_flag += 1;

	if (pc_data_flag > 20) pc_data_flag = 20;
}


void M1_Short_Processing_Print() {

	M1_Processing_Print_Data(FC_SYSTEM_CONTROL_1);
	M1_Processing_Print_Data(FC_SYSTEM_CONTROL_2);
	M1_Processing_Print_Data(FC_FAN_MODE_1);
	M1_Processing_Print_Data(FC_DUTY_CYCLE_PER_1);
	M1_Processing_Print_Data(FC_FAN_MODE_2);
	M1_Processing_Print_Data(FC_DUTY_CYCLE_PER_2);
}


void M1_Long_Processing_Print() {

	M1_Processing_Print_Data(FC_FAN_VCC_1);
	M1_Processing_Print_Data(FC_FAN_VCC_2);
	M1_Processing_Print_Data(FC_VCC_ICF);
	M1_Processing_Print_Data(FC_FAN_RPM_1);
	M1_Processing_Print_Data(FC_FAN_RPM_2);
	M1_Processing_Print_Data(FC_FAN_RPM_3);
	M1_Processing_Print_Data(FC_FAN_RPM_4);
	M1_Processing_Print_Data(FC_FAN_RPM_5);
	M1_Processing_Print_Data(FC_FAN_RPM_6);
	M1_Processing_Print_Data(FC_TEMP_AN1);
	M1_Processing_Print_Data(FC_TEMP_AN2);
	M1_Processing_Print_Data(FC_TEMP_DGF);
	M1_Processing_Print_Data(FC_TEMP_ICF);
	M1_Processing_Print_Data(FC_FREQ_VCC_1);
	M1_Processing_Print_Data(FC_FREQ_PWM_1);
	M1_Processing_Print_Data(FC_FREQ_VCC_2);
	M1_Processing_Print_Data(FC_FREQ_PWM_2);
	M1_Processing_Print_Data(FC_RAM_ICF);
	if (temp_OW_2 > 0) temp_OW_1_present = true;
	if (temp_OW_3 > 0) temp_OW_2_present = true;
	if (temp_I2C_1 > 0) temp_I2C_1_present = true;
	if (temp_I2C_2 > 0) temp_I2C_1_present = true;
	if (temp_OW_2_present) M1_Processing_Print_Data(FC_TEMP_OW_3);
	if (temp_OW_3_present) M1_Processing_Print_Data(FC_TEMP_OW_4);
	if (temp_I2C_1_present) M1_Processing_Print_Data(FC_TEMP_I2C_3);
	if (temp_I2C_2_present) M1_Processing_Print_Data(FC_TEMP_I2C_4);
}


void M1_Processing_Print_Data (const unsigned int processing_type) {

	static unsigned int processing_current_data;
	static unsigned int processing_previous_data[18];
	static unsigned int processing_crc;
	static float processing_new_data;
	static byte processing_decimal_places;
	static boolean processing_update_data;

	processing_update_data = false;

	switch (processing_type) {

		case (FC_SYSTEM_CONTROL_1):						// OVD_OFF/OVD_ON
			processing_current_data = (unsigned int)system_control_1;
			processing_new_data = float(processing_current_data);
			processing_decimal_places = 0;
			processing_update_data = true;					
			break;

		case (FC_SYSTEM_CONTROL_2):						// MAN/AUTO
			processing_current_data = (unsigned int)system_control_2;
			processing_new_data = float(processing_current_data);
			processing_decimal_places = 0;
			processing_update_data = true;					
			break;

		case (FC_FAN_VCC_1):							// FAN_VCC_1
			processing_current_data = fan_vcc_1;
			processing_new_data = (float(processing_current_data) / 1000.0);
			processing_decimal_places = 3;
			processing_update_data = true;					
			break;

		case (FC_FAN_VCC_2):							// FAN_VCC_2
			processing_current_data = fan_vcc_2;
			processing_new_data = (float(processing_current_data) / 1000.0);
			processing_decimal_places = 3;
			processing_update_data = true;					
			break;

		case (FC_VCC_ICF):							// VCC_ICF (BOTTOM)
			processing_current_data = bottom_vcc_IC;
			if ((processing_current_data != processing_previous_data[0]) ||		// UPDATE
			   (processing_current_data == 0) ||
			   (currentStartupCount < STARTUP_INTERVAL)) {
				processing_previous_data[0] = processing_current_data;
				processing_new_data = (float(processing_current_data) / 1000.0);
				processing_decimal_places = 3;
				processing_update_data = true;					
			}
			break;

		case (FC_FAN_MODE_1):							// VCC_1/PWM_1
			processing_current_data = (unsigned int)fan_mode_1;
			processing_new_data = float(processing_current_data);
			processing_decimal_places = 0;
			processing_update_data = true;					
			break;

		case (FC_FAN_RPM_1):							// FAN_RPM_1
			processing_current_data = fan_rpm_1;
			if ((processing_current_data != processing_previous_data[1]) ||		// UPDATE
			   (processing_current_data == 0) ||
			   (currentStartupCount < STARTUP_INTERVAL)) {
				processing_previous_data[1] = processing_current_data;
				processing_new_data = float(processing_current_data);
				processing_decimal_places = 0;
				processing_update_data = true;					
			}
			break;

		case (FC_FAN_RPM_2):							// FAN_RPM_2
			processing_current_data = fan_rpm_2;
			if ((processing_current_data != processing_previous_data[2]) ||		// UPDATE
			   (processing_current_data == 0) ||
			   (currentStartupCount < STARTUP_INTERVAL)) {
				processing_previous_data[2] = processing_current_data;
				processing_new_data = float(processing_current_data);
				processing_decimal_places = 0;
				processing_update_data = true;					
			}
			break;

		case (FC_FAN_RPM_3):							// FAN_RPM_3
			processing_current_data = fan_rpm_3;
			if ((processing_current_data != processing_previous_data[3]) ||		// UPDATE
			   (processing_current_data == 0) ||
			   (currentStartupCount < STARTUP_INTERVAL)) {
				processing_previous_data[3] = processing_current_data;
				processing_new_data = float(processing_current_data);
				processing_decimal_places = 0;
				processing_update_data = true;					
			}
			break;

		case (FC_FAN_RPM_4):							// FAN_RPM_4
			processing_current_data = fan_rpm_4;
			if ((processing_current_data != processing_previous_data[4]) ||		// UPDATE
			   (processing_current_data == 0) ||
			   (currentStartupCount < STARTUP_INTERVAL)) {
				processing_previous_data[4] = processing_current_data;
				processing_new_data = float(processing_current_data);
				processing_decimal_places = 0;
				processing_update_data = true;					
			}
			break;

		case (FC_DUTY_CYCLE_PER_1):						// DUTY_CYCLE_PER_1
			processing_current_data = duty_cycle_per_1;
			processing_new_data = float(processing_current_data);
			processing_decimal_places = 0;
			processing_update_data = true;					
			break;

		case (FC_FAN_MODE_2):							// VCC_2/PWM_2
			processing_current_data = (unsigned int)fan_mode_2;
			processing_new_data = float(processing_current_data);
			processing_decimal_places = 0;
			processing_update_data = true;					
			break;

		case (FC_FAN_RPM_5):							// FAN_RPM_5
			processing_current_data = fan_rpm_5;
			if ((processing_current_data != processing_previous_data[5]) ||		// UPDATE
			   (processing_current_data == 0) ||
			   (currentStartupCount < STARTUP_INTERVAL)) {
				processing_previous_data[5] = processing_current_data;
				processing_new_data = float(processing_current_data);
				processing_decimal_places = 0;
				processing_update_data = true;					
			}
			break;

		case (FC_FAN_RPM_6):							// FAN_RPM_6
			processing_current_data = fan_rpm_6;
			if ((processing_current_data != processing_previous_data[6]) ||		// UPDATE
			   (processing_current_data == 0) ||
			   (currentStartupCount < STARTUP_INTERVAL)) {
				processing_previous_data[6] = processing_current_data;
				processing_new_data = float(processing_current_data);
				processing_decimal_places = 0;
				processing_update_data = true;					
			}
			break;

		case (FC_DUTY_CYCLE_PER_2):						// DUTY_CYCLE_PER_2
			processing_current_data = duty_cycle_per_2;
			processing_new_data = float(processing_current_data);
			processing_decimal_places = 0;
			processing_update_data = true;					
			break;

		case (FC_TEMP_AN1):							// TEMP_AN3
			processing_current_data = temp_AN1;
			processing_new_data = (float(processing_current_data) / 10.0);
			processing_decimal_places = 1;
			processing_update_data = true;					
			break;

		case (FC_TEMP_AN2):							// TEMP_AN4
			processing_current_data = temp_AN2;
			processing_new_data = (float(processing_current_data) / 10.0);
			processing_decimal_places = 1;
			processing_update_data = true;					
			break;

		case (FC_TEMP_DGF):							// TEMP_DGF (TEMP_OW_1)
			processing_current_data = temp_OW_1;
			if ((processing_current_data != processing_previous_data[7]) ||		// UPDATE
			   (processing_current_data == 0) ||
			   (currentStartupCount < STARTUP_INTERVAL)) {
				processing_previous_data[7] = processing_current_data;
				processing_new_data = (float(processing_current_data) / 10.0);
				processing_decimal_places = 1;
				processing_update_data = true;					
			}
			break;

		case (FC_TEMP_ICF):							// TEMP_ICF (TOP)
			processing_current_data = top_temp_IC;
			if ((processing_current_data != processing_previous_data[8]) ||		// UPDATE
			   (processing_current_data == 0) ||
			   (currentStartupCount < STARTUP_INTERVAL)) {
				processing_previous_data[8] = processing_current_data;
				processing_new_data = (float(processing_current_data) / 10.0);
				processing_decimal_places = 1;
				processing_update_data = true;					
			}
			break;

		case (FC_FREQ_VCC_1):							// FREQ_VCC_1
			processing_current_data = freq_VCC_1;
			if ((processing_current_data != processing_previous_data[9]) ||		// UPDATE
			   (processing_current_data == 0) ||
			   (currentStartupCount < STARTUP_INTERVAL)) {
				processing_previous_data[9] = processing_current_data;
				processing_new_data = (float(processing_current_data) / 1000.0);
				processing_decimal_places = 3;
				processing_update_data = true;					
			}
			break;

		case (FC_FREQ_PWM_1):							// FREQ_PWM_1
			processing_current_data = freq_PWM_1;
			if ((processing_current_data != processing_previous_data[10]) ||	// UPDATE
			   (processing_current_data == 0) ||
			   (currentStartupCount < STARTUP_INTERVAL)) {
				processing_previous_data[10] = processing_current_data;
				processing_new_data = (float(processing_current_data) / 1000.0);
				processing_decimal_places = 3;
				processing_update_data = true;					
			}
			break;

		case (FC_FREQ_VCC_2):							// FREQ_VCC_2
			processing_current_data = freq_VCC_2;
			if ((processing_current_data != processing_previous_data[11]) ||	// UPDATE
			   (processing_current_data == 0) ||
			   (currentStartupCount < STARTUP_INTERVAL)) {
				processing_previous_data[11] = processing_current_data;
				processing_new_data = (float(processing_current_data) / 1000.0);
				processing_decimal_places = 3;
				processing_update_data = true;					
			}
			break;

		case (FC_FREQ_PWM_2):							// FREQ_PWM_2
			processing_current_data = freq_PWM_2;
			if ((processing_current_data != processing_previous_data[12]) ||	// UPDATE
			   (processing_current_data == 0) ||
			   (currentStartupCount < STARTUP_INTERVAL)) {
				processing_previous_data[12] = processing_current_data;
				processing_new_data = (float(processing_current_data) / 1000.0);
				processing_decimal_places = 3;
				processing_update_data = true;					
			}
			break;

		case (FC_RAM_ICF):							// RAM_ICF (BOTTOM)
			processing_current_data = bottom_ram_IC;
			if ((processing_current_data != processing_previous_data[13]) ||	// UPDATE
			   (processing_current_data <= 800) ||
			   (currentStartupCount < STARTUP_INTERVAL)) {
				processing_previous_data[13] = processing_current_data;
				processing_new_data = float(processing_current_data);
				processing_decimal_places = 0;
				processing_update_data = true;					
			}
			break;

		case (FC_TEMP_OW_3):							// TEMP_OW_2
			processing_current_data = temp_OW_2;
//			if ((processing_current_data != processing_previous_data[14]) ||		// UPDATE
//			   (processing_current_data == 0) ||
//			   (currentStartupCount < STARTUP_INTERVAL)) {
				processing_previous_data[14] = processing_current_data;
				processing_new_data = (float(processing_current_data) / 10.0);
				processing_decimal_places = 1;
				processing_update_data = true;					
//			}
			break;

		case (FC_TEMP_OW_4):							// TEMP_OW_3
			processing_current_data = temp_OW_3;
//			if ((processing_current_data != processing_previous_data[15]) ||		// UPDATE
//			   (processing_current_data == 0) ||
//			   (currentStartupCount < STARTUP_INTERVAL)) {
				processing_previous_data[15] = processing_current_data;
				processing_new_data = (float(processing_current_data) / 10.0);
				processing_decimal_places = 1;
				processing_update_data = true;					
//			}
			break;

		case (FC_TEMP_I2C_3):							// TEMP_I2C_1
			processing_current_data = temp_I2C_1;
//			if ((processing_current_data != processing_previous_data[16]) ||		// UPDATE
//			   (processing_current_data == 0) ||
//			   (currentStartupCount < STARTUP_INTERVAL)) {
				processing_previous_data[16] = processing_current_data;
				processing_new_data = (float(processing_current_data) / 10.0);
				processing_decimal_places = 1;
				processing_update_data = true;					
//			}
			break;

		case (FC_TEMP_I2C_4):							// TEMP_I2C_2
			processing_current_data = temp_I2C_2;
//			if ((processing_current_data != processing_previous_data[17]) ||		// UPDATE
//			   (processing_current_data == 0) ||
//			   (currentStartupCount < STARTUP_INTERVAL)) {
				processing_previous_data[17] = processing_current_data;
				processing_new_data = (float(processing_current_data) / 10.0);
				processing_decimal_places = 1;
				processing_update_data = true;					
//			}
			break;
	}

	if (processing_update_data) {
	
		processing_crc = (processing_type + processing_current_data);
		if (processing_crc <= 9999) processing_crc += 9999;
		Serial.print(processing_type);					// PROCESSING_TYPE
		Serial.print(F(","));
		Serial.print(processing_crc);					// PROCESSING_CRC
		Serial.print(F(","));
	      	Serial.print(processing_new_data, processing_decimal_places); 	// PROCESSING_DATA
 	      	Serial.print(F("!"));
	}
}


void print_Long_Update() {

	if (PRINT_FIRST_ROW) {

		Serial.println(F(""));
		Serial.println(F(""));

		Serial.print(F("FREQ_VCC_1: "));		
		Serial.print((float(freq_VCC_1)/1000.0), 3);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("FREQ_VCC_2: "));		
		Serial.print((float(freq_VCC_2)/1000.0), 3);
		Serial.print('\t');
		Serial.print('\t');

		if (!system_control_1) Serial.print(F("OVD_OFF"));
		else Serial.print(F("OVD_ON"));
		Serial.print('\t');
		Serial.print('\t');

		if (!fan_mode_1) Serial.print(F("VCC_1"));
		else Serial.print(F("PWM_1"));
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("FAN_CONTROL_1: "));		
		Serial.print(fan_control_1);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("VCC_DRIVE_1: "));		
		Serial.print(M1_duty_cycle_1);
		Serial.print('\t');

		Serial.print(F("PWM_DRIVE_1: "));		
		Serial.print(M2_duty_cycle_1);
		Serial.print('\t');

		Serial.print(F("duty_cycle_per_1: "));		
		Serial.print(duty_cycle_per_1);
		Serial.print(F("%"));	
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("FAN_VCC_1: "));		
		Serial.print((float(fan_vcc_1)/1000.0), 3);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("TEMP_AN1: "));		
		Serial.print((float(temp_AN1)/10.0), 1);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("TOP_TEMP_AN: "));	
		Serial.print((float(top_temp_AN)/10.0), 1);
	}

	if (PRINT_SECOND_ROW) {

		Serial.println(F(""));
		Serial.println(F(""));

		Serial.print(F("FREQ_PWM_1: "));		
		Serial.print((float(freq_PWM_1)/1000.0), 3);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("FREQ_PWM_2: "));		
		Serial.print((float(freq_PWM_2)/1000.0), 3);
		Serial.print('\t');
		Serial.print('\t');

		if (!system_control_2) Serial.print(F("MAN"));
		else Serial.print(F("AUTO"));
		Serial.print('\t');
		Serial.print('\t');

		if (!fan_mode_2) Serial.print(F("VCC_2"));
		else Serial.print(F("PWM_2"));
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("FAN_CONTROL_2: "));		
		Serial.print(fan_control_2);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("VCC_DRIVE_2: "));		
		Serial.print(M1_duty_cycle_2);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("PWM_DRIVE_2: "));		
		Serial.print(M2_duty_cycle_2);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("duty_cycle_per_2: "));		
		Serial.print(duty_cycle_per_2);
		Serial.print(F("%"));	
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("FAN_VCC_2: "));		
		Serial.print((float(fan_vcc_2)/1000.0), 3);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("TEMP_AN2: "));		
		Serial.print((float(temp_AN2)/10.0), 1);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("TOP_TEMP_AN: "));
		Serial.print((float(top_temp_AN)/10.0), 1);
	}

	if (PRINT_THIRD_ROW) {

		Serial.println(F(""));
		Serial.println(F(""));

		Serial.print(F("M1_VCC_IC: "));		
		Serial.print((float(M1_vcc_IC)/1000.0), 3);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("M2_VCC_IC: "));		
		Serial.print((float(M2_vcc_IC)/1000.0), 3);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("BOTTOM_VCC_IC: "));		
		Serial.print((float(bottom_vcc_IC)/1000.0), 3);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("M1_TEMP_IC: "));		
		Serial.print((float(M1_temp_IC)/10.0), 1);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("M2_TEMP_IC: "));		
		Serial.print((float(M2_temp_IC)/10.0), 1);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("TOP_TEMP_IC: "));		
		Serial.print((float(top_temp_IC)/10.0), 1);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("M1_RAM_IC: "));		
		Serial.print(M1_ram_IC);

		Serial.print(F("M2_RAM_IC: "));		
		Serial.print(M2_ram_IC);

		Serial.print(F("BOTTOM_RAM_IC: "));		
		Serial.print(bottom_ram_IC);
	}

	if (PRINT_FOURTH_ROW) {

		Serial.print(F("FAN_RPM_1: "));		
		Serial.print(fan_rpm_1);
		Serial.print('\t');
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("FAN_RPM_2: "));		
		Serial.print(fan_rpm_2);
		Serial.print('\t');
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("FAN_RPM_3: "));		
		Serial.print(fan_rpm_3);
		Serial.print('\t');
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("FAN_RPM_4: "));		
		Serial.print(fan_rpm_4);
		Serial.print('\t');
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("FAN_RPM_5: "));		
		Serial.print(fan_rpm_5);
		Serial.print('\t');
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("FAN_RPM_6: "));		
		Serial.print(fan_rpm_6);
		Serial.print('\t');
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("TEMP_OW_1: "));		
		Serial.print((float(temp_OW_1)/10.0), 1);
		Serial.print('\t');
		Serial.print('\t');
		Serial.print('\t');

		if (temp_OW_2 > 0) temp_OW_1_present = true;
		if (temp_OW_3 > 0) temp_OW_2_present = true;
		if (temp_I2C_1 > 0) temp_I2C_1_present = true;
		if (temp_I2C_2 > 0) temp_I2C_1_present = true;

		if (temp_OW_2_present) {
			Serial.print(F("TEMP_OW_2: "));		
			Serial.print((float(temp_OW_2)/10.0), 1);
			Serial.print('\t');
			Serial.print('\t');
			Serial.print('\t');
		}

		if (temp_OW_3_present) {
			Serial.print(F("TEMP_OW_3: "));		
			Serial.print((float(temp_OW_3)/10.0), 1);
			Serial.print('\t');
			Serial.print('\t');
			Serial.print('\t');
		}

		if (temp_I2C_1_present) {
			Serial.print(F("TEMP_I2C_1: "));		
			Serial.print((float(temp_I2C_1)/10.0), 1);
			Serial.print('\t');
			Serial.print('\t');
			Serial.print('\t');
		}

		if (temp_I2C_2_present) {
			Serial.print(F("TEMP_I2C_2: "));		
			Serial.print((float(temp_I2C_2)/10.0), 1);
			Serial.print('\t');
			Serial.print('\t');
			Serial.print('\t');
		}
	}
}



