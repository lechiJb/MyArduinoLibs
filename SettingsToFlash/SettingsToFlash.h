/*
  SettingsToFlash.h - Save settings to ESP8266 flash
  Created by Christian Lechner, August 27, 2017.
  Released into the public domain.
*/
#ifndef SETTINGSTOFLASH_H
#define SETTINGSTOFLASH_H

#include "Arduino.h"
#include "Vector.h"

enum FlashType{
	Flash_int = 1,
	Flash_float,
	Flash_string,
	Flash_bool
};
	

class SettingEntry
{
public:
	SettingEntry(void* pointer, String name, FlashType tye);
	~SettingEntry();

	void* getPointer(){return mPointer;};
	String getName(){return mName;};
	FlashType getType(){return mType;};

private:

	void* mPointer;
	FlashType mType;
	String mName;
};


class SettingsToFlash
{
public:

	SettingsToFlash();
	~SettingsToFlash();

	bool writeConfig();
	bool readConfig();

	bool addSetting(int* value, String name);
	bool addSetting(float* value, String name);
	bool addSetting(String* value, String name);
	bool addSetting(bool* value, String name);

private:

	bool writeHeader();
	bool writeCount();
	bool writeString(String value, String name, FlashType type);

	bool readHeader();
	int readSize();
	bool readString();
	
	Vector<SettingEntry*> mSetting;

	int mCount;
	int mOffset;
	int mMaxLength;

	void* mPointer;


};




#endif
