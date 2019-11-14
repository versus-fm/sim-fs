#pragma once
#include <iostream>
#include <string>
#include <vector>
class Block
{
public:
	Block();
	Block(const Block &block);
	~Block();
	int blockSize();
	void reset(char c);
	int writeBlock(const std::string &strBlock);
	int writeBlock(const std::vector<char> &vec);
	void writeBlock(const char cArr[]);
	Block readBlock();
	char* getData();

	Block &operator =(const Block &block);
	char operator[](int index);

	std::string toString();
private:
	int size;
	char *block;
};

