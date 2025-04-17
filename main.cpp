#include <iostream>
#include <string>
#include "temp.h"
using namespace std;

int main()
{
	temp::menu();
	std::string ss;
	getline(cin, ss);
	
	
	std::cout << ss << "\n";
}