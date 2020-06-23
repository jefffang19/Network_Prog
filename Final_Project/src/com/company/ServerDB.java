package com.company;

import java.util.HashMap;
import java.util.Map;

//record playing data (packets send between players)
class PlayingData{
    static private boolean moved = false;
    static private String cardPacket;
    static private boolean changeTurn = false;
    static public int winLose; //1 p1 win ; 2 p2 win
    static public synchronized void setMoved(boolean set){ moved = set; }
    static public synchronized boolean getMoved(){ return moved; }
    static public synchronized void setCardPacket(String str){ cardPacket = str; }
    static public synchronized String getCardPacket(){ return cardPacket; }
    static public synchronized boolean getChangeTurn(){ return changeTurn; }
    static public synchronized void setChangeTurn(boolean change){ changeTurn = change; }
}

public class ServerDB {
    //room information
    static class Rooms {
        public String player1;
        public String player2;
        public boolean playing = false;
        public boolean waiting = false;
    }

    static class UserInfo{
        public int win = 0;
        public int lose = 0;
        public int tie = 0;
        public boolean online = false;
    }

    static private final int maxRoom = 5;
    static Rooms[] rooms = new Rooms[maxRoom];
    static public int currentRooms = 0;

    //hashtable username -> info
    static public Map<String,UserInfo> userList = new HashMap<>();
}
