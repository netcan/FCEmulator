#include <iostream>
#include "Famicom.h"

int main(int argc, char **argv) {
	if(argc > 1) {
		Famicom fc(argv[1]);
		fc.Run();
	}

    return 0;
}