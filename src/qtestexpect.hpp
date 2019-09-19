#ifndef QTESTEXPECT_H
#define QTESTEXPECT_H

template<typename T>
class QTestExpect
{
	public:
		
		QTestExpect(T a, bool& result) : val(a), result(result) {};
		
		template<typename CT>
		void toBe(CT compare);
		
	private:
		T val;
		bool& result;
};


template<typename T>
template<typename CT>
void QTestExpect<T>::toBe(CT compare)
{
	result &= (val == compare);
}


#endif // QTESTEXPECT_H
