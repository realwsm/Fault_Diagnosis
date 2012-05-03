#ifndef FILLCONTENT_H_H
#define FILLCONTENT_H_H

#include <cstring>
#include <vector>
#include <string>

#include "fd_exception.h"
#ifdef WIN32
#ifdef API_EXPORT
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif
#else
#define API
#endif

const std::size_t READ_N_GROUP_DATA = 10;
const std::size_t READ_COUNT_OF_PER_GROUP = 50;
const std::size_t MIN_DATA_SIZE = 1000;

/**
 * Define the type of one date file.
 **/
typedef struct Node {
    std::size_t mGroup; //the group of the data.
    std::size_t mFaultNum; //the fault number.

    /**
     * Segment the date file to the n sampling fileds.
     * n equal to mValue.size().
     **/
    std::vector< std::vector<double> > mValue;

    void clearContent() throw()
    {
        std::vector< std::vector<double> > emptyContent;
        mValue.swap(emptyContent);
    }

    Node() : mGroup(0), mFaultNum(0) {}
} NodeType;

#ifdef __cplusplus
extern "C" {
#endif
    
API void GetFileName(const std::string &filePath, std::string &fileName) throw (ParameterErrorException);
    
API void SplitFileName(const std::string &fileName, std::vector<std::string> &result) throw (ParameterErrorException);


/**
 * Read the forder named @forderPath and get the name of the files
 * which satisfy the format. The format is the file must be end with ".out".
 * PARAM
 * @forderPath The target forder.
 * @fileNames the name of the files which are end with ".out" in the
 *            target forder.
 * RETURN
 * void
 **/
API void ReadForder(const std::string &forderPath, std::vector<std::string> &fileNames)
        throw (ParameterErrorException, ReadForderException);


/**
 * Get the content of the file. sample the point and return.
 * PARAM
 * @fileName The target file.
 * @nodeContent the node content.
 * RETURN
 * void
 **/
API void GetNodeContent(const std::string &fileName, NodeType &nodeContent) throw (ParameterErrorException, ReadContentException);
    
#ifdef __cplusplus
}
#endif

#endif
