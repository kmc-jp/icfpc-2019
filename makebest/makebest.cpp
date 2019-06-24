#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
using namespace std;

int main(){
  vector<bool> tobuy(300, false);
  ifstream fin("../result");
  int id;
  while(1){
    assert(fin >> id);
    if(id==-1) break;
    tobuy[id]=true;
  }
  fin.close();
  for(int id = 0; id < 300; id++){

    std::string num = std::to_string(id+1);
    std::string pad = "";
    while(pad.length()+num.length()<3) pad+="0";
    num=pad+num;

    string inputfile;
    if(tobuy[id]){
      inputfile="../benchmarker-buy/bestzip/prob-"+num+".sol";
    }else{
      inputfile="../benchmarker/bestzip/prob-"+num+".sol";
    }
    cout << inputfile << endl;
    string outputfile="best/prob-"+num+".sol";
    ifstream fin(inputfile);
    ofstream fout(outputfile);
    string s;
    assert(fin >> s);
    fout << s << flush;
    fin.close();
    fout.close();

    if(tobuy[id]){
      string buyfile="best/prob-"+num+".buy";
      ofstream fout(buyfile);
      fout << 'C' << flush;
      fout.close();
    }
  }
}