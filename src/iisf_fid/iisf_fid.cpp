#define API_EXPORT
#include "iisf_fid.h"
#include "iisf_fid_impl.h"

API IisfFid *CreatIisfFidObject()
{
    return new IisfFidWithKnn();
}

API void DestoryIisfFidObject(IisfFid * pIisfFid)
{
    if (pIisfFid != 0)
    {
        delete pIisfFid;
    }
    
    return;
}
