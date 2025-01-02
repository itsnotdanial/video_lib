#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "funcs.h"

int main() {
    std::cout << "Hello World" << std::endl;
    std::string test = "test.bin";
    std::cout << "Creating Funcs object" << std::endl;
    Funcs funcs(test);
    std::cout << "Calling reverse method" << std::endl;
    funcs.reverse();
    std::cout << "Finished reverse method" << std::endl;
    return 0;
}
