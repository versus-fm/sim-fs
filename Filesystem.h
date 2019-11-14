#pragma once
#include <string>
#include <vector>
#include "Blockset.h"
#include "Inode.h"
#include "Directory.h"
#include "File.h"
class Filesystem
{
public:
	Filesystem();
	~Filesystem();
	void createFileSystem(int size = 250);
	void writeImage(std::string path);
	void readImage(std::string path);
	void write(int block, char *data);
	void write(char *data, int *blocks, int size);
	void claimInode(int index);
	void freeInode(int index);
	int getFreeInode();
	Inode readInode(int index);
	void writeInode(Inode &inode, int index);
	char* read(int *blockPointers, int size);
	char* read(int block);
	FilesystemObject *createFile(Directory *parent, char mode, std::string name);
	FilesystemObject *createFile(char mode, std::string name);
	void changeWorkingDirectory(std::string path);
	void changeWorkingDirectory(Directory *dir);
	void writeDirectory(Directory *dir, int inode);
	int writeFile(std::string filename, File *file);
	int writeFile(Directory *dir, std::string filename, File *file);
	int removeFile(int inode);
	std::vector<std::string>* getFilesInPath(std::string path);
	Directory *getWorkingDirectory();
	Directory *getDirectory(int inode);
	File *getFile(int inode, int &status, bool bypass = false);
	FilesystemObject *resolveTargetPath(std::string path, int offset = 0, std::vector<std::string> *paths = nullptr);
	std::string getWorkingDirectoryPath();

private:
	Blockset blockset;
	std::vector<bool> inodeBitset;
	int dataBlockOffset;
	int inodeBlockOffset;
	int size;
	int inodesPerBlock;
	Directory *workingDirectory;
};

