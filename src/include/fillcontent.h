#ifndef FILLCONTENT_H_H
#define FILLCONTENT_H_H

#include <cstring>
#include <vector>
#include <string>

#ifdef WIN32
#ifdef API_EXPORT
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif
#else
#define API
#endif

#define EIGENVALUE_SIZE 25

typedef struct DataType{
  int identifier;
  double eigenvalue[EIGENVALUE_SIZE];

  DataType() {
    identifier = -1;
    ::memset(eigenvalue, 0, EIGENVALUE_SIZE * sizeof(double));
  }
} datatype;

typedef struct NODE{
  std::vector<std::string> filename;
  double frenquency;
  std::vector<double> value;
  
  NODE() : frenquency(0.0) {
    filename.clear();
    value.clear();
  }
} node;

class fillcontent {
public:
  fillcontent() {};
  virtual ~fillcontent() {};
  
  virtual bool loadfile(const std::string &filepath) = 0;
  virtual void printcontent() = 0;

  virtual bool loadnewfile(const std::string &filepath) = 0;
  virtual void printnewcontent() = 0;
  
  virtual bool readforder(const std::string &forder_path) = 0;
  virtual void printallfordername() = 0;
  
  virtual bool fillnodes() = 0;
};

#ifdef __cplusplus
extern "C" {
#endif
  API bool getfilename(const std::string &filepath, std::string &filename);
  API bool splitfilename(const std::string &filename, std::vector<std::string> &result);
  API fillcontent *creatfillcontentobject();
  API void destroyfillcontentobject(fillcontent *fc);
#ifdef __cplusplus
}
#endif

#endif
