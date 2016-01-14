#include "common.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <time.h>

extern HWND hTxt[2] ;
extern vector<string> vcLog;

string GetConfigSetting(char* p_key)
{
	char* ini_filename = "config.txt";
	char* result = new char[100];
	std::ifstream ini_file(ini_filename);
	if(!ini_file)
	{
		std::cout << "cannot open the file!"<<std::endl;
		return "Error: Cannot open the file.";
	}
	std::string line;
	while(std::getline(ini_file,line))
	{
		int i = line.find(p_key,0);
		if(i==0)
		{
			int k = line.find("=",0);
			std::string value = line.substr(k+1);
			//result = const_cast<char*> (value.c_str());
			return value;
		}
	}
	return "Failed.";
}


//////////////////////////////////////////////////////////////////
void showText(HWND hWnd,string s)
{
	size_t size = s.length();
	wchar_t *buffer = new wchar_t[size+1];
	MultiByteToWideChar( CP_ACP, 0, s.c_str(), size, buffer, size * sizeof(wchar_t) );
	buffer[size] = 0;  // 确保以 '\0' 结尾 
	//return(buffer);
	SetWindowText(hWnd,buffer);
}
//////////////////////////////////////////////////////////////////
void showPrice(HWND hwnd,double price)
{
	double d = price;
	if(d == DBL_MAX || d == DBL_MIN)
		d = 0;

	char c_temp[20];
	sprintf(c_temp,"%.3f",d);
	size_t size = sizeof(c_temp);
	wchar_t *buffer = new wchar_t[size+1];
	MultiByteToWideChar( CP_ACP, 0, c_temp, size, buffer, size * sizeof(wchar_t) );
	buffer[size] = 0;  // 确保以 '\0' 结尾 
	SetWindowText(hwnd, buffer);
}

//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
string GetNow()
{
	struct tm *newtime;
	char tmpbuf[128];
	time_t tt;
	tt=time( NULL );
	newtime=localtime(&tt);
	strftime( tmpbuf, 128, "[%Y-%m-%d %H:%M:%S]", newtime);
	//strftime( tmpbuf, 128, now_format.c_str(), newtime);
	string s = tmpbuf;
	return s;
}
//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
string GetDate()
{
	struct tm *newtime;
	char tmpbuf[128];
	time_t tt;
	tt=time( NULL );
	newtime=localtime(&tt);
	strftime( tmpbuf, 128, "%Y%m%d", newtime);
	//strftime( tmpbuf, 128, now_format.c_str(), newtime);
	string s = tmpbuf;
	return s;
}
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
void WriteLog(string s)
{

	//if(vcLog.size()>20)
	//	vcLog.clear();

	//vcLog.push_back(s);
	//string sTemp = GetNow();
	//for(vector<string>::size_type ix = 0; ix != vcLog.size(); ++ ix)
	//	sTemp = sTemp + vcLog[ix] + + "\r\n";

	//size_t size = s.length();
	//wchar_t *buffer = new wchar_t[size+1];
	//MultiByteToWideChar( CP_ACP, 0, s.c_str(), size, buffer, size * sizeof(wchar_t) );
	//buffer[size] = 0;  // 确保以 '\0' 结尾 
	//SetWindowTextW(hTxt[9],buffer);
	ofstream log_file;
	log_file.open("log"+GetDate()+".txt",ios::app);
	log_file << GetNow() << s << endl;
	log_file.close();

}


