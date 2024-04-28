#include "loadtas.h"
#include "system_vars.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

std::vector<uint8_t> tas_inputs;

void load_tas(std::string filename) {
    std::ifstream file;
    file.open(filename);
    std::string input_line;
    if (!file.is_open()) {
        perror("Cannot read from file");
        exit(1);
    }
    while (file.good()) {
        std::getline(file, input_line);

        std::stringstream input_line_stream(input_line);
        std::string input;
        std::vector<std::string> input_list;
        uint8_t input_value = 0;

        while(std::getline(input_line_stream, input, ' '))
        {
            if (input == "A") {
                input_value += 1 << 0;
            }
            else if (input == "B") {
                input_value += 1 << 1;
            }
            else if (input == "SELECT") {
                input_value += 1 << 2;
            }
            else if (input == "START") {
                input_value += 1 << 3;
            }
            else if (input == "UP") {
                input_value += 1 << 4;
            }
            else if (input == "DOWN") {
                input_value += 1 << 5;
            }
            else if (input == "LEFT") {
                input_value += 1 << 6;
            }
            else if (input == "RIGHT") {
                input_value += 1 << 7;
            }
            else {
                std::cout << "TAS input \'" << input_line <<  "\' not recognized";
                exit(1);
            }
        }

        tas_inputs.push_back(input_value);

        if (file.eof()) {
            break;
        }
    }
}