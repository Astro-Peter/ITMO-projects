#include <lib/parser.h>
#include <iostream>
#include <vector>
#include <sstream>

std::vector<std::string> GetArgAndName(std::string k) {

}

int main() {
    auto p = omfl::parse("C:\\Projects\\labwork-6-Astro-Peter\\example\\config.omfl");
    std::cout << p.valid() << '\n';
    std::cout << p.Get("servers.second.ip").AsString();
}
