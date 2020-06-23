package com.company;

import java.io.IOException;
import java.util.Scanner;

public class Main {

    public static void main(String[] args) throws IOException {
        System.out.println("Hello world!");
        System.out.println("0 = client, 1 = server");
        Scanner scanner = new Scanner(System.in);
        int decision = scanner.nextInt();
        switch(decision) {
            case 0:
                new Client();
                break;
            case 1:
                Server server = new Server();
                server.readClient();
        }

    }
}
