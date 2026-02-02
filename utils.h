#pragma once

#include <chrono>
#include <string>

std::chrono::system_clock::time_point parseTime(const std::string& timeStr);
std::string formatTime(const std::chrono::system_clock::time_point &Tmpt);