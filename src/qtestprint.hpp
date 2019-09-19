#ifndef QTESTPRINT_H
#define QTESTPRINT_H

#include <iostream>
#include <windows.h>
#include <string>

class QTestPrint
{
	using string = std::string;
	
	public:
		QTestPrint();
		void print_description(string& str);
		void print_description_extended(string& str, string& file);
		void print_test(string& str, bool good);
		void print_falied_test(string& str);
		void print_statistics(int tests_count, int tests_failed);
		void print_start();
		void print_title(string str);
		void print_delimeter();
		void print_delimeter(string c);
		void print(string s);
		void print_error(string s);
		void print_success(string s);
		void print_neutral(string s);
		void print_grey(string s);
		void print_error_sign();
		void print_success_sign();
		void print_succeed_message();
		void print_failure_message();
		
	private:
		HANDLE hConsole;
		DWORD def_bgcolor, def_color;
		int line_length = 60;
		
		const string newline = "\n";
		
		string create_titled_message(string str);
		void processConsoleWindow();
		void set_color_default();
		void set_color_error();
		void set_color_success();
		void set_color_neutral();
		void set_color_grey();
};


QTestPrint::QTestPrint()
{
	processConsoleWindow();
}

void QTestPrint::print_description(string& str)
{
	print(newline);
	print(" "+str+newline);
}

void QTestPrint::print_description_extended(string& str, string& file)
{
	print(newline);
	print(" "+str);
	print_neutral("("+file+")");
	print(newline);
}

void QTestPrint::print_test(string& str, bool good)
{
	print("    ");
	(good) ? print_success_sign() : print_error_sign();
	print(" ");
	good ? print_grey(str) : print_error(str);
	print(newline);
}

void QTestPrint::print_falied_test(string& str)
{
	print("    ");
	print_error_sign();
	print(" ");
	print_grey(str);
	print(newline);
}

void QTestPrint::print_error_sign()
{
	print_error("[x]");
}

void QTestPrint::print_success_sign()
{
	print_success("[o]");
}

void QTestPrint::print_statistics(int tests_count, int tests_failed)
{
	print(newline);
	print_title("STATISTICS");
	print(newline);
	
	print_success_sign();
	print_grey(" SUCCEED: ");
	print(std::to_string(tests_count-tests_failed));
	print(newline);
	
	if(tests_failed){
		print_error_sign();
		print_grey(" FAILED: ");
		print(std::to_string(tests_failed));
		print(newline);
	}
	print(newline);
}

void QTestPrint::print_start()
{
	print_delimeter("_");
	print_title("TESTING");
}

std::string QTestPrint::create_titled_message(string str)
{
	string ret = "";
	int blength = line_length-(int)str.size();
	for(int i=0;i<blength/2;i++)
		ret += "*";
	ret += str;
	for(int i=0;i<blength-blength/2;i++)
		ret += "*";
	return ret;
}

void QTestPrint::print_title(string str)
{
	print(create_titled_message(str));
}

void QTestPrint::print_delimeter()
{
	print_delimeter("*");
}

void QTestPrint::print_delimeter(string c = "*")
{
	for(int i=0;i<line_length;i++)
		print(c);
}

void QTestPrint::print(string s)
{
	std::cout << s;
}

void QTestPrint::print_error(string s)
{
	set_color_error();
	print(s);
	set_color_default();
}

void QTestPrint::print_success(string s)
{
	set_color_success();
	print(s);
	set_color_default();
}

void QTestPrint::print_neutral(string s)
{
	set_color_neutral();
	print(s);
	set_color_default();
}

void QTestPrint::print_grey(string s)
{
	set_color_grey();
	print(s);
	set_color_default();
}

void QTestPrint::processConsoleWindow()
{
	hConsole = GetStdHandle (STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	
    line_length = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    
	def_color = (csbi.wAttributes%16);
	def_bgcolor = ((csbi.wAttributes-def_color)%128);
}

void QTestPrint::set_color_default()
{
	SetConsoleTextAttribute(hConsole, def_color | def_bgcolor);
}

void QTestPrint::set_color_error()
{
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | def_bgcolor);
}

void QTestPrint::set_color_success()
{
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | def_bgcolor);
}

void QTestPrint::set_color_neutral()
{
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY | def_bgcolor);
}

void QTestPrint::set_color_grey()
{
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | def_bgcolor);
}

void QTestPrint::print_succeed_message()
{
	print_success(create_titled_message("SUCCEED"));
}

void QTestPrint::print_failure_message()
{
	print_error(create_titled_message("FAILED"));
}


#endif //QTESTPRINT_H
