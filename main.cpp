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

std::string findValueByKey(const std::string& json, const std::string& key) {
    std::regex keyRegex("\"" + key + "\":\\s*\\{\\s*\"value\":\\s*(\\d+)");
    std::smatch match;

    if (std::regex_search(json, match, keyRegex)) {
        return match[1].str();
    }

    return "";
}

std::string fetchValueFromJSON(const std::wstring& url, const std::string& key) {
    HINTERNET hInternet = InternetOpen(L"Fetch JSON Example", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) {
        std::cerr << "Failed to initialize WinINet." << std::endl;
        return "";
    }

    HINTERNET hConnect = InternetOpenUrl(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        std::cerr << "Failed to open URL." << std::endl;
        InternetCloseHandle(hInternet);
        return "";
    }

    char buffer[1024];
    std::stringstream jsonStream;

    DWORD bytesRead;
    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        jsonStream.write(buffer, bytesRead);
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    std::string jsonData = jsonStream.str();
    size_t found = jsonData.find("{");
    if (found != std::string::npos) {
        jsonData = jsonData.substr(found);

        // Replace line breaks and tabs to make it a single line
        std::regex r("[\n\t]");
        jsonData = std::regex_replace(jsonData, r, "");

        // Find the value for the specified key
        return findValueByKey(jsonData, key);
    }
    return "";
}

void fetchOffsets() {
    const std::wstring urlOffsets = L"https://github.com/a2x/cs2-dumper/raw/main/generated/offsets.json";
    const std::wstring urlClientDLL = L"https://github.com/a2x/cs2-dumper/raw/main/generated/client.dll.json";

    std::vector<std::string> keysToFindOffsets = {
        "dwEntityList"
    };
    std::vector<std::string> keysToFindClientDLL = {
        "m_flDetectedByEnemySensorTime",
        "m_hPlayerPawn"
    };

    for (const auto& key : keysToFindOffsets) {
        std::string value = fetchValueFromJSON(urlOffsets, key);
        if (!value.empty()) {
             ptrdiff_t hexValue = std::stoll(value, 0, 0);
            std::cout << COLOR_GREEN << key << " (Offsets): " << COLOR_YELLOW << "0x" << std::uppercase << std::hex << hexValue << std::nouppercase << std::dec << std::endl;

            if (key == "dwEntityList") {
                offsets::p_entity_list = hexValue;
            }
        }
        else {
            std::cerr << COLOR_RED << "Key '" << key << "' (Offsets) not found in the JSON data." << std::endl;
        }
    }
    for (const auto& key : keysToFindClientDLL) {
        std::string value = fetchValueFromJSON(urlClientDLL, key);
        if (!value.empty()) {
            long long hexValue = std::stoll(value, 0, 0);
            std::cout << COLOR_GREEN << key << " (ClientDLL): " << COLOR_YELLOW << "0x" << std::uppercase << std::hex << hexValue << std::nouppercase << std::dec << std::endl;

            // Handle the specific key-value assignments here
            if (key == "dwEntityList") {
                offsets::p_entity_list = hexValue;
            }
            else if (key == "m_flDetectedByEnemySensorTime") {
                offsets::m_fl_detected_by_enemy_sensor_time = hexValue;
            }
            else if (key == "m_hPlayerPawn") {
                offsets::m_h_player_pawn = hexValue;
            }
        }
        else {
            std::cerr << COLOR_RED << "Key '" << key << "' (ClientDLL) not found in the JSON data." << std::endl;
        }
    }
}
