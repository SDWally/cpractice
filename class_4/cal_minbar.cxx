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
        data.push_back(tickData);
    }
    file.close();
}


void generateKLineByTick(const std::vector<TickData>& data, std::map<std::string, std::vector<KLineData>>& kLines, int interval) {
    std::map<std::string, KLineData> currentKLine;
    for (const auto& tick : data) {
        int intervalStartTime = (tick.nTime / (100000)) / (interval) * (interval);
        if (currentKLine.find(tick.szWindCode) == currentKLine.end() || currentKLine[tick.szWindCode].startTime != intervalStartTime) {
            if (currentKLine.find(tick.szWindCode) != currentKLine.end()) {
                kLines[tick.szWindCode].push_back(currentKLine[tick.szWindCode]);
            }
            currentKLine[tick.szWindCode] = {tick.nMatch, tick.nMatch, tick.nMatch, tick.nMatch, intervalStartTime};
        } else {
            currentKLine[tick.szWindCode].high = std::max(currentKLine[tick.szWindCode].high, tick.nMatch);
            currentKLine[tick.szWindCode].low = std::min(currentKLine[tick.szWindCode].low, tick.nMatch);
            currentKLine[tick.szWindCode].close = tick.nMatch;
        }
    }
    for (auto& pair : currentKLine) {
        kLines[pair.first].push_back(pair.second);
    }
}


void generateKLineByKline(const std::map<std::string, std::vector<KLineData>>& data, std::map<std::string, std::vector<KLineData>>& kLines, int interval) {
    for (const auto& pair : data) {
        KLineData currentKLine;
        for (const auto& tick : pair.second) {
            int intervalStartTime = (tick.startTime / interval * interval);
            if (currentKLine.startTime != intervalStartTime) {
                kLines[pair.first].push_back(currentKLine);
                currentKLine = {tick.open, tick.high, tick.low, tick.close, intervalStartTime};
            } else {
                currentKLine.high = std::max(currentKLine.high, tick.high);
                currentKLine.low = std::min(currentKLine.low, tick.low);
                currentKLine.close = tick.close;
            }
        }
        kLines[pair.first].push_back(currentKLine);
    }
}



void writeCSV(const std::string& filename, const std::map<std::string, std::vector<KLineData>>& kLines) {
    std::ofstream file(filename);
    file << "szWindCode,startTime,open,high,low,close\n";
    for (const auto& pair : kLines) {
        for (const auto& kLine : pair.second) {
            file << pair.first << ","
                 << kLine.startTime << ","
                 << kLine.open << ","
                 << kLine.high << ","
                 << kLine.low << ","
                 << kLine.close << "\n";
        }
    }
}

int main() {
    std::vector<TickData> tickData;
    readCSV("input.csv", tickData);
    std::map<std::string, std::vector<KLineData>> kLines1min, kLines5min, kLines10min, kLines30min;
    std::vector<int> KlineNum({5, 10, 30});
    for (const auto& tick : tickData) {
        std::cout << "szWindCode: " << tick.szWindCode << ", "
                  << "nTradingDay: " << tick.nTradingDay << ", "
                  << "nTime: " << tick.nTime << ", "
                  << "nMatch: " << tick.nMatch << std::endl;
    }
    generateKLineByTick(tickData, kLines1min, 1);
    writeCSV("kline_1min.csv", kLines1min);
    for (const auto& num : KlineNum) {
        std::map<std::string, std::vector<KLineData>> kLinesNmin;
        generateKLineByKline(kLines1min, kLinesNmin, num);
        writeCSV("kline_" + std::to_string(num) + "min.csv", kLinesNmin);
    }
    return 0;
}