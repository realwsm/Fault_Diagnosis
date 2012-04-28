#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include "fillcontent.h"
#include "fillcontentimpl.h"

#ifdef WIN32
#include <atlstr.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#endif

bool fillcontentimpl::loadfile(const std::string &fillpath) {
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

void fillcontentimpl::printcontent() {
    for (size_t i = 0; i < eigenvalues.size(); i++) {
        std::cout << eigenvalues[i].identifier << ":" << std::endl;
        for (int j = 0; j < EIGENVALUE_SIZE; j++) {
            std::cout << eigenvalues[i].eigenvalue[j] << " ";
        }
        std::cout << std::endl;
    }
}


bool fillcontentimpl::loadnewfile(const std::string &path) {
    std::ifstream ifs(path.c_str());

    if (!ifs.is_open()) {
        std::cerr << "open file fail!" << std::endl;
    }

    std::string line;
    std::size_t currentline = 0;
    std::size_t totalline = 0;
    node tempnode;
    nodes.push_back(tempnode);
    while(std::getline(ifs, line)) {
        if (currentline == 0) {
            nodes[nodes.size() - 1].frenquency = std::atof(line.c_str());
            currentline++;
            continue;
        }
        if (currentline == 1) {
            totalline = std::atoi(line.c_str());
            nodes[nodes.size() - 1].value.reserve(totalline);
            currentline++;
            continue;
        }
        nodes[nodes.size() - 1].value.push_back(std::atof(line.c_str()));

        if ((currentline - 2) == totalline) {
            break;
        }
    
        currentline++;
    }
    ifs.clear();
    ifs.close();
    if ((currentline - 2) != totalline) {
        nodes.pop_back();
        std::cerr << "the file has wrong format" << std::endl;
        return false;
    }
  
    return true;
}

void fillcontentimpl::printnewcontent() {
    std::cout << "the file contents:" << std::endl;
    std::size_t minSize = -1;
    for (std::size_t i = 0; i < nodes.size(); i++) {
#if 0
        std::cout << "nodes" << i << "'s frenquency:" << nodes[i].frenquency << std::endl;
        std::cout << "nodes" << i << "'s size:" << nodes[i].value.size() << std::endl;

        if (nodes[i].value.size() < minSize) {
            minSize = nodes[i].value.size();
        }
#endif

        if (nodes[i].value.size() == 1000) {
            for (std::size_t j = 0; j < nodes[i].filename.size(); ++j) {
                std::cout << nodes[i].filename[j] << std::endl;
                std::cout << "+++++++++++++++++++++" << std::endl;
            }
        }
    
#if 0
        std::cout << "values:" << std::endl;
        std::cout << "|";
        for (std::size_t y = 0; y < nodes[i].value.size(); y++) {
            std::cout << nodes[i].value[y] << "|";
        }
#endif    
        std::cout << std::endl;
    }
    std::cout << "minSize: " << minSize << std::endl;
}


bool fillcontentimpl::readforder(const std::string &forder_path) {
    return readforder(forder_path, this->filenames);
}

bool fillcontentimpl::readforder(const std::string &forder_path, std::vector<std::string> &filenames) {
    if (forder_path.size() == 0) {
        std::cerr << "the forder has wrong path" << std::endl;
        return false;
    }
    this->forder_path = forder_path;
    filenames.clear();

#ifdef WIN32
    return readwinforder(forder_path, filenames);  
#else
    return readunwinforder(forder_path, filenames);
#endif

}


/**
 * check the suffix of the data file.
 * The format of the file name must be end with ".out"
 * PARAM
 * @targetFileName The file name to check.
 * RETURN
 * @targetFileName is end with ".out" or not.
 * if follow the format, return true.
 * if not, return false.
 **/
inline bool CheckSuffix(const char * targetFileName)
{
    std::size_t strLength = strlen(targetFileName);
  
    /*filename must be end with ".out"*/
    if (strLength > 4 && strncmp(targetFileName + strLength - 4, ".out", 4) == 0)
    {
        return true;
    }

    return false;
}

#ifdef WIN32
bool fillcontentimpl::readwinforder(const std::string &forder_path, std::vector<std::string> &filenames) {
    filenames.clear();
    std::string szFind = forder_path, newpath = forder_path;
    if (*(szFind.end() - 1) != '\\') {
        newpath += "\\";
        szFind += "\\*.*";
    } else {
        szFind += "*.*";
    }

    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = FindFirstFile(szFind.c_str(), &FindFileData);
    if(hFind==INVALID_HANDLE_VALUE)
        return false;
    do {
        if(FindFileData.cFileName[0] == '.' )
            continue;

        if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))  {
            if (CheckSuffix(FindFileData.cFileName))
            {
                filenames.push_back(newpath + FindFileData.cFileName);
            }
        }
    }while(FindNextFile(hFind,& FindFileData));

    FindClose(hFind);
    return true;
}
#else
bool fillcontentimpl::readunwinforder(const std::string &forder_path, std::vector<std::string> &filenames) {
    struct stat statbuf;
    struct dirent *dirp;
    DIR *dp;
    std::string newpath = forder_path;
    if (lstat(newpath.c_str(), &statbuf)) {
        std::cerr << "read stat error!" << std::endl;
        return false;
    }
    if (!S_ISDIR(statbuf.st_mode)) {
        std::cerr << "it is not a directory" << std::endl;
        return false;
    }
    if (*(newpath.end() - 1) != '/') {
        newpath.push_back('/');
    }
    if ((dp = opendir(newpath.c_str())) == NULL) {
        std::cerr << "open dir fail!" << std::endl;
        return false;
    }
    while ((dirp = readdir(dp)) != NULL) {
        std::string temppath = newpath + dirp->d_name;
        if (lstat(temppath.c_str(), &statbuf)) {
            std::cerr << "read stat error!" << std::endl;
            continue;
        }
        if (S_ISREG(statbuf.st_mode) == 0) {
            //std::cerr << "it is not a directory" << std::endl;
            continue;
        }
        
        if (CheckSuffix(temppath.c_str()))
        {
            filenames.push_back(temppath);
        }
    }
    return true;
}
#endif

void fillcontentimpl::printallfordername() {
    std::cout << "filenames:" << std::endl;
    for (std::size_t i = 0; i < filenames.size(); i++) {
        std::cout << filenames[i] << std::endl;
    }
}


bool fillcontentimpl::fillnodes() {
    for (std::size_t i = 0; i < filenames.size(); i++) {
        std::string filename;
        if (!getfilename(filenames[i], filename)) {
            continue;
        }
        std::vector<std::string> tempsplitfilename;
        if (splitfilename(filename, tempsplitfilename)) {
            if (loadnewfile(filenames[i])) {
                (nodes.end() - 1)->filename = tempsplitfilename;
            } else {
                std::cerr << filenames[i] << "load fail!" << std::endl;
            }
        }
    }
    if (nodes.size() == 0) {
        std::cerr << "no node parse!" << std::endl;
        return false;
    }
    return true;
}
