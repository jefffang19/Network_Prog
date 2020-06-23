package com.company;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;

class ThreadSocket extends Thread {
    public Socket socket;
    public String username;
    public boolean isHost; //if is the host of the room
    public int userRoomNum; //if user create room, this is it's room number
    private boolean endThread;
    private PrintWriter printWriter;
    static private ServerDB serverDB;

    BufferedReader reader;

    public ThreadSocket() {
        username = "Default";
        isHost = false;
        endThread = false;
        serverDB = new ServerDB();
    }

    public void run() {
        InputStream in = null;
        try {
            in = socket.getInputStream();
        } catch (IOException e) {
            e.printStackTrace();
        }
        reader = new BufferedReader(new InputStreamReader(in));
        try {
            while(!endThread){
                String cmd = reader.readLine();
                //System.out.println("debug" + Integer.valueOf(cmd.charAt(0)));
                //check user command
                handleCommand(cmd);
            }
        } catch (IOException | InterruptedException e) {
            System.out.println("Connection error");
            System.out.println(e.toString());
        }
    }

    //write to socket buffer tobe send to client
    private void writeBuffer(String data) throws IOException {
        OutputStream out = socket.getOutputStream();
        printWriter = new PrintWriter(out, true);
        printWriter.println(data);
    }

    private void handleCommand(String cmd) throws IOException, InterruptedException {
        switch(Integer.valueOf(cmd.charAt(0) - '0')){
            //case login
            case 0:
                System.out.println("Client UserName : " + cmd.substring(1));
                username = cmd.substring(1);
                //if username exist, change status to online
                if(serverDB.userList.containsKey(username)) serverDB.userList.get(username).online = true;
                    //if doesn't exist, create profile
                else{
                    ServerDB.UserInfo newAccount = new ServerDB.UserInfo();
                    newAccount.online = true;
                    serverDB.userList.put(username, newAccount);
                }
                break;
            //case "CreateRoom"
            case 1:
                isHost = true;
                //set room hosting information
                synchronized (serverDB){
                    System.out.println("Room created by : " + username);
                    ServerDB.rooms[ServerDB.currentRooms] = new ServerDB.Rooms();
                    ServerDB.rooms[ServerDB.currentRooms].waiting = true;
                    ServerDB.rooms[ServerDB.currentRooms].player1 = username;
                    userRoomNum = ServerDB.currentRooms++;
                }

                while(true){
                    synchronized (serverDB){
                        //if match found
                        if(!ServerDB.rooms[userRoomNum].waiting) break;
                    }
                    Thread.sleep(1000);
                }

                //return game start
                writeBuffer("0");
                gameHandler(true);

                break;
            //case "SearchRoom"
            case 2:
                String roomList = "";
                System.out.println("searching room");
                for(int i = 0 ; i < serverDB.currentRooms ; ++i)
                    if(serverDB.rooms[i].waiting)
                        //System.out.println("Room " + i + " Room owner : " + game.rooms[i].player1);
                        roomList = roomList + "Room " + i + ", host is " + serverDB.rooms[i].player1 + "\n";

                writeBuffer(roomList);
                break;
            //case "JoinRoom"
            case 3:
                int roomNum = Integer.valueOf(cmd.charAt(1) - '0');
                synchronized (serverDB){
                    serverDB.rooms[roomNum].waiting = false;
                    serverDB.rooms[roomNum].playing = true;
                    serverDB.rooms[roomNum].player2 = username;
                }
                gameHandler(false);
                break;
            //case "Users"
            case 4:
                String data = "";
                for(String user : ServerDB.userList.keySet()){
                    //create data for packet
                    data = data + "Username " + user + ", online : " + ServerDB.userList.get(user).online + " Win " + ServerDB.userList.get(user).win + ",Lose " + ServerDB.userList.get(user).lose + ",tie " + ServerDB.userList.get(user).tie + ";";
                }
                writeBuffer(data);
                break;
            //case "Disconnect"
            case 5:
                System.out.println("Client Disconnect " + username);
                try{
                    socket.close();
                } catch (IOException e) {
                    System.out.println("Close socket error");
                    System.out.println(e.toString());
                }

                endThread = true;
                break;
        }
    }

    //pass the game data to another player's thread
    private void gameHandler(boolean firstPlayer) throws IOException, InterruptedException {
        if(firstPlayer){
            //first player thread host the game
            Game game = new Game();
            //the first packet for player2
            PlayingData.setCardPacket(game.getCard());
            PlayingData.setMoved(true);
            //the first packet is the initial cards on the board, to player1
            writeBuffer(PlayingData.getCardPacket());
            //player1 turn
            while(true){
                String playerCmd = reader.readLine();
                //debug
                System.out.println("player" + username + " " + playerCmd + ":"+ playerCmd.equals("1"));
                //case hit
                if(playerCmd.equals("1")){
                    Card c = game.draw();
                    game.hit(1,c);
                    String card = game.cardString(c);
                    //debug
                    System.out.println("The return packet should look like " + card);
                    //to player1
                    writeBuffer(card);
                    //to player2
                    PlayingData.setCardPacket(card);
                    PlayingData.setMoved(true);
                }
                //case stand
                else if(playerCmd.equals("2")){
                    //debug
                    System.out.println("Stand");
                    //player1's turn end
                    PlayingData.setChangeTurn(true);
                    //signal player 2
                    PlayingData.setMoved(true);
                    Thread.sleep(1001);
                    break;
                }
            }
            //player2 turn
            while(true){
                //wait for opponent to move
                while(!PlayingData.getMoved()) Thread.sleep(1000);
                String cmd = PlayingData.getCardPacket();
                PlayingData.setMoved(false);
                //process opponent's cmd
                if(cmd.equals("1")){
                    Card c = game.draw();
                    game.hit(2,c);
                    String card = game.cardString(c);
                    //send to player1
                    writeBuffer(card);
                    //send to player2
                    PlayingData.setCardPacket(card);
                    PlayingData.setMoved(true);
                    Thread.sleep(1001);
                }
                else if(cmd.equals("2")){
                    //player2's turn end
                    PlayingData.setChangeTurn(false);
                    //check win or lose
                    int win = game.whoWin();
                    System.out.println("Who win " + win);
                    //send to player1
                    writeBuffer(("" + win));
                    //send to player2
                    PlayingData.setCardPacket((""+ win));
                    PlayingData.setMoved(true);
                    break;
                }
            }
        }
        else{
            //the first packet is the initial cards on the board
            while(!PlayingData.getMoved()) Thread.sleep(1000);
            writeBuffer(PlayingData.getCardPacket());
            PlayingData.setMoved(false);

            //player 1 turn
            while(true){
                while(!PlayingData.getMoved()) Thread.sleep(1000);
                PlayingData.setMoved(false);
                if(!PlayingData.getChangeTurn()) writeBuffer(PlayingData.getCardPacket());
                else{
                    //end player 1's turn
                    System.out.println("Now is player 2's turn");
                    writeBuffer("3");
                    break;
                }
            }
            //player 2 turn
            while(true){
                String playerCmd = reader.readLine();
                //pass player 2's cmd to player1 thread
                PlayingData.setCardPacket(playerCmd);
                PlayingData.setMoved(true);
                Thread.sleep(1001);
                //wait for respond from player 1 thread
                while(!PlayingData.getMoved()) Thread.sleep(1000);
                writeBuffer(PlayingData.getCardPacket());
                PlayingData.setMoved(false);
                //check if player2 end
                if(!PlayingData.getChangeTurn()) break;
            }
        }
    }
}

public class Server {
    private ServerSocket serverSocket;
    private final int serverPort = 1234;
    private final int maxClient = 5;
    private ThreadSocket[] threadSocket;
    public static int currentNumClient;


    public Server() throws IOException {
        try {
            serverSocket = new ServerSocket(1234);
        } catch (IOException e) {
            System.out.println("Server Socket create failed");
            System.out.println(e.toString());
        }

        threadSocket = new ThreadSocket[maxClient];
        for(int i = 0 ; i < maxClient ; ++i) threadSocket[i] = new ThreadSocket();
        currentNumClient = 0;
    }

    public void readClient() {
        System.out.println("Server online!\nListening");
        while(true){
            try {
                Socket tmpSocket = serverSocket.accept();
                if(currentNumClient < maxClient){
                    threadSocket[currentNumClient].socket = tmpSocket;
                    System.out.println(threadSocket[currentNumClient].socket.getInetAddress());
                    threadSocket[currentNumClient].socket.setSoTimeout(120000);
                    threadSocket[currentNumClient].start(); //start thread
                    currentNumClient++;
                    //debug
                    System.out.println("Debug : current online " + currentNumClient);
                }
                else{
                    //reply we are full
                    System.out.println("Our Server is full");
                }
            } catch (IOException e) {
                System.out.println("Server accept error");
                System.out.println(e.toString());
            }
        }
    }
}
