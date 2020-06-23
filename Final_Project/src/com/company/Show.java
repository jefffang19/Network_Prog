package com.company;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;
import java.net.Socket;

public class Show extends JFrame {
    Card[] p1cards = new Card[10];
    Card[] p2cards = new Card[10]; //opponent's card
    int p1CardNum = 0;
    int p2CardNum = 0;
    public boolean yourTurn;
    public boolean youFirst;
    public boolean flipCard = false;

    //connection
    PrintWriter printWriter;

    DrawCanvas canvas;


    public Show(Socket s, String uname) throws IOException {
        printWriter = new PrintWriter(s.getOutputStream(), true);

        setTitle(uname);

        for(int i = 0 ; i < 10 ; ++i){
            p1cards[i] = new Card();
            p2cards[i] = new Card();
        }
        canvas = new DrawCanvas();
        setSize(800,600);
        setContentPane(canvas);
        canvas.setLayout(new BorderLayout());

        JButton hit = new JButton("hit");
        JButton stand = new JButton("stand");

        //hit stand button
        hit.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if(yourTurn){
                    System.out.println("Hit");
                    printWriter.println("1");
                }
            }
        });

        stand.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if(yourTurn){
                    System.out.println("Stand");
                    printWriter.println("2");
                    yourTurn = false;
                }
            }
        });

        //button layout
        GridLayout buttonLayout = new GridLayout(7,1);
        JPanel eastPanel = new JPanel(buttonLayout);
        eastPanel.setBackground(new Color(0x00A600));
        canvas.add(eastPanel, BorderLayout.EAST);
        eastPanel.add(new JLabel());
        eastPanel.add(hit);
        eastPanel.add(new JLabel());
        eastPanel.add(stand);
        eastPanel.add(new JLabel());


        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setVisible(true);

    }

    public void initWithSrv(String info){
        int cnt = 0;
        int p1len = info.charAt(0) - '0',
            p2len = info.charAt(1) - '0';
        for(int i = 3 ; i < info.length() ; i+=3){
            switch(info.charAt(i)){
                case 'C':
                    if(cnt<p1len) p1cards[cnt].suit = Card.Suit.club;
                    else p2cards[cnt-p1len].suit = Card.Suit.club;
                    break;
                case 'S':
                    if(cnt<p1len) p1cards[cnt].suit = Card.Suit.spade;
                    else p2cards[cnt-p1len].suit = Card.Suit.spade;
                    break;
                case 'H':
                    if(cnt<p1len) p1cards[cnt].suit = Card.Suit.heart;
                    else p2cards[cnt-p1len].suit = Card.Suit.heart;
                    break;
                case 'D':
                    if(cnt<p1len) p1cards[cnt].suit = Card.Suit.diamond;
                    else p2cards[cnt-p1len].suit = Card.Suit.diamond;
                    break;
            }
            if(cnt<p1len){
                switch(info.charAt(i+1)){
                    case 'T':
                        p1cards[cnt].face = 10;
                        break;
                    case 'J':
                        p1cards[cnt].face = 11;
                        break;
                    case 'Q':
                        p1cards[cnt].face = 12;
                        break;
                    case 'K':
                        p1cards[cnt].face = 13;
                        break;
                    default:
                        p1cards[cnt].face = info.charAt(i+1) - '0';
                        break;
                }
            }
            else{
                switch(info.charAt(i+1)){
                    case 'T':
                        p2cards[cnt-p1len].face = 10;
                        break;
                    case 'J':
                        p2cards[cnt-p1len].face = 11;
                        break;
                    case 'Q':
                        p2cards[cnt-p1len].face = 12;
                        break;
                    case 'K':
                        p2cards[cnt-p1len].face = 13;
                        break;
                    default:
                        p2cards[cnt-p1len].face = info.charAt(i+1) - '0';
                        break;
                }
            }
            ++cnt;
        }
        p1CardNum = p1len;
        p2CardNum = p2len;

        //debug
        showCards();

        repaint();
    }

    public void p2Move(String info){
        switch(info.charAt(0)){
            case 'C':
                p2cards[p2CardNum].suit = Card.Suit.club;
                break;
            case 'S':
                p2cards[p2CardNum].suit = Card.Suit.spade;
                break;
            case 'H':
                p2cards[p2CardNum].suit = Card.Suit.heart;
                break;
            case 'D':
                p2cards[p2CardNum].suit = Card.Suit.diamond;
                break;
        }
        switch(info.charAt(1)){
            case 'T':
                p2cards[p2CardNum].face = 10;
                break;
            case 'J':
                p2cards[p2CardNum].face = 11;
                break;
            case 'Q':
                p2cards[p2CardNum].face = 12;
                break;
            case 'K':
                p2cards[p2CardNum].face = 13;
                break;
            default:
                p2cards[p2CardNum].face = info.charAt(1) - '0';
                break;
        }
        p2CardNum++;
        repaint();
    }

    public void p1Move(String info){
        switch(info.charAt(0)){
            case 'C':
                p1cards[p1CardNum].suit = Card.Suit.club;
                break;
            case 'S':
                p1cards[p1CardNum].suit = Card.Suit.spade;
                break;
            case 'H':
                p1cards[p1CardNum].suit = Card.Suit.heart;
                break;
            case 'D':
                p1cards[p1CardNum].suit = Card.Suit.diamond;
                break;
        }
        switch(info.charAt(1)){
            case 'T':
                p1cards[p1CardNum].face = 10;
                break;
            case 'J':
                p1cards[p1CardNum].face = 11;
                break;
            case 'Q':
                p1cards[p1CardNum].face = 12;
                break;
            case 'K':
                p1cards[p1CardNum].face = 13;
                break;
            default:
                p1cards[p1CardNum].face = info.charAt(1) - '0';
                break;
        }
        p1CardNum++;
        repaint();
    }

    public void whoWin(int who){
        flipCard = true;
        repaint();
        switch(who){
            case 0:
                JOptionPane.showConfirmDialog(canvas,"You Win", "Result", JOptionPane.DEFAULT_OPTION);
                System.out.println("Debug Pressed ok!");
            break;
            case 1:
                JOptionPane.showConfirmDialog(canvas,"You Lose", "Result", JOptionPane.DEFAULT_OPTION);
                break;
            case 2:
                JOptionPane.showConfirmDialog(canvas,"Tie", "Result", JOptionPane.DEFAULT_OPTION);
                break;
        }
    }

    //debug option
    public void showCards(){
        for(int i = 0 ; i < p1CardNum ; ++i){
            System.out.println("" + p1cards[i].suit + p1cards[i].face);
        }
        for(int i = 0 ; i < p2CardNum ; ++i){
            System.out.println("" + p2cards[i].suit + p2cards[i].face);
        }
    }

    class DrawCanvas extends JPanel {
        @Override
        protected void paintComponent(Graphics g) {
            super.paintComponent(g);
            setBackground(new Color(0x00A600));
            try {
                if(youFirst){
                    //draw my card
                    for(int i = 0 ; i < p1CardNum ; ++i){
                        String filename = "cards\\" + p1cards[i].face+ p1cards[i].suit.toString().toUpperCase().charAt(0) + ".png";
                        System.out.println("debug input file: " + filename);
                        File file = new File(filename);
                        System.out.println("debug: " + file.exists());
                        Image image = ImageIO.read(file);
                        image = image.getScaledInstance(80,122,Image.SCALE_DEFAULT);
                        g.drawImage(image,10+100*i ,300, null);
                    }
                    //draw opponent back of card
                    String filename;
                    Image image;
                    File file;
                    if(p2CardNum > 0){
                        if(flipCard) filename = "cards\\" + p2cards[0].face+ p2cards[0].suit.toString().toUpperCase().charAt(0) + ".png";
                        else filename = "cards\\yellow_back.png";
                        System.out.println("debug op input file: " + filename);
                        file = new File(filename);
                        System.out.println("debug: " + file.exists());
                        image = ImageIO.read(file);
                        image = image.getScaledInstance(80,122,Image.SCALE_DEFAULT);
                        g.drawImage(image,10 ,10, null);
                    }
                    //draw opponent card
                    for(int i = 1 ; i < p2CardNum ; ++i){
                        filename = "cards\\" + p2cards[i].face+ p2cards[i].suit.toString().toUpperCase().charAt(0) + ".png";
                        System.out.println("debug op input file: " + filename);
                        file = new File(filename);
                        System.out.println("debug: " + file.exists());
                        image = ImageIO.read(file);
                        image = image.getScaledInstance(80,122,Image.SCALE_DEFAULT);
                        g.drawImage(image,10 + 100*i ,10, null);
                    }
                }
                else{
                    //draw my card
                    for(int i = 0 ; i < p2CardNum ; ++i){
                        String filename = "cards\\" + p2cards[i].face+ p2cards[i].suit.toString().toUpperCase().charAt(0) + ".png";
                        System.out.println("debug input file: " + filename);
                        File file = new File(filename);
                        System.out.println("debug: " + file.exists());
                        Image image = ImageIO.read(file);
                        image = image.getScaledInstance(80,122,Image.SCALE_DEFAULT);
                        g.drawImage(image,10+100*i ,300, null);
                    }

                    String filename;
                    Image image;
                    File file;
                    //draw opponent card
                    for(int i = 0 ; i < p1CardNum ; ++i){
                        filename = "cards\\" + p1cards[i].face+ p1cards[i].suit.toString().toUpperCase().charAt(0) + ".png";
                        System.out.println("debug op input file: " + filename);
                        file = new File(filename);
                        System.out.println("debug: " + file.exists());
                        image = ImageIO.read(file);
                        image = image.getScaledInstance(80,122,Image.SCALE_DEFAULT);
                        g.drawImage(image,10 + 100*i ,10, null);
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

}
