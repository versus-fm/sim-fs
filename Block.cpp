#include "Block.h"



Block::Block()
{
	this->size = 512;
	this->block = new char[512];
}

Block::Block(const Block & block)
{
	this->size = block.size;
	this->block = new char[this->size];
	for (int i = 0; i < this->size; ++i)
	{
		this->block[i] = block.block[i];
	}
}


Block::~Block()
{
	delete[] block;
}

int Block::blockSize()
{
	return this->size;
}

void Block::reset(char c) {
	for (int i = 0; i < this->size; ++i)
		this->block[i] = c;
}


int Block::writeBlock(const std::string & strBlock)
{
	int status = -2;
	if (strBlock.size() == (unsigned long)this->size) {
		for (int i = 0; i < this->size; ++i) {
			this->block[i] = strBlock[i];
		}
		status = 1;
	}
	return status;
}

int Block::writeBlock(const std::vector<char>& vec)
{
	int status = -2;
	if (vec.size() == (unsigned long)this->size) {
		for (unsigned long int i = 0; i < vec.size(); ++i) {
			this->block[i] = vec[i];
		}
		status = 1;
	}
	return status;
}

void Block::writeBlock(const char cArr[])
{
	for (int i = 0; i < this->size; ++i) {
		this->block[i] = cArr[i];
	}
}

Block Block::readBlock()
{
	return Block(*this);
}

char * Block::getData()
{
	char *data = new char[size];
	for (int i = 0; i < size; i++)
	{
		data[i] = this->block[i];
	}
	return data;
}

Block & Block::operator=(const Block & block)
{
	delete[] this->block;
	this->size = block.size;
	this->block = new char[this->size];
	for (int i = 0; i < this->size; ++i)
		this->block[i] = block.block[i];
	return *this;
}

char Block::operator[](int index)
{
	if (index < 0 || index >= this->size) {
		throw std::out_of_range("Illegal access\n");
	}
	else {
		return this->block[index];
	}
}

std::string Block::toString() {
	std::string output;
	output.reserve(this->size);
	for (int i = 0; i < this->size; ++i)
		output += this->block[i];
	return output;
}
