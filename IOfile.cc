/* ---------------------------------------------------------------- */
/* Copyright 2003 (c) by RWTH Aachen - Lehrstuhl fuer Informatik VI */
/* Richard Zens                                                     */
/* ---------------------------------------------------------------- */
#include "IOfile.hh"
#include "gzstream.hh"
#include <fstream>

std::istream* Ifile::open(const char* f,bool z) {
  close();
  name=std::string(f);zip=z;
  if(zip) 
    if(name!="") s=new igzstream(f);     else s=new igzstream("stdin");
  else
    if(name!="") s=new std::ifstream(f); else s=&std::cin;
  return s;
}

void Ifile::close() {
  if(s) {
    if(zip) static_cast<igzstream*>(s)->close(); 
    if(name!="") delete s;
    s=0;
  }
}


std::ostream* Ofile::open(const char* f,bool z) {
  close();
  name=std::string(f);zip=z;
  if(zip)
    if(name!="") s=new ogzstream(f);     else s=new ogzstream("stdout");
  else
    if(name!="") s=new std::ofstream(f); else s=&std::cout;
  return s;
}

void Ofile::close() {
  if(s) {
    if(zip) static_cast<ogzstream*>(s)->close();
    if(name!="") delete s;
    s=0;
  }
}
