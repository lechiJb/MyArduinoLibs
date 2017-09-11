/*
  DynamicWebServer.cpp - Library create a dynamic HTTP-server, dependig on Input/Output parameters.
  Created by Christian Lechner, August 13, 2017.
  Released into the public domain.
*/
#include "Arduino.h"
#include "DynamicWebServer.h"
#include <math.h>

#define MAX_RESPONSE_LENGTH 1400

//copy string with converting from unicode...
String convertStringFromHTML(String src){

	int len = src.length();
	String dst;
	int d;

	for(int s=0; s < len; s++){
		if(src[s] == '%'){
			if(len-s > 2){
				
				bool invalid = false;

				char c[2];
				c[0] = (char)src[s+1];
				c[1] = (char)src[s+2];

				for(int i = 0; i< 2; i=i+1)
				{
					if(c[i] >= 48  &&  c[i] <= 57){
						c[i] -= 48;	
					}
					else if(c[i] >= 65  &&  c[i] <= 70){
						c[i] -= 55;
					}
					else{
						invalid = true;
					}
				}

				if(invalid == false){
					dst+=(char)(c[0] * 16 + c[1]);
					s+=2;
					continue;
				}
			}
		}
		dst+=src[s];
	}

	return dst;
}


//copy string with converting to unicode...
String convertStringToHTML(String src){

	int len = src.length();
	//char src[len] = source.c_str();
	String dst;
	int d;

	for(int s=0; s < len; s++){
		if(src[s] < 48  ||  (src[s] > 57  &&  src[s] < 64)  ||  (src[s] > 90  &&  src[s] < 97) ||  src[s] > 122){
			
			char u[10];
			sprintf(u, "%%%02X", src[s]);
			dst+=u;
			continue;
		}
		dst+=src[s];
	}

	return dst;
}



String convertColor(int r, int g, int b) {
	if(r>255 || r<0 || g>255 || g<0 || b>255 || b<0)
		return "";

	char cstr_r[3];
	char cstr_g[3];
	char cstr_b[3];
	sprintf(cstr_r, "%02X", r);
	sprintf(cstr_g, "%02X", g);
	sprintf(cstr_b, "%02X", b);

	String color = "#" + String(cstr_r) + String(cstr_g) + String(cstr_b);
	return color;
}


bool convertColor(String s, int* r, int* g, int*b) {

	if(s.length() != 7)
		return false;

	if(s[0] != '#')
		return false;

	int rgb[3];

	for(int i = 0; i< 6; i=i+2)
	{
		char c[2];
		c[0] = (char)s[i+1];
		c[1] = (char)s[i+2];

		for(int ii = 0; ii< 2; ii=ii+1)
		{
			if(c[ii] >= 48  &&  c[ii] <= 57){
				c[ii] -= 48;	
			}
			else if(c[ii] >= 65  &&  c[ii] <= 70){
				c[ii] -= 55;
			}
			else if(c[ii] >= 97  &&  c[ii] <= 102){
				c[ii] -= 87;
			}
			else{
				return false;
			}
		}
		
		rgb[i/2] = c[0]*16 + c[1]; 

	}

	*r = rgb[0];
	*g = rgb[1];
	*b = rgb[2];

	return true;

}



//####################################################Category###################################################
DynamicCategory::DynamicCategory(String name, String displayName, String description) {
	mName = name;
	if(displayName != "")
		mDisplayName = displayName;
	else
		mDisplayName = name;

	mDescription = description;
}


DynamicCategory::~DynamicCategory() {
	for(int i = 0; i < (int)mIsExtern.size(); i++)
	{
		if(mIsExtern[i] == false)
		{
			if(mContentList[i] != NULL)
			{
				delete(mContentList[i]);
				mContentList[i] = NULL;
			}
		}
	}
}


DynamicContent* DynamicCategory::getContent(String name) {
	for(int i = 0; i < (int)mContentList.size(); i++)
	{
		if(mContentList[i] == NULL)
			continue;
		
		if(mContentList[i]->getName() == name)
			return mContentList[i];

	}

	return NULL;
}


DynamicContent* DynamicCategory::getContent(int index) {
	if(index >= (int)mContentList.size())
	{	
		return NULL;
	}

	return mContentList[index];
}


bool DynamicCategory::addContent(DynamicContent* entry) {
	for(int i = 0; i < (int)mContentList.size(); i++) 
	{
		if(mContentList[i] == NULL)
			continue;

		if(mContentList[i]->getName() == entry->getName())
			return false;
	}	

	mIsExtern.push_back(true);
	mContentList.push_back(entry);
	return true;
}


bool DynamicCategory::addContent(Content_type type, void* pointer, String name, String displayName, String description, HTML_type html_type) {
	for(int i = 0; i < (int)mContentList.size(); i++) 
	{
		if(mContentList[i] == NULL)
			continue;

		if(mContentList[i]->getName() == name)
			return false;
	}	

	DynamicContent* entry = new DynamicContent(type, pointer, name, displayName, description, html_type);

	mContentList.push_back(entry);
	mIsExtern.push_back(false);
	return true;
}



bool DynamicCategory::setMinValue(String name, int min_value) {

	DynamicContent* content = getContent(name);
	if(content == NULL)
		return false;

	return content->setMinValue(min_value);
}


bool DynamicCategory::setMaxValue(String name, int max_value) {

	DynamicContent* content = getContent(name);
	if(content == NULL)
		return false;

	return content->setMaxValue(max_value);
}


bool DynamicCategory::setMinValue(String name, float min_value) {

	DynamicContent* content = getContent(name);
	if(content == NULL)
		return false;

	return content->setMinValue(min_value);
}


bool DynamicCategory::setMaxValue(String name, float max_value) {

	DynamicContent* content = getContent(name);
	if(content == NULL)
		return false;

	return content->setMaxValue(max_value);
}


bool DynamicCategory::setMaxLength(String name, int max_length) {

	DynamicContent* content = getContent(name);
	if(content == NULL)
		return false;

	return content->setMaxLength(max_length);
}


bool DynamicCategory::enableWarning(String name) {

	DynamicContent* content = getContent(name);
	if(content == NULL)
		return false;

	return content->enableWarning();
}


bool DynamicCategory::disableWarning(String name) {

	DynamicContent* content = getContent(name);
	if(content == NULL)
		return false;

	return content->disableWarning();
}

bool DynamicCategory::setSize(String name, int size) {
	
	DynamicContent* content = getContent(name);
	if(content == NULL)
		return false;

	return content->setSize(size);
}


bool DynamicCategory::setColor(String name, String s) {

	DynamicContent* content = getContent(name);
	if(content == NULL)
		return false;

	return content->setColor(s);
}


bool DynamicCategory::setColor(String name, int r, int g, int b) {

	DynamicContent* content = getContent(name);
	if(content == NULL)
		return false;

	return content->setColor(r, g, b);
}


bool DynamicCategory::setButtonColor(String s) {

	for(int i = 0; i < (int)mContentList.size(); i++)
	{
		if(mContentList[i] != NULL)
			mContentList[i]->setButtonColor(s);
	}

	return true;
}


bool DynamicCategory::setButtonColor(int r, int g, int b) {

	for(int i = 0; i < (int)mContentList.size(); i++)
	{
		if(mContentList[i] != NULL)
			mContentList[i]->setButtonColor(r, g, b);
	}

	return true;
}


bool DynamicCategory::setStep(String name, int step) {

	DynamicContent* content = getContent(name);
	if(content == NULL)
		return false;

	return content->setStep(step);
}


bool DynamicCategory::setStep(String name, float step) {

	DynamicContent* content = getContent(name);
	if(content == NULL)
		return false;

	return content->setStep(step);
}


bool DynamicCategory::removeContent(String name) {

	for(int i = 0; i < (int)mContentList.size(); i++)
	{
		if(mContentList[i] == NULL)
			continue;

		if(mIsExtern[i] == false)
		{
			delete(mContentList[i]);
			mContentList[i] = NULL;
			return true;
		}
	}
}


//####################################################Content###################################################

DynamicContent::DynamicContent(Content_type type, void* pointer, String name, String displayName, String description, HTML_type html_type) {

	mName = name;
	mDisplayName = displayName;
	mDescription = description;
	mPointer = pointer;
	
	mWarning = "";
	mWarningEnabled = true;

	mMinValue_int = 0;
	mMaxValue_int = 0;
	mStep_int = 0;
	mMinValue_float = 0.0;
	mMaxValue_float = 0.0;
	mStep_float = 0.0;
	mSize = 2;
	mMinEnabled = false;
	mMaxEnabled = false;
	mStepEnabled = false;

	mColor = "#39B4CC"; 
	mButtonColor = "#101010";

	mMaxLength = 0;	
	mMaxLengthEnabled = false;

	mHTMLType = html_type;
	mType = type;
}


bool DynamicContent::setMinValue(int min_value) {
	mMinValue_int = min_value;
	mMinEnabled = true;
	return true;
}


bool DynamicContent::setMaxValue(int max_value) {
	mMaxValue_int = max_value;
	mMaxEnabled = true;
	return true;
}


bool DynamicContent::setStep(int step) {
	mStep_int = step;
	mStepEnabled = true;
	return true;
}


bool DynamicContent::setMinValue(float min_value) {
	mMinValue_float = min_value;
	mMinEnabled = true;
	return true;
}


bool DynamicContent::setMaxValue(float max_value) {
	mMaxValue_float = max_value;
	mMaxEnabled = true;
	return true;
}


bool DynamicContent::setStep(float step) {
	mStep_float = step;
	mStepEnabled = true;
	return true;
}


bool DynamicContent::setMaxLength(int maxLength) {
	mMaxLength = maxLength;
	mMaxLengthEnabled = true;
	return true;
}

bool DynamicContent::setSize(int size) {
	mSize = size;
	return true;
}


bool DynamicContent::setWarning(String warning) {
	mWarning = warning;
	return true;
}


bool DynamicContent::enableWarning() {
	mWarningEnabled = true;
	return true;
}


bool DynamicContent::disableWarning() {
	mWarningEnabled = false;
	return true;
}

bool DynamicContent::setColor(String s) {
	int r, g, b;
	if(!convertColor(s, &r, &g, &b))
		return false;

	mColor = s;
	return true;
}

bool DynamicContent::setColor(int r, int g, int b) {

	String s = convertColor(r, g, b);
	if(s == "")
		return false,

	mColor = s;
	return true;
}


bool DynamicContent::setButtonColor(String s) {

	int r, g, b;
	if(!convertColor(s, &r, &g, &b))
		return false;

	mButtonColor = s;
	return true;
}


bool DynamicContent::setButtonColor(int r, int g, int b) {

	String s = convertColor(r, g, b);
	if(s == "")
		return false,

	mButtonColor = s;
	return true;
}


bool DynamicContent::setHTMLType(HTML_type type) {
	mHTMLType = type;
	return true;
}





bool DynamicContent::readString(String value) {
	
	switch(mType)
	{
		//Integer
		case Content_integer:
		{
			int v_int = value.toInt();
			if(mMinEnabled == true  && v_int < mMinValue_int)
			{
				mWarning = "Integer value out of rage, min-value = " + String(mMinValue_int);
				return false;
			}

			if(mMaxEnabled == true  && v_int > mMaxValue_int)
			{
				mWarning = "Integer value out of rage, max-value = " + String(mMaxValue_int);
				return false;
			}

			*((int*)mPointer) = v_int;
			return true;
			break;
		}
		
		//Float
		case Content_float:
		{
			float v_float = value.toFloat();
			if(mMinEnabled == true  && v_float < mMinValue_float)
			{
				mWarning = "Integer value out of rage, min-value = " + String(mMinValue_float);
				return false;
			}

			if(mMaxEnabled == true  && v_float > mMaxValue_float)
			{
				mWarning = "Integer value out of rage, max-value = " + String(mMaxValue_float);
				return false;
			}

			*((float*)mPointer) = v_float;
			return true;
			break;
		}

		//String
		case Content_string:
		{	
			String v_string = value;
			if(mMaxLengthEnabled == true  && v_string.length() < mMaxLength)
			{
				mWarning = "String is too long, max-length = " + String(mMaxLength);
				return false;
			}

			*((String*)mPointer) = v_string;
			return true;
			break;
		}
		
		//Bool
		case Content_bool:
		{
			bool v_bool;
			if(value == "0"  || value == "true")
				v_bool = false;
			else
				v_bool = true;

			*((bool*)mPointer) = v_bool;
			return true;
			break;
		}

		//Function
		case Content_function:
		{
			void (*function)() = (void (*)())mPointer;
			(*function)();
			
			return true;
			break;
		}

		//Else...
		default:
		{		
			mWarning = "Undefined content";
			return false;
			break;
		}
	
	}
	
	return false;

}


String DynamicContent::getHTMLString() {

	String value;

	switch(mType)
	{
		//Integer
		case Content_integer:
		{
			value = String(*((int*)mPointer));
			break;
		}		

		//Float
		case Content_float:
		{		
			value = String(*((float*)mPointer));
			break;
		}

		//String
		case Content_string:
		{		
			value = *((String*)mPointer);
			break;
		}

		//Bool
		case Content_bool:
		{		
			if(*((bool*)mPointer) == true)
				value = "true";
			else
				value = "false";
			break;
		}

		//Function
		case Content_function:
		{
			value = "";
			break;		
		}

		//Else...
		default:
		{		
			mWarning = "No valid type";
			value = "";
			break;
		}
	}
	
	return value;
}


String DynamicContent::getHTMLResponse(String category) {
	
	String value = getHTMLString();	
	String code = "";

	switch(mHTMLType)
	{
		//Button
		case HTML_button:
		{
			if(mType != Content_function)
			{	
				mWarning = "Wrong content for HTML_submit (only functions allowed)";
				return "";
			}			

			code = 	"<form></b>" + mDescription + 
				" &nbsp</b><input type=\"submit\" name=\"" + mName + "!" + category +
				"\" value=\"" + mDisplayName + "\"></form>";
	
			break;
		}

		//Checkbox
		case HTML_checkbox:
		{		
			if(mType != Content_bool)
			{	
				mWarning = "Wrong content for HTML_checkbox (only bool allowed)";
				return "";
			}

			String checked;			
			if(value == "true")
				checked = "checked><input type=\"hidden\" value=\"0\" name=\"" + mName + "!" + category + "\"";
			else
				checked = "";

			code = 	"<form><b>" + mDisplayName + 
				" &nbsp</b><input type=\"checkbox\" name=\"" + mName + "!" + category +
				"\" value=\"" + value + 
				"\" " + checked + ">&nbsp" + mDescription + 
				"<br><input type=\"submit\" value=\"OK\"></form>";

			break;
		}

		//Color
		case HTML_color:			
		{		
			if(mType != Content_string)
			{	
				mWarning = "Wrong content for HTML_color (only bool string)";
				return "";
			}

			code = 	"<form><b>" + mDisplayName + 
				" &nbsp</b><input type=\"color\" name=\"" + mName + "!" + category +
				"\" value=\"" + value + 
				"\">&nbsp " + mDescription + 
				"<br><input type=\"submit\" value=\"OK\"></form>";

			break;
		}

		//Date
		case HTML_date:
		{
			if(mType != Content_string)
			{	
				mWarning = "Wrong content for HTML_date (only bool string)";
				return "";
			}

			code = 	"<form><b>" + mDisplayName + 
				" &nbsp</b><input type=\"date\" name=\"" + mName + "!" + category +
				"\" value=\"" + value + 
				"\">&nbsp " + mDescription + 
				"<br><input type=\"submit\" value=\"OK\"></form>";

			break;
		}

		//Email
		case HTML_email:
		{
			if(mType != Content_string)
			{	
				mWarning = "Wrong content for HTML_email (only bool string)";
				return "";
			}

			code = 	"<form><b>" + mDisplayName + 
				" &nbsp</b><input type=\"email\" name=\"" + mName + "!" + category +
				"\" value=\"" + value + 
				"\">&nbsp " + mDescription + 
				"<br><input type=\"submit\" value=\"OK\"></form>";

			break;
		}

		//Number
		case HTML_number:
		{
			if(mType != Content_integer)
			{	
				mWarning = "Wrong content for HTML_number (only integer string)";
				return "";
			}

			code = 	"<form><b>" + mDisplayName + 
				" &nbsp</b><input type=\"number\" name=\"" + mName + "!" + category +
				"\" value=\"" + value + 
				"\">&nbsp " + mDescription + 
				"<br><input type=\"submit\" value=\"OK\"></form>";

			break;
		}

		//Password
		case HTML_password:
		{
			code = 	"<form><b>" + mDisplayName + 
				" &nbsp</b><input type=\"password\" name=\"" + mName + "!" + category +
				"\" value=\"" + value + 
				"\">&nbsp " + mDescription + 
				"<br><input type=\"submit\" value=\"OK\"></form>";

			break;
		}

		//Radio
		case HTML_radio:
		{
			if(mType != Content_bool)
			{	
				mWarning = "Wrong content for HTML_radio (only bool allowed)";
				return "";
			}

			String checked;			
			if(value == "true")
				checked = "checked><input type=\"hidden\" value=\"0\" name=\"" + mName + "!" + category + "\">";
			else
				checked = ">";

			code = 	"<form><b>" + mDisplayName + 
				" &nbsp</b><input type=\"radio\" name=\"" + mName + "!" + category +
				"\" value=\"" + value + 
				"\" " + checked + ">&nbsp " + mDescription + 
				"<br><input type=\"submit\" value=\"OK\"></form>";

			break;
		}

		//Range
		case HTML_range:
		{
			if(mType != Content_float &&  mType != Content_integer)
			{

				mWarning = "Wrong content for HTML_range (only float or integer)";
				return "";
			}
			else
			{

				if(mStepEnabled == false  ||  mMinEnabled == false || mMaxEnabled == false)
				{
					mWarning = "Min / Max / Step not enabled";
					return "";
				}
				else
				{

					String min_val = "";
					String max_val = "";
					String step = "";
					if(mType == Content_float)
					{
						min_val = String(mMinValue_float);
						max_val = String(mMaxValue_float);
						step = String(mStep_float);
					}
					else if(mType == Content_integer)
					{
						min_val = String(mMinValue_int);
						max_val = String(mMaxValue_int);
						step = String(mStep_int);
					}

					code = "<form><b>" + mDisplayName + " &nbsp</b><input style=\"width: 50%;\" type=\"range\" id=\"" + mName + "_" + category +"_inputID\" name=\"" + mName + "!" + category + "\" min=\"" + min_val + "\" max=\"" + max_val + "\" step=\"" + step + "\" value=\"" + value + "\" oninput=\"" + mName + "_" + category + "_outputID.value = " + mName + "_" + category + "_inputID.value\">&nbsp <output name=\"" + mName + "_" + category + "_outputName\" id=\""+ mName + "_" + category + "_outputID\">" + value + "</output><br>" + mDescription + "&nbsp&nbsp<input type=\"submit\" value=\"OK\"></form>";

				}

			}

			break;
		}


		//Text
		case HTML_text:
		{
			code = 	"<form><b>" + mDisplayName + 
				" &nbsp</b><input type=\"text\" name=\"" + mName + "!" + category +
				"\" value=\"" + value + 
				"\">&nbsp " + mDescription + 
				"<br><input type=\"submit\" value=\"OK\"></form>";

			break;
		}

		//Time
		case HTML_time:
		{
			if(mType != Content_string)
			{	
				mWarning = "Wrong content for HTML_time (only bool string)";
				return "";
			}

			code = 	"<form><b>" + mDisplayName + 
				" &nbsp</b><input type=\"time\" name=\"" + mName + "!" + category +
				"\" value=\"" + value + 
				"\">&nbsp " + mDescription + 
				"<br><input type=\"submit\" value=\"OK\"></form>";

			break;
		}

		//URL
		case HTML_url:
		{
			if(mType != Content_string)
			{	
				mWarning = "Wrong content for HTML_url (only bool string)";
				return "";
			}

			code = 	"<form><b>" + mDisplayName + 
				" &nbsp</b><input type=\"url\" name=\"" + mName + "!" + category +
				"\" value=\"" + value + 
				"\">&nbsp " + mDescription + 
				"<br><input type=\"submit\" value=\"OK\"></form>";

			break;
		}

		//Circle
		case HTML_circle:
		{
			if(mType != Content_float &&  mType != Content_integer)
			{

				mWarning = "Wrong content for HTML_circle (only float or integer)";
				return "";
			}
			else
			{
				
				if(mMinEnabled == false || mMaxEnabled == false)
				{
					mWarning = "Min / Max not enabled";
					return "";
				}
				else
				{

					int color_diff_bg = 200;
					int color_diff_1 = -30;
					int color_diff_2 = 50;
					int rgb[3];

					if(!convertColor(mColor, &(rgb[0]), &(rgb[1]), &(rgb[2])))
					{
						mWarning = "Color-convertion failed";
						return "";
					}

					int rgb_bg[3], rgb_c1[3], rgb_c2[3];
					for(int i = 0; i < 3; i++)
					{
						rgb_bg[i]=rgb[i] + color_diff_bg;
						rgb_c1[i]=rgb[i] + color_diff_1;
						rgb_c2[i]=rgb[i] + color_diff_2;
						if(rgb_bg[i] < 0)
							rgb_bg[i] = 0;
						if(rgb_bg[i] > 255)
							rgb_bg[i] = 255;

						if(rgb_c1[i] < 0)
							rgb_c1[i] = 0;
						if(rgb_c1[i] > 255)
							rgb_c1[i] = 255;

						if(rgb_c2[i] < 0)
							rgb_c2[i] = 0;
						if(rgb_c2[i] > 255)
							rgb_c2[i] = 255;
					} 

					String background_color = convertColor( rgb_bg[0], rgb_bg[1], rgb_bg[2]);
					String color1 = convertColor( rgb_c1[0], rgb_c1[1], rgb_c1[2]);
					String color2 = convertColor( rgb_c2[0], rgb_c2[1], rgb_c2[2]);

					String color3 = color2;
					int size = 100*mSize;
					int size_border = size + 20*mSize;
					int offset = 10*mSize;
					int font_size = 15*mSize;
					int font_pos = 45*mSize;
					String new_value_circle ="";

					if(mType == Content_float)
					{
						float val = *((float*)mPointer);
						val = val/((mMaxValue_float - mMinValue_float)/360.0);
						if(val < 0.0)
							val = 0.0;
						else if(val > 360.0)
							val = 360.0;

						if(val <= 180.0)
						{
							val += 90.0;
						}
						else
						{
							val -= 90.0;
							color2 = color1;
						}

						new_value_circle = String(val);
					}
					else if(mType == Content_integer)
					{
						int val = *((int*)mPointer);
						val = val/((mMaxValue_int - mMinValue_int)/360);
						if(val < 0)
							val = 0;
						else if(val > 360)
							val = 360;

						if(val <= 180)
						{
							val += 90;
						}
						else
						{
							val -= 90;
							color2 = color1;
						}

						new_value_circle = String(val);
					}
					else
					{
						new_value_circle = "90";
					}

					code = 	"<form><center><style>.prec{top: " + String(font_pos) + "px;position: relative;font-size: " + String(font_size) + "px;}.circle{position: relative;top: " + String(offset) + "px;left: " + String(offset) + "px;text-align: center;width: " + String(size) + "px;height: " + String(size) + "px;border-radius: 100%;background-color: " + background_color + ";}.active-border{position: relative;text-align: center;width: " + String(size_border) + "px;height: " + String(size_border) + "px;border-radius: 100%;background-color:" + color1 + ";background-image:linear-gradient(" + new_value_circle + "deg, transparent 50%, " + color2 + " 50%),linear-gradient(90deg, " + color3 + " 50%, transparent 50%);}</style><div id=\"activeBorder\" class=\"active-border\"><div id=\"circle\" class=\"circle\">""<span class=\"prec 180\" id=\"prec\"><b>" + value + "</b></span></div></div></center>" + mDescription + "</form>";

				}

			}
			break;
		}

		default:
			break;

	}

	return code;
}


//####################################################Server###################################################

DynamicWebServer::DynamicWebServer(String title, String description, bool isBlocking, int port) {

	//Init Variable
	setTitle(title);
	setDescription(description);
	mPort = port;
	//mCategoryID = -1;
	mRequestCount = 0;
	mFontColor = "#010101";
	mBackgroundColor = "#f0f0f0";
	mWarnColor = "#f00000";
	mHeaderFontColor = "#606060";
	mBlocking = isBlocking;


	mPath = "";
	mParam = "";
	mCmd = "";
	mEqu = "";
	mCategory = "";
	mEqu_noCategory = "";
	mEqu_ASCII = "";

	mAutoRefresh = false;
	mRefreshTime = 3;

	mServer = new WiFiServer(mPort);
	mServer->begin();

}

DynamicWebServer::~DynamicWebServer() {
	if(mServer != NULL)	
		delete mServer;

	for(int i = 0; i < (int)mDynamicCategory.size(); i++)
	{
		if(mDynamicCategory[i] == NULL)
			continue;

		if(mIsExtern[i] == false)
		{
			delete(mDynamicCategory[i]);
			mDynamicCategory[i] = NULL;
		}
	}
}


bool DynamicWebServer::setFontColor(String color) {
	mFontColor=color;
}


bool DynamicWebServer::setBackgroundColor(String color) {
	mBackgroundColor=color;
}


bool DynamicWebServer::setWarnColor(String color) {
	mWarnColor=color;
}


bool DynamicWebServer::setHeaderFontColor(String color) {
	mHeaderFontColor=color;
}


int DynamicWebServer::waitForConnection() {
	
// Check if a client has connected
	while(!mClient) {	
		mClient = mServer->available();
		if(mBlocking == false)
			break;

		delay(50);
	}


// Wait until the client sends some data
	unsigned long ultimeout = millis()+250;
	while(!mClient.available() && (millis()<ultimeout) )
	{
		delay(1);
	}
	if(millis()>ultimeout) 
	{ 
		return -2; 
	}

// Read the first line of the request
	String sRequest = mClient.readStringUntil('\r');
	mClient.flush();

// stop client, if request is empty
	if(sRequest=="")
	{
		mClient.stop();
		return -3;
	}

// get positions for parameters
	mPath="";
	mParam="";
	mCmd="";
	mEqu="";
	mCategory="";
	mEqu_noCategory="";
	mEqu_ASCII="";

	String sGetstart="GET ";
	int iStart,iEndSpace,iEndQuest;
	iStart = sRequest.indexOf(sGetstart);
	if (iStart>=0)
	{
		iStart+=+sGetstart.length();
		iEndSpace = sRequest.indexOf(" ",iStart);
		iEndQuest = sRequest.indexOf("?",iStart);
    
		// are there parameters?
		if(iEndSpace>0)
		{
			if(iEndQuest>0)
			{
				// there are parameters
				mPath  = sRequest.substring(iStart,iEndQuest);
				mParam = sRequest.substring(iEndQuest,iEndSpace);
			}
			else
			{
				// NO parameters
				mPath  = sRequest.substring(iStart,iEndSpace);
			}
		}
	}
  

// devide parameters into equation and command.
	if(mParam.length()>0)
	{
		int iEqu=mParam.indexOf("=");
		if(iEqu>=0)
		{
			mEqu = mParam.substring(1, iEqu);
			mCmd = mParam.substring(iEqu+1,mParam.length());
			
		}
	}


	if(mPath=="/")
	{

//Valid request
		if (mEqu.length()>0)
		{
			//Split in content and category
			mEqu_ASCII = convertStringFromHTML(mEqu);
			int iEndEqu = mEqu_ASCII.indexOf("!");
			int iStartCategory = iEndEqu+1;
			mEqu_noCategory = mEqu_ASCII.substring(0, iEndEqu);
			mCategory = mEqu_ASCII.substring(iStartCategory, mEqu_ASCII.length());

			//Search all categories:
			bool ready = false;
			for(int c = 0; c < (int)mDynamicCategory.size(); c++) {
				
				if(mDynamicCategory[c] == NULL)
					continue;

				if(mCategory != mDynamicCategory[c]->getName())
					continue;				

				DynamicContent* content = NULL;				
				if(mDynamicCategory[c] != NULL)
					content = mDynamicCategory[c]->getContent(mEqu_noCategory);

				if(content != NULL) {
					content->readString(convertStringFromHTML(mCmd));
				}
			}
		}
	}
	
	return 0;

}


int DynamicWebServer::addToResponse(String* responseArray, String newResponse, int counter) {
	if(responseArray[counter].length() + newResponse.length() > MAX_RESPONSE_LENGTH)
	{
		counter++;
		if(counter > MAX_RESPONSE-1)
		{
			return counter-1;
		}

		responseArray[counter] += newResponse;
	}
	else
	{
		responseArray[counter] += newResponse;
	}

	return counter;

}


int DynamicWebServer::sendResponse() {

	if(!mClient)
		return -1;

	String sResponse[MAX_RESPONSE];
	for(int i = 0; i < MAX_RESPONSE; i++)
	{
		sResponse[i] = "";
	}
	int rCounter = 0;
	String sHeader = "";

	if(mPath != "/")
	{
//Send a 404-error
		rCounter = addToResponse(sResponse, "<html><head><title>404 Not Found</title></head><body><h1>", rCounter);
		rCounter = addToResponse(sResponse, "Not Found</h1><p>The requested URL was not found on this server.</p></body></html>", rCounter);
		sHeader  = "HTTP/1.1 404 Not found\r\n";
		sHeader += "Content-Length: ";
		int response_length = 0;
		for(int i = 0; i < MAX_RESPONSE; i++)
		{
			response_length += sResponse[i].length();
		}		
		sHeader += String(response_length);
		sHeader += "\r\n";
		sHeader += "Content-Type: text/html\r\n";
		sHeader += "Connection: close\r\n";
		sHeader += "\r\n";

	}
	else
	{

//send new homepage

		//HTML Start
		mRequestCount++;
		rCounter = addToResponse(sResponse, "<html><head><title>" + mTitle + "</title>", rCounter);	
		rCounter = addToResponse(sResponse, "<meta charset=\"utf-8\">", rCounter);
		rCounter = addToResponse(sResponse, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=yes\">", rCounter);
		if(mAutoRefresh == true)		
			rCounter = addToResponse(sResponse, "<meta http-equiv=\"refresh\" content=\""+ String(mRefreshTime) + "\" >", rCounter);

		//Calculate color information
		int r, g, b;
		convertColor(mBackgroundColor, &r, &g, &b);
		int diffStyle1 = 4;
		int diffStyle2 = 10;
		int rStyle1 = (r - diffStyle1 < 0)? 0 : (r - diffStyle1);
		int gStyle1 = (g - diffStyle1 < 0)? 0 : (g - diffStyle1);
		int bStyle1 = (b - diffStyle1 < 0)? 0 : (b - diffStyle1);
		int rStyle2 = (r - diffStyle2 < 0)? 0 : (r - diffStyle2);
		int gStyle2 = (g - diffStyle2 < 0)? 0 : (g - diffStyle2);
		int bStyle2 = (b - diffStyle2 < 0)? 0 : (b - diffStyle2);
		
		String colorStyle1 = convertColor(rStyle1, gStyle1, bStyle1);
		String colorStyle2 = convertColor(rStyle2, gStyle2, bStyle2);
		
		 
		//Style-information
		rCounter = addToResponse(sResponse, "<style>", rCounter);
		rCounter = addToResponse(sResponse, "body{background-color: " + mBackgroundColor + ";}body, input, textarea, select{font-family: 'Open Sans', sans-serif;line-height: 1.8em;color: " + mFontColor + ";font-weight: 300;}", rCounter);
		rCounter = addToResponse(sResponse, "h1 {font-size: 2em;color: " + mHeaderFontColor + "; font-variant: small-caps;}", rCounter);
		rCounter = addToResponse(sResponse, ".wrapper {box-shadow: inset 0px 1px 0px 0px rgba(0, 0, 0, 0.15), inset 0px 2px 3px 0px rgba(0, 0, 0, 0.15);}", rCounter);
		rCounter = addToResponse(sResponse, ".wrapper.style1 {background-color: " + mBackgroundColor + ";padding-top: 2em;}", rCounter);
		rCounter = addToResponse(sResponse, ".wrapper.style2 {background-color: " + colorStyle1 + ";padding-top: 10px;padding-bottom: 1em;}", rCounter);
		rCounter = addToResponse(sResponse, ".wrapper.style3 {background-color: " + colorStyle2 + ";padding-top: 10px;padding-bottom: 1em;}", rCounter);
		rCounter = addToResponse(sResponse, "#nav {background-color: #282828;text-align: center;position: fixed;left: 0;top: 0;width: 100%;z-index: 10000;cursor: default;}", rCounter);
		rCounter = addToResponse(sResponse, "#nav ul {margin-bottom: 0;margin-top: 0;}", rCounter);
		rCounter = addToResponse(sResponse, "#nav li {display: inline-block;padding-right: 1em;}", rCounter);
		rCounter = addToResponse(sResponse, "#nav a {position: relative;display: block;color: #fff;text-decoration: none;outline: 0;}", rCounter);
		rCounter = addToResponse(sResponse, "</style>", rCounter);
		
		//Send navigation-line
		rCounter = addToResponse(sResponse, "</head><body><nav id=\"nav\"><ul class=\"container\">", rCounter);
		rCounter = addToResponse(sResponse, "<li><a href=\"#\">Home</a></li>", rCounter);
		for(int c = 0; c < (int)mDynamicCategory.size(); c++) 
		{
			if(mDynamicCategory[c] == NULL)
					continue;
			rCounter = addToResponse(sResponse, "<li><a href=\"#" + mDynamicCategory[c]->getName() + "\">" + mDynamicCategory[c]->getDisplayName() + "</a></li>", rCounter);
		}
		rCounter = addToResponse(sResponse, "</ul></nav>", rCounter);
		rCounter = addToResponse(sResponse, "</ul></nav><div class=\"wrapper style1 first\"><article class=\"container\"><header><h1>", rCounter);
		rCounter = addToResponse(sResponse, mTitle + "</h1></header>", rCounter);
		rCounter = addToResponse(sResponse, "<p>" + mDescription + "</p><br>", rCounter);

		
		//Send each category and content
		for(int c = 0; c < (int)mDynamicCategory.size(); c++) 
		{
			if(mDynamicCategory[c] == NULL)
					continue;
			
			String wrapper_style = "";
			if(c%2 == 1)
				wrapper_style = "wrapper style2";
			else
				wrapper_style = "wrapper style3";

			//Create category-name
			rCounter = addToResponse(sResponse, "<div class=\"" + wrapper_style + "\" id=\"", rCounter);
			rCounter = addToResponse(sResponse, mDynamicCategory[c]->getName()+ "\"><article><header><h2>", rCounter);
			rCounter = addToResponse(sResponse, mDynamicCategory[c]->getDisplayName(), rCounter);
			rCounter = addToResponse(sResponse, "</h2><p>" + mDynamicCategory[c]->getDescription() + "</p></header>", rCounter);

			for(int i = 0; i < mDynamicCategory[c]->size(); i++) 
			{
				String tmp_response = "";

				if(mDynamicCategory[c]->getContent(i) == NULL)
					continue;

				
				//Write entry
				tmp_response += mDynamicCategory[c]->getContent(i)->getHTMLResponse(mDynamicCategory[c]->getName());

				//Write warning
				if(mDynamicCategory[c]->getContent(i)->isWarningEnabled() == true)
				{
					if(mDynamicCategory[c]->getContent(i)->getWarning() != "")
					{
						tmp_response += "<font color=\"" + mWarnColor + "\">";
						tmp_response += mDynamicCategory[c]->getContent(i)->getWarning() + "</font>";
					}
				}
				tmp_response += "<br>";
				
				rCounter = addToResponse(sResponse, tmp_response, rCounter);


			}
			rCounter = addToResponse(sResponse, "</article></div>", rCounter);

		}

		
		rCounter = addToResponse(sResponse, "</body></html>", rCounter);

		sHeader  = "HTTP/1.1 200 OK\r\n";
		sHeader += "Content-Length: ";		
		int response_length = 0;
		for(int i = 0; i < MAX_RESPONSE; i++)
		{
			//Serial.println("sResponseLength = " + String(((int)sResponse[i].length())));
			response_length += sResponse[i].length();
		}
		sHeader += String(response_length);
		sHeader += "\r\n";
		sHeader += "Content-Type: text/html\r\n";
		sHeader += "Connection: close\r\n";
		sHeader += "\r\n";
		
	}
  
// Send the response to the client
	mClient.print(sHeader);
	for(int i = 0; i < MAX_RESPONSE; i++)
	{
		if(sResponse[i] != "")
		{
			//Serial.println("sResponse = " + sResponse[i]);
			mClient.print(sResponse[i]);
		}
	}
  
 // and stop the client
	mClient.stop();

	return 0;

}


void DynamicWebServer::setTitle(String title) {

	mTitle = title;
}


void DynamicWebServer::setDescription(String description) {

	mDescription = description;
}


bool DynamicWebServer::addCategory(DynamicCategory* category) {
	if(category == NULL)
		return false;	

	mDynamicCategory.push_back(category);
	mIsExtern.push_back(true);
	return true;
}


bool DynamicWebServer::addCategory(String name, String displayName, String description) {

	if(getCategory(name) != NULL)
		return false;

	DynamicCategory* category = new DynamicCategory(name, displayName);
	mDynamicCategory.push_back(category);
	mIsExtern.push_back(false);
	return true;
}


bool DynamicWebServer::addContent(String category, DynamicContent* entry) {
	
	DynamicCategory* pointer = getCategory(category);
	if(pointer == NULL)
		return false;
	
	if(entry == NULL)
		return false;

	return pointer->addContent(entry);
}


bool DynamicWebServer::addContent(String category, 
		Content_type type, void* m_pointer, String name, String displayName, String description, HTML_type html_type) {

	DynamicCategory* pointer = getCategory(category);
	if(pointer == NULL)
		return false;

	return pointer->addContent(type, m_pointer, name, displayName, description, html_type);
}


DynamicCategory* DynamicWebServer::getCategory(String name) {

	for(int i = 0; i< (int)mDynamicCategory.size(); i++)
	{
		if(mDynamicCategory[i] == NULL)
			continue;

		if(mDynamicCategory[i]->getName() == name)
			return mDynamicCategory[i];
	}

	return NULL;
}


DynamicContent* DynamicWebServer::getContent(String category, String name) {
	
	DynamicCategory* pointer = getCategory(category);
	if(pointer == NULL)
		return NULL;

	return pointer->getContent(name);
}



bool DynamicWebServer::setMinValue(String category, String name, int min_value) {

	DynamicCategory* pointer = getCategory(category);
	if(pointer == NULL)
		return false;

	DynamicContent* pContent = pointer->getContent(name);
	if(pContent == NULL)
		return false;

	pContent->setMinValue(min_value);
	return true;
}


bool DynamicWebServer::setMaxValue(String category, String name, int max_value) {

	DynamicCategory* pointer = getCategory(category);
	if(pointer == NULL)
		return false;

	DynamicContent* pContent = pointer->getContent(name);
	if(pContent == NULL)
		return false;

	pContent->setMaxValue(max_value);
	return true;
}


bool DynamicWebServer::setMinValue(String category, String name, float min_value) {

	DynamicCategory* pointer = getCategory(category);
	if(pointer == NULL)
		return false;

	DynamicContent* pContent = pointer->getContent(name);
	if(pContent == NULL)
		return false;

	pContent->setMinValue(min_value);
	return true;
}


bool DynamicWebServer::setMaxValue(String category, String name, float max_value) {

	DynamicCategory* pointer = getCategory(category);
	if(pointer == NULL)
		return false;

	DynamicContent* pContent = pointer->getContent(name);
	if(pContent == NULL)
		return false;

	pContent->setMaxValue(max_value);
	return true;
}


bool DynamicWebServer::setMaxLength(String category, String name, int max_length) {

	DynamicCategory* pointer = getCategory(category);
	if(pointer == NULL)
		return false;

	DynamicContent* pContent = pointer->getContent(name);
	if(pContent == NULL)
		return false;

	pContent->setMaxLength(max_length);
	return true;
}


bool DynamicWebServer::enableWarning(String category, String name) {

	DynamicCategory* pointer = getCategory(category);
	if(pointer == NULL)
		return false;

	DynamicContent* pContent = pointer->getContent(name);
	if(pContent == NULL)
		return false;

	pContent->enableWarning();
	return true;
}


bool DynamicWebServer::disableWarning(String category, String name) {

	DynamicCategory* pointer = getCategory(category);
	if(pointer == NULL)
		return false;

	DynamicContent* pContent = pointer->getContent(name);
	if(pContent == NULL)
		return false;

	pContent->disableWarning();
	return true;
}


bool DynamicWebServer::setSize(String category, String name, int size) {
	
	DynamicCategory* pointer = getCategory(category);
	if(pointer == NULL)
		return false;

	DynamicContent* pContent = pointer->getContent(name);
	if(pContent == NULL)
		return false;

	pContent->setSize(size);
	return true;
}


bool DynamicWebServer::setColor(String category, String name, String s) {
	
	DynamicCategory* pointer = getCategory(category);
	if(pointer == NULL)
		return false;

	DynamicContent* pContent = pointer->getContent(name);
	if(pContent == NULL)
		return false;

	pContent->setColor(s);
	return true;
}


bool DynamicWebServer::setColor(String category, String name, int r, int g, int b) {
	
	DynamicCategory* pointer = getCategory(category);
	if(pointer == NULL)
		return false;

	DynamicContent* pContent = pointer->getContent(name);
	if(pContent == NULL)
		return false;

	pContent->setColor(r, g, b);
	return true;
}


bool DynamicWebServer::setButtonColor(String s) {
	
	for(int i = 0; i < (int) mDynamicCategory.size(); i++)
	{
		if(mDynamicCategory[i] != NULL)
			mDynamicCategory[i]->setButtonColor(s);
	}

	return true;
}


bool DynamicWebServer::setButtonColor(int r, int g, int b) {
	
	for(int i = 0; i < (int) mDynamicCategory.size(); i++)
	{
		if(mDynamicCategory[i] != NULL)
			mDynamicCategory[i]->setButtonColor(r, g, b);
	}

	return true;
}


bool DynamicWebServer::setStep(String category, String name, int step) {
	
	DynamicCategory* pointer = getCategory(category);
	if(pointer == NULL)
		return false;

	DynamicContent* pContent = pointer->getContent(name);
	if(pContent == NULL)
		return false;

	pContent->setStep(step);
	return true;
}


bool DynamicWebServer::setStep(String category, String name, float step) {
	
	DynamicCategory* pointer = getCategory(category);
	if(pointer == NULL)
		return false;

	DynamicContent* pContent = pointer->getContent(name);
	if(pContent == NULL)
		return false;

	pContent->setStep(step);
	return true;
}


bool DynamicWebServer::removeCategory(String category) {

	for(int i = 0; i< (int)mDynamicCategory.size(); i++)
	{
		if(mDynamicCategory[i] == NULL)
			continue;

		if(mDynamicCategory[i]->getName() == category)
		{
			if(mIsExtern[i] == false)
			{
				delete(mDynamicCategory[i]);
				mDynamicCategory[i] = NULL;
			}
			else
			{
				mDynamicCategory[i] = NULL;
			}
			return true;
		}
	}

	return false;
}


bool DynamicWebServer::removeContent(String category, String name) {

	DynamicCategory* pointer = getCategory(category);
	if(pointer == NULL)
		return false;
	
	return pointer->removeContent(name);
}


bool DynamicWebServer::setAutoRefresh(int time) {

	if(time > 0)
	{
		mAutoRefresh = true;
		mRefreshTime = time;
	}
	else
	{
		mAutoRefresh = false;
	}

	return true;

}
