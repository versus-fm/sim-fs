#pragma once
#include "FilesystemObject.h"
class File :
	public FilesystemObject
{
public:
	File();
	File(int inode, int size, char *data);
	~File();
	virtual char *getData(int &size);
	void setData(char *data);
	void setSize(int size);
	int getSize();
private:
	char *data;
	int size;
};

