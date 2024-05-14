#ifndef TEXT_H
#define TEXT_H
#include "olcPixelGameEngine.h"

#pragma once
class Text
{
public:
	Text() : sText(""), size(0) {};
	Text(std::string sText, int size) : sText(sText), size(size) {};
	std::string sText;
	int size;
	int getSize() { return size; };
};

#endif //TEXT_H
