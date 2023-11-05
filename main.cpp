#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <dlfcn.h>
#include <cstdlib>
#include <ctime>


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


typedef char* (*EncryptFunction)(char*, int);
typedef char* (*DecryptFunction)(char*, int);

int GenerateRandomKey(){
    return rand() %26 + 1;
}

int main() {

    srand(static_cast<unsigned int>(time(NULL)));

    void *library = dlopen("/Users/ostapturash/pp_4/caesar.dylib", RTLD_LAZY);

    if (!library) {
        std::cerr << "Unable to open the library" << dlerror() << std::endl;
        return 1;
    }

    dlerror();

    EncryptFunction encrypt = (EncryptFunction) dlsym(library, "encrypt");
    DecryptFunction decrypt = (DecryptFunction) dlsym(library, "decrypt");

    if (!encrypt || !decrypt) {
        std::cerr << "Failed to load the function" << dlerror() << std::endl;
        return 1;
    }

    FileReader fileReader;
    FileWriter fileWriter;

    std::string inputPath, outputPath;
    int key = 0;
    char operation;

    std::cout << "Welcome to the Caesar Cipher!" << std::endl;
    std::cout << "Choose mode (n for normal, s for secret): ";
    std::cin >> operation;

    std::cout << "Enter the file path to read: ";
    std::cin >> inputPath;

    std::cout << "Enter the file path to write: ";
    std::cin >> outputPath;

    std::string content;
    try {
        content = fileReader.read(inputPath);
    } catch (const std::exception &e) {
        std::cerr << "Error happened trying to open the file: " << e.what() << std::endl;
        dlclose(library);
        return 1;
    }

    char *buffer = strdup(content.c_str());

    if (operation == 'n') {
        std::cout << "Enter the key (1 - 26): ";
        std::cin >> key;
        if (key < 1 || key > 26) {
            std::cerr << "Invalid key" << std::endl;
            dlclose(library);
            return 1;
        }

        std::cout << "Choose operation (e for encryption and d for decryption): ";
        std::cin >> operation;
        if (operation == 'e') {
            char *encrypted = encrypt(buffer, key);
            if (encrypted) {
                content.assign(encrypted);
                free(encrypted);
            }
        } else if (operation == 'd') {
            char *decrypted = decrypt(buffer, key);
            if (decrypted) {
                content.assign(decrypted);
                free(decrypted);
            }
        } else {
            std::cerr << "Invalid operation" << std::endl;
            dlclose(library);
            return 1;
        }
    } else if (operation == 's') {
        key = GenerateRandomKey();
        std::cout << "Random key generated: " << key << std::endl;
        char *encrypted = encrypt(buffer, key);
        if (encrypted) {
            content.assign(encrypted);
            free(encrypted);
        }
    } else {
        std::cerr << "Invalid mode selected" << std::endl;
        dlclose(library);
        return 1;
    }

    try {
        fileWriter.write(outputPath, content);
    } catch (const std::exception &e) {
        std::cerr << "Error happened trying to write the file" << e.what() << std::endl;
        dlclose(library);
        return 1;
    }

    free(buffer);

    std::cout << "Operation completed successfully" << std::endl;
    dlclose(library);
    return 0;
}

