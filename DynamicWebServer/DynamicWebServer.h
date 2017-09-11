/*
  DynamicWebServer.h - Library create a dynamic HTTP-server, dependig on Input/Output parameters.
  Created by Christian Lechner, August 13, 2017.
  Released into the public domain.
*/
#ifndef DYNAMIC_WEB_SERVER_H
#define DYNAMIC_WEB_SERVER_H

#include "Arduino.h"
#include <Vector.h>
#include <ESP8266WiFi.h>


#ifndef MAX_RESPONSE
	#define MAX_RESPONSE 25
#endif

enum HTML_type{
	HTML_button,
	HTML_checkbox,
	HTML_color,
	HTML_date,
	HTML_email,
	HTML_number,
	HTML_password,
	HTML_radio,
	HTML_range,
	HTML_text,
	HTML_time,
	HTML_url,
	HTML_circle
};


enum Content_type{
	Content_integer = 0,
	Content_float,
	Content_string,
	Content_bool,
	Content_function

};

String convertColor(int r, int g, int b);
bool convertColor(String s, int* r, int* g, int*b);

//#####################################################Dynamic Content#########################################


class DynamicContent
{
public:

	DynamicContent(Content_type type, void* m_pointer, String name, String displayName, String description = "", HTML_type html_type = HTML_text);

	bool setMinValue(int min_value);
	bool setMaxValue(int max_value);
	bool setStep(int step);

	bool setMinValue(float min_value);
	bool setMaxValue(float max_value);
	bool setStep(float step);

	bool setMaxLength(int max_length);
	bool setSize(int size);

	bool setWarning(String warning);
	String getWarning(){return mWarning;};
	bool enableWarning();
	bool disableWarning();
	bool isWarningEnabled(){return mWarningEnabled;};
	bool setColor(String s);
	bool setColor(int r, int g, int b);
	bool setButtonColor(String s);
	bool setButtonColor(int r, int g, int b);

	bool setHTMLType(HTML_type type);
	bool readString(String value);
	String getHTMLString();
	String getHTMLResponse(String category);
	
	String getName(){return mName;};
	String getDisplayName(){return mDisplayName;};
	
private:
	String mName;
	String mDisplayName;
	String mDescription;
	void* mPointer;
	
	String mWarning;
	bool mWarningEnabled;

	int mMinValue_int;
	int mMaxValue_int;
	int mStep_int;
	float mMinValue_float;
	float mMaxValue_float;
	float mStep_float;
	int mSize;
	bool mMinEnabled;
	bool mMaxEnabled;
	bool mStepEnabled;

	String mColor;
	String mButtonColor;

	int mMaxLength;	
	bool mMaxLengthEnabled;

	HTML_type mHTMLType;

	Content_type mType;
};


//#####################################################Dynamic Category#########################################


class DynamicCategory
{
public:

	DynamicCategory(String name, String displayName = "", String description = "");
	~DynamicCategory();
	
	bool addContent(DynamicContent* entry);
	bool addContent(Content_type type, void* m_pointer, String name, String displayName, String description = "", HTML_type html_type = HTML_text);

	DynamicContent* getContent(String name);
	DynamicContent* getContent(int index);

	int size() {return (int)mContentList.size();};
	String getName(){return mName;};
	String getDisplayName(){return mDisplayName;};
	String getDescription(){return mDescription;};

	bool setMinValue(String name, int min_value);
	bool setMaxValue(String name, int max_value);
	bool setStep(String name, int step);

	bool setMinValue(String name, float min_value);
	bool setMaxValue(String name, float max_value);
	bool setStep(String name, float step);

	bool setMaxLength(String name, int max_length);
	bool setSize(String name, int size);

	bool setColor(String name, String s);
	bool setColor(String name, int r, int g, int b);
	bool setButtonColor(String s);
	bool setButtonColor(int r, int g, int b);

	bool enableWarning(String name);
	bool disableWarning(String name);

	bool removeContent(String name);
	
private:

	String mName;
	String mDisplayName;
	String mDescription;
	Vector<DynamicContent*> mContentList;
	Vector<bool> mIsExtern;
	
};

//#####################################################Dynamic WebServer#########################################


class DynamicWebServer
{
public:
	DynamicWebServer(String title, String description, bool Isblocking = false, int port = 80);
	~DynamicWebServer();

	int waitForConnection();
	int sendResponse();

	void setTitle(String title);
	void setDescription(String description);
	bool setFontColor(String color);
	bool setBackgroundColor(String color);
	bool setWarnColor(String color);
	bool setHeaderFontColor(String color);

	bool addCategory(DynamicCategory* category);
	bool addCategory(String name, String displayName, String description = "");
	bool addContent(String category, DynamicContent* entry);
	bool addContent(String category, 
			Content_type type, void* m_pointer, String name, String displayName, String description  ="", HTML_type html_type = HTML_text);

	DynamicCategory* getCategory(String name);
	DynamicContent* getContent(String category, String name);

	bool setMinValue(String category, String name, int min_value);
	bool setMaxValue(String category, String name, int max_value);
	bool setStep(String category, String name, int step);

	bool setMinValue(String category, String name, float min_value);
	bool setMaxValue(String category, String name, float max_value);
	bool setStep(String category, String name, float step);

	bool setMaxLength(String category, String name, int max_length);

	bool enableWarning(String category, String name);
	bool disableWarning(String category, String name);

	bool setSize(String category, String name, int size);

	bool setColor(String category, String name, String s);
	bool setColor(String category, String name, int r, int g, int b);
	bool setButtonColor(String s);
	bool setButtonColor(int r, int g, int b);

	bool removeCategory(String category);
	bool removeContent(String category, String name);

	bool setAutoRefresh(int time);


private:
	String mTitle;
	String mDescription;
	int mPort;
	bool mBlocking;
	WiFiClient mClient;

	Vector<DynamicCategory*> mDynamicCategory;
	Vector<bool> mIsExtern;

	unsigned long mRequestCount;
	WiFiServer* mServer;	
	String mFontColor;
	String mBackgroundColor;
	String mWarnColor;
	String mHeaderFontColor;

	String mPath;
	String mParam;
	String mCmd;
	String mEqu;
	String mCategory;
	String mEqu_noCategory;
	String mEqu_ASCII;

	bool mAutoRefresh;
	int mRefreshTime;

	int addToResponse(String* responseArray, String newResponse, int counter);

};





#endif
