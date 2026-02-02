#include "utils.h"
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>


// All time stamps are stored as LOCAL TIME for convenience
// Convert string time to time point
std::chrono::system_clock::time_point parseTime(const std::string& timeStr){
    std::istringstream ss(timeStr);
    std::tm tm = {};

    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    if (ss.fail()) {
        // Try without Z (new local format)
        ss.clear();
        ss.str(timeStr);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    }

    // check if parsing failed
    // If bad input, return "now" as safe fallback
    if (ss.fail()){
        std::cerr << "Warning: Failed to parse time:" << timeStr << ". Using current time instead.\n";
        return std::chrono::system_clock::now(); 
    }
    // Convert tm -> time_t -> time_point
    auto time_t_value = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(time_t_value);
}

// Convert timepoint to string time
std::string formatTime(const std::chrono::system_clock::time_point &Tmpt){
    // Convert time_point ->time_t -> tm
    std::time_t tim_t_value = std::chrono::system_clock::to_time_t(Tmpt);
    std::tm* tm = std::localtime(&tim_t_value);

    std::ostringstream ss;
    ss << std::put_time(tm, "%Y-%m-%dT%H:%M:%S");

    return ss.str();
}
