#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <iomanip>
#include <list>
#include <string>
#include <wtypes.h>
#include <stack>
#include <queue>
using namespace std;

// FILE CLASS

class File {
    friend class Folder;
    friend class DOS;
    friend class Tree;
public:
	string name;
	string path;
	int noOflines;
	int noOfchar;
	int avgNoOflines;
	bool readonly;
	bool hidden;
	time_t creationTime;
	string owner;
	string fileType;
	time_t timeToPrint;
	list<char> content;

public:

	File(string fileName = "null", string filePath = "", string extension = "", string Owner = "")
	{
		name = fileName;
		path = filePath;
		fileType = extension;
		readonly = false;
		hidden = false;
		owner = Owner;
		

	}

	void PrintAttribute()
	{
		cout << "File Name:" << name << endl;
		cout << "File Path:" << path << endl;
		cout << "File Owner:" << owner << endl;
		cout << "File Type:" << fileType << endl;
		cout << "Hidden:" << (hidden ? "Yes" : "No") << endl;
		cout << "Read-Only:" << (readonly ? "Yes" : "No") << endl;	
	}

	void Copy(File* destinationFile)
	{
		destinationFile->content=content;
	}

	void SetContent(const list<char>& newContent) 
	{
        content = newContent;
    }
};

class TextEditor {
public:
    list<char> content;
    deque<list<char>> undoHistory;
    deque<list<char>> redoHistory;
    size_t cursorIndex;

public:
    TextEditor() : cursorIndex(0) {}

    void printEditor() {
        system("cls");
        int i = 0;
        for (auto& ch : content) {
            if (i == cursorIndex) {
                cout << "|";
            }
            cout << ch;
            i++;
        }
        if (cursorIndex == i) {
            cout << "|";
        }
        cout << endl;
    }

    void insertCharacter(char ch) {
        content.insert(next(content.begin(), cursorIndex), ch);
        cursorIndex++;
    }

    void deleteCharacter() {
        if (cursorIndex < content.size()) {
            content.erase(next(content.begin(), cursorIndex));
        }
    }

    void backspaceCharacter() {
        if (cursorIndex > 0) {
            content.erase(prev(content.begin(), cursorIndex));
            cursorIndex--;
        }
    }

    void appendLine() {
        content.insert(next(content.begin(), cursorIndex), '\n');
        cursorIndex++;
    }

    void moveCursorLeft() {
        if (cursorIndex > 0) {
            cursorIndex--;
        }
    }

    void moveCursorRight() {
        if (cursorIndex < content.size()) {
            cursorIndex++;
        }
    }

  void moveCursorUp() {
        auto iterator = prev(content.begin(), cursorIndex);
        int count = 0;
        while (iterator != content.begin() && count < 5) {
            iterator--;
            if (*iterator == '\n') {
                count++;
            }
        }

        cursorIndex = distance(content.begin(), iterator);
    }

    void moveCursorDown() {
        auto iterator = next(content.begin(), cursorIndex);
        int count = 0;
        while (iterator != content.end() && count < 5 && *iterator != '\n') {
            iterator++;
            if (*iterator == '\n') {
                count++;
            }
        }
        cursorIndex = distance(content.begin(), iterator);
        if (cursorIndex < content.size() && *iterator == '\n') {
            cursorIndex++;
        }
    }

    void undo() {
        if (!undoHistory.empty()) {
            redoHistory.push_back(content);
            content = undoHistory.back();
            undoHistory.pop_back();
            if (cursorIndex > content.size()) {
                cursorIndex = content.size();
            }
        }
    }

    void redo() {
        if (!redoHistory.empty()) {
            undoHistory.push_back(content);
            content = redoHistory.back();
            redoHistory.pop_back();
            if (cursorIndex > content.size()) {
                cursorIndex = content.size();
            }
        }
    }

    void edit() {
        int userInput;
        while (true) {
            printEditor();

            userInput = _getch();

            if (userInput == 224) {
                userInput = _getch();
                if (userInput == 75) {
                    moveCursorLeft();
                } else if (userInput == 77) {
                    moveCursorRight();
                }
				 else if (userInput == 72) {
                    moveCursorUp();  
                } else if (userInput == 80) {
                    moveCursorDown(); 
                }

            } else if (userInput == 8) {
                undoHistory.push_back(content);
                backspaceCharacter();
            } else if (userInput == 13) {
                undoHistory.push_back(content);
                appendLine();
            } else if ((userInput >= 32 && userInput <= 126) || userInput == 9) {
                undoHistory.push_back(content);
                insertCharacter(static_cast<char>(userInput));
            } else if (userInput == 26) {
                undo();
            } else if (userInput == 25) {
                redo();
            } else {
                break;
            }
        }
    }
};

// FOLDER CLASS

class Folder {
    friend class File;
    friend class DOS;
    friend class Tree;
public:
	string name;
	Folder* Parent;
	bool hidden;
	string owner;
	string path;
	time_t creationTime;
	time_t timeToPrint;
	list<Folder*> subFolders;
	list<File*> Files;
	list<File*> printQueue;
    list<File*> priorityQueue;

public:
	Folder(string directoryName="null", string Path="", list<Folder*> subFol=list<Folder*>(), list<File*> file=list<File*>(), Folder* parent = nullptr)
	{
		name = directoryName;
		path = Path;
		subFolders = subFol;
		Files = file;
		hidden = false;
		Parent = parent;
	}

	void remove_File(string name)
	{
		for(auto file=Files.begin(); file != Files.end();file++)
		{
			if ((*file)->name == name)
			{
				file = Files.erase(file);
				break;
			}
		}
	}

	void remove_Folder(string name)
	{
		for(auto folder=subFolders.begin(); folder != subFolders.end();folder++)
		{
			if ((*folder)->name == name)
			{
			    folder = subFolders.erase(folder);
                break;
			}
		}
	}

	void EmptyDirectory()
	{
		Files.clear();
		subFolders.clear();
	}


	File* Find(string name)
	{
        list<File *>::iterator i;
		for (i = Files.begin();i!=Files.end();i++)
		{
			if ((*i)->name == name)
			{
				return (*i);
			}
		}
		return nullptr;
	}

	Folder* FindFolder(string name)
	{
		list<Folder *>::iterator i;
		for (i = subFolders.begin();i!=subFolders.end();i++)
		{
			if ((*i)->name == name)
			{
				return (*i);
			}
		}
		return nullptr;
	}

	void insertFile(File* file)
	{
		Files.insert(Files.end(), file);
	}

	void insertFolder(Folder* folder)
	{
		subFolders.insert(subFolders.end(), folder);
	}

	void AddSubfolder(Folder* folder)
	{
		subFolders.insert(subFolders.end(), folder);
		folder->path = path + "\\" + name;
	}
	void print()
	{
		cout << "Directory of" << "\\" << name << endl << endl;
		list<Folder *>::iterator i;
		for (i = subFolders.begin();i!=subFolders.end();i++)
        {
			cout << "              <DIR>       " << (*i)->name << endl;

		}
		list<File *>::iterator j;
		for (j = Files.begin();j!=Files.end();j++)
        {
			cout << "              <FILE>       " << (*j)->name << endl;

		}
	}

};

// TREE CLASS

class Tree {

    friend class File;
    friend class DOS;
    friend class Folder;
public:

	Folder* currentFolder;
	Folder* Root;

public: 

	Tree()
	{
		currentFolder = Root = new Folder("V");
	}

	void setPath(Folder* f)
	{
		currentFolder = f;
	}
	
	void print(string prompt = "\\>")
	{
		if (currentFolder->name == "V")
		{
			cout << "V:" << prompt;
		}
		else
		{
			cout << "V:" << currentFolder->path.substr(2, currentFolder->path.length()) << "\\" << currentFolder->name << prompt;
		}

	}

	void printTree(Folder* currentFolder, int level = 0)
	{
        for (int i = 0; i < level; ++i)
		{
            cout << "  ";
        }
        cout << "|--" << currentFolder->name << endl;

        for (auto subfolder : currentFolder->subFolders) 
		{
            printTree(subfolder, level + 1);
        }

        for (auto file : currentFolder->Files) 
		{
            for (int i = 0; i <= level; ++i) 
			{
                cout << "  ";  
            }
            cout << "|--" << file->name << endl;
        }
    }

};

class DOS {
    friend class File;
    friend class Folder;
    friend class Tree;
	friend class TreePrinter;
private:
	string promptString;

	string lower(string ToLowerString)
	{
		for (int i = 0;i < ToLowerString.length();i++)
		{
			ToLowerString[i] = tolower(ToLowerString[i]);
		}
		return ToLowerString;
	}

	bool prompt = false;

public :
	Tree t;
	Folder f;
    File fileObj;
	string command;
	string user = "Rida";
	DOS()
	{
		promptString="\\>";
	}


	void print()
	{
		cout << "                   ______________________"<<endl<<endl;
		cout << "\t\t\t" << "Rida Mushtaq" << "\t\t" << endl;
		cout << "\t\t\t" << "2022-CS-168" << "\t\t" << endl;
		cout << "                   _______________________"<< endl;
		t.print(promptString);
		
	}
	void editFile(File* file)
	{
		system("color 70");
        TextEditor textEditor;
        textEditor.content = file->content;
        textEditor.cursorIndex = 0;
        textEditor.edit();

        file->content = textEditor.content;
    }
	bool isMakeDir()
	{
		string opr = command.substr(0, command.find(' '));
		opr = lower(opr);
		if (opr == "mkdir") 
		{
			string folderName = command.substr(6);
        	Folder* newFolder = new Folder(folderName, t.currentFolder->path + "\\" + t.currentFolder->name);
        
        	t.currentFolder->insertFolder(newFolder);
			return true;
		}
		return false;
	}
	bool isEdit()
	{
		if (lower(command.substr(0, 4)) == "edit") 
		{
            string fileName = command.substr(5);
            File* fileToEdit = t.currentFolder->Find(fileName);
            if (fileToEdit != nullptr) {
                editFile(fileToEdit);
                cout << "File edited successfully." << endl;
            } else {
                cout << "File not found." << endl;
            }
            return true;
        }
		return false;
	}

	bool isRemoveDir()
	{
		string opr = command.substr(0, command.find(' '));
		opr = lower(opr);
		if (opr == "rmdir") 
		{
			string folderName = command.substr(6);
			Folder* folderToRemove =t.currentFolder->FindFolder(folderName);
			if(folderToRemove!=nullptr)
			{
				t.currentFolder->remove_Folder(folderName);
				delete folderToRemove;

				cout << "Directory removed successfully." << endl;
			} 
			else
			{
				cout << "Directory not found." << endl;
			}
			return true;
		}
		return false;
	}

	
	bool isDel()
	{
		string opr = command.substr(0, command.find(' '));
		opr = lower(opr);
		if (opr == "del") 
		{
			string fileName = command.substr(4);
			File* fileToDelete =t.currentFolder->Find(fileName);
			if(fileToDelete!=nullptr)
			{
				t.currentFolder->remove_File(fileName);
				delete fileToDelete;

				cout << "File Deleted successfully." << endl;
			} 
			else
			{
				cout << "File not found." << endl;
			}
			return true;
		}
		return false;
	}
	
	bool isRename()
	{
		string opr = command.substr(0, command.find(' '));
		opr = lower(opr);
		if (opr == "rename") 
		{
			string currentName = command.substr(7, command.find(' ', 7) - 7);
			string newName = command.substr(command.find(' ', 7) + 1);
			File* fileToRename =t.currentFolder->Find(currentName);
			if(fileToRename!=nullptr)
			{
				fileToRename->name=newName;

				cout << "File renamed successfully." << endl;
			} 
			else 
			{
				cout << "File not found." << endl;
			}
			return true;
		}
		return false;
	}


	bool isDir()
	{
		if (command == "dir")
		{
			t.currentFolder->print();
			return true;
		}
		return false;
	}

    bool isAttrib()
	{
		string opr = command.substr(0, command.find(' '));
		opr = lower(opr);
		if (opr == "attrib") 
		{
			string fileName = command.substr(7);

			File* foundFile = t.currentFolder->Find(fileName);

			if (foundFile != nullptr) 
			{
				foundFile->PrintAttribute();
			} 
			else 
			{
				cout << "File not found." << endl;
			}
			return true;
		}
		return false;
	}

	bool isCD()
	{
		string opr = command.substr(0, 2);
		opr = lower(opr);
		if (opr == "cd")
		{
			string argument = command.substr(3);  
			if (argument == ".")
			{
				t.print(promptString); 
			}
			else if (argument == "..")
			{
				if (t.currentFolder->Parent != nullptr)
				{
					t.setPath(t.currentFolder->Parent);
					t.print(promptString); 
				}
				else
				{
					cout << "Already at the root directory." << endl;
				}
			}
			else if (argument == "\\")
			{
				t.setPath(t.Root);
				t.print(promptString); 
			}
			else
			{
				Folder* folderToNavigate = t.currentFolder->FindFolder(argument);
				if (folderToNavigate != nullptr)
				{
					t.setPath(folderToNavigate);
					t.print(promptString); 
				}
				else
				{
					cout << "Directory not found." << endl;
				}
			}
			return true;
		}
		return false;
	}

	bool isCopy()
	{
		string opr = command.substr(0, command.find(' '));
		opr = lower(opr);
		if (opr == "copy")
		{
			string sourceFileName = command.substr(5, command.find(' ', 5) - 5);
        	string destinationPath = command.substr(command.find(' ', 5) + 1);

			File* sourceFile = t.currentFolder->Find(sourceFileName);

			if (sourceFile != nullptr)
			{
				File* CopyFile=new File(*sourceFile);
				CopyFile->path=destinationPath;

				Folder* destinationFolder = t.currentFolder->FindFolder(destinationPath);
				if (destinationFolder!=nullptr)
				{
					destinationFolder->insertFile(CopyFile);
					cout << "File copied successfully." << endl;
           		} 
				else 
				{
					cout << "Destination directory not found." << endl;
					delete CopyFile;  
          		}
			}
			else 
			{
				cout << "Source file not found." << endl;
			}

        	return true;
    	}

   	 return false;
			
	}

	bool isMove()
	{
		string opr = command.substr(0, command.find(' '));
		opr = lower(opr);
		if (opr == "move")
		{
			string sourceFileName = command.substr(5, command.find(' ', 5) - 5);
        	string destinationPath = command.substr(command.find(' ', 5) + 1);

			File* sourceFile = t.currentFolder->Find(sourceFileName);

			if (sourceFile != nullptr)
			{
				File* movedFile=new File(*sourceFile);
				movedFile->path=destinationPath;

				t.currentFolder->remove_File(sourceFileName);

				Folder* destinationFolder = t.currentFolder->FindFolder(destinationPath);

				if (destinationFolder!=nullptr)
				{
					destinationFolder->insertFile(movedFile);
					cout << "File Moved successfully." << endl;
           		} 
				else 
				{
					cout << "Destination directory not found." << endl;
					delete movedFile;  
          		}
			}
			else 
			{
				cout << "Source file not found." << endl;
			}
        	return true;
    	}

   	 return false;
			
	}

	bool isCreate()
{
    string opr = command.substr(0, command.find(' '));
    opr = lower(opr);
    if (opr == "create")
    {
        string fileName = command.substr(7);
        File* newFile = new File(fileName, t.currentFolder->path, "txt", "user");

        cout << "Enter file contents. Type 'exit' on a new line to finish." << endl;
        string line;
        while (true)
        {
            getline(cin, line);
            if (line == "exit")
                break;

            for (char ch : line)
            {
                newFile->content.push_back(ch);
            }

            // Add a newline character after each line
            newFile->content.push_back('\n');
        }

        t.currentFolder->insertFile(newFile);
        cout << "File created successfully." << endl;
        return true;
    }

    return false;
}
	bool isFind()
	{
		string opr=command.substr(5,command.find(' '));
		opr=lower(opr);
		if(opr=="find")
		{
			string fileName=command.substr(5);
			File* foundFile=t.currentFolder->Find(fileName);

			if(foundFile!=nullptr)
			{
				 cout << "File found: " << foundFile->path + "\\" + foundFile->name << endl;
   		    } 
			else
			{
           		cout << "File not found." << endl;
            }

        	return true;
   		}
   		 return false;
    }

	
	bool isFormat()
	{
		if (command == "format")
		{
            t.currentFolder->EmptyDirectory();

			cout << "Directory formatted successfully." << endl;
			return true;
		}
		return false;
	}

	bool isVersion()
	{
		if (command == "isVer")
		{
			cout << "DOS Shell Version 1.0" << endl;
       		return true;
		}
		return false;
	}


	bool isCONVERT()
	{
		string opr = command.substr(0, command.find(' '));
		opr = lower(opr);
		if (opr == "convert") 
		{
			string fromExtension = command.substr(8, 3);
			string toExtension = command.substr(12, 3);

			for (auto file : t.currentFolder->Files)
			{
				if (file->fileType == fromExtension)
				{
					file->fileType = toExtension;
				}
			}

			cout << "Conversion completed." << endl;
			return true;
		}
		return false;
	}

	bool isEXIT() 
	{
		string opr = lower(command);
		if (opr == "exit") 
		{
			cout << "Exiting the program." << endl;
			exit(0);
			return true;
		}
		return false;
	}

	bool isHELP() 
{
    string opr = lower(command);
    if (opr == "help") 
    {
        cout << "Available Commands:" << endl;
        cout << "ATTRIB\t\tDisplays file attributes." << endl;
        cout << "CD\t\tDisplays or changes the current directory." << endl;
        cout << "CONVERT\t\tConverts file extensions." << endl;
        cout << "COPY\t\tCopies a file to another location." << endl;
        cout << "CREATE\t\tCreates a new text file." << endl;
        cout << "DEL\t\tDeletes a file." << endl;
        cout << "DIR\t\tDisplays a list of files and subdirectories." << endl;
        cout << "EDIT\t\tEdits the content of a text file." << endl;
        cout << "FIND\t\tFinds a file in the current directory." << endl;
        cout << "FORMAT\t\tFormats the current directory." << endl;
        cout << "HELP\t\tDisplays available commands." << endl;
        cout << "MKDIR\t\tCreates a new directory." << endl;
        cout << "RMDIR\t\tRemoves a directory." << endl;
        cout << "MOVE\t\tMoves a file to another location." << endl;
        cout << "PRINTTREE\tDisplays the directory tree." << endl;
        cout << "RENAME\t\tRenames a file." << endl;
        cout << "VERSION\t\tDisplays the DOS Shell version." << endl;
        cout << "PROMPT\t\tChanges the command prompt." << endl;
        cout << "EXIT\t\tExits the program." << endl;
        cout << "PPRINT\t\tAdds a file to the priority queue." << endl;

        return true;
    }
    return false;
}
	bool isPPrint()
	{
		string opr = command.substr(0, command.find(' '));
		opr = lower(opr);
		if (opr == "pprint") 
		{
			string fileName=command.substr(7);

			File* fileToAdd=t.currentFolder->Find(fileName);
			if (!fileToAdd)
			{
				t.currentFolder->priorityQueue.push_back(fileToAdd);
				cout << "File added to priority queue successfully." << endl;

				 cout << "Priority Queue:" << endl;
				 for(auto file:t.currentFolder->priorityQueue)
				 {
					cout << file->name << endl;
				 }
			} 
			else 
			{
                cout << "File not found." << endl;
            }

            return true;
        }
        return false;
	}	
	
	bool isPROMPT()
	{
		string opr = command.substr(0, command.find(' '));
		opr = lower(opr);
		if (opr == "prompt")
		{
			promptString = command.substr(7);
			cout << "Command prompt changed to " << promptString << endl;
			return true;
		}
		return false;
	}

	bool isPrintTree()
	{
		if (command == "tree")
		{
			t.printTree(t.Root);
			return true;
		}
		return false;
	}

	void saveState(const string& filename) {
        ofstream outFile(filename, ios::out);

        if (!outFile.is_open()) {
            cout << "Error opening file for writing." << endl;
            return;
        }

        saveStateRecursive(t.Root, outFile);

        outFile.close();
        cout << "State saved successfully." << endl;
    }

    void loadState(const string& filename) {
        ifstream inFile(filename, ios::in);

        if (!inFile.is_open()) {
            cout << "Error opening file for reading." << endl;
            return;
        }

        t.Root = new Folder();
        loadStateRecursive(t.Root, inFile);

        inFile.close();
        cout << "State loaded successfully." << endl;
    }

	void input()
	{
        getline(cin,command);
		if (isAttrib())
		{
			t.print(promptString);
		}
		else if (isCD())
		{

		}
		else if (isEdit())
		{
			t.print(promptString);
		}
		else if (isCONVERT())
		{
			t.print(promptString);
		}
		 else if (isCopy())
		{
			t.print(promptString);
		}
		else if (isCreate())
		{
			t.print(promptString);
		}
		else if (isDel())
		{
			t.print(promptString);
		}
        else if (isDir())
		{
			t.print(promptString);
		}
		else if (isFind())
		{
			t.print(promptString);
		}
	
		else if (isFormat())
		{
			t.print(promptString);
		}
		else if (isHELP())
		{
			t.print(promptString);
		}
		else if (isMakeDir())
		{
			t.print(promptString);
		}
		else if (isRemoveDir())
		{
			t.print(promptString);
		}
		else if (isMove())
		{
			t.print(promptString);
		}
		else if (isPrintTree())
		{
			t.print(promptString);
		}
		else if (isRename())
		{
			t.print(promptString);
		}
		
		else if (isVersion())
		{
			t.print(promptString);
		}
		else if (isPROMPT())
		{
			t.print(promptString);
		}
	}

private:

    void saveStateRecursive(Folder* folder, ofstream& outFile) 
	{
        outFile << "F " << folder->name << " " << folder->path << endl;

        for (auto subfolder : folder->subFolders) {
            saveStateRecursive(subfolder, outFile);
        }

        for (auto file : folder->Files) 
		{
            outFile << "C " << file->name << " " << file->path << " " << file->fileType << " " << file->owner << endl;
            // Save file content
            for (auto content : file->content) 
			{
                outFile << content;
            }
            outFile << endl;
        }
    }

    void loadStateRecursive(Folder* folder, ifstream& inFile) 
	{
    string type, name, path, fileType, owner;
    inFile >> type >> name >> path;

    if (type == "F") {
        folder->name = name;
        folder->path = path;

        while (true) {
            inFile >> type;
            if (type == "F") 
			{
                Folder* subfolder = new Folder();
                folder->subFolders.push_back(subfolder);
                loadStateRecursive(subfolder, inFile);
            } 
			else if (type == "C") 
			{
                inFile >> name >> path >> fileType >> owner;
                File* file = new File(name, path, fileType, owner);

                // Read file content until newline character
                char content;
                while (inFile.get(content)) 
				{
                    if (content == '\n') break;
                    file->content.push_back(content);
                }

                folder->Files.push_back(file);
            } 
			else 
			{
                break;
            }
        }
    }
}
	
};

int main()
{
	DOS dosShell;
    dosShell.print();

    while (true) {
		system("color 07");
        dosShell.input();

        if (dosShell.isEXIT()) {
            dosShell.saveState("saved_state.txt");
            break;
        }
    }

    return 0;
}