#include "huffman.h"
#define BYTE 8
int orisize=0; //origin file byte
int cpsize=0; //compress file byte



//interface
void use_huff(int cmd, string fname){
	switch(cmd){
		case 1:
		{
			string compname = "."+fname+"_";
			compress(fname, compname);
			break;
		}
		case 2:
		{
			string decompname = fname + "decomp";
			decompname = decompname.erase(0,1);
			decompress(fname,decompname);
			break;
		}
	}
	return;
}

bool compar(node* a, node* b){ return a->freq > b->freq || (a->freq==b->freq && a->key[0] > b->key[0]); }  //descending partition

//function definition
void huffman(node* root, map<unsigned char,vector<int>> &dict){
	if(!root) return;
	//preorder traversal
	if(root->key.size()==1)
		dict[root->key[0]]=root->route; //we only care leaf node
	huffman(root->left,dict);
	huffman(root->right,dict);
}

void makeroute(node* root, int v){
	if(!root) return;
	root->route.push_back(v);
	makeroute(root->left,v);
	makeroute(root->right,v);
}

void formbyte(string s, ofstream &out){
	int addzero=BYTE-(s.size()%BYTE);
	for(int i=0;i<addzero;++i) s+='0';
	cpsize+=s.size()/BYTE;
	out << addzero << endl << orisize << endl << cpsize << endl << (double)orisize/(double)cpsize << endl;
	int n=0;
	for(int i=0,j=BYTE-1;i<(int)s.size();++i,--j){
		if(s[i]=='1')
			n+=(int)pow(2,j);
		if(i%BYTE==BYTE-1){
			out << (unsigned char)n;
			j=BYTE;
			n=0;
		}
	}
}
	
	

void compress(string ifname, string ofname){
	ifstream input;
	input.open(ifname,ios::binary);
	input.unsetf(ios::skipws);
	unsigned char a; //read input
	map<unsigned char,long long> cnt; //count times an input appears
	while(input >> a){
		++cnt[a];
		++orisize;
	}
	input.close();
	vector<node*> arr;
	for(map<unsigned char,long long>::iterator it=cnt.begin();it!=cnt.end();++it){
		node* temp = new node(it->first,it->second); //store the input data in a node, note that the inputs are leaf node
		arr.push_back(temp);
	}
	sort(arr.begin(),arr.end(),compar);
	//merge
	while(arr.size()-1){
		node *temproot = new node; //use to combine the smallest two frequency node
		//smaller alphabetically order one in left subtree, bigger in right subtree
		bool com=false; //compare the alphabetically order
		for(unsigned i=0;i<arr[arr.size()-1]->key.size();++i)
			if(arr[arr.size()-2]->key[0] > arr[arr.size()-1]->key[i]){ com=true; break;}
		if(com){ 
			temproot->left=arr[arr.size()-1];
			temproot->right=arr[arr.size()-2];
		}
		else{ 
			temproot->left=arr[arr.size()-2];
			temproot->right=arr[arr.size()-1];
		}
		//combine all the children's key
		for(unsigned i=0;i<arr[arr.size()-1]->key.size();++i) temproot->key.push_back(arr[arr.size()-1]->key[i]);
		for(unsigned i=0;i<arr[arr.size()-2]->key.size();++i) temproot->key.push_back(arr[arr.size()-2]->key[i]);
		sort(temproot->key.begin(),temproot->key.end());
		arr.pop_back(); arr.pop_back();
		makeroute(temproot->left,0); //add 0 to huffunam node of all the node's of left subtree
		makeroute(temproot->right,1); //add 1 to huffunam node of all the node's of right subtree
		temproot->freq=temproot->left->freq+temproot->right->freq; //combine the two son's frequency
		arr.push_back(temproot);
		sort(arr.begin(),arr.end(),compar);
	}
	map<unsigned char,vector<int>> dict;
	huffman(arr[0],dict); //construct the huffman code dictionary
	ofstream output;
	output.open(ofname,ios::binary);
	//output header
	//huffman code
	for(map<unsigned char,vector<int>>::iterator it=dict.begin();it!=dict.end();++it){
		output << it->first << "=";
		cpsize+=2;
		for(int i=it->second.size()-1;i>=0;--i){ output << it->second[i]; ++cpsize; }
		output << endl;
	}
	//output context
	input.open(ifname,ios::binary);
	string s="";
	while(input >> a)
		for(int i=dict[a].size()-1;i>=0;--i) s+=(dict[a][i]+'0');
	formbyte(s,output);
	input.close();
	output.close();
}

string convert_to_binary(unsigned char c){
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

void decompress(string ifname, string ofname){
	ifstream input; //read compressed file
	ofstream output; //output decompressed file
	input.open(ifname,ios::binary);
	output.open(ofname,ios::binary);
	input.unsetf(ios::skipws);
	map<string,unsigned char> dict; //read the huffman code
	string in=""; //read dictionary lines
	unsigned char ch1, ch2;
	int k=4; //ignore header
	int addzero;
	while(input >> ch1 >> ch2){
		if(ch2=='=') while(input >> ch2){
			if(ch2!='\n') in+=ch2;
			else{ dict[in]=ch1; in=""; break;}
		}
		else{
			in+=ch1;
			while(ch2!='\n'){ in+=ch2; input>> ch2;}
			if(k==4) addzero=stoi(in);
			--k;
			in="";
		}
		if(k==0) break;
	}
	//turn huffman code back to it's original form(aka decompress)
	string s="";
	while(input>>ch1){
		s+=convert_to_binary(ch1);
	}
	string temp="";
	for(int i=0;i<(int)s.size()-addzero;++i){
		temp+=s[i];
		if(dict.count(temp)){output << dict[temp]; temp="";}
	}
	input.close();
	output.close();
}
	