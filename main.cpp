#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <dlfcn.h>

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

class IWriter{
public:
    virtual ~IWriter(){}
    virtual void write(const std::string& filePath, const std::string& content) = 0;
};

class FileWriter: public IWriter{
public:
    void write(const std::string& filePath, const std::string& content) override{
        std::ifstream infile(filePath);
        if (infile.good()){
            throw std::runtime_error("File already exists");
        }

        std::ofstream file(filePath, std::ios::binary);
        if (!file.is_open()){
            throw std::runtime_error("Unable to open the file");
        }

        file.write(content.c_str(), content.size());

        file.close();
    }
};

