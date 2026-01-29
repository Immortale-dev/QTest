#ifndef QTESTEXPECT_H
#define QTESTEXPECT_H

#include <vector>
#include <cmath>
#include <string_view>
#include <initializer_list>
#include <type_traits>
#include <iomanip>

namespace Q_TEST_NS_DETAIL {

struct ErrorReport {
	std::string value = {};
	std::string compare = {};
	std::string_view func = {};
	bool inverse = false;
	bool has_compare = false;
	bool value_substituted = false;
	bool compare_substituted = false;
};

template <typename T, typename = void>
struct is_streamable : std::false_type {};

struct NonStreamable{};

template <typename T>
struct is_streamable<T, std::enable_if_t<
	std::is_convertible_v<
		decltype(std::declval<std::ostream &>() << std::declval<T>()),
		std::ostream &
	>
>> : std::true_type {};

template<typename T>
class QTestExpect
{
	public:
		QTestExpect(T&& a, bool* result, ErrorReport* error)
			: val(std::move(a)), result(result), error(error) {};
		QTestExpect(T& a, bool* result, ErrorReport* error)
			: val(a), result(result), error(error) {};
		bool toBe(T&& compare);
		bool toBe(T& compare);
		template<typename C> bool toBe(C&& compare);
		bool toBeCloseTo(T compare, T precision);
		bool toBeGreaterThan(T compare);
		bool toBeGreaterThanOrEqual(T compare);
		bool toBeLessThan(T compare);
		bool toBeLessThanOrEqual(T compare);
		bool toBeNull();
		bool toBeNullPtr();
		template<typename CT> bool toBeIterableEqual(CT&& compare);
		template<typename CT> bool toBeIterableEqual(std::initializer_list<CT> list);
		bool toThrowError();
		template<typename E> bool toThrowError();
		bool toReturnTrue();
		template<typename V> bool toReturn(V&& compare);
		QTestExpect<T> NOT();
		bool fail();

		//Aliases
		bool to_be(T&& compare) { return toBe(std::move(compare)); }
		bool to_be(T& compare) { return toBe(compare); }
		template<typename C> bool to_be(C&& compare){ return toBe(std::move(compare)); }
		bool to_be_close_to(T compare, T precision){ return toBeCloseTo(compare, precision); }
		bool to_be_greater_than(T compare){ return toBeGreaterThan(compare); }
		bool to_be_greater_than_or_equal(T compare){ return toBeGreaterThanOrEqual(compare); }
		bool to_be_less_than(T compare){ return toBeLessThan(compare); }
		bool to_be_less_than_or_equal(T compare){ return toBeLessThanOrEqual(compare); }
		bool to_be_null(){ return toBeNull(); }
		bool to_be_null_ptr(){ return toBeNullPtr(); }
		template<typename CT> bool to_be_iterable_equal(CT&& compare){ return toBeIterableEqual(std::forward(compare)); }
		template<typename CT> bool to_be_iterable_equal(std::initializer_list<CT> list){ return toBeIterableEqual(list); }
		bool to_throw_error(){ return toThrowError(); }
		template<typename E> bool to_throw_error(){ return toThrowError<E>(); }
		bool to_return_true(){ return toReturnTrue(); }
		template<typename V> bool to_return(V&& compare) { return toReturn(compare); }

	private:
		bool proceed_result(bool result);
		void report_error_resolved(std::string_view func, std::string_view value, std::string_view compare);
		template<typename V, typename C> void report_error(std::string_view func, V&& value, C&& compare);
		template<typename V> void report_error(std::string_view func, V&& value);
		template<typename CT> std::string iterable_to_str(CT&& value);
		template<typename CT> std::string streamable_to_str(CT&& value);

		T val;
		bool* result;
		ErrorReport* error;
		bool inv = false;
};

template<typename T>
bool QTestExpect<T>::toBe(T&& compare)
{
	if (!(*result &= proceed_result(val == compare))) {
		report_error(__func__, val, compare);
	}
	return *result;
}

template<typename T>
bool QTestExpect<T>::toBe(T& compare)
{
	if (!(*result &= proceed_result(val == compare))) {
		report_error(__func__, val, compare);
	}
	return *result;
}

template<typename T>
template<typename C>
bool QTestExpect<T>::toBe(C&& compare)
{
	if (!(*result &= proceed_result(val == compare))) {
		report_error(__func__, val, compare);
	}
	return *result;
}

template<typename T>
bool QTestExpect<T>::toBeCloseTo(T compare, T precision)
{
	T v = val;
	if (!(*result &= proceed_result(std::abs(v-compare) <= std::abs(precision)))) {
		report_error_resolved(__func__, streamable_to_str(val), streamable_to_str(compare) + ", " + streamable_to_str(precision));
	}
	return *result;
}

template<typename T>
bool QTestExpect<T>::toBeGreaterThan(T compare)
{
	if (!(*result &= proceed_result(val > compare))) {
		report_error(__func__, val, compare);
	}
	return *result;
}

template<typename T>
bool QTestExpect<T>::toBeGreaterThanOrEqual(T compare)
{
	if(!(*result &= proceed_result(val >= compare))) {
		report_error(__func__, val, compare);
	}
	return *result;
}

template<typename T>
bool QTestExpect<T>::toBeLessThan(T compare)
{
	if(!(*result &= proceed_result(val < compare))) {
		report_error(__func__, val, compare);
	}
	return *result;
}

template<typename T>
bool QTestExpect<T>::toBeLessThanOrEqual(T compare)
{
	if(!(*result &= proceed_result(val <= compare))) {
		report_error(__func__, val, compare);
	}
	return *result;
}

template<typename T>
bool QTestExpect<T>::toBeNull()
{
	if(!(*result &= proceed_result(val == NULL))) {
		report_error(__func__, val);
	}
	return *result;
}

template<typename T>
bool QTestExpect<T>::toBeNullPtr()
{
	if(!(*result &= proceed_result(val == nullptr))) {
		report_error(__func__, val);
	}
	return *result;
}

template<typename T>
template<typename CT>
bool QTestExpect<T>::toBeIterableEqual(CT&& compare)
{
	bool res = true;
	auto it=val.begin();
	auto itc=compare.begin();
	for(;it!=val.end() && itc!=compare.end();++it,++itc){
		res &= (*it) == (*itc);
	}
	res &= it == val.end();
	res &= itc == compare.end();
	if(!(*result &= proceed_result(res))) {
		using iter_value_t = decltype(*std::begin(compare));
		if constexpr (is_streamable<iter_value_t>::value) {
			report_error_resolved(__func__, iterable_to_str(val), iterable_to_str(compare));
		} else {
			report_error(__func__, val, compare);
		}
	}
	return *result;
}

template<typename T>
template<typename CT>
bool QTestExpect<T>::toBeIterableEqual(std::initializer_list<CT> list)
{
	return toBeIterableEqual(std::vector<CT>(list));
}

template<typename T>
bool QTestExpect<T>::toThrowError()
{
	bool res = false;
	try{
		val();
	}catch(...){
		res = true;
	}
	if(!(*result &= proceed_result(res))) {
		report_error(__func__, val);
	}
	return *result;
}

template<typename T>
template<typename E>
bool QTestExpect<T>::toThrowError()
{
	bool res = false;
	try{
		val();
	} catch(E) {
		res = true;
	}
	if(!(*result &= proceed_result(res))) {
		report_error(__func__, val);
	}
	return *result;
}

template<typename T>
bool QTestExpect<T>::toReturnTrue()
{
	if(!(*result &= proceed_result(val()))) {
		report_error(__func__, val);
	}
	return *result;
}

template<typename T>
template<typename V>
bool QTestExpect<T>::toReturn(V&& compare)
{
	if(!(*result &= proceed_result(val() == compare))) {
		report_error(__func__, val, compare);
	}
	return *result;
}

template<typename T>
bool QTestExpect<T>::fail()
{
	*result = 0;
	if (val.size()) {
		report_error(__func__, val);
	} else {
		report_error<NonStreamable>(__func__, {});
	}
	return *result;
}

template<typename T>
QTestExpect<T> QTestExpect<T>::NOT()
{
	inv = true;
	return *this;
}

template<typename T>
bool QTestExpect<T>::proceed_result(bool result)
{
	if(inv) return !result;
	return result;
}

template<typename T>
void QTestExpect<T>::report_error_resolved(std::string_view func, std::string_view value, std::string_view compare)
{
	error->func = func;
	error->inverse = inv;
	error->has_compare = true;
	error->value_substituted = true;
	error->value = value;
	error->compare_substituted = true;
	error->compare = compare;
}

template<typename T>
template<typename V, typename C>
void QTestExpect<T>::report_error(std::string_view func, V&& value, C&& compare)
{
	error->func = func;
	error->inverse = inv;
	error->has_compare = true;
	if constexpr (is_streamable<V>::value) {
		error->value_substituted = true;
		error->value = streamable_to_str(value);
	}
	if constexpr (is_streamable<C>::value) {
		error->compare_substituted = true;
		error->compare = streamable_to_str(compare);
	}
}

template<typename T>
template<typename V>
void QTestExpect<T>::report_error(std::string_view func, V&& value)
{
	error->func = func;
	error->inverse = inv;
	if constexpr (is_streamable<V>::value) {
		error->value = streamable_to_str(value);
		error->value_substituted = true;
	}
}

template<typename T>
template<typename CT>
std::string QTestExpect<T>::iterable_to_str(CT&& value)
{
	std::stringstream ss;
	std::string value_str = "{";
	for (auto &v : value) {
		ss << v << ", ";
	}
	value_str += std::move(ss).str();
	if (value.size()) {
		// Remove last delimeter and space.
		value_str.pop_back();
		value_str.pop_back();
	}
	value_str += "}";
	return value_str;
}

template<typename T>
template<typename CT>
std::string QTestExpect<T>::streamable_to_str(CT&& value)
{
	std::stringstream ss;
	ss << value;
	if constexpr (std::is_convertible_v<CT, std::string_view>) {
		return streamable_to_str(std::quoted(std::move(ss).str()));
	} else {
		return std::move(ss).str();
	}
}

} // Q_TEST_NS_DETAIL

#endif // QTESTEXPECT_H
