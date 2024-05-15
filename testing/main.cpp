#include <iostream>
#include <fstream>
#include <string>

void test(std::ifstream& tt)
{
    std::string tmp;
    std::getline(tt, tmp);
}

int main()
{
    std::ifstream file;

    file.open("./testing/file.txt");
    if (!file.is_open()) {
        std::cerr << "Error opening the file" << '\n';
    }

    test(file);

    std::string line;

    std::getline(file, line);
    std::cout << line << '\n';

    file.close();
    return 0;
}