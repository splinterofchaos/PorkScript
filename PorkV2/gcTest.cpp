#include <iostream>
#include <string>
#include "PGameContainer.h"
using namespace GameContainer;
using std::cout;
using std::cin;
using std::endl;
using std::string;

int main()
{
	int option;
	PGameContainer theCont;
	string filename;

	cout << "Welcome to the PGameContainer test program! What would you like to do?" << endl;
	cout << "1:\tOpen a GameContainer\n2:\tCreate a GameContainer" << endl;
	cin >> option;

	// Get the user's option of whether they want to open to create a GC
	switch(option)
	{
	case 1: cout << "\nOpen a GameContainer";
		cout << "\nEnter the filename: ";
		cin >> filename;
		theCont = GameContainer::ReadFromFile(filename);
		break;
		
	case 2: cout << "\nCreate a GameContainer";
		cout << "\nEnter name of new GC: ";
		cin >> filename;
		theCont = PGameContainer(filename, "testAuth", 1.0);
		break;
		
	default: cout << "Invalid option! Exiting...";
		break;
	}

	bool exit = false;
	// Basic, common string variables the user will need access to
	do
	{
		cout << "Main Menu\n1:\tAdd a Map\n2:\tAdd an Item\n3:\tAdd a RequiresFile\n4:\tGet a Map\n5:\tGet an Item\n6:\tGet a RequiresFile\n7:\tExit program" << endl;
		cin >> option;

		switch(option)
		{
		case 1: cout << "\nAdd a Map";
			AddMap(&theCont);
			break;
		case 2: cout << "\nAdd an Item";
			AddItem(&theCont);
			break;
		case 3: cout << "\nAdd a RequiresFile";
			AddReqFile(&theCont);
			break;
		case 4: cout << "\nGet a Map";

		}
		
	} while(exit == false);

	fflush(stdin);
	cout << "\n\n\nPress [Enter] to terminate...";
	cin.get();

	return 0;
}

void AddMap(PGameContainer *gc)
{
	string name;
	string ID;
	cout << "\nEnter the name: ";
	cin >> name;
	cout << "\nEnter an ID: ";
	cin >> ID;

	try
	{
		gc->CreatePMap(ID, name, "This is a desc.");
	}
	catch(xReturnError& x)
	{
		cout << "\n\n" << x.GetErrorDesc() << endl;
	}
	catch(...)
	{
		cout << "\n\nUndefined error has occurred!" << endl;
	}
	return;
}

void AddItem(PGameContainer *gc)
{
	string ID;
	string name;
	cout << "\nEnter the name: ";
	cin >> name;
	cout << "\nEnter the ID: ";
	cin >> ID;

	try
	{
		gc->CreatePItem(ID, name, "This is a desc.");
	}
	catch(xReturnError& x)
	{
		cout << "\n\n" << x.GetErrorDesc() << endl;
	}
	catch(...)
	{
		cout << "\n\nUndefined error has occurred!" << endl;
	}
	return;
}

void AddReqFile(PGameContainer *gc)
{
	string name;
	cout << "\nEnter the name: ";
	cin >> name;

	try
	{
		gc->CreatePRequiresFile(name);
	}
	catch(xReturnError& x)
	{
		cout << "\n\n" << x.GetErrorDesc() << endl;
	}
	catch(...)
	{
		cout << "\n\nUndefined error has occurred!" << endl;
	}
	return;
}

void GetAndUseMap(PGameContainer *gc)
{
	string ID;
	int option;
	cout << "\nEnter the ID of the Map: ";
	cin >> ID;

	PMap* tempMap = gc->GetPMap(ID);

	cout << "\n\nPMap Menu:\n1:\t" << endl;
	cin >> option;

	switch(option)
	{
	case 1: tempMap->