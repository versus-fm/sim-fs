#include "DirectoryLinkedList.h"



DirectoryLinkedList::DirectoryLinkedList()
{
	this->currentEntry = nullptr;
	this->head = nullptr;
}


DirectoryLinkedList::~DirectoryLinkedList()
{
	this->reset();
	while (this->currentEntry != nullptr)
	{
		DirectoryEntry *next = this->currentEntry->next;
		delete this->currentEntry;
		this->currentEntry = next;
	}
}

void DirectoryLinkedList::insert(std::string name, int inode)
{
	DirectoryEntry *current = this->currentEntry;
	this->currentEntry = new DirectoryEntry(name, inode);
	this->currentEntry->next = current;
	this->head = this->currentEntry;
}

void DirectoryLinkedList::remove(std::string name, int inode)
{
	this->reset();
	DirectoryEntry *node = this->currentEntry;
	DirectoryEntry *previous = nullptr;
	while (node != nullptr)
	{
		if (node->name == name && node->inode == inode)
		{
			DirectoryEntry *toDelete = node;
			if (previous != nullptr)
			{
				previous->next = node->next;
			}
			else
			{
				head = node->next;
			}
			delete toDelete;
			this->reset();
			return;
		}
		else
		{
			previous = node;
			node = next();
		}
		
	}
	this->reset();
}

void DirectoryLinkedList::remove(std::string name)
{
	this->reset();
	DirectoryEntry *node = this->currentEntry;
	DirectoryEntry *previous = nullptr;
	while (node != nullptr)
	{
		if (node->name == name)
		{
			DirectoryEntry *toDelete = node;
			if (previous != nullptr)
			{
				previous->next = node->next;
			}
			else
			{
				head = node->next;
			}
			delete toDelete;
			this->reset();
			return;
		}
		else
		{
			previous = node;
			node = next();
		}

	}
	this->reset();
}

void DirectoryLinkedList::empty()
{
	this->reset();
	while (this->currentEntry != nullptr)
	{
		DirectoryEntry *next = this->currentEntry->next;
		delete this->currentEntry;
		this->currentEntry = next;
	}
	this->currentEntry = nullptr;
	this->head = nullptr;
}

int DirectoryLinkedList::findInode(std::string name)
{
	DirectoryEntry *node = this->currentEntry;
	while (node != nullptr)
	{
		if (node->name == name)
		{
			return node->inode;
		}
		node = node->next;
	}
	return -1;
}

std::string DirectoryLinkedList::findFileName(int inode)
{
	DirectoryEntry *node = this->currentEntry;
	while (node != nullptr)
	{
		if (node->inode == inode)
		{
			return node->name;
		}
		node = node->next;
	}
	return "";
}


bool DirectoryLinkedList::containsName(std::string name)
{
	DirectoryEntry *node = this->currentEntry;
	while (node != nullptr)
	{
		if (node->name == name)
		{
			return true;
		}
		node = node->next;
	}
	return false;
}

DirectoryEntry * DirectoryLinkedList::reset()
{
	this->currentEntry = this->head;
	return current();
}

DirectoryEntry * DirectoryLinkedList::current()
{
	return this->currentEntry;
}

DirectoryEntry * DirectoryLinkedList::next()
{
	if(this->currentEntry != nullptr) this->currentEntry = this->currentEntry->next;
	return current();
}
