/*Copyright (c) 2018 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#include "MapAreas.h"
#include "Log.h"
#include <fstream>
#include <functional>



EU4World::mapAreas::mapAreas(const std::string& filename):
	theAreas()
{
	registerKeyword(std::regex("[\\w_]+"), [this](const std::string& areaName, std::istream& areasFile)
		{
			mapArea newArea(areasFile);
			theAreas.insert(make_pair(areaName, newArea));
		}
	);

	parseFile(filename);
}


const std::vector<int> EU4World::mapAreas::getProvincesInArea(const std::string& area) const
{
	auto areaItr(theAreas.find(area));
	if (areaItr != theAreas.end())
	{
		return areaItr->second.getProvinces();
	}
	else
	{
		return {};
	}
}


EU4World::mapArea::mapArea(std::istream& theStream)
{
	registerKeyword(std::regex("color"), [this](const std::string& colorToken, std::istream& areaFile)
		{
			Color newColor(areaFile);
			color = newColor;
		}
	);

	auto token = getNextToken(theStream);
	while (token && (*token != "}"))
	{
		if ((token->find('=') == std::string::npos) && (token->find('{') == std::string::npos))
		{
			provinces.push_back(std::stoi(*token));
		}
		token = getNextToken(theStream);
	}
}