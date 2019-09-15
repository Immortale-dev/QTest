#ifndef QTEST_H
#define QTEST_H

#include <iostream>
#include <functional>


#define DESCRIBE(a, ...) QTESTOBJECTDEFINED->describe(a, [&]()__VA_ARGS__ );

using function_cb_t = std::function<void()>;

class QTest
{
	public:
		QTest();
		
		void describe(const char* str, function_cb_t f );
		/*
		void before();
		void before_each();
		void after();
		void after_each();
		void it();
		void expect();*/
		
	private:
		int a;
		// ...
};

QTest::QTest()
{
	
}

void QTest::describe(const char* str, function_cb_t f )
{
	std::cout << "GOOD" << std::endl;
	f();
}

// Initialize file local test object
namespace{
	QTest *QTESTOBJECTDEFINED = new QTest();
}



/** ***********************USAGE****************************
 * 
 * 
 * DESCRIBE("...",{
 *     BEFOREALL({ ... });
 *     BEFOREEACH({ ... });
 * 
 *     DESCRIBE("...", {
 * 
 *         IT("...", { 
 *         
 *             ...
 *             EXPECT(expression).[...] /// EQUALS | GREATERTHAN | LOWERTHATN | GREATEREQUALS | LOWEREUQUALS | ... see https://jasmine.github.io/api/3.4/matchers.html
 * 
 *         });
 * 
 *     });
 * 
 *     DESCRIBE(...);
 * 
 *     DESCRIBE(...);
 * 
 *     DESCRIBE(...);
 * 
 *     AFTEREACH({ ... });	
 *     AFTERALL({...});
 * });
 * 
 * ***********************************************************/


#endif //QTEST_H
