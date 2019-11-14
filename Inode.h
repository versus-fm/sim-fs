#pragma once
class Inode
{
public:
	Inode(int size = 0, char mode = -1, char rights = -1);
	~Inode();
	char *deconstructInode();
	void constructInode(char *data);
	void changePointer(int n, int ptr);
	int* getBlockPointers();
	int getSize();
	char getMode();
	char getRights();

	void setSize(int size);
	void setMode(char mode);
	void setRights(char rights);
private:
	int blockPointers[6];
	int size;
	char mode;
	char rights;
};

