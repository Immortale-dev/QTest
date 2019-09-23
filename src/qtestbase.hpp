#ifndef QTESTBASE_H
#define QTESTBASE_H


#include <functional>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include "qtestexpect.hpp"
#include "qtestprint.hpp"
#include "qtestutils.hpp"

class QTestBase
{
	struct func;
	struct test;
	struct node;
	
	using function_cb_t = std::function<void()>;
	using string = std::string;
	using func_arr = std::vector<func*>;
	
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
		
		void describe(const char* str, function_cb_t fnn, int param, const char* file);
		void before(function_cb_t fn);
		void before_each(function_cb_t fn);
		void after(function_cb_t fn);
		void after_each(function_cb_t fn);
		void it(const char* str, function_cb_t fn, int param);
		void callback();
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


QTestBase::QTestBase()
{
	reset();
	P = new QTestPrint();
	
	P->print_start();
}

QTestBase::~QTestBase()
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

void QTestBase::callback()
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
}

void QTestBase::describe(const char* str, function_cb_t fnn, int param, const char* file)
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
		// mark all parents with precall = true
		node* tmp = new_node;
		while(tmp && !tmp->precall){
			tmp->precall = true;
			tmp = tmp->parent;
		}
	} 
	
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

void QTestBase::it(const char* str, function_cb_t fn, int param)
{
	test* t = new test(str, fn);
	t->only = param == QTEST_ONLY_PARAM_ID;
	t->skip = param == QTEST_SKIP_PARAM_ID;
	
	if(t->only){
		// mark all parents with precall = true
		node* tmp = current;
		while(tmp && !tmp->precall){
			tmp->precall = true;
			tmp = tmp->parent;
		}
	}
	
	current->tests.push_back(t);
}

template<typename T>
QTestExpect<T> QTestBase::expect(T a)
{
	return QTestExpect<T>(a, &(current_test->result) );
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
	tests_skipped = 0;
	
	#ifdef TEST_ONLY_RULE
	tests_only = true;
	#endif
	
	tree = new node();
	current = tree;
}

QTestBase::string QTestBase::generate_description(node* n)
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

void QTestBase::prepare_node(node* n)
{
	if(n->parent && n->parent->skip){
		// inherit skip from parent
		n->skip = true;
	}
	
	if(n->parent && n->parent->only){
		// inherit only from parent
		n->only = true;
	}
	
	if(n->only){
		// mark all tests as only
		for(auto it : n->tests){
			it->only = true;
		}
	}
	
	if(n->skip){
		// mark all tests as skip
		for(auto it : n->tests){
			it->skip = true;
		}
	}
}

QTestBase::func_arr QTestBase::get_before_each(node* n)
{
	func_arr beforeEach;
	node* temp = n;
	while(temp){
		for(int i=temp->before_each.size()-1;i>=0;i--)
			beforeEach.push_back(temp->before_each[i]);
		temp = temp->parent;
	}
	std::reverse(beforeEach.begin(), beforeEach.end());
	return beforeEach;
}

QTestBase::func_arr QTestBase::get_after_each(node* n)
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

QTestBase::func_arr QTestBase::get_before_all(node* n)
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

QTestBase::func_arr QTestBase::get_after_all(node* n)
{
	func_arr afterAll(n->after_all.begin(), n->after_all.end());
	n->after_all.resize(0);
	return afterAll;
}

void QTestBase::run_funcs(func_arr& funcs)
{
	for(auto it : funcs)
		it->fn();
}

void QTestBase::call_before_all(node* n)
{
	func_arr tests = get_before_all(n);
	run_funcs(tests);
}

void QTestBase::call_after_all(node* n)
{
	func_arr tests = get_after_all(n);
	run_funcs(tests);
}

void QTestBase::call_before_each(node* n)
{
	func_arr tests = get_before_each(n);
	run_funcs(tests);
}

void QTestBase::call_after_each(node* n)
{
	func_arr tests = get_after_each(n);
	run_funcs(tests);
}

void QTestBase::run_tests(node* n)
{
	string descr = generate_description(n);
	
	// inherit parent rules
	prepare_node(n);
	
	bool have_callable_tests = false;
	bool precall_tests = (!tests_only || n->precall || n->only);
	
	// check for any test to be executed in scope of this node
	for(auto it : n->tests){
		if(it->only)
			have_callable_tests = true;
	}
	
	if(n->tests.size()){
		if(!tests_only || have_callable_tests){
			P->print_description(descr);
			if(!n->skip){
				// if we found at least one test that should
				// be executed while moving through the node tree
				// run and remove all before_all tests from root
				// to current node
				call_before_all(n);
			}
		}
		
		for(auto it : n->tests){
			current_test = it;
			
			// Check for current test to be called
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

			// Print test
			P->print_test(current_test->descr, current_test->result, current_test->skip);
			
			if(!current_test->skip){
				call_after_each(n);
			}
		}
		
		if(!n->skip && precall_tests){
			// if at least one test from tree path was called
			// call current node after_all functions as we are 
			// going to leave this node
			call_after_all(n);
		}
	}
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
