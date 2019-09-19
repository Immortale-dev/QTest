#ifndef QTESTEXPECT_H
#define QTESTEXPECT_H

template<typename T>
class QTestExpect
{
	public:
		
		QTestExpect(T a, bool& result) : val(a), result(result) {};
		
		void toBe(T compare);
		
	private:
		T val;
		bool& result;
};


template<typename T>
void QTestExpect<T>::toBe(T compare)
{
	result &= (val == compare);
}




#endif // QTESTEXPECT_H
