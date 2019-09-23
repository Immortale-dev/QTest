#include <iostream>
#include <unistd.h>
#include <list>

#define TEST_ONLY_RULE
#include "qtest.hpp"

using namespace std;


DESCRIBE_ONLY("[Test]", {
		
	DESCRIBE("second level", {

		IT("should succeed as TEST_SUCCEED method used", {
			TEST_SUCCEED();
		});
		
		IT("should fail as TEST_FAILED method used", {
			TEST_FAILED();
		});
	});
	
	DESCRIBE("Static variables", {
		
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
				
			});
			
			DESCRIBE("To check static variables scope", {
				
				IT("inc variable should be equals to 2", {
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
			
			IT("Vector should contains 10 items", {
				EXPECT((int)vec.size()).toBe(10);
			});
			
		});
		
		DESCRIBE("vector<int> type variable second section", {
			
			vector<int> vec;
			
			BEFORE_ALL({
				
				for(int i=0;i<8;i++)
					vec.push_back(i);
				
			});
			
			IT("Vector should contains 8 items", {
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
	
	DESCRIBE("Dynamic duration variables", {
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
		
		AFTER_ALL({
			a = 1;
		});
	});
	
	DESCRIBE("toBeCloseTo expect method", {
		auto a = 15.5;
		
		IT("a should be close to 15 with precision of 0.5", {
			EXPECT(a).toBeCloseTo(15, 0.5);
		});
	});
	
	DESCRIBE("toBeIteratableEqual expect method", {
		auto vec = vector<int>();
		
		BEFORE_ALL({
			for(int i=5;i<10;i++)
				vec.push_back(i);
		});
		
		IT("vec should toBeIteratableEqual to {5,6,7,8,9}", {
			EXPECT(vec).toBeIteratableEqual({5,6,7,8,9});
		});
		
		IT("vec should toBeIteratableEqual to list<int>{5,6,7,8,9}", {
			list<int> comp;
			for(int i=5;i<10;i++)
				comp.push_back(i);
			
			EXPECT(vec).toBeIteratableEqual(comp);
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
			exit(1);
		});
		
		BEFORE_EACH({
			//should be skipped
			exit(1);
		});
		
		AFTER_ALL({
			//should be skipped
			exit(1);
		});
		
		AFTER_EACH({
			//should be skipped
			exit(1);
		});
		
		IT("should be skipped", {
			TEST_SUCCEED();
		});
		
		IT("should be skipped as well", {
			TEST_FAILED();
		});
	});
	
	DESCRIBE("Delayed tests", {
		IT("should be delayed 1 sec", {
			sleep(1);
			TEST_SUCCEED();
		});
		IT("should be delayed 2 sec", {
			sleep(2);	
			TEST_SUCCEED();
		});
		IT("should be delayed 3 sec", {
			sleep(3);	
			TEST_SUCCEED();
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
	});
	
	IT("should not be visible eaither", {
		TEST_FAILED();
	});
	
});

int main() { return 0; }
