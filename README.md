![QTest logo](https://raw.githubusercontent.com/webdeveloperukraine/webdeveloperukraine.github.io/master/_resources/qtest/qtest_logo.png)

## What the Freak ?

QTest is a **C++** Behavior Driven Development testing framework. 
There is a lot of good test frameworks like google-test or catch2, and if you are looking for the great, popular test frameworks with huge community, you should probably try them first, but if you are looking for the **lightweight** test framework that looks so familiar and simple, or you would like to contribute and modify some test framework for your specific project or environment, and you don't know what to start from, or you don't want to use huge test frameworks for your project, you might found **QTest** as a good thing for your needs.
Just take a look for the syntax and some examples, and you may fell in love with it :)

## Table of Contest

 * [WTF?](#what-the-freak-)
 * [Table of Contest](#table-of-contest)
 * [Build](#build)
 * [Dist](#dist)
 * [Docs](#docs)
	 * [Usage](#usage)
	 * [Syntax](#syntax)
	 * [Code scope](#code-scope)
	 * [Macros](#macros)
		 * [SCENARIO_START](#scenario_start)
		 * [SCENARIO_END](#scenario_end)
		 * [DESCRIBE (string description, {})](#describe-string-description-)
		 * [DESCRIBE_SKIP (string description, {})](#describe_skip-string-description-)
		 * [DESCRIBE_ONLY (string description, {})](#describe_only-string-description-)
		 * [BEFORE_ALL ({})](#before_all-)
		 * [AFTER_ALL ({})](#after_all-)
		 * [BEFORE_EACH ({})](#before_each-)
		 * [AFTER_EACH ({})](#after_each-)
		 * [IT (string description, {})](#it-string-description-)
		 * [IT_SKIP (string description, {})](#it_skip-string-description-)
		 * [IT_ONLY (string description, {})](#it_only-string-description-)
		 * [EXPECT (T value)](#expect-t-value)
		 * [TEST_SUCCEED ()](#test_succeed-)
		 * [TEST_FAILED ([string reason])](#test_failed-string-reason)
		 * [INFO_PRINT (T value)](#info_print-t-value)
	 * [QTestExpect (T&& actual)](#qtestexpect-t-actual)
		 * [toBe (C&& compare)](#tobe-c-compare)
		 * [toBeCloseTo (T compare, T precision)](#tobecloseto-t-compare-t-precision)
		 * [toBeGreaterThan (T compare)](#tobegreaterthan-t-compare)
		 * [toBeGreaterThanOrEqual (T compare)](#tobegreaterthanorequal-t-compare)
		 * [toBeLessThan (T compare)](#tobelessthan-t-compare)
		 * [toBeLessThanOrEqual (T compare)](#tobelessthanorequal-t-compare)
		 * [toBeNull ()](#tobenull-)
		 * [toBeNullPtr ()](#tobenullptr-)
		 * [toBeIterableEqual (CT&& compare)](#tobeiterableequal-ct-compare)
		 * [toThrowError ()](#tothrowerror-)
		 * [toThrowError<E> ()](#tothrowerrore-)
		 * [toReturnTrue ()](#toreturntrue-)
		 * [toReturn (C&& compare)](#toreturn-c-compare)
		 * [NOT ()](#not-)
 * [Tips](#tips)
	 * [Garbage test result](#garbage-test-result)
	 * [Cycles and conditions](#cycles-and-conditions)
	 * [Expect object aliases](#expect-object-aliases)
	 * [“ONLY” rule](#only-rule)
	 * [“SCENARIO” macroses](#scenario-macroses)
	 * [Test ordering](#test-ordering)
	 * [Failed EXPECT results](#failed-expect-results)
	 * [V1 -> V2 changes](#v1---v2-changes)
 * [More](#more)
 * [License](#license)

## Build

You can use QTest for both Windows and Linux platforms. **C++17** or later version of c++ required.
You can find **make** file in the root of repository used to build the test program. Also, there is **test/test.cpp** file that shows some useful examples of how to use this test framework.
If you want quickly build and run the example tests, you can run `make test`, and then run tests using `./test.exe`.
To clean the artifacts you can use `make clean` command.

## Dist

Additionally we've generated single **.hpp** file you can use to include full testing framework functionality. 
You can find this file placed to **/dist** folder. It is lightweight - less than **28kb** totally. 


## Docs 

### Usage

QTest is a behaviour driven C++ test framework. To start working with it, you need to include **qtest.hpp** file to your test project. You can download single **qtest.hpp** file from **/dist** folder of this repository, or use it separated.

After **.hpp** file included, you can start writing tests using several predefined macros explained in the **Macros** section of documentation.

We will move through all of them below.

***Example:***
```c++
#include "qtest.hpp"

SCENARIO_START

DESCRIBE("[Test]", {
	int a,b,c;
	BEFORE_EACH({
		a = 1;
		b = 2;
		c = 3;
	});
	DESCRIBE("Ariphmetic Operations", {
		IT("sum of `a` and `b` should be equal to `c`", {
			EXPECT(a+b).toBe(c);
		});
	});
});

SCENARIO_END

int main(){return 0;}
```

Notice, we don't need to put our tests to `main()` or any else function. Because of that, it is allowed to split your tests to different files, there is even no need to include them as long as they are linked to the executable.

Notice, you must put every your `DESCRIBE` macro between exactly one `SCENARIO_START` / `SCENARIO_END`. It is not required to have exactly one scenario in a file, you could have multiple.

***Example:***
```c++
// ariphmethics.test.cpp
#include "qtest.hpp"

SCENARIO_START

DESCRIBE("ariphmethics", {
	IT("should be 10", {
		EXPECT(5+5).toBe(10);
	});

	IT("should be less than 15", {
		EXPECT(3*3).toBeLessThan(15);
	});
});

SCENARIO_END
```

```c++
// my_program_code.test.cpp
#include "qtest.hpp"

SCENARIO_START

DESCRIBE("program", {
	BEFORE_EACH({ ... });
	AFTER_EACH({ ... });

	DESCRIBE("one part", {
		IT("should be fine", {
			EXPECT(...).toBe(...);
		});

		IT("should be something else", {
			EXPECT(...).toBeLessThan(...);
		});
	});
});

SCENARIO_END
```

```c++
// test.cpp
int main(){return 0;}
```

When all files compiled and linked together, the program will execute and show the test results.
____

### Syntax

It is important to follow the syntax rules of building test cases: 
- All your `EXPECT` macros *must* be called directly from `IT` macro wrapper.
- And all `IT` macros *must* be called directly from `DESCRIBE` macro wrapper.
- All `BEFORE_ALL`, `BEFORE_EACH`, `AFTER_ALL` and `AFTER_EACH` macros *must* be called directly from `DESCRIBE` macro wrapper.
- `DESCRIBE` macros *must* be called from another `DESCRIBE` macro wrappers, or as independent from "test tree" call. 
- All `DESCRIBE` macros should be placed between `SCENARIO_START` and `SCENARIO_END` macros.

Not following those rules may cause **undefined** behavior.

***Correct examples:***
```c++
SCENARIO_START
...
DESCRIBE("...", {
	int a=3,b,c;
	BEFORE_EACH({ ... });
	AFTER_EACH({ ... });
	IT("...", {
		int a,x=10,z=5;
		EXPECT(5+5).toBe(10);
		EXPECT(11/2).toBe(5);
	});
	IT("...", {
		EXPECT(...).[...](...);
	});
	IT("...", {
		EXPECT(...).[...](...);
	});
	
	DESCRIBE("...", {
		double a, anotherDouble = 13.5, etc;
		IT("...", {
			EXPECT(5+5).toBe(10);
		});
		BEFORE_ALL({ ... });
		AFTER_ALL({ ... });
	});
});
DESCRIBE("...", {
	IT("...", {
		EXPECT(...).[...](...);
	});
});
...
SCENARIO_END
```

***Incorrect  examples:***
```c++
...
DESCRIBE("...", {
	// Incorrect. You MUST use `EXPECT` directly from `IT`
	EXPECT(5+5).toBe(10);

	BEFORE_EACH({
		// Incorrect. You MUST use `IT` directly from `DESCRIBE`
		IT("...", {
			EXPECT(...).[...](...);
		})
	});

	IT("...", {
		// Incorrect. You MUST use `BEFORE_ALL` directly from `DESCRIBE`
		BEFORE_ALL({ ... });
	});

	if(stars_lined_up){
		// Incorrect. You MUST use `IT` directly from `DESCRIBE` code scope (not from `if`, `while` or any another)
		IT("...", {
			EXPECT(...).[...](...);
		});
	}
});
...
```
____

### Code scope
You can use all global variables inside any of *predefined macros*, also, all variables that were defined in one of the `DESCRIBE` macro code scope will be available as a **reference** in all "child" macro calls. Formally, you can treat the 
```c++
DESCRIBE("...", { /* More code here */ });
```
as actually call of lambda function with reference variables rule
```c++
[&](){ /* More code here */ });
```
And you can rely on such **lambda** functionality behavior.

***Example:***

```c++
...
DESCRIBE("Some test", {
	int a = 10;
	IT("`a` should be equals to 10", {
		EXPECT(a).toBe(10); // Works as expected
	});
	DESCRIBE("One more inner describe", {
		int a = 5;
		BEFORE_ALL({ /* `a` is accesible and equals to 5 */ });
		IT("`a` should be equals to 5", {
			EXPECT(a).toBe(5); // As expected
		});
	});
	DESCRIBE("Inner describe", {
		IT("`a` should still be equals to 10", {
			EXPECT(a).toBe(10); // Works as expected
		});
	});
});
...
```
____

### Macros

There are 2 system macros you must use to define "borders" for all your test cases:

- **SCENARIO_START**
- **SCENARIO_END**

To build your test cases you are allowed to use the following predefined c++ macro functions:

- **DESCRIBE**
- **BEFORE_ALL**
- **AFTER_ALL**
- **BEFORE_EACH**
- **AFTER_EACH**
- **IT**
- **EXPECT**

Few more macros that allow us to debug code while testing it:

- **DESCRIBE_SKIP**
- **DESCRIBE_ONLY**
- **IT_SKIP**
- **IT_ONLY**

Two more macros to work with test cases:

- **TEST_SUCCEED**
- **TEST_FAILED**

And one macro for providing additional information to the test case result:

- **INFO_PRINT**

Lets move through all of them
____

#### SCENARIO_START
System macro you should use to define beginning of your test cases

***Example:***
```c++
SCENARIO_START
...
DESCRIBE("...", {
...
```
____

#### SCENARIO_END
System macro you should use to define ending of your test cases

***Example:***
```c++
SCENARIO_START
...
DESCRIBE("Test", {
...
});
DESCRIBE("Another Test", {
...
});
...
SCENARIO_END
```
____

#### DESCRIBE (string description, {})
`DESCRIBE` macro is used to group some test cases, enclose the scope, split up the test cases, and provide the useful description for the group of tests, or preparation part. The description is going to be generated using current and all parent to current `DESCRIBE`'s. 
It receives **2** parameters: first one is the **string description** - which is the simple text description that will be used to generate the group of test overall description, and the second one is the "code scope" to be ran (see examples to get the better understanding). 

***Example:***
Consider we have the next structure of our test cases:
```c++
...
DESCRIBE("Test numbers", {
	DESCRIBE("With floating point", {
		DESCRIBE("Generated by arithmetic operations", {
			...
		});
		...
	});
	...
});
...
```
*Notice, we didn't put `SCENARIO` macros in the examples, but they are required in real test cases*

And consider we have the `IT` macro (which is the real test definer) placed to each of the "describe", the result would be the following:
- Test numbers
	- Test case 1
	- Test case 2
	- ...
- Test numbers With floating point
	- Test case 1
	- Test case 2
	- ...
- Test numbers With floating point Generated by arithmetic operations
	- Test case 1
	- Test case 2
	- ...

But if one of `DESCRIBE` blocks does not contain direct `IT` calls, then this group of tests will not be showed (as it is not contains any test cases, so it does not make sense to show it).
____

#### DESCRIBE_SKIP (string description, {})
`DESCRIBE_SKIP` macro has the same syntax, call rules and used for the same purposes as `DESCRIBE`, but instead of running the tests, it is actually skip all the test cases with its preparations (`BEFORE_...` and `AFTER_...`) of current `DESCRIBE_SKIP` and all of it children ones. 

This macro also used for the debugging purposes, when you want to disable tests to be running, for a while (or intentionally disable some of the test cases).

Skipped tests will anyway be showed in the test results, but they will be marked as *"(skipped)"* and indicated with different color.
____

#### DESCRIBE_ONLY (string description, {})
`DESCRIBE_ONLY` macro has the same syntax, call rules and used for the same purposes as `DESCRIBE`, but it is disabling all tests to be called outside of the `DESCRIBE_ONLY` (or `IT_ONLY`) blocks. (All of it children tests will be called as well). 

This macro also used for the debugging purposes, when you want to focus on one or few important `DESCRIBE`'s while building the test cases. 
But unlike the `DESCRIBE_SKIP`, tests that was "filtered" by `DESCRIBE_ONLY` rule will not be showed in the test results.

***Important:*** this macro will work as expected only if additionally `TEST_ONLY_RULE` variable is defined before the `#include "qtest.hpp"`. Otherwise it will work as a simple `DESCRIBE` rule.

***Example:***
```c++
...
#define TEST_ONLY_RULE
#include "qtest.hpp"

DESCRIBE("...", {
	// Will not be called
	IT("...", { ... });
	// Will not be called as well
	BEFORE_EACH({ ... });
});
DESCRIBE_ONLY("...", {
	// Will be called as wrapped to `DESCRIBE_ONLY`
	IT("...", { ... });
	// Will be called as well
	BEFORE_EACH({ ... });
});
DESCRIBE("...", {
	// Will be called as IT called with ONLY rule as well
	IT_ONLY("...", { ... });
	// Will be called as well, as this code preparation block relates to the test above.
	BEFORE_EACH({ ... });
});
```
____
#### BEFORE_ALL ({})
This macro requires only the **code scope** to be passed as a first parameter. The code inside the brackets will be called only once, before the first test from the `DESCRIBE` where this `BEFORE_ALL` placed is executed. Usually this rule is used to initialize the variables needed for the test cases.

***Example:***
```c++
...
DESCRIBE("Test step array values", {
	int* arr;
	BEFORE_ALL({
		arr = new int[10];
		arr[0] = 2;
		for(int i=1;i<10;i++)
			arr[i] = arr[i-1]*2;
	});
	IT("the 5th item from `arr` should be equal to 32", {
		EXPECT(arr[4]).toBe(32);
	});
	IT("the 10th item from `arr` should be equal to 1024", {
		EXPECT(arr[9]).toBe(1024);
	});
});
``` 
____

#### AFTER_ALL ({})
This macro requires only the **code scope** to be passed as the first parameter. The code inside the brackets will be called only once, after the final test case of the `DESCRIBE` where this `AFTER_ALL` placed is executed. Usually this rule is used to release the memory assumed by the test cases.

***Example:***
```c++
DESCRIBE("Test step array values", {
	int* arr;
	BEFORE_ALL({
		arr = new int[10];
		arr[0] = 2;
		for(int i=1;i<10;i++)
			arr[i] = arr[i-1]*2;
	});
	AFTER_ALL({
		delete[] arr;
	});
	IT("...", { ... });
	IT("...", { ... });
});
```
____

#### BEFORE_EACH ({})
This macro requires only the **code scope** to be passed as the first parameter. The code inside the brackets will be called before **every** test case of the `DESCRIBE` where this `BEFORE_EACH` placed to, and all of this children `DESCRIBE`'s test cases. Usually this rule used to make some variable preparations that needs to be similar before each test case.

***Example:***
```c++
DESCRIBE("Test std::set", {
	std::set<int> s;
	BEFORE_EACH({
		s.clear();
		s.insert(10);
		s.insert(15);
		s.insert(5);
	});
	IT("first element of `s` should be 5", {
		EXPECT(*s.begin()).toBe(5);
	});
	IT("last element of `s` should be 15", {
		EXPECT( *(--s.end()) ).toBe(15);
	});
	DESCRIBE("After removing first element from s", {
		BEFORE_EACH({
			s.erase(s.begin());
		});
		IT("first element of `s` should be 5", {
			EXPECT(*s.begin()).toBe(10);
		});
		IT("last element of `s` should be 15", {
			EXPECT( *(--s.end()) ).toBe(15);
		});
	});
});
```
____

#### AFTER_EACH ({})
This macro requires only the **code scope** to be passed as the first parameter. The code inside the brackets will be called after **every** test case of the `DESCRIBE` where this `AFTER_EACH` placed to, and all of this children `DESCRIBE`'s test cases. Usually this rule used to make some variable release memory after each test case, or clear mutable changes.

***Example:***
```c++
DESCRIBE("Test std::set", {
	std::set<int> s;
	BEFORE_EACH({
		s.insert(10);
		s.insert(15);
		s.insert(5);
	});
	AFTER_EACH({
		s.clear();
	});
	IT("first element of `s` should be 5", { ... });
	IT("last element of `s` should be 15", { ... });
	DESCRIBE("After removing first element from s", { ... });
});
```
____

#### IT (string description, {})
This macro is used to define the actual test case. First parameter requires **string description** that represents the description of the test case that will be showed in test results, and the second parameter is a **code scope** of current test. You can define as many additional variables as you need inside the test code scope to execute the actual test.  

***Example:***
```c++
...
DESCRIBE("Some very important test", {
	IT("the size of vector should be one more than the size of hash table", {
		vector<int> v{1,1,2,3,5,8,13};
		unordered_set<int> s;
		for(auto &it : v){
			s.insert(it);
		}
		EXPECT(v.size()).toBe(s.size() + 1);
	});
});
...
```
____

#### IT_SKIP (string description, {})
`IT_SKIP` macro has the same syntax, call rules and used for the same purposes as `IT`, but instead of running the test, it is actually skip it.

This macro usually used for the debugging purposes, when you want to disable tests to be running for a while (or intentionally disable some of the test cases).

Skipped tests will anyway be showed in the test results, but they will be marked as *"(skipped)"* and indicated with different color.
____

#### IT_ONLY (string description, {})
`IT_ONLY` macro has the same syntax, call rules and used for the same purposes as `IT`, but it is disabling all another tests to be called that are not marked as `_ONLY`.

This macro also used for the debugging purposes, when you want to focus on the one or few important test cases, but unlike the `IT_SKIP`, tests that were "filtered" by `IT_ONLY` rule will not be showed in the test results.

***Important:*** this macro will work as expected only if additionally `TEST_ONLY_RULE` variable is defined before the `#include "qtest.hpp"`. Otherwise it will work as a simple `IT` rule.
____


#### EXPECT (T value)
This macro used to resolve the test case. It is required one parameter to be passed. You can send any **value** type you want, this macro will return **QTestExpect** class instance, which has different methods to proceed with. You can find all available methods in [QTestExpect class](#qtestexpect-t-actual) section.

You can use as many `EXPECT` calls as you need for every separate test case.

***Exapmle:***
```c++
...
IT("the `a` variable should be between `10` and `15`", {
	EXPECT(a).toBeLessThan(15);
	EXPECT(a).toBeGreaterThan(10);
});
...
```
____

#### TEST_SUCCEED ()
This macro requires no parameters to be passed, and used only inside the `IT` macro **code scope** and will **succeed** the test case it is used for. This macro equivalent to `EXPECT(1).toBe(1)` .

Note, if there are more expect constructions in a test, and one of them fail, the test is considered as failed!

***Example:***
```c++
...
IT("Should succeed", {
	TEST_SUCCEED();
});
...
```
____

#### TEST_FAILED ([string reason])
This macro accepts one optional parameter, the macro only inside the `IT` macro **code scope** and will **fail** the test case it is used for.

The reason of the failure will be shown under the failed test.

***Example:***
```c++
...
IT("Should fail", {
	TEST_FAILED();
});
...
```
____

#### INFO_PRINT (T value)
This macro accepts one optional parameter to be passed **T value** that must be **streamable**, meaning the operation `std::ostream << value` must be valid. And used to provide additional information to the test case. Used only inside the `IT` macro **code scope** and will print the text you pass after the test case result.
It also returns a reference to `std::ostream` object, so you can use `<<` operator to continue putting some more info to the info log.

***Example:***
```c++
...
IT("Additional info", {
	if(stars_lined_up){
		TEST_SUCCEED();
		INFO_PRINT("Stars Lined Up") << " don't forget to take a photo!";
	}
});
...
```

### QTestExpect (T&& actual)
This class used to make some "predictions" about the value you pass to `EXPECT` macro. It requires one parameter to be passed (any type allowed). The class contains a bunch of methods you can use to assert your actual value with some expected value.
There is a list of all methods allowed:

- toBe(T&& cmp) *(to_be)*
- toBeCloseTo(T&& cmp) *(to_be_close_to)*
- toBeGreaterThan(T&& cmp) *(to_be_greater_than)*
- toBeGreaterThanOrEqual(T&& cmp) *(to_be_greater_than_or_equal)*
- toBeLessThan(T&& cmp) *(to_be_less_than)*
- toBeLessThanOrEqual(T&& cmp) *(to_be_less_than_or_equal)*
- toBeNull() *(to_be_null)*
- toBeNullPtr() *(to_be_null_ptr)*
- toBeIteratableEqual(Iterable&& cmp) *(to_be_iteratable_equal)*
- toThrowError() *(to_throw_error)*
- toThrowError\<T\>() *(to_throw_error)*
- toReturnTrue *(to_return_true)*
- toReturn(T&& cmp) *to_return*

And one method `NOT()` that will return another `QTestExpect` instance, but the result of all methods described above will be reversed.

Lets move through.
___

#### toBe (C&& compare)
Method used to compare expected value with the actual value you have. It is requires one parameter to be passed. **Notice**, type of `compare` may not be the same as type of actual value, that was passed to `EXPECT` macro, but the comparison operator `==` must be defined between the types. **Note**: passing different integral types to the the `EXPECT` and `toBe` parameters might result in the **warning** depending on your compiler flags.

***Example:***
```c++
...
IT("Varialbes should be equal", {
	long a = 10;
	int exp = 10;
	EXPECT(a).toBe((long)exp);
});

IT("Vector size should be 10", {
	EXPECT((int)(vector<int>{1,2,3,4,5,6,7,8,9,10}).size()).to_be(10);
});

// Will throw an error
IT("Classes to be equal", {
	struct A{ ... };
	struct B{ ... };
	EXPECT(A()).toBe(B());
});
...
```
***Note:*** There is also `to_be` alias for this method allowed.
____

#### toBeCloseTo (T compare, T precision)
Method used to compare actual value to be within a specified precision of the expected value. It is requires 2 **T** parameters to be passed: the `compare` and `precision`. **T** parameter must be the same as the one passed to the `EXPECT`. This method will succeed the test if actual value lies within the [`compare - precision` , `compare + precision`] range.

***Example:***
```c++
...
IT("a should be close to 15 with precision of 0.5", {
	auto a = 15.5;
	EXPECT(a).toBeCloseTo(15, 0.5);
});
...
```
***Note:*** There is also `to_be_close_to` alias for this method allowed.
____

#### toBeGreaterThan (T compare)
Method will succeed test if the actual value is *greater than* the expected value. Requires one parameter to be passed with **the same type** as actual value you put to `EXPECT` macro.

***Example:***
```c++
...
EXPECT(15).toBeGreaterThan(10);
...
```
***Note:*** There is also `to_be_greater_than` alias for this method allowed.
____

#### toBeGreaterThanOrEqual (T compare)
Method will succeed test if the actual value is *greater than or equal* to the expected value. Requires one parameter to be passed with **the same type** as actual value you put to `EXPECT` macro.

***Example:***
```c++
...
EXPECT(15).toBeGreaterThanOrEqual(15);
...
```
***Note:*** There is also `to_be_greater_than_or_equal` alias for this method allowed.
____

#### toBeLessThan (T compare)
Method will succeed test if the actual value is *less than* the expected value. Requires one parameter to be passed with **the same type** as actual value you put to `EXPECT` macro.

***Example:***
```c++
...
EXPECT(15).toBeLessThan(20);
...
```
***Note:*** There is also `to_be_less_than` alias for this method allowed.
____

#### toBeLessThanOrEqual (T compare)
Method will succeed test if the actual value is *less than or equal* to the expected value. Requires one parameter to be passed with **the same type** as actual value you put to `EXPECT` macro.

***Example:***
```c++
...
EXPECT(15).toBeLessThanOrEqual(15);
...
```
***Note:*** There is also `to_be_less_than_or_equal` alias for this method allowed.
____

#### toBeNull ()
Method will succeed test of the actual value is equal to `NULL`. No parameters required.

***Example:***
```c++
...
int* a = NULL;
EXPECT(a).toBeNull();
...
```
***Note:*** There is also `to_be_null` alias for this method allowed.
____

#### toBeNullPtr ()
Method will succeed test of the actual value is equal to `nullptr`. No parameters required.

***Example:***
```c++
...
int* a = nullptr;
EXPECT(a).toBeNullPtr();
...
```
***Note:*** There is also `to_be_null_ptr` alias for this method allowed.
____

#### toBeIterableEqual (CT&& compare)
Method used to compare your **iterable** object with the list of items (or another iterable object). It is requires one parameter to be passed to compare with. 

***Important:*** Iterable objects - the objects that have predefined methods such as `begin` and `end` that returns [Iterator object](https://en.wikipedia.org/wiki/Iterator)

***Example:***
```c++
...
auto vec = vector<int>{5,6,7,8,9};

IT("vec should toBeIterableEqual to {5,6,7,8,9}", {
	EXPECT(vec).toBeIterableEqual({5,6,7,8,9});
});

IT("vec should toBeIterableEqual to list<int>{5,6,7,8,9}", {
	list<int> comp;
	for(int i=5;i<10;i++)
		comp.push_back(i);
	
	EXPECT(vec).toBeIterableEqual(comp);
});
...
```
***Note:*** There is also `to_be_iterable_equal` alias for this method allowed.
____

#### toThrowError ()
This method requires the **function** to be passed to `EXPECT` macro. When called, it will succeed the test case, if the function passed to `EXPECT` is going to throw an error.

***Example:***
```c++
EXPECT([](){ throw "error here"; }).toThrowError();
```
***Note:*** There is also `to_throw_error` alias for this method allowed.
____

#### toThrowError\<E\> ()
This method requires the **function** to be passed to `EXPECT` macro. When called, it will succeed the test case, if the function passed to `EXPECT` is going to throw an error with type == `E`.

***Example:***
```c++
EXPECT([](){ throw std::logic_error("error"); }).toThrowError<std::logic_error>();
```
***Note:*** There is also `to_throw_error<E>` alias for this method allowed.
____

#### toReturnTrue ()
This method requires the **function** to be passed to `EXPECT` macro. When called, it will succeed the test case, if the function passed to `EXPECT` is going to return `true`.

#### toReturn (C&& compare)
This method requires the **function** to be passed to `EXPECT` macro. When called, it will succeed the test case, if the function passed to `EXPECT` is going to return the value that is `== compare`.

***Example:***
```c++
EXPECT([]() -> bool { return 1; }).toReturnTrue();
```
***Note:*** There is also `to_return_true` alias for this method allowed.
____

#### NOT ()
This method does not requires any parameters to be passed, and will return another `QTestExpect` class instance, but with reversed results of methods. See examples to get better understanding.

***Example:***
```c++
EXPECT(42).NOT().toBe(72) 

EXPECT(15.5).NOT().toBeCloseTo(15,0.49);

EXPECT([](){
	int foo = 5;
	int bar = foo%2;
	foo = bar+foo;
	// party hard...
}).NOT().toThrowError();

// All EXPECT's succeed
``` 

## Tips

### Garbage test result

**For Linux users:** If after executing test, you can see some garbage instead of actual test results, most probably your console does not support colors. If you anyway want to continue using this console to view the test results, you can try the following option for turning off the colored test results. 
To do that, all you need is to **define** `TEST_RESULTS_NO_COLOR` variable **before the include** test framework **.hpp** file.

***Example:***
```c++
...
#define TEST_RESULTS_NO_COLOR
#include "qtest.hpp"

DESCRIBE("...", {
...
```
____

### Cycles and conditions
You are **not allowed** to use cycles for tests definitions.

***Example:***
```c++
...
DESCRIBE("Test `calc` method", {
	vector<int> v{1, 5, 10, 1000, 42, 525, 1024, 0, -1, -999};
	int index = 0;

	AFTER_EACH({
		// Increment index
		index++;
	});

	for(int i=0;i<v.size();i++){
		IT("with "+to_string(v[i]), {
			EXPECT(yourTestObject.calc(v[index])).toBe(true);
		});
	}
});
...
```
It will not work as you **MUST** call `IT` directly from `DESCRIBE` macro block scope. Same with `DESCRIBE` macros.

But it is allowed to use cycles and conditions inside the `IT` macro.

***Example:***
```c++
...
IT("Bunch of checks", {
    for(int i=0;i<10;i++){
        EXPECT(i).toBeLessThan(10);
    }
});
...
```
____

### Expect object aliases
Each of the `QTestExpect` class instance methods (except `NOT`) has aliases in **snake case** format (small characters delimited with underscore `_` character). 

***Example:*** 
```c++
...
EXPECT(1).to_be(1);
EXPECT(15.5).to_be_close_to(15,0.5);
EXPECT([](){...}).NOT().to_throw_error();
...
```
____

### "ONLY" rule
If you are going to use `DESCRIBE_ONLY` or `IT_ONLY` macros, don't forget to put `#define TEST_ONLY_RULE` before the `#include "qtest.hpp"`

____

### "SCENARIO" macroses
Now, you MUST put all your `DESCIBE` macros between `SCENARIO_START` and `SCENARIO_END` macros.

### Test ordering
All of the tests are going to be executed in the same order they are written.

***Example:***
```c++
...
DESCRIBE("group 1", {
	IT("test1", { ... });

	DESCRIBE("group 2", {
		IT("test2", { ... });
	});

	IT("test3", { ... });
});
...
```

The order of execution is:
* test1
* test2
* test3

Just like this, the `BEFORE_EACH`, and `AFTER_EACH` constructions are executed only for tests that are written after these constructions were written.

***Example:***
```c++
...
DESCRIBE("...", {
	IT("test1", { ... });

	BEFORE_EACH({ ... });

	IT("test2", { ... });
});
...
```

`BEFORE_EACH` will be executed only before the "test2".

It is a good rule of thump to write `BEFORE_EACH` and `AFTER_EACH` at the beginning of the `DESCRIBE` construction.

Similarly `BEFORE_ALL` construction will be ran before the first test that follows it.

### Failed EXPECT results

When the `EXPECT` construction fails, the code inside the `IT` clause stops execution.

Additionally, when the failed tests are shown, it provides information about which specifically `EXPECT` failed.

***Example:***
```c++
DESCRIBE("group", {
	int a = 42;
	IT("should be equal 69", {
		EXPECT(a).toBe(68);
	});
});
```

This test will fail and result in something like:
```
...
  group
    [x] should be equal 69 (file.cpp:123)
         - EXPECT(a[=42]).toBe(69) FAILED!
...
```

And `TEST_FAILED("reason")` construction will inform the passed reason of the failure.

### V1 -> V2 changes

* The expected C++ version was increased from **C++11** to **C++17**.
* `TEST_FAILED` macro now accepts optional string parameter.
* Test execution order became much more predictable.
* `INFO_PRINT` now accepts any streamable value.
* `IT` execution stops immediately after first failed `EXPECT`.
* Failed tests show the **file name** and **line number**, which makes it easier to locate.
* Failed tests show extra info.

## More

If you found some issues, or you have some questions about related to **QTest**, feel free to post them to [issue tracker](https://github.com/webdeveloperukraine/QTest/issues) here on github. 

## License

MIT

Have fun :)
