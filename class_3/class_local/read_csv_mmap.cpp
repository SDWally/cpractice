#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

std::vector<std::vector<std::string>> readCSV(const std::string& filename) {
    std::vector<std::vector<std::string>> data;

    // 打开文件
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return data;
    }

    // 获取文件大小
    struct stat fileStat;
    if (fstat(fd, &fileStat) == -1) {
        std::cerr << "Error getting file size: " << filename << std::endl;
        close(fd);
        return data;
    }
    size_t fileSize = fileStat.st_size;

    // 映射文件到内存
    char* fileData = static_cast<char*>(mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, fd, 0));
    if (fileData == MAP_FAILED) {
        std::cerr << "Error mapping file to memory: " << filename << std::endl;
        close(fd);
        return data;
    }

    // 读取CSV数据
    std::vector<std::string> row;
    std::string cell;
    bool inQuotes = false;

    for (size_t i = 0; i < fileSize; ++i) {
        char ch = fileData[i];

        if (ch == '"') {
            inQuotes = !inQuotes;
        } else if (ch == ',' && !inQuotes) {
            row.push_back(cell);
            cell.clear();
        } else if (ch == '\n' && !inQuotes) {
            row.push_back(cell);
            data.push_back(row);
            row.clear();
            cell.clear();
        } else {
            cell += ch;
        }
    }

    // 处理文件末尾可能的最后一行
    if (!cell.empty() || !row.empty()) {
        row.push_back(cell);
        data.push_back(row);
    }

    // 解除内存映射
    if (munmap(fileData, fileSize) == -1) {
        std::cerr << "Error unmapping file from memory: " << filename << std::endl;
    }

    // 关闭文件
    close(fd);

    return data;
}

int main() {
    std::string filename = "data.csv"; // 替换为你的CSV文件路径
    std::vector<std::vector<std::string>> data = readCSV(filename);

    // 打印读取的数据
    for (const auto& row : data) {
        for (const auto& cell : row) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}