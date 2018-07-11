// split sentences into subsentences no more than defined maximum sentence lengths
// Jia Xu RWTH-Aachen


#include <iostream>
#include <fstream>
#include <math.h>
#include <assert.h>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include "Lexicon.d.hh" 


double nomfac=1;
int minwords=4;

using namespace std;

typedef vector <string> Sent;
typedef vector < Word > DSent;
typedef vector < vector < double > > Table;

void readSentence(istream & input, Sent & sent, DSent & ncSent, Vcb& vcb){
  sent.clear();
  ncSent.clear();
  string word, lastword;
  while (input>>word){
    if(word=="{"){
      input>>word;
      string inCate=lastword+" {";
      sent.pop_back();
      while(word!="}"){
	inCate+=" "+word;
	input>>word;
      }
      inCate+=" } ";
      sent.push_back(inCate);
    }else{
      sent.push_back(word);
      Vcb::iterator itv=vcb.find(word);
      if(itv!=vcb.end()){
	ncSent.push_back(itv->second);
      }else{
	ncSent.push_back(0);
      }
    }
    lastword=word;
  }
}

void printTable(vector < vector < double > >& table){
  for (int j=0; j<table.size(); ++j){
    for (int i=0; i<table[j].size(); ++i){
      cout<<table[j][i]<<" ";
    }
    cout<<endl;
  }
}

double IBM1(int spos, int tpos, const vector < vector < double > > & ibm1table, bool up){

  int J=ibm1table.size();
  int I=ibm1table[0].size();

  if(spos>=J||tpos>=I){
    cerr<<"wrong length "<<spos<<" "<<J<<" "<<tpos<<" "<<I<<endl;
  }

  double pJinLog=0, Idown=double(1)/double(tpos+1);
  double pJoutLog=0, Iup=double(1)/double(I-tpos-1);


  if(up){
    for(int j=0; j<=spos; ++j){
      double pI=0;
      for(int i=0; i<=tpos; ++i){
        pI+=ibm1table[j][i];
      }
      pJinLog+=log(pI*Idown);
    }   
    for(int j=spos+1; j<J; ++j){
   double pI=0;
      for(int i=tpos+1; i<I; ++i){
        pI+=ibm1table[j][i];
      }
      pJoutLog+=log(pI*Iup);
    }
  }else{
    for(int j=0; j<=spos; ++j){
      double pI=0;
      for(int i=tpos+1;i<I; ++i){
        pI+=ibm1table[j][i];
      }
      pJinLog+=log(pI*Iup);
    }
    for(int j=spos+1; j<J; ++j){
      double pI=0;
      for(int i=0; i<=tpos; ++i){
        pI+=ibm1table[j][i];
      }
      pJoutLog+=log(pI*Idown);
    }
  }

  double p=-pJinLog*((1/(double)(spos+1)-1)*nomfac+1)-pJoutLog*((1/(double)(J-spos-1)-1)*nomfac+1);

  return p;
}


double IBM1inv(int spos, int tpos, const vector < vector < double > > & ibm1invtable, bool up){

  int J=ibm1invtable.size();
  int I=ibm1invtable[0].size();

  if(spos>=J||tpos>=I){
    cerr<<"wrong length "<<spos<<" "<<J<<" "<<tpos<<" "<<I<<endl;
  }

  double pIinLog=0, Jdown=double(1)/double(spos+1);
  double pIoutLog=0, Jup=double(1)/double(J-spos-1);

  if(up){
    for(int i=0; i<=tpos; ++i){
      double pJ=0;
      for (int j=0; j<=spos; ++j){
	pJ+=ibm1invtable[j][i];
      }
      pIinLog+=log(pJ*Jdown);
    } 
    for(int i=tpos+1; i<I; ++i){
      double pJ=0;
      for(int j=spos+1; j<J; ++j){
        pJ+=ibm1invtable[j][i];
      }
      pIoutLog+=log(pJ*Jup);
    }
  }else{
    for(int i=0; i<=tpos; ++i){
      double pJ=0;
      for(int j=spos+1;j<J; ++j){
        pJ+=ibm1invtable[j][i];
      }
      pIinLog+=log(pJ*Jup);
    }
    for(int i=tpos+1; i<I; ++i){
      double pJ=0;
      for(int j=0; j<=spos; ++j){
        pJ+=ibm1invtable[j][i];
      }
      pIoutLog+=log(pJ*Jdown);
    }
  }
  double p=-pIinLog*((1/(double)(tpos+1)-1)*nomfac+1)-pIoutLog*((1/(double)(I-tpos-1)-1)*nomfac+1);

  return p;
}



void subSent(vector < string >& sent , vector < string >& newSent, int s, int e){
  newSent.erase(newSent.begin(),newSent.end());
  for(int i=s; i<=e;++i){
    newSent.push_back(sent[i]);
  }
}
    

// loc : 0 : left bottom ; 1 : right bottom ; 2 : left upper ; 3 right upper

void subTable( const vector < vector < double > >& table, int spos, int tpos, int loc, vector < vector < double > > & newTable){

  int J=table.size();
  int I=table[0].size();

  //cout<<"sub table "<<J<<" "<<I<<" "<<spos<<" "<<tpos<<" location "<<loc<<endl;
  
  if(loc==0){
      for(int j=0; j<=spos; ++j){
	vector<double> tgtVec;
	for (int i=0; i<=tpos; ++i){
	  tgtVec.push_back(table[j][i]);
	}
	newTable.push_back(tgtVec);
      }
  }
  if(loc==1){
      for(int j=spos+1; j<J; ++j){
	vector<double> tgtVec;
	for (int i=0; i<=tpos; ++i){
	  tgtVec.push_back(table[j][i]);
	}
	newTable.push_back(tgtVec);
      }
  }
  if(loc==2){
      for(int j=0; j<=spos; ++j){
	vector<double> tgtVec;
	for (int i=tpos+1; i<I; ++i){
	  tgtVec.push_back(table[j][i]);
	}
	newTable.push_back(tgtVec);
      }
  }
  if(loc==3){
      for(int j=spos+1; j<J; ++j){
	vector<double> tgtVec;
	for (int i=tpos+1; i<I; ++i){
	  tgtVec.push_back(table[j][i]);
	}
	newTable.push_back(tgtVec);
      }
  }
}

//split sentences

void printSent(vector<string>& sent, Ofile& fo){
  for(int i=0; i< sent.size(); ++i){
    fo<<sent[i]<<" ";
  }
  fo<<"\n";
}


void split( vector < string > & sSent, vector < string > & tSent, const Table & ibm1table, const Table & ibm1invtable, int msrcl, int mtgtl, Ofile& ffo, Ofile& efo, double lambda, bool debug){


  //search the minimum score, cuz its -log(p)
  int J=sSent.size(); int I=tSent.size();
  double minScore=1E10;
  int spos; int tpos;bool isUp;
  for (int j=0; j<J-1; ++j){
    for (int i=0; i<I-1; ++i){
      double upScore=lambda*IBM1(j, i, ibm1table, true)+(1-lambda)*IBM1inv(j, i, ibm1invtable, true);
      double downScore=lambda*IBM1(j, i, ibm1table, false)+(1-lambda)*IBM1inv(j, i, ibm1invtable, false);
      if( min(upScore, downScore) < minScore ){
        minScore=min(upScore, downScore);
        spos=j;
        tpos=i;
        isUp=(upScore<=downScore);
      }
    }
  }


  assert(spos>=0);assert(tpos>=0&&tpos<=100); 

  if(isUp){
    vector<string> sleft, tleft, sright, tright;
    subSent(sSent,sleft,0,spos);
    subSent(tSent,tleft,0,tpos);
    subSent(sSent,sright,spos+1,J-1);
    subSent(tSent,tright,tpos+1,I-1);

    if((spos>msrcl||tpos>mtgtl)&&sleft.size()>1&&tleft.size()>1&&sleft.size()>=2*minwords&&tleft.size()>=2*minwords){

	Table subibm1, subibm1inv;
	subTable(ibm1table, spos, tpos, 0, subibm1);
	subTable(ibm1invtable, spos, tpos, 0, subibm1inv);
	if(debug){
	  cout<<spos<<" "<<tpos<<" "<<J<<" "<<I<<endl;
	  cout<<"subibm1 table"<<endl;printTable(subibm1);
	  cout<<"subibm1 inverse table"<<endl;printTable(subibm1inv);
	}
	split(sleft, tleft, subibm1, subibm1inv, msrcl, mtgtl, ffo, efo, lambda, debug);
    }else{
	printSent(sleft,ffo);
	printSent(tleft,efo);
    }

    if((J-spos>msrcl||I-tpos>mtgtl)&&sright.size()>1&&tright.size()>1&&sright.size()>=2*minwords&&tright.size()>=2*minwords){
    
      Table subibm1, subibm1inv;
      subTable(ibm1table, spos, tpos, 3, subibm1);
      subTable(ibm1invtable, spos, tpos, 3, subibm1inv);
      if(debug){
	cout<<spos<<" "<<tpos<<" "<<J<<" "<<I<<endl;
	cout<<"subibm1 table"<<endl;printTable(subibm1);
	cout<<"subibm1inv table"<<endl;printTable(subibm1inv);
      }
      split(sright, tright, subibm1, subibm1inv, msrcl, mtgtl, ffo, efo, lambda, debug);
    }else{
      printSent(sright,ffo);
      printSent(tright,efo);
    }
    
  }else{
    vector<string> sleft, tleft, sright, tright;

    subSent(sSent,sleft,0,spos);
    subSent(tSent,tleft,tpos+1,I-1);
    subSent(sSent,sright,spos+1,J-1);
    subSent(tSent,tright,0,tpos);
    if((spos>msrcl||I-tpos>mtgtl)&&sleft.size()>1&&tleft.size()>1&&sleft.size()>=2*minwords&&tleft.size()>=2*minwords){

      Table subibm1, subibm1inv;
      subTable(ibm1table, spos, tpos, 2, subibm1);
      subTable(ibm1invtable, spos, tpos, 2, subibm1inv);
      if(debug){
	cout<<spos<<" "<<tpos<<" "<<J<<" "<<I<<endl;
	cout<<"subibm1 table"<<endl;printTable(subibm1);
	cout<<"subibm1inv table"<<endl;printTable(subibm1inv);
      }
      split(sleft, tleft, subibm1, subibm1inv,  msrcl, mtgtl, ffo, efo, lambda, debug);
    }else{ 
      printSent(sleft,ffo);
      printSent(tleft,efo);
    }
    if((J-spos>msrcl||tpos>mtgtl)&&sright.size()>1&&tright.size()>1&&sright.size()>=2*minwords&&tright.size()>=2*minwords){

      Table subibm1, subibm1inv;
      subTable(ibm1table, spos, tpos, 1 , subibm1);
      subTable(ibm1invtable, spos, tpos, 1 , subibm1inv);
      if(debug){
	cout<<spos<<" "<<tpos<<" "<<J<<" "<<I<<endl;
	cout<<"subibm1 table"<<endl;printTable(subibm1);
	cout<<"subibm1inv table"<<endl;printTable(subibm1inv);
      }
      split(sright, tright, subibm1, subibm1inv, msrcl, mtgtl, ffo, efo, lambda, debug);
    }else{
      printSent(sright,ffo);
      printSent(tright,efo);
    }
  }
}

 

int main(int argc, char**argv) {
  
  bool zip=0;
  string src;
  string tgt;
  char* lexife;//-inv
  char* lexief;
  char* dev;
  string nullWord="NULL";
  string sout="x";
  string tout="y";
  bool debugLexicon=false;
  bool debugLexiconInv=false;
  int maxSrcLen=5;
  int maxTgtLen=5;
  double lambda=0;
  bool debug=false;

  if(argc<=1){cout<<"sentence splitting with IBM1 \n"
          <<" options:\n"
          <<"    -zip     -- gzip output\n"
    <<"    -s file  -- source file\n"
    <<"    -t file  -- target file\n"
    <<"    -lfe file -- lexicon file f/e TTable\n"
    <<"    -lef file -- inversed lexicon file e/f TTable\n"
    <<"    -os file  -- source output file\n"        
    <<"    -ot file  -- target output file\n"
          <<"    -maxSrcLen -- maximal source sentence length\n"
    <<"    -maxTgtLen -- maximal target sentence length\n" 
    <<"    -lambda -- combine lexicon lambda*P(fe)+(1-lambda)*P(ef)\n"
    <<"    -nom -- length nomalization factor 1/J*nom+1*(1-nom)\n";
      return -1;}

  for(int i=1;i<argc;++i) {
    string s(argv[i]);
    if(s=="-zip") zip=1;
    else if(s=="-s") src=string(argv[++i]);
    else if(s=="-t") tgt=string(argv[++i]);
    else if(s=="-lfe") lexife=argv[++i];
    else if(s=="-lef")  lexief=argv[++i];
    else if(s=="-os") sout=string(argv[++i]);
    else if(s=="-ot") tout=string(argv[++i]);
    else if(s=="-maxSrcLen") maxSrcLen=atoi(argv[++i]);
    else if(s=="-maxTgtLen") maxTgtLen=atoi(argv[++i]);
    else if(s=="-lambda") lambda=atof(argv[++i]);
    else if(s=="-nom") nomfac=atof(argv[++i]);
    else if(s=="-minwords") minwords=atoi(argv[++i]);
    else if(s=="-d") debug=atoi(argv[++i]);
    else if(s.size()==0) s="-h";
    else if(s=="-h") {
      cout<<"sentence splitting with IBM1 \n"
          <<" options:\n"
          <<"    -zip     -- gzip output\n"
	  <<"    -s file  -- source file\n"
	  <<"    -t file  -- target file\n"
	  <<"    -lfe file -- lexicon file f/e TTable\n"
	  <<"    -lef file -- inversed lexicon file e/f TTable\n"
	  <<"    -os file  -- source output file\n"        
	  <<"    -ot file  -- target output file\n"
          <<"    -maxSrcLen -- maximal source sentence length\n"
	  <<"    -maxTgtLen -- maximal target sentence length\n" 
	  <<"    -lambda -- combine lexicon lambda*P(fe)+(1-lambda)*P(ef)\n"
	  <<"    -nom -- length nomalization factor 1/J*nom+1*(1-nom)\n";
      return 1;}
    else {
      cerr<<"unknown option: "<<s<<endl;
      return 1;
    }
  }

 
  cout<<"wow lexicon table "<<lexife<<" "<<lexief<<endl;
  Vcb srcvcb, tgtvcb;
  Lexicon *lexfe=new Lexicon(lexife, true, srcvcb, tgtvcb);
  Lexicon *lexef=new Lexicon(lexief, false, srcvcb, tgtvcb);

 
  if(debugLexicon){
    map< pair < Word, Word >, double >::iterator it;
    for(it=lexfe->entry.begin();it!=lexfe->entry.end();++it){
      cout<<it->first.first<<" "<<it->first.second<<" "<<it->second<<endl;
    }
  }

   if(debugLexiconInv){
    map< pair < Word, Word >, double >::iterator it;
    for(it=lexef->entry.begin();it!=lexef->entry.end();++it){
      cout<<it->first.first<<" "<<it->first.second<<" "<<it->second<<endl;
    }
  }

  Ifile fIf(src);
  Ifile eIf(tgt);
  Ofile ffo(sout,zip);
  Ofile efo(tout,zip);

  cout<<"begin to read files"<<endl;
  string str, fline, eline;
  
  int lcnt=0;
 while (getline(fIf, fline)&&getline(eIf,eline)){
   lcnt+=1;
   cout<<"line "<<lcnt<<endl;
   if((lcnt%1000)==0){cout<<"split sentences "<<lcnt<<endl;}
     istringstream isf(fline.c_str());
     istringstream ise(eline.c_str());
     Sent sSent, tSent; DSent sncSent, tncSent; 
     readSentence(isf,sSent,sncSent,srcvcb);
     readSentence(ise,tSent,tncSent,tgtvcb);	  

     if(debug){
     cout<<"source nc"<<endl;
     for(int i=0; i<sncSent.size();++i){
       cout<<sncSent[i]<<" ";
     }
     cout<<endl;
     cout<<"target nc"<<endl;
     for(int i=0; i<tncSent.size();++i){
       cout<<tncSent[i]<<" ";
     }
     cout<<endl;
     cout<<"source"<<endl;
     for(int i=0; i<sSent.size();++i){
       cout<<sSent[i]<<" ";
     }
     cout<<endl;
     cout<<"target"<<endl;
     for(int i=0; i<tSent.size();++i){
       cout<<tSent[i]<<" ";
     }
     cout<<endl;
     }


     int J=sncSent.size();
     int I=tncSent.size();

     assert(sncSent.size()==J&&tncSent.size()==I);

     if((J>maxSrcLen||I>maxTgtLen)&&J>1&&I>1){
   
     vector < vector < double > > ibm1table;
     vector < vector < double > > ibm1tableInv;
	     
     for (int jtab=0; jtab<J; ++jtab){
       vector<double> col, colInv;
       for (int itab=0; itab<I; ++itab){
	 col.push_back(lexfe->getProb(sncSent[jtab],tncSent[itab]));
	 colInv.push_back(lexef->getProb(sncSent[jtab],tncSent[itab]));
       }
       ibm1table.push_back(col);
       ibm1tableInv.push_back(colInv);
     }
     if(debug){
       cout<<"*******ibm1 original table***************"<<endl;
       printTable(ibm1table);
       cout<<"*******ibm1 invesed original table***************"<<endl;
       printTable(ibm1tableInv);
       cout<<"*****************************************"<<endl;
     }
     split(sSent, tSent, ibm1table, ibm1tableInv, maxSrcLen, maxTgtLen, ffo, efo, lambda, debug);
     }else{
       printSent(sSent,ffo);
       printSent(tSent,efo); 
     }
 }
  
     fIf.close();
     eIf.close();
     ffo.close();
     efo.close();
     return 0;
  
 }




