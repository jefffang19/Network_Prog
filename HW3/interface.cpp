#include <iostream>
#include <string.h>
#include "network.h"
using namespace std;

Network network; //all the network functions

/************************************************/
// Main
/************************************************/
int main(int argv, char ** argc){
    //check if run as server or client
    if(argv != 2){
        cout << "Please choose to run as client or server by \"[program name] client/server\"\n";
        return 0;
    }
    if(strcmp(argc[1],"client") == 0){
        network.run_interface(0);
    }
    else if(strcmp(argc[1],"server") == 0){
       network.run_interface(1);
    }
    return 0;
}