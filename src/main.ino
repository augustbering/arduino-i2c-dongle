/*
 I2C-Master --> USB Converter
 Author: O. Borges
 Version 0.2
 TE Connectivity Sensor Solutions
 Copyright by TE
 */
#include <Wire.h>
#define BUFFER_SIZE 256
#define MyName "ArduinoUnoI2CUSBConverter"
char I2CReceive[BUFFER_SIZE];
int I2CReceiveCounter = 0;
char SerialReceive[BUFFER_SIZE];
int SerialReceiveCounter = 0;
char I2CAdress = 92;
int RecByteCounter = 0;
boolean DoEchos;

void SerialInterpreter() {
	if (SerialReceive[0] == 'r' && SerialReceiveCounter > 1) {
		if (DoEchos == true) {
			Serial.print('r');
			Serial.print(SerialReceive[1]);
			Serial.println("");
		}
		Wire.requestFrom(I2CAdress, SerialReceive[1]); // request X bytes from I2C slave device
		while (Wire.available()) // slave may send less than requested
		{
			char c = Wire.read(); // receive a byte as character
			Serial.print(c); // print the character
		}
		Serial.println("");
	} else if (SerialReceive[0] == 'a' && SerialReceiveCounter > 1) {
		I2CAdress = SerialReceive[1];
		if (DoEchos == true) {
			Serial.print('a');
			Serial.print(I2CAdress);
			Serial.println("");
		}
	} else if (SerialReceive[0] == 'w' && SerialReceiveCounter > 1) {
		Wire.beginTransmission(I2CAdress);
		if (DoEchos == true) {
			Serial.print('w');
		}
		for (int i = 1; i <= SerialReceiveCounter - 2; i++) {
			Wire.write(SerialReceive[i]);
			if (DoEchos == true) {
				Serial.print(SerialReceive[i]);
			}
		}
		Wire.endTransmission();
		if (DoEchos == true) {
			Serial.println("");
		}
	} else if (SerialReceive[0] == 'e') {
		DoEchos = false;
		Serial.println("e");
	} else if (SerialReceive[0] == 'E') {
		DoEchos = true;
		Serial.println("E");
	} else if (SerialReceive[0] == 'W' && SerialReceive[1] == 'h'
			&& SerialReceive[2] == 'o' && SerialReceive[3] == 'A'
			&& SerialReceive[4] == 'r' && SerialReceive[5] == 'e'
			&& SerialReceive[6] == 'Y' && SerialReceive[7] == 'o'
			&& SerialReceive[8] == 'u') {
		Serial.println(MyName);
	}
}

void writeMem(byte i2caddr,byte addr,byte *buf,int len){
	Wire.beginTransmission(i2caddr);
	Wire.write(addr);

	 for (int i=0;i<len;i++)
	      Wire.write(buf[i]);
	 Wire.endTransmission();
}

void setup() {
	Wire.begin();
// Serial.begin(115200); // baud rates > 9600 cause phantom bytes several 0xF0 or 0xFF when PC open its Port.
	Serial.begin(9600);
	while (!Serial) {
	}
	while (Serial.available()) {
		Serial.read();
	}
	Serial.println("hello!");
	Serial.flush();
	/*byte deviceaddress=0x50;
	Wire.beginTransmission(deviceaddress);
	    Wire.write(0);
	    Wire.endTransmission();
	    int length=8;
	    byte buffer[8];
	    Wire.requestFrom((byte)deviceaddress,(byte)length,(byte)true);
	    int c = 0;
	    Serial.println("data:");
	    for ( c = 0; c < length; c++ ){
	      if (Wire.available()) buffer[c] = (byte)Wire.read();
	      Serial.printf("%x,",buffer[c]);
	    }*/
	DoEchos = false;
	Serial.setTimeout(100);
}
void loop() {
	byte buf[100];
	if (Serial.available() > 0) {
		char c = Serial.read();
		byte len=0;
		int res=Serial.readBytes(&len,1);//cmd len
		if (res<1|| Serial.readBytes(buf,len-2)<len-2){
			Serial.println("timout");
			return;
		}

		int pos=0;
		switch (c){
		case 'w':{
			byte i2caddr=buf[pos++];
			int addr=buf[pos++];

			writeMem(i2caddr,addr,buf+pos,len-4);
//			Serial.printf("ia:%x, len:%d, addr:%x, b:%s",i2caddr,len, addr,buf+pos);

			 Serial.print('r');
			 Serial.write(3);
			 Serial.write(0);//error code 0
			 Serial.flush();
		}
		break;
		case 'r':{
			byte i2caddr=buf[pos++];
			int addr=buf[pos++];
			int length=buf[pos++];

			//Serial.printf("i2c:%x, addr:%x, len:%d\n",i2caddr,addr, length);
			Wire.beginTransmission(i2caddr);
				    Wire.write(addr);
				    Wire.endTransmission();
				    int read=Wire.requestFrom((byte)i2caddr,(byte)length,(byte)true);
				    int c = 0;
				    if (read>0){
				    Serial.print("d");
				    Serial.write(read);
				    for ( c = 0; c < read; c++ ){
				    	byte b=0;
				      if (Wire.available())
				    	  b=(byte)Wire.read();
				      Serial.write(b);
				    }
				    }
				    else{
				    	Serial.print("e");
				    	Serial.write(0);
				    }

				    Serial.flush();
				    break;
		}
		default:
			Serial.println("bad cmd");
			for (int i=2;i<len;i++)
				Serial.read();
		}
		/*SerialReceive[SerialReceiveCounter] = c;
		SerialReceiveCounter++;
		if (SerialReceiveCounter > 1
				&& SerialReceive[SerialReceiveCounter - 1] == '\n'){
			SerialInterpreter();
			Serial.write("r\r\n");
			SerialReceiveCounter = 0;
		}
		if (SerialReceiveCounter > BUFFER_SIZE) {
			SerialReceiveCounter = 0; // overflow
		}*/
	}
}
