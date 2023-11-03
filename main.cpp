#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

class IReader{
public:
    virtual ~IReader(){}
    virtual std::string read(const std::string& filePath) = 0;
};

class FileReader: public IReader{
public:
    std::string read(const std::string& filePath) override{
        std:: ifstream file(filePath, std::ios::binary);
        if (!file.is_open()){
            throw std::runtime_error("Error happened trying to open the file");
        }

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        file.close();

        return content;
    }
};
