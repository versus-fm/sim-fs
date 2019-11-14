#define _SCL_SECURE_NO_WARNINGS
#include "Filesystem.h"
#include <algorithm>
#include <fstream>



Filesystem::Filesystem()
{
}


Filesystem::~Filesystem()
{
	delete this->workingDirectory;
}

void Filesystem::createFileSystem(int size)
{
	this->size = size;
	this->blockset = Blockset(size);
	this->inodeBlockOffset = 3;
	int nInodes = size;
	this->inodesPerBlock = size / 30;
	int inodeBlocksReserved = std::ceil((double)nInodes / (double)inodesPerBlock);
	this->dataBlockOffset = this->inodeBlockOffset + inodeBlocksReserved;
	this->inodeBitset = std::vector<bool>(nInodes);
	
	this->blockset.claimBlocks(0, this->dataBlockOffset);
	for (int i = inodeBlockOffset; i < inodeBlockOffset + inodeBlocksReserved; i++)
	{
		char *data = new char[size];
		for (int j = 0; j < inodesPerBlock; j++)
		{
			Inode inode;
			char *inodeData = inode.deconstructInode();
			for (int k = 0; k < 30; k++)
			{
				data[j * 30 + k] = inodeData[k];
			}
			delete[] inodeData;

		}
		this->blockset.write(i, data);
		delete[] data;
	}

	this->inodeBitset[0] = true;
	Inode node;
	node.setMode(1);
	node.setRights(1);
	this->writeInode(node, 0);
	workingDirectory = new Directory(0, "");
	workingDirectory->getDirectoryInfo().insert("..", 0);
	workingDirectory->getDirectoryInfo().insert(".", 0);
	this->writeDirectory(workingDirectory, 0);

}

void Filesystem::writeImage(std::string path)
{
	char *superblock = new char[16];
	superblock[0] = ((unsigned int)this->size >> 24) & 0xFF;
	superblock[1] = ((unsigned int)this->size >> 16) & 0xFF;
	superblock[2] = ((unsigned int)this->size >> 8) & 0xFF;
	superblock[3] = (unsigned int)this->size & 0xFF;

	superblock[4] = ((unsigned int)this->inodesPerBlock >> 24) & 0xFF;
	superblock[5] = ((unsigned int)this->inodesPerBlock >> 16) & 0xFF;
	superblock[6] = ((unsigned int)this->inodesPerBlock >> 8) & 0xFF;
	superblock[7] = (unsigned int)this->inodesPerBlock & 0xFF;

	superblock[8] = ((unsigned int)this->inodeBlockOffset >> 24) & 0xFF;
	superblock[9] = ((unsigned int)this->inodeBlockOffset >> 16) & 0xFF;
	superblock[10] = ((unsigned int)this->inodeBlockOffset >> 8) & 0xFF;
	superblock[11] = (unsigned int)this->inodeBlockOffset & 0xFF;

	superblock[12] = ((unsigned int)this->dataBlockOffset >> 24) & 0xFF;
	superblock[13] = ((unsigned int)this->dataBlockOffset >> 16) & 0xFF;
	superblock[14] = ((unsigned int)this->dataBlockOffset >> 8) & 0xFF;
	superblock[15] = (unsigned int)this->dataBlockOffset & 0xFF;

	this->write(0, superblock);
	delete[] superblock;

	char *inodeBitsetData = new char[size];
	for (int i = 0; i < size; i++)
	{
		if (inodeBitset[i]) inodeBitsetData[i] = 1;
		else inodeBitsetData[i] = 0;
	}

	char *blockBitsetData = new char[size];
	for (int i = 0; i < size; i++)
	{
		if (blockset.isFree(i)) blockBitsetData[i] = 0;
		else blockBitsetData[i] = 1;
	}

	this->write(1, blockBitsetData);
	this->write(2, inodeBitsetData);

	delete[] blockBitsetData;
	delete[] inodeBitsetData;


	std::ofstream output(path, std::ios_base::binary);
	for (int i = 0; i < this->size; i++)
	{
		char *data = blockset.read(i);
		output.write(data, 512);
		delete[] data;
	}
	output.close();

}

void Filesystem::readImage(std::string path)
{
	std::ifstream input;
	input.open(path, std::ios_base::binary);
	int tempsize = 250;
	for (int i = 0; i < tempsize; i++)
	{
		char *data = new char[512];
		input.read(data, 512);
		if (i == 0)
		{
			this->size = ((unsigned char)data[0] << 24) + ((unsigned char)data[1] << 16) + ((unsigned char)data[2] << 8) + (unsigned char)data[3];
			this->inodesPerBlock = ((unsigned char)data[4] << 24) + ((unsigned char)data[5] << 16) + ((unsigned char)data[6] << 8) + (unsigned char)data[7];
			this->inodeBlockOffset = ((unsigned char)data[8] << 24) + ((unsigned char)data[9] << 16) + ((unsigned char)data[10] << 8) + (unsigned char)data[11];
			this->dataBlockOffset = ((unsigned char)data[12] << 24) + ((unsigned char)data[13] << 16) + ((unsigned char)data[14] << 8) + (unsigned char)data[15];
			this->blockset = Blockset(this->size);
			this->blockset.write(0, data);
			tempsize = this->size;
		}
		else if(i == 1)
		{
			for (int i = 0; i < this->size; i++)
			{
				if (data[i] == 1) this->blockset.claimBlocks(i, 1);
				else if (data[i] == 0) this->blockset.freeBlocks(i, 1);
			}
			this->blockset.write(1, data);
		}
		else if (i == 2)
		{
			this->inodeBitset = std::vector<bool>(this->size);
			for (int i = 0; i < this->size; i++)
			{
				if (data[i] == 1) this->inodeBitset[i] = true;
				else if (data[i] == 0) this->inodeBitset[i] = false;
			}
			this->blockset.write(2, data);
		}
		else
		{
			this->blockset.write(i, data);
		}
		delete[] data;
	}
	this->workingDirectory = getDirectory(0);
}

void Filesystem::write(int block, char * data)
{
	blockset.write(block, data);
}

void Filesystem::write(char * data, int * blocks, int size)
{
	int blockPointerIndex = 0;
	int block = blocks[blockPointerIndex];
	char *container = new char[512];
	bool pendingWrite = false;
	for (int i = 0; i < size; i++)
	{
		if (i % 512 == 0 && i != 0)
		{
			pendingWrite = false;
			blockset.write(block, container);
			delete[] container;
			container = new char[512];
			blockPointerIndex++;
			block = blocks[blockPointerIndex];
		}
		container[i % 512] = data[i];
		pendingWrite = true;
	}
	if (pendingWrite)
	{
		blockset.write(block, container);
		delete[] container;
	}
}

void Filesystem::claimInode(int index)
{
	this->inodeBitset[index] = true;
}

void Filesystem::freeInode(int index)
{
	this->inodeBitset[index] = false;
}

int Filesystem::getFreeInode()
{
	for (int i = 0; i < this->size; i++)
	{
		if (!this->inodeBitset[i])
		{
			return i;
		}
	}
	return -1;
}

Inode Filesystem::readInode(int index)
{
	int block = index / inodesPerBlock + inodeBlockOffset;
	int section = index % inodesPerBlock;
	char *blockdata = this->read(block);
	char *inode = new char[30];
	for (int i = section * 30; i < (section * 30) + 30; i++)
	{
		inode[i - (section * 30)] = blockdata[i];
	}
	Inode n;
	n.constructInode(inode);
	delete[] blockdata;
	delete[] inode;
	return n;
}

void Filesystem::writeInode(Inode & inode, int index)
{
	int block = index / inodesPerBlock + inodeBlockOffset;
	int section = index % inodesPerBlock;
	char *blockdata = this->read(block);
	char *inodeData = inode.deconstructInode();
	for (int i = section * 30; i < (section * 30) + 30; i++)
	{
		blockdata[i] = inodeData[i - (section * 30)];
	}
	this->blockset.write(block, blockdata);
	delete[] blockdata;
	delete[] inodeData;
}
char * Filesystem::read(int * blockPointers, int size)
{
	int bytes = size;
	int bytesRead = 0;
	char *data = new char[bytes];
	for (int i = 0; i < (int)ceil(((double)bytes / 512.0)); i++)
	{
		if (blockPointers[i] != -1)
		{
			char *blockdata = blockset.read(blockPointers[i]);
			for (int j = 0; j < 512; j++)
			{
				data[i * 512 + j] = blockdata[j];
				bytesRead++;
				if (bytesRead >= size)
				{
					delete[] blockdata;
					return data;
				}
			}
			delete[] blockdata;
		}

	}
	return data;
}

char * Filesystem::read(int block)
{
	return blockset.read(block);
}

FilesystemObject* Filesystem::createFile(Directory * parent, char mode, std::string name)
{
	if (parent == nullptr) parent = workingDirectory;
	if (parent->getDirectoryInfo().containsName(name)) {
		return getDirectory(parent->getDirectoryInfo().findInode(name));
	}
	FilesystemObject *obj = nullptr;
	int id = this->getFreeInode();
	this->claimInode(id);
	Inode inode = Inode(0, mode, 1);
	this->writeInode(inode, id);

	parent->getDirectoryInfo().insert(name, id);
	Inode wdnode = this->readInode(parent->getInode());
	this->blockset.freeBlocks(wdnode.getBlockPointers(), 6);

	//Save current directory new metadata
	int size = 0;
	char *directoryData = parent->deconstructDirectory(size);

	int neededBlocks = (int)ceil((double)size / (double)512);
	int *freeBlocks = this->blockset.getFreeBlocks(neededBlocks);
	this->blockset.claimBlocks(freeBlocks, neededBlocks);
	this->write(directoryData, freeBlocks, size);
	for (int i = 0; i < neededBlocks; i++)
	{
		wdnode.changePointer(i, freeBlocks[i]);
	}
	wdnode.setSize(size);
	this->writeInode(wdnode, parent->getInode());


	//Construct . and .. directory in new directory
	if (mode == 1)
	{
		Directory *newDir = new Directory();
		newDir->setInode(id);
		newDir->getDirectoryInfo().insert("..", parent->getInode());
		newDir->getDirectoryInfo().insert(".", id);
		this->writeDirectory(newDir, id);
		obj = newDir;
	}
	if (mode == 2)
	{
		File *file = new File(id, 0, nullptr);
		obj = file;
	}


	delete[] directoryData;
	delete[] freeBlocks;
	return obj;
}

FilesystemObject* Filesystem::createFile(char mode, std::string name)
{
	return createFile(workingDirectory, mode, name);
}

void Filesystem::changeWorkingDirectory(std::string path)
{
	int id = this->workingDirectory->getDirectoryInfo().findInode(path);
	if (id != -1)
	{
		Inode inode = this->readInode(id);
		char *dirData = this->read(inode.getBlockPointers(), inode.getSize());
		Directory *dir = new Directory(id, path);
		dir->constructDirectory(dirData, inode.getSize());
		delete[] dirData;
		delete workingDirectory;
		workingDirectory = dir;
	}

}

void Filesystem::changeWorkingDirectory(Directory * dir)
{
	delete workingDirectory;
	this->workingDirectory = dir;
}


void Filesystem::writeDirectory(Directory *dir, int id)
{
	Inode inode = this->readInode(id);
	int size = 0;
	char *newDirData = dir->deconstructDirectory(size);

	this->blockset.freeBlocks(inode.getBlockPointers(), 6);

	int neededBlocks = (int)ceil((double)size / (double)512);
	int *freeBlocks = this->blockset.getFreeBlocks(neededBlocks);
	this->blockset.claimBlocks(freeBlocks, neededBlocks);

	this->write(newDirData, freeBlocks, size);
	for (int i = 0; i < neededBlocks; i++)
	{
		inode.changePointer(i, freeBlocks[i]);
	}
	inode.setSize(size);
	this->writeInode(inode, id);
	delete[] newDirData;
	delete[] freeBlocks;
}

int Filesystem::writeFile(std::string filename, File * file)
{
	return this->writeFile(workingDirectory, filename, file);
}

int Filesystem::writeFile(Directory * dir, std::string filename, File * file)
{
	int status = -1;
	int fileInode = dir->getDirectoryInfo().findInode(filename);
	if (fileInode == -1)
	{
		fileInode = this->getFreeInode();
		this->claimInode(fileInode);
		dir->getDirectoryInfo().insert(filename, fileInode);
		writeDirectory(dir, dir->getDirectoryInfo().findInode("."));
	}

	file->setInode(fileInode);

	Inode inode = this->readInode(fileInode);
	if (inode.getRights() == 1 || inode.getRights() == 3)
	{
		this->blockset.freeBlocks(inode.getBlockPointers(), 6);
		int size = 0;
		char *data = file->getData(size);
		inode.setSize(size);

		int neededBlocks = (int)ceil((double)size / (double)512);
		int *freeBlocks = this->blockset.getFreeBlocks(neededBlocks);
		this->blockset.claimBlocks(freeBlocks, neededBlocks);

		this->write(data, freeBlocks, size);
		for (int i = 0; i < neededBlocks; i++)
		{
			inode.changePointer(i, freeBlocks[i]);
		}
		this->writeInode(inode, fileInode);
		delete[] freeBlocks;
		status = 0;
	}
	return status;
}

int Filesystem::removeFile(int id)
{
	int status = -1;
	Inode inode = this->readInode(id);
	if (inode.getRights() == 1 || inode.getRights() == 3)
	{
		this->blockset.freeBlocks(inode.getBlockPointers(), 6);
		for (int i = 0; i < 6; i++)
		{
			inode.changePointer(i, -1);
		}
		inode.setSize(0);
		inode.setMode(-1);
		inode.setRights(-1);
		this->writeInode(inode, id);
		this->freeInode(id);
		status = 0;
	}
	return status;
}

std::vector<std::string>* Filesystem::getFilesInPath(std::string path)
{
	std::string searchPath = "";
	if (path[0] == '/')
	{
		searchPath = path.substr(1);
	}
	else
	{
		searchPath = path;
	}

	char *file;
	char *context;
	file = strtok_s(&searchPath[0], "/", &context);
	std::vector<std::string> *filePaths = new std::vector<std::string>();
	while (file != nullptr)
	{
		filePaths->push_back(file);
		file = strtok_s(nullptr, "/", &context);
	}
	delete[] file;
	return filePaths;
}

Directory *Filesystem::getWorkingDirectory()
{
	return this->workingDirectory;
}

Directory * Filesystem::getDirectory(int inode)
{
	Inode node = this->readInode(inode);
	char *data = this->read(node.getBlockPointers(), node.getSize());
    

	Directory *dir = new Directory();
	dir->constructDirectory(data, node.getSize());
	std::string name = "";
	if (inode == 0)
	{
		name = "";
	}
	else
	{
		int parentID = dir->getDirectoryInfo().findInode("..");
		Inode parentNode = this->readInode(parentID);
		char *parent = this->read(parentNode.getBlockPointers(), parentNode.getSize());
		Directory parentDir;
		parentDir.constructDirectory(parent, parentNode.getSize());
		name = parentDir.getDirectoryInfo().findFileName(inode);

		delete[] parent;
	}
	dir->setInode(inode);
	dir->setName(name);
	delete[] data;
	return dir;
}

File * Filesystem::getFile(int inode, int &status, bool bypass)
{
	status = -1;
	Inode node = this->readInode(inode);
	if (node.getRights() == 2 || node.getRights() == 1 || bypass)
	{
		status = 0;
		char *data = this->read(node.getBlockPointers(), node.getSize());
		return new File(inode, node.getSize(), data);
	}
	return nullptr;
}

FilesystemObject * Filesystem::resolveTargetPath(std::string path, int offset, std::vector<std::string> *paths)
{
	Directory *dir;
	FilesystemObject *obj = nullptr;
	if (path[0] == '/')
	{
		dir = getDirectory(0);
	}
	else
	{
		dir = getDirectory(workingDirectory->getInode());
	}
	obj = dir;
	std::vector<std::string> *filePaths = nullptr;
	if (paths != nullptr)
	{
		paths = getFilesInPath(path);
		filePaths = paths;
	}
	else
	{
		filePaths = getFilesInPath(path);
	}



	for (int i = 0; i < (int)filePaths->size() - offset; i++)
	{
		Directory *old = dir;
		int id = dir->getDirectoryInfo().findInode(filePaths->at(i));
		if (id == -1)
		{
			obj = nullptr;
			delete old;
			break;
		}
		else
		{
			Inode inode = this->readInode(id);
			if (inode.getMode() == 1)
			{
				dir = getDirectory(id);
				obj = dir;
				obj->setInode(id);
			}
			if (inode.getMode() == 2)
			{
				if (i == filePaths->size() - 1 - offset)
				{
					int status;
					obj = this->getFile(id, status, true);
					obj->setInode(id);
				}
				else
				{
					obj = nullptr;
				}
			}
			delete old;
		}
	}
	if(paths == nullptr)
	{
		delete filePaths;
	}
	
	return obj;
}

std::string Filesystem::getWorkingDirectoryPath()
{
	std::string path = "";
	int inode = workingDirectory->getInode();
	Directory *dir = this->getDirectory(inode);
	while (inode != 0) {
		path.insert(0, dir->getName() + "/");
		inode = dir->getDirectoryInfo().findInode("..");
		delete dir;
		dir = getDirectory(inode);
	}
	delete dir;
	path.insert(0, "/");
	return path;
}




