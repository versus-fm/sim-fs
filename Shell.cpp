#include <iostream>
#include "Blockset.h"
#include "Block.h"
#include "DirectoryLinkedList.h"
#include "Directory.h"
#include "Filesystem.h"
#include "Inode.h"
#include <sstream>

const int MAXCOMMANDS = 8;
const int NUMAVAILABLECOMMANDS = 16;
Filesystem sys;

std::string availableCommands[NUMAVAILABLECOMMANDS] = {
	"quit","format","ls","create","cat","createImage","restoreImage",
	"rm","cp","append","mv","mkdir","cd","pwd","help","chmod"
};

/* Takes usercommand from input and returns number of commands, commands are stored in strArr[] */
int parseCommandString(const std::string &userCommand, std::string strArr[]);
int findCommand(std::string &command);
bool quit();
std::string help();

/* More functions ... */
void listFiles(std::string path = "");
void makeDir(std::string dir);
void changeDir(std::string dir);
void moveDir(std::string src, std::string dest);
void outputWithPadding(std::string text, int positions);
void createFile(std::string path);
void printFile(std::string path);
void removeFile(std::string path);
void copyFile(std::string src, std::string dest);
void writeImage(std::string file);
void readImage(std::string file);
void append(std::string file1, std::string file2);
void changeRights(std::string file, int rights);
Directory *getDirectoryFile(std::string path, std::string &filename);
void pwd();

int main(void) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	std::string userCommand, commandArr[MAXCOMMANDS];
	std::string user = "user@DV1492";    // Change this if you want another user to be displayed
	std::string currentDir = "/";    // current directory, used for output


	bool bRun = true;

	do {
		std::cout << user << ":" << currentDir << "$ ";
		getline(std::cin, userCommand);

		int nrOfCommands = parseCommandString(userCommand, commandArr);
		if (nrOfCommands > 0) {

			int cIndex = findCommand(commandArr[0]);
			switch (cIndex) {

			case 0: //quit
				bRun = quit();
				break;
			case 1: // format
				sys.createFileSystem();
				break;
			case 2: // ls
				std::cout << "Listing directory" << std::endl;
				if(nrOfCommands < 2) listFiles();
				else listFiles(commandArr[1]);
				break;
			case 3: // create
				createFile(commandArr[1]);
				break;
			case 4: // cat
				printFile(commandArr[1]);
				break;
			case 5: // createImage
				writeImage(commandArr[1]);
				break;
			case 6: // restoreImage
				readImage(commandArr[1]);
				break;
			case 7: // rm
				removeFile(commandArr[1]);
				break;
			case 8: // cp
				copyFile(commandArr[1], commandArr[2]);
				break;
			case 9: // append
				append(commandArr[1], commandArr[2]);
				break;
			case 10: // mv
				moveDir(commandArr[1], commandArr[2]);
				break;
			case 11: // mkdir
				makeDir(commandArr[1]);
				break;
			case 12: // cd
				changeDir(commandArr[1]);
				break;
			case 13: // pwd
				pwd();
				break;
			case 14: // help
				std::cout << help() << std::endl;
				break;
			case 15: // chmod
				changeRights(commandArr[2], stoi(commandArr[1]));
				break;
			default:
				std::cout << "Unknown command: " << commandArr[0] << std::endl;
			}
		}
		sys.changeWorkingDirectory(sys.getDirectory(sys.getWorkingDirectory()->getDirectoryInfo().findInode(".")));
		currentDir = sys.getWorkingDirectoryPath();
	} while (bRun == true);

	return 0;
}

void listFiles(std::string path)
{
	Directory *directory = nullptr;
	if (path == "")
	{
		directory = sys.getDirectory(sys.getWorkingDirectory()->getInode());
	}
	else
	{
		directory = (Directory*)sys.resolveTargetPath(path);
	}

	if (directory == nullptr)
	{
		std::cout << path << " Not Found" << std::endl;
	}
	else
	{
		std::cout << "Type          Name                          Size       Permissions" << std::endl;
		DirectoryEntry *node = directory->getDirectoryInfo().current();
		while (node != nullptr)
		{
			Inode inode = sys.readInode(node->inode);
			if (node->name != "." && node->name != "..")
			{
				std::string type = "";
				if (inode.getMode() == 1)
				{
					type = "DIR";
				}
				else if (inode.getMode() == 2)
				{
					type = "FILE";
				}
				outputWithPadding(type, 14);
				std::string name = node->name;
				outputWithPadding(name, 30);
				outputWithPadding(std::to_string(inode.getSize()), 11);
				outputWithPadding(std::to_string((int)inode.getRights()), 10);
				std::cout << std::endl;
			}

			node = node->next;
		}
	}
	delete directory;
}

void makeDir(std::string dir)
{
	std::vector<std::string> *path = sys.getFilesInPath(dir);
	Directory *directory = nullptr;
	if (dir[0] == '/')
	{
		directory = sys.getDirectory(0);
	}
	for (int i = 0; i < (int)path->size(); i++)
	{
		Directory *old = directory;
		directory = (Directory*)sys.createFile(directory, 1, path->at(i));
		delete old;
	}
	delete path;
	delete directory;
}

void changeDir(std::string dir)
{
	Directory *directory = (Directory*)sys.resolveTargetPath(dir);
	if (directory == nullptr)
	{
		std::cout << dir << " Not Found" << std::endl;
	}
	else
	{
		sys.changeWorkingDirectory(directory);
	}
}

void moveDir(std::string src, std::string des)
{
	std::vector<std::string> *destpath = sys.getFilesInPath(des);
	std::string srcFileName;
	Directory *srcdir = getDirectoryFile(src, srcFileName);
	std::string dest;
	Directory *directory = getDirectoryFile(des, dest);

	if (!directory->getDirectoryInfo().containsName(dest))
	{
		int srcnode = srcdir->getDirectoryInfo().findInode(srcFileName);
		std::string srcfile = srcdir->getDirectoryInfo().findFileName(srcnode);
		srcdir->getDirectoryInfo().remove(srcfile);
		directory->getDirectoryInfo().insert(dest, srcnode);

		Inode in = sys.readInode(srcnode);
		if (in.getMode() == 1)
		{
			Directory *dirMoved = sys.getDirectory(srcnode);
			dirMoved->getDirectoryInfo().remove("..");
			dirMoved->getDirectoryInfo().insert("..", directory->getInode());
			sys.writeDirectory(dirMoved, srcnode);
			delete dirMoved;
		}
		if (directory->getInode() == srcdir->getInode())
		{
			directory->getDirectoryInfo().remove(srcfile);
			sys.writeDirectory(directory, directory->getInode());
		}
		else
		{
			sys.writeDirectory(directory, directory->getInode());
			sys.writeDirectory(srcdir, srcdir->getInode());
		}
	}
	delete directory;
	delete srcdir;
	delete destpath;
}

void outputWithPadding(std::string text, int positions)
{
	for (int i = 0; i < (int)text.length() || i < positions; i++)
	{
		if (i >= (int)text.length())
		{
			std::cout << " ";
		}
		else
		{
			std::cout << text[i];
		}
	}
}

void createFile(std::string path)
{
	std::string filename;
	Directory *directory = getDirectoryFile(path, filename);
	if (directory->getDirectoryInfo().containsName(filename))
	{
		std::cout << "File already exists" << std::endl;
		return;
	}
	File *file = (File*)sys.createFile(directory, 2, filename);
	std::string input;
	std::cout << "File content: ";
	getline(std::cin, input);
	char *data = new char[input.length()];
	for (int i = 0; i < input.length(); i++)
	{
		data[i] = input[i];
	}
	file->setData(data);
	file->setSize(input.length());
	sys.writeFile(directory, filename, file);

	delete directory;
	delete file;
}

void printFile(std::string path)
{
	std::string fileName;
	Directory *directory = getDirectoryFile(path, fileName);
	int inode = directory->getDirectoryInfo().findInode(fileName);
	if (inode == -1)
	{
		std::cout << "File doesn't exist" << std::endl;
	}
	int status;
	File *file = sys.getFile(inode, status);
	if (status == -1)
	{
		std::cout << "Permission denied" << std::endl;
		return;
	}
	int size = 0;
	char *data = file->getData(size);
	for (int i = 0; i < file->getSize(); i++)
	{
		std::cout << data[i];
	}
	std::cout << std::endl;
	delete file;
	delete directory;
}

void removeFile(std::string path)
{
	std::string file;
	Directory *directory = getDirectoryFile(path, file);
	int id = directory->getDirectoryInfo().findInode(file);
	Inode inode = sys.readInode(id);

	if (inode.getMode() == 2)
	{
		int status = sys.removeFile(id);
		if (status == -1)
		{
			std::cout << "Permission denied" << std::endl;
			delete directory;
			return;
		}
		directory->getDirectoryInfo().remove(file);
		sys.writeDirectory(directory, directory->getInode());
	}
	else
	{
		std::cout << "Not a file" << std::endl;
	}
	delete directory;
}

void copyFile(std::string src, std::string dest)
{
	std::string srcFileName;
	Directory *srcDir = getDirectoryFile(src, srcFileName);
	std::string destFileName;
	Directory *destDir = getDirectoryFile(dest, destFileName);
	
	int srcId = srcDir->getDirectoryInfo().findInode(srcFileName);
	int status;
	File *srcFile = sys.getFile(srcId, status);
	if (status == -1)
	{
		std::cout << "Permission denied" << std::endl;
		delete srcDir;
		delete destDir;
		return;
	}

	if (destDir->getDirectoryInfo().containsName(destFileName))
	{
		std::cout << "File already exists" << std::endl;
		return;
	}
	File *destFile = (File*)sys.createFile(destDir, 2, destFileName);
	int size;
	char *srcdata = srcFile->getData(size);
	char *destdata = new char[size];
	for (int i = 0; i < size; i++)
	{
		destdata[i] = srcdata[i];
	}
	destFile->setData(destdata);
	destFile->setSize(size);
	status = sys.writeFile(destDir, destFileName, destFile);
	if (status == -1)
	{
		std::cout << "Permission denied" << std::endl;
	}

	delete destFile;
	delete srcFile;
	delete srcDir;
	delete destDir;
}

void writeImage(std::string file)
{
	sys.writeImage(file);
}

void readImage(std::string file)
{
	sys.readImage(file);
}

void append(std::string file1, std::string file2)
{
	std::string srcFileName;
	Directory *srcDir = getDirectoryFile(file1, srcFileName);
	std::string destFileName;
	Directory *destDir = getDirectoryFile(file2, destFileName);


	int srcId = srcDir->getDirectoryInfo().findInode(srcFileName);
	int destId = destDir->getDirectoryInfo().findInode(destFileName);

	int status1;
	int status2;
	File *srcFile = sys.getFile(srcId, status1);
	File *destFile = sys.getFile(destId, status2);

	if (status1 == -1 || status2 == -1)
	{
		std::cout << "Permission denied" << std::endl;
		delete srcDir;
		delete destDir;
		return;
	}

	Inode inode1 = sys.readInode(srcId);
	Inode inode2 = sys.readInode(destId);

	char *newData = new char[inode1.getSize() + inode2.getSize()];
	int size;
	char *data1 = srcFile->getData(size);
	char *data2 = destFile->getData(size);

	for (int i = 0; i < inode2.getSize(); i++)
	{
		newData[i] = data2[i];
	}
	for (int i = 0; i < inode1.getSize(); i++)
	{
		newData[i + inode2.getSize()] = data1[i];
	}

	destFile->setData(newData);
	destFile->setSize(inode1.getSize() + inode2.getSize());
	int status = sys.writeFile(destDir, destFileName, destFile);
	if (status == -1)
	{
		std::cout << "Permission denied" << std::endl;
	}

	delete srcDir;
	delete destDir;
	delete srcFile;
	delete destFile;
}

void changeRights(std::string file, int rights)
{
	std::string filename;
	Directory *dir = getDirectoryFile(file, filename);

	int id = dir->getDirectoryInfo().findInode(filename);
	Inode inode = sys.readInode(id);

	inode.setRights(rights);
	sys.writeInode(inode, id);
	delete dir;
}

Directory * getDirectoryFile(std::string path, std::string & filename)
{
	std::vector<std::string> *paths = sys.getFilesInPath(path);
	Directory *directory = nullptr;
	if (path[0] == '/')
	{
		directory = sys.getDirectory(0);
	}
	else
	{
		directory = sys.getDirectory(sys.getWorkingDirectory()->getInode());
	}
	for (int i = 0; i < (int)paths->size() - 1; i++)
	{
		int inode = directory->getDirectoryInfo().findInode(paths->at(i));
		Inode in = sys.readInode(inode);
		if (in.getMode() == 2)
		{
			std::cout << "Source was a file" << std::endl;
			return nullptr;
		}
		if (inode == -1)
		{
			std::cout << "Source doesn't exist" << std::endl;
			return nullptr;
		}
		Directory *old = directory;
		directory = sys.getDirectory(inode);
		delete old;
	}
	filename = paths->at(paths->size() - 1);
	delete paths;
	return directory;
}

void pwd()
{
	std::cout << sys.getWorkingDirectoryPath() << std::endl;
}

int parseCommandString(const std::string &userCommand, std::string strArr[]) {
	std::stringstream ssin(userCommand);
	int counter = 0;
	while (ssin.good() && counter < MAXCOMMANDS) {
		ssin >> strArr[counter];
		counter++;
	}
	if (strArr[0] == "") {
		counter = 0;
	}
	return counter;
}
int findCommand(std::string &command) {
	int index = -1;
	for (int i = 0; i < NUMAVAILABLECOMMANDS && index == -1; ++i) {
		if (command == availableCommands[i]) {
			index = i;
		}
	}
	return index;
}

bool quit() {
	std::cout << "Exiting\n";
	return false;
}

std::string help() {
	std::string helpStr;
	helpStr += "OSD Disk Tool .oO Help Screen Oo.\n";
	helpStr += "-----------------------------------------------------------------------------------\n";
	helpStr += "* quit:                             Quit OSD Disk Tool\n";
	helpStr += "* format;                           Formats disk\n";
	helpStr += "* ls     <path>:                    Lists contents of <path>.\n";
	helpStr += "* create <path>:                    Creates a file and stores contents in <path>\n";
	helpStr += "* cat    <path>:                    Dumps contents of <file>.\n";
	helpStr += "* createImage  <real-file>:         Saves disk to <real-file>\n";
	helpStr += "* restoreImage <real-file>:         Reads <real-file> onto disk\n";
	helpStr += "* rm     <file>:                    Removes <file>\n";
	helpStr += "* cp     <source> <destination>:    Copy <source> to <destination>\n";
	helpStr += "* append <source> <destination>:    Appends contents of <source> to <destination>\n";
	helpStr += "* mv     <old-file> <new-file>:     Renames <old-file> to <new-file>\n";
	helpStr += "* mkdir  <directory>:               Creates a new directory called <directory>\n";
	helpStr += "* cd     <directory>:               Changes current working directory to <directory>\n";
	helpStr += "* pwd:                              Get current working directory\n";
	helpStr += "* chmod <rights> <file>:            Changes access rights for <file> to <rights>\n";
	helpStr += "* help:                             Prints this help screen\n";
	
	return helpStr;
}

/* Insert code for your shell functions and call them from the switch-case */
