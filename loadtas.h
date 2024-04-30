#pragma once

#include <vector>
#include <utility>
#include <string>
#include <cstdint>

extern std::vector<std::pair<uint64_t, uint8_t>> tas_inputs;
extern void load_tas(std::string filename);
extern void write_tas(std::string filename);
