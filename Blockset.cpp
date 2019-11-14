#include "Blockset.h"


Blockset::Blockset(const int size)
{
	this->blocks = new Block[size];
	this->bitset = std::vector<bool>(size);
	this->size = size;
}

Blockset::Blockset(const Blockset & blockset)
{
	this->size = blockset.size;
	this->blocks = new Block[this->size];
	this->bitset = std::vector<bool>(this->size);
	for (int i = 0; i < this->size; i++)
	{
		this->blocks[i] = blockset.blocks[i];
		this->bitset[i] = blockset.bitset[i];
	}
}

bool Blockset::isFree(const int i)
{
	return !this->bitset[i];
}

int * Blockset::getFreeBlocks(const int n)
{
	int *freeBlocks = new int[n];
	int blocksRetrieved = 0;
	for (int i = 0; i < size && blocksRetrieved < n; i++)
	{
		if (!this->bitset[i]) 
		{
			freeBlocks[blocksRetrieved] = i;
			blocksRetrieved++;
		}
	}
	if (blocksRetrieved < n)
	{
		delete[] freeBlocks;
		return nullptr;
	}
	else
	{
		return freeBlocks;
	}
}

void Blockset::claimBlocks(const int *n, int count)
{
	for (int i = 0; i < count; i++)
	{
		if (n[i] != -1)
		{
			this->bitset[n[i]] = true;
		}
	}
}
void Blockset::claimBlocks(std::vector<int> blocks)
{
	for (int i = 0; i < (int)blocks.size(); i++)
	{
		this->bitset[blocks[i]] = true;
	}
}
void Blockset::claimBlocks(const int index, const int length)
{
	for (int i = index; i < length; i++)
	{
		this->bitset[i] = true;
	}
}
void Blockset::freeBlocks(const int *n, int count)
{
	for (int i = 0; i < count; i++)
	{
		if(n[i] != -1) this->bitset[n[i]] = false;
	}
}

void Blockset::freeBlocks(std::vector<int> blocks)
{
	for (int i = 0; i < (int)blocks.size(); i++)
	{
		if (blocks[i] != -1) this->bitset[blocks[i]] = false;
	}
}

void Blockset::freeBlocks(const int index, const int length)
{
	for (int i = index; i < length; i++)
	{
		if(index != -1) this->bitset[i] = false;
	}
}

void Blockset::write(int block, char * data)
{
	this->blocks[block].writeBlock(data);
}

char * Blockset::read(int block)
{
	return this->blocks[block].getData();
}

std::vector<bool>& Blockset::getBitset()
{
	return this->bitset;
}

Block & Blockset::getBlock(int n)
{
	return this->blocks[n];
}

Blockset::~Blockset()
{
	delete[] this->blocks;
}

Blockset & Blockset::operator=(const Blockset & blockset)
{
	delete[] this->blocks;
	this->size = blockset.size;
	this->blocks = new Block[this->size];
	this->bitset = std::vector<bool>(this->size);
	for (int i = 0; i < this->size; i++)
	{
		this->blocks[i] = blockset.blocks[i];
		this->bitset[i] = blockset.bitset[i];
	}
	return *this;
}
