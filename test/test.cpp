#include <iostream>
#include <unistd.h>
#include <list>
#include <set>
#include <unordered_set>

#define TEST_ONLY_RULE
#include "qtest.hpp"

using namespace std;

SCENARIO_START

DESCRIBE_ONLY("[Test]", {

	DESCRIBE("Second level", {
		IT("should succeed as TEST_SUCCEED method used", {
			TEST_SUCCEED();
		});

		IT("should fail as TEST_FAILED method used", {
			TEST_FAILED();
		});

		IT("should fail with a message", {
			TEST_FAILED("Yeah, I'm the message!");
		});

		IT("should fail with NOT", {
			EXPECT(10).NOT().toBe(10);
		});

		IT("should fail string comparison", {
			std::string str = "hello";
			EXPECT(str).toBe("world");
		});
	});

	DESCRIBE("Variables", {
		DESCRIBE("Int type variable", {
			int inc = 2;

			DESCRIBE("With before_each increment function", {
				int inc = 0;

				BEFORE_EACH({
					inc++;
				});

				IT("First test - inc variable should be equals to 1", {
					EXPECT(inc).toBe(1);
				});

				IT("Second test - inc variable should be equals to 2", {
					EXPECT(inc).toBe(2);
				});

				IT("Third test - inc variable should be equals to 3", {
					EXPECT(inc).toBe(3);
				});

				IT("should fail with info", {
					EXPECT(inc).toBe(10);
					INFO_PRINT() << "INFO" << 13;
				});
			});

			DESCRIBE("Sum of variables", {
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

			DESCRIBE("And check variables scope", {
				IT("inc variable should be equals to 2 here", {
					EXPECT(inc).toBe(2);
				});
			});
		});

		DESCRIBE("vector<int> type varialbe first section", {
			static vector<int> vec;

			BEFORE_ALL({
				for(int i=0;i<10;i++)
					vec.push_back(i);
			});

			IT("Vector should contain 10 items", {
				EXPECT((int)vec.size()).toBe(10);
			});
		});

		DESCRIBE("vector<int> type variable second section", {
			vector<int> vec;

			BEFORE_ALL({
				for(int i=0;i<8;i++)
					vec.push_back(i);
			});

			IT("Vector should contain 8 items", {
				EXPECT((int)vec.size()).toBe(8);
			});
		});

		DESCRIBE("int variable that will be shared", {
			int shared = 5;

			DESCRIBE("Redeclared", {
				int shared = 15;

				IT("shared variable should be eq to 15", {
					EXPECT(shared).toBe(15);
				});
			});

			DESCRIBE("Reuse of shared variable", {

				BEFORE_ALL({
					shared += 5;
				});

				IT("shared variable should be eq to 5", {
					EXPECT(shared).toBe(10);
				});
			});
		});

		DESCRIBE("auto type variables", {
			auto vec = vector<int>();

			for(int i=0;i<100;i++)
				vec.push_back(i);

			IT("vec variable should contain 100 elements", {
				EXPECT((int)vec.size()).toBe(100);
			});
		});

		DESCRIBE("auto pointer type variables", {
			auto vec = new vector<int>();

			BEFORE_ALL({
				for(int i=0;i<100;i++){
					vec->push_back(i);
				}
			});

			AFTER_ALL({
				delete vec;
			});

			IT("vec variable should be a pointer and contain 100 elements", {
				EXPECT((int)vec->size()).toBe(100);
			});
		});

	});

	DESCRIBE("Dynamic time duration variables", {
		int* a = new int(0);

		DESCRIBE("with before_each increment function", {
			BEFORE_EACH({
				(*a)++;
			});

			IT("a variable should be equal to 1", {
				EXPECT(*a).toBe(1);
			});

			IT("a variable should be equal to 2", {
				EXPECT(*a).toBe(2);
			});

			AFTER_ALL({
				delete a;
			});
		});
	});

	DESCRIBE("NOT expect method", {
		auto a = 1;
		auto b = 15.5;

		BEFORE_ALL({
			a = 10;
		});

		IT("a should not toBe equal to 1", {
			EXPECT(a).NOT().toBe(1);
		});

		IT("a should not toBeLessThan 10", {
			EXPECT(a).NOT().toBeLessThan(10);
		});

		IT_SKIP("a should not toBeLessThanOrEqual 9", {
			EXPECT(a).NOT().toBeLessThanOrEqual(9);
		});

		IT("should not toThrowError", {
			EXPECT([](){
				int foo = 5;
				int bar = foo%2;
				foo = bar+foo;
				// party hard...
			}).NOT().toThrowError();
		});

		IT("b should not toBeCloseTo 15 with precision of 0.49", {
			EXPECT(b).NOT().toBeCloseTo(15,0.49);
		});

		IT("should return false", {
			EXPECT([](){ return !1; }).NOT().toReturnTrue();
		});

		IT("should fail", {
			EXPECT([](){ return !1; }).toReturnTrue();
		});

		AFTER_ALL({
			a = 1;
		});
	});

	DESCRIBE("toBeCloseTo expect method", {
		auto a = 15.5;

		IT("a should be close to 15 with precision of 0.5", {
			EXPECT(a).toBeCloseTo(15, 0.5);
		});

		IT("should fail", {
			EXPECT(a).toBeCloseTo(16, 0.25);
		});
	});

	DESCRIBE("toBeIterableEqual expect method", {
		auto vec = vector<int>();

		BEFORE_ALL({
			for(int i=5;i<10;i++)
				vec.push_back(i);
		});

		IT("vec should toBeIterableEqual to {5,6,7,8,9}", {
			EXPECT(vec).toBeIterableEqual({5,6,7,8,9});
		});

		IT("vec should toBeIterableEqual to list<int>{5,6,7,8,9}", {
			list<int> comp;
			for(int i=5;i<10;i++)
				comp.push_back(i);

			EXPECT(vec).toBeIterableEqual(comp);
		});

		IT("should fail", {
			vector<int> a{1,2,3,4,5};
			vector<int> b{2,3,4,5,6};

			EXPECT(a).toBeIterableEqual(b);
			// Never called
			exit(3);
		});
	});

	DESCRIBE("toReturnTrue expect method", {
		auto fun = [](){ return 10>5; };

		IT("fun should return true", {
			EXPECT(fun).toReturnTrue();
		});
	});

	DESCRIBE("toThrowError expect method", {
		IT("Should throw an error", {
			EXPECT([](){ throw "error here"; }).toThrowError();
		});
	});

	DESCRIBE_SKIP("skip describe", {

		BEFORE_ALL({
			//should be skipped
			exit(2);
		});

		BEFORE_EACH({
			//should be skipped
			exit(2);
		});

		AFTER_ALL({
			//should be skipped
			exit(2);
		});

		AFTER_EACH({
			//should be skipped
			exit(2);
		});

		IT("should be skipped", {
			TEST_SUCCEED();
		});

		IT("should be skipped as well", {
			TEST_FAILED();
		});
	});

	DESCRIBE("Scope", {
		int a = 10;
		IT("the `a` variable should be between `10` and `15`", {
			EXPECT(a).toBeLessThan(15);
			EXPECT(a).toBeGreaterThanOrEqual(10);
		});
		IT("should fail", {
			EXPECT(a).toBeLessThan(15);
			EXPECT(a).toBeLessThan(5);
		});
		IT("`a` should be equals to 10", {
			EXPECT(a).toBe(10); // Works as expected
		});
		DESCRIBE("Inner describe", {
			IT("`a` should still be equals to 10", {
				EXPECT(a).toBe(10); // Works as expected
			});
		});
		DESCRIBE("One more inner describe", {
			int a = 5;
			BEFORE_ALL({ /* `a` is accesible and equals to 5 */ });
			IT("`a` should be equals to 5", {
				EXPECT(a).toBe(5); // As expected
			});
		});
	});

	DESCRIBE("Step array values", {
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
		AFTER_ALL({
			delete[] arr;
		});
	});

	DESCRIBE("std::set", {
		std::set<int> s;
		BEFORE_EACH({
			s.insert(10);
			s.insert(15);
			s.insert(5);
		});
		AFTER_EACH({
			s.clear();
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

	DESCRIBE("Additional variables inside `IT`", {
		IT("The size of vector should be one more than the size of hash table", {
			vector<int> v{1,1,2,3,5,8,13};
			unordered_set<int> s;
			for(auto &it : v){
				s.insert(it);
			}
			EXPECT(v.size()).toBe(s.size()+1);
		});
	});

	DESCRIBE("Type transform", {
		IT("Varialbes should be equal", {
			long a = 10;
			int exp = 10;
			EXPECT(a).toBe((long)exp);
		});

		IT("Vector size should be 10", {
			EXPECT((int)(vector<int>{1,2,3,4,5,6,7,8,9,10}).size()).to_be(10);
		});
	});

	DESCRIBE("Before hooks order", {
		vector<int> order_list;
		BEFORE_ALL({
			order_list.push_back(1);
		});
		BEFORE_EACH({
			order_list.push_back(2);
		});

		DESCRIBE("with multi level of DESCRIBE's", {
			BEFORE_EACH({
				order_list.push_back(4);
			});
			BEFORE_ALL({
				order_list.push_back(3);
			});

			IT("`order_list should contain values {1,2,3,4}`", {
				EXPECT(order_list).toBeIterableEqual({1,2,3,4});
			});
		});

		IT("`order_list` should contain values {1,2,3,4,2}", {
			EXPECT(order_list).toBeIterableEqual({1,2,3,4,2});
		});
	});

	vector<int> order_list;
	DESCRIBE("After hooks order", {
		DESCRIBE("Start", {
			AFTER_ALL({
				order_list.push_back(1);
			});
			AFTER_EACH({
				order_list.push_back(2);
			});

			DESCRIBE("with multi level of DESCRIBE's", {
				AFTER_EACH({
					order_list.push_back(4);
				});
				AFTER_ALL({
					order_list.push_back(3);
				});

				IT("`order_list should contain values {}`", {
					EXPECT(order_list.size()).toBe(0);
				});

				IT("`order_list should contain values {4,2}`", {
					EXPECT(order_list).toBeIterableEqual({4,2});
				});
			});

			IT("`order_list` should contain values {4,2,4,2,3}", {
				EXPECT(order_list).toBeIterableEqual({4,2,4,2,3});
			});
		});
		IT("`order_list` should contain values {4,2,4,2,3,2,1}", {
			EXPECT(order_list).toBe({4,2,4,2,3,2,1});
		});
	});

	DESCRIBE("Execution order", {
		int ta = 1;

		IT("`ta` should be equal to 1", {
			EXPECT(ta).toBe(1);
		});

		DESCRIBE("overwrite `ta` with 2", {
			ta = 2;

			IT("`ta` should be equal to 2", {
				EXPECT(ta).toBe(2);
			});
		});
	});

    DESCRIBE("brefore/after run for skipped content", {
        bool should_stay_false = false;
        DESCRIBE("Everything skipped inside", {

            BEFORE_ALL({
                should_stay_false = true;
            });

            AFTER_ALL({
                should_stay_false = true;
            });

            BEFORE_EACH({
                should_stay_false = true;
            });

            AFTER_EACH({
                should_stay_false = true;
            });

            DESCRIBE_SKIP("Skipped content", {
                IT("I am skipped", {
                    TEST_FAILED();
                });

                DESCRIBE("Also skipped content", {
                    IT("I am skipped", {
                        TEST_FAILED();
                    });
                });
            });
        });

        DESCRIBE("Check skippness", {
            IT("`should_stay_false` should still be `false`", {
                EXPECT(should_stay_false).toBe(false);
            });
        });
    });
});


DESCRIBE("This one is another describe that beside to DESCRIBE_ONLY rule", {

	IT("should not be visilble", {
		TEST_FAILED();
	});

	IT("should not be visilble too", {
		TEST_FAILED();
	});

	IT_ONLY("should be visible as IT_ONLY rule applied", {
		TEST_SUCCEED();
		INFO_PRINT("some additional info");
		INFO_PRINT() << "another " << 123 << " info";
	});

	IT("should not be visible eaither", {
		TEST_FAILED();
	});
});

SCENARIO_END

int main() { return 0; }
