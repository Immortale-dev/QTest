#ifndef QTEST_H
#define QTEST_H

#include "qtestbase.hpp"
#include "qtestutils.hpp"

#define DESCRIBE(a, ...) Q_TEST_NS_DETAIL::BASE.describe(a, Q_TEST__LAMBDA_CALLBACK(__VA_ARGS__), QTEST_TEST_PARAM_ID, __FILE__)
#define DESCRIBE_ONLY(a, ...) Q_TEST_NS_DETAIL::BASE.describe(a, Q_TEST__LAMBDA_CALLBACK(__VA_ARGS__), QTEST_ONLY_PARAM_ID, __FILE__)
#define DESCRIBE_SKIP(a, ...) Q_TEST_NS_DETAIL::BASE.describe(a, Q_TEST__LAMBDA_CALLBACK(__VA_ARGS__), QTEST_SKIP_PARAM_ID, __FILE__)
#define BEFORE_ALL(...) Q_TEST_NS_DETAIL::BASE.before(Q_TEST__LAMBDA(__VA_ARGS__))
#define BEFORE_EACH(...) Q_TEST_NS_DETAIL::BASE.before_each(Q_TEST__LAMBDA(__VA_ARGS__))
#define AFTER_ALL(...) Q_TEST_NS_DETAIL::BASE.after(Q_TEST__LAMBDA(__VA_ARGS__))
#define AFTER_EACH(...) Q_TEST_NS_DETAIL::BASE.after_each(Q_TEST__LAMBDA(__VA_ARGS__))
#define IT(a, ...) Q_TEST_NS_DETAIL::BASE.it(a, Q_TEST__LAMBDA(__VA_ARGS__), QTEST_TEST_PARAM_ID, __LINE__)
#define IT_ONLY(a, ...) Q_TEST_NS_DETAIL::BASE.it(a, Q_TEST__LAMBDA(__VA_ARGS__), QTEST_ONLY_PARAM_ID, __LINE__)
#define IT_SKIP(a, ...) Q_TEST_NS_DETAIL::BASE.it(a, Q_TEST__LAMBDA(__VA_ARGS__), QTEST_SKIP_PARAM_ID, __LINE__)
#define EXPECT(a) Q_TEST__RETURN_IF_FALSE(Q_TEST_NS_DETAIL::BASE.expect((a), Q_TEST__STRINGIFY(a)))
#define INFO_PRINT(a) Q_TEST_NS_DETAIL::BASE.info_print(a)
#define TEST_FAILED(a) EXPECT(std::string{a}).fail();
#define TEST_SUCCEED() EXPECT(1).toBe(1)
#define SCENARIO_START Q_TEST__TEST_UNIT ([]{ Q_TEST_NS_DETAIL::BASE.script([]{
#define SCENARIO_END }); });

namespace Q_TEST_NS_DETAIL {
	// Initialize file local helpers
	inline QTestBase BASE;
}

/** ***********************USAGE****************************
 *
 * SCENARIO_START
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
 * SCENARIO_END
 *
 * ***********************************************************/

#endif // QTEST_H
