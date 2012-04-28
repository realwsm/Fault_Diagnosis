#include "fillcontent.h"

int main() {
  fillcontent *newconfig = creatfillcontentobject();
  //std::string path("D:\\temp\\data.txt");
  //newconfig.loadfile(path);
  //newconfig.printcontent();
  //getchar();
  //std::string path("/media/000D418D00029F3E/开题/软件代码、文档/电路板4数据（10.27）/原始数据/NORMAL[1]-D6.2_data_201007210831.out");
  //std::string path("/home/wang/testpointselectionallplatform/NORMAL[1]-D6.2_data_201007210831.out");
  //newconfig.loadnewfile(path);
  //newconfig.printnewcontent();
  //std::string path("/home/w/data");
  std::string path("/media/share/tmp/data/");
  
  newconfig->readforder(path);
  newconfig->printallfordername();
  newconfig->fillnodes();
  newconfig->printnewcontent();

  destroyfillcontentobject(newconfig);
  return 0;
}
