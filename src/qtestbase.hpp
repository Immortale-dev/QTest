#ifndef QTESTBASE_H
#define QTESTBASE_H


#include <functional>
#include <vector>
#include <string>
#include "qtestexpect.hpp"
#include "qtestprint.hpp"

template<typename T>
class QTestExpect;
class QTestPrint;

class QTestBase
{
	using function_cb_t = std::function<void()>;
	using string = std::string;
	
	public:
		QTestBase();
		~QTestBase();
		void describe(const char* str, function_cb_t fn, const char* file );
		void before(function_cb_t fn);
		void before_each(function_cb_t fn);
		void after(function_cb_t fn);
		void after_each(function_cb_t fn);
		void it(const char* str, function_cb_t fn);
		template<typename T>
		QTestExpect<T> expect(T a);
		
		struct test
		{
			string descr;
			function_cb_t fn;
			bool result = true;
			test(string str, function_cb_t fn) : descr(str), fn(fn) {};
		};
		
	private:
		struct func
		{
			function_cb_t fn;
			func(function_cb_t fn) : fn(fn) {};
		};
		struct node
		{
			string descr;
			string file;
			std::vector<node*> childs;
			std::vector<test*> tests;
			std::vector<func*> before_all, after_all, before_each, after_each;
		};
		
		node* tree = nullptr;
		node* current = nullptr;
		test* current_test = nullptr;
		
		QTestPrint* P;
		
		int tests_count = 0;
		int tests_failed = 0;
		
		void run_tests();
		void run_tests(node* n, string& str, std::vector<func*>& beforeEach, std::vector<func*>& afterEach);
		void show_failed_tests();
		void show_failed_tests(node* n, string& str);
		void show_succeed();
		void release_tree();
		void release_tree(node* n);
		void reset();
};



QTestBase::QTestBase()
{
	reset();
	P = new QTestPrint();
}
QTestBase::~QTestBase()
{
	P->print_start();
	run_tests();
	P->print_statistics(tests_count, tests_failed);
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

void QTestBase::describe(const char* str, function_cb_t fn, const char* file)
{
	node* old_node = current;
	node* new_node = new node();
	
	new_node->descr = str;
	new_node->file = file;
	current->childs.push_back(new_node);
	current = new_node;
	
	fn();
	
	current = old_node;
}

void QTestBase::before(function_cb_t fn)
{
	current->before_all.push_back(new func(fn));
}

void QTestBase::before_each(function_cb_t fn)
{
	current->before_each.push_back(new func(fn));
}

void QTestBase::after(function_cb_t fn)
{
	current->after_all.push_back(new func(fn));
}

void QTestBase::after_each(function_cb_t fn)
{
	current->after_each.push_back(new func(fn));
}

void QTestBase::it(const char* str, function_cb_t fn)
{
	current->tests.push_back(new test(str, fn));
}

template<typename T>
QTestExpect<T> QTestBase::expect(T a)
{
	return QTestExpect<T>(a, current_test->result);
}

void QTestBase::release_tree()
{
	release_tree(tree);
	delete tree;
	tree = nullptr;
}

void QTestBase::release_tree(node* n)
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

void QTestBase::reset()
{
	release_tree();
	tests_failed = 0;
	tests_count = 0;
	tree = new node();
	current = tree;
}

void QTestBase::run_tests()
{
	std::vector<func*> be, ae;
	string str = "";
	run_tests(tree, str, be, ae);
}

void QTestBase::run_tests(node* n, string& str, std::vector<func*>& beforeEach, std::vector<func*>& afterEach)
{
	// Save values for backtrack work
	int descr_length = str.size();
	int before_each_length = beforeEach.size();
	int after_each_length = afterEach.size();
	
	// Prepare test message
	str += n->descr + " ";
	
	// Call for all new "before all" functions
	for(auto it : n->before_all)
		it->fn();
	
	// Add new functions that should be called on each tests during the scope
	for(auto it : n->before_each)
		beforeEach.push_back(it);
	for(auto it : n->after_each)
		afterEach.push_back(it);
		
	if(n->tests.size()){
		// Print generated tree path description
		P->print_description(str);
		
		// Run all of the current node tests
		for(auto it : n->tests){
			
			// Run all beforeEach functions
			for(auto f : beforeEach)
				f->fn();
				
			tests_count++;
			current_test = it;
			current_test->fn();
			if(!current_test->result)
				tests_failed++;
			P->print_test(current_test->descr, current_test->result);
			
			// Run all afterEach functions
			for(auto f : afterEach)
				f->fn();
		}
	}
	
	// Recoursively run tests for all childsren nodes
	for(auto it : n->childs){
		run_tests(it, str, beforeEach, afterEach);
	}
	
	// Call for all new "after all" functions
	for(auto it : n->after_all)
		it->fn();
		
	// Backtrack
	str.resize(descr_length);
	beforeEach.resize(before_each_length);
	beforeEach.resize(after_each_length);
}

void QTestBase::show_failed_tests()
{
	P->print_failure_message();
	
	string str = "";
	show_failed_tests(tree, str);
}

void QTestBase::show_failed_tests(node* n, string& str)
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

void QTestBase::show_succeed()
{
	P->print_succeed_message();
}

#endif //QTESTBASE_H
