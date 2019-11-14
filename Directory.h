#pragma once
#include "FilesystemObject.h"
#include "DirectoryLinkedList.h"
#include <vector>
#include <string>
class Directory :
	public FilesystemObject
{
public:
	Directory();
	Directory(int inode, std::string name);
	Directory(const Directory &dir);
	~Directory();
	void constructDirectory(char *data, int size);
	void setName(std::string name);
	char* deconstructDirectory(int &size);
	std::string getName();
	std::vector<int> getInodes();
	std::vector<std::string> getFileNames();
	DirectoryLinkedList& getDirectoryInfo();
	virtual char *getData(int &size);
private:
	DirectoryLinkedList directoryList;
	std::string name;
};

