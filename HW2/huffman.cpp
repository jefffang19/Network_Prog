#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <cmath>
using namespace std;

class node{
public:
    bool is_data;
    char data;
    int freq;
    node *left, *right;
    node():is_data(false),data('0'),freq(0),left(NULL),right(NULL){}
};

class encode{
public:
    unsigned char ori;
    int enc;
    encode():ori('0'),enc(0){}
};


// void huffman(string ori_str){
//     map<char,int> table;
//     for(int i = 0 ; i < ori_str.size() ; ++i){
//         table[ori_str[i]]+=1;
//     }
//     /*
//     for(map<char,int>::iterator i = table.begin() ; i != table.end() ; ++i){
//         cout << i->first << " " << i->second << endl;
//     }*/
//     //make every char a node
//     vector<node> tree;
//     for(map<char,int>::iterator i = table.begin() ; i != table.end() ; ++i){
//         node *new_node = new node;
//         new_node->is_data = true;
//         new_node->data = i->first;
//         new_node->freq = i->second;
        
//     }
//     //find least freq 2, combine

// }

vector<encode> fix_len_huffman(string ori_str){
    set<unsigned char> table;
    for(int i = 0 ; i < ori_str.size() ; ++i){
        table.insert(ori_str[i]);
    }
    vector<encode> enc_table;
    int size = table.size();
    int enc_len = ceil(log(size)/log(2));
    int count = 0;
    for(auto i : table){
        encode j;
        j.ori = i;
        j.enc = count++;
        enc_table.push_back(j);
    }
    return enc_table;
}

int main(){
    string test = "i am a good person";
    vector<encode> a = fix_len_huffman(test);
    for(int i=0;i<a.size();++i) cout << a[i].ori << " " << a[i].enc << endl;
    return 0;
}