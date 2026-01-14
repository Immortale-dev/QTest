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

#include <vector>
#include <iostream>
#include <string>
#include <string_view>
#include <algorithm>
#include <sstream>

namespace Q_TEST_NS_DETAIL {

class QTestPrint
{
	using test_infos = std::vector<std::stringstream>;

	public:
		QTestPrint();
		void print(std::string_view s);
		void print(std::string&& s);
		void print(const char* s);

		void print_description(std::string_view str);
		void print_test(std::string_view str, bool good, bool skipped);
		void print_test_info(std::string_view arr);
		void print_test_error(std::string_view s);
		void print_failed_test(std::string_view str, std::string_view file, int line);
		void print_statistics(int tests_count, int tests_failed, int tests_skipped);
		void print_start();
		void print_title(std::string_view str);
		void print_delimeter();
		void print_delimeter(std::string_view c);
		void print_error(std::string_view s);
		void print_success(std::string_view s);
		void print_neutral(std::string_view s);
		void print_grey(std::string_view s);
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

		static constexpr std::string_view newline = "\n";
		static constexpr std::string_view tab = "    ";
		static constexpr std::string_view delim_txt = "*";
		static constexpr std::string_view succeed_txt = "succeed";
		static constexpr std::string_view failed_txt = "failed";
		static constexpr std::string_view skipped_txt = "skipped";
		static constexpr std::string_view statistics_txt = "statistics";
		static constexpr std::string_view testing_txt = "testing";
		static constexpr std::string_view succ_sign = "[/]";
		static constexpr std::string_view fail_sign = "[x]";
		static constexpr std::string_view skip_sign = "[-]";

		std::string create_titled_message(std::string_view str);
		std::string toupper(std::string_view txt);
		void processConsoleWindow();
		void set_color_default();
		void set_color_error();
		void set_color_success();
		void set_color_neutral();
		void set_color_grey();
		void set_color(Color c);
};


inline QTestPrint::QTestPrint()
{
	processConsoleWindow();
}

inline void QTestPrint::print_description(std::string_view str)
{
	print(newline);
	print("  ");
	print(str);
	print(newline);
}

inline void QTestPrint::print_test(std::string_view str, bool good, bool skipped)
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
	print_grey(str);

	if(skipped) {
		std::string str = std::string{" ("} + std::string(skipped_txt) + std::string{")"};
		print_neutral(str);
	}

	print(newline);
}

inline void QTestPrint::print_test_info(std::string_view s)
{
	print("        ");
	print(" - ");
	print_grey(s);
	print(newline);
}

inline void QTestPrint::print_test_error(std::string_view s)
{
	print("        ");
	print(" - ");
	print_error(s);
	print(newline);
}

inline void QTestPrint::print_failed_test(std::string_view str, std::string_view file, int line)
{
	print(tab);
	print_error_sign();
	print(" ");
	print_grey(str);
	std::string pstr = std::string{" ("} + std::string(file) + std::string{":"} + std::to_string(line) + std::string{")"};
	print_neutral(pstr);
	print(newline);
}

inline void QTestPrint::print_error_sign()
{
	print_error(fail_sign);
}

inline void QTestPrint::print_success_sign()
{
	print_success(succ_sign);
}

inline void QTestPrint::print_skip_sign()
{
	print_neutral(skip_sign);
}

inline void QTestPrint::print_statistics(int tests_count, int tests_failed, int tests_skipped)
{
	print(newline);
	std::string title_upper = toupper(statistics_txt);
	print_title(title_upper);
	print(newline);

	print_success_sign();
	std::string success_message = std::string{" "} + toupper(succeed_txt) + std::string(": ");
	print_grey(success_message);
	print(std::to_string(tests_count-tests_failed-tests_skipped));
	print(newline);

	if(tests_skipped){
		print_skip_sign();
		std::string skip_message = std::string{" "} + toupper(skipped_txt) + std::string{": "};
		print_grey(skip_message);
		print(std::to_string(tests_skipped));
		print(newline);
	}

	if(tests_failed){
		print_error_sign();
		std::string failed_message = std::string{" "} + toupper(failed_txt) + std::string{": "};
		print_grey(failed_message);
		print(std::to_string(tests_failed));
		print(newline);
	}
	print(newline);
}

inline void QTestPrint::print_start()
{
	print_delimeter("_");
	print_title(toupper(testing_txt));
}

inline std::string QTestPrint::create_titled_message(std::string_view str)
{
	std::string ret = "";
	int blength = line_length-(int)str.size();
	for(int i=0;i<blength/2;i++)
		ret += delim_txt;
	ret += str;
	for(int i=0;i<blength-blength/2;i++)
		ret += delim_txt;
	return ret;
}

inline void QTestPrint::print_title(std::string_view str)
{
	print(create_titled_message(str));
}

inline void QTestPrint::print_delimeter()
{
	print_delimeter(delim_txt);
}

inline void QTestPrint::print_delimeter(std::string_view c)
{
	for(int i=0;i<line_length;i++)
		print(c);
}

inline void QTestPrint::print(std::string_view s)
{
	std::cout << s;
}

inline void QTestPrint::print(std::string&& s)
{
	print(s);
}

inline void QTestPrint::print(const char* s)
{
	print(std::string_view(s));
}

inline void QTestPrint::print_error(std::string_view s)
{
	set_color_error();
	print(s);
	set_color_default();
}

inline void QTestPrint::print_success(std::string_view s)
{
	set_color_success();
	print(s);
	set_color_default();
}

inline void QTestPrint::print_neutral(std::string_view s)
{
	set_color_neutral();
	print(s);
	set_color_default();
}

inline void QTestPrint::print_grey(std::string_view s)
{
	set_color_grey();
	print(s);
	set_color_default();
}

inline std::string QTestPrint::toupper(std::string_view txt)
{
	std::string txtc(txt);
	std::transform(txtc.begin(), txtc.end(), txtc.begin(), ::toupper); 
	return txtc;
}

inline void QTestPrint::processConsoleWindow()
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

inline void QTestPrint::set_color_default()
{
	set_color(Color::Default);
}

inline void QTestPrint::set_color_error()
{
	set_color(Color::Error);
}

inline void QTestPrint::set_color_success()
{
	set_color(Color::Success);
}

inline void QTestPrint::set_color_neutral()
{
	set_color(Color::Neutral);
}

inline void QTestPrint::set_color_grey()
{
	set_color(Color::Grey);
}

inline void QTestPrint::set_color(Color c)
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
		std::string color;
		switch(c){
			case Color::Success:
				color = "\033[32m";
				break;
			case Color::Error:
				color = "\033[31m";
				break;
			case Color::Neutral:
				color = "\033[96m";
				break;
			case Color::Grey:
				color = "\033[37m";
				break;
			default:
				color = "\033[39m";
		}
		print(color);
	#endif
}

inline void QTestPrint::print_succeed_message()
{
	print_success(create_titled_message(toupper(succeed_txt)));
}

inline void QTestPrint::print_failure_message()
{
	print_error(create_titled_message(toupper(failed_txt)));
}

} // Q_TEST_NS_DETAIL

#endif //QTESTPRINT_H
