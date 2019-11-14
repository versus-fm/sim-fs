#pragma once
class FilesystemObject
{
public:
	FilesystemObject();
	~FilesystemObject();
	int getInode();
	void setInode(int inode);
	virtual char *getData(int &size) = 0;
private:
	int inode;
};

