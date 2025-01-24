#pragma once
#include "sha1.h"
#include <string>
#include<direct.h>
#include <iostream>
using namespace std;
#include"Source.h"
#include <fstream>
#include <cstdio>

class IPFS
{
	int number_of_machines, size_of_identifier_space;
	int mbtree;
public:
	IPFS()
	{
		mbtree = 0;
		number_of_machines = 0;
		size_of_identifier_space = 0;
	}
	int hash_func(string f)
	{
		SHA1 checksum;
		checksum.update(f);
		const string hash = checksum.final();

		int ret_val = 0;
		for (int i = 0;i < size_of_identifier_space;i++)
		{
			ret_val += hash[i];
		}
		cout << "=====================value : " << ret_val << endl;
		return ret_val;

	}
	void Menu()
	{
		cout << "///////////////////////////////////////////////////////////////////////////////////////////////\n"
			<< "\n\n       WELCOME TO IPFS\n"
			<< "                                  CREATED BY: \n"
			<< "                                      (I22-0942) MUHAMMAD UMAR HASSAN \n"
			<< "                                      (I22-1234) MUHAMMAD RAZA \n"
			<< "                                      (I22-1220) MUHAMMAD HUZAIFA \n"
			<< "///////////////////////////////////////////////////////////////////////////////////////////////\n\n";

		///////////////////////////////////////////initialization///////////////////////////////////////////////
		cout << "\n\nHow many number of machines you want in your system : ";
		cin >> number_of_machines;
		///////////////////////////////////order of Btree////////////////////////////////
		cout << "What would be the order of Btree you want in your system : ";
		cin >> mbtree;
		mbtree = ceil(static_cast<double>(mbtree) / 2.0);
		/////////////////////////identifier space/////////////////////////////////////////
		cout << "Would you like to specify the size of identifier space in bits, i.e., 160 bits, 4 bits etc : ";
		cin >> size_of_identifier_space;
		//////////////////////////////////////file handle////////////////////////////
		string de_path = "C:/IPFS_of_bit_";
		de_path += to_string(size_of_identifier_space);

		int res=_mkdir(de_path.c_str());

		de_path += '/';
		int choice_machine = 0;
		//////////////////////////option for assign ids//////////////////////////////////////////////////
		cout << "Select option \n 1. You will assign IDS manually to machine \n  2. You will allow us to assign IDS automatically to machines\n Your choice :  ";
		cin >> choice_machine;

		//////////////////////////////////////Ring DHT////////////////////////////
		int space = size_of_identifier_space;
		ring_DHT r(space);
		//////////////////////////////Asiigning IDS to machines///////////////////

		if (choice_machine == 1)
		{
			int i = 0;
			while (i < number_of_machines)
			{
				int Machine_id = 0;
				cout << "Enter ID for machine to insert : " << endl;
				cin >> Machine_id;

				// Insert machine 
				r.add_machine(Machine_id, mbtree);
				//make folders
				string path = "";
				path = de_path;
				path += to_string(Machine_id);
				int res= _mkdir(path.c_str());

				i++;
			}
		}
		else
		{
			int i = 0;
			while (i < number_of_machines)
			{
				int Machine_id = 0;
				Machine_id = i;
				// Insert machine 
				r.add_machine(Machine_id, mbtree);
				//make folders
				string path = "";
				path = de_path;
				path += to_string(Machine_id);
				int res= _mkdir(path.c_str());
				i++;
			}
		}

		///////////////////////////////////////////MAIN options for user to operate on system///////////////////////////////////////
		while (true)
		{
			int user_choice = 0;
			cout << "\n\nSelect option \n"
				<< "1. You want to insert file to machine \n"
				<< "2. You want to remove file from machine \n"
				<< "3. You want to add a machine \n"
				<< "4. You want to remove a machine \n"
				<< "5. You want to display the routing table of a machine \n"
				<< "6. You want to display the Btree of a machine \n"
				<< "0. Exit \n\n\n"
				<< "Your choice : ";
			cin >> user_choice;
			int con = 0;
			int Machine_id = 0;
			string filename = "";
			int hashI = 0;

			file f;
			string path = "", source = "", dest = "";
			int mac_id = 0;
			int res = 0;
			switch (user_choice)
			{
			case 1:
			{
				// Insert (key,val) in target_machine 
				cout << "Enter the name of a file to upload on a machine : ";
				cin >> filename;
				hashI = hash_func(filename);
				f.name = filename;
				f.hash = hashI;
				mac_id=r.insert_file(f, f.hash);
				///////////file handle///////////
				source += f.name;
				source += ".txt";

				cout << "The file: " << f.name << " is uploaded to machine of ID: " << mac_id << endl;
				dest = de_path;
				dest += to_string(mac_id);
				dest += "/";
				dest += filename;
				dest += ".txt";
				cout << dest << " |  " << source << endl;

				ifstream sourceFile(source, std::ios::binary);
				if (!sourceFile) {
					std::cerr << "Error opening source file: " << source << std::endl;
					return;
				}

				// Open the destination file for binary output
				std::ofstream destFile(dest, std::ios::binary);

				if (!destFile) {
					std::cerr << "Error opening destination file: " << dest << std::endl;
					return;
				}
				destFile << sourceFile.rdbuf();    // cpy content
				sourceFile.close();
				destFile.close();

				std::cout << "File copied successfully from " << source << " to " << dest << std::endl;
				///////////////////////////////////////file handle///////////////////////////////////////////////

			}
			break;
			case 2:
				// Delete (key) from target_machine 
				cout << "Enter the name of a file to remove from a machine : ";
				cin >> filename;
				hashI = hash_func(filename);
				mac_id=r.delete_file(hashI);
				/////////////////////////file handle//////////////////
				
				dest = de_path;
				dest += to_string(mac_id);
				dest += "/";
				dest += filename;
				dest += ".txt";
				cout << dest << " |  "<< endl;

				if (std::remove(dest.c_str()) == 0) {
					std::cout << "File removed successfully: " << dest << std::endl;
				}
				else {
					std::cerr << "Error removing file: " << dest << std::endl;
				}
				///////////////////////////////////////////////////////////file handle///////////////////////////
				break;
			case 3:
				// Insert machine 
				cout << "Enter ID for machine to be added  : " << endl;
				cin >> Machine_id;
				// Insert machine 
				r.add_machine(Machine_id, mbtree);
				//////////////////////////////////////////file handle///////////////////
				//make folders
				path = de_path;
				path += to_string(Machine_id);
				res = _mkdir(path.c_str());
				//////////////////////////////////file handle//////////////////////////
				break;
			case 4:
				//delete machine
				cout << "Enter ID for machine to remove  : " << endl;
				cin >> Machine_id;
				 r.remove_machine(Machine_id, de_path);
				/////file handling///
				dest = "";
				dest = de_path;
				dest += to_string(Machine_id);
				 res = _rmdir(dest.c_str());

				if (res == 0) {
					std::cout << "Directory deleted successfully: " << path << std::endl;
				}
				else {
					std::cerr << "Error deleting directory: " << path << std::endl;
				}
				/////file handling///

				break;
			case 5:
				//display routing table
				cout << "Enter ID for machine to display its routing table : " << endl;
				cin >> Machine_id;
				cout << "\n---------Routing table for ID: " << Machine_id << " ------------------\n";
				r.Print_rtable(Machine_id);
				cout << endl;
				break;
			case 6:
				//display Btree
				cout << "Enter ID for machine to display its Btree : " << endl;
				cin >> Machine_id;
				r.btree_display(Machine_id);
				cout << endl;
				break;
			case 0:
				con = 1;
				break;
			}
			if (con)
				break;
		}
	}
};

