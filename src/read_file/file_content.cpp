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

API void SplitFileName(const std::string &fileName, std::vector<std::string> &result) throw (ParameterErrorException)
{
    if (fileName.size() == 0)
    {
        throw ParameterErrorException();
    }
    
    char *newPath = new char[fileName.size() + 1];
    std::strcpy(newPath, fileName.c_str());
    result.clear();
    char *cFaultName = 0, *cGroupName = 0, *cPointName = 0, *cDataStr = 0, *cDate = 0;
    
    do
    {
        if ((cFaultName = std::strtok(newPath, "[")) == 0)
        {
            break;
        }
        
        if ((cGroupName = std::strtok(0, "]")) == 0)
        {
            break;
        }
        
        char *test = std::strtok(0, "-");
        
        if ((cPointName = std::strtok(test, "_")) == 0)
        {
            break;
        }
        
        if ((cDataStr = std::strtok(0, "_")) == 0)
        {
            break;
        }
        
        if ((cDate = std::strtok(0, ".")) == 0)
        {
            break;
        }

        result.push_back(std::string(cFaultName));
        result.push_back(std::string(cGroupName));
        result.push_back(std::string(cPointName));
        result.push_back(std::string(cDataStr));
        result.push_back(std::string(cDate));

    } while (0);

    delete [] newPath;
    return;
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
    bool operator() (const std::string &str1, const std::string &str2) throw()
    {
        return str1.compare(str2) <= 0;
    }
} compStr;

#ifdef WIN32
void ReadWinForder(const std::string &forderPath, std::vector<std::string> &fileNames)
        throw (ReadForderException)
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
    
    if ((dp = opendir(newPath.c_str())) == 0)
    {
        throw ReadForderException();
    }
    
    while ((dirp = readdir(dp)) != 0)
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

void ReadForder(const std::string &forderPath, std::vector<std::string> &fileNames)
        throw (ParameterErrorException, ReadForderException)
{
    if (forderPath.size() == 0) {
        throw ParameterErrorException();
    }
    
    fileNames.clear();

#ifdef WIN32
    ReadWinForder(forderPath, fileNames);  
#else
    ReadUnwinForder(forderPath, fileNames);
#endif
    
    return;
}
