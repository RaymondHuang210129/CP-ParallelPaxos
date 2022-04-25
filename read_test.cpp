#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
# include "node.h"

int main(){
    std::vector<std::pair<std::string, int> > replicas;
    std::vector<std::pair<std::string, int> > leaders;
    std::vector<std::pair<std::string, int> > acceptors;
    read_config(replicas, leaders, acceptors);
    return 0;
}
