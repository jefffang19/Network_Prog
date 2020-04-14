#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#define BYTE 8
using namespace std;


class encode{
public:
    //encoding function, return encoded text (in byte form)
    vector<unsigned char> encode_file(string filename);
    //decode function, save as filename
    void decode_file(string filename);
private:    
    int orisize=0; //origin file byte
    int cpsize=0; //compress file byte
    int zero_pad; //how many zero padded
    int enc_len; //fix-len encoding's length
    unsigned char delim = -1; //delimiter for header
    map<unsigned char, int> enc_table; // encoding hash table for input file
    map<string, unsigned char> dec_table; // decoding hash table for the compressed file

    vector<unsigned char> read_bfile(string filename); //read binary file
    vector<unsigned char> fix_len_encode(vector<unsigned char> ori); //return encode
    int read_header(vector<unsigned char> cp); //read the header of the compressed file, return the starting point of actual data
    string formbyte(vector<unsigned char> s); //turn bits to byte
    string convert_to_binary(unsigned char c); //convert input bit into binary
    string convert_to_binary(int c, int len); //len decide length of the binary data
}huff;

/*header of the compressed data looks like*/
/*
[number of keys in hash table](delim)(encode len)(delim)[key1](delim)[key2](delim)...(delim)[keyN](delim)[number of zero padding](delim)[original filesize](delim)[here is the compress data]
.
.
.
[here is the compress data]
*/

// main program
int main(){
    string file_name = "test";
    vector<unsigned char> e = huff.encode_file(file_name);
    ofstream out;
    out.open("temp");
    for(int i=0;i<e.size();++i) out << e[i];
    out.close();
    
    huff.decode_file("temp");    
    return 0;
}



//return encode
vector<unsigned char> encode::fix_len_encode(vector<unsigned char> ori){
    set<unsigned char> plantext_table;
    
    for(int i = 0 ; i < ori.size() ; ++i){
        plantext_table.insert(ori[i]);
    }

    //create encoding hash table
    int count = 0;
    for(auto i : plantext_table){
        enc_table[i] = count++;
    }
    
    //encode the origin text
    int plan_len = ori.size();
    for(int i = 0; i < plan_len ; ++i){
        ori[i] = enc_table[ori[i]] + '0' ;
    }

    enc_len = ceil(log(plantext_table.size()) / log(2)); //count length of the fix-length encode
    //turn to binary form
    vector<unsigned char> bits_collect;
    for(auto i : ori){
        string s = convert_to_binary(i - '0', enc_len);
        for(auto j : s) bits_collect.push_back(j);
    }
    
    //debug
    for(auto i : bits_collect) cout << i;
    cout << endl << endl;

    return bits_collect;
}

void encode::decode_file(string filename){
    vector<unsigned char> cp_file = read_bfile(filename);
    int dec_pos = read_header(cp_file); //the starting position of encoded file

    //debug hash table
    for(auto i : dec_table) cout << i.first << " " << i.second << endl;

        //start decoding
    string bits_collector = "";
    for(int i = dec_pos ; i < cpsize - zero_pad ; ++i){
        bits_collector += convert_to_binary(cp_file[i]);
        //cout << dec_table[bits_collector.substr(0,enc_len)];
        //cout << bits_collector << endl;
        cout << cp_file[i] << " ";
        bits_collector.erase(0,enc_len);
    }

    return;
}

int encode::read_header(vector<unsigned char> cp){
    int num_of_keys; // number of keys in hash table
    num_of_keys = cp[0] - '0';

    int cur_val = 2; //pointer for reading header

    //fix length encoding's len
    enc_len = cp[cur_val++] - '0';
    ++cur_val; //skip delim

    for(int i = 0 ; i< num_of_keys ; ++i){
        string bits = convert_to_binary(cp[cur_val + 2] - '0', enc_len);
        dec_table[bits] = cp[cur_val]; //combine the key and the value to form a hash
        cur_val += 4;
    }

     //compress file size was read in read_bfile()
     cpsize = orisize;

    //extract the zero padding info
    string siz = "";
    while(cp[cur_val] != delim){
        siz.push_back(cp[cur_val]);
        ++cur_val;
    }
    zero_pad = stoi(siz);

   //extract the origin file size info
    siz = ""; ++cur_val;
    while(cp[cur_val] != delim) {
        siz.push_back(cp[cur_val]);
        ++cur_val;
    }
    orisize = stoi(siz);

    return cur_val + 1;
}

//read binary file
vector<unsigned char> encode::read_bfile(string filename){
    ifstream ifile;
    ifile.open(filename, ios::binary); //open file as binary, which lets us process image, video ..0. etc
    unsigned char in; //store binary as unsigned char

    vector<unsigned char> return_vec;
    while(ifile >> in){
        return_vec.push_back(in);
    }
    orisize = return_vec.size(); // keep track of original file size

    ifile.close();

    return return_vec;
}

vector<unsigned char> encode::encode_file(string filename){
    // read in file
    vector<unsigned char> in_file = read_bfile(filename);
    in_file = fix_len_encode(in_file); //encode the input file
    string in_file_byte = formbyte(in_file); //turn encode data form bits to bytes

    vector<unsigned char> enc;

    //header: fix_len encoding's len
    enc.push_back(enc_len + '0');
    enc.push_back(delim);
    
    //add header of encoding
    int cnt = 0; //length of the encoding key
    for(auto i : enc_table){
        enc.push_back(i.first);
        enc.push_back('=');
        enc.push_back(i.second + '0');
        enc.push_back(delim);
        ++cnt;
    }

    //add length of encoding key to front
    enc.insert(enc.begin(),delim);
    enc.insert(enc.begin(),cnt + '0' );

    //then combine with encoded data
    for(int i = 0; i < in_file.size() ; ++i){
        enc.push_back(in_file_byte[i]);
    }

    return enc;
}

string encode::convert_to_binary(unsigned char c){
	int number = (int)c;
	string s = "";
	while(number){
		if(number%2) s = '1' + s;
		else s = '0' + s;
		number/=2;
	}
	while(s.size() < BYTE) s = '0' + s;
	return s;
}

string encode::convert_to_binary(int c, int len){
	int number = c;
	string s = "";
	while(number){
		if(number%2) s = '1' + s;
		else s = '0' + s;
		number/=2;
	}
	while(s.size() < len) s = '0' + s;
	return s;
}

string encode::formbyte(vector<unsigned char> s){
    string byte_data = ""; //the return data of this function
	int addzero=BYTE-(s.size()%BYTE); //count pad how many 0s
	for(int i=0;i<addzero;++i) s.push_back('0');

    /* the header looks like:
    zero padding count (delimiter) original file size (delimiter) compressed file size (delimiter)
    */
   string deli = "";
   deli.push_back(delim);
	byte_data = to_string(addzero) + deli + to_string(orisize + 1) + deli ;

    //transform bits into bytes
	int n=0;
	for(int i=0,j=BYTE-1;i<(int)s.size();++i,--j){
		if(s[i]=='1')
			n+=(int)pow(2,j);
		if(i%BYTE==BYTE-1){
			byte_data.push_back(char(n));
			j=BYTE;
			n=0;
		}
	}

    //debug
    cout << byte_data << endl << endl;

    return byte_data;
}
