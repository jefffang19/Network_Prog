作者：方郁文 
學號：B053040017
日期：2020/5/20

P.S.
本次作業使用到 multi-processing


包含檔案:
makefile
interface.cpp
network.cpp
network.h
README.txt

如何編譯：
在終端機輸入make

如何執行:
(As server)
./run server

(As client)
./run client

檔案說明：
interface.cpp吃cmdline arg決定執行者是server還是client

network.h定義了使用的網路function和記憶體共享function(用來作interprocess communication)


執行結果
(As server)
./run server
Server Port Number(at least 4 digits): 1234 /*此處請輸入port*/
[Server @ CSE ~] bind server socket successfully
[Server @ CSE ~] Server startup
[Server @ CSE ~] Server ip: 127.0.0.1 port: 1234
[Server @ CSE ~] Listening ...
/*server開始聽client連線*/

/*server收到連線後顯示的訊息*/
[Server @ CSE ~] A Client 127.0.0.1 has connected via port num 51568 using SOCK_STREAM (TCP)
Current Accounts:  jeff  /*目前有哪些帳號*/
[Server @ CSE ~] A Client 127.0.0.1 has connected via port num 51576 using SOCK_STREAM (TCP)
Current Accounts:  jeff jason  /*目前有哪些帳號*/
[Server @ CSE ~] Client 127.0.0.1 with port num 51576 send a msg: jeff "hello world" /*哪個帳號送了什麼訊息*/
[Server @ CSE ~] Client 127.0.0.1 with port num 51572 send a msg: jason "you too"
[Server @ CSE ~] Client 127.0.0.1 with port num 51572 has disconnected /*哪個帳號下線了*/
[Server @ CSE ~] Client 127.0.0.1 with port num 51576 has disconnected


/***********************************************************/
(As client)
./run client
Client socket create successful

/*可輸入指令connect, bye, chat*/
/*connect ip port username*/
/*chat username (other usernames) "message"*/
/*bye*/

[client @ CSE ~ ]$ connect 127.0.0.1 1234 jeff
connect to server 127.0.0.1:1234 success
Welcome jeff  /*歡迎訊息*/

<MSG FROM SEVER>  <User jeff is_online ,IP: 127.0.0.1:51572>

<MSG FROM SEVER>  <User jason is_online ,IP: 127.0.0.1:51576>
/*他人在上線的訊息*/


<MSG FROM SEVER>  [ jason send you a msg : "hello world" at Wed May 20 23:40:03 2020
]
/*他人送你訊息*/

<MSG FROM SEVER>  <User jeff has disconnected>
/*他人下線的訊息*/

chat jason "you too" /*聊天指令*/
bye /*離線指令*/
Client Socket Closed
See you next time.




