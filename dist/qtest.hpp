#ifndef QTEST_H
#define QTEST_H

#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <initializer_list>

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
#define Q_TEST__UNIQ_NAME_GENERATE(a) Q_TEST__CONCAT( Q_TEST__CONCAT(a, __LINE__) , __COUNTER__)
#define Q_TEST__UNIQ_NAME() Q_TEST__UNIQ_NAME_GENERATE(___Q___TEST___U_N_I_Q___)
#define Q_TEST__DESCRIBE_STRUCT(...) QTestDescribe_t Q_TEST__UNIQ_NAME() (__VA_ARGS__)
#define Q_TEST__CALLBACK() QTestFinally<function_cb_t> Q_TEST__UNIQ_NAME() ([](){ QTESTOBJECTDEFINED.callback(); })
#define Q_TEST__CALLBACK_ARG() QTestFinally<function_cb_t> ([](){ QTESTOBJECTDEFINED.callback(); })
#define Q_TEST__DESCRIBE(...) Q_TEST__DESCRIBE_STRUCT(__VA_ARGS__); Q_TEST__CALLBACK()
#define Q_TEST__LAMBDA(...) [Q_TEST__SCOPE]()__VA_ARGS__
#define Q_TEST__LAMBDA_CALLBACK(...) Q_TEST__LAMBDA({ do __VA_ARGS__ while( (QTESTOBJECTDEFINED.callback(), false) ); })

#define DESCRIBE(a, ...) Q_TEST__DESCRIBE(a, Q_TEST__LAMBDA_CALLBACK(__VA_ARGS__), QTEST_TEST_PARAM_ID, __FILE__)
#define DESCRIBE_ONLY(a, ...) Q_TEST__DESCRIBE(a, Q_TEST__LAMBDA_CALLBACK(__VA_ARGS__), QTEST_ONLY_PARAM_ID, __FILE__)
#define DESCRIBE_SKIP(a, ...) Q_TEST__DESCRIBE(a, Q_TEST__LAMBDA_CALLBACK(__VA_ARGS__), QTEST_SKIP_PARAM_ID, __FILE__)
#define BEFORE_ALL(...) QTESTOBJECTDEFINED.before(Q_TEST__LAMBDA(__VA_ARGS__))
#define BEFORE_EACH(...) QTESTOBJECTDEFINED.before_each(Q_TEST__LAMBDA(__VA_ARGS__))
#define AFTER_ALL(...) QTESTOBJECTDEFINED.after(Q_TEST__LAMBDA(__VA_ARGS__))
#define AFTER_EACH(...) QTESTOBJECTDEFINED.after_each(Q_TEST__LAMBDA(__VA_ARGS__))
#define IT(a, ...) QTESTOBJECTDEFINED.it(a, Q_TEST__LAMBDA(__VA_ARGS__), QTEST_TEST_PARAM_ID); Q_TEST__CALLBACK()
#define IT_ONLY(a, ...) QTESTOBJECTDEFINED.it(a, Q_TEST__LAMBDA(__VA_ARGS__), QTEST_ONLY_PARAM_ID); Q_TEST__CALLBACK()
#define IT_SKIP(a, ...) QTESTOBJECTDEFINED.it(a, Q_TEST__LAMBDA(__VA_ARGS__), QTEST_SKIP_PARAM_ID); Q_TEST__CALLBACK()
#define EXPECT(a) QTESTOBJECTDEFINED.expect((a))
#define INFO_PRINT(a) QTESTOBJECTDEFINED.info_print(a)
#define TEST_FAILED() EXPECT(1).toBe(0)
#define TEST_SUCCEED() EXPECT(1).toBe(1)

template<typename T>
class QTestFinally
{
	T fn;
	public:
		QTestFinally(T fn) : fn(fn){};
		~QTestFinally(){ fn(); };
};

// QTextExpect
template<typename T>
class QTestExpect
{
	public:
		QTestExpect(T a, bool* result) : val(a), result(result) {};
		void toBe(T compare);
		void toBeCloseTo(double compare, double precision);
		void toBeGreaterThan(T compare);
		void toBeGreaterThanOrEqual(T compare);
		void toBeLessThan(T compare);
		void toBeLessThanOrEqual(T compare);
		void toBeNull();
		void toBeNullPtr();
		template<typename CT>
		void toBeIterableEqual(CT compare);
		template<typename CT>
		void toBeIterableEqual(std::initializer_list<CT> list);
		void toThrowError();
		void toReturnTrue();
		QTestExpect<T> NOT();
		void to_be(T compare){ return toBe(compare); };
		void to_be_close_to( double compare, double precision){ return toBeCloseTo(compare, precision); };
		void to_be_greater_than(T compare){ return toBeGreaterThan(compare); };
		void to_be_greater_than_or_equal(T compare){ return toBeGreaterThanOrEqual(compare); };
		void to_be_less_than(T compare){ return toBeLessThan(compare); };
		void to_be_less_than_or_equal(T compare){ return toBeLessThanOrEqual(compare); };
		void to_be_null(){ return toBeNull(); };
		void to_be_null_ptr(){ return toBeNullPtr(); };
		template<typename CT>
		void to_be_iterable_equal(CT compare){ return toBeIterableEqual(compare); };
		template<typename CT>
		void to_be_iterable_equal(std::initializer_list<CT> list){ return toBeIterableEqual(list); };
		void to_throw_error(){ return toThrowError(); };
		void to_return_true(){ return toReturnTrue(); };
		
	private:
		T val;
		bool* result;
		bool inv = false;
		bool proceed_result(bool result);
};


template<typename T>
void QTestExpect<T>::toBe(T compare)
{
	(*result) &= proceed_result(val == compare);
}

template<typename T>
void QTestExpect<T>::toBeCloseTo(double compare, double precision)
{
	double v = (double)val;
	(*result) &= proceed_result(std::abs(v-compare) <= std::abs(precision));
}

template<typename T>
void QTestExpect<T>::toBeGreaterThan(T compare)
{
	(*result) &= proceed_result(val > compare);
}

template<typename T>
void QTestExpect<T>::toBeGreaterThanOrEqual(T compare)
{
	(*result) &= proceed_result(val >= compare);
}

template<typename T>
void QTestExpect<T>::toBeLessThan(T compare)
{
	(*result) &= proceed_result(val < compare);
}

template<typename T>
void QTestExpect<T>::toBeLessThanOrEqual(T compare)
{
	(*result) &= proceed_result(val <= compare);
}

template<typename T>
void QTestExpect<T>::toBeNull()
{
	(*result) &= proceed_result(val == NULL);
}

template<typename T>
void QTestExpect<T>::toBeNullPtr()
{
	(*result) &= proceed_result(val == nullptr);
}

template<typename T>
template<typename CT>
void QTestExpect<T>::toBeIterableEqual(CT compare)
{
	bool res = true;
	auto it=val.begin();
	auto itc=compare.begin();
	for(;it!=val.end() && itc!=compare.end();++it,++itc){
		res &= (*it) == (*itc);
	}
	res &= it == val.end();
	res &= itc == compare.end();
	(*result) = proceed_result(res);
}

template<typename T>
template<typename CT>
void QTestExpect<T>::toBeIterableEqual(std::initializer_list<CT> list)
{
	toBeIterableEqual(std::vector<CT>(list));
}

template<typename T>
void QTestExpect<T>::toThrowError()
{
	bool res = false;
	try{
		val();
	}catch(...){
		res = true;
	}
	(*result) &= proceed_result(res);
}

template<typename T>
void QTestExpect<T>::toReturnTrue()
{
	proceed_result(val());
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
	if(inv) 
		return !result;
	return result;
}

// QTestPrint
class QTestPrint
{
	using string = std::string;
	using test_infos = std::vector<string>;
	public:
		QTestPrint();
		void print_description(string& str);
		void print_description_extended(string& str, string& file);
		void print_test(string& str, bool good, bool skipped);
		void print_test_info(test_infos& arr);
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

inline QTestPrint::QTestPrint()
{
	processConsoleWindow();
}

inline void QTestPrint::print_description(string& str)
{
	print(newline);
	print(" "+str+newline);
}

inline void QTestPrint::print_description_extended(string& str, string& file)
{
	print(newline);
	print(" "+str);
	print_neutral("("+file+")");
	print(newline);
}

inline void QTestPrint::print_test(string& str, bool good, bool skipped)
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

inline void QTestPrint::print_test_info(test_infos& arr)
{
	for(auto& it : arr){
		print("        ");
		print(" - ");
		print_grey(it);
		print(newline);
	}
}

inline void QTestPrint::print_falied_test(string& str)
{
	print(tab);
	print_error_sign();
	print(" ");
	print_grey(str);
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

inline void QTestPrint::print_start()
{
	print_delimeter("_");
	print_title(toupper(testing_txt));
}

inline std::string QTestPrint::create_titled_message(string str)
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

inline void QTestPrint::print_title(string str)
{
	print(create_titled_message(str));
}

inline void QTestPrint::print_delimeter()
{
	print_delimeter(delim_txt);
}

inline void QTestPrint::print_delimeter(string c)
{
	for(int i=0;i<line_length;i++)
		print(c);
}

inline void QTestPrint::print(string s)
{
	std::cout << s;
}

inline void QTestPrint::print_error(string s)
{
	set_color_error();
	print(s);
	set_color_default();
}

inline void QTestPrint::print_success(string s)
{
	set_color_success();
	print(s);
	set_color_default();
}

inline void QTestPrint::print_neutral(string s)
{
	set_color_neutral();
	print(s);
	set_color_default();
}

inline void QTestPrint::print_grey(string s)
{
	set_color_grey();
	print(s);
	set_color_default();
}

inline std::string QTestPrint::toupper(string txt)
{
	string txtc(txt);
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

inline void QTestPrint::print_succeed_message()
{
	print_success(create_titled_message(toupper(succeed_txt)));
}

inline void QTestPrint::print_failure_message()
{
	print_error(create_titled_message(toupper(failed_txt)));
}

// QTestBase
class QTestBase
{
	struct func;
	struct test;
	struct node;
	using function_cb_t = std::function<void()>;
	using string = std::string;
	using func_arr = std::vector<func*>;
	using info_prints_t = std::vector<string>;
	struct func
	{
		function_cb_t fn;
		func(function_cb_t fn) : fn(fn) {};
	};
	struct test
	{
		string descr;
		function_cb_t fn;
		bool result = true;
		bool only;
		bool skip;
		info_prints_t info_prints;
		test(string str, function_cb_t fn) : descr(str), fn(fn) {};
	};
	struct node
	{
		function_cb_t fn;
		string descr;
		string file;
		bool only;
		bool skip;
		bool called = false;
		bool precall = false;
		node* parent = nullptr;
		std::vector<node*> childs;
		std::vector<test*> tests;
		std::vector<func*> before_all, after_all, before_each, after_each;
	};
	
	public:
		QTestBase();
		~QTestBase();
		void describe(string str, function_cb_t fnn, int param, const char* file);
		void before(function_cb_t fn);
		void before_each(function_cb_t fn);
		void after(function_cb_t fn);
		void after_each(function_cb_t fn);
		void it(string str, function_cb_t fn, int param);
		void callback();
		void info_print(string str);
		template<typename T>
		QTestExpect<T> expect(T a);
		
	private:
		node* tree = nullptr;
		node* current = nullptr;
		test* current_test = nullptr;
		QTestPrint* P;
		int tests_count = 0;
		int tests_failed = 0;
		int tests_skipped = 0;
		bool tests_only = false;
		string generate_description(node* n);
		func_arr get_before_each(node* n);
		func_arr get_after_each(node* n);
		func_arr get_before_all(node* n);
		func_arr get_after_all(node* n);
		void run_funcs(func_arr& funcs);
		void call_before_all(node* n);
		void call_after_all(node* n);
		void call_before_each(node* n);
		void call_after_each(node* n);
		void prepare_node(node* n);
		void run_tests(node *n);
		void show_failed_tests();
		void show_failed_tests(node* n, string& str);
		void show_succeed();
		void release_tree();
		void release_tree(node* n);
		void reset();
};


inline QTestBase::QTestBase()
{
	reset();
	P = new QTestPrint();
	P->print_start();
}

inline QTestBase::~QTestBase()
{
	P->print_statistics(tests_count, tests_failed, tests_skipped);
	if(tests_failed){
		show_failed_tests();
	}
	else{
		show_succeed();
	}
	P->print_delimeter("_");
	release_tree();
	delete P;
	if(tests_failed){
		exit(1);
	}
}

inline void QTestBase::callback()
{
	if(current->called)
		return;
	node* n = current;
	run_tests(n);
	n->called = true;
	for(auto it : n->childs){
		current = it;
		current->fn();
	}
	bool precall_tests = (!tests_only || n->precall || n->only);
	if(!n->skip && precall_tests){
		call_after_all(n);
	}
}

inline void QTestBase::describe(string str, function_cb_t fnn, int param, const char* file)
{
	node* new_node = new node();
	new_node->descr = str;
	new_node->only = param == QTEST_ONLY_PARAM_ID;
	new_node->skip = param == QTEST_SKIP_PARAM_ID;
	new_node->parent = current;
	new_node->fn = fnn;
	new_node->file = file;
	current->childs.push_back(new_node);
	if(new_node->only){
		node* tmp = new_node;
		while(tmp && !tmp->precall){
			tmp->precall = true;
			tmp = tmp->parent;
		}
	}
}

inline void QTestBase::before(function_cb_t fn)
{
	current->before_all.push_back(new func(fn));
}

inline void QTestBase::before_each(function_cb_t fn)
{
	current->before_each.push_back(new func(fn));
}

inline void QTestBase::after(function_cb_t fn)
{
	current->after_all.push_back(new func(fn));
}

inline void QTestBase::after_each(function_cb_t fn)
{
	current->after_each.push_back(new func(fn));
}

inline void QTestBase::it(string str, function_cb_t fn, int param)
{
	test* t = new test(str, fn);
	t->only = param == QTEST_ONLY_PARAM_ID;
	t->skip = param == QTEST_SKIP_PARAM_ID;
	if(t->only){
		node* tmp = current;
		while(tmp && !tmp->precall){
			tmp->precall = true;
			tmp = tmp->parent;
		}
	}
	current->tests.push_back(t);
}

inline void QTestBase::info_print(string str)
{
	current_test->info_prints.push_back(str);
}

template<typename T>
QTestExpect<T> QTestBase::expect(T a)
{
	return QTestExpect<T>(a, &(current_test->result) );
}

inline void QTestBase::release_tree()
{
	release_tree(tree);
	delete tree;
	tree = nullptr;
}

inline void QTestBase::release_tree(node* n)
{
	if(!n)
		return;
	for(auto it : n->tests)
		delete it;
	for(auto it : n->before_all)
		delete it;
	for(auto it : n->after_all)
		delete it;
	for(auto it : n->before_each)
		delete it;
	for(auto it : n->after_each)
		delete it;
	for(auto it : n->childs){
		release_tree(it);
		delete it;
	}
}

inline void QTestBase::reset()
{
	release_tree();
	tests_failed = 0;
	tests_count = 0;
	tests_skipped = 0;
	#ifdef TEST_ONLY_RULE
	tests_only = true;
	#endif
	tree = new node();
	current = tree;
}

inline QTestBase::string QTestBase::generate_description(node* n)
{
	node *temp;
	std::vector<string> descr_arr;
	temp = n;
	while(temp){
		descr_arr.push_back(temp->descr);
		temp = temp->parent;
	}
	string descr = "";
	std::reverse(descr_arr.begin(), descr_arr.end());
	for(auto &it : descr_arr)
		descr += it + " ";
	return descr;
}

inline void QTestBase::prepare_node(node* n)
{
	if(n->parent && n->parent->skip){
		n->skip = true;
	}
	if(n->parent && n->parent->only){
		n->only = true;
	}
	if(n->only){
		for(auto it : n->tests){
			it->only = true;
		}
	}
	if(n->skip){
		for(auto it : n->tests){
			it->skip = true;
		}
	}
}

inline QTestBase::func_arr QTestBase::get_before_each(node* n)
{
	func_arr beforeEach;
	node* temp = n;
	while(temp){
		for(int i=temp->before_each.size()-1;i>=0;i--)
			beforeEach.push_back(temp->before_each[i]);
		for(int i=temp->before_all.size()-1;i>=0;i--)
			beforeEach.push_back(temp->before_all[i]);
		temp->before_all.resize(0);
		temp = temp->parent;
	}
	std::reverse(beforeEach.begin(), beforeEach.end());
	return beforeEach;
}

inline QTestBase::func_arr QTestBase::get_after_each(node* n)
{
	func_arr afterEach;
	node* temp = n;
	while(temp){
		for(int i=temp->after_each.size()-1;i>=0;i--)
			afterEach.push_back(temp->after_each[i]);
		temp = temp->parent;
	}
	std::reverse(afterEach.begin(), afterEach.end());
	return afterEach;
}

inline QTestBase::func_arr QTestBase::get_before_all(node* n)
{
	func_arr beforeAll;
	node* temp = n;
	while(temp){
		for(int i=temp->before_all.size()-1;i>=0;i--)
			beforeAll.push_back(temp->before_all[i]);
		temp->before_all.resize(0);
		temp = temp->parent;
	}
	std::reverse(beforeAll.begin(), beforeAll.end());
	return beforeAll;
}

inline QTestBase::func_arr QTestBase::get_after_all(node* n)
{
	func_arr afterAll(n->after_all.begin(), n->after_all.end());
	n->after_all.resize(0);
	return afterAll;
}

inline void QTestBase::run_funcs(func_arr& funcs)
{
	for(auto it : funcs)
		it->fn();
}

inline void QTestBase::call_before_all(node* n)
{
	func_arr tests = get_before_all(n);
	run_funcs(tests);
}

inline void QTestBase::call_after_all(node* n)
{
	func_arr tests = get_after_all(n);
	run_funcs(tests);
}

inline void QTestBase::call_before_each(node* n)
{
	func_arr tests = get_before_each(n);
	run_funcs(tests);
}

inline void QTestBase::call_after_each(node* n)
{
	func_arr tests = get_after_each(n);
	run_funcs(tests);
}

inline void QTestBase::run_tests(node* n)
{
	string descr = generate_description(n);
	prepare_node(n);
	bool have_callable_tests = false;
	for(auto it : n->tests){
		if(it->only)
			have_callable_tests = true;
	}
	if(n->tests.size()){
		if(!tests_only || have_callable_tests){
			P->print_description(descr);
		}
		for(auto it : n->tests){
			current_test = it;
			if(tests_only && !current_test->only)
				continue;
			if(!current_test->skip){
				call_before_each(n);
			}
			tests_count++;
			if(current_test->skip)
				tests_skipped++;
			if(!current_test->skip)
				current_test->fn();
			if(!current_test->result)
				tests_failed++;
			P->print_test(current_test->descr, current_test->result, current_test->skip);
			P->print_test_info(current_test->info_prints);
			if(!current_test->skip){
				call_after_each(n);
			}
		}
	}
}

inline void QTestBase::show_failed_tests()
{
	P->print_failure_message();
	string str = "";
	show_failed_tests(tree, str);
}

inline void QTestBase::show_failed_tests(node* n, string& str)
{
	int str_length = str.size();
	str += n->descr+" ";
	bool hasFailed = false;
	for(auto it : n->tests){
		if(!it->result)
			hasFailed = true;
	}
	if(hasFailed){
		P->print_description_extended(str, n->file);
	}
	for(auto it : n->tests){
		if(!it->result)
			P->print_falied_test(it->descr);
	}
	for(auto it : n->childs){
		show_failed_tests(it, str);
	}
	str.resize(str_length);
}

inline void QTestBase::show_succeed()
{
	P->print_succeed_message();
}

namespace{
	QTestBase QTESTOBJECTDEFINED = QTestBase();
	using function_cb_t = std::function<void()>;
	struct QTestDescribe_t
	{
		QTestDescribe_t(std::string str, function_cb_t fn, int param, const char* file){ QTESTOBJECTDEFINED.describe(str, fn, param, file); };
	};
	Q_TEST__CALLBACK();
}
#endif // QTEST_H
