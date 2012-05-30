#include <iostream>
#include "file_content.h"
#include "iisf_fid.h"

int main() {
    
#if 0    
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
    //newconfig->printnewcontent();

    destroyfillcontentobject(newconfig);
#endif

#if 0
    std::vector<std::string> result;
    SplitFileName("V9OPEN[3]-U26.2_data_201007221742.out", result);

    for (std::vector<std::string>::iterator it = result.begin(); it != result.end(); ++it)
    {
        std::cout << *it << std::endl;
    }
#endif

#if 0
    NodeType node;
    GetNodeContent("/media/share/tmp/data/NORMAL[1]-Q2.E_data_201007210831.out", node);

    for (std::vector< std::vector<double> >::iterator it = node.mValue.begin(); it != node.mValue.end(); it++)
    {
        for (std::vector<double>::iterator itn = (*it).begin(); itn != (*it).end(); itn++)
        {
            std::cout << "|" << *itn;
        }
        std::cout << "|" << std::endl;
    }
#endif    
    IisfFid * pIF = CreatIisfFidObject();
    pIF->ReadData("/media/share/tmp/data/");
    pIF->HandleData();
    DestoryIisfFidObject(pIF);
    return 0;
}
