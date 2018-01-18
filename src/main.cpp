#include <iostream>
#include "Famicom.h"

int main() {
    Famicom fc("./tests/nestest.nes");
	fc.Run();

    return 0;
}