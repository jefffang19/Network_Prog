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
#define SERVER_PORT 1234
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
    char buf[512]; // server: read client data.  client: send data, index 0 is saved for : -1 = disconnect, 0 = not data end, 1 = dataend
    void create_client_sock(uint16_t port);
    void create_server_sock(uint16_t port);
    void connect2srv(string ip, uint16_t port); //client connect to server
    void disconnect(); //close client connection to server
    void send_file(string filename);
    void listen2cli();
    int read_cli(); // read from client, return data length
}network;

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

 void Network::run_interface(int choice){
     // user input
    string userin, temp;
     switch(choice){
         // 0 as client
         case 0:
            // create client socket
            cout << "Client Port Number(at least 4 digits): ";
            uint16_t cli_port;
            cin >> cli_port; 
            getchar(); //remove \n
            create_client_sock(cli_port);
            cout << "Client ip: 127.0.0.1 port: " << cli_port << "\n";
            while(1){
                cout << "[student @ CSE ~ ]$ ";
                getline(cin,userin);
                stringstream ss(userin);
                ss >> temp;
                 if(temp == "connect"){
                     u_int16_t port;
                     ss >> temp;
                     ss >> port;
                    connect2srv(temp,port);
                 }
                else if(temp == "upload"){
                    ss >> temp;
                    send_file(temp);
                }
                else if(temp == "goodbye"){
                    disconnect();
                    cout << "See you next time.\n";
                    return;
                }
                else{
                    cout << "Unknown Command: " << temp << endl;
                }
            }
            return;
        // 1 as server
        case 1:
            // create server socket
            network.create_server_sock(SERVER_PORT);
            cout << "[TA @ CSE ~]  Server startup\n";
            cout << "[TA @ CSE ~]  Server ip: 127.0.0.1 port: " << SERVER_PORT << "\n";
            cout << "[TA @ CSE ~]  Listening ...\n";
            while(1){
                listen2cli();
                //create a child process to handle connection with client
                pid_t pid;
                pid = fork();
                if(pid == 0)
                    while(1) read_cli();
            }
            return;
     }
 }


void Network::create_client_sock(uint16_t port){
    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(client_fd < 0){
        cerr << "Client socket create error\n";
        exit(1);
    }
    else cout << "Client socket create successful\n";

    //bind port
    struct sockaddr_in cli;

    cli.sin_family = AF_INET; //internet address family (ipv4)
    cli.sin_port = htons(port);
    cli.sin_addr.s_addr = inet_addr("127.0.0.1");

    //bind port
    if(bind(client_fd, (struct sockaddr*) &cli, sizeof(cli)) < 0){
        cerr << "bind client socket error\n"; exit(1);
    }
    else cout << "bind client socket successfully\n";

}


void Network::create_server_sock( uint16_t port){
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(server_fd < 0){
        cerr << "[TA @ CSE ~]  Create server socket error\n";
    }
   struct sockaddr_in srv;

    srv.sin_family = AF_INET; //internet address family (ipv4)
    srv.sin_port = htons(port);
    srv.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind port
    if(bind(server_fd, (struct sockaddr*) &srv, sizeof(srv)) < 0){
        cerr << "[TA @ CSE ~] bind server socket error\n"; exit(1);
    }
    else cout << "[TA @ CSE ~] bind server socket successfully\n";
 }

 void Network::connect2srv(string ip, uint16_t port){
     struct sockaddr_in srv;

     srv.sin_family = AF_INET;
     srv.sin_port = htons(port); // target server's port
     srv.sin_addr.s_addr = inet_addr(ip.c_str()); // target server's ip

     cout << "debug: ip.c_str() == " << ip.c_str() << endl;

     if( connect(client_fd, (struct sockaddr *)&srv, sizeof(srv)) < 0){
         cerr << "connect to server " << ip << ":" << port << " error\n";
     }
     else cout << "connect to server " << ip << ":" << port << " success\n";
 }

void Network::send_file(string filename){
    ifstream infile;
    infile.open(filename, ios::in | ios::binary);

    char c;
    vector<unsigned char> in_data;

    while(!infile.eof()){
        infile.get(c);
        in_data.push_back(c);
    }
    cout << "debug data size: " << in_data.size() << endl;

    //copy to buffer
    for(int i = 0 ; i < in_data.size() ; ++ i) buf [i]= in_data[i];

    int nbytes; // use to check how many bytes had been writen
    if( (nbytes = write(client_fd, buf, sizeof(buf)))  < 0){
        cerr << "Write to client socket Error\n";
        exit(1);
    }
    return;
}

void Network::listen2cli(){
    if(listen(server_fd, ACPT_CLIENT) < 0){
        cerr << "Listen for client error\n";
        exit(1);
    }

    // accpet procedure
    struct sockaddr_in cli; // info regarding incoming client
    int cli_len = sizeof(cli);

    // blocking function
    child_fd[child_cnt] = accept(server_fd, (struct sockaddr *) &cli, (socklen_t *) &cli_len);
    if(child_fd[child_cnt] < 0){
        cerr << "Accept client error\n";
        exit(1);
    }
    else{
        cli_addr[child_cnt] = cli;

         ++child_cnt;

         cout << "A Client " << inet_ntoa(cli.sin_addr) << " has connected via port num " << ntohs(cli.sin_port) << " using SOCK_STREAM (TCP)\n";
    }

}

int Network::read_cli(){
    int nbytes; //count data length from client
    if( (nbytes = read(child_fd[child_cnt-1], buf, sizeof(buf))) < 0){
        cerr << "Read from client Error\n";
        exit(1);
    }
    //check if disconnect signal
    if(buf[0] == (char)(-1)){
        cout << "Client " << inet_ntoa(cli_addr[child_cnt-1].sin_addr) << " with port num " << ntohs(cli_addr[child_cnt-1].sin_port) << "has disconnected\n";
        exit(1);
    }
    //debug
    cout << "Debug: " << buf << endl;
    return nbytes;
}

void Network::disconnect(){
     buf[0]= (char)(-1);

    int nbytes; // use to check how many bytes had been writen
    if( (nbytes = write(client_fd, buf, sizeof(buf)))  < 0){
        cerr << "Close socket Error\n";
        exit(1);
    }

    if( close(client_fd) != 0){
        cerr << "Close client socket failed\n";
    }
    else cout << "Client Socket Closed\n";
} 