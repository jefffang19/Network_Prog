作者：方郁文 
學號：B053040017
日期：2020/4/15

如何編譯：
在終端機輸入make

如何執行:
(As server)
./run server

(As client)
./run client


提供測試檔案test1.txt,test2.pdf,test3.mp4,test4.png

main在interface.cpp中，壓縮使用huffman variable length壓縮
網路相關function寫在network.cpp中，詳情請參照network.h
使用者可自行設定client和server的port，但因linux的設計，只能設1000以上
網路部份主要由run_interface()驅動，執行此function會帶出client或server的使用者介面，client端可以下connect/upload/goodbye三個指令，而server則會自動listen，因此無法下指令。
Server在接到連線後會fork一個child process來和client進行檔案傳輸

