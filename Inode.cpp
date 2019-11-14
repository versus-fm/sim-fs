#include "Inode.h"


Inode::Inode(int size, char mode, char rights)
{
	this->size = size;
	this->mode = mode;
	this->rights = rights;
	for (int i = 0; i < 6; i++)
	{
		blockPointers[i] = -1;
	}
}

Inode::~Inode()
{
}

char * Inode::deconstructInode()
{
	char *data = new char[30];
	data[0] = ((unsigned int)this->size >> 24) & 0xFF;
	data[1] = ((unsigned int)this->size >> 16) & 0xFF;
	data[2] = ((unsigned int)this->size >> 8) & 0xFF;
	data[3] = (unsigned int)this->size & 0xFF;
	data[4] = this->mode;
	data[5] = this->rights;
	for (int i = 0; i < 6; i++)
	{
		data[6 + i * 4] = ((unsigned int)this->blockPointers[i] >> 24) & 0xFF;
		data[7 + i * 4] = ((unsigned int)this->blockPointers[i] >> 16) & 0xFF;
		data[8 + i * 4] = ((unsigned int)this->blockPointers[i] >> 8) & 0xFF;
		data[9 + i * 4] = (unsigned int)this->blockPointers[i] & 0xFF;
	}
	return data;
}

void Inode::constructInode(char * data)
{
	this->size = ((unsigned char)data[0] << 24) + ((unsigned char)data[1] << 16) + ((unsigned char)data[2] << 8) + (unsigned char)data[3];
	this->mode = data[4];
	this->rights = data[5];
	for (int i = 0; i < 6; i++)
	{
		this->blockPointers[i] = ((unsigned char)data[6 + i * 4] << 24) + ((unsigned char)data[7 + i * 4] << 16) + ((unsigned char)data[8 + i * 4] << 8) + (unsigned char)data[9 + i * 4];
	}
}

void Inode::changePointer(int n, int ptr)
{
	this->blockPointers[n] = ptr;
}

int * Inode::getBlockPointers()
{
	return this->blockPointers;
}

int Inode::getSize()
{
	return this->size;
}

char Inode::getMode()
{
	return this->mode;
}

char Inode::getRights()
{
	return this->rights;
}

void Inode::setSize(int size)
{
	this->size = size;
}

void Inode::setMode(char mode)
{
	this->mode = mode;
}

void Inode::setRights(char rights)
{
	this->rights = rights;
}
