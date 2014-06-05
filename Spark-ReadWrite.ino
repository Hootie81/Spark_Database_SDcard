/*
  SD card read/write
 
 This example shows how to read and write data to and from an SD card file  
 The circuit:
 * SD card attached to SPI bus as follows:
  Refer to "libraries/SdFat/Sd2Card_config.h" 
 
 
 created   Nov 2010
 by David A. Mellis
 updated 2 Dec 2010
 by Tom Igoe
 modified for Maple(STM32 micros)/libmaple
 17 Mar 2012
 by dinau   
 
 This example code is in the public domain.
   
 */

#include "application.h"
#include "SD.h"
#include "EDB.h"

File dbFile;
boolean Existing = false;

#define TABLE_SIZE 512
#define RECORDS_TO_CREATE 10

// SOFTWARE SPI pin configuration - modify as required
// The default pins are the same as HARDWARE SPI
const uint8_t chipSelect = A2;    // Also used for HARDWARE SPI setup
const uint8_t mosiPin = A5;
const uint8_t misoPin = A4;
const uint8_t clockPin = A3;

struct LogEvent {
  int id;
  int temperature;
}
logEvent;
 
void writer(unsigned long address, byte data)
{
  dbFile.seek(address);
  dbFile.write(data);
  dbFile.flush();
}
 
byte reader(unsigned long address)
{
  dbFile.seek(address);
  return dbFile.read();
}
 
 
EDB db(&writer, &reader);

void setup()
{
  Serial.begin(115200);
  while (!Serial.available());

  Serial.print("Initializing SD card...");
   
  // Initialize HARDWARE SPI with user defined chipSelect
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }

  Serial.println("initialization done.");
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  if (SD.exists("example.db")) {
    Serial.println("example.db exists.");
	Existing = true;
  }
  else {
    Serial.println("example.db doesn't exist.");
  }
	
	dbFile = SD.open("example.db", FILE_WRITE);
  
  if (dbFile) {   
	int recno;
	
	if (!Existing){
		db.create(0, TABLE_SIZE, sizeof(logEvent));
		
		Serial.print("Record Count: "); Serial.println(db.count());
		Serial.println("Creating Records...");
	
		for (recno = 1; recno <= RECORDS_TO_CREATE; recno++)
		{
			logEvent.id = recno;
			logEvent.temperature = recno * 4;
			db.appendRec(EDB_REC logEvent);
		}
	}
   
	Serial.print("Record Count: "); Serial.println(db.count());
	
	if (Existing) {
		Serial.print("opening DB");
		db.open(0);
	}
		
	Serial.print("Record Count: "); Serial.println(db.count());
	
	for (recno = 1; recno < RECORDS_TO_CREATE; recno++)
	{
		db.readRec(recno, EDB_REC logEvent);
		Serial.print("ID: "); Serial.println(logEvent.id);
		Serial.print("Temp: "); Serial.println(logEvent.temperature);  
	}
  
    dbFile.close();
  } 
  
  else {
    // if the file didn't open, print an error:
    Serial.println("error opening example.db");
  }
}

void loop()
{
  // nothing happens after setup
}