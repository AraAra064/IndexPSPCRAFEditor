#include <iostream>
#include <string>
#include <vector>

#include "IndexPSP/CRAF.hpp"
#include "IndexLoader.hpp"

namespace CLI
{
	std::string ReturnArgStr(const char* fileName)
	{
		//ipce.exe CRAF_IN FILE_LIST CRAF_OUT
		std::string argStr = std::string(fileName)+" CRAF_IN FILE_LIST CRAF_OUT";
		argStr += "\nCRAF_IN = CRAF file to edit";
		argStr += "\nFILE_LIST = List to use to edit CRAF file";
		argStr += "\nCRAF_OUT = File to save CRAF as";

		return argStr;
	}

	int CommandLine(int nArgs, const char** arg)
	{
		if (nArgs < 4) //not enough args
		{
			std::cout << "Not enough arguments." << std::endl;
			std::cout << ReturnArgStr(arg[0]) << std::endl;
			return 1;
		}

		std::vector<uint8_t> fileData = LoadFile(arg[1]);
		if (fileData.empty()) //file doesn't exist or cannot be read from
		{
			std::cout << "CRAF input file doesn't exist or cannot be read from." << std::endl;
			std::cout << ReturnArgStr(arg[0]) << std::endl;
			return 2;
		}

		auto crafFiles = IndexCRAF::GetAllFiles(fileData);

		IndexLoader::LoadList(fileData, crafFiles, std::string(arg[2]));
		bool e = IndexCRAF::CreateCRAF(arg[3], crafFiles);

		if (!e)
		{
			std::cout << "Error creating CRAF file." << std::endl;
			std::cout << "[REASON]" << std::endl;
			return 3;
		}

		std::cout << "Successfully created " << arg[3] << std::endl;
		return 0;
	}
}
