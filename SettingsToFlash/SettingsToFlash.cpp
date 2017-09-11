/*
  SettingsToFlash.cpp - Save settings to ESP8266 flash
  Created by Christian Lechner, August 27, 2017.
  Released into the public domain.
*/
#include "Arduino.h"
#include "SettingsToFlash.h"
#include <EEPROM.h>


#define HEADER "18181818"
#define MAX_SIZE 1024


SettingEntry::SettingEntry(void* pointer, String name, FlashType type) {

	mPointer = pointer;
	mName = name;
	mType = type;
}

SettingEntry::~SettingEntry() {

}




SettingsToFlash::SettingsToFlash() {

	mCount = 0;
	mOffset = 0;
}



SettingsToFlash::~SettingsToFlash() {
	for(int i = 0; i < (int)mSetting.size(); i++)
	{
		if(mSetting[i] != NULL)
		{
			delete(mSetting[i]);
			mSetting[i] = NULL;
		}
	}

}



bool SettingsToFlash::addSetting(int* value, String name) {
	
	for(int i = 0; i < (int)mSetting.size(); i++)
	{
		if(mSetting[i]->getName() == name)
			return false;	
	}

	Serial.println("SettingsToFlash - INT set int to list, name = " + name + ", value = " + String(*value));
	Serial.printf("SettingsToFlash - pointer = %d\n", value);
	SettingEntry* entry = new SettingEntry((void*)value, name, Flash_int);
	mSetting.push_back(entry);
	mCount++;
	return true;
}


bool SettingsToFlash::addSetting(float* value, String name) {

	for(int i = 0; i < (int)mSetting.size(); i++)
	{
		if(mSetting[i]->getName() == name)
			return false;	
	}

	Serial.println("SettingsToFlash - FLOAT set float to list, name = " + name + ", value = " + String(*value));

	SettingEntry* entry = new SettingEntry((void*)value, name, Flash_float);
	mSetting.push_back(entry);
	mCount++;
	return true;
}


bool SettingsToFlash::addSetting(String* value, String name) {

	for(int i = 0; i < (int)mSetting.size(); i++)
	{
		if(mSetting[i]->getName() == name)
			return false;	
	}

	Serial.println("SettingsToFlash - set string to list, name = " + name + ", value = " + *value);

	SettingEntry* entry = new SettingEntry((void*)value, name, Flash_string);
	mSetting.push_back(entry);
	mCount++;
	return true;
}


bool SettingsToFlash::addSetting(bool* value, String name) {

	for(int i = 0; i < (int)mSetting.size(); i++)
	{
		if(mSetting[i]->getName() == name)
			return false;	
	}

	SettingEntry* entry = new SettingEntry((void*)value, name, Flash_bool);
	mSetting.push_back(entry);
	mCount++;
	return true;
}


bool SettingsToFlash::writeHeader() {
	
	String header = HEADER;
	int size = header.length();
	
	Serial.println("SettingsToFlash - Write header");	

	if(mOffset+size+1 >= MAX_SIZE)
		return false;
	
	Serial.println("SettingsToFlash - mOffset = " + String(mOffset) + ", size = " + String(size));

	Serial.printf("SettingsToFlash - ");
	for(int i = 0; i < size; i++)
	{
		Serial.printf("%c", header[i]);
		EEPROM.write(mOffset++, header[i]);
	}
	Serial.println(" - end.");

	EEPROM.write(mOffset++, '|');

	return true;
}


bool SettingsToFlash::writeCount() {
	
	String size_string = String(mCount);
	int size = size_string.length();

	Serial.println("SettingsToFlash - mOffset = " + String(mOffset) + ", size = " + String(size));

	if(mOffset+size+1 >= MAX_SIZE)
		return false;

	Serial.printf("SettingsToFlash - ");
	for(int i = 0; i < size; i++)
	{
		Serial.printf("%c", size_string[i]);
		EEPROM.write(mOffset++, size_string[i]);
	}
	Serial.println(" - end.");

	EEPROM.write(mOffset++, '|');
	
	return true;
}


bool SettingsToFlash::writeString(String value, String name, FlashType flash_type) {
	
	char type = (char)flash_type;
	int name_size = name.length();
	int value_size = value.length();
	
	Serial.println("SettingsToFlash - mOffset = " + String(mOffset) + ", name_size = " + String(name_size) + ", value_size = " + String(value_size));

	Serial.println("SettingsToFlash - value = " + value + ", name = " + name);

	if(mOffset+name_size+value_size+4 >= MAX_SIZE)
		return false;

	EEPROM.write(mOffset++, type);
	EEPROM.write(mOffset++, '|');

	Serial.printf("SettingsToFlash - ");
	for(int i = 0; i < name_size; i++)
	{
		Serial.printf("%c", name[i]);
		EEPROM.write(mOffset++, name[i]);
	}
	Serial.println(" - end.");

	EEPROM.write(mOffset++, '|');
	
	Serial.printf("SettingsToFlash - ");
	for(int i = 0; i < value_size; i++)
	{
		Serial.printf("%c", value[i]);
		EEPROM.write(mOffset++, value[i]);
	}
	Serial.println(" - end.");

	EEPROM.write(mOffset++, '|');

	return true;
}


bool SettingsToFlash::writeConfig() {

	EEPROM.begin(MAX_SIZE);

	bool ret;
	mOffset = 0;
	Serial.println("SettingsToFlash - Start writing settings...");
	ret = writeHeader();
	if(ret == false)
		return false;

	Serial.println("SettingsToFlash - Finished writing header");

	ret = writeCount();
	if(ret == false)
		return false;

	Serial.println("SettingsToFlash - Finished writing count");

	for(int i = 0; i < (int) mSetting.size(); i++)
	{
		String value = "";
		FlashType type = mSetting[i]->getType();

		Serial.println("SettingsToFlash - Write entry " + String(i) + ", name = " + mSetting[i]->getName() + ", type = " + String((int)type));

		switch(type)
		{
			case Flash_int:
			{
				int* ptr = (int*)(mSetting[i]->getPointer());
				int value_int = *ptr;
				Serial.printf("SettingsToFlash - int-value = %d\n", value_int); 
				value = String(*ptr);
				Serial.println("SettingsToFlash - write integer, value = "+ value);
				break;
			}
			case Flash_float:
			{
				float* ptr = (float*)mSetting[i]->getPointer();
				value = String(*ptr);
				Serial.println("SettingsToFlash - write float, value = "+ value);
				break;
			}
			case Flash_string:
			{
				String* ptr = (String*)mSetting[i]->getPointer();
				value = *ptr;
				Serial.println("SettingsToFlash - write string, value = "+ value);
				break;
			}
			case Flash_bool:
			{
				bool* bool_value = (bool*)mSetting[i]->getPointer();
				if(*bool_value == true)
					value = "1";
				else
					value = "0";

				Serial.println("SettingsToFlash - write bool, value = " + value);

				break;
			}
			default:
			{
				Serial.println("SettingsToFlash - wrong type = " + String(type));
				return false;
			}
		}
		ret = writeString(value, mSetting[i]->getName(), type);
	}

	EEPROM.commit();
	EEPROM.end();	

	return true;
}


bool SettingsToFlash::readHeader() {
	
	char read_char = 0;
	String header = "";
	Serial.printf("SettingToFlash - Header: ");
	while(read_char != '|' &&  mOffset-1 < MAX_SIZE)
	{
		
		read_char = EEPROM.read(mOffset++);
		Serial.printf("%c", read_char);
		if(read_char == '|')
			break;
		header+= read_char;
	}
	Serial.println("");
	Serial.println("SettingsToFlash - Stop read header, read_char = " + String(read_char));

	if(header != HEADER  ||  read_char != '|')
		return false;

	return true;
}


int SettingsToFlash::readSize() {
	
	char read_char = 0;
	String size_string = "";
	
	Serial.printf("SettingsToFlash - Size: ");
	while(read_char != '|' &&  mOffset-1 < MAX_SIZE)
	{
		read_char = EEPROM.read(mOffset++);
		Serial.printf("%c", read_char);		
		if(read_char == '|')
			break;
		size_string+= read_char;
	}
	Serial.println("");

	int size = size_string.toInt();	
	Serial.println("SettingsToFlash - stop reading size, size = " + String(size));
	if(size <= 0)
		return -1;

	return size;
}


bool SettingsToFlash::readString() {

	Serial.println("SettingsToFlash - Start read string, mOffset = " + String(mOffset));

	//Read type
	char read_char=0;
	String value_type = "";
	Serial.printf("SettingsToFlash - Type: ");
	while(read_char != '|' &&  mOffset-1 < MAX_SIZE)
	{
		read_char = EEPROM.read(mOffset++);
		Serial.printf("%d", read_char);	
		if(read_char == '|')
			break;
		value_type+= read_char;
	}
	Serial.println("");
	
	//Check type
	if(value_type.length() != 1  ||  read_char != '|')
		return false;

	if(value_type[0] < 1  ||  value_type[0] > 4)
		return false;

	FlashType type = (FlashType)(value_type[0]);

	//Read Name
	read_char = 0;
	String value_name = "";
	Serial.printf("SettingsToFlash - Name: ");
	while(read_char != '|' &&  mOffset-1 < MAX_SIZE)
	{
		read_char = EEPROM.read(mOffset++);
		Serial.printf("%c", read_char);
		if(read_char == '|')
			break;
		value_name+= read_char;
	}
	Serial.println("");

	if(read_char != '|')
		return false;

	//Read value
	read_char = 0;
	String value = "";
	Serial.printf("SettingsToFlash - Value: ");
	while(read_char != '|' &&  mOffset < MAX_SIZE)
	{
		read_char = EEPROM.read(mOffset++);
		Serial.printf("%c", read_char);
		if(read_char == '|')
			break;
		value+= read_char;
	}
	Serial.println("");

	if(read_char != '|')
		return false;

	//Convert value
	
	bool found = false;
	for(int i = 0; i < (int)mSetting.size(); i++)
	{
		if(mSetting[i]->getName() == value_name)
		{
			if(type != mSetting[i]->getType())
				continue;
			
			switch(type)
			{
				case Flash_int:
				{
					Serial.println("SettingsToFlash - int value = " + value);
					*((int*)(mSetting[i]->getPointer())) = value.toInt();
					break;
				}
				case Flash_float:
				{
					Serial.println("SettingsToFlash - float value = " + value);
					*((float*)(mSetting[i]->getPointer())) = value.toFloat();
					break;
				}
				case Flash_string:
				{
					Serial.println("SettingsToFlash - string value = " + value);
					*((String*)(mSetting[i]->getPointer())) = value;
					break;
				}
				case Flash_bool:
				{
					Serial.println("SettingsToFlash - bool value = " + value);
					bool bool_value = false;
					if(value == "1")
						bool_value = true;
					*((bool*)(mSetting[i]->getPointer())) = bool_value;
					break;
				}
				default:
				{
					return false;
				}
			}
			
		}

	}

	return true;

}


bool SettingsToFlash::readConfig() {

	EEPROM.begin(MAX_SIZE);

	Serial.println("SettingsToFlash - Start reading config...");	

	bool ret;
	mOffset = 0;
	
	ret = readHeader();
	if(ret == false)
	{
		Serial.println("SettingsToFlash - Failed to read header");
		return false;
	}


	int size = readSize();
	if(size <= 0)
		return false;

	if(size != mCount)
		return false;
	
	for(int i = 0; i < size; i++)
	{
		ret = readString();
		if(ret == false)
			continue;
	}

	EEPROM.end();
	
	return true;

}

