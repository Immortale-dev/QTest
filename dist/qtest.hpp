/* v2.0.2 */
#ifndef QTEST_H
#define QTEST_H

#include <functional>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <memory>
#include <string_view>
#include <cmath>
#include <initializer_list>
#include <type_traits>
#include <iomanip>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#define QTEST_TEST_PARAM_ID 0
#define QTEST_ONLY_PARAM_ID 1
#define QTEST_SKIP_PARAM_ID 2
#define Q_TEST__SCOPE &
#define Q_TEST__CONCAT_TOKENPASTE(a,b) a ## b
#define Q_TEST__CONCAT(a,b) Q_TEST__CONCAT_TOKENPASTE(a,b)
#define Q_TEST__STRINGIFY_TOKENPASTE(x) #x
#define Q_TEST__STRINGIFY(x) Q_TEST__STRINGIFY_TOKENPASTE(x)
#define Q_TEST__UNIQ_NAME_GENERATE(a) Q_TEST__CONCAT( Q_TEST__CONCAT(a, __LINE__), __COUNTER__)
#define Q_TEST__UNIQ_NAME() Q_TEST__UNIQ_NAME_GENERATE(Q___TEST___U_N_I_Q___)
#define Q_TEST__LAMBDA(...) [Q_TEST__SCOPE]()__VA_ARGS__
#define Q_TEST__LAMBDA_CALLBACK(...) [Q_TEST__SCOPE](auto callback){ do __VA_ARGS__ while((callback(), false)); }
#define Q_TEST__TEST_UNIT inline static Q_TEST_NS_DETAIL::QTestScenario Q_TEST__UNIQ_NAME()
#define Q_TEST__RETURN_IF_FALSE(...) for (int _some_val_=1,_result_val_=1;;) if (!_some_val_--) { if (!_result_val_) return; break; } else _result_val_ = __VA_ARGS__

// PUBLIC API
#define DESCRIBE(a, ...) Q_TEST_NS_DETAIL::BASE.describe(a, Q_TEST__LAMBDA_CALLBACK(__VA_ARGS__), QTEST_TEST_PARAM_ID, __FILE__)
#define DESCRIBE_ONLY(a, ...) Q_TEST_NS_DETAIL::BASE.describe(a, Q_TEST__LAMBDA_CALLBACK(__VA_ARGS__), QTEST_ONLY_PARAM_ID, __FILE__)
#define DESCRIBE_SKIP(a, ...) Q_TEST_NS_DETAIL::BASE.describe(a, Q_TEST__LAMBDA_CALLBACK(__VA_ARGS__), QTEST_SKIP_PARAM_ID, __FILE__)
#define BEFORE_ALL(...) Q_TEST_NS_DETAIL::BASE.before(Q_TEST__LAMBDA(__VA_ARGS__))
#define BEFORE_EACH(...) Q_TEST_NS_DETAIL::BASE.before_each(Q_TEST__LAMBDA(__VA_ARGS__))
#define AFTER_ALL(...) Q_TEST_NS_DETAIL::BASE.after(Q_TEST__LAMBDA(__VA_ARGS__))
#define AFTER_EACH(...) Q_TEST_NS_DETAIL::BASE.after_each(Q_TEST__LAMBDA(__VA_ARGS__))
#define IT(a, ...) Q_TEST_NS_DETAIL::BASE.it(a, Q_TEST__LAMBDA(__VA_ARGS__), QTEST_TEST_PARAM_ID, __LINE__)
#define IT_ONLY(a, ...) Q_TEST_NS_DETAIL::BASE.it(a, Q_TEST__LAMBDA(__VA_ARGS__), QTEST_ONLY_PARAM_ID, __LINE__)
#define IT_SKIP(a, ...) Q_TEST_NS_DETAIL::BASE.it(a, Q_TEST__LAMBDA(__VA_ARGS__), QTEST_SKIP_PARAM_ID, __LINE__)
#define EXPECT(a) Q_TEST__RETURN_IF_FALSE(Q_TEST_NS_DETAIL::BASE.expect((a), Q_TEST__STRINGIFY(a)))
#define INFO_PRINT(a) Q_TEST_NS_DETAIL::BASE.info_print(a)
#define TEST_FAILED(a) EXPECT(std::string{a}).fail();
#define TEST_SUCCEED() EXPECT(1).toBe(1)
#define SCENARIO_START Q_TEST__TEST_UNIT ([]{
#define SCENARIO_END });
// END OF PUBLIC API

namespace Q_TEST_NS_DETAIL {

struct ErrorReport {
	std::string value = {};
	std::string compare = {};
	std::string_view func = {};
	bool inverse = false;
	bool has_compare = false;
	bool value_substituted = false;
	bool compare_substituted = false;
};

template <typename T, typename = void>
struct is_streamable : std::false_type {};

struct NonStreamable{};

template <typename T>
struct is_streamable<T, std::enable_if_t<std::is_convertible_v<decltype(std::declval<std::ostream &>() << std::declval<T>()),std::ostream &>>> : std::true_type {};

template<typename T>
class QTestExpect {
	public:
		QTestExpect(T&& a, bool* result, ErrorReport* error) : val(std::move(a)), result(result), error(error) {};
		QTestExpect(T& a, bool* result, ErrorReport* error) : val(a), result(result), error(error) {};
		template<typename C> bool toBe(C&& compare);
		bool toBeCloseTo(T compare, T precision);
		bool toBeGreaterThan(T compare);
		bool toBeGreaterThanOrEqual(T compare);
		bool toBeLessThan(T compare);
		bool toBeLessThanOrEqual(T compare);
		bool toBeNull();
		bool toBeNullPtr();
		template<typename CT> bool toBeIterableEqual(CT&& compare);
		template<typename CT> bool toBeIterableEqual(std::initializer_list<CT> list);
		bool toThrowError();
		template<typename E> bool toThrowError();
		bool toReturnTrue();
		template<typename V> bool toReturn(V&& compare);
		QTestExpect<T> NOT();
		bool fail();

		template<typename C> bool to_be(C&& compare){ return toBe(std::move(compare)); }
		bool to_be_close_to(T compare, T precision){ return toBeCloseTo(compare, precision); }
		bool to_be_greater_than(T compare){ return toBeGreaterThan(compare); }
		bool to_be_greater_than_or_equal(T compare){ return toBeGreaterThanOrEqual(compare); }
		bool to_be_less_than(T compare){ return toBeLessThan(compare); }
		bool to_be_less_than_or_equal(T compare){ return toBeLessThanOrEqual(compare); }
		bool to_be_null(){ return toBeNull(); }
		bool to_be_null_ptr(){ return toBeNullPtr(); }
		template<typename CT> bool to_be_iterable_equal(CT&& compare){ return toBeIterableEqual(std::forward(compare)); }
		template<typename CT> bool to_be_iterable_equal(std::initializer_list<CT> list){ return toBeIterableEqual(list); }
		bool to_throw_error(){ return toThrowError(); }
		template<typename E> bool to_throw_error(){ return toThrowError<E>(); }
		bool to_return_true(){ return toReturnTrue(); }
		template<typename V> bool to_return(V&& compare) { return toReturn(compare); }

	private:
		bool proceed_result(bool result);
		void report_error_resolved(std::string_view func, std::string_view value, std::string_view compare);
		template<typename V, typename C> void report_error(std::string_view func, V&& value, C&& compare);
		template<typename V> void report_error(std::string_view func, V&& value);
		template<typename CT> std::string iterable_to_str(CT&& value);
		template<typename CT> std::string streamable_to_str(CT&& value);

		T val;
		bool* result;
		ErrorReport* error;
		bool inv = false;
};

class QTestPrint {
	enum class Color{Success, Error, Neutral, Grey, Default};
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
		std::string create_titled_message(std::string_view str);
		std::string toupper(std::string_view txt);
		void processConsoleWindow();
		void set_color_default();
		void set_color_error();
		void set_color_success();
		void set_color_neutral();
		void set_color_grey();
		void set_color(Color c);

		#ifdef _WIN32
		HANDLE hConsole;
		DWORD def_bgcolor, def_color;
		#endif
		int line_length = 60;
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
};

struct QTestScenario { QTestScenario(std::function<void()> fn) { fn(); } };

inline std::string sanitize(std::string& value)
{
	std::string str;
	for (char c : value) {
		switch(c) {
			case '\n':
				str.append("\\n");
				break;
			case '\r':
				str.append("\\r");
				break;
			case '\t':
				str.append("\\t");
				break;
			case '\b':
				str.append("\\b");
				break;
			case '\x1b':
				str.append("\\x1b");
				break;
			default:
				str.push_back(c);
		}
	}
	return str;
}

class QTestBase {
	using function_cb_t = std::function<void()>;
	using describe_function_cb_t = std::function<void(std::function<void()>)>;
	struct Describe {
		Describe(std::string str, std::string_view file, int mode) : text(str), file(file), mode(mode) {}
		bool is_skip() { return mode == QTEST_SKIP_PARAM_ID; }
		bool is_only() { return mode == QTEST_ONLY_PARAM_ID; }
		std::string text;
		std::string_view file;
		int mode;
		int tests_ran = 0;
		std::vector<function_cb_t> before_alls = {};
		std::vector<function_cb_t> after_alls = {};
		std::vector<function_cb_t> before_eachs = {};
		std::vector<function_cb_t> after_eachs = {};
	};
	struct Test {
		Test(std::string str, int line) : text(str), line(line) {}
		std::string text;
		int line;
		std::string_view expect_str = "";
		std::vector<std::stringstream> info_prints = {};
		ErrorReport error = {};
		bool result = true;
	};
	struct FailedTest {
		std::vector<std::shared_ptr<Test>> tests;
		std::vector<std::shared_ptr<Describe>> stack;
	};

	public:
		QTestBase();
		~QTestBase();
		void describe(std::string str, describe_function_cb_t fn, int param, std::string_view file);
		void before(function_cb_t fn);
		void before_each(function_cb_t fn);
		void after(function_cb_t fn);
		void after_each(function_cb_t fn);
		void it(std::string str, function_cb_t fn, int param, int line);
		template<typename T> std::basic_ostream<char>& info_print(T&& str);
		std::basic_ostream<char>& info_print();
		template<typename T> QTestExpect<T> expect(T&& a, std::string_view s);
		template<typename T> QTestExpect<T> expect(T& a, std::string_view s);
		bool current_test_good();

	private:
		Describe& current_describe();
		void test_precalls();
		void test_postcalls();
		bool in_skip_describe();
		bool in_only_describe();
		void current_describe_ran_inc();
		bool current_describe_ran();
		void call_before_all(Describe& d);
		void call_after_all(Describe& d);
		void call_before_each(Describe& d);
		void call_after_each(Describe& d);
		void show_start();
		void show_statistics();
		void show_test_results(Test& t, bool is_skip);
		void show_failed_test_results(Test& t, std::string_view file);
		void show_test_infos(Test& t);
		void show_failed_tests();
		void show_succeed();

		std::string generate_describes_text(std::vector<std::shared_ptr<Describe>>& descrs);
		std::string generate_test_error(std::string_view expect_str, ErrorReport& error);
		std::vector<std::shared_ptr<Describe>> describes;
		std::vector<FailedTest> failed_tests;
		std::shared_ptr<Test> current_test;
		std::unique_ptr<QTestPrint> P;
		int tests_count = 0;
		int tests_failed = 0;
		int tests_skipped = 0;
		bool tests_only = false;
		bool describes_changed = false;
};

template<typename T>
template<typename C>
bool QTestExpect<T>::toBe(C&& compare)
{
	if (!(*result &= proceed_result(val == compare))) report_error(__func__, val, compare);
	return *result;
}

template<typename T>
bool QTestExpect<T>::toBeCloseTo(T compare, T precision)
{
	T v = val;
	if (!(*result &= proceed_result(std::abs(v-compare) <= std::abs(precision)))) {
		report_error_resolved(__func__, streamable_to_str(val), streamable_to_str(compare) + ", " + streamable_to_str(precision));
	}
	return *result;
}

template<typename T>
bool QTestExpect<T>::toBeGreaterThan(T compare)
{
	if (!(*result &= proceed_result(val > compare))) report_error(__func__, val, compare);
	return *result;
}

template<typename T>
bool QTestExpect<T>::toBeGreaterThanOrEqual(T compare)
{
	if(!(*result &= proceed_result(val >= compare))) report_error(__func__, val, compare);
	return *result;
}

template<typename T>
bool QTestExpect<T>::toBeLessThan(T compare)
{
	if(!(*result &= proceed_result(val < compare))) report_error(__func__, val, compare);
	return *result;
}

template<typename T>
bool QTestExpect<T>::toBeLessThanOrEqual(T compare)
{
	if(!(*result &= proceed_result(val <= compare))) report_error(__func__, val, compare);
	return *result;
}

template<typename T>
bool QTestExpect<T>::toBeNull()
{
	if(!(*result &= proceed_result(val == NULL))) report_error(__func__, val);
	return *result;
}

template<typename T>
bool QTestExpect<T>::toBeNullPtr()
{
	if(!(*result &= proceed_result(val == nullptr))) report_error(__func__, val);
	return *result;
}

template<typename T>
template<typename CT>
bool QTestExpect<T>::toBeIterableEqual(CT&& compare)
{
	bool res = true;
	auto it=val.begin();
	auto itc=compare.begin();
	for(;it!=val.end() && itc!=compare.end();++it,++itc){
		res &= (*it) == (*itc);
	}
	res &= it == val.end();
	res &= itc == compare.end();
	if(!(*result &= proceed_result(res))) {
		using iter_value_t = decltype(*std::begin(compare));
		if constexpr (is_streamable<iter_value_t>::value) {
			report_error_resolved(__func__, iterable_to_str(val), iterable_to_str(compare));
		} else {
			report_error(__func__, val, compare);
		}
	}
	return *result;
}

template<typename T>
template<typename CT>
bool QTestExpect<T>::toBeIterableEqual(std::initializer_list<CT> list)
{
	return toBeIterableEqual(std::vector<CT>(list));
}

template<typename T>
bool QTestExpect<T>::toThrowError()
{
	bool res = false;
	try{ val(); } catch(...) { res = true; }
	if(!(*result &= proceed_result(res))) report_error(__func__, val);
	return *result;
}

template<typename T>
template<typename E>
bool QTestExpect<T>::toThrowError()
{
	bool res = false;
	try{ val(); } catch(E) { res = true; }
	if(!(*result &= proceed_result(res))) report_error(__func__, val);
	return *result;
}

template<typename T>
bool QTestExpect<T>::toReturnTrue()
{
	if(!(*result &= proceed_result(val()))) report_error(__func__, val);
	return *result;
}

template<typename T>
template<typename V>
bool QTestExpect<T>::toReturn(V&& compare)
{
	if(!(*result &= proceed_result(val() == compare))) report_error(__func__, val, compare);
	return *result;
}

template<typename T>
bool QTestExpect<T>::fail()
{
	*result = 0;
	if (val.size()) {
		report_error(__func__, val);
	} else {
		report_error<NonStreamable>(__func__, {});
	}
	return *result;
}

template<typename T>
QTestExpect<T> QTestExpect<T>::NOT()
{
	inv = true;
	return *this;
}

template<typename T>
bool QTestExpect<T>::proceed_result(bool result)
{
	if(inv) return !result;
	return result;
}

template<typename T>
void QTestExpect<T>::report_error_resolved(std::string_view func, std::string_view value, std::string_view compare)
{
	error->func = func;
	error->inverse = inv;
	error->has_compare = true;
	error->value_substituted = true;
	error->value = value;
	error->compare_substituted = true;
	error->compare = compare;
}

template<typename T>
template<typename V, typename C>
void QTestExpect<T>::report_error(std::string_view func, V&& value, C&& compare)
{
	error->func = func;
	error->inverse = inv;
	error->has_compare = true;
	if constexpr (is_streamable<V>::value) {
		error->value_substituted = true;
		error->value = streamable_to_str(value);
	}
	if constexpr (is_streamable<C>::value) {
		error->compare_substituted = true;
		error->compare = streamable_to_str(compare);
	}
}

template<typename T>
template<typename V>
void QTestExpect<T>::report_error(std::string_view func, V&& value)
{
	error->func = func;
	error->inverse = inv;
	if constexpr (is_streamable<V>::value) {
		error->value = streamable_to_str(value);
		error->value_substituted = true;
	}
}

template<typename T>
template<typename CT>
std::string QTestExpect<T>::iterable_to_str(CT&& value)
{
	std::stringstream ss;
	std::string value_str = "{";
	for (auto &v : value) {
		ss << v << ", ";
	}
	value_str += std::move(ss).str();
	if (value.size()) {
		value_str.pop_back();
		value_str.pop_back();
	}
	value_str += "}";
	return value_str;
}

template<typename T>
template<typename CT>
std::string QTestExpect<T>::streamable_to_str(CT&& value)
{
	std::stringstream ss;
	ss << value;
	if constexpr (std::is_convertible_v<CT, std::string_view>) {
		return streamable_to_str(std::quoted(std::move(ss).str()));
	} else {
		return std::move(ss).str();
	}
}

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
	else if(good) {
		print_success_sign();
	} else {
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
	for(int i=0;i<blength/2;i++) ret += delim_txt;
	ret += str;
	for(int i=0;i<blength-blength/2;i++) ret += delim_txt;
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
	for(int i=0;i<line_length;i++) print(c);
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

inline QTestBase::QTestBase()
{
	#ifdef TEST_ONLY_RULE
	tests_only = true;
	#endif
	P = std::make_unique<QTestPrint>();
	show_start();
}

inline QTestBase::~QTestBase()
{
	show_statistics();
	if(tests_failed) exit(1);
}

inline void QTestBase::describe(std::string str, describe_function_cb_t fn, int param, std::string_view file)
{
	describes.push_back(std::make_shared<Describe>(str, file, param));
	describes_changed = true;
	fn([this]{ if (current_describe_ran()) call_after_all(current_describe()); });
	describes.pop_back();
	describes_changed = true;
}

inline void QTestBase::it(std::string str, function_cb_t fn, int param, int line)
{
	if (tests_only && param != QTEST_ONLY_PARAM_ID && !in_only_describe()) return;
	if (describes_changed) {
		std::string descr = generate_describes_text(describes);
		P->print_description(descr);
	}
	tests_count++;
	current_test = std::make_shared<Test>(str, line);
	bool is_skip = (param == QTEST_SKIP_PARAM_ID || in_skip_describe());
	if (!is_skip) {
		test_precalls();
		fn();
		test_postcalls();
		current_describe_ran_inc();
	}
	if (is_skip) {
		++tests_skipped;
	}
	if (!current_test->result) {
		++tests_failed;
		if (describes_changed || !failed_tests.size()) {
			failed_tests.push_back({std::vector{current_test}, describes});
		} else {
			failed_tests.back().tests.push_back(current_test);
		}
	}
	show_test_results(*current_test, is_skip);
	describes_changed = false;
}

inline void QTestBase::before(function_cb_t fn)
{
	current_describe().before_alls.push_back(fn);
}

inline void QTestBase::before_each(function_cb_t fn)
{
	current_describe().before_eachs.push_back(fn);
}

inline void QTestBase::after(function_cb_t fn)
{
	current_describe().after_alls.push_back(fn);
}

inline void QTestBase::after_each(function_cb_t fn)
{
	current_describe().after_eachs.push_back(fn);
}

template<typename T>
inline std::basic_ostream<char>& QTestBase::info_print(T&& str)
{
	current_test->info_prints.push_back(std::stringstream{});
	return current_test->info_prints.back() << str;
}

inline std::basic_ostream<char>& QTestBase::info_print()
{
	current_test->info_prints.push_back(std::stringstream{});
	return current_test->info_prints.back();
}

template<typename T>
QTestExpect<T> QTestBase::expect(T&& a, std::string_view s)
{
	current_test->expect_str = s;
	current_test->error = {};
	return QTestExpect<T>(std::move(a), &(current_test->result), &current_test->error);
}

template<typename T>
QTestExpect<T> QTestBase::expect(T& a, std::string_view s)
{
	current_test->expect_str = s;
	current_test->error = {};
	return QTestExpect<T>(a, &(current_test->result), &current_test->error);
}

inline bool QTestBase::current_test_good()
{
	return current_test->result;
}

inline std::string QTestBase::generate_describes_text(std::vector<std::shared_ptr<Describe>>& descrs)
{
	std::string res;
	for (auto d : descrs) {
		res += d->text;
		res.push_back(' ');
	}
	return res;
}

inline bool QTestBase::current_describe_ran()
{
	return current_describe().tests_ran > 0;
}

inline void QTestBase::current_describe_ran_inc()
{
	current_describe().tests_ran++;
}

inline void QTestBase::test_precalls()
{
	for (auto &d : describes) {
		call_before_all(*d);
		call_before_each(*d);
	}
}

inline void QTestBase::test_postcalls()
{
	for (auto it=describes.rbegin();it!=describes.rend();++it) {
		call_after_each(**it);
	}
}

inline bool QTestBase::in_skip_describe()
{
	for (auto &d : describes) if (d->is_skip()) return true;
	return false;
}

inline bool QTestBase::in_only_describe()
{
	for (auto &d : describes) if (d->is_only()) return true;
	return false;
}

inline QTestBase::Describe& QTestBase::current_describe()
{
	return *describes.back();
}

inline void QTestBase::call_before_all(Describe& d)
{
	for (auto fn : d.before_alls) fn();
	d.before_alls.clear();
}

inline void QTestBase::call_after_all(Describe& d)
{
	for (auto fn : d.after_alls) fn();
	d.after_alls.clear();
}

inline void QTestBase::call_before_each(Describe& d)
{
	for (auto fn : d.before_eachs) fn();
}

inline void QTestBase::call_after_each(Describe& d)
{
	for (auto fn : d.after_eachs) fn();
}

inline std::string QTestBase::generate_test_error(std::string_view expect_str, ErrorReport& error)
{
	std::string res;
	if (error.func == "fail") {
		res += "TEST_FAILED("+error.value+") was called!";
		return res;
	}
	error.value = sanitize(error.value);
	error.compare = sanitize(error.compare);
	if (error.value.size() > 20) {
		error.value.resize(20);
		error.value += "...";
	}
	if (error.compare.size() > 20) {
		error.compare.resize(20);
		error.compare += "...";
	}
	std::string value_str = ((error.value_substituted && error.value != expect_str) ? "[="+error.value+"]" : "");
	std::string compare_str = "";
	if (error.has_compare) {
		compare_str = (error.compare_substituted ? error.compare : "...");
	}
	std::string not_str = error.inverse ? ".NOT()" : "";
	res += "EXPECT(" + std::string(expect_str) + value_str + ")" + not_str + "." + std::string(error.func) + "(" + compare_str + ") FAILED!";
	return res;
}

inline void QTestBase::show_failed_tests()
{
	P->print_failure_message();
	std::string res;
	for (auto& [tests, describes] : failed_tests) {
		std::string descr = generate_describes_text(describes);
		P->print_description(descr);
		for (auto t : tests) {
			show_failed_test_results(*t, describes[0]->file);
		}
	}
}

inline void QTestBase::show_start()
{
	P->print_start();
}

inline void QTestBase::show_statistics()
{
	P->print_statistics(tests_count, tests_failed, tests_skipped);
	if(tests_failed){
		show_failed_tests();
	} else {
		show_succeed();
	}
	P->print_delimeter("_");
}

inline void QTestBase::show_test_infos(Test& t)
{
	for (auto &s : t.info_prints) {
		P->print_test_info(s.str());
	}
	if (!t.result) {
		std::string error_text = generate_test_error(t.expect_str, t.error);
		P->print_test_error(error_text);
	}
}

inline void QTestBase::show_test_results(Test& t, bool is_skip)
{
	P->print_test(t.text, t.result, is_skip);
	show_test_infos(t);
}

inline void QTestBase::show_failed_test_results(Test& t, std::string_view file)
{
	P->print_failed_test(t.text, file, t.line);
	show_test_infos(t);
}

inline void QTestBase::show_succeed()
{
	P->print_succeed_message();
}

inline QTestBase BASE;

} // Q_TEST_NS_DETAIL

#endif // QTEST_H
