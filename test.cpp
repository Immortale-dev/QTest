#include <iostream>

#include "qtest.hpp"

using namespace std;

int main()
{
	
	
	
	DESCRIBE("Test", {
		
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
				static int inc = 2;
				
				DESCRIBE("With before_each increment function", {
					
					static int inc = 0;
					
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
				
				static vector<int> vec;
				
				BEFORE_ALL({
					
					for(int i=0;i<8;i++)
						vec.push_back(i);
					
				});
				
				IT("Vector should contains 8 items", {
					EXPECT((int)vec.size()).toBe(8);
				});
				
			});
			
			DESCRIBE("int variable that will be shared", {
				static int shared = 5;
				
				DESCRIBE("Redeclared", {
					static int shared = 15;
					
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
				
				VAR vec = vector<int>();
				
				for(int i=0;i<100;i++)
					vec.push_back(i);
					
					
				IT("vec variable should contain 100 elements", {
					EXPECT((int)vec.size()).toBe(100);
				});
				
			});
			
			DESCRIBE("auto pointer type variables", {
				VAR vec = new vector<int>();
				
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
			static int* a = new int(0);
			
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
		
	});
	
	
	return 0;
}
