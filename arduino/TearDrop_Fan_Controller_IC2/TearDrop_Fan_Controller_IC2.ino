
// TearDrop
// Fan Controller 
// IC 2
// Version 99.0
// ATmega328P
// Created by Nadav Matalon
// 24 August 2014

// (TDCR_IC2_FAN_99)


// FAN CONTROLLER: IC_2 (COM4)					

// PIN_D2  (PIN 4)  [PD2] - FAN_RPM_5
// PIN_D3  (PIN 5)  [PD3] - PWM_DRIVE_2 (F_5-6) 
// PIN_D4  (PIN 6)  [PD4] - GND		
// PIN_D5  (PIN 11) [PD5] - GND	
// PIN_D6  (PIN 12) [PD6] - GND	
// PIN_D7  (PIN 13) [PD7] - GND
// PIN_D8  (PIN 14) [PB0] - GND
// PIN_D9  (PIN 15) [PB1] - ONE_WIRE
// PIN_D10 (PIN 16) [PB2] - PWM_DRIVE_1 (F_1-4)
// PIN_D11 (PIN 17) [PB3] - FAN_MODE_1 (VCC_1/PWM_1)
// PIN_D12 (PIN 18) [PB4] - FAN_MODE_2 (VCC_2/PWM_2)
// PIN_D13 (PIN 19) [PB5] - SYSTEM_CONTROL_2 (MAN/AUTO)
// PIN_A0  (PIN 23) [PC0] - TEMP_AN1
// PIN_A1  (PIN 24) [PC1] - FAN_CONTROL_2
// PIN_A2  (PIN 25) [PC2] - FAN_CONTROL_1
// PIN_A3  (PIN 26) [PC3] - TEMP_AN2


// http://playground.arduino.cc/Main/PinChangeInt
// http://code.google.com/p/arduino-pinchangeint/
#include <PinChangeInt.h>

// http://playground.arduino.cc/Learning/OneWire
// https://github.com/ntruchsess/arduino-OneWire/tree/master
#include <OneWire.h>

// https://github.com/steamfire/WSWireLib
#include <WSWire.h>


const boolean DEBUG_MODE = false;		// DEBUG_MODE
// const boolean DEBUG_MODE = true;		// DEBUG_MODE

// const boolean PRINT_FIRST_ROW = false;	// DEBUG_MODE
const boolean PRINT_FIRST_ROW = true;		// DEBUG_MODE

const boolean PRINT_SECOND_ROW = false;		// DEBUG_MODE
// const boolean PRINT_SECOND_ROW = true;	// DEBUG_MODE


const byte PIN_D2 = 2;				// FAN_RPM_5	
const byte PIN_D3 = 3;				// PWM_DRIVE_2 (F_5-6) 
const byte PIN_D4 = 4;				// GND
const byte PIN_D5 = 5;				// GND
const byte PIN_D6 = 6;				// GND
const byte PIN_D7 = 7;				// GND
const byte PIN_D8 = 8;				// GND
const byte PIN_D9 = 9;				// ONE_WIRE
const byte PIN_D10 = 10;			// PWM_DRIVE_1 (F_1-4)
const byte PIN_D11 = 11;			// FAN_MODE_1 (VCC_1/PWM_1)
const byte PIN_D12 = 12;			// FAN_MODE_2 (VCC_2/PWM_2)
const byte PIN_D13 = 13;			// SYSTEM_CONTROL_2 (MAN/AUTO)
const byte PIN_A0 = A0;				// TEMP_AN1
const byte PIN_A1 = A1;				// FAN_CONTROL_2
const byte PIN_A2 = A2;				// FAN_CONTROL_1
const byte PIN_A3 = A3;				// TEMP_AN2

const byte ADC_A0 = 0;				// TEMP_AN1				// ADC input channel PIN_A0
const byte ADC_A1 = 1;				// FAN_CONTROL_2 (RAW_POT_2)		// ADC input channel PIN_A1
const byte ADC_A2 = 2;				// FAN_CONTROL_1 (RAW_POT_1)		// ADC input channel PIN_A2
const byte ADC_A3 = 3;				// TEMP_AN2				// ADC input channel PIN_A3
const byte ADC_A4 = 4;									// ADC input channel PIN_A4
const byte ADC_A5 = 5;									// ADC input channel PIN_A5
const byte ADC_TEMP = 8;			// M2_TEMP_IC				// ADC input channel IC_TEMP
const byte ADC_VCC = 14;			// M2_VCC_IC				// ADC input channel IC_VCC
const byte ADC_GND = 15;								// ADC input channel IC_GND

const float TEMP_OFFSET_A0 = 1.00;							// TEMP_AN1 - calibration function: higher OFFSET precentage => lower temperature readings
const float TEMP_OFFSET_A3 = 1.00;							// TEMP_AN2 - calibration function: higher OFFSET precentage => lower temperature readings

const unsigned int TEMP_RESISTOR_A0 = 10000;	// TEMP_AN1_RESISTOR			// TEMP_AN1 - resistor value on PIN_A0 (in Ohms)
const unsigned int TEMP_RESISTOR_A3 = 10000;	// TEMP_AN2_RESISTOR			// TEMP_AN2 - resistor value on PIN_A3 (in Ohms)

volatile unsigned long ovf1 = 0;		// TIMER1_OVERFLOW			// stores M2_TIMER1 overflow count
volatile unsigned long ovf2 = 0;		// TIMER2_OVERFLOW			// stores M2_TIMER2 overflow count

volatile unsigned long currentBurp_D2 = 0;						// FAN_RPM_5 - stores rpm count for ISR on PIN_D2

OneWire wireBus(PIN_D9);  						        	// sets up One-Wire bus on PIN_D9

const byte MAX_OW_DEVICES = 3;           						// determines maximum number of One-Wire devices on bus
const byte OW_DEVICE_1 = 0;								// One-Wire device 1 location in device address arrey
const byte OW_DEVICE_2 = 1;								// One-Wire device 2 location in device address arrey
const byte OW_DEVICE_3 = 2;								// One-Wire device 3 location in device address arrey

const byte MAX_I2C_TEMP_DEVICES = 2;             					// sets maximum number of digital temperature sensors on I2C bus
const byte I2C_DEVICE_1 = 0;								// I2C_device_1 location in I2C_address arrey
const byte I2C_DEVICE_2 = 1;								// I2C_device_2 location in I2C_address arrey

const byte M2_ADDRESS = 40;			// FAN_CONTROLLER_IC2			// M2 I2C device address

const byte M1_ADDRESS = 50;			// FAN_CONTROLLER_IC1			// M1 I2C device address

// const byte PC_ADDRESS = 60;			// PC_I2C_ADDRESS			// PC I2C device address

const byte M2_DATA_IN_COUNT = 8; 							// number of bytes recieved (max: 32 bytes)
const byte M2_DATA_OUT_COUNT = 30;

const byte M2_PACKAGE_1 = 241;
const byte M2_PACKAGE_2 = 242;
const byte M2_PACKAGE_3 = 243;

unsigned int freq_PWM_1 = 0;			// FREQ_PWM_1				// stores M2 TIMER1 frequency reading
unsigned int freq_PWM_2 = 0;			// FREQ_PWM_2				// stores M2 TIMER2 frequency reading

unsigned int M2_duty_cycle_1 = 638;		// M2_DUTY_CYCLE_1 (PWM_1)
unsigned int M2_duty_cycle_2 = 78;		// M2_DUTY_CYCLE_2 (PWM_2)

boolean system_control_2 = true;		// SYSTEM_CONTROL_2 (MAN/AUTO)		// SYSTEM_CONTROL_2 (0 => MAN / 1 => AUTO)

boolean fan_mode_1 = true;			// FAN_MODE_1 (VCC_1/PWM_1)		// FAN_MODE_1 (0 => VCC_1; 1 => PWM_1)

boolean fan_mode_2 = true;			// FAN_MODE_2 (VCC_2/PWM_2)		// FAN_MODE_2 (0 => VCC_2; 1 => PWM_2)

unsigned int fan_control_1 = 980;		// FAN_CONTROL_1 (RAW_POT_1)		// FAN_CONTROL_1 - raw pot reading for CHANNEL1
unsigned int fan_control_2 = 980;		// FAN_CONTROL_2 (RAW_POT_2)		// FAN_CONTROL_2 - raw pot reading for CHANNEL2

unsigned int real_pot_1 = 0;			// DEBUG_MODE
unsigned int real_pot_2 = 0;			// DEBUG_MODE

unsigned int fan_rpm_5 = 0;			// FAN_RPM_5				// FAN_RPM_5 - stores rpm reading on ATMEGA328P_2

unsigned int temp_AN1 = 0;			// TEMP_AN1				// stores TEMP_AN1 raw data
unsigned int temp_AN2 = 0;			// TEMP_AN2				// stores TEMP_AN2 raw data

unsigned int top_temp_AN = 550;			// TEMP_AN_TOP

byte wire_address [MAX_OW_DEVICES] [8] = {0};           				// sets up array of arrays for storing 8-byte address of each One-Wire device
byte total_OW_devices = 0;    	 							// stores current number of devices discovered on M2 One-Wire bus
boolean temp_OW_multiple = false;
unsigned int temp_OW_1 = 0;			// TEMP_OW_1				// stores temperature reading of OW_Device_1
unsigned int temp_OW_2 = 0;			// TEMP_OW_2				// stores temperature reading of OW_Device_2
unsigned int temp_OW_3 = 0;			// TEMP_OW_3				// stores temperature reading of OW_Device_3

byte I2C_address[MAX_I2C_TEMP_DEVICES] = {0};						// stores indivitual address of each I2C digital temperator sensor
byte total_I2C_devices = 0;            	                				// stores total number of digital temperature sensors discovered on I2C bus
boolean temp_I2C_present = false;
unsigned int temp_I2C_1 = 0;			// TEMP_I2C_1				// stores current temperature reading of I2C_device_1
unsigned int temp_I2C_2 = 0;			// TEMP_I2C_2				// stores current temperature reading of I2C_device_2

unsigned int vcc_IC = 0;			// VCC_IC				// stores M2_VCC_IC (in mV)
unsigned int temp_IC = 0;			// TEMP_IC				// stores M2_TEMP_IC in °C (multiplied by 10)
unsigned int ram_IC = 0;			// RAM_IC				// stores M2_RAM_IC

boolean serial_open = false;								// stores state of serial port

const unsigned long LONG_UPDATE_INTERVAL = 1555556;  					// stores long time interval (in uS) for data updates
const float LONG_UPDATE_DIVISOR = 1.555556;        	       				// stores long update divisor (in Sec) for data updates
unsigned long long_update_tick = 0;							// stores current time reference for long update loop
unsigned long long_update_tock = 0;							// stores previous time reference for long update loop

const unsigned long SHORT_UPDATE_INTERVAL = 333334;  					// stores short time interval (in uS) for data updates
unsigned long short_update_tick = 0;							// stores current time reference for short update loop
unsigned long short_update_tock = 0;							// stores previous time reference for short update loop

const unsigned long PACKAGE_2_INTERVAL = 600000;  						// stores short time interval (in uS) for data updates
unsigned long package_2_tick = 0;								// stores current time reference for short update loop
unsigned long package_2_tock = 0;								// stores previous time reference for short update loop

const unsigned long PACKAGE_3_INTERVAL = 900000;  						// stores short time interval (in uS) for data updates
unsigned long package_3_tick = 0;								// stores current time reference for short update loop
unsigned long package_3_tock = 0;								// stores previous time reference for short update loop


void setup() {

	DDRD &= ~(1 << DDD2); PORTD |= (1 << PORTD2);	// FAN_RPM_5 (PIN_D2)		// FAN_RPM_5 - pinMode(PIN_D2, INPUT_PULLUP)
	DDRD |= (1 << DDD3); 				// M2_TIMER2 (PIN_D3)		// PWM_DRIVE_2 (M2_TIMER2) - pinMode(PIN_D3, OUTPUT)
	DDRD &= ~(1 << DDD4);  								// GND - pinMode(PIN_D4, INPUT)
	DDRD &= ~(1 << DDD5);  								// GND - pinMode(PIN_D5, INPUT)
	DDRD &= ~(1 << DDD6);  								// GND - pinMode(PIN_D6, INPUT)
	DDRD &= ~(1 << DDD7);  								// GND - pinMode(PIN_D7, INPUT)
	DDRB &= ~(1 << DDB0);  								// GND - pinMode(PIN_D8, INPUT)
	DDRB |= (1 << DDB2); 				// M2_TIMER1 (PIN_D10)		// PWM_DRIVE_1 (M2_TIMER1) - pinMode(PIN_D10, OUTPUT)
	DDRB &= ~(1 << DDB3); 				// FAN_MODE_1 (PIN_D11)		// FAN_MODE_1 (VCC_1/PWM_1) - pinMode(PIN_D11, INPUT)
	DDRB &= ~(1 << DDB4); 				// FAN_MODE_2 (PIN_D12)		// FAN_MODE_2 (VCC_2/PWM_2) - pinMode(PIN_D12, INPUT)
	DDRB &= ~(1 << DDB5); 				// SYSTEM_CONTROL_2 (PIN_D13)	// SYSTEM_CONTROL_2 (MAN/AUTO) - pinMode(PIN_D13, INPUT)
	DDRC &= ~(1 << DDC0);				// TEMP_AN1			// TEMP_AN1 - pinMode(PIN_A0, INPUT)
	DDRC &= ~(1 << DDC1);				// FAN_CONTROL_2 (RAW_POT)	// FAN_CONTROL_2 - pinMode(PIN_A1, INPUT)
	DDRC &= ~(1 << DDC2);				// FAN_CONTROL_1 (RAW_POT)	// FAN_CONTROL_1 - pinMode(PIN_A2, INPUT)
	DDRC &= ~(1 << DDC3);				// TEMP_AN2			// TEMP_AN2 - pinMode(PIN_A3, INPUT)

// read_ADC() SETUP //

	ADMUX = 0; 									// resets entire ADMUX register to zero (ADC OFF, auto-trigger disabled)
	ADMUX |= (1 << REFS0);								// configures AVCC as ADC voltage reference
	ADMUX |= ADC_GND;								// sets ADC Mux input channel to GND							
        ADCSRA = 0;									// resets entire ADCSRA register to zero
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);				// sets ADC with prescaler 128 (16MHz / 128 = 125KHz)
//	ADCSRA |= (1 << ADPS2) | (1 << ADPS1);						// sets ADC with prescaler 64 (16MHz / 64 = 250KHz)
//	ADCSRA |= (1 << ADPS2) | (1 << ADPS0);						// sets ADC with prescaler 32 (16MHz / 32 = 500KHz)
	ADCSRA |= (1 << ADEN) | (1 << ADSC);						// turns on ADC & preforms initial conversion

// M2_TIMER1 SETUP //

	cli();										// disables global interrupts
		TCCR1A = 0;								// resets entire Timer1 "Control Register A" to zero
											// (normal operation: OC1A & OC1B disconnected)
		TCCR1B = 0;								// resets entire Timer1 "Control Register B" to zero 
											// (Timer1 is OFF)										
		TIMSK1 |= (1 << TOIE1); 						// enables Timer1 "compare interrupt mask register A"
		TCCR1A |= (1 << COM1B1) | (1 << WGM11) | (1 << WGM10);			// set up Timer1 in FAST PWM mode (together with TCCR1B settings below)
											// (Timer1 prescaler = 1)
		TCCR1B |= (1 << WGM13) | (1 << WGM12);					// clears OC1B on Compare Match mode (OC1A as "TOP" & OC1B as "BOTTOM")
		OCR1A = 639;								// sets Timer1 "TOP" to generate 25KHz frequency (16Mhz / 1 / 640)
											// (Timer1 OVF ISR invoked every 40uS)
		OCR1B = 638;								// sets initial duty cycle on PIN_D10 (OC1B) to maximum value ("TOP"-1)
		TCCR1B |= (1 << CS10);							// turn on Timer1 (with prescaler 1)

// M2_TIMER2 SETUP //

		TCCR2A = 0;								// resets entire Timer2 "Control Register A" to zero
											// (normal operation: OC2A & OC2B disconnected)
		TCCR2B = 0;								// resets entire Timer2 "Control Register B" to zero
											// (Timer2 is OFF)
		TIMSK2 |= (1 << TOIE2);							// enables Timer2 "compare interrupt mask register A"
		TCCR2A |= (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);			// set ups Timer2 in FAST PWM mode (together with TCCR2B settings below)
		TCCR2B |= (1 << WGM22);							// clears OC2B on Compare Match mode (OC2A as "TOP" & OC2B as "BOTTOM")
		OCR2A = 79;								// sets Timer2 "TOP" to generate 25KHz frequency (16Mhz / 8 / 80 = 25KHz)
											// (Timer2 OVF ISR invoked every 40uS)
		OCR2B = 78;								// sets initial duty cycle on PIN_D3 (OC2B) to maximum value ("TOP"-1)
		TCCR2B |= (1 << CS21);							// turns on Timer2 with prescaler 8
	sei();										// enables global interrupts

	PCintPort::attachInterrupt(PIN_D2, burpCount_D2, FALLING);			// FAN_RPM_5 - attaches software interrupt on PIN_D2


	total_OW_devices = discover_OW_Devices();					// searches & stores total number of temperature sensors on One-Wire bus (max: 4 devices)

	if (total_OW_devices > 1) temp_OW_multiple = true;


	total_I2C_devices = discover_I2C_Temp();    					// searchs for new devices on I2C bus

	if (total_I2C_devices > 0) temp_I2C_present = true;

 
  	Wire.begin(M2_ADDRESS);                	        				// joins I2C bus

	TWBR = 12; 									// TWBR 12: 400KHz (max); 32: 200KHz; 72: 100KHz (default)


	if (DEBUG_MODE) {

		Serial.begin(115200);							// opens serial port & begins serial communication

		check_Serial();								// checks if serial port is open
	}

	while (micros() < 150000) {}							// delay to desyncronize M2 to M1 & M1 to PC updates
}


void loop() {

	short_update_tock = micros();							// current time reference for short update function (in uS)

	package_2_tock = short_update_tock;

	package_3_tock = short_update_tock;

	long_update_tock = micros();							// current time reference for long update function (in uS)


	if ((short_update_tock - short_update_tick) >= SHORT_UPDATE_INTERVAL) {		// update function preformed every 0.333334 secondS

		short_update_tick = short_update_tock;					// stores current time reference for next update

		system_control_2 = update_Switch(PIN_D13);	// MAN/AUTO

		fan_mode_1 = update_Switch(PIN_D11);		// VCC_1/PWM_1

		fan_mode_2 = update_Switch(PIN_D12);		// VCC_2/PWM_2

		if (!system_control_2) {

			fan_control_1 = update_Fan_Control(PIN_A2);	// FAN_CONTROL_1

			fan_control_2 = update_Fan_Control(PIN_A1);	// FAN_CONTROL_2
		}

		M1_Get_Duty_Data(M1_ADDRESS);			// GET_DUTY_CYCLE	// gets M2_TIMER1 & M2_TIMER2 duty cycle data from M1

		update_Duty_Cycle(PIN_D10);			// PWM_1 		// DUTY_CYCLE_1 (M2_TIMER1)

		update_Duty_Cycle(PIN_D3);			// PWM_2 		// DUTY_CYCLE_2 (M2_TIMER2)

		M2_Send_Data_M1(M2_PACKAGE_1); 
	}



	if ((package_2_tock - package_2_tick) >= PACKAGE_2_INTERVAL) {				// update function preformed every 1.555556 secondS

		package_2_tick = package_2_tock;						// stores current time reference for next update

		M2_Send_Data_M1(M2_PACKAGE_2); 
 	}

	if ((package_3_tock - package_3_tick) >= PACKAGE_3_INTERVAL) {				// update function preformed every 1.555556 secondS

		package_3_tick = package_3_tock;						// stores current time reference for next update

		M2_Send_Data_M1(M2_PACKAGE_3);  
 	}


	if ((long_update_tock - long_update_tick) >= LONG_UPDATE_INTERVAL) {		// update function preformed every 1.555556 secondS

		long_update_tick = long_update_tock;					// stores current time reference for next update

		freq_PWM_1 = update_Frequency(PIN_D10);		// FREQ_PWM_1		// calculates PWM frequency on PIN_D10

		freq_PWM_2 = update_Frequency(PIN_D3);		// FREQ_PWM_2		// calculates PWM frequency on PIN_D3

		temp_OW_1 = read_OW_Temp(OW_DEVICE_1);		// TEMP_OW_1

		vcc_IC = update_Vcc_IC();			// VCC_IC		// reads VCC_IC

		temp_AN1 = update_Temp_AN(PIN_A0);		// TEMP_AN1

		fan_rpm_5 = update_Fan_RPM(PIN_D2);		// FAN_RPM_5		// updates rpm reading on PIN_D2

		temp_AN2 = update_Temp_AN(PIN_A3);		// TEMP_AN2

		top_temp_AN = update_Top_Temp_AN();		// TEMP_AN_TOP

		if (temp_OW_multiple) update_OW_Temp();		// TEMP_OW		// updates reading of One-Wire temperature sensors if available

		temp_IC = update_Temp_IC();			// TEMP_IC		// reads temp_IC

		if (temp_I2C_present) update_I2C_Temp();	// TEMP_I2C		// updates reading of I2C temeprature sensors if available

		ram_IC = update_Ram_IC();			// RAM_IC		// reads ram_IC

		if ((DEBUG_MODE) && (serial_open)) print_Long_Update();			// preforms debugging serial print function

		if ((DEBUG_MODE) && (!serial_open)) check_Serial();			// checks if serial port in open for communication
	}
}


ISR(TIMER1_OVF_vect) {									// Timer1 ISR (pwm frequency counter)
  	ovf1++;										// counts the number of Timer1 overflows
}


ISR(TIMER2_OVF_vect) {									// Timer2 ISR (pwm frequency counter)
  	ovf2++;										// counts the number of Timer2 overflows
}


void burpCount_D2() {									// ISR for interrupt triggers on PIN_D2
  	currentBurp_D2++;
}


boolean update_Switch (const byte switch_pin) {

	static boolean switch_result;

	switch (switch_pin) {

		case (PIN_D11):					// VCC_1/PWM_1

			switch_result = bitRead(PINB, 3);							// FAN_MODE_1 (0 => VCC_1; 1 => PWM_1) - digitalRead(PIN_D11)
		
			break;

		case (PIN_D12):					// VCC_2/PWM_2

			switch_result = bitRead(PINB, 4);							// FAN_MODE_2 (0 => VCC_2; 1 => PWM_2) - digitalRead(PIN_D12)

			break;

		case (PIN_D13):					// MAN/AUTO

			switch_result = bitRead(PINB, 5);							// SYSTEM_CONTROL_2 (0 => MAN; 1 => AUTO) - digitalRead(PIN_D13)

			break;
	}

	return switch_result;
}


unsigned int update_Fan_Control (const byte control_pin) {

	static unsigned int current_pot;
	static unsigned int previous_pot_1;					// stores previous reading on selected pin (multiplied by 10)
	static unsigned int previous_pot_2;					// stores previous reading on selected pin (multiplied by 10)
	static unsigned int delta_pot;						// stores delta between current_pot_data_1 & previous_pot_data_1 (multiplied by 10)

	switch (control_pin) {							

		case (PIN_A2): 					// FAN_CONTROL_1

			current_pot = read_ADC(ADC_A2);
			if (DEBUG_MODE) real_pot_1 = current_pot;
			if (current_pot < 40) current_pot = 40;
			if (current_pot > 880) current_pot = 880;
			delta_pot = abs(current_pot - previous_pot_1);
			if ((delta_pot >= 25) || (current_pot < 65) || (current_pot > 850) || (previous_pot_1 == 0)) previous_pot_1 = current_pot;
		        else current_pot = previous_pot_1;
			break;

		case (PIN_A1): 					// FAN_cONTROL_2

			current_pot = read_ADC(ADC_A1);
			if (DEBUG_MODE) real_pot_2 = current_pot;
			if (current_pot < 40) current_pot = 40;
			if (current_pot > 880) current_pot = 880;
			delta_pot = abs(current_pot - previous_pot_2);
			if ((delta_pot >= 25) || (current_pot < 65) || (current_pot > 850) || (previous_pot_2 == 0)) previous_pot_2 = current_pot;
		        else current_pot = previous_pot_2;
			break;
	}

	if (current_pot < 40) current_pot = 40;
	if (current_pot > 880) current_pot = 880;

	return current_pot;
}


unsigned int update_Fan_RPM (const byte fan_pin) {

	static unsigned int current_rpm;								// stores current rpm reading on selected pin
	static unsigned int previous_rpm;								// stores previous rpm reading on selected pin
	static unsigned int delta_rpm;									// stores delta rpm between previous & current readings

     	previous_rpm = fan_rpm_5;

	cli();												// disables global interrupts
		current_rpm = currentBurp_D2;								// obtains current rpm count from interrupt routine
		currentBurp_D2 = 0;									// resets rpm count on PIN_D9 to zero
	sei();												// enables global interrupts

	current_rpm = round((current_rpm * 30.0) / LONG_UPDATE_DIVISOR);				// calculates current rpm within the update time interval
	delta_rpm = abs(current_rpm - previous_rpm);							// calculates delta between previous & current rpm readings 
	if (current_rpm < 300) current_rpm = 0;
	if (current_rpm > 4000) current_rpm = previous_rpm;
	if (current_rpm > previous_rpm) current_rpm = round((current_rpm + previous_rpm) / 2.0);					// averages previous & current readings
	if (current_rpm < 300) current_rpm = 0;
	if ((delta_rpm < 15) && (current_rpm > 300)) current_rpm = previous_rpm;
	else previous_rpm = current_rpm;

	return current_rpm;
}


unsigned int update_Frequency (const byte freq_pin) {

	static unsigned int current_freq;			      					// stores total overflow count of selected timer
//	static volatile unsigned int current_freq;		      					// stores total overflow count of selected timer
	static unsigned int previous_freq_1;
	static unsigned int previous_freq_2;
	static unsigned int delta_freq;

	switch (freq_pin) {

		case (PIN_D10):						// FREQ_PWM_1

			cli();
				current_freq = ovf1; 							// stores number of M2_TIMER1 overflow
				ovf1 = 0; 								// resets M2_TIMER1 overflow counter to zero
			sei();

		 	current_freq = round(current_freq / LONG_UPDATE_DIVISOR);
//		 	current_freq = round(float(current_freq) / LONG_UPDATE_DIVISOR);

			delta_freq = abs(current_freq - previous_freq_1);

			if ((delta_freq >= 10) || (current_freq < 24500)) previous_freq_1 = current_freq;
			else current_freq = previous_freq_1;

			break;

		case (PIN_D3):						// FREQ_PWM_2

			cli();
				current_freq = ovf2; 							// stores number of M2_TIMER2 overflow
				ovf2 = 0; 								// resets M2_TIMER2 overflow counter to zero
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


void update_Duty_Cycle (const byte duty_pin) {

	static unsigned int previous_duty_cycle_1;
	static unsigned int previous_duty_cycle_2;

	switch (duty_pin) {

		case (PIN_D10):					// M2_TIMER1 (PWM_1)

			if (M2_duty_cycle_1 != previous_duty_cycle_1) {

				cli();
					OCR1B = M2_duty_cycle_1;
				sei();

				previous_duty_cycle_1 = M2_duty_cycle_1;
			}

			break;


		case (PIN_D3):					// M2_TIMER2 (PWM_2)

			if (M2_duty_cycle_2 != previous_duty_cycle_2) {

				cli();
					OCR2B = M2_duty_cycle_2;
				sei();

				previous_duty_cycle_2 = M2_duty_cycle_2;
			}
			break;
	}
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


int update_Ram_IC() {

	extern int __heap_start, *__brkval; 
	int v; 
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}


unsigned int update_Temp_AN (const byte temp_pin) {							

	static float analog_temp;
	static unsigned int current_an_data;										// stores current reading for return on selected pin (multiplied by 10)
	static unsigned int previous_an_data_1;
	static unsigned int previous_an_data_2;
	static unsigned int delta_an_data;
	
	switch (temp_pin) {												// sets offset coefficient for selected pin

		case (PIN_A0):						// TEMP_AN1 

			current_an_data = read_ADC(ADC_A0);								// calculates temperature on selected pin

   			if (current_an_data != 0) {									// if result is different from zero:
	 			analog_temp = log((1024.0 * (float(TEMP_RESISTOR_A0) * TEMP_OFFSET_A0) / 		// calculates temperature reading in 0°C
					      float(current_an_data)) - (float(TEMP_RESISTOR_A0) * TEMP_OFFSET_A0));	// and multiplies result by 10
				current_an_data = round((1.0 / (0.001129148 + (0.000234125 *  				// returns analog temperature reading (*10) 
 	            				  analog_temp) + (0.0000000876741 * analog_temp * 
		    				  analog_temp * analog_temp)) - 273.15) * 10.0);
   	    		}
 			if (current_an_data <= 1500) {

				delta_an_data = abs(current_an_data - previous_an_data_1);
				if ((delta_an_data > 5) || (current_an_data < 50)) previous_an_data_1 = current_an_data;
				else current_an_data = previous_an_data_1;
			} else {
				current_an_data = 0;
			}
			break;


		case (PIN_A3): 						// TEMP_AN2

			current_an_data = read_ADC(ADC_A3);								// calculates temperature on selected pin

   			if (current_an_data != 0) {									// if result is different from zero:
	 			analog_temp = log((1024.0 * (float(TEMP_RESISTOR_A3) * TEMP_OFFSET_A3) / 		// calculates temperature reading in 0°C
					      float(current_an_data)) - (float(TEMP_RESISTOR_A3) * TEMP_OFFSET_A3));	// and multiplies result by 10
				current_an_data = round((1.0 / (0.001129148 + (0.000234125 *  				// returns analog temperature reading (*10) 
 	            				  analog_temp) + (0.0000000876741 * analog_temp * 
		    				  analog_temp * analog_temp)) - 273.15) * 10.0);
   	    		}
 			if (current_an_data <= 1500) {

				delta_an_data = abs(current_an_data - previous_an_data_2);
				if ((delta_an_data > 5) || (current_an_data < 50)) previous_an_data_2 = current_an_data;
				else current_an_data = previous_an_data_2;
			} else {
				current_an_data = 0;
			}
			break;
	}
	return current_an_data;
}


unsigned int update_Top_Temp_AN() {

	static unsigned int current_top_temp;
	static unsigned int previous_top_temp;
	static unsigned int delta_top_temp;

	if (temp_AN1 >= temp_AN2) current_top_temp = temp_AN1;
	else current_top_temp = temp_AN2;

	delta_top_temp = abs(current_top_temp - previous_top_temp);

	if (delta_top_temp > 10) previous_top_temp = current_top_temp;
	else current_top_temp = previous_top_temp;

	if (current_top_temp < 200) current_top_temp = 200;

	if (current_top_temp > 550) current_top_temp = 550;

	return current_top_temp;	
}


unsigned int read_ADC (const byte adc_channel) {		        				// analogRead() of selected input source

	static unsigned int adc_delay; 									// stoes adc delay length between first & second conversion
        ADMUX = 0;                                                              			// resets entire ADMUX reguster to zero
 	if (adc_channel != ADC_TEMP) ADMUX |= (1 << REFS0) | adc_channel;				// sets AVCC as ADC voltage reference and selected ADC input channel
	else ADMUX |= (1 << REFS1) | (1 << REFS0) | adc_channel;					// sets Internal 1V1 as ADC voltage reference and selected ADC input channel
	switch (adc_channel) {
                case (ADC_TEMP): adc_delay = 6500; break;						// waits 6500uS for ADC_TEMP channel reading 
		case (ADC_VCC): adc_delay = 500; break;			       				// waits 500uS for ADC_VCC channel reading 
		default: adc_delay = 20; break;	    		                			// waits 20uS for ADC_VCC channel reading 
	}
        delayMicroseconds(adc_delay);   
	ADCSRA |= (1 << ADSC);				  	                			// turns on ADC & preforms initial conversion
	while (bit_is_set(ADCSRA, ADSC));  								// waits until ADC conversion is done (ADSC bit is cleared)
	if ((adc_channel == ADC_A1) || (adc_channel == ADC_A2)) {					// preforms second reading for fan_control_1 & fan_control_2 channels
		ADCSRA |= (1 << ADSC);				                			// turns on ADC & preforms initial conversion
		while (bit_is_set(ADCSRA, ADSC));  							// waits until ADC conversion is done (ADSC bit is cleared)
	} 
        return ADCW; 	                                      	    	    				// returns latest ADC reading
}


void check_Serial () {

	if (Serial) serial_open = true;									// checks if serial port is open
	else serial_open = false;
}


byte discover_OW_Devices() {										// One-wire device search and initialization

	static byte wire_device_count;									// stores number of address of each One-Wire discovered device

	while ((wire_device_count < MAX_OW_DEVICES) && 							// searches One-Wire bus for devices & stores the
	      (wireBus.search(wire_address[wire_device_count]))) {  					// indivitual 8-byte address of each discovered device
	    	wire_device_count++;
	}
	wireBus.reset();										// resets One-Wire bus
	if (wire_device_count == 0) {									// second attempt to discover devices if no devices found during first attempt
		delayMicroseconds(250);
		while ((wire_device_count < MAX_OW_DEVICES) && 						// re-searches bus for devices & stores the
		       	(wireBus.search(wire_address[wire_device_count]))) {   				// stores indivitual 8-byte address of each device
			wire_device_count++;
		}
  		wireBus.reset();									// resets One-Wire bus
	}
	if (wire_device_count > 0) {									// initializes temperature sensor wire devices that were discovered
		for (byte k=0; k < wire_device_count; k++) {						// initialization function for each discovered device
			if ((wire_address[k][0] == 0x28) &&			      			// if wire device is Dallas DS18B20 temperature sensor and
			   ((OneWire::crc8(wire_address[k], 7)) == (wire_address[k][7]))) {    		// crc valitation of device address is successful:
             	  		wireBus.reset();							// resets One-Wire bus
             	  		wireBus.select(wire_address[k]);					// selects a specific device
              	 		wireBus.write(0x44);         						// starts temperature reading (conversion) fuction
             	  		wireBus.reset();							// resets One-Wire bus
			} 
		}
	}
	return wire_device_count;	              							// returns total number of One-Wire devices discovered on bus
}


void update_OW_Temp() {

	switch (total_OW_devices) {									// reads available One-Wire temperature sensors on One-Wire bus
		case (2): temp_OW_2 = read_OW_Temp(OW_DEVICE_2); break;					// reads second One-Wire digital temperature sensor
		case (3): temp_OW_2 = read_OW_Temp(OW_DEVICE_2); 					// reads second One-Wire digital temperature sensor
			  temp_OW_3 = read_OW_Temp(OW_DEVICE_3); break;		 			// reads third One-Wire digital temperature sensor
	}
}


unsigned int read_OW_Temp (const byte wire_device_number) {							// One-wire digital temperature sensor reading

	static boolean wire_device_present;								// stores device presence (device indicates presence by sending 1 to bus master)
	static byte wire_raw_data[12];									// stores raw data of temeprature reading from device
	static float current_temp_wire_data;  								// stores new dats reading from device
	static byte wire_validation_flag[MAX_OW_DEVICES];						// stores value of crc validation flag counter for each One-Wire device
	static unsigned int new_temp_wire;								// stores new temperature reading from device
	static unsigned int previous_temp_wire[MAX_OW_DEVICES];						// stores previous temperature reading from device
	static unsigned int delta_temp_wire;

	new_temp_wire = 0;

	wire_device_present = wireBus.reset();                						// checks if device(s) is present on the bus

	if (wire_device_present) {   									// verifies device(s) presence
        	wireBus.select(wire_address[wire_device_number]);					// selects specific device for communication
		wireBus.write(0xBE);         								// requests raw data of latest temperature reading from device
		for (byte i = 0; i < 9; i++) {      		     					// reads 9 bytes of raw data
	      		wire_raw_data[i] = wireBus.read();						// resolution in set by default to 12-bits (750ms conversion time)
          	}
		if ((wire_raw_data[8]) == (OneWire::crc8(wire_raw_data, 8))) {					// crc valiation check of raw data (compares recieved crc data byte with calculated crc)
  			current_temp_wire_data = (((wire_raw_data[1] << 8) | wire_raw_data[0]) / 16.0);		// stores new temperature reading based on raw data calculation
			new_temp_wire = round(current_temp_wire_data * 10.0);					// calculates new temperature reading multiplied by 10
		}
	
		if (new_temp_wire < 1500) {					// LEGITIMATE VALUE

			wire_validation_flag[wire_device_number] = 0;						// resets crc validation flag count on successful validation
			delta_temp_wire = abs(new_temp_wire - previous_temp_wire[wire_device_number]);
			if ((delta_temp_wire > 5) || (new_temp_wire < 50)) previous_temp_wire[wire_device_number] = new_temp_wire;
			else new_temp_wire = previous_temp_wire[wire_device_number];

		} else {							// ILLEGITIMATE VALUE		// if reading value not valid
			wire_validation_flag[wire_device_number]++;						// increments data validation flag count by one
			if ((wire_validation_flag[wire_device_number]) > 1) {    				// if 2 or more crc checks are unsuccessful in a row
				wire_validation_flag[wire_device_number] = 2;					// resets wire validation flag number to 2 to prevent byte overflow
				new_temp_wire = 0;								// sets new temperature reading to 0°C
  			} else {										// if less than 3 crc checks unsuccessful
				new_temp_wire = previous_temp_wire[wire_device_number]; 			// sets previous reading of WIRE_DEVICE
			}
   	 	}
	}
	wireBus.reset();										// resets One-Wire bus
	wireBus.select(wire_address[wire_device_number]);						// re-selects device for communication
	wireBus.write(0x44); 										// starts new temperature reading (conversion) fuction
	wireBus.reset();										// resets One-Wire bus

	return new_temp_wire;
}	


byte discover_I2C_Temp() {									// I2c digital sensor search & log function

	static byte I2C_device_count;								// stores number of address of each I2C discovered device
	for (byte i = 76; i < 80; i++) {							// searches bus for devices & stores the indivitual
		if (I2C_device_count < MAX_I2C_TEMP_DEVICES) { 					// searches I2C bus for devices (MAX: 4 devices)
			Wire.beginTransmission (i);						
    			if (Wire.endTransmission() == 0) {					// checks if I2C device/s respond to I2C Master call
      				I2C_address[I2C_device_count] = i;				// stores indivitual 1-byte address of each discovered device
      				I2C_device_count++;		
      			}
		}
 	}
	return I2C_device_count;
}


void update_I2C_Temp() {

	switch (total_I2C_devices) {								// reads available temperature sensors on I2C bus
		case (1): temp_I2C_1 = read_I2C_Temp(I2C_DEVICE_1); break;
		case (2): temp_I2C_1 = read_I2C_Temp(I2C_DEVICE_1);
			  temp_I2C_2 = read_I2C_Temp(I2C_DEVICE_2); break;
	}
}


unsigned int read_I2C_Temp (const byte I2C_temp_device_number) {   				// I2C digital temperature sensor reading function

	static unsigned int I2C_device_address;							// stores I2C_address of selected device
//	static byte totalBytesRecieved;								// stores total number of bytes recieved from device
	static byte high_byte;           	             	    				// device sends MSB first
	static byte low_byte;               	         	    				// device sends LSB second
	static float current_temp_I2C_data;							// stores current value of device
	static byte I2C_validation_flag[MAX_I2C_TEMP_DEVICES];					// stores value of crc validation flag counter for each I2C digital sensor
	static unsigned int new_I2C_temp;
	static unsigned int previous_I2C_temp[MAX_I2C_TEMP_DEVICES];				// stores previous value of device
	static unsigned int delta_I2C_temp;

	switch(I2C_temp_device_number) {
		case (I2C_DEVICE_1): I2C_device_address = I2C_address[I2C_DEVICE_1]; break;	// sets address of I2C_DEVICE_1
		case (I2C_DEVICE_2): I2C_device_address = I2C_address[I2C_DEVICE_2]; break;	// sets address of I2C_DEVICE_2
	}

	new_I2C_temp = 0;

//	totalBytesRecieved = Wire.requestFrom(I2C_device_address, 2);      			// requests 2 bytes from device (function returns the number of bytes recieved
//	if (totalBytesRecieved == 2) {					// GOT_RAW_DATA		// if exactly 2 bytes were recieved from device

	if ((Wire.requestFrom(I2C_device_address, 2)) == 2) {		// GOT_RAW_DATA		// if exactly 2 bytes were recieved from device

		high_byte = Wire.read();							// reads MSB byte from device
 		low_byte = Wire.read();								// reads LSB byte from device
 		low_byte &= 224;                          					// removes all bits except the 3 left bits
   		low_byte = (low_byte >> 5);                            				// shifts remaining 5 bits for the right value
 		current_temp_I2C_data = (((high_byte * 8) + low_byte) * 0.125); 		// MSB * 8 (first bit is not 1, but 8, second is 16, and so on); 0.125°C/unit
		new_I2C_temp = round(current_temp_I2C_data * 10.0);				// calculates new temperature reading multiplied by 10
 	
		if (new_I2C_temp < 1500) {					// LEGITIMATE VALUE
			I2C_validation_flag[I2C_temp_device_number] = 0;				// resets crc validation flag count on legitimate reading value
 
			delta_I2C_temp = abs(new_I2C_temp - previous_I2C_temp[I2C_temp_device_number]);

			if ((delta_I2C_temp > 5) || (new_I2C_temp < 50)) previous_I2C_temp[I2C_temp_device_number] = new_I2C_temp;
			else new_I2C_temp = previous_I2C_temp[I2C_temp_device_number];

		} else {							// ILLEGITIMATE VALUE	// if reading value is illegitimate

			I2C_validation_flag[I2C_temp_device_number]++;					// increments data validation flag count by one
			if ((I2C_validation_flag[I2C_temp_device_number]) > 1) {			// if 2 or more crc validation checks unsuccessful in a row:
				I2C_validation_flag[I2C_temp_device_number] = 2;			// resets crc validation flag number to 2 to prevent byte  overflow
				new_I2C_temp = 0;							// sets reading to 0°C
			} else {									// if less than 2 crc validation checks unsuccessful in a row:
				new_I2C_temp = previous_I2C_temp[I2C_temp_device_number];
			}
		}

	}

	return new_I2C_temp;
}


void print_Long_Update() {

	if (PRINT_FIRST_ROW) {

		Serial.println(F(""));
		Serial.println(F(""));

		Serial.print(F("freq_PWM_1: "));		
		Serial.print((float(freq_PWM_1)/1000.0), 3);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("freq_PWM_2: "));		
		Serial.print((float(freq_PWM_2)/1000.0), 3);
		Serial.print('\t');
		Serial.print('\t');

		if (!system_control_2) Serial.print(F("MAN"));
		else Serial.print(F("AUTO"));
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("FAN_MODE_1: "));		
		if (!fan_mode_1) Serial.print(F("VCC_1"));
		else Serial.print(F("PWM_1"));
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("REAL_POT_1: "));		
		Serial.print(real_pot_1);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("FAN_CONTROL_1: "));		
		Serial.print(fan_control_1);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("PWM_DRIVE_1 (OCR1B): "));		
		Serial.print(M2_duty_cycle_1);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("FAN_MODE_2: "));		
		if (!fan_mode_2) Serial.print(F("VCC_2"));
		else Serial.print(F("PWM_2"));
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("REAL_POT_2: "));		
		Serial.print(real_pot_2);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("FAN_CONTROL_2: "));		
		Serial.print(fan_control_2);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("PWM_DRIVE_2 (OCR2B): "));		
		Serial.print(M2_duty_cycle_2);
	}


	if (PRINT_SECOND_ROW) {

		Serial.println(F(""));
		Serial.println(F(""));

		Serial.print(F("FAN_RPM_5: "));		
		Serial.print(fan_rpm_5);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("TEMP_AN1: "));		
		Serial.print((float(temp_AN1)/10.0), 1);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("TEMP_AN2: "));		
		Serial.print((float(temp_AN2)/10.0), 1);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("TEMP_OW_1: "));		
		Serial.print((float(temp_OW_1)/10.0), 1);
		Serial.print('\t');
		Serial.print('\t');

		if (total_OW_devices > 1) {
			Serial.print(F("TEMP_OW_2: "));		
			Serial.print((float(temp_OW_2)/10.0), 1);
			Serial.print('\t');
			Serial.print('\t');
		}

		if (total_OW_devices > 2) {
			Serial.print(F("TEMP_OW_3: "));		
			Serial.print((float(temp_OW_3)/10.0), 1);
			Serial.print('\t');
			Serial.print('\t');
		}

		if (total_I2C_devices > 0) {
			Serial.print(F("temp_I2C_1: "));		
			Serial.print((float(temp_I2C_1)/10.0), 1);
			Serial.print('\t');
				Serial.print('\t');
		}

		if (total_I2C_devices > 1) {
			Serial.print(F("temp_I2C_2: "));		
			Serial.print((float(temp_I2C_2)/10.0), 1);
			Serial.print('\t');
			Serial.print('\t');
		}


		Serial.print(F("VCC_IC: "));		
		Serial.print((float(vcc_IC)/1000.0), 3);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("TEMP_IC: "));		
		Serial.print((float(temp_IC)/10.0), 1);
		Serial.print('\t');
		Serial.print('\t');

		Serial.print(F("RAM_IC: "));		
		Serial.print(ram_IC);
	}
}


void M2_Send_Data_M1 (const byte M2_package_identifier) { 

	static byte M2_data_out[M2_DATA_OUT_COUNT];
	static union dataItem {byte b[2]; unsigned int i;} data_item_1, data_item_2, data_item_3, data_item_4, data_item_5, data_item_6, data_item_7,
							   crc_item_1, crc_item_2, crc_item_3, crc_item_4, crc_item_5, crc_item_6, crc_item_7;

	switch (M2_package_identifier) {

		case(M2_PACKAGE_1):

			M2_data_out[0] = M2_PACKAGE_1;
			M2_data_out[1] = M2_PACKAGE_1;

			if (!system_control_2) data_item_1.i = 100;
			else data_item_1.i = 200;
			crc_item_1.i = data_item_1.i;

			if (!fan_mode_1) data_item_2.i = 300;
			else data_item_2.i = 400;
			crc_item_2.i = data_item_2.i;

			if (!fan_mode_2) data_item_3.i = 500;
			else data_item_3.i = 600;
			crc_item_3.i = data_item_3.i;

			data_item_4.i = fan_control_1;
			crc_item_4.i = data_item_4.i;

			data_item_5.i = fan_control_2;
			crc_item_5.i = data_item_5.i;

			data_item_6.i = 0;
			crc_item_6.i = 0;

			data_item_7.i = 0;
			crc_item_7.i = 0;

			break;

		case(M2_PACKAGE_2):

			M2_data_out[0] = M2_PACKAGE_2;
			M2_data_out[1] = M2_PACKAGE_2;

			data_item_1.i = temp_AN1;
			crc_item_1.i = data_item_1.i;

			data_item_2.i = temp_AN2;
			crc_item_2.i = data_item_2.i;

			data_item_3.i = temp_OW_1;
			crc_item_3.i = data_item_3.i;
	
			data_item_4.i = fan_rpm_5;
			crc_item_4.i = data_item_4.i;

			data_item_5.i = freq_PWM_1;
			crc_item_5.i = data_item_5.i;

			data_item_6.i = freq_PWM_2;
			crc_item_6.i = data_item_6.i;

			data_item_7.i = temp_OW_2;
			crc_item_7.i = data_item_7.i;

			break;

		case(M2_PACKAGE_3):

			M2_data_out[0] = M2_PACKAGE_3;
			M2_data_out[1] = M2_PACKAGE_3;

			data_item_1.i = vcc_IC;
			crc_item_1.i = data_item_1.i;

			data_item_2.i = temp_IC;
			crc_item_2.i = data_item_2.i;

			data_item_3.i = ram_IC;
			crc_item_3.i = data_item_3.i;

			data_item_4.i = top_temp_AN;
			crc_item_4.i = data_item_4.i;

			data_item_5.i = temp_OW_3;
			crc_item_5.i = data_item_5.i;

			data_item_6.i = temp_I2C_1;
			crc_item_6.i = data_item_6.i;

			data_item_7.i = temp_I2C_2;
			crc_item_7.i = data_item_7.i;

			break;
	}

	M2_data_out[2] = data_item_1.b[0];
	M2_data_out[3] = data_item_1.b[1];

	M2_data_out[4] = crc_item_1.b[0];
	M2_data_out[5] = crc_item_1.b[1];

	M2_data_out[6] = data_item_2.b[0];
	M2_data_out[7] = data_item_2.b[1];

	M2_data_out[8] = crc_item_2.b[0];
	M2_data_out[9] = crc_item_2.b[1];

	M2_data_out[10] = data_item_3.b[0];
	M2_data_out[11] = data_item_3.b[1];

	M2_data_out[12] = crc_item_3.b[0];
	M2_data_out[13] = crc_item_3.b[1];

	M2_data_out[14] = data_item_4.b[0];
	M2_data_out[15] = data_item_4.b[1];

	M2_data_out[16] = crc_item_4.b[0];
	M2_data_out[17] = crc_item_4.b[1];

	M2_data_out[18] = data_item_5.b[0];
	M2_data_out[19] = data_item_5.b[1];

	M2_data_out[20] = crc_item_5.b[0];
	M2_data_out[21] = crc_item_5.b[1];

	M2_data_out[22] = data_item_6.b[0];
	M2_data_out[23] = data_item_6.b[1];

	M2_data_out[24] = crc_item_6.b[0];
	M2_data_out[25] = crc_item_6.b[1];

	M2_data_out[26] = data_item_7.b[0];
	M2_data_out[27] = data_item_7.b[1];

	M2_data_out[28] = crc_item_7.b[0];
	M2_data_out[29] = crc_item_7.b[1];

	Wire.beginTransmission (M1_ADDRESS);
	Wire.write(M2_data_out, M2_DATA_OUT_COUNT);
	Wire.endTransmission();
}


void M1_Get_Duty_Data (const byte device_address) { 

	static union rawDataDuty {byte b[2]; unsigned int i;} raw_duty_1, raw_duty_2, crc_duty_1, crc_duty_2;
	static byte raw_data[M2_DATA_IN_COUNT];

	if ((Wire.requestFrom(device_address, M2_DATA_IN_COUNT)) == M2_DATA_IN_COUNT) {

		for (byte i=0; i < M2_DATA_IN_COUNT; i++) {

			raw_data[i] = Wire.read();
		}

		cli();
			raw_duty_1.b[0] = raw_data[0];
			raw_duty_1.b[1] = raw_data[1];
			crc_duty_1.b[0] = raw_data[2];
			crc_duty_1.b[1] = raw_data[3];
			raw_duty_2.b[0] = raw_data[4];
			raw_duty_2.b[1] = raw_data[5];
			crc_duty_2.b[0] = raw_data[6];
			crc_duty_2.b[1] = raw_data[7];
		sei();

		if (raw_duty_1.i == crc_duty_1.i) M2_duty_cycle_1 = raw_duty_1.i;
		if (raw_duty_2.i == crc_duty_2.i) M2_duty_cycle_2 = raw_duty_2.i;
	}
}



