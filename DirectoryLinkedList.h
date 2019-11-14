#pragma once
#include <string>
struct DirectoryEntry
{
	DirectoryEntry(std::string name, int inode)
	{
		this->name = name;
		this->inode = inode;
		this->next = nullptr;
	}
	int inode;
	std::string name;
	DirectoryEntry *next;
};
class DirectoryLinkedList
{
public:

	DirectoryLinkedList();
	~DirectoryLinkedList();
	void insert(std::string name, int inode);
	void remove(std::string name, int inode);
	void remove(std::string name);
	void empty();
	int findInode(std::string name);
	std::string findFileName(int inode);
	bool containsName(std::string name);
	DirectoryEntry* reset();
	DirectoryEntry* current();
	DirectoryEntry* next();


private:
	DirectoryEntry *head;
	DirectoryEntry *currentEntry;

};

