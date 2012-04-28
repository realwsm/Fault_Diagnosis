#include<cstring>
#include<vector>
#include<string>
#include<iostream>

#define API_EXPORT

#include "fillcontent.h"
#include "fillcontentimpl.h"


API fillcontent *creatfillcontentobject() {
  return new fillcontentimpl();
}

API void destroyfillcontentobject(fillcontent *fc) {
  if (fc != NULL) {
    delete fc;
  }
}

API bool getfilename(const std::string &filepath, std::string &filename) {
  if (filepath.size() == 0) {
    std::cerr << "get filename fail!" << std::endl;
    return false;
  }
  
#ifdef WIN32
  std::size_t npos = filepath.rfind("\\");
#else
  std::size_t npos = filepath.rfind("/");
#endif

  if (npos == -1) {
    filename.assign(filepath);
  } else {
    filename.assign(filepath, npos + 1, filepath.size() - npos);
  }
  return true;
}

API bool splitfilename(const std::string &filename, std::vector<std::string> &result) {
  if (filename.size() == 0) {
    return false;
  }
  char *newpath = new char[filename.size() + 1];
  std::strcpy(newpath, filename.c_str());
  result.clear();
  char *cfaultname, *cgroupname, *cpointname, *cdatastr, *cdate;
  do {
    if ((cfaultname = std::strtok(newpath, "[")) == NULL) {
      //std::cerr << "no faultname!" << std::endl;
      break;
    }
    if ((cgroupname = std::strtok(NULL, "]")) == NULL) {
      //std::cerr << "no groupname!" << std::endl;
      break;
    }
    char *test = std::strtok(NULL, "-");
    if ((cpointname = std::strtok(test, "_")) == NULL) {
      //std::cerr << "no pointname" << std::endl;
      break;
    }
    if ((cdatastr = std::strtok(NULL, "_")) == NULL) {
      //std::cerr << "no data string" << std::endl;
      break;
    }
    if ((cdate = std::strtok(NULL, ".")) == NULL) {
      //std::cerr << "no date" << std::endl;
      break;
    }

    result.push_back(std::string(cfaultname));
    result.push_back(std::string(cgroupname));
    result.push_back(std::string(cpointname));
    result.push_back(std::string(cdatastr));
    result.push_back(std::string(cdate));
    delete [] newpath;
    return true;
  } while (0);

  delete [] newpath;
  return false;
}
