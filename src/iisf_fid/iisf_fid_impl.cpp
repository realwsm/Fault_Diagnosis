#include <vector>
#include <cstdlib>
#include <cmath>
#include <algorithm>

#if defined(DEBUG)
#include <iostream>
#endif

#include "iisf_fid_impl.h"
#include "file_content.h"

void IisfFidWithKnn::ReadData(const std::string &forderPath)
{
    if (forderPath.length() == 0)
    {
        throw ParameterErrorException();
    }
    
    mForderPath = forderPath;

    std::vector<std::string> fileNames;
    ReadForder(forderPath, fileNames);

    if (fileNames.size() == 0)
    {
        throw ReadDataException();
    }
    
    for (std::vector<std::string>::iterator fnIt = fileNames.begin();
         fnIt != fileNames.end();
         fnIt++)
    {
        std::string fileName;
        GetFileName(*fnIt, fileName);

        std::vector<std::string> splitedFileName;
        SplitFileName(fileName, splitedFileName);

        if (splitedFileName.size() != 5)
        {
            throw ReadDataException();
        }

        std::size_t faultNum = mFaultName2FaultNum.size();
        std::pair<std::map<std::string, std::size_t>::iterator, bool> retFaultNum = 
                mFaultName2FaultNum.insert(std::map<std::string, std::size_t>::value_type(splitedFileName[0], faultNum));

        std::pair<DataType::iterator, bool> ret = mFaults.insert(DataType::value_type(splitedFileName[0], PointType()));
        std::pair<PointType::iterator, bool> retPoint = ((ret.first)->second).insert(PointType::value_type(splitedFileName[2], std::list<NodeType>()));
        ((retPoint.first)->second).push_front(NodeType());
        ((retPoint.first)->second).begin()->mGroup = std::atoi(splitedFileName[1].c_str());
        ((retPoint.first)->second).begin()->mFaultNum = retFaultNum.first->second;
        ((retPoint.first)->second).begin()->mFilePath = *fnIt;
    }

    for (std::map<std::string, std::size_t>::iterator it = mFaultName2FaultNum.begin();
         it != mFaultName2FaultNum.end();
         it++)
    {
        mFaultNum2FaultName.insert(std::map<std::size_t, std::string>::value_type(it->second, it->first));
    }

    if (mFaultNum2FaultName.size() != mFaultName2FaultNum.size())
    {
        throw ReadDataException();
    }

    for (DataType::iterator dIt = mFaults.begin(); dIt != mFaults.end(); dIt++)
    {
        for (PointType::iterator pIt = (dIt->second).begin(); pIt != (dIt->second).end(); pIt++)
        {
            for (std::list<NodeType>::iterator lIt = (pIt->second).begin(); lIt != (pIt->second).end(); lIt++)
            {
                GetNodeContent(lIt->mFilePath, *lIt);
            }
        }
    }


#if defined(DEBUG)
#if 0    
    for (DataType::iterator dIt = mFaults.begin(); dIt != mFaults.end(); dIt++)
    {
        for (PointType::iterator pIt = (dIt->second).begin(); pIt != (dIt->second).end(); pIt++)
        {
            for (std::list<NodeType>::iterator lIt = (pIt->second).begin(); lIt != (pIt->second).end(); lIt++)
            {
                std::cout << lIt->mFilePath << std::endl;
                for (std::vector< std::vector<double> >::iterator vIt = lIt->mValue.begin(); vIt != lIt->mValue.end(); vIt++)
                {
                    for (std::vector<double>::iterator dvIt = vIt->begin(); dvIt != vIt->end(); dvIt++)
                    {
                        std::cout << *dvIt << " | ";
                    }
                    
                    std::cout << std::endl;
                }
            }
        }
    }
#endif
    std::cout << "Read data done!" << std::endl;
#endif

    //todo: check map.
    return;
}

static double CalculateDistance(const std::vector<double> &subNIt, const std::vector<double> &subN1It)
{
    if (subNIt.size() != subN1It.size())
    {
        throw CalculateDistanceException();
    }

    double squaresSum = 0.0;
    double difference = 0.0;
    
    for (std::vector<double>::const_iterator it1 = subNIt.begin(), it2 = subN1It.begin(); it1 != subNIt.end(); it1++, it2++)
    {
        difference = *it1 - *it2;
        squaresSum += (difference * difference);
    }

    return std::sqrt(squaresSum);
}


static double CalculateDistance(const std::vector< std::vector<double> >::iterator &subNIt, const std::vector< std::vector<double> >::iterator &subN1It)
{
    return CalculateDistance(*subNIt, *subN1It);
}

static double CalculateD(const PointType::iterator &dtIt, const std::size_t n)
{
    double distanceSum = 0.0;
    for (std::list<NodeType>::iterator nIt = (dtIt->second).begin(); nIt != (dtIt->second).end(); nIt++)
    {
        for (std::vector< std::vector<double> >::iterator subNIt = (nIt->mValue).begin(); subNIt != (nIt->mValue).end(); subNIt++)
        {
            for (std::list<NodeType>::iterator n1It = (dtIt->second).begin(); n1It != (dtIt->second).end(); n1It++)
            {
                for (std::vector< std::vector<double> >::iterator subN1It = (n1It->mValue).begin(); subN1It != (n1It->mValue).end(); subN1It++)
                {
                    distanceSum += CalculateDistance(subNIt, subN1It);
                }//for
            }//for
        }//for
    }//for

    double Dij = distanceSum / static_cast<double>(n * (n - 1));
    return Dij;
}

void IisfFidWithKnn::GetInnerSF(std::vector<double> &result)
{
    std::vector<double> emptyV;
    result.swap(emptyV);

    result.resize((mFaults.begin()->second).size(),  0.0);

    std::size_t n = (((mFaults.begin()->second).begin()->second).size()) * (((mFaults.begin()->second).begin()->second).begin()->mValue.size());

    if (n < 2)
    {
        throw GetInnerSFException();
    }
    
    std::vector< std::vector<double> > d(mFaults.size(), std::vector<double>((mFaults.begin()->second).size(), 0.0));
    std::vector< std::vector<double> >::iterator dFaultIt = d.begin();
    std::vector<double>::iterator dPointIt;
    std::vector<double>::iterator resultIt;

    for (DataType::iterator dtIt = mFaults.begin(); dtIt != mFaults.end(); dtIt++)
    {
        dPointIt = dFaultIt->begin();
        resultIt = result.begin();
        for (PointType::iterator ptIt = (dtIt->second).begin(); ptIt != (dtIt->second).end(); ptIt++, resultIt++)
        {
            *dPointIt = CalculateD(ptIt, n);
            *resultIt += *dPointIt;
            
            dPointIt++;
        }
        dFaultIt++;
    }

    std::size_t m = mFaults.size();

    for (resultIt = result.begin(); resultIt != result.end(); resultIt++)
    {
        *resultIt /=  static_cast<double>(m);
    }

    //std::sort(result.begin(), result.end());
    return;
}

static double CalculateQ(const DataType &faults, const std::string &key)
{
    std::vector< std::vector<double> > qij(faults.size(), std::vector<double>(((((faults.begin()->second).begin())->second).begin())->mValue.begin()->size(), 0.0));
    std::vector< std::vector<double> >::iterator qijIt = qij.begin();

    std::size_t n = (((faults.begin()->second).begin()->second).size()) * (((faults.begin()->second).begin()->second).begin()->mValue.size());
    
    if (n < 2)
    {
        throw DataFormatException();
    }

    for (DataType::const_iterator dtIt = faults.begin(); dtIt != faults.end(); dtIt++, qijIt++)
    {
        PointType::const_iterator ptIt = (dtIt->second).find(key);

        if (ptIt == (dtIt->second).end())
        {
            throw DataFormatException();
        }

        std::vector<double>::iterator qiIt = qijIt->begin();

        for (std::list<NodeType>::const_iterator lIt = (ptIt->second).begin(); lIt != (ptIt->second).end(); lIt++)
        {
            for (std::vector< std::vector<double> >::const_iterator vIt = lIt->mValue.begin(); vIt != lIt->mValue.end(); vIt++)
            {
                for (std::vector<double>::const_iterator subVIt = vIt->begin(); subVIt != vIt->end(); subVIt++, qiIt++)
                {
                    *qiIt += *subVIt;
                }

                qiIt = qijIt->begin();
            }
        }

        for (qiIt = qijIt->begin(); qiIt != qijIt->end(); qiIt++)
        {
            *qiIt /= static_cast<double>(n);
        }

    }

    double distanceSum = 0.0;
    for (std::vector< std::vector<double> >::iterator qijIt1 = qij.begin(); qijIt1 != qij.end(); qijIt1++)
    {
        for (std::vector< std::vector<double> >::iterator qijIt2 = qij.begin(); qijIt2 != qij.end(); qijIt2++)
        {
            distanceSum += CalculateDistance(qijIt1, qijIt2);
        }
    }

    std::size_t m = faults.size();

    if (m < 2)
    {
        throw DataFormatException();
    }

    double Di1 = distanceSum / static_cast<double>(m * (m - 1));
    
    return Di1;
}

void IisfFidWithKnn::GetInterSF(std::vector<double> &result)
{
    std::vector<double> emptyV;
    result.swap(emptyV);

    result.resize((mFaults.begin()->second).size(),  0.0);

    std::vector<double>::iterator rIt = result.begin();
    
    for (PointType::iterator ptIt = (mFaults.begin()->second).begin(); ptIt != (mFaults.begin()->second).end(); ptIt++, rIt++)
    {
        *rIt = CalculateQ(mFaults, ptIt->first);
    }

    return;
}

void IisfFidWithKnn::GetAlpha(const std::vector<double> &vD, const std::vector<double> &vQ, std::vector<double> &result)
{
    if (vD.size() != vQ.size())
    {
        throw GetAlphaException();
    }
    
    double maxD = *(std::max_element(vD.begin(), vD.end()));

    if (maxD == 0.0)
    {
        throw GetAlphaException();
    }

    double minQ = *(std::min_element(vQ.begin(), vQ.end()));
    
    std::vector<double> emptyV;
    result.swap(emptyV);

    result.resize(vD.size());
    std::vector<double>::iterator rIt = result.begin();
    
    for (std::vector<double>::const_iterator vDIt = vD.begin(), vQIt = vQ.begin();
         vDIt != vD.end();
         vDIt++, vQIt++, rIt++)
    {
        if (*vDIt == 0.0)
        {
            throw GetAlphaException();
        }

        *rIt = (*vQIt / *vDIt) * (minQ / maxD);
    }

    return;
}

struct FDCompAlphaType
{
    bool operator() (const AlphaType &alphaType1, const AlphaType &alphaType2)
    {
        return (alphaType1.mAlpha > alphaType2.mAlpha);
    }
} FDCompAlpha;

static void ConvertAlpha(const DataType &faults,
                         const std::vector<double> &resultAlpha,
                         std::vector<AlphaType> &resultAlphaWithPoint)
{
    std::vector<AlphaType> emptyV;
    resultAlphaWithPoint.swap(emptyV);
    resultAlphaWithPoint.resize(resultAlpha.size());

    std::vector<AlphaType>::iterator atIt = resultAlphaWithPoint.begin();
    PointType::const_iterator ptIt = faults.begin()->second.begin();
    
    for (std::vector<double>::const_iterator it = resultAlpha.begin(); it != resultAlpha.end(); it++, ptIt++, atIt++)
    {
        atIt->mPointName = ptIt->first;
        atIt->mAlpha = *it;
    }

    return;
}

struct FDKnnCompareType
{
    bool operator() (const NeighborDistanceType &nDT1, const NeighborDistanceType &nDT2)
    {
        return nDT1.mDistance < nDT2.mDistance;
    }
} FDKnnComp;

struct FDKnnMapCompareType
{
    bool operator() (const std::map<std::size_t, std::size_t>::value_type & mVT1, const std::map<std::size_t, std::size_t>::value_type & mVT2)
    {
        return mVT1.second < mVT2.second;
    }
} FDKnnMapComp;
    
    
static std::size_t Knn(const DataType &faults,
                       const std::string &key,
                       const std::size_t trainingQuantity,
                       const std::vector<double> &instance)
{
    std::vector<NeighborDistanceType> neightborDistances;

    PointType::const_iterator ptIt;
    std::list<NodeType>::const_iterator ntIt;
    
    for (DataType::const_iterator dtIt = faults.begin(); dtIt != faults.end(); dtIt++)
    {
        ntIt = dtIt->second.find(key)->second.begin();

        for (std::size_t i = 0; i < trainingQuantity; i++, ntIt++)
        {
            for (std::vector< std::vector<double> >::const_iterator vIt = ntIt->mValue.begin(); vIt != ntIt->mValue.end(); vIt++)
            {
                neightborDistances.push_back(NeighborDistanceType());
                (neightborDistances.end() - 1)->mDistance = CalculateDistance(*vIt, instance);
                (neightborDistances.end() - 1)->mFaultNum = ntIt->mFaultNum;
            }
        }
        
    }

    std::sort(neightborDistances.begin(), neightborDistances.end(), FDKnnComp);
    std::map<std::size_t, std::size_t> nearestNeighborAmount;

    std::vector<NeighborDistance>::const_iterator nDIt = neightborDistances.begin();
    for (std::size_t i = 0; i < K && nDIt != neightborDistances.end(); i++, nDIt++)
    {
        std::pair<std::map<std::size_t, std::size_t>::iterator, bool> ret = nearestNeighborAmount.insert(std::pair<std::size_t, std::size_t>(nDIt->mFaultNum, 1));

        if (!ret.second)
        {
            (ret.first->second)++;
        }
    }

    std::map<std::size_t, std::size_t>::iterator maxElementIt = std::max_element(nearestNeighborAmount.begin(), nearestNeighborAmount.end(), FDKnnMapComp);

    return maxElementIt->first;
}

static void DiagnoseWithKnn(const DataType &faults,
                            const std::map<std::size_t, std::string> &faultNum2FaultName,
                            const std::string &key,
                            std::vector<std::string> &pointFaults)
{
    std::size_t trainingQuantity = ((faults.begin()->second.begin()->second.size() + 1) / 5) * 4;
    PointType::const_iterator ptIt;

    for (DataType::const_iterator dtIt = faults.begin(); dtIt != faults.end(); dtIt++)
    {
        ptIt = dtIt->second.find(key);
        
        std::list<NodeType>::const_reverse_iterator ntIt = ptIt->second.rbegin();

        bool state = true;
        for (std::size_t  i = 0; i < ptIt->second.size() - trainingQuantity; i++, ntIt++)
        {
            for (std::vector< std::vector<double> >::const_iterator vIt = ntIt->mValue.begin(); vIt != ntIt->mValue.end(); vIt++)
            {
                std::size_t diagnoseResult = Knn(faults, key, trainingQuantity, *vIt);
#if 0
                std::cout << diagnoseResult << std::endl;
                std::cout << faultNum2FaultName.find(diagnoseResult)->second << std::endl;
                std::cout << dtIt->first << std::endl;
#endif
                if (faultNum2FaultName.find(diagnoseResult)->second != dtIt->first)
                {
                    state = false;
                }
            }
        }

        if (state)
        {
            pointFaults.push_back(dtIt->first);
        }
    }

    return;
}

void IisfFidWithKnn::GetReducedPoint(const std::vector<AlphaType> &alphaWithPoint, std::vector<FaultIsolationGroup> &N)
{
    std::vector<FaultIsolationGroup> emptyV;
    N.swap(emptyV);

    std::set<std::string> subFaultSet;

    for (std::vector<AlphaType>::const_iterator atIt = alphaWithPoint.begin(); atIt != alphaWithPoint.end(); atIt++)
    {
        N.push_back(FaultIsolationGroup(atIt->mPointName));
        DiagnoseWithKnn(mFaults, mFaultNum2FaultName, atIt->mPointName, (N.end() - 1)->mFaultGroup);

        bool state = false;
        std::pair<std::set<std::string>::iterator, bool> ret;
        for (std::vector<std::string>::const_iterator vIt = (N.end() - 1)->mFaultGroup.begin(); vIt != (N.end() - 1)->mFaultGroup.end(); vIt++)
        {
            ret = subFaultSet.insert(*vIt);

            if (ret.second)
            {
                state = true;
            }
        }

        if (!state)
        {
            N.pop_back();
        }

        if (subFaultSet.size() == mFaults.size())
        {
            std::cout << "subFaultSet size: " << subFaultSet.size() << std::endl;
            return;
        }
    }

#if defined(DEBUG)    
    std::cout << "subFaultSet size: " << subFaultSet.size() << std::endl;
    for (std::set<std::string>::iterator it = subFaultSet.begin(); it != subFaultSet.end(); it++)
    {
        std::cout << *it << '|';
    }
    std::cout << std::endl;
#endif    
    
    return;
}

void IisfFidWithKnn::HandleData()
{
    std::vector<double> resultD;
    GetInnerSF(resultD);

    std::vector<double> resultQ;
    GetInterSF(resultQ);

    std::vector<double> resultAlpha;
    GetAlpha(resultD, resultQ, resultAlpha);

    std::vector<AlphaType> resultAlphaWithPoint;
    ConvertAlpha(mFaults, resultAlpha, resultAlphaWithPoint);

    std::sort(resultAlphaWithPoint.begin(), resultAlphaWithPoint.end(), FDCompAlpha);
    
    std::vector<FaultIsolationGroup> N;
    GetReducedPoint(resultAlphaWithPoint, N);
    
#if defined(DEBUG)

    std::cout << "fault size: " << mFaults.size() << std::endl;
    std::cout << "point size: " << mFaults.begin()->second.size() << std::endl;
    std::cout << "list size: " << mFaults.begin()->second.begin()->second.size() << std::endl;
    std::cout << "instance size: " << mFaults.begin()->second.begin()->second.begin()->mValue.size() << std::endl;
    std::cout << "fault name: " << mFaults.begin()->first << std::endl;

#if 0
    std::cout << resultAlpha.size() << std::endl;
#endif

    std::cout << "----------mFaults------------" << std::endl;
    for (DataType::iterator it = mFaults.begin(); it != mFaults.end(); it++)
    {
        std::cout << it->first << std::endl;
    }

    std::cout << "----------mFaults2Num-------" << std::endl;
    for (std::map<std::string, std::size_t>::iterator it = mFaultName2FaultNum.begin(); it != mFaultName2FaultNum.end(); it++)
    {
        std::cout << it->first << " | " << it->second << std::endl;
    }

    std::cout << "----------mNum2Fault-----------" << std::endl;
    for (std::map<std::size_t, std::string>::iterator it = mFaultNum2FaultName.begin(); it != mFaultNum2FaultName.end(); it++)
    {
        std::cout << it->first << " | " << it->second << std::endl;
    }
    
    for (std::vector<double>::iterator it = resultD.begin(); it != resultD.end(); it++)
    {
        std::cout << *it << std::endl;
    }

    std::cout << "---------------------------" << std::endl;
    
    for (std::vector<double>::iterator it = resultQ.begin(); it != resultQ.end(); it++)
    {
        std::cout << *it << std::endl;
    }

    std::cout << "----------alpha------------" << std::endl;

    for (std::vector<double>::iterator it = resultAlpha.begin(); it != resultAlpha.end(); it++)
    {
        std::cout << *it << std::endl;
    }

    std::cout << "---------alpha with name----" << std::endl;
    for (std::vector<AlphaType>::iterator it= resultAlphaWithPoint.begin(); it != resultAlphaWithPoint.end(); it++)
    {
        std::cout << it->mPointName << " | " << it->mAlpha << std::endl;
    }

    std::cout << "---------fault isolation group---------" << std::endl;
    std::cout << "N size: " << N.size() << std::endl;
    
    for (std::vector<FaultIsolationGroup>::iterator it = N.begin(); it != N.end(); it++)
    {
        std::cout << it->mPointName << " | ";

        for (std::vector<std::string>::iterator subIt = it->mFaultGroup.begin(); subIt != it->mFaultGroup.end(); subIt++)
        {
            std::cout << *subIt << " | ";
            
        }
        
        std::cout << std::endl;
    }
#endif
    
}














