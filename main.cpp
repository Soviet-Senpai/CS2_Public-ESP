#include <Windows.h>
#include <stdio.h>
#include <TlHelp32.h>
#include <iostream>
#include <thread>
#include <wininet.h>
#include <string>
#include <sstream>
#include <regex>
#include <vector>
#include <map>
#include <iomanip>

namespace offsets
{
    ptrdiff_t p_entity_list = 0; 
    ptrdiff_t m_h_player_pawn = 0;
    ptrdiff_t m_fl_detected_by_enemy_sensor_time = 0;
}

struct JsonData {
    std::string key;
    long long value;
};

#pragma comment(lib, "wininet.lib")

const char* COLOR_RED = "\033[31m";
const char* COLOR_GREEN = "\033[32m";
const char* COLOR_YELLOW = "\033[33m";
const char* COLOR_BLUE = "\033[34m";
const char* COLOR_RESET = "\033[0m";
