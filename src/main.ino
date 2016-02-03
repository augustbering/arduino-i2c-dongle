/*
 I2C-Master --> USB Converter for Arduino UNO (and many others)
 By August Bering
 */
#include <Wire.h>
#define BUFFER_SIZE 256
char I2CReceive[BUFFER_SIZE];
int I2CReceiveCounter = 0;
char SerialReceive[BUFFER_SIZE];
int SerialReceiveCounter = 0;
char I2CAdress = 92;
boolean echoOn = true;

void writeCr(const char *str) {
	while (*str != 0) {
		if (*str == '\n')
			Serial.write('\r');
		Serial.write(*str);
		str++;
	}
	Serial.println();
}

void readMem(byte i2caddr, int addr, bool addr2byte, char *params) {
	int length = (int) strtoul(params, &params, 0);

	//Serial.printf("i2c:%d,addr:%d,len: %d\n", i2caddr, addr, length);

	Wire.beginTransmission(i2caddr);
	if (addr2byte) {
		Wire.write(addr >> 8);
		Wire.write(addr);
	} else
		Wire.write(addr);

	Serial.printf("# Reading %d bytes from address 0x%x\n", length, addr);
	Wire.endTransmission();
	int read = Wire.requestFrom((byte) i2caddr, (byte) length, (byte) true);
	int c = 0;
	if (read > 0) {
		Serial.print("OK(hex) ");
		char out[10];
		for (c = 0; c < read; c++) {
			byte b = 0;
			if (Wire.available())
				b = (byte) Wire.read();
			itoa(b, out, 16);
			Serial.write(out);
			Serial.write(",");
		}
	} else {
		Serial.print("ERR no data read");
	}

	Serial.println();
	Serial.flush();
}
void writeMem(byte i2caddr, int addr, bool addr2byte, char *params) {

	int len = 0;
	byte binBuf[100];
	while (true) {
		char *end = 0;
		byte b = (byte) strtoul(params, &end, 0);
		binBuf[len] = b;
		if (end == params) {
			//ok, we're done
			break;
		}
		len++;
		params = end;
	}

	//Serial.printf("i2c:%d,addr:%d,len: %d\n", i2caddr, addr, len);
	//uint8_t err = writeMem(i2caddr, addr, binBuf, len);
	Serial.printf("# Writing %d bytes to address 0x%x:%d\n", len, addr,
			binBuf[0]);
	Wire.beginTransmission(i2caddr);
	if (addr2byte) {
		Wire.write(addr >> 8);
		Wire.write(addr);
	} else
		Wire.write(addr);

	for (int i = 0; i < len; i++)
		Wire.write(binBuf[i]);
	uint8_t err = Wire.endTransmission();

	Serial.flush();
	switch (err) {
	case 0:
		Serial.println("OK");
		break;
	default:
		Serial.printf("ERR(i2c code:%d)\n", err);
	}

}
void SerialInterpreter() {
	char *params = strstr(SerialReceive, " ");
	char *buf = SerialReceive;
	bool addr2byte = false; //we're using 2 byte addressing?
	switch (buf[0]) {
	case 'r':
	case 'w': {
		byte i2caddr = (byte) strtoul(params, &params, 0);
		char *start = params;
		int addr = (int) strtoul(params, &params, 0);
		if (params > start + 3) {
			addr2byte = true;
			Serial.printf("# 2byte addressing\n");
		}
		if (buf[0] == 'r')
			readMem(i2caddr, addr, addr2byte, params);
		else
			writeMem(i2caddr, addr, addr2byte, params);

		break;
	}
	case 'e':
		echoOn = !echoOn;
		Serial.printf("OK(echo %s)\n", echoOn ? "on" : "off");
		break;
	case 'h': {
		char *help =
				"Usage:\n"
						" Reading\n"
						" read <i2c device address> <register address> <length>\n"
						"\n"
						" Writing\n"
						" write <i2c device address> <register address> <data byte 1> <data byte 2> ...\n"
						"\n"
						" Toggle echo (default is \"on\")\n"
						" echo (toggles echo)\n"
						"\n"
						" Display this help\n"
						" help\n"
						"\n"
						" All numbers can be in dec, oct (prefix 0) or hex (prefix 0x) format.\n"
						"\n"
						" i2c device address is in 7 bit format\n"
						"\n"
						" Commands can be shorted to just the first letter.\n"
						"\n"
						" NOTE: If <register address> is more than 2 characters it will be interpreted as a 16 bit address and 16 bit addressing will\n"
						" be used. For example the AT24CS32 eeprom uses this adressing mode.\n"
						"\n"
						" Example writing:\n"
						" > w 0x50 0 10 1 2 5\n"
						" Writes 4 bytes to address 0 of an eeprom memory (0x50 is standard address for 24cxx memories)\n"
						" Output:\n"
						" OK\n"
						"\n"
						" Example reading:\n"
						" > r 0x50 0 10\n"
						" Reads 10 bytes from address 0 of that same eeprom.\n"
						" Output:\n"
						" OK(hex) a,1,2,5,1,0,0,2,0,0,\n";

		writeCr(help);

	}
		break;
	default:
		Serial.println("ERR(bad command)");
	}
}
/**
 Usage:
 Reading
 read <i2c device address> <register address> <length>

 Writing
 write <i2c device address> <register address> <data byte 1> <data byte 2> ...

 Toggle echo (default is "on")
 echo (toggles echo)

 Display this help
 help

 All numbers can be in dec, oct (prefix 0) or hex (prefix 0x) format.

 i2c device address is in 7 bit format

 Commands can be shorted to just the first letter.

 NOTE: If <register address> is more than 2 characters it will be interpreted as a 16 bit address and 16 bit addressing will
 be used. For example the AT24CS32 eeprom uses this adressing mode.

 Example writing:
 > w 0x50 0 10 1 2 5
 Writes 4 bytes to address 0 of an eeprom memory (0x50 is standard address for 24cxx memories)
 Output:
 OK

 Example reading:
 > r 0x50 0 10
 Reads 10 bytes from address 0 of that same eeprom.
 Output:
 OK(hex) a,1,2,5,1,0,0,2,0,0,
 */

void setup() {
	Wire.begin();
// Serial.begin(115200); // baud rates > 9600 cause phantom bytes several 0xF0 or 0xFF when PC open its Port.
	Serial.begin(115200);
	while (!Serial) {
	}
	while (Serial.available()) {
		Serial.read();
	}
	Serial.println("i2ctool for Arduino v1");
	Serial.flush();
	Serial.setTimeout(100);
}

#define BS 8 //vt102 backspace

void loop() {
	if (Serial.available() > 0) {
		char c = Serial.read();

		if (echoOn)
			Serial.write(c);

		if (c == BS) {
			if (SerialReceiveCounter > 0)
				SerialReceiveCounter--;
			return;
		}
		SerialReceive[SerialReceiveCounter] = c;
		SerialReceiveCounter++;

		if (c == '\r') {
			Serial.println();

			if (SerialReceiveCounter > 1) {
				SerialInterpreter();
			}
			SerialReceiveCounter = 0;
		}
		if (SerialReceiveCounter > BUFFER_SIZE) {
			SerialReceiveCounter = 0; // overflow
			Serial.println("ERR(overflow)");
		}
	}
}
