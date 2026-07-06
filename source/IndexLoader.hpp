#include <vector>
#include <string>

#include "ConsoleGraphics.hpp"
#include "BLC/BetterLessColours.hpp"
#include "FileLoader.hpp"
#include "IndexPSP/SHTXPS.hpp"
#include "IndexPSP/CRAF.hpp"
#include "StringParser.hpp"

namespace IndexLoader
{
	//Can't call it ReplaceFile, yay
	//When updating an image, the original size will be used
	std::vector<uint8_t> UpdateFile(std::string fileName, uint32_t imgW, uint32_t imgH)
	{
		std::vector<uint8_t> fileData;

		std::string fileNameUpper = StringParser::ToUpper(fileName);
		if (fileNameUpper.find(".PNG") != std::string::npos || fileNameUpper.find(".BMP") != std::string::npos)
		{
			cg::Image image;
			bool retVal = LoadImg(image, fileName); //".NewFiles\\"+

			if (retVal)
			{
				if (imgW != 0 && imgH != 0)
				{
					float f = (float)imgW / (float)image.getWidth();
					cg::InterpolationMethod im = (f < 1.f ? cg::InterpolationMethod::AreaAveraging : cg::InterpolationMethod::NearestNeighbor);
					image.resize(imgW, imgH, im);
				}

				auto pixels = CGImgTo32Int(image);
				//bool d = (std::max(image.getWidth(), image.getHeight()) <= 256) ? settings.dither1 : settings.dither2;
				//ReduceColours3(image, image, settings.colourTableSize, 0, settings.percentileStep, d);
				auto ct = blc::GetColourTable(pixels);
				pixels = blc::ReduceColours(pixels, ct);
				
				uint32_t w = image.getWidth();
				uint32_t h = image.getHeight();

				fileData = IndexSHTXPS::CreateSHTX(pixels, w, h);
			}
		}
		else fileData = LoadFile(fileName); //".NewFiles\\"+

		return fileData;
	}

	////Stuff like getting model's name, image's size...
	//std::string GetFileInfo(uint32 fileType, std::vector<uint8>& fileData, cg::Image& image)
	//{
	//	std::string fileInfo;
	//
	//	switch (fileType)
	//	{
	//	case 0x58544853: //SHTX
	//		fileInfo = '[' + std::to_string(image.getWidth()) + ", " + std::to_string(image.getHeight()) + ']';
	//		break;
	//
	//	case 0x44415353: //SSAD
	//		for (uint32 i = 0x34; fileData[i] != '\0'; i++) {
	//			fileInfo += fileData[i];
	//		}
	//		break;
	//
	//	case 0x36435350: //PSC6
	//	{
	//		uint32* fileNameOffset = (uint32*)&fileData[0x44];
	//		for (uint32 i = *fileNameOffset; fileData[i] != '\0'; i++) {
	//			fileInfo += fileData[i];
	//		}
	//	}
	//	break;
	//
	//	default:
	//		break;
	//	}
	//
	//	return fileInfo;
	//}

	void LoadList(std::vector<uint8_t> crafData, std::deque<std::vector<uint8_t>>& crafFiles, std::string fileName)
	{
		std::ifstream readFile(fileName.c_str());
		if (readFile.is_open())
		{
			std::string line, dir = ".";
			while (std::getline(readFile, line))
			{
				switch (line[0])
				{
					case '!': //Change directory
						dir = std::string(line.c_str() + 1);
						continue;

					case '=': //Size
						crafFiles.resize(strtoul(line.c_str() + 1, nullptr, 0));
						continue;

					case '#': //Comment
						continue;

					default:
						break;
				}

				auto rd = StringParser::Split(line, ':');
				if (rd.size() < 2)
				{
					//CRAFEditor::PrintOutput("[LOAD_LIST]: Invalid instuction \"" + line + "\". Skipping...");
					continue;
				}

				uint32_t fileIndex = strtoul(rd[0].c_str(), nullptr, 0) % crafFiles.size();
				std::string fileName = dir + '\\' + rd[1]; //dir+'\\'+

				bool b = (fileIndex < IndexCRAF::GetNumberOfFiles(crafData));
				std::vector<uint8_t> oldData = b ? IndexCRAF::GetFileData(crafData, fileIndex) : std::vector<uint8_t>();
				int oW = 0;
				int oH = 0;
				if (IndexSHTXPS::IsValid(oldData))
				{
					auto oldII = IndexSHTXPS::GetImageInfo(oldData);
					oW = oldII.width;
					oH = oldII.height;
				}
				crafFiles[fileIndex] = UpdateFile(fileName, oW, oH); //".NewFiles\\"+
			}
		}

		return;
	}
}