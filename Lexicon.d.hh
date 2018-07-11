#ifndef Lexicon_HH_
#define Lexicon_HH_
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <set>

#include "IOfile.hh"


using namespace std;
typedef unsigned int  Word;
typedef map < string, Word > Vcb;
typedef map < pair < Word , Word > , double > Entry;

class Lexicon{

private:
static const double zero0;//=1E-10;

public: 
  Entry entry;

  Lexicon(char* l, bool fe, Vcb& srcvcb, Vcb& tgtvcb) {read(l, fe, srcvcb, tgtvcb);}
  
  bool read(char* pLexTab, bool fe, Vcb& srcvcb, Vcb& tgtvcb);
  double getProb(Word& sWord, Word& tWord);
};

#endif
