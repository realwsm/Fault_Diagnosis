#pragma once
#include<cstdlib>
#include<vector>
#include<string>

#define EIGENVALUE_SIZE 25

typedef struct DataType{
	int identifier;
	double eigenvalue[EIGENVALUE_SIZE];

	DataType() {
		identifier = -1;
		std::memset(eigenvalue, 0, EIGENVALUE_SIZE * sizeof(double));
	}
} datatype;

class fillcontent
{
private:
	std::vector<datatype> eigenvalues;
public:
	fillcontent(void) {}
	~fillcontent(void) {};

	bool loadfile(std::string filepath);
	void printcontent();
};

