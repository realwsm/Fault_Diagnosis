#ifndef FD_EXCEPTION_H_H
#define FD_EXCEPTION_H_H
#include <exception>

class ParameterErrorException : public std::exception
{
public:
    const char *what() const throw()
    {
        return "ParameterErrorException occured!";
    }
};

class ReadForderException : public std::exception
{
public:
    const char *what() const throw()
    {
        return "When read forder, it occurs error!";
    }
};

class ReadContentException : public std::exception
{
public:
    const char *what() const throw()
    {
        return "when read file, it occurs error!";
    }
};

class ReadDataException : public std::exception
{
public:
    const char *what() const throw()
    {
        return "when read data from forder, it occurs error!";
    }
};

class DataFormatException : public std::exception
{
public:
    const char *what() const throw()
    {
        return "the format of the data is wrong!";
    }
};

class CalculateDistanceException : public std::exception
{
public:
    const char *what() const throw()
    {
        return "when calculate distance, it occurs error!";
    }
};

class GetInnerSFException : public std::exception
{
public:
    const char *what() const throw()
    {
        return "when eval function named GetInnerSF, it occurs error!";
    }
};

class GetAlphaException : public std::exception
{
public:
    const char *what() const throw()
    {
        return "when get alpha, it occurs error!";
    }
};
#endif






