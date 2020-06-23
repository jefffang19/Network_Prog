package com.company;

public class Game {
    Deck deck = new Deck();
    final int cardsNum = 10;
    Card[] player1 = new Card[cardsNum];
    Card[] player2 = new Card[cardsNum];
    int player1Cards = 2;
    int player2Cards = 2;

    public Game (){
        for(int i = 0 ; i < cardsNum ; ++i){
            player1[i] = new Card();
            player2[i] = new Card();
        }
        //detribute cards
        player1[0] = deck.card[deck.cardPointer++];
        player1[1] = deck.card[deck.cardPointer++];
        player2[0] = deck.card[deck.cardPointer++];
        player2[1] = deck.card[deck.cardPointer++];
    }

    public void reset (){
        player1Cards = 2;
        player2Cards = 2;
        //detribute cards
        if(deck.cardPointer > 48) deck.cardPointer = 0;
        player1[0] = deck.card[deck.cardPointer++];
        player1[1] = deck.card[deck.cardPointer++];
        player2[0] = deck.card[deck.cardPointer++];
        player2[0] = deck.card[deck.cardPointer++];
    }

    //get all the cards on board now
    public String getCard(){
        String info;
        char face;

        //the header
        info = "" + player1Cards + player2Cards + ",";
        for(int i = 0 ; i < player1Cards ; ++i){
            if(player1[i].face >=10){
                switch(player1[i].face){
                    case 10:
                        face = 'T';
                        break;
                    case 11:
                        face = 'J';
                        break;
                    case 12:
                        face = 'Q';
                        break;
                    case 13:
                        face = 'K';
                        break;
                    default:
                        face = 'W';
                        break;
                }
                info = info + player1[i].suit.toString().toUpperCase().charAt(0) + face + ",";
            }
            else info = info + player1[i].suit.toString().toUpperCase().charAt(0) + player1[i].face + ",";
        }
        for(int i = 0 ; i < player2Cards ; ++i){
            if(player2[i].face >=10){
                switch(player2[i].face){
                    case 10:
                        face = 'T';
                        break;
                    case 11:
                        face = 'J';
                        break;
                    case 12:
                        face = 'Q';
                        break;
                    case 13:
                        face = 'K';
                        break;
                    default:
                        face = 'W';
                        break;
                }
                info = info + player1[i].suit.toString().toUpperCase().charAt(0) + face + ",";
            }
            else info = info + player2[i].suit.toString().toUpperCase().charAt(0) + player2[i].face + ",";
        }

        return info;
    }

    //draw a card
    public Card draw(){
        Card c = deck.card[deck.cardPointer++];
        if(deck.cardPointer > 52) deck.cardPointer = 0;
        //debug
        System.out.println("A card was drawn");
        return c;
    }

    //convert card to a string
    public String cardString(Card c){
        char face = 'W';
        String s = "Something is wrong";
        if(c.face >= 10){
            switch(c.face){
                case 10:
                    face = 'T';
                case 11:
                    face = 'J';
                    break;
                case 12:
                    face = 'Q';
                    break;
                case 13:
                    face = 'K';
                    break;
            }
            s = "" + c.suit.toString().toUpperCase().charAt(0) + face ;
        }
        else{
            s = "" + c.suit.toString().toUpperCase().charAt(0) + c.face ;
        }
        return s;
    }

    //return false if over 21 points
    public boolean hit(int player, Card c){
        //debug
        System.out.println("player " + player + "Hit!");
        switch(player){
            case 1:
                player1[player1Cards++] = c;
                break;
            case 2:
                player2[player2Cards++] = c;
                break;
        }
        if(CheckSum(player) > 21) return false;
        else return true;
    }

    int CheckSum(int player) {
        int psum = 0;
        switch(player){
            case 1:
                for(int i=0;i<player1Cards;i++){
                    if(player1[i].face==11||player1[i].face==12||player1[i].face==13) psum+=10 ;
                    else if(player1[i].face==1) psum+=11 ;
                    else psum+=player1[i].face ;
                }
                //if over 21 , check if ace exist
                if(psum>21){
                    for(int i=0;i<player1Cards;i++){
                        if(player1[i].face==1){
                            psum -= 10;
                            break;
                        }
                    }
                }
                break;
            case 2:
                for(int i=0;i<player2Cards;i++){
                    if(player2[i].face==11||player2[i].face==12||player2[i].face==13) psum+=10 ;
                    else if(player2[i].face==1) psum+=11 ;
                    else psum+=player2[i].face ;
                }
                //if over 21 , check if ace exist
                if(psum>21){
                    for(int i=0;i<player2Cards;i++){
                        if(player2[i].face==1){
                            psum -= 10;
                            break;
                        }
                    }
                }
                break;
        }

        return psum;
    }

    //return 0 if tie
    public int whoWin(){
        int p1sum = CheckSum(1);
        int p2sum = CheckSum(2);
        System.out.println("player1 " + p1sum + ",player2 " +p2sum );
        if(p1sum > 21 && p2sum > 21) return 0;
        else if(p1sum > 21) return 2;
        else if(p2sum > 21) return 1;
        else if(p1sum > p2sum) return 1;
        else if(p2sum > p1sum) return 2;
        else return -1;
    }

}
