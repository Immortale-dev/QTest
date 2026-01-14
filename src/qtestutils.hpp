#ifndef QTESTUTILS_H
#define QTESTUTILS_H

#include <functional>

#define QTEST_TEST_PARAM_ID 0
#define QTEST_ONLY_PARAM_ID 1
#define QTEST_SKIP_PARAM_ID 2

#define Q_TEST__SCOPE &
#define Q_TEST__CONCAT_TOKENPASTE(a,b) a ## b
#define Q_TEST__CONCAT(a,b) Q_TEST__CONCAT_TOKENPASTE(a,b)
#define Q_TEST__STRINGIFY_TOKENPASTE(x) #x
#define Q_TEST__STRINGIFY(x) Q_TEST__STRINGIFY_TOKENPASTE(x)
#define Q_TEST__UNIQ_NAME_GENERATE(a) Q_TEST__CONCAT( Q_TEST__CONCAT(a, __LINE__), __COUNTER__)
#define Q_TEST__UNIQ_NAME() Q_TEST__UNIQ_NAME_GENERATE(Q___TEST___U_N_I_Q___)
#define Q_TEST__LAMBDA(...) [Q_TEST__SCOPE]()__VA_ARGS__
#define Q_TEST__LAMBDA_CALLBACK(...) [Q_TEST__SCOPE](auto callback){ do __VA_ARGS__ while((callback(), false)); }
#define Q_TEST__TEST_UNIT inline static Q_TEST_NS_DETAIL::QTestScenario Q_TEST__UNIQ_NAME()
#define Q_TEST__RETURN_IF_FALSE(...) for (int _some_val_=1,_result_val_=1;;) if (!_some_val_--) { if (!_result_val_) return; break; } else _result_val_ = __VA_ARGS__

namespace Q_TEST_NS_DETAIL {
	struct QTestScenario { QTestScenario(std::function<void()> fn) { fn(); } };
}

#endif //QTESTUTILS_H
