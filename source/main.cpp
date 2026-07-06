#include <iostream>
#include <direct.h>

#include "ConsoleGraphics.hpp"
#include "CLI.hpp"

void drawImage(cg::ConsoleGraphics& graphics, cg::Image& image, float imgScale = 1.f, cg::DrawType drawMode = cg::DrawType::Repeat, void (*drawFunc)(std::pair<uint32, uint8>*, void*) = nullptr, void* data = nullptr)
{
	int32_t srcX = image.getPosX(), srcY = image.getPosY();
	uint32_t dstX = srcX < 0 ? 0 : srcX, dstY = srcY < 0 ? 0 : srcY;
	uint32_t w = srcX < 0 ? image.getWidth() - abs(srcX) : image.getWidth(), h = srcY < 0 ? image.getHeight() - abs(srcY) : image.getHeight();
	graphics.drawEX(image, srcX < 0 ? abs(srcX) : 0, srcY < 0 ? abs(srcY) : 0, dstX, dstY, w * imgScale, h * imgScale, drawMode, drawFunc, data);
	return;
}

int main(int nArgs, const char** arg)
{
	//cg::Image font("DefaultFontx2.bmp");
	//font.setColourToAlpha(cg::BGR(255, 0, 0));
	//font.scale(0.5f, cg::InterpolationMethod::AreaAveraging);
	//cg::Text text;
	//text.setFont(&font, 8, 13);

	//Init window
	//cg::ConsoleGraphics graphics(480, 272, true, 3, true);
	//graphics.enableAlpha();
	//graphics.setTitle("IndexPSPCRAFEditor");

	if (nArgs > 1)
	{
		return CLI::CommandLine(nArgs, arg);
	}

	std::cout << "No ";

	return 0;
}

/*std::cout<<"Enter the file name of a CRAF file to edit:"<<std::flush;
	std::string fileName;
	std::getline(std::cin, fileName);
	
	std::vector<uint8> crafData = LoadFile(fileName);
	if (crafData.empty())
	{
		std::cerr<<"Error loading CRAF file..."<<std::endl;
		pause();
		return -1;
	}
	
	graphics.setTitle(graphics.getTitle()+": "+fileName);
	
	//A = Add file to CRAF [YES]
	//R = Replace file [YES]
	//X = Remove file from CRAF [YES]
	//E = Extract all [YES]
	//W = Extract current file (if image, save as all formats. PNG, SHTXPS...) [YES]
	//C = Create new CRAF [YES]
	//T = Reset file [NO]
	//L = Load list
	//K = Create list
	//J = Jump to index [MAYBE][NO]
	//ARROWS KEYS = Select a different file
	
	std::deque<std::vector<uint8>> crafFiles; //Holds decoded file data
	for (uint32 i = 0, n = IndexCRAF::GetNumberOfFiles(crafData); i < n; i++)
	{
		auto fileData = IndexCRAF::GetFileData(crafData, i);
		crafFiles.push_back(fileData);
	}
//	std::vector<bool> isChanged(crafFiles.size(), false);
	std::deque<std::string> listCommands;
	
	cg::Image preview;
	bool load = true;
	uint32 fileIndex = 0;
	
	Keyboard keyboard;
	HighResClock frameTime;
	std::string fileInfo;
	while (!keyboard.isKeyDown(VK_ESCAPE))
	{
		float deltaTime = frameTime.getElapsedTimeAsSeconds();
		frameTime.restart();
		
		if (load)
		{
//			graphics.setTitle(IndexCRAF::GetFileTypeStr(crafData, fileIndex));
//			graphics.setTitle(std::to_string(fileIndex));
			uint32 ft = *(uint32*)&crafFiles[fileIndex][0];
			std::string fileTypeStr = StringParser::ToLower(IndexCRAF::GetFileTypeStr(crafData, fileIndex));
			
			std::string fn;
			uint32 sz = 4;
			switch (ft)
			{
				case 0x58544853: //SHTX, Image
					preview = IndexSHTXPS::LoadImage(crafFiles[fileIndex]);
					break;
				
				case 0x00435450: //PTC
					sz = 3;
				case 0x36435350: //PSC6
				case 0x304D5350: //PSM0
				case 0x44415353: //SSAD
					fn = std::string((char*)&ft, sz);
					preview = cg::Image("Images\\" + fn + ".bmp");
					break;

				default: //UNKNOWN BINARY
					preview = cg::Image("Images\\unknown.bmp");
					break;
			}
			
			fileInfo = GetFileInfo(ft, crafFiles[fileIndex], preview);
			
			load = false;
		}
		
		//Keyboard events
		if (keyboard.isKeyDown(VK_RIGHT))
		{
			fileIndex++;
			fileIndex %= crafFiles.size();
			load = true;
		} else if (keyboard.isKeyDown(VK_LEFT))
		{
			fileIndex--;
			fileIndex = std::min<uint32>(fileIndex, crafFiles.size()-1); //If underflow, fileIndex > crafFiles.size()-1
			load = true;
		}
		
		if (keyboard.isKeyDown('R'))
		{
			clearInput();
			std::cout<<"In current folder:\".NewFiles\""<<std::endl;
			std::cout<<"Enter the name of the file to replace:"<<std::flush;
			std::string fileName;
			std::getline(std::cin, fileName);
			fileName = ".NewFiles\\"+fileName;
			std::vector<uint8> fileData = LoadFile(fileName);
			
			if (!fileData.empty())
			{
				bool b = (fileIndex < IndexCRAF::GetNumberOfFiles(crafData));
				std::vector<uint8> oldFileData = b ? IndexCRAF::GetFileData(crafData, fileIndex) : std::vector<uint8>();
				cg::Image oldImage = IndexSHTXPS::LoadImage(oldFileData);
				crafFiles[fileIndex] = _ReplaceFile(fileName, oldImage.getWidth(), oldImage.getHeight(), b ? &oldFileData[0] : nullptr, settings);
				listCommands.push_back(CRAFEditor::ReplaceCommand(fileIndex, fileName));
//				isChanged[fileIndex] = true;
				load = true;
			} else CRAFEditor::PrintError("ERROR [REPLACE_FILE]: Can\'t read \""+fileName+"\".");
		}
//		if (keyboard.isKeyDown('T')) //Reset file
//		{
//			crafFiles[fileIndex] = IndexCRAF::GetFileData(crafData, fileIndex);
//			isChanged[fileIndex] = false;
//			load = true;
//		}
		
		if (keyboard.isKeyDown('C'))
		{
			clearInput();
//			std::cout<<"In current folder:\".NewFiles\""<<std::endl;
			std::cout<<"Enter the name of file to save as:"<<std::flush;
			std::string fileName;
			std::getline(std::cin, fileName);
			
			bool retVal = IndexCRAF::CreateFile(fileName, crafFiles);
			
			if (!retVal){CRAFEditor::PrintError("ERROR [CREATE_CRAF]: Can\'t create \""+fileName+"\".");
			}
		}
		
		if (keyboard.isKeyDown('X') && crafFiles.size() > 1) //Remove current file
		{
			crafFiles.erase(crafFiles.begin() + fileIndex);
//			isChanged.erase(isChanged.begin() + fileIndex);
			fileIndex = std::min<uint32>(crafFiles.size() - 1, fileIndex);
			listCommands.push_back(CRAFEditor::RemoveCommand(fileIndex));
			
			load = true;
		}
		
		if (keyboard.isKeyDown('A')) //Add file to CRAF
		{
			clearInput();
			std::cout<<"In current folder:\".NewFiles\""<<std::endl;
			std::cout<<"Enter the name of the file to add:"<<std::flush;
			std::string fileName;
			std::getline(std::cin, fileName);
			fileName = ".NewFiles\\"+fileName;
			std::vector<uint8> fileData = LoadFile(fileName);
			
			if (!fileData.empty())
			{
				fileData = _ReplaceFile(fileName, 256, 256, nullptr, settings);
//				crafFiles.insert(crafFiles.begin() + fileIndex, fd);
//				isChanged.insert(isChanged.begin() + fileIndex, true);
				crafFiles.push_back(fileData);
//				isChanged.push_back(true);
				listCommands.push_back(CRAFEditor::AddCommand(fileName));
				load = true;
			} //else error
		}
		
		char singleExtract = 'W';
		if (keyboard.isKeyDown('E') || keyboard.isKeyDown(singleExtract, true))
		{
			bool once = keyboard.isKeyDown(singleExtract);
			
			std::string dir = ".SavedFiles\\"+StringParser::Split(fileName, '.')[0];
			_mkdir(dir.c_str());
			
			for (uint32 i = 0, n = IndexCRAF::GetNumberOfFiles(crafData); i < n; i++)
			{
				if (once && i != fileIndex){continue;
				}
				
				auto fd = IndexCRAF::GetFileData(crafData, i, false);
				
				std::ofstream writeFile;
				
				uint32 fileType = IndexCRAF::GetFileType(crafData, i);
				std::string fileTypeStr = ((fileType != 0x05 && fileType != 0x00) ? IndexCRAF::GetFileTypeStr(crafData, i) : "bin"); //0x05 == Unknown Binary
				writeFile.open((dir+'\\'+std::string("EncodedFile")+std::to_string(i)+'.'+fileTypeStr).c_str(), std::ios::binary);
				writeFile.write((char*)&fd[0], fd.size() * sizeof(uint8));
				writeFile.close();
				
				fd = KadaTools::DecodeRLE2(fd);
				writeFile.open((dir+'\\'+std::string("File")+std::to_string(i)+'.'+fileTypeStr).c_str(), std::ios::binary);
				writeFile.write((char*)&fd[0], fd.size() * sizeof(uint8));
				writeFile.close();
				
				if (fileType == 0x03) //0x03 == SHTXPS
				{
					bool isLoaded;
					cg::Image image = IndexSHTXPS::LoadImage(fd, &isLoaded);
					
					std::vector<uint8> p;
					//BGR-A -> RGBA
					for (uint32 i = 0; i < image.getWidth() * image.getHeight(); ++i)
					{
						p.push_back(cg::GetR(image[i]->first));
						p.push_back(cg::GetG(image[i]->first));
						p.push_back(cg::GetB(image[i]->first));
						p.push_back(image[i]->second);
					}
					
					if (isLoaded){lodepng::encode(dir+'\\'+std::string("File")+std::to_string(i)+".png", p.data(), image.getWidth(), image.getHeight());
					}
				}
			}
			
			CRAFEditor::PrintOutput("[CRAF_EXTRACT]: Extracted files saved to \""+dir+"\".");
		}
		
		if (keyboard.isKeyDown('K'))
		{
			if (listCommands.size() > 0)
			{
				clearInput();
				std::cout<<"In current folder \"CRAFLists\""<<std::endl;
				std::cout<<"Enter a filename to save the list as:"<<std::flush;
				std::string fileName;
				std::getline(std::cin, fileName);
				fileName = "CRAFLists\\"+fileName;
				
				std::ofstream writeFile(fileName.c_str());
				if (writeFile.is_open())
				{
					for (std::string &cmd : listCommands){writeFile.write(&cmd[0], cmd.size() * sizeof(char));
					}
					writeFile.close();
				} else CRAFEditor::PrintError("ERROR [CREATE_LIST]: Can\'t create \""+fileName+"\".");
			} else CRAFEditor::PrintError("ERROR [CREATE_LIST]: listCommands.size() == 0");
		}
		if (keyboard.isKeyDown('L'))
		{
			clearInput();
			std::cout<<"In current folder \"CRAFLists\""<<std::endl;
			std::cout<<"Enter the name of the CRAF file to load:"<<std::flush;
			std::string fileName;
			std::getline(std::cin, fileName);
			fileName = "CRAFLists\\"+fileName;
			
			LoadList(crafData, crafFiles, fileName, settings);
			load = true;
		}
		
		graphics.drawRect(0, 0, graphics.getWidth(), graphics.getHeight(), 0x005F5FAF);
		
		uint32 w = preview.getWidth(), h = preview.getHeight();
		float f = 256.f/(float)std::max(w, h);
		preview.setPos((graphics.getWidth()/2)-(w*f/2), (graphics.getHeight()/2)-(h*f/2));
		drawImage(graphics, preview, f, cg::DrawType::Resize);
		
		//Top
		uint32 rows = 2;
		graphics.drawRectA(0, 0, graphics.getWidth(), text.getCharHeight() * rows, 0x00FFFFFF, 0x3F);
		
		text.setPos(0, 0);
		text.setText("FileIndex="+std::to_string(fileIndex)+"\nESC=Exit");
		graphics.draw(text);
		
		text.setText(fileInfo);
		text.setPos((graphics.getWidth()/2)-(text.getWidth()/2), 0);
		graphics.draw(text);
		
		text.setText("NumFiles="+std::to_string(crafFiles.size()));
		text.setPos(graphics.getWidth() - text.getWidth(), 0);
		graphics.draw(text);
		
		//Bottom
		rows = 3;
		uint32 y = graphics.getHeight() - (text.getCharHeight() * rows);
		graphics.drawRectA(0, y, graphics.getWidth(), text.getCharHeight() * rows, 0x00FFFFFF, 0x3F);
		
		text.setPos(0, y);
		text.setText("A=Add file\nX=Remove current file \nR=Replace file");
		graphics.draw(text);
		
		text.setText("E=Extract all files\nW=Extract current file \nC=Create new CRAF");
		text.setPos((graphics.getWidth()/2)-(text.getWidth()/2), y);
		graphics.draw(text);
		
		text.setText("L=Load list\nK=Create list\nLEFT\\RIGHT=Move");
		text.setPos(graphics.getWidth() - text.getWidth(), y);
		graphics.draw(text);
		
		graphics.display();
		
		keyboard.update(deltaTime);
	}*/