#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <limits>

#include "file_content.h"
#include "iisf_fid.h"
#include "fd_exception.h"

const std::size_t K = 7;

typedef std::map< std::string, std::map< std::string, std::list<NodeType> > > DataType;
typedef std::map< std::string, std::list<NodeType> > PointType;

typedef struct Alpha
{
    std::string mPointName;
    double mAlpha;

    Alpha() {mAlpha = std::numeric_limits<double>::min();}
} AlphaType;

typedef struct NeighborDistance
{
    std::size_t mFaultNum;
    double mDistance;

    NeighborDistance()
    {
        mFaultNum = std::numeric_limits<std::size_t>::max();
        mDistance = std::numeric_limits<double>::max();
    }
} NeighborDistanceType;

typedef struct FaultIsolationGroupType
{
    std::string mPointName;
    std::vector<std::string> mFaultGroup;

    FaultIsolationGroupType(const std::string & pointName)
        : mPointName(pointName)
    {}
} FaultIsolationGroup;

class IisfFidWithKnn : public IisfFid
{
public:
    /* override */ void ReadData(const std::string &forderPath);
    /* override */ void HandleData();

private:
    void GetInnerSF(std::vector<double> &result);
    void GetInterSF(std::vector<double> &result);
    void GetAlpha(const std::vector<double> &vD, const std::vector<double> &vQ, std::vector<double> &result);
    void GetReducedPoint(const std::vector<AlphaType> &alphaWithPoint, std::vector<FaultIsolationGroup> &N);

private:    
    std::map<std::string, std::size_t> mFaultName2FaultNum;
    std::map<std::size_t, std::string> mFaultNum2FaultName;
    
    std::string mForderPath;
    
    DataType mFaults;
};










