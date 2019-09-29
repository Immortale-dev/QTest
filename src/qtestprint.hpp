#ifndef QTESTPRINT_H
#define QTESTPRINT_H

#ifdef _WIN32
// For windows
#include <windows.h>
#else
// For unix
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include <iostream>
#include <string>
#include <algorithm>

class QTestPrint
{
	using string = std::string;
	
	public:
		QTestPrint();
		void print_description(string& str);
		void print_description_extended(string& str, string& file);
		void print_test(string& str, bool good, bool skipped);
		void print_falied_test(string& str);
		void print_statistics(int tests_count, int tests_failed, int tests_skipped);
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
		void print_skip_sign();
		void print_succeed_message();
		void print_failure_message();
		
	private:
		#ifdef _WIN32
		HANDLE hConsole;
		DWORD def_bgcolor, def_color;
		#endif
		int line_length = 60;
		
		enum class Color{Success, Error, Neutral, Grey, Default};
		
		const string newline = "\n";
		const string tab = "    ";
		const string delim_txt = "*";
		const string succeed_txt = "succeed";
		const string failed_txt = "failed";
		const string skipped_txt = "skipped";
		const string statistics_txt = "statistics";
		const string testing_txt = "testing";
		const string succ_sign = "[/]";
		const string fail_sign = "[x]";
		const string skip_sign = "[-]";
		
		string create_titled_message(string str);
		string toupper(string txt);
		void processConsoleWindow();
		void set_color_default();
		void set_color_error();
		void set_color_success();
		void set_color_neutral();
		void set_color_grey();
		void set_color(Color c);
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

void QTestPrint::print_test(string& str, bool good, bool skipped)
{
	print("    ");
	
	if(skipped){
		print_skip_sign();
	}
	else if(good){
		print_success_sign();
	}
	else{
		print_error_sign();
	}
		
	print(" ");
	good ? print_grey(str) : print_error(str);
	
	if(skipped)
		print_neutral(" ("+skipped_txt+")");
	
	print(newline);
}

void QTestPrint::print_falied_test(string& str)
{
	print(tab);
	print_error_sign();
	print(" ");
	print_grey(str);
	print(newline);
}

void QTestPrint::print_error_sign()
{
	print_error(fail_sign);
}

void QTestPrint::print_success_sign()
{
	print_success(succ_sign);
}

void QTestPrint::print_skip_sign()
{
	print_neutral(skip_sign);
}

void QTestPrint::print_statistics(int tests_count, int tests_failed, int tests_skipped)
{
	print(newline);
	print_title(toupper(statistics_txt));
	print(newline);
	
	print_success_sign();
	print_grey(" "+toupper(succeed_txt)+": ");
	print(std::to_string(tests_count-tests_failed-tests_skipped));
	print(newline);
	
	if(tests_skipped){
		print_skip_sign();
		print_grey(" "+toupper(skipped_txt)+": ");
		print(std::to_string(tests_skipped));
		print(newline);
	}
	
	if(tests_failed){
		print_error_sign();
		print_grey(" "+toupper(failed_txt)+": ");
		print(std::to_string(tests_failed));
		print(newline);
	}
	print(newline);
}

void QTestPrint::print_start()
{
	print_delimeter("_");
	print_title(toupper(testing_txt));
}

std::string QTestPrint::create_titled_message(string str)
{
	string ret = "";
	int blength = line_length-(int)str.size();
	for(int i=0;i<blength/2;i++)
		ret += delim_txt;
	ret += str;
	for(int i=0;i<blength-blength/2;i++)
		ret += delim_txt;
	return ret;
}

void QTestPrint::print_title(string str)
{
	print(create_titled_message(str));
}

void QTestPrint::print_delimeter()
{
	print_delimeter(delim_txt);
}

void QTestPrint::print_delimeter(string c)
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

std::string QTestPrint::toupper(string txt)
{
	string txtc(txt);
	std::transform(txtc.begin(), txtc.end(), txtc.begin(), ::toupper); 
	return txtc;
}

void QTestPrint::processConsoleWindow()
{
	#ifdef _WIN32
		hConsole = GetStdHandle (STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(hConsole, &csbi);
		line_length = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		def_color = (csbi.wAttributes%16);
		def_bgcolor = ((csbi.wAttributes-def_color)%128);
	#else
		struct winsize wn;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &wn);
		line_length = wn.ws_col;
    #endif
}

void QTestPrint::set_color_default()
{
	set_color(Color::Default);
}

void QTestPrint::set_color_error()
{
	set_color(Color::Error);
}

void QTestPrint::set_color_success()
{
	set_color(Color::Success);
}

void QTestPrint::set_color_neutral()
{
	set_color(Color::Neutral);
}

void QTestPrint::set_color_grey()
{
	set_color(Color::Grey);
}

void QTestPrint::set_color(Color c)
{
	#ifdef TEST_RESULTS_NO_COLOR
		return;
	#endif
	#ifdef _WIN32
		int color;
		switch(c){
			case Color::Success:
				color = FOREGROUND_GREEN | def_bgcolor;
				break;
			case Color::Error:
				color = FOREGROUND_RED | def_bgcolor;
				break;
			case Color::Neutral:
				color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY | def_bgcolor;
				break;
			case Color::Grey:
				color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | def_bgcolor;
				break;
			default:
				color = def_color | def_bgcolor;
		}
		SetConsoleTextAttribute(hConsole, color);
	#else
		string color;
		switch(c){
			case Color::Success:
				color = "\e[32m";
				break;
			case Color::Error:
				color = "\e[31m";
				break;
			case Color::Neutral:
				color = "\e[96m";
				break;
			case Color::Grey:
				color = "\e[37m";
				break;
			default:
				color = "\e[39m";
		}
		print(color);
	#endif
}

void QTestPrint::print_succeed_message()
{
	print_success(create_titled_message(toupper(succeed_txt)));
}

void QTestPrint::print_failure_message()
{
	print_error(create_titled_message(toupper(failed_txt)));
}


#endif //QTESTPRINT_H
