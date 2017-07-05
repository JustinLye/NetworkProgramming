#include<iostream>
#include<fstream>
#include"../include/Message.h"

int main(int argc, char* argv[])
{
    std::ofstream Test_file;
    Test_file.open(argv[1]);
    if(Test_file.rdstate() != std::ios_base::goodbit) {
        jl::System_error_message Sys_error(jl::Message_source(__FILE__, __FUNCTION__, __LINE__), errno);
        std::cout << Sys_error << std::endl;
    }
    Test_file.close();
    return 0;
}
