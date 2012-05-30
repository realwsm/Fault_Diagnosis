#ifndef IISF_FID_H_H
#define IISF_FID_H_H

#include <string>

#ifdef WIN32
#ifdef API_EXPORT
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif
#else
#define API
#endif

class IisfFid
{
public:
    IisfFid() {}
    virtual ~IisfFid() {}

public:
    virtual void ReadData(const std::string &forderPath) = 0;
    virtual void HandleData() = 0;
};

#if defined(_cplusplus)
extern "C" {
#endif

API IisfFid *CreatIisfFidObject();
API void DestoryIisfFidObject(IisfFid * pIisfFid);
    
#if defined(_cplusplus)
}
#endif    
#endif
