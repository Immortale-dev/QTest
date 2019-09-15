#include <iostream>

#include "qtest.h"

using namespace std;

int main(){
	
	
	cout << "Hello World" << endl;
	
	int a = 2;
	
	DESCRIBE("TEST", {
		cout << "Describe works fine!" << a << endl;
		
		DESCRIBE("ANOTHER TEST", {
		
			cout << "AWESOME!" << a << endl;
			
		});
	});
	
	return 0;
}
