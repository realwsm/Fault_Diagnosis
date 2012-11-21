#include<cstring>
#include<vector>
#include<string>
#include<algorithm>
#include<fstream>

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

const std::size_t READ_N_GROUP_DATA = 4;
const std::size_t READ_COUNT_OF_PER_GROUP = 50;
//const std::size_t MIN_DATA_SIZE = 1000;
const std::size_t MIN_DATA_SIZE = 500;

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
        
//        if ((cDataStr = std::strtok(0, "_")) == 0)
//        {
//            break;
//        }
//        
//        if ((cDate = std::strtok(0, ".")) == 0)
//        {
//            break;
//        }
//
        if ((cDataStr = std::strtok(0, "_")) == 0)
        {
            cDataStr = "";
        }
        
        if ((cDate = std::strtok(0, ".")) == 0)
        {
            cDate = "";
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

static inline void CheckDataSize(const double &tempData) throw (ReadContentException)
{
    if (static_cast<int>(tempData) < MIN_DATA_SIZE)
    {
        throw ReadContentException();
    }

    return;
}

/* find the max and min data of the @ifs */

/**
 * find the max and min data of the @ifs
 * PARAM
 * @fullDataSize The second line of the file indict that the max data count of the file.
 * 
 **/
static void FindMaxAndMIN(std::ifstream &ifs, const int fullDataSize, double &min, double &max)
{
    if (!ifs.good())
    {
        throw ParameterErrorException();
    }

    /* In the end, it should reset the position. */
    std::streampos pos= ifs.tellg();

    double tempData = 0.0;

    /* In the function call, how much data it read */
    std::size_t count = 0;

    if (ifs >> tempData)
    {
        min = max = tempData;
        count++;
    }
    else
    {
        throw ParameterErrorException();
    }

    /* find the max and the min */
    while (ifs >> tempData)
    {
        if (tempData > max)
        {
            max = tempData;
        }

        if (tempData < min)
        {
            min = tempData;
        }
        
        count++;
    }

    /* check the format of the file is correct or not */
    if (count != fullDataSize)
    {
        throw ParameterErrorException();
    }
    
    ifs.clear();
    /* reset the position */
    ifs.seekg(pos);
    
    return;
}

/**
 * Set the Position to the first point of the next upper boundary.
 **/
static std::size_t JumpToBoundary(std::ifstream &ifs, const double min,
                                  const double max, const std::size_t maxReadSize)
        throw (ParameterErrorException)
{
    double difference = max - min;

    if (difference < 1.5)
    {
        return 0;
    }
#if 0    
    /* ----------------------------- */
    /* when (max - min) / max < 0.15, the waveform is a straight line */
    if ((max - min) == 0)
    {
        return 0;
    }

    if (max != 0.0 && (max - min) / max < 0.15)
    {
        return 0;
    }

    if (min != 0.0 && (max - min) / min < 0.15)
    {
        return 0;
    }
    /* -----------------------------*/
#endif
    
    if (!ifs.good())
    {
        throw ParameterErrorException();
    }

    double tempData = 0.0;
    std::size_t dataCount = 0;
    int filePos = ifs.tellg();

    /* skip the upper boundary */
    while (ifs >> tempData)
    {
        if (tempData < (min + (difference * 0.08)))
        {
            dataCount++;
            break;
        }
        
        dataCount++;
    }

    /* skip the lower boundary */
    while (ifs >> tempData)
    {
        if (tempData > (max - (difference * 0.08)))
        {
            dataCount++;
            break;
        }

        dataCount++;
    }


    /* reserve enough points for the next group.*/
    if (dataCount > maxReadSize)
    {
        //throw ReadContentException();
        dataCount = 0;
        if (!ifs.good())
        {
            ifs.clear();
        }
        ifs.seekg(filePos, std::ios::beg);
    }
    
    return dataCount;
}

static inline std::size_t ReadOneGroup(std::ifstream &ifs, std::vector<double> &result)
        throw (ParameterErrorException, ReadContentException)
{
    if (!ifs.is_open())
    {
        throw ParameterErrorException();
    }

    double tempData = 0.0;
    std::size_t count = 0;
    
    for (;ifs >> tempData && count < READ_COUNT_OF_PER_GROUP; count++)
    {
        result.push_back(tempData);
    }

    if (count != READ_COUNT_OF_PER_GROUP)
    {
        throw ReadContentException();
    }
    
    return count;
}

void GetNodeContent(const std::string &fileName, NodeType &nodeContent) throw (ParameterErrorException, ReadContentException)
{
    if(fileName.length() == 0)
    {
        throw ParameterErrorException();
    }

    nodeContent.clearContent();

    std::ifstream ifs(fileName.c_str());
    
    if (!ifs.is_open())
    {
        throw ReadContentException();
    }

    double tempData = 0.0;
    std::size_t dataCount = 0;
    int fullDataSize = 0;
    std::streampos dataPos;
    
    while (ifs >> tempData)
    {
        if (dataCount == 0)
        {
            ++dataCount;
            continue;
        }
        
        if (dataCount == 1)
        {
            CheckDataSize(tempData);
            fullDataSize = static_cast<int>(tempData);
            ++dataCount;
            break;
        }
    }

    try
    {
        double min = 0.0, max = 0.0;

        FindMaxAndMIN(ifs, fullDataSize, min, max);

        for (std::size_t i = 0; i < READ_N_GROUP_DATA; i++)
        {
            std::vector<double> emptyV;
            nodeContent.mValue.push_back(emptyV);
            
            dataCount += JumpToBoundary(ifs, min, max, (fullDataSize - dataCount - (READ_N_GROUP_DATA - i) * READ_COUNT_OF_PER_GROUP));   
            dataCount += ReadOneGroup(ifs, *(nodeContent.mValue.end() - 1));
        }
    }
    catch (std::exception &ex)
    {
        ifs.close();
        throw ReadContentException();
    }

    ifs.close();
    return;
}
