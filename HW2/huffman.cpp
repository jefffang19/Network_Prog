#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <cmath>
#include <fstream>
using namespace std;

class huffman_encode{
public:

    //encoding function, return encoded text
    vector<unsigned char> encode_file(string filename);

private:

    // encoding hash table for input file
    map<unsigned char, int> enc_table;

    //read binary file
    vector<unsigned char> read_bfile(string filename);

    //return encode
    vector<unsigned char> fix_len_huffman(vector<unsigned char> ori);
}huff;



// main program
int main(){
    string file_name = "test";
    vector<unsigned char> e = huff.encode_file(file_name);
    for(int i=0;i<e.size();++i) cout << e[i];
    cout << endl;    
    return 0;
}


//read binary file
vector<unsigned char> huffman_encode::read_bfile(string filename){
    ifstream ifile;
    ifile.open(filename, ios::binary); //open file as binary, which lets us process image, video ... etc
    unsigned char in; //store binary as unsigned char

    vector<unsigned char> return_vec;
    while(ifile >> in){
        return_vec.push_back(in);
    }

    return return_vec;
}

//return encode
vector<unsigned char> huffman_encode::fix_len_huffman(vector<unsigned char> ori){
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
    return ori;
}

vector<unsigned char> huffman_encode::encode_file(string filename){
    // read in file
    vector<unsigned char> in_file = read_bfile(filename);
    in_file = fix_len_huffman(in_file);
    vector<unsigned char> enc;
    
    //add header of encoding
    int cnt = 0; //length of the encoding key
    for(auto i : enc_table){
        enc.push_back(i.first);
        enc.push_back('=');
        enc.push_back(i.second + '0');
        enc.push_back(',');
        ++cnt;
    }

    //add length of encoding key to front
    enc.insert(enc.begin(),',');
    enc.insert(enc.begin(),cnt + '0' );

    //then combine with encoded data
    for(int i = 0; i < in_file.size() ; ++i){
        enc.push_back(in_file[i]);
    }

    return enc;
}