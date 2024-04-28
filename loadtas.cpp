#include "loadtas.h"
#include "system_vars.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

std::vector<Joypad> tas_inputs;

void load_tas(std::string filename) {
    std::ifstream file;
    file.open(filename);
    std::string input_line;
    if (!file.is_open()) {
        perror("Cannot read from file");
        exit(-1);
    }
    while (file.good()) {
        std::getline(file, input_line);
        if (input_line == "\n") {
            break;
        }
        std::cout << input_line << '\n';
    }
}