#ifndef QTESTUTILS_H
#define QTESTUTILS_H

#define QTEST_TEST_PARAM_ID 0
#define QTEST_ONLY_PARAM_ID 1
#define QTEST_SKIP_PARAM_ID 2

#define Q_TEST__SCOPE &
#define Q_TEST__CONCAT_TOKENPASTE(a,b) a ## b
#define Q_TEST__CONCAT(a,b) Q_TEST__CONCAT_TOKENPASTE(a,b)
#define Q_TEST__UNIQ_NAME_GENERATE(a) Q_TEST__CONCAT( Q_TEST__CONCAT(a, __LINE__) , __COUNTER__)
#define Q_TEST__UNIQ_NAME() Q_TEST__UNIQ_NAME_GENERATE(___Q___TEST___U_N_I_Q___)
#define Q_TEST__DESCRIBE_STRUCT(...) QTestDescribe_t Q_TEST__UNIQ_NAME() (__VA_ARGS__)
#define Q_TEST__CALLBACK() QTestFinally<function_cb_t> Q_TEST__UNIQ_NAME() ([](){ QTESTOBJECTDEFINED.callback(); })
#define Q_TEST__CALLBACK_ARG() QTestFinally<function_cb_t> ([](){ QTESTOBJECTDEFINED.callback(); })
#define Q_TEST__DESCRIBE(...) Q_TEST__DESCRIBE_STRUCT(__VA_ARGS__)
#define Q_TEST__LAMBDA(...) [Q_TEST__SCOPE]()__VA_ARGS__
#define Q_TEST__LAMBDA_CALLBACK(...) Q_TEST__LAMBDA({ do __VA_ARGS__ while( (QTESTOBJECTDEFINED.callback(), false) ); })

template<typename T>
class QTestFinally
{
	T fn;
	public:
		QTestFinally(T fn) : fn(fn){};
		~QTestFinally(){ fn(); };
};

#endif //QTESTUTILS_H
