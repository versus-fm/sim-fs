#include "FilesystemObject.h"



FilesystemObject::FilesystemObject()
{
	this->inode = -1;
}


FilesystemObject::~FilesystemObject()
{
}

int FilesystemObject::getInode()
{
	return this->inode;
}

void FilesystemObject::setInode(int inode)
{
	this->inode = inode;
}
