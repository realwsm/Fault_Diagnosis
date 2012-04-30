#include<cstring>
#include<vector>
#include<string>
#include<algorithm>

#ifdef WIN32
#include <atlstr.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#endif

#define API_EXPORT

#include "file_content.h"

API void GetFileName(const std::string &filePath, std::string &fileName) throw (ParameterErrorException)
{
    if (filePath.size() == 0) {
        throw ParameterErrorException();
    }
  
#ifdef WIN32
    std::size_t npos = filePath.rfind("\\");
#else
    std::size_t npos = filePath.rfind("/");
#endif

    if (npos == -1) {
        fileName.assign(filePath);
    } else {
        fileName.assign(filePath, npos + 1, filePath.size() - npos);
    }
    
    return;
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
            break;
        }
        if ((cgroupname = std::strtok(NULL, "]")) == NULL) {
            break;
        }
        char *test = std::strtok(NULL, "-");
        if ((cpointname = std::strtok(test, "_")) == NULL) {
            break;
        }
        if ((cdatastr = std::strtok(NULL, "_")) == NULL) {
            break;
        }
        if ((cdate = std::strtok(NULL, ".")) == NULL) {
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
inline bool CheckSuffix(const char * targetFileName) throw()
{
    std::size_t strLength = strlen(targetFileName);
  
    /*filename must be end with ".out"*/
    if (strLength > 4 && strncmp(targetFileName + strLength - 4, ".out", 4) == 0)
    {
        return true;
    }

    return false;
}

/**
 * compare two string. use for sort.
 **/
struct CompStr
{
    bool operator() (const std::string &str1, const std::string &str2)
    {
        return str1.compare(str2) <= 0;
    }
} compStr;

#ifdef WIN32
void ReadWinForder(const std::string &forderPath, std::vector<std::string> &fileNames)
{
    fileNames.clear();
    std::string szFind = forderPath, newPath = forderPath;
    
    if (*(szFind.end() - 1) != '\\')
    {
        newPath += "\\";
        szFind += "\\*.*";
    }
    else
    {
        szFind += "*.*";
    }

    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = FindFirstFile(szFind.c_str(), &FindFileData);
    
    if(hFind==INVALID_HANDLE_VALUE)
    {
        throw ReadForderException();
    }
    
    do
    {
        if(FindFileData.cFileName[0] == '.' )
        {
            continue;
        }

        if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            if (CheckSuffix(FindFileData.cFileName))
            {
                fileNames.push_back(newPath + FindFileData.cFileName);
            }
        }
    }while(FindNextFile(hFind,& FindFileData));

    FindClose(hFind);
    std::sort(fileNames.begin(), fileNames.end(), compStr);
    return;
}
#else
void ReadUnwinForder(const std::string &forderPath, std::vector<std::string> &fileNames) throw (ReadForderException)
{
    struct stat statbuf;
    struct dirent *dirp;
    DIR *dp;
    std::string newPath = forderPath;
    
    if (lstat(newPath.c_str(), &statbuf) || !S_ISDIR(statbuf.st_mode))
    {
        throw ReadForderException();
    }
    
    if (*(newPath.end() - 1) != '/')
    {
        newPath.push_back('/');
    }
    
    if ((dp = opendir(newPath.c_str())) == NULL)
    {
        throw ReadForderException();
    }
    
    while ((dirp = readdir(dp)) != NULL)
    {
        std::string temppath = newPath + dirp->d_name;
        
        if (lstat(temppath.c_str(), &statbuf)) {
            continue;
        }
        if (S_ISREG(statbuf.st_mode) == 0) {
            continue;
        }
        
        if (CheckSuffix(temppath.c_str()))
        {
            fileNames.push_back(temppath);
        }
    }
    
    std::sort(fileNames.begin(), fileNames.end(), compStr);
    
    return;
}
#endif

void ReadForder(const std::string &forderPath, std::vector<std::string> &fileNames) {
    if (forderPath.size() == 0) {
        throw ParameterErrorException();
    }
    
    fileNames.clear();

#ifdef WIN32
    return ReadWinForder(forderPath, fileNames);  
#else
    return ReadUnwinForder(forderPath, fileNames);
#endif
}
