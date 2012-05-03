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

#endif






