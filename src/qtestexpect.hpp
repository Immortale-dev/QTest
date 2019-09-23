#ifndef QTESTEXPECT_H
#define QTESTEXPECT_H

#include <iostream>
#include <functional>
#include <cmath>
#include <vector>
#include <initializer_list>

template<typename T>
class QTestExpect
{
	public:
		QTestExpect(T a, bool* result) : val(a), result(result) {};
		void toBe(T compare);
		void toBeCloseTo(double compare, double precision);
		void toBeGreaterThan(T compare);
		void toBeGreaterThanOrEqual(T compare);
		void toBeLessThan(T compare);
		void toBeLessThanOrEqual(T compare);
		void toBeNull();
		void toBeNullPtr();
		template<typename CT>
		void toBeIteratableEqual(CT compare);
		template<typename CT>
		void toBeIteratableEqual(std::initializer_list<CT> list);
		void toThrowError();
		void toReturnTrue();
		QTestExpect<T> NOT();
		
	private:
		T val;
		bool* result;
		bool inv = false;
		bool proceed_result(bool result);
};


template<typename T>
void QTestExpect<T>::toBe(T compare)
{
	(*result) &= proceed_result(val == compare);
}

template<typename T>
void QTestExpect<T>::toBeCloseTo(double compare, double precision)
{
	double v = (double)val;
	(*result) &= proceed_result(std::abs(v-compare) <= std::abs(precision));
}

template<typename T>
void QTestExpect<T>::toBeGreaterThan(T compare)
{
	(*result) &= proceed_result(val > compare);
}

template<typename T>
void QTestExpect<T>::toBeGreaterThanOrEqual(T compare)
{
	(*result) &= proceed_result(val >= compare);
}

template<typename T>
void QTestExpect<T>::toBeLessThan(T compare)
{
	(*result) &= proceed_result(val < compare);
}

template<typename T>
void QTestExpect<T>::toBeLessThanOrEqual(T compare)
{
	(*result) &= proceed_result(val <= compare);
}

template<typename T>
void QTestExpect<T>::toBeNull()
{
	(*result) &= proceed_result(val == NULL);
}

template<typename T>
void QTestExpect<T>::toBeNullPtr()
{
	(*result) &= proceed_result(val == nullptr);
}

template<typename T>
template<typename CT>
void QTestExpect<T>::toBeIteratableEqual(CT compare)
{
	bool res = true;
	auto it=val.begin();
	auto itc=compare.begin();
	for(;it!=val.end() && itc!=compare.end();++it,++itc){
		res &= (*it) == (*itc);
	}
	res &= it == val.end();
	res &= itc == compare.end();
	(*result) = proceed_result(res);
}

template<typename T>
template<typename CT>
void QTestExpect<T>::toBeIteratableEqual(std::initializer_list<CT> list)
{
	toBeIteratableEqual(std::vector<CT>(list));
}

template<typename T>
void QTestExpect<T>::toThrowError()
{
	bool res = false;
	try{
		val();
	}catch(...){
		res = true;
	}
	(*result) &= proceed_result(res);
}

template<typename T>
void QTestExpect<T>::toReturnTrue()
{
	proceed_result(val());
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
	if(inv) 
		return !result;
	return result;
}

#endif // QTESTEXPECT_H
