#ifndef QTEST_H
#define QTEST_H


#include <functional>
#include "qtestbase.hpp"
#include "qtestutils.hpp"

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


// Initialize file local helpers
namespace{
	QTestBase QTESTOBJECTDEFINED = QTestBase();
	using function_cb_t = std::function<void()>;
	struct QTestDescribe_t
	{
		QTestDescribe_t(std::string str, function_cb_t fn, int param, const char* file){ QTESTOBJECTDEFINED.describe(str, fn, param, file); };
	};
	Q_TEST__CALLBACK();
}




/** ***********************USAGE****************************
 * 
 * 
 * DESCRIBE("...",{
 * 
 *     auto a = ...
 * 
 *     BEFOREALL({ ... });
 *     BEFOREEACH({ ... });
 * 
 *     DESCRIBE("...", {
 * 
 *         int b = ...
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
