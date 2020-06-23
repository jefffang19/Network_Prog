package com.company;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;

class Card {
    public int face;
    enum Suit {spade, heart, club, diamond};
    public Suit suit;
}

public class Deck {
    Card[] card = new Card[52];
    public int cardPointer = 0;

    Deck(){
        for(int i = 0 ; i < 52 ; ++i){
            card[i] = new Card();
            card[i].face = (i%13) + 1;
            if(i < 13) card[i].suit = Card.Suit.spade;
            else if(i < 26) card[i].suit = Card.Suit.heart;
            else if(i < 39) card[i].suit = Card.Suit.club;
            else if(i < 52) card[i].suit = Card.Suit.diamond;
        }
        List<Card> unWashed = Arrays.asList(card);
        Collections.shuffle(unWashed);
        unWashed.toArray(card);
    }

}
