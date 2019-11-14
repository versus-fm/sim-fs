#define _SCL_SECURE_NO_WARNINGS
#include "Directory.h"
#include <algorithm>
#include <iostream>



Directory::Directory()
{
}

Directory::Directory(int inode, std::string name)
{
	this->setInode(inode);
	this->name = name;
}

Directory::Directory(const Directory & dir)
{
}


Directory::~Directory()
{
}

void Directory::constructDirectory(char * data, int size)
{
	directoryList.empty();
	std::string name = "";
	int inode = -1;
	for (int i = 0; i < size; i++)
	{
		if (inode == -1)
		{
			inode = ((unsigned char)data[i] << 24) + ((unsigned char)data[i+1] << 16) + ((unsigned char)data[i+2] << 8) + (unsigned char)data[i+3];
			i += 4;
		}
		if (data[i] == 10)
		{
			directoryList.insert(name, inode);
			inode = -1;
			name = "";
		}
		else
		{
			name += data[i];
		}
	}
}

void Directory::setName(std::string name)
{
	this->name = name;
}

char * Directory::deconstructDirectory(int &size)
{
	std::vector<char> data;
	DirectoryEntry *node = directoryList.current();
	while (node != nullptr)
	{
		data.insert(data.begin(), 10);
		for (int i = std::min((int)node->name.length() - 1, 26); i >= 0; i--)
		{
			data.insert(data.begin(), node->name[i]);
		}
		char intbytes[4];
		intbytes[0] = ((unsigned int)node->inode >> 24) & 0xFF;
		intbytes[1] = ((unsigned int)node->inode >> 16) & 0xFF;
		intbytes[2] = ((unsigned int)node->inode >> 8) & 0xFF;
		intbytes[3] = (unsigned int)node->inode & 0xFF;
		for (int i = 3; i >= 0; i--)
		{
			data.insert(data.begin(), intbytes[i]);
		}
		node = directoryList.next();
	}
	directoryList.reset();
	size = data.size();
	char *bytes = new char[data.size()];
	std::copy(data.begin(), data.end(), bytes);
	return bytes;
}

std::string Directory::getName()
{
	return this->name;
}

std::vector<int> Directory::getInodes()
{
	return std::vector<int>();
}

std::vector<std::string> Directory::getFileNames()
{
	std::vector<std::string> names;
	DirectoryEntry *node = getDirectoryInfo().current();
	while (node != nullptr)
	{
		names.push_back(node->name);
		node = node->next;
	}
	return names;
}

DirectoryLinkedList & Directory::getDirectoryInfo()
{
	return this->directoryList;
}

char * Directory::getData(int &size)
{
	return deconstructDirectory(size);
}
