#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>


struct TickData {
    std::string szWindCode;
    int nTradingDay;
    int nTime; // 毫秒,如 93001000
    double nMatch;
};

struct KLineData {
    double open;
    double high;
    double low;
    double close;
    int startTime;   // 形如1030的分钟整型
    int minuteTime;  // 将小时转化为分钟的整型，便于采样
};

// 1130 —> 11 * 60 + 30
int sixtyToDecimal(int sixtyTime) {
    return (sixtyTime / 100) * 60 + (sixtyTime % 100);
}
// 11 * 60 + 30 -> 1130
int DecimalTosixty(int DecimalTime) {
    return (DecimalTime / 60) * 100 + (DecimalTime % 60);
}


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

    std::vector<std::string> lines;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    // // Read the data lines
    // while (std::getline(file, line)) {
    //     std::istringstream ss(line);
    //     std::string token;
    //     TickData tickData;
    //     for (int i = 0; i < 75; ++i) {
    //         if (!std::getline(ss, token, ',')) {
    //             std::cerr << "Error parsing line: " << line << std::endl;
    //             break;
    //         }
    //         if (i == 54) {
    //             tickData.nMatch = std::stod(token);
    //         }
    //         else if (i == 62) {
    //             tickData.nTime = std::stoi(token);
    //         }
    //         else if (i == 66) {
    //             tickData.nTradingDay = std::stoi(token);
    //         }
    //         else if (i == 72) {
    //             tickData.szWindCode = token;
    //         }
    //     }
    //     data.push_back(tickData);
    // }
    file.close();
}



// void readCSV(const std::string& filename, std::vector<TickData>& data) {
//     std::ifstream file(filename.c_str());
//     std::string line;

//     if (!file.is_open()) {
//         std::cerr << "Failed to open file: " << filename << std::endl;
//         return;
//     }

//     // Read the header line
//     if (std::getline(file, line)) {
//         // Skip the header line
//     }

//     // Reserve memory for data
//     data.reserve(5000000); // Adjust the size based on your expected number of lines

//     // Read the data lines
//     while (std::getline(file, line)) {
//         std::istringstream ss(line);
//         std::string token;
//         TickData tickData = {"", 0, 0, 0};
//         int i = 0;
//         while (std::getline(ss, token, ',')) {
//             if (i == 54) {
//                 tickData.nMatch = std::stod(token);
//             } else if (i == 62) {
//                 tickData.nTime = std::stoi(token);
//             } else if (i == 66) {
//                 tickData.nTradingDay = std::stoi(token);
//             } else if (i == 72) {
//                 tickData.szWindCode = std::move(token); // Use move to avoid copy
//             }
//             ++i;
//         }
//         data.push_back(std::move(tickData)); // Use move to avoid copy
//     }
//     file.close();
// }


void generateKLineByTick(const std::vector<TickData>& data, std::unordered_map<std::string, std::vector<KLineData>>& kLines, int interval) {
    std::unordered_map<std::string, KLineData> currentKLine;
    for (const auto& tick : data) {
        int intervalStartTime = (tick.nTime / (100000)) / (interval) * (interval);
        if (intervalStartTime < 930 || intervalStartTime > 1500) {
            continue;
        }
        int intervalMinuteTime = sixtyToDecimal(intervalStartTime);
        if (currentKLine.find(tick.szWindCode) == currentKLine.end() || currentKLine[tick.szWindCode].startTime != intervalStartTime) {
            if (currentKLine.find(tick.szWindCode) != currentKLine.end()) {
                kLines[tick.szWindCode].push_back(currentKLine[tick.szWindCode]);
            }
            currentKLine[tick.szWindCode] = {tick.nMatch, tick.nMatch, tick.nMatch, tick.nMatch, intervalStartTime, intervalMinuteTime};
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


void generateKLineByKline(const std::unordered_map<std::string, std::vector<KLineData>>& data, std::unordered_map<std::string, std::vector<KLineData>>& kLines, int interval) {
    for (const auto& pair : data) {
        // 第一个元素初始化当前K线；
        KLineData currentKLine(pair.second[0]);
        for (const auto& tick : pair.second) {
            int intervalMinuteTime = (tick.minuteTime / interval * interval);
            int intervalStartTime = DecimalTosixty(intervalMinuteTime);
            if (currentKLine.minuteTime != intervalMinuteTime) {
                kLines[pair.first].push_back(currentKLine);
                currentKLine = {tick.open, tick.high, tick.low, tick.close, intervalStartTime, intervalMinuteTime};
            } else {
                currentKLine.high = std::max(currentKLine.high, tick.high);
                currentKLine.low = std::min(currentKLine.low, tick.low);
                currentKLine.close = tick.close;
            }
        }
        kLines[pair.first].push_back(currentKLine);
    }
}


void writeCSV(const std::string& filename, std::unordered_map<std::string, std::vector<KLineData>>& kLines) {
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


void generateKline(const std::string& filename) {
    std::vector<TickData> tickData;
    std::cout << "start read csv " << filename << std::endl;
    readCSV(filename, tickData);
    // kline 1
    std::cout << "generating kline 1min" << std::endl;
    std::unordered_map<std::string, std::vector<KLineData>> kLines1min;
    generateKLineByTick(tickData, kLines1min, 1);
    writeCSV("kline_1min.csv", kLines1min);
    // kline 5, 10, 30
    std::vector<int> KlineNum({5, 10, 30});
    for (const auto& num : KlineNum) {
        std::cout << "generating kline " << std::to_string(num) << "min" << std::endl;
        std::unordered_map<std::string, std::vector<KLineData>> kLinesNmin;
        generateKLineByKline(kLines1min, kLinesNmin, num);
        writeCSV("kline_" + std::to_string(num) + "min.csv", kLinesNmin);
    }
    std::cout << "success." << std::endl;
}

int main() {
    std::string filename = "md_20221110.csv";
    // 开始计时
    auto start = std::chrono::high_resolution_clock::now();

    generateKline(filename);

    // 结束计时
    auto end = std::chrono::high_resolution_clock::now();

    // 计算执行时间
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;

    return 0;
}