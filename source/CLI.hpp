#include <iostream>
#include <string>
#include <vector>

#include "IndexPSP/CRAF.hpp"
#include "IndexLoader.hpp"

namespace CLI
{
	std::string ReturnArgStr(void)
	{
		//ipce.exe CRAF_IN FILE_LIST CRAF_OUT
		std::string argStr = "Batch Replace:";
		argStr += "\nipce.exe BR CRAF_IN FILE_LIST CRAF_OUT";
		argStr += "\nCRAF_IN = CRAF file to edit";
		argStr += "\n[*]FILE_LIST = List to use to edit CRAF file";
		argStr += "\nCRAF_OUT = File to save CRAF as";

		argStr += "\n\nExtract All:";
		argStr += "\nipce.exe EA CRAF_IN FOLDER_NAME";
		argStr += "\nCRAF_IN = CRAF file to extract";
		argStr += "\nFOLDER_NAME = List to use to edit CRAF file";

		argStr += "\n\n[*]More information about file lists can be found in README.txt";

		return argStr;
	}

	int CommandLine(int nArgs, const char** arg)
	{
		std::vector<std::string> argStr(nArgs);
		for (int i = 0; i < nArgs; i++)
		{
			argStr[i] = arg[i];
		}

		if (nArgs <= 3)
		{
			std::cout << "Not enough arguments." << std::endl;
			std::cout << ReturnArgStr() << std::endl;
			return 1;
		}

		//Batch Replace
		if (nArgs > 4 && argStr[1] == "BR")
		{
			std::vector<uint8_t> fileData = LoadFile(argStr[2]);
			if (fileData.empty()) //file doesn't exist or cannot be read from
			{
				std::cout << "CRAF input file doesn't exist or cannot be read from." << std::endl;
				std::cout << ReturnArgStr() << std::endl;
				return 2;
			}

			auto crafFiles = IndexCRAF::GetAllFiles(fileData);

			IndexLoader::LoadList(fileData, crafFiles, argStr[3]);
			bool e = IndexCRAF::CreateCRAF(argStr[4], crafFiles);

			if (!e)
			{
				std::cout << "Error creating CRAF file." << std::endl;
				return 3;
			}

			std::cout << "Successfully created " << argStr[4] << std::endl;
			return 0;
		}

		//Extract All
		if (argStr[1] == "EA")
		{
			std::vector<uint8_t> fileData = LoadFile(argStr[2]);
			if (fileData.empty())
			{
				std::cout << "CRAF file doesn't exist or cannot be read from." << std::endl;
				std::cout << ReturnArgStr() << std::endl;
				return 2;
			}

			auto crafFiles = IndexCRAF::GetAllFiles(fileData);

			std::string folderName = argStr[3], fileName;

			int e = _mkdir(folderName.c_str());
			if (e != 0)
			{
				std::cout << "Error creating folder \"" << folderName << "\"." << std::endl;
				return 3;
			}

			for (size_t i = 0; i < crafFiles.size(); i++)
			{
				auto& file = crafFiles[i];
				fileName = "File" + std::to_string(i) + "." + IndexCRAF::GetFileTypeStr(fileData, i);
				bool e = MakeFile(folderName + '\\' + fileName, file);

				if (!e)
				{
					std::cout << "Error while writing \"" << fileName << "\"." << std::endl;
					return 3;
				}
			}

			std::cout << "Successfully created " << arg[3] << std::endl;
			return 0;
		}

		std::cout << "Function not found." << std::endl;
		std::cout << ReturnArgStr() << std::endl;
		return 1;
	}
}
