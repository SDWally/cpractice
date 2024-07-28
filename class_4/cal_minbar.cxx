#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>

using namespace std;

struct TickData {
    std::string szWindCode;
    int nTradingDay;
    int nTime; // 假设时间已经转换为总毫秒数
    double nMatch;
};

struct KLineData {
    double open;
    double high;
    double low;
    double close;
    int startTime;
};

void readCSV(const std::string& filename,  std::vector<TickData>& data) {
    std::ifstream file(filename.c_str());
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
    }

    // Read the header line
    if (std::getline(file, line)) {
        // Skip the header line
    }

    // Read the data lines
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string token;
        TickData tickData;

        // Skip the first 35 columns
        for (int i = 0; i < 75; ++i) {
            if (!std::getline(ss, token, ',')) {
                std::cerr << "Error parsing line: " << line << std::endl;
                break;
            }
            if (i == 54) {
                tickData.nMatch = std::stod(token);
            }
            else if (i == 62) {
                tickData.nTime = std::stoi(token);
            }
            else if (i == 66) {
                tickData.nTradingDay = std::stoi(token);
            }
            else if (i == 72) {
                tickData.szWindCode = token;
            }
        }

        // // Read szWindCode (column 36)
        // if (std::getline(ss, token, ',')) tickData.szWindCode = token;

        // // Skip columns 37 to 48
        // for (int i = 0; i < 12; ++i) {
        //     if (!std::getline(ss, token, ',')) {
        //         std::cerr << "Error parsing line: " << line << std::endl;
        //         break;
        //     }
        // }

        // // Read nTradingDay (column 49)
        // if (std::getline(ss, token, ',')) tickData.nTradingDay = std::stoi(token);

        // // Skip columns 50 to 54
        // for (int i = 0; i < 5; ++i) {
        //     if (!std::getline(ss, token, ',')) {
        //         std::cerr << "Error parsing line: " << line << std::endl;
        //         break;
        //     }
        // }

        // // Read nTime (column 55)
        // if (std::getline(ss, token, ',')) tickData.nTime = std::stoi(token);

        // // Skip columns 56 to 55
        // for (int i = 0; i < 1; ++i) {
        //     if (!std::getline(ss, token, ',')) {
        //         std::cerr << "Error parsing line: " << line << std::endl;
        //         break;
        //     }
        // }

        // // Read nMatch (column 56)
        // if (std::getline(ss, token, ',')) tickData.nMatch = std::stod(token);

        data.push_back(tickData);
    }

    file.close();
}

// void readCSV(const std::string& filename, std::vector<TickData>& data) {
//     std::ifstream file(filename);
//     std::string line;
//     std::getline(file, line); // 跳过标题行
//     while (std::getline(file, line)) {
//         std::stringstream ss(line);
//         TickData tick;
//         std::getline(ss, tick.szWindCode, ',');
//         ss >> tick.nTradingDay;
//         ss.ignore();
//         ss >> tick.nTime;
//         ss.ignore();
//         ss >> tick.nMatch;
//         data.push_back(tick);
//     }
// }

// void generateKLine(const std::vector<TickData>& data, std::map<std::string, std::vector<KLineData>>& kLines, int interval) {
//     for (const auto& tick : data) {
//         static std::map<std::string, KLineData> currentKLine;
//         int intervalStartTime = (tick.nTime / (interval * 1000)) * (interval * 1000);
//         if (currentKLine.find(tick.szWindCode) == currentKLine.end() || currentKLine[tick.szWindCode].startTime != intervalStartTime) {
//             if (currentKLine.find(tick.szWindCode) != currentKLine.end()) {
//                 kLines[tick.szWindCode].push_back(currentKLine[tick.szWindCode]);
//             }
//             currentKLine[tick.szWindCode] = {tick.nMatch, tick.nMatch, tick.nMatch, tick.nMatch, intervalStartTime};
//         } else {
//             currentKLine[tick.szWindCode].high = std::max(currentKLine[tick.szWindCode].high, tick.nMatch);
//             currentKLine[tick.szWindCode].low = std::min(currentKLine[tick.szWindCode].low, tick.nMatch);
//             currentKLine[tick.szWindCode].close = tick.nMatch;
//         }
//     }
//     for (auto& pair : currentKLine) {
//         kLines[pair.first].push_back(pair.second);
//     }
// }

// void writeCSV(const std::string& filename, const std::map<std::string, std::vector<KLineData>>& kLines) {
//     std::ofstream file(filename);
//     file << "szWindCode,startTime,open,high,low,close\n";
//     for (const auto& pair : kLines) {
//         for (const auto& kLine : pair.second) {
//             file << pair.first << ","
//                  << kLine.startTime << ","
//                  << kLine.open << ","
//                  << kLine.high << ","
//                  << kLine.low << ","
//                  << kLine.close << "\n";
//         }
//     }
// }

int main() {
    std::vector<TickData> tickData;
    readCSV("input.csv", tickData);
    for (const auto& tick : tickData) {
        std::cout << "szWindCode: " << tick.szWindCode << ", "
                  << "nTradingDay: " << tick.nTradingDay << ", "
                  << "nTime: " << tick.nTime << ", "
                  << "nMatch: " << tick.nMatch << std::endl;
    }

    return 0;
    // std::map<std::string, std::vector<KLineData>> kLines1min, kLines5min, kLines10min, kLines30min;
    // generateKLine(data, kLines1min, 60);
    // generateKLine(data, kLines5min, 300);
    // generateKLine(data, kLines10min, 600);
    // generateKLine(data, kLines30min, 1800);

    // writeCSV("output_1min.csv", kLines1min);
    // writeCSV("output_5min.csv", kLines5min);
    // writeCSV("output_10min.csv", kLines10min);
    // writeCSV("output_30min.csv", kLines30min);

    return 0;
}