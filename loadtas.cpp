#include "loadtas.h"
#include "system_vars.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

std::vector<std::pair<uint64_t, uint8_t>> tas_inputs;

void load_tas(std::string filename) {
    std::ifstream file(filename);
    std::string input_line;
    if (!file.is_open()) {
        perror("Cannot read from file");
        exit(1);
    }
    while (file.good()) {
        std::getline(file, input_line);
        if (input_line.empty()) {
            break;
        }

        std::stringstream input_line_stream(input_line);
        std::string input;
        std::vector<std::string> input_list;
        uint8_t input_value = 0;

        // Frame number
        std::getline(input_line_stream, input, ' ');
        tas_inputs.push_back({stoi(input), 0});

        while (std::getline(input_line_stream, input, ' '))
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
                std::cout << "TAS input \'" << input <<  "\' not recognized";
                exit(1);
            }
        }

        tas_inputs.back().second = input_value;

        if (file.eof()) {
            break;
        }
    }
}

void write_tas(std::string filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        perror("Cannot write to file");
        exit(1);
    }
    for (auto input : tas_inputs) {
        std::string input_string = "";
        if ((input.second >> 0) & 1) {
            input_string += "A ";
        }
        if ((input.second >> 1) & 1) {
            input_string += "B ";
        }
        if ((input.second >> 2) & 1) {
            input_string += "SELECT ";
        }
        if ((input.second >> 3) & 1) {
            input_string += "START ";
        }
        if ((input.second >> 4) & 1) {
            input_string += "UP ";
        }
        if ((input.second >> 5) & 1) {
            input_string += "DOWN ";
        }
        if ((input.second >> 6) & 1) {
            input_string += "LEFT ";
        }
        if ((input.second >> 7) & 1) {
            input_string += "RIGHT ";
        }
        if (!input_string.empty()) {
            input_string.pop_back();
        }
        file << input.first << ' ' << input_string << '\n';
    }
}