#ifndef QTEST_H
#define QTEST_H

#include "qtestbase.hpp"

#define DESCRIBE(a, ...) QTESTOBJECTDEFINED.describe(a, [&]()__VA_ARGS__, __FILE__)
#define BEFORE_ALL(...) QTESTOBJECTDEFINED.before([&]()__VA_ARGS__)
#define BEFORE_EACH(...) QTESTOBJECTDEFINED.before_each([&]()__VA_ARGS__)
#define AFTER_ALL(...) QTESTOBJECTDEFINED.after([&]()__VA_ARGS__)
#define AFTER_EACH(...) QTESTOBJECTDEFINED.after_each([&]()__VA_ARGS__)
#define IT(a, ...) QTESTOBJECTDEFINED.it(a, [&]()__VA_ARGS__)
#define EXPECT(a) QTESTOBJECTDEFINED.expect((a))
#define TEST_FAILED() EXPECT(1).toBe(0)
#define TEST_SUCCEED() EXPECT(1).toBe(1)
#define VAR static auto



// Initialize file local test object
namespace{
	QTestBase QTESTOBJECTDEFINED = QTestBase();
}


/** ***********************USAGE****************************
 * 
 * 
 * DESCRIBE("...",{
 * 
 *     VAR a = ...
 * 
 *     BEFOREALL({ ... });
 *     BEFOREEACH({ ... });
 * 
 *     DESCRIBE("...", {
 * 
 *         VAR b = ...
 * 
 *         BEFOREALL({ ... });
 *         BEFOREEACH({ ... });
 * 
 *         IT("...", { 
 *         
 *             ...
 *             EXPECT(expression).[...] /// toBe | ... 
 * 
 *         });
 * 
 *         AFTEREACH({ ... });	
 *         AFTERALL({...});
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

#endif // QTEST_H
