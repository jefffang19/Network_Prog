#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <string.h>
#include <sstream> 
#include <unistd.h> //read(), write()
#include <fstream>
#include <vector>
//network libraries
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define ACPT_CLIENT 5 //number of clients a server can accept
#define BUFF_SIZE 512
using namespace std;

class Network{
public:
    void run_interface(int choice); //create an enviroment to let user type in command
    Network(){ child_cnt = 0; }
private:
    int client_fd; int server_fd; //file descripter of client socket and server socket
    int child_fd[ACPT_CLIENT]; //child create by accept()
    struct sockaddr_in cli_addr[ACPT_CLIENT]; //address of clients
    int child_cnt; //count how many client currently connect to server
    char buf[BUFF_SIZE]; // server: read client data.  client: send data, index [0, 1] is saved for : [0] = 0 and [1] = 0 => disconnect, [0] = 0 but [1] != 0 => not data end, else  => last packet, and it's data size
    void create_client_sock(uint16_t port);
    void create_server_sock(uint16_t port);
    void connect2srv(string ip, uint16_t port); //client connect to server
    void disconnect(); //close client connection to server
    void send_file(string filename);
    void listen2cli();
    void read_cli(); // read data from client
};

#endif