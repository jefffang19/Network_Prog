package com.company;

import java.util.Scanner;

public class Play {
    Game game = new Game();
    boolean[] stand = {false, false};

    //return the card player drown
    public Card inputCommand(int player, int cmd){
        switch(cmd){
            case 1:
                Card c = game.draw();
                game.hit(player,c);
                return c;
            case 2:
                stand[player-1] = true;
                break;
        }

        return new Card();
    }

    public String showCard(int player){
        String info = "";
        switch(player){
            case 1:
                for(int i = 0 ; i < game.player1Cards ; ++i){
                    info = info + game.player1[i].suit.toString().charAt(0) + game.player1[i].face + ",";
                }
                break;
            case 2:
                for(int i = 0 ; i < game.player2Cards ; ++i){
                    info = info + game.player2[i].suit.toString().charAt(0) + game.player2[i].face + ",";
                }
                break;
        }
        return info;
    }

    public Play(){
        inputCommand(1,1);
        inputCommand(1,2);
        System.out.println(showCard(1));
        inputCommand(2,1);
        inputCommand(2,2);
        System.out.println(showCard(2));
        if(stand[0] && stand[1]){
            System.out.println("who win" + game.whoWin());
        }
    }
}
