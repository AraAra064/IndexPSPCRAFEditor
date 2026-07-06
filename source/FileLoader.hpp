#include <string>
#include <vector>
#include <fstream>

#include "ConsoleGraphics.hpp"
#include "lodepng/lodepng.cpp"

std::vector<uint8_t> LoadFile(std::string fileName)
{
	std::vector<uint8_t> retVal;
	std::ifstream readFile(fileName.c_str(), std::ios::binary);

	if (readFile.is_open())
	{
		readFile.seekg(0, std::ios::end);
		retVal.resize((size_t)readFile.tellg());
		readFile.seekg(0, std::ios::beg);
		readFile.read((char*)retVal.data(), retVal.size());
		readFile.close();
	}

	return retVal;
}

bool LoadPNG(cg::Image& image, std::string fileName)
{
	std::vector<uint8_t> pixelData;
	unsigned int width, height;
	bool v = (lodepng::decode(pixelData, width, height, fileName) == 0);
	if (v)
	{
		for (uint32_t i = 0; i < pixelData.size() - 3; i += 4) {
			std::swap(pixelData[i], pixelData[i + 2]); //RGB to BGR
		}
		image.loadImageFromArray((uint32_t*)pixelData.data(), width, height, true);
	}
	return v;
}

bool LoadImg(cg::Image& image, std::string fileName)
{
	for (auto& c : fileName)
	{
		c = toupper(c);
	}
	//std::transform(str.begin(), str.end(), str.begin(), [](char c)->char {return toupper(c);});

	bool b;
	if (fileName.find(".PNG") != std::string::npos) {
		b = LoadPNG(image, fileName);
	}
	else b = (image.loadImage(fileName) != 0);
	return b;
}

std::vector<uint32_t> CGImgTo32Int(cg::Image& img)
{
	std::vector<uint32_t> pixels(img.getWidth() * img.getHeight());

	for (size_t i = 0; i < pixels.size(); i++)
	{
		int x = (i % img.getWidth());
		int y = i / img.getHeight();
		auto p = img.accessPixel(x, y);
		pixels[i] = p->first | ((uint32_t)p->second << 24);
	}

	return pixels;
}