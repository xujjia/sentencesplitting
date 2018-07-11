
#include "Lexicon.d.hh"

const double Lexicon::zero0=1E-10;

bool Lexicon::read(char* lex , bool fe, Vcb& srcvcb, Vcb& tgtvcb ){
 
  Ifile lexFile(lex);
 
  string line; Word sidx=0; Word tidx=0; Word src, tgt;

  while (getline(lexFile, line)){

    istringstream istrLexTab(line.c_str());
     
      string tabsrc, tabtgt, tabprob, aWord;
     
      if(fe){ 
	istrLexTab>>aWord>>tabtgt>>tabsrc>>tabprob; // read lexicon-inv:ibm3-ct0-inv.t.final.gz
      }else{ 
	istrLexTab>>aWord>>tabsrc>>tabtgt>>tabprob;  // read lexicon:ibm3-ct0.t.final.gz
      }

      Vcb::iterator sit=srcvcb.find(tabsrc);
      if(sit==srcvcb.end()){
	sidx++; srcvcb[tabsrc]=sidx; src=sidx;
      }else{
	src=srcvcb[tabsrc];
      }

      Vcb::iterator tit=tgtvcb.find(tabtgt);
      if(tit==tgtvcb.end()){
	tidx++; tgtvcb[tabtgt]=tidx; tgt=tidx;
      }else{
	tgt=tgtvcb[tabtgt];
      }

      entry[make_pair(src, tgt)]=atof(tabprob.c_str());
      
  }

  lexFile.close();
  return 0;
  }


double Lexicon::getProb(Word& sWord, Word& tWord){
     
    Entry::iterator position = entry.find(make_pair(sWord,tWord));
    if(position!=entry.end()){
      if(position->second!=0){
	return position->second;
      }
    }
    return zero0;
}


