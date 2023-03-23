#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <cstdlib>
#include <cstring>

#include <windows.h>

#define DEFAULT_KEY_SIZE 16

void generate_key(char* buffer, int size);

void xor_file(std::ifstream& input_file, std::ofstream& output_file, char* key, int keysize);

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cout << "Usage: encryptor.exe [keysize|decrypt] file\n";
        return 1;
    }
    std::vector<char> key;
    int keysize;
    std::ifstream input_file(argv[2], std::ios::binary);
    if (input_file.fail())
    {
        std::cout << "Cannot open file " << argv[2] << ".\n";
        return 2;
    }
    std::string output_file_name = argv[2];
    output_file_name.append("__tmp__");
    std::ofstream output_file(output_file_name, std::ios::binary);
    if (output_file.fail())
    {
        std::cout << "Cannot open temporary buffer file.\n";
        return 2;
    }
    if (std::strcmp(argv[1], "decrypt") == 0)
    {
        input_file.read((char*)&keysize, sizeof(int));
        key.resize(keysize);
        input_file.read(key.data(), keysize);
    }
    else
    {
        char* endptr = nullptr;
        int n = std::strlen(argv[1]);
        keysize = std::strtol(argv[1], &endptr, 0);
        if (endptr != argv[1] + n)
        {
            std::cout << "Invalid key size.\n";
            std::cout << "Using default key size of " << DEFAULT_KEY_SIZE << " bytes.\n";
            keysize = DEFAULT_KEY_SIZE;
        }
        key.resize(keysize);
        generate_key(key.data(), keysize);
        output_file.write((char*)&keysize, sizeof(keysize));
        output_file.write(key.data(), keysize);
    }

    xor_file(input_file, output_file, key.data(), keysize);

    input_file.close();
    output_file.close();

    DeleteFileA(argv[2]);
    MoveFileA(output_file_name.c_str(), argv[2]);

    return 0;
}

void generate_key(char* buffer, int size)
{
    std::default_random_engine eng;
    std::uniform_int_distribution<int> dist(0, 255);
    for (int i = 0; i < size; ++i)
    {
        buffer[i] = dist(eng);
    }
}

void xor_file(std::ifstream& input_file, std::ofstream& output_file, char* key, int keysize)
{
    int i = 0;
    char ch = input_file.get();
    while (input_file)
    {
        ch = ch ^ key[i];
        output_file.write(&ch, sizeof(ch));
        ++i;
        if (i == keysize)
            i = 0;
        ch = input_file.get();
    }
}
