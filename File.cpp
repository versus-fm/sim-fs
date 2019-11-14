#include "File.h"


File::File()
{
	this->size = 0;
	this->data = nullptr;
	this->setInode(-1);
}

File::File(int inode, int size, char * data)
{
	this->data = data;
	this->size = size;
	this->setInode(inode);
}

File::~File()
{
	delete[] data;
}

char * File::getData(int & size)
{
	size = this->size;
	return this->data;
}

void File::setData(char * data)
{
	delete[] this->data;
	this->data = data;
}

void File::setSize(int size)
{
	this->size = size;
}

int File::getSize()
{
	return this->size;
}
