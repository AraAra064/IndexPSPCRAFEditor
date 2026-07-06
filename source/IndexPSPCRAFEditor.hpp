std::vector<uint8> LoadFile(std::string fileName)
{
	std::vector<uint8> retVal;
	std::ifstream readFile(fileName.c_str(), std::ios::binary);
	if (readFile.is_open())
	{
		while (!readFile.eof())
		{
			uint8 c;
			readFile.read((char*)&c, sizeof(uint8));
			retVal.push_back(c);
		}

		readFile.close();
	}

	return retVal;
}

struct Settings
{
	bool promptException;
	bool exceptionDefault;

	float percentileStep;
	uint16 colourTableSize;
	uint8 colourReductionType;
	bool dither1;
	bool dither2;

	uint32 windowWidth;
	uint32 windowHeight;
	uint8 pixelSize;

	Settings(bool promptException = true, bool exceptionDefault = false, float percentileStep = 0.78125f, uint16 colourTableSize = 256, uint8 colourReductionType = 0, bool dither1 = true, bool dither2 = false)
	{
		this->promptException = promptException;
		this->exceptionDefault = exceptionDefault;

		this->percentileStep = percentileStep;
		this->colourTableSize = colourTableSize;
		this->colourReductionType = colourReductionType;
		this->dither1 = dither1;
		this->dither2 = dither2;
	}
};

namespace CRAFEditor
{
	void LoadSettings(std::string fileName, Settings &settings)
	{
		auto fd = LoadFile(fileName);
		
		if (!fd.empty())
		{
			std::string fileData = std::string((char*)&fd[0], fd.size());
			
			auto s = StringParser::GetAllDataByTags(fileData);
			if (!s["CRAF_EDITOR"].empty())
			{
				std::string _s = s["CRAF_EDITOR"];
				
				//Window Stuff
				std::string windowData;
				
				windowData = StringParser::GetDataByTag(_s, "WINDOW_SIZE");
				if (!windowData.empty())
				{
					auto wd = StringParser::Split(windowData, ',');
					for (auto &i : wd){i = StringParser::Strip(i);
					}
					
					if (wd.size() >= 3)
					{
						settings.windowWidth = strtoul(wd[0].c_str(), nullptr, 0);
						settings.windowHeight = strtoul(wd[1].c_str(), nullptr, 0);
						settings.pixelSize = strtoul(wd[2].c_str(), nullptr, 0);
					}
				}
				
				//Colour Reduction Stuff
				std::string reductionData;
				
				reductionData = StringParser::GetDataByTag(_s, "COLOUR_TABLE_SIZE");
				if (!reductionData.empty()){settings.colourTableSize = strtoul(reductionData.c_str(), nullptr, 0);
				}
				reductionData = StringParser::GetDataByTag(_s, "PRECENTILE_STEP");
				if (!reductionData.empty()){settings.percentileStep = strtof(reductionData.c_str(), nullptr);
				}
				reductionData = StringParser::GetDataByTag(_s, "DITHER_LESS_EQUAL_256X256");
				if (!reductionData.empty()){settings.dither1 = (StringParser::ToLower(reductionData) == "true");
				}
				reductionData = StringParser::GetDataByTag(_s, "DITHER_GREATER_256X256");
				if (!reductionData.empty()){settings.dither2 = (StringParser::ToLower(reductionData) == "true");
				}
				reductionData = StringParser::GetDataByTag(_s, "COLOUR_REDUCTION_TYPE");
				if (!reductionData.empty()){settings.colourReductionType = strtoul(reductionData.c_str(), nullptr, 0);
				}
				
				//Exception stuff
				std::string exceptionData;
				
				exceptionData = StringParser::GetDataByTag(_s, "PROMPT_EXCEPTIONS");
				if (!exceptionData.empty()){settings.promptException = (StringParser::ToLower(exceptionData) == "true");
				}
				exceptionData = StringParser::GetDataByTag(_s, "EXCEPTIONS_DEFAULT");
				if (!exceptionData.empty()){settings.exceptionDefault = (StringParser::ToLower(exceptionData) == "true");
				}
			}
		}
		
		return;
	}
	
	//List formatting stuff
	//Comment = #
	std::string ReplaceCommand(uint32 fileIndex, std::string fileName){return std::to_string(fileIndex)+':'+fileName+'\n';
	}
	std::string RemoveCommand(uint32 fileIndex){return "!REM "+std::to_string(fileIndex)+'\n';
	}
	std::string AddCommand(std::string fileName){return "!ADD "+fileName+'\n';
	}
	std::string DirectoryCommand(std::string dir){return "!DIR "+dir+'\n';
	}
	
	
	void PrintOutput(std::string msg)
	{
		std::cout<<msg<<std::endl;
		Sleep(500);
		return;
	}
	void PrintError(std::string msg)
	{
		std::cerr<<msg<<std::endl;
		Sleep(1000);
		return;
	}
}

