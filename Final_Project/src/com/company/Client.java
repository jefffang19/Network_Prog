package com.company;

import javax.swing.*;
import java.io.*;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.Scanner;

public class Client {
    private String addr = "127.0.0.1";
    private int port = 1234;
    private PrintWriter printWriter;
    private String username;
    Show show;
    Socket client;

    public Client() {
        client = new Socket();
        InetSocketAddress isa = new InetSocketAddress(this.addr, this.port);

        try {
            client.connect(isa, 120000);

            //login user
            Scanner scanner = new Scanner(System.in);
            System.out.print("Username : ");
            username = scanner.next();
            writeClient(("0" + username));

            while(true){
                //user input command
                int userCmd = getUserInput();
                //process command
                processUserCommand(userCmd);
                System.out.println("Debug Message : Packet Sent");
                //check if break
                if(userCmd == 5) break;
                Thread.sleep(1000);
            }

            client.close();
        } catch (IOException | InterruptedException e) {
            System.out.println("Socket Connect Error");
            System.out.println(e.toString());
        }

    }

    //write to client buffer to send to Server
    private void writeClient(String data) throws IOException {
        OutputStream out = client.getOutputStream();
        printWriter = new PrintWriter(out, true);
        printWriter.println(data);
    }

    //read client buffer for data Sent by Server
    private String readClient() throws IOException {
        InputStream in = client.getInputStream();
        BufferedReader reader = new BufferedReader(new InputStreamReader(in));
        String data = reader.readLine();
        return data;
    }

    //get user input command
    private int getUserInput(){
        Scanner scanner = new Scanner(System.in);
        System.out.println("Command List\n(1) create room\n(2) search room\n(3) join room\n(4) check Users\n(5) Disconnect");
        int userCmd = scanner.nextInt();
        return userCmd;
    }

    //process User's input command, return true if action success"
    private boolean processUserCommand(int cmd) throws IOException, InterruptedException {
        Scanner scanner = new Scanner(System.in);
        String procCmd = "";

        switch(cmd){
            //case create room
            case 1:
                procCmd = Integer.toString(cmd);
                writeClient(procCmd);

                //check if create room successful
                try {
                    readClient();
                    System.out.println("Game starts");
                } catch (Exception e) {
                    return false;
                }

                //game start
                show = new Show(client, username);
                String initString = readClient();
                System.out.println("debug initStr : " + initString);
                show.initWithSrv(initString);
                show.yourTurn = true;
                show.youFirst = true;
                //communicate
                while(true){
                    String Action = readClient();
                    //decide who win
                    if(Action.length() == 1 && !show.yourTurn){
                        if(Action.equals("1")) show.whoWin(0);
                        else if(Action.equals("0")) show.whoWin(2);
                        else show.whoWin(1);
                        break;
                    }
                    if(show.yourTurn) show.p1Move(Action);
                    else show.p2Move(Action);
                }
                break;
                //join room
            case 3:
                System.out.print("Please enter Room Number : ");
                String roomNum = scanner.next();
                procCmd = Integer.toString(cmd) + roomNum;
                writeClient(procCmd);
                //game start
                show = new Show(client, username);
                String initStr = readClient();
                System.out.println("debug initStr : " + initStr);
                show.initWithSrv(initStr);
                show.yourTurn = false;
                show.youFirst = false;
                //communicate
                while(true){
                    String Action = readClient();
                    System.out.println("debug packet data " + Action + ";" + Action.length() + ";" + show.yourTurn);
                    //decide who win
                    if(Action.length() == 1){
                        if(!Action.equals("3")){
                            System.out.println("debug p2 end");
                            if(Action.equals("2")) show.whoWin(0);
                            else if(Action.equals("0")) show.whoWin(2);
                            else show.whoWin(1);
                            break;
                        }
                        //if switch to your turn
                        else{
                            System.out.println("debug should not end here");
                            show.yourTurn = true;
                            continue;
                        }
                    }

                    if(show.yourTurn) show.p2Move(Action);
                    else show.p1Move(Action);
                }
                break;
                //case 2.search room, 4. current users, 5. disconnect
            case 2:
            case 4:
                procCmd = Integer.toString(cmd);
                writeClient(procCmd);
                procCmd = readClient();
                //debug
                System.out.println(procCmd);
                break;
            case 5:
                procCmd = Integer.toString(cmd);
                writeClient(procCmd);
                break;
        }
        return true;
    }
}
