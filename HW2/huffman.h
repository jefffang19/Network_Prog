#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <string.h>
#include <cmath>
using namespace std;

class node{
public:
	node():freq(0),left(NULL),right(NULL){};
	node(unsigned char k, int f):freq(f),left(NULL),right(NULL){ key.push_back(k); };
	vector<unsigned char> key; //store the node's keys
	vector<int> route; //store the key's huffman code
	int freq; //store the frequency of the key appearence
	node* left, *right;
};
bool compar(node* a, node* b);  //descending partition
void makeroute(node* root, int v); //use when making huffuman code
void huffman(node* root, map<unsigned char,vector<int>> &dict); //ouput the huffuman code to file
void formbyte(string s, ofstream &out); //turn bits to byte
void readbits(); //turn read in into bits
string convert_to_binary(unsigned char c); //convert input into binary

//interface
void compress(string ifname, string ofname); //compress the file
void decompress(string ifname, string ofname); //decompress the file

//interface
void use_huff(int cmd, string fname); //1 compress, 2 decompress


#endif