#include "network.h"

//for share memory
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <sstream>
#include <signal.h>

#define STDIN 0 // standard input 's file descriptor

char idle_str[] = "<idle>";

//return share memory of size "size"
void* create_sharemem(size_t size);
//addr of share memory with childs
void *share_mem[ACPT_CLIENT];

 void Network::run_interface(int choice){

    time_t rawtime;
    time (&rawtime);
    
     // user input
    string userin, temp;
    switch(choice){
         // 0 as client
         case 0:
            // create client socket
            create_client_sock();
            
            //connect to srv
            while(1){
                cout << "[client @ CSE ~ ]$ ";
                getline(cin,userin);
                stringstream ss(userin);
                ss >> temp;
                if(temp == "connect"){
                    u_int16_t port;
                    string uname;
                    ss >> temp;
                    ss >> port;
                    ss >> uname;
                    connect2srv(temp,port,uname); 
                    break;
                }
                else { cout << "Please connect to server first! \n"; }
            }

            //fork a child to listen to server's msg
            pid_t pid1; // distinguish parent
            pid1 = fork();
            if(pid1 == -1){
                cerr << "fork failed!\n";
                exit(1);
            }

            //read user input and send to server
            if(pid1 != 0){
                while(1){
                    //cout << "[client @ CSE ~ ]$ ";
                    getline(cin,userin);
                    stringstream ss(userin);
                    ss >> temp;
                    if(temp == "chat"){
                        string cmb = "";
                        while(ss >> temp){
                            cmb = cmb + temp + " ";
                        }
                        send_msg(client_fd ,cmb);
                    }
                    else if(temp == "bye"){
                        disconnect();
                        cout << "See you next time.\n";
                        //kill child process
                        kill(pid1, SIGKILL);
                        return;
                    }
                    else{
                        cout << "Unknown Command: " << temp << endl;
                    }
                }
            }
            //read msg from srv
            else if(pid1 == 0){
                while(1){
                    string srv_msg = read_srv();
                    cout << "\n<MSG FROM SEVER>  " << srv_msg << endl;
                }
            }
            
            return;
        // 1 as server
        case 1:
            // create server socket
            cout << "Server Port Number(at least 4 digits): ";
            uint16_t srv_port;
            cin >> srv_port; 
            getchar(); //remove \n
            create_server_sock(srv_port);
            cout << "[Server @ CSE ~] Server startup\n";
            cout << "[Server @ CSE ~] Server ip: 127.0.0.1 port: " << srv_port << "\n";
            cout << "[Server @ CSE ~] Listening ...\n";

            pid_t pid; // distinguish parent

            //set initial command
            string msg_parent[ACPT_CLIENT]; //for parent
            for(int i= 0 ; i < ACPT_CLIENT ; ++i) msg_parent[i] = "<idle>"; 
            string recv_msg;

            //create share memory
            for(int i = 0 ; i < ACPT_CLIENT ; ++i){
                share_mem[i] = create_sharemem(100);
                memcpy(share_mem[i], idle_str, sizeof(idle_str));
            }

            //fork a child for IPC
            pid = fork();

            //deal with IPC
            if(pid == 0){
                string name_list[ACPT_CLIENT];
                while(1){
                    for(int i = 0 ; i < ACPT_CLIENT ; ++i){
                        //read msg from child
                        recv_msg = read_sharemem(i);       //read msg from child

                        //ignore these
                        if(recv_msg[0] == '[' || recv_msg == "<idle>") continue;
                        //if is parent's broadcase remeber username
                        else if(recv_msg[0] == '<' && recv_msg[1] != 'i'){
                            stringstream ss(recv_msg);
                            ss >> temp; ss >> temp;
                            //if username exist ignore, else remeber
                            if(!cli_name.count(temp)){
                                name_list[child_cnt] = temp;
                                cli_name[temp] = child_cnt++;
                                //show current registered accounts
                                cout << "Current Accounts:  ";
                                for(int k = 0 ; k < ACPT_CLIENT ; ++k ) cout << name_list[k] << " ";
                                cout << endl;
                            }
                            else{
                                cout << "User " << temp << " is back online\n";
                            }
                        }
                        //msg from child process
                        else{
                            //reset share buffer
                            memset( share_mem[i], 0, 100);
                            memcpy(share_mem[i], idle_str, sizeof(idle_str));
                            //process msg from child
                            stringstream ss(recv_msg);

                            //get usernames
                            vector <string> to_who;
                            bool names = true;

                            stringstream ss4; //concat msg if contains space
                            while(ss >> temp){
                                if(temp[0] == '\"') names = false;
                                
                                if(names){
                                    to_who.push_back(temp);
                                }
                                else if(temp[temp.size()-1] != '\"'){
                                    ss4 << temp << " ";
                                } 
                                else{
                                    ss4 << temp;
                                    stringstream ss2;
                                    ss2 << "[ " << name_list[i] << " send you a msg : " << ss4.str() << " at " << ctime(&rawtime) << "]\n";
                                    temp = ss2.str();
                                    string wrong_uname = "";
                                    for(int j = 0 ; j < to_who.size() ; ++j){
                                        if(cli_name.count(to_who[j])) msg_parent[cli_name[to_who[j]]] = temp;
                                        else{
                                            stringstream ss3;
                                            ss3 << "[ " << to_who[j] << " does not exist]\n";
                                            wrong_uname = wrong_uname + ss3.str();
                                        }
                                    }
                                    if(wrong_uname != "") msg_parent[i] = wrong_uname;
                                }
                            }
                        }
                    }
                    for(int i = 0 ; i < ACPT_CLIENT ; ++i ){
                        //send msg to target child
                        if(msg_parent[i] != "<idle>"){
                            ipc(i, msg_parent[i]);
                            msg_parent[i] = "<idle>";
                        }
                    }
                }
            }
            else{
                while(1){
                    int child_id;
                    string welcm_msg = listen2cli(child_id);

                    //create a child process to handle connection with client
                    pid = fork();
                    if(pid == 0){
                        //create a child process to handle msg from client
                        pid_t pid2;
                        pid2 = fork();
                        if(pid2 == 0){
                            while(1){
                                string cli_msg = read_cli(child_id);
                                // child to parent
                                ipc(child_id, cli_msg);
                            }
                        }
                        //read cmd from parent process and send to client
                        else{
                            usleep(15000);
                            while(1){
                                recv_msg = read_sharemem(child_id);

                                // check if msg is for me
                                if(strcmp(recv_msg.c_str(),idle_str)!=0 && (recv_msg[0] == '[' || recv_msg[0] == '<')){
                                    //send msg to client
                                    send_msg(child_fd[child_id], recv_msg);
                                    usleep(50000);
                                    //reset share buffer
                                    memset(share_mem[child_id], 0, 100);
                                    memcpy(share_mem[child_id], idle_str, sizeof(idle_str));
                                }
                            }
                        }

                    }
                    //parent broadcast welcome msg to all children
                    else{
                        broadcast2child(welcm_msg);
                    }     
                }
            }

        return;
    }
 }


void Network::create_client_sock(){
    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(client_fd < 0){
        cerr << "Client socket create error\n";
        exit(1);
    }
    else cout << "Client socket create successful\n";
}


void Network::create_server_sock( uint16_t port){
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(server_fd < 0){
        cerr << "[Server @ CSE ~] Create server socket error\n";
    }
   struct sockaddr_in srv;

    srv.sin_family = AF_INET; //internet address family (ipv4)
    srv.sin_port = htons(port);
    srv.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind port
    if(bind(server_fd, (struct sockaddr*) &srv, sizeof(srv)) < 0){
        cerr << "[Server @ CSE ~] bind server socket error\n"; exit(1);
    }
    else cout << "[Server @ CSE ~] bind server socket successfully\n";
 }

 void Network::connect2srv(string ip, uint16_t port, string uname){
     struct sockaddr_in srv;

     srv.sin_family = AF_INET;
     srv.sin_port = htons(port); // target server's port
     srv.sin_addr.s_addr = inet_addr(ip.c_str()); // target server's ip

     if( connect(client_fd, (struct sockaddr *)&srv, sizeof(srv)) < 0){
         cerr << "connect to server " << ip << ":" << port << " error\n";
     }
     else cout << "connect to server " << ip << ":" << port << " success\n";

    //copy username to buffer
    buf[0] = (unsigned char)uname.size(); //name length
    for(int i = 0 ; i < uname.size() ; ++i){
        buf[i+1] = uname[i];
     }
    int nbytes; // use to check how many bytes had been writen
    if( (nbytes = write(client_fd, buf, sizeof(buf)))  < 0){
        cerr << "Write to client socket Error\n";
        exit(1);
    }
    else{
        cout << "Welcome " << uname << endl;
    }

 }

void Network::send_msg(int fd, string msg){

    //copy to buffer
    buf[0] = (char)(msg.size()); // msg length
    buf[1] = (char)(0);

    for(int i = 2 ; i < msg.size() + 2 ; ++ i){
        buf[i]= msg[i - 2];
    }

    int nbytes; // use to check how many bytes had been writen
    if( (nbytes = write(fd, buf, sizeof(buf)))  < 0){
        cerr << "Write to socket Error\n";
        exit(1);
    }

    return;
}

string Network::listen2cli(int &child_id){
    if(listen(server_fd, ACPT_CLIENT) < 0){
        cerr << "Listen for client error\n";
        exit(1);
    }

    // accpet procedure
    struct sockaddr_in cli; // info regarding incoming client
    int cli_len = sizeof(cli);
    // blocking function
    int temp_fd;
    temp_fd = accept(server_fd, (struct sockaddr *) &cli, (socklen_t *) &cli_len);
    if(temp_fd < 0){
        cerr << "Accept client error\n";
        exit(1);
    }

    cout << "[Server @ CSE ~] A Client " << inet_ntoa(cli.sin_addr) << " has connected via port num " << ntohs(cli.sin_port) << " using SOCK_STREAM (TCP)\n";


    //get username
    int nbytes; //count data length from client
    if( (nbytes = read(temp_fd, buf, sizeof(buf))) < 0){
        cerr << "Read client username Error\n";
        exit(1);
    }
    int name_len = (unsigned char)buf[0];
    string uname = "";
    for(int i =1 ; i <= name_len ; ++i){
        uname.push_back(buf[i]);
    }

    username = uname;

    //check if username exist
    if(cli_name.count(uname)){
        child_fd[cli_name[uname]] = temp_fd;
        //get child id
        child_id = cli_name[uname];
        cli_addr[child_id] = cli;
    }
    else{
        child_fd[child_cnt] = temp_fd;
        //map username with its respectively socket fd
        cli_name[uname] = child_cnt;
        child_id = child_cnt++;
        cli_addr[child_id] = cli;
    }

    // create welcome mesage
    stringstream ss;
    ss << "<User " <<  uname <<  " is_online ,IP: " <<  inet_ntoa(cli.sin_addr) <<  ":" << ntohs(cli.sin_port) << ">";
    string retval = ss.str();

    return retval;

}

string Network::read_cli(int child_id){
    string msg = "";
    int nbytes; //count data length from client
    if( (nbytes = read(child_fd[child_id], buf, sizeof(buf))) < 0){
        cerr << "Read from client Error\n";
        exit(1);
    }
    //check if disconnect signal
    if((unsigned char)buf[0] == (unsigned char)(0) && (unsigned char)buf[1] == (unsigned char)(0)){
        cout << "[Server @ CSE ~] Client " << inet_ntoa(cli_addr[child_cnt-1].sin_addr) << " with port num " << ntohs(cli_addr[child_cnt-1].sin_port) << " has disconnected\n";
        stringstream ss;
        ss << "<User " << username << " has disconnected>\n"; 
        broadcast2child(ss.str());
        exit(1);
    }
    if((unsigned char)buf[0] == (unsigned char)0 && (unsigned char)buf[1] != (unsigned char)(0)){
        for(int i = 2 ; i < BUFF_SIZE ; ++i){
            msg.push_back(buf[i]);
        }
    }
    else{
        int last_size = (unsigned char)buf[0] + (unsigned char)buf[1];
        for(int i = 2 ; i <=  last_size ; ++i){
            msg.push_back(buf[i]);
        }
    }


    cout << "[Server @ CSE ~] Client " << inet_ntoa(cli_addr[child_id].sin_addr) << " with port num " << ntohs(cli_addr[child_id].sin_port) << " send a msg: " << msg << endl;

    return msg;
}

string Network::read_srv(){
    string msg = "";
    int nbytes; //count data length from client
    if( (nbytes = read(client_fd, buf, sizeof(buf))) < 0){
        cerr << "Read from server Error\n";
        exit(1);
    }
    int len = (unsigned char)buf[0];
    for(int i = 2 ; i <  len + 2  ; ++i){
        msg.push_back(buf[i]);
    }

    return msg;
}

void Network::disconnect(){
     buf[0]= (char)(0);
     buf[1]=(char)0;

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

void* create_sharemem(size_t size) {
  // mem buffer  readable and writable:
  int protection = PROT_READ | PROT_WRITE;

  // The buffer will be shared
  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  return mmap(NULL, size, protection, visibility, -1, 0);
}

void Network::broadcast2child(string msg){
    sleep(1);
    for(int i = 0 ; i < ACPT_CLIENT ; ++i){
        memset( share_mem[i], 0, 100);
        memcpy(share_mem[i], msg.c_str(), msg.size());
    }
}

void Network::ipc(int id, string msg){
    memset( share_mem[id], 0, 100);
    memcpy( share_mem[id], msg.c_str(),  msg.size());
    usleep(3000);
}

string Network::read_sharemem(int shm){
    char s[100];
    strcpy(s,(char*)share_mem[shm]);
    usleep(10000);

    return string(s);
}
