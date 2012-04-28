#ifndef FILLCONTENTIMPL_H_H
#define FILLCONTENTIMPL_H_H

#include<cstdlib>
#include<cstring>
#include<vector>
#include<string>

#include "fillcontent.h"

class fillcontentimpl : public fillcontent {
 private:
  std::vector<datatype> eigenvalues;
  std::vector<node> nodes;
  std::string forder_path;
  std::vector<std::string> filenames;
 public:
  fillcontentimpl() {}
  ~fillcontentimpl() {};

  bool loadfile(const std::string &filepath);
  void printcontent();

  bool loadnewfile(const std::string &filepath);
  void printnewcontent();

  bool readforder(const std::string &forder_path);
  bool readforder(const std::string &forder_path, std::vector<std::string> &filenames);
  void printallfordername();
  
  bool fillnodes();
 private:
  
#ifdef WIN32
  bool readwinforder(const std::string &forder_path, std::vector<std::string> &filenames);
#else
  bool readunwinforder(const std::string &forder_path, std::vector<std::string> &filenames);
#endif
  
};

#endif
