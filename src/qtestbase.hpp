#ifndef QTESTBASE_H
#define QTESTBASE_H

#include <functional>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <memory>
#include <string_view>

#include "qtestexpect.hpp"
#include "qtestprint.hpp"
#include "qtestutils.hpp"

namespace Q_TEST_NS_DETAIL {

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

class QTestBase
{
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
	if(tests_failed) {
		exit(1);
	}
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
	// Don't call if the TEST_ONLY mode is on and only param is not set
	if (tests_only && param != QTEST_ONLY_PARAM_ID && !in_only_describe()) return;

	// Print test describes
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

	// Print test
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
	for (auto &d : describes) {
		if (d->is_skip()) return true;
	}
	return false;
}

inline bool QTestBase::in_only_describe()
{
	for (auto &d : describes) {
		if (d->is_only()) return true;
	}
	return false;
}

inline QTestBase::Describe& QTestBase::current_describe()
{
	return *describes.back();
}

inline void QTestBase::call_before_all(Describe& d)
{
	for (auto fn : d.before_alls) {
		fn();
	}
	d.before_alls.clear();
}

inline void QTestBase::call_after_all(Describe& d)
{
	for (auto fn : d.after_alls) {
		fn();
	}
	d.after_alls.clear();
}

inline void QTestBase::call_before_each(Describe& d)
{
	for (auto fn : d.before_eachs) {
		fn();
	}
}

inline void QTestBase::call_after_each(Describe& d)
{
	for (auto fn : d.after_eachs) {
		fn();
	}
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
	} else{
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

} // Q_TEST_NS_DETAIL

#endif //QTESTBASE_H
