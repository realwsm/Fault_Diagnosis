#include <iostream>
#include "fill_content.h"

int main() {
  FaultDiagnosis::fillcontent newconfig;
  //std::string path("D:\\temp\\data.txt");
  //newconfig.loadfile(path);
  //newconfig.printcontent();
  //getchar();
  std::string path("/home/wang/share/tmp/data/NORMAL[1]-D6.2_data_201007210831.out");
  newconfig.loadnewfile(path);
  std::cout << "print"
            << std::endl
            << "-------------------------------------------------"
            << std::endl;
  newconfig.printnewcontent();
  return 0;
}
