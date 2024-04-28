#include "loadtas.h"
#include "system_vars.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

std::vector<int> tas_inputs;

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
        if (input_line == "A") {
            tas_inputs.push_back(0);
        }
        else if (input_line == "B") {
            tas_inputs.push_back(1);
        }
        else if (input_line == "SELECT") {
            tas_inputs.push_back(2);
        }
        else if (input_line == "START") {
            tas_inputs.push_back(3);
        }
        else if (input_line == "UP") {
            tas_inputs.push_back(4);
        }
        else if (input_line == "DOWN") {
            tas_inputs.push_back(5);
        }
        else if (input_line == "LEFT") {
            tas_inputs.push_back(6);
        }
        else if (input_line == "RIGHT") {
            tas_inputs.push_back(7);
        }
        else {
            std::cout << "TAS input \'" << input_line <<  "\' not recognized";
            exit(1);
        }
        
        std::cout << input_line << '\n';

        if (file.eof()) {
            break;
        }
    }
}