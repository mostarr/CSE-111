#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "Vstring.h"

using namespace std;

int main(int argc, char **argv) {
	Vstring vstring;
	std::cout << "Main.cpp" << std::endl;
	vstring.addChar('a');
	vstring.print();
	return 1;
}
