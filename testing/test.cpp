#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

void parse_error_page_line(std::string line, std::map<std::string, std::string>& _error_pages) {

    std::string status_code;
    std::string file_name;

    size_t pos = line.find(" ", 4);
    size_t endPos = line.find(" ", pos + 1);
    status_code = line.substr(pos + 1, endPos - (pos + 1));

    pos = endPos + 1;
    file_name = line.substr(pos, line.length() - pos - 1);
    _error_pages.insert(std::make_pair(status_code, file_name));
}


int main()
{
    std::map<std::string, std::string> myMap;
    std::string line = "    error_page 404 404.html;";

    parse_error_page_line(line, myMap);

    for (const auto& elem : myMap) {
        std::cout << "Status code: " << elem.first << '\n' << "File name: " << elem.second << '\n';
        std::cout << "Status code length: " << elem.first.length() << '\n' << "File name length: " << elem.second.length() << '\n';
    }

    return 0;
}