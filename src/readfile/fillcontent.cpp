#include "fillcontent.h"
#define API_EXPORT
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>

bool fillcontent::loadfile(std::string fillpath) {
  std::ifstream ifs(fillpath.c_str());
  if (ifs.is_open()) {
    std::string line;
    std::size_t currentline = 0;
    while (std::getline(ifs, line)) {
      currentline++;
      datatype tmpdata;
      std::stringstream sst(line);
      std::size_t datacount = 0;
      while (sst >> tmpdata.eigenvalue[datacount]) {
	if (datacount == EIGENVALUE_SIZE - 1) {
	  break;
	}
	datacount++;
      }
      if (datacount != EIGENVALUE_SIZE - 1) {
	std::cerr << "line " << currentline << "has error!" << std::endl;
	continue;
      } else {
	int tempidentifier = -1;
	tmpdata.identifier = 0;
	while (sst >> tempidentifier) {
	  //if (tempidentifier == '!') {
	  //	break;
	  //}
	  tmpdata.identifier = tmpdata.identifier * 2 + tempidentifier;
	}
	if (tempidentifier == -1) {
	  std::cerr << "line" << currentline << "has error!" << std::endl;
	  continue;
	}
      }
      eigenvalues.push_back(tmpdata);
    }
    ifs.clear();
    ifs.close();
    return true;
  } else {
    std::cerr << "open file fail!" << std::endl;
    return false;
  }
}

void fillcontent::printcontent() {
  for (size_t i = 0; i < eigenvalues.size(); i++) {
    std::cout << eigenvalues[i].identifier << ":" << std::endl;
    for (int j = 0; j < EIGENVALUE_SIZE; j++) {
      std::cout << eigenvalues[i].eigenvalue[j] << " ";
    }
    std::cout << std::endl;
  }
}

bool fillcontent::loadnewfile(std::string path) {
  std::ifstream ifs(path.c_str());
  if (!ifs.is_open()) {
    std::cerr << "open " << path << " fail!" << std::endl;
    return false;
  }
  
  std::string line;
  std::size_t currentline = 0;
  std::size_t totalline = 0;
  while(std::getline(ifs, line)) {
    node tempnode;
    if (currentline == 0) {
      tempnode.frenquency = std::atof(line.c_str());
      currentline++;
      continue;
    }
    
    if (currentline == 1) {
      totalline = std::atoi(line.c_str());
      tempnode.value.reserve(totalline);
      currentline++;
      continue;
    }
    
    tempnode.value.push_back(std::atof(line.c_str()));
    currentline++;
  }
  ifs.clear();
  ifs.close();
  if ((currentline - 1) != totalline) {
    std::cerr << "the file has wrong format" << std::endl;
    return false;
  }
  return true;
}

void fillcontent::printnewcontent() {
  for (std::size_t i = 0; i < nodes.size(); i++) {
    std::cout << "nodes" << i << "'s frenquency:"<< nodes[i].frenquency << "|" << std::endl;
    std::cout << "values:" << std::endl;
    std::cout << "|";
    for (std::size_t y = 0; y < nodes[i].value.size(); y++) {
      std::cout << nodes[i].value[y] << "|";
    }
  }
}
