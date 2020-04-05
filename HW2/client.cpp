#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
using namespace std;

class Network{
public:
    void run_interface(); //create an enviroment to let user type in command

//private:
    int client_fd; int server_fd; //file descripter of client socket and server socket
    void create_client_sock(uint16_t port);
    void create_server_sock(uint16_t port);
    void connect(string ip, uint16_t port); //client connect to server
    void disconnect();
    void send_file(string filename);
};

int main(){
    Network network;
    //network.create_client_sock(1256);
    network.create_server_sock(1256);
}

void Network::create_client_sock(uint16_t port){
    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(client_fd < 0){
        cerr << "Client socket create error\n";
        exit(1);
    }
}


void Network::create_server_sock( uint16_t port){
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(server_fd < 0){
        cerr << "Create server socket error\n";
    }
void connect(string ip, uint16_t port);void connect(strivoid connect(string ip, uint16_t port);ng ip, uint16_t port);
    struct sockaddr_in srv;

    srv.sin_family = AF_INET; //internet address family (ipv4)
    srv.sin_port = htons(port);
    srv.sin_addr.s_addr = INADDR_ANY;

    //bind port
    if(bind(server_fd, (struct sockaddr*) &srv, sizeof(srv)) < 0){
        cerr << "bind server socket error\n"; exit(1);
    }
    else cout << "bind server socket successfully\n";
 }

 void Network::connect(string ip, uint16_t port){
     struct sockaddr_in srv;

     srv.sin_family = AF_INET;
     srv.sin_port = htons(port); // target server's port
     srv.sin_addr.s_addr = inet_addr(ip.c_str()); // target server's ip

     if( connect(client_fd, (struct sockaddr *)&srv, sizeof(srv)) < 0){
         cerr << "connect to server " << ip << ":" << port << " error\n";
     }
     else cout << "connect to server " << ip << ":" << port << " success\n";
 }