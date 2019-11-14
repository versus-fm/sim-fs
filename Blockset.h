#pragma once
#include <vector>
#include "Block.h"
class Blockset
{
public:
	Blockset(const int size = 250);
	Blockset(const Blockset &blockset);
	bool isFree(const int i);
	int* getFreeBlocks(const int n);
	void claimBlocks(const int *n, int count);
	void claimBlocks(std::vector<int> blocks);
	void claimBlocks(const int index, const int length);
	void freeBlocks(const int *n, int count);
	void freeBlocks(std::vector<int> blocks);
	void freeBlocks(const int index, const int length);
	void write(int block, char *data);
	char* read(int block);
	std::vector<bool>& getBitset();
	Block& getBlock(int n);
	~Blockset();

	Blockset &operator =(const Blockset &blockset);

private:
	int size;
	std::vector<bool> bitset;
	Block *blocks;
};

