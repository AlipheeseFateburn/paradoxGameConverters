#include "EU3World.h"
#include <string>
#include <queue>
#include <algorithm>
#include <io.h>
#include <set>
#include "..\Log.h"
#include "..\temp.h"
#include "..\Configuration.h"
#include "..\Parsers\Parser.h"
#include "..\Parsers\Object.h"
#include "..\CK2World\CK2Barony.h"
#include "..\CK2World\CK2Title.h"
#include "..\CK2World\CK2Province.h"
#include	"..\CK2World\CK2World.h"
#include "..\CK2World\CK2Character.h"
#include "EU3Province.h"
#include "EU3Country.h"
#include "EU3Ruler.h"
#include "EU3Advisor.h"
#include "EU3Diplomacy.h"
using namespace std;



EU3World::EU3World(CK2World* srcWorld, EU3Tech* _techData)
{
	startDate = srcWorld->getEndDate();
	techData = _techData;

	provinces.clear();
	countries.clear();
	convertedCountries.clear();
	advisors.clear();
	mapSpreadStrings.clear();

	if (Configuration::getAdvisors() == "normal")
	{
		options[ADVISORS] = 0;
	}
	else if (Configuration::getAdvisors() == "historical")
	{
		options[ADVISORS] = 1;
	}
	else if (Configuration::getAdvisors() == "event")
	{
		options[ADVISORS] = 2;
	}

	if (Configuration::getLeaders() == "normal")
	{
		options[LEADERS] = 0;
	}
	else if (Configuration::getLeaders() == "historical")
	{
		options[LEADERS] = 1;
	}
	else if (Configuration::getLeaders() == "event")
	{
		options[LEADERS] = 2;
	}

	if (Configuration::getColonists() == "normal")
	{
		options[COLONISTS] = 0;
	}
	else if (Configuration::getColonists() == "free")
	{
		options[COLONISTS] = 1;
	}

	if (Configuration::getMerchants() == "normal")
	{
		options[MERCHANTS] = 0;
	}
	else if (Configuration::getMerchants() == "free")
	{
		options[MERCHANTS] = 1;
	}

	if (Configuration::getMissionaries() == "normal")
	{
		options[MISSIONARIES] = 0;
	}
	else if (Configuration::getMissionaries() == "free")
	{
		options[MISSIONARIES] = 1;
	}

	if (Configuration::getInflation() == "normal")
	{
		options[INFLATION] = 0;
	}
	else if (Configuration::getInflation() == "from gold")
	{
		options[INFLATION] = 1;
	}
	else if (Configuration::getInflation() == "none")
	{
		options[INFLATION] = 2;
	}

	if (Configuration::getColonist_size() == "normal")
	{
		options[COLONIST_SIZE] = 0;
	}
	else if (Configuration::getColonist_size() == "200")
	{
		options[COLONIST_SIZE] = 1;
	}
	else if (Configuration::getColonist_size() == "300")
	{
		options[COLONIST_SIZE] = 2;
	}
	else if (Configuration::getColonist_size() == "400")
	{
		options[COLONIST_SIZE] = 3;
	}

	if (Configuration::getDifficulty() == "very easy")
	{
		options[DIFFICULTY] = 0;
	}
	else if (Configuration::getDifficulty() == "easy")
	{
		options[DIFFICULTY] = 1;
	}
	else if (Configuration::getDifficulty() == "normal")
	{
		options[DIFFICULTY] = 2;
	}
	else if (Configuration::getDifficulty() == "hard")
	{
		options[DIFFICULTY] = 3;
	}
	else if (Configuration::getDifficulty() == "very hard")
	{
		options[DIFFICULTY] = 4;
	}

	if (Configuration::getAI_aggressiveness() == "normal")
	{
		options[AI_AGGRESSIVENESS] = 0;
	}
	else if (Configuration::getAI_aggressiveness() == "low")
	{
		options[AI_AGGRESSIVENESS] = 1;
	}
	else if (Configuration::getAI_aggressiveness() == "high")
	{
		options[AI_AGGRESSIVENESS] = 2;
	}

	if (Configuration::getLand_spread() == "25")
	{
		options[LAND_SPREAD] = 0;
	}
	else if (Configuration::getLand_spread() == "normal (50)")
	{
		options[LAND_SPREAD] = 1;
	}
	else if (Configuration::getLand_spread() == "75")
	{
		options[LAND_SPREAD] = 2;
	}
	else if (Configuration::getLand_spread() == "100")
	{
		options[LAND_SPREAD] = 3;
	}
	else if (Configuration::getLand_spread() == "200")
	{
		options[LAND_SPREAD] = 4;
	}

	if (Configuration::getSea_spread() == "25")
	{
		options[SEA_SPREAD] = 0;
	}
	else if (Configuration::getSea_spread() == "normal (50)")
	{
		options[SEA_SPREAD] = 1;
	}
	else if (Configuration::getSea_spread() == "75")
	{
		options[SEA_SPREAD] = 2;
	}
	else if (Configuration::getSea_spread() == "100")
	{
		options[SEA_SPREAD] = 3;
	}
	else if (Configuration::getSea_spread() == "200")
	{
		options[SEA_SPREAD] = 4;
	}

	if (Configuration::getSpies() == "normal")
	{
		options[SPIES] = 0;
	}
	else if (Configuration::getSpies() == "free")
	{
		options[SPIES] = 1;
	}

	if (Configuration::getLucky_nations() == "historical")
	{
		options[LUCKY_NATIONS] = 0;
	}
	else if (Configuration::getLucky_nations() == "random")
	{
		options[LUCKY_NATIONS] = 1;
	}
	else if (Configuration::getLucky_nations() == "none")
	{
		options[LUCKY_NATIONS] = 2;
	}

	diplomacy = new EU3Diplomacy();

	japaneseEmperor	= NULL;
	daimyos.clear();
	shogun				= NULL;
	shogunPower			= -1.0f;
}


void EU3World::output(FILE* output)
{
	fprintf(output, "date=\"%s\"\n", startDate.toString().c_str());
	outputTempHeader(output);
	fprintf(output, "gameplaysettings=\n");
	fprintf(output, "{\n");
	fprintf(output, "\tsetgameplayoptions=\n");
	fprintf(output, "\t{\n");
	fprintf(output, "\t\t");
	for (unsigned int i = 0; i < OPTIONS_END; i++)
	{
		fprintf(output, "%d ", options[i]);
	}
	fprintf(output, "\n");
	fprintf(output, "\t}\n");
	fprintf(output, "}\n");
	outputTempHeader2(output);
	for (map<int, EU3Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		if (i->second != NULL)
		{
			i->second->output(output);
		}
		else
		{
			log("\tError: EU3 province %d is unmapped!\n", i->first);
		}
	}
	for (map<string, EU3Country*>::iterator i = countries.begin(); i != countries.end(); i++)
	{
		i->second->output(output);
	}
	fprintf(output, "active_advisors=\n");
	fprintf(output, "{\n");
	fprintf(output, "\tnotechgroup=\n");
	fprintf(output, "\t{\n");
	for (unsigned int i = 0; i < advisors.size(); i++)
	{
		advisors[i]->outputInActive(output);
	}
	fprintf(output, "\t}\n");
	fprintf(output, "}\n");

	diplomacy->output(output);

	if (shogunPower != -1.0f)
	{
		fprintf(output, "shogun=\n");
		fprintf(output, "{\n");
		fprintf(output, "\tshogun=%f\n", shogunPower);
		fprintf(output, "\temperor=\n");
		fprintf(output, "\t{\n");
		fprintf(output, "\t\tstatus=0\n");
		fprintf(output, "\t\temperor=1\n");
		fprintf(output, "\t\tback_kampaku=0\n");
		fprintf(output, "\t\tactive=1\n");
		fprintf(output, "\t\tcountry=\"%s\"\n", japaneseEmperor->getTag().c_str());
		fprintf(output, "\t}\n");
		for(unsigned int i = 0; i < daimyos.size(); i++)
		{
			fprintf(output, "\tdaimyo=\n");
			fprintf(output, "\t{\n");
			if (daimyos[i] == shogun)
			{
				fprintf(output, "\t\tstatus=3\n");
			}
			else
			{
				fprintf(output, "\t\tstatus=0\n");
			}
			fprintf(output, "\t\temperor=0\n");
			fprintf(output, "\t\tback_kampaku=0\n");
			fprintf(output, "\t\tactive=1\n");
			fprintf(output, "\t\tcountry=\"%s\"\n", daimyos[i]->getTag().c_str());
			fprintf(output, "\t}\n");
		}
		fprintf(output, "}\n");
	}
}


void EU3World::addHistoricalCountries()
{
	string EU3Loc = Configuration::getEU3Path();

	struct _finddata_t	countryDirData;
	intptr_t					fileListing;
	if ( (fileListing = _findfirst( (EU3Loc + "\\history\\countries\\*").c_str(), &countryDirData)) == -1L)
	{
		log("Error: Could not open country history directory.\n");
		return;
	}
	do
	{
		if (strcmp(countryDirData.name, ".") == 0 || strcmp(countryDirData.name, "..") == 0 )
		{
				continue;
		}

		string filename;
		filename = countryDirData.name;

		string tag;
		tag = filename.substr(0, 3);
		transform(tag.begin(), tag.end(), tag.begin(), toupper);

		if (tag == "REB")
		{
			continue;
		}

		EU3Country* newCountry = new EU3Country(this, tag, filename, startDate, techData);
		countries.insert(make_pair(tag, newCountry));

	} while(_findnext(fileListing, &countryDirData) == 0);
	_findclose(fileListing);
}


void EU3World::setupProvinces(provinceMapping& provinceMap)
{
	for (provinceMapping::iterator i = provinceMap.begin(); i != provinceMap.end(); i++)
	{
		//parse relevant file
		Object* obj;
		char num[5];
		_itoa_s(i->first, num, 5, 10);
		
		string filename = Configuration::getEU3Path() + "\\history\\provinces\\" + num + "*-*.txt";
		struct _finddata_t	fileData;
		intptr_t					fileListing;
		if ( (fileListing = _findfirst(filename.c_str(), &fileData)) != -1L)
		{
			obj = doParseFile( (Configuration::getEU3Path() + "\\history\\provinces\\" + fileData.name).c_str() );
			if (obj == NULL)
			{
				log("Error: Could not open %s\n", (Configuration::getEU3Path() + "\\history\\provinces\\" + fileData.name).c_str());
				printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "\\history\\provinces\\" + fileData.name).c_str());
				exit(-1);
			}
			_findclose(fileListing);
		}
		else
		{
			obj = new Object("NULL Object");
		}

		//initialize province
		EU3Province* newProvince = new EU3Province(i->first, obj, startDate);

		//add to provinces list
		provinces.insert( make_pair(i->first, newProvince) );
	}
}


void EU3World::convertCountries(map<string, CK2Title*> CK2Titles, const religionMapping& religionMap, const cultureMapping& cultureMap, const inverseProvinceMapping inverseProvinceMap)
{
	for (map<string, CK2Title*>::iterator titleItr = CK2Titles.begin(); titleItr != CK2Titles.end(); titleItr++)
	{
		EU3Country* newCountry = new EU3Country(titleItr->second, religionMap, cultureMap, inverseProvinceMap);
		convertedCountries.push_back(newCountry);
	}
}


void EU3World::convertProvinces(provinceMapping& provinceMap, map<int, CK2Province*>& allSrcProvinces, cultureMapping& cultureMap, religionMapping& religionMap, continentMapping& continentMap, adjacencyMapping& adjacencyMap, const tradeGoodMapping& tradeGoodMap, const religionGroupMapping& EU3ReligionGroupMap)
{
	double totalHistoricalBaseTax		= 0.0f;
	double totalHistoricalPopulation = 0.0f;
	double totalHistoricalManpower	= 0.0f;
	for (provinceMapping::const_iterator provItr = provinceMap.begin(); provItr != provinceMap.end(); provItr++)
	{
		if (provItr->second[0] != 0)
		{
			totalHistoricalBaseTax		+= provinces.find(provItr->first)->second->getBaseTax();
			totalHistoricalPopulation	+= provinces.find(provItr->first)->second->getPopulation();
			totalHistoricalManpower		+= provinces.find(provItr->first)->second->getManpower();
		}
	}
	log("\tTotal historical base tax is %f.\n", totalHistoricalBaseTax);
	log("\tTotal historical population is %f.\n", totalHistoricalPopulation);
	log("\tTotal historical manpower is %f.\n", totalHistoricalManpower);

	double totalBaseTaxProxy	= 0.0f;
	double totalPopProxy			= 0.0f;
	double totalManpowerProxy	= 0.0f;
	for (map<int, CK2Province*>::const_iterator srcItr = allSrcProvinces.begin(); srcItr != allSrcProvinces.end(); srcItr++)
	{
		vector<CK2Barony*> baronies = srcItr->second->getBaronies();
		for (vector<CK2Barony*>::const_iterator baronyItr = baronies.begin(); baronyItr != baronies.end(); baronyItr++)
		{
			totalBaseTaxProxy		+= (*baronyItr)->getBaseTaxProxy();
			totalPopProxy			+= (*baronyItr)->getPopProxy();
			totalManpowerProxy	+= (*baronyItr)->getManpowerProxy();
		}
	}

	for(provinceMapping::iterator i = provinceMap.begin(); i != provinceMap.end(); i++)
	{
		if (i->second[0] == 0)
		{
			map<int, EU3Province*>::iterator		provItr	= provinces.find(i->first);
			map<string, EU3Country*>::iterator	owner		= countries.find(provItr->second->getOwnerStr());
			if (owner != countries.end())
			{
				provItr->second->setOwner(owner->second);
			}
			else
			{
				provItr->second->setOwner(NULL);
			}
			continue;
		}
		
		vector<int>	srcProvinceNums = i->second;
		vector<CK2Province*> srcProvinces;
		for (unsigned j = 0; j < srcProvinceNums.size(); j++)
		{
			CK2Province* srcProvince = allSrcProvinces[ srcProvinceNums[j] ];
			if (srcProvince != NULL)
			{
				srcProvinces.push_back(srcProvince);
			}
		}

		vector<CK2Barony*> baronies;
		int numBaronies = 0;
		for (unsigned int j = 0; j < srcProvinces.size(); j++)
		{
			vector<CK2Barony*> srcBaronies = srcProvinces[j]->getBaronies();
			for(unsigned int k = 0; k < srcBaronies.size(); k++)
			{
				baronies.push_back(srcBaronies[k]);
				numBaronies++;
			}
		}

		double	baseTaxProxy	= 0.0f;
		double	popProxy			= 0.0f;
		double	manpowerProxy	= 0.0f;
		bool		inHRE		= false;
		vector< pair<const CK2Title*, int > > owners;	// ownerTitle, numBaronies
		for (unsigned int j = 0; j < baronies.size(); j++)
		{
			baseTaxProxy	+= baronies[j]->getBaseTaxProxy();
			popProxy			+= baronies[j]->getPopProxy();
			manpowerProxy	+= baronies[j]->getManpowerProxy();

			const CK2Title* title = baronies[j]->getTitle();
			while( !title->isIndependent() )
			{
				title = title->getLiege();
			}

			bool ownerFound = false;
			for(unsigned int k = 0; k < owners.size(); k++)
			{
				if (owners[k].first == title)
				{
					owners[k].second++;
					ownerFound = true;
				}
			}
			if (!ownerFound)
			{
				owners.push_back( make_pair(title, 1) );
				if (title->isInHRE())
				{
					inHRE = true;
				}
			}
		}

		vector<EU3Country*> cores;
		for (vector<CK2Province*>::iterator provItr = srcProvinces.begin(); provItr != srcProvinces.end(); provItr++)
		{
			vector<CK2Barony*> srcBaronies = (*provItr)->getBaronies();
			if (srcBaronies.size() > 0)
			{
				const CK2Title* current	= srcBaronies[0]->getTitle();
				const CK2Title* next		= current->getDeJureLiege();
				while( (next != NULL) && /*( next->getHolder() != NULL) &&*/ (next->getTitleString() != Configuration::getHRETitle()) )
				{
					EU3Country* core = next->getDstCountry();
					if (core != NULL)
					{
						cores.push_back(core);
					}
					current	= next;
					next		= current->getDeJureLiege();
				}
				if ( (next != NULL) && (next->getTitleString() == Configuration::getHRETitle()) )
				{
					inHRE = true;
				}
			}
		}

		if (Configuration::getMultipleProvsMethod() == "average")
		{
			baseTaxProxy	/= srcProvinces.size();
			popProxy			/= srcProvinces.size();
			manpowerProxy	/= srcProvinces.size();
		}

		map<int, EU3Province*>::iterator provItr = provinces.find(i->first);
		provItr->second->convert(i->first, inHRE, srcProvinces, cores);
		for (vector<EU3Country*>::iterator coreItr = cores.begin(); coreItr != cores.end(); coreItr++)
		{
			(*coreItr)->addCore(provItr->second);
		}

		const CK2Title*	greatestOwner;
		int					greatestOwnerNum = 0;
		for (unsigned int j = 0; j < owners.size(); j++)
		{
			provItr->second->addCore(owners[j].first->getDstCountry());
			owners[j].first->getDstCountry()->addCore(provItr->second);
			if (owners[j].second > greatestOwnerNum)
			{
				greatestOwner		= owners[j].first;
				greatestOwnerNum	= owners[j].second;
			}
		}
		if (owners.size() > 0)
		{
			provItr->second->setOwner(greatestOwner->getDstCountry());
			provItr->second->setSrcOwner(greatestOwner);
			greatestOwner->getDstCountry()->addProvince(provItr->second);
			provItr->second->setContinent(continentMap[provItr->first]);
			if ( continentMap[greatestOwner->getDstCountry()->getCapital()] == continentMap[provItr->first])
			{
				provItr->second->setSameContinent(true);
			}
			else
			{
				provItr->second->setSameContinent(false);
			}
		}

		if (Configuration::getBasetax() == "converted")
		{
			provItr->second->setBaseTax(totalHistoricalBaseTax * baseTaxProxy / totalBaseTaxProxy);
		}
		else if (Configuration::getBasetax() == "blended")
		{
			double blendAmount = atof( Configuration::getBasetaxblendamount().c_str() );
			provItr->second->setBaseTax( (blendAmount * provItr->second->getBaseTax()) + ((1 - blendAmount) * totalHistoricalBaseTax * baseTaxProxy / totalBaseTaxProxy) );
		}
		if (Configuration::getPopulation() == "converted")
		{
			provItr->second->setPopulation(totalHistoricalPopulation * popProxy / totalPopProxy);
		}
		else if (Configuration::getPopulation() == "blended")
		{
			double blendAmount = atof( Configuration::getPopulationblendamount().c_str() );
			provItr->second->setPopulation( (blendAmount * provItr->second->getPopulation()) + ((1 - blendAmount) * totalHistoricalPopulation * popProxy / totalPopProxy) );
		}
		else if (Configuration::getPopulation() == "historical")
		{
			if (provItr->second->getPopulation() < 1000.0f)
			{
				provItr->second->setPopulation(1000.0f);
			}
		}
		if (Configuration::getManpower() == "converted")
		{
			provItr->second->setManpower(totalHistoricalManpower * manpowerProxy / totalManpowerProxy);
		}
		if (Configuration::getManpower() == "blended")
		{
			double blendAmount = atof( Configuration::getManpowerblendamount().c_str() );
			provItr->second->setManpower( (blendAmount * provItr->second->getManpower()) + ((1 - blendAmount) * totalHistoricalManpower * manpowerProxy / totalManpowerProxy) );
		}
		provItr->second->determineCulture(cultureMap, srcProvinces, baronies);
		provItr->second->determineReligion(religionMap, srcProvinces);
	}

	// find all land connections to capitals
	for (map<string, EU3Country*>::iterator countryItr = countries.begin(); countryItr != countries.end(); countryItr++)
	{
		map<int, EU3Province*>	openProvinces = provinces;
		queue<int>					goodProvinces;

		map<int, EU3Province*>::iterator openItr = openProvinces.find(countryItr->second->getCapital());
		if (openItr == openProvinces.end())
		{
			continue;
		}
		openItr->second->setLandConnection(true);
		goodProvinces.push(openItr->first);
		openProvinces.erase(openItr);

		do 
		{
			int currentProvince = goodProvinces.front();
			goodProvinces.pop();
			vector<adjacency> adjacencies = adjacencyMap[currentProvince];
			for (unsigned int i = 0; i < adjacencies.size(); i++)
			{
				map<int, EU3Province*>::iterator openItr = openProvinces.find(i);
				if (openItr == openProvinces.end())
				{
					continue;
				}
				if (!openItr->second->isLand())
				{
					continue;
				}
				if (openItr->second->getOwner() != countryItr->second)
				{
					continue;
				}
				openItr->second->setLandConnection(true);
				goodProvinces.push(openItr->first);
				openProvinces.erase(openItr);
			}
		} while (goodProvinces.size() > 1);
	}

	// Determine supply and demand
	for(map<int, EU3Province*>::iterator itr = provinces.begin(); itr != provinces.end(); itr++)
	{
		if (itr->second->getOwner() == NULL)
		{
			continue;
		}
		itr->second->determineGoodsSupply(tradeGoodMap);
		itr->second->determineGoodsDemand(tradeGoodMap, EU3ReligionGroupMap);
	}
}


void EU3World::addAcceptedCultures()
{
	for(map<string, EU3Country*>::iterator countryItr = countries.begin(); countryItr != countries.end(); countryItr++)
	{
		countryItr->second->addAcceptedCultures();
	}
}


void EU3World::convertAdvisors(inverseProvinceMapping& inverseProvinceMap, provinceMapping& provinceMap, CK2World& srcWorld)
{
	// CK2 Advisors
	map<string, CK2Title*> titles = srcWorld.getAllTitles();
	map<int, CK2Character*> rulers;
	for (map<string, CK2Title*>::iterator i = titles.begin(); i != titles.end(); i++)
	{
		CK2Character* ruler = i->second->getHolder();
		if (ruler != NULL)
		{
			rulers.insert( make_pair(ruler->getNum(), ruler) );
		}
	}

	for (map<int, CK2Character*>::iterator i = rulers.begin(); i != rulers.end(); i++)
	{
		CK2Character** srcAdvisors = i->second->getAdvisors();
		for (unsigned int i = 0; i < 5; i++)
		{
			if (srcAdvisors[i] != NULL)
			{
				EU3Advisor*	newAdvisor		= new EU3Advisor(srcAdvisors[i], inverseProvinceMap, provinces, startDate);
				int			CK2Location		= srcAdvisors[i]->getLocationNum();

				vector<int>		provinceNums;
				provinceNums.clear();
				if (CK2Location != NULL)
				{
					provinceNums = inverseProvinceMap[CK2Location];
				}

				EU3Province*	advisorHome = NULL;
				if (provinceNums.size() > 0)
				{
					advisorHome = provinces[ provinceNums[0] ];
					newAdvisor->setLocation( provinceNums[0] );
				}
				if (advisorHome != NULL)
				{
					advisorHome->addAdvisor(newAdvisor);
					advisors.push_back(newAdvisor);
				}
			}
		}
	}

	// ROTW Advisors
	string EU3Loc = Configuration::getEU3Path();

	struct _finddata_t	advisorDirData;
	intptr_t					fileListing;
	if ( (fileListing = _findfirst( (EU3Loc + "\\history\\advisors\\*").c_str(), &advisorDirData)) == -1L)
	{
		log("\tError: Could not open advisors history directory.\n");
		return;
	}
	do
	{
		if (strcmp(advisorDirData.name, ".") == 0 || strcmp(advisorDirData.name, "..") == 0 )
		{
				continue;
		}

		string filename;
		filename = advisorDirData.name;

		Object* obj;
		obj = doParseFile((Configuration::getEU3Path() + "/history/advisors/" + filename).c_str());
		if (obj == NULL)
		{
			log("Error: Could not open %s\n", (Configuration::getEU3Path() + "/history/advisors/" + filename).c_str());
			printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "/history/advisors/" + filename).c_str());
			exit(-1);
		}

		vector<Object*> advisorsObj = obj->getLeaves();
		for (unsigned int i = 0; i < advisorsObj.size(); i++)
		{
			EU3Advisor* newAdvisor = new EU3Advisor(advisorsObj[i], provinces);
			if ( (newAdvisor->getStartDate() < startDate) && (startDate < newAdvisor->getDeathDate()) )
			{
				vector<int> srcLocationNums = provinceMap[newAdvisor->getLocation()];
				if ( (srcLocationNums.size() == 1) && (srcLocationNums[0] == 0) )
				{
					provinces[newAdvisor->getLocation()]->addAdvisor(newAdvisor);
					advisors.push_back(newAdvisor);
				}
			}
		}

	} while(_findnext(fileListing, &advisorDirData) == 0);
	_findclose(fileListing);
}


void EU3World::convertTech(const religionGroupMapping& religionGroupMap, const CK2World& srcWorld)
{
	vector<double> avgTechLevels = srcWorld.getAverageTechLevels();

	double highestLearningScore = 0.0f;
	for (vector<EU3Country*>::iterator countryItr = convertedCountries.begin(); countryItr != convertedCountries.end(); countryItr++)
	{
		(*countryItr)->determineLearningScore();
		if ((*countryItr)->getLearningScore() > highestLearningScore)
		{
			highestLearningScore = (*countryItr)->getLearningScore();
		}
	}

	for (vector<EU3Country*>::iterator countryItr = convertedCountries.begin(); countryItr != convertedCountries.end(); countryItr++)
	{
		string religion	= (*countryItr)->getSrcCountry()->getHolder()->getReligion();
		string title		= (*countryItr)->getSrcCountry()->getTitleString();
		if (  ( (title == "e_golden_horde") || (title == "e_il-khanate") || (title == "e_timurids") ) && (religionGroupMap.find(religion)->second != "christian")  )
		{
			(*countryItr)->setTechGroup("nomad_group");
			log("\t%s is in tech group nomad.\n", (*countryItr)->getSrcCountry()->getTitleString().c_str());
		}
		else if ((*countryItr)->getLearningScore() <= 0.2 * highestLearningScore)
		{
			(*countryItr)->setTechGroup("sub_saharan");
			log("\t%s is in tech group sub saharan.\n", (*countryItr)->getSrcCountry()->getTitleString().c_str());
		}
		else if ((*countryItr)->getLearningScore() <= 0.75 * highestLearningScore)
		{
			(*countryItr)->setTechGroup("muslim");
			log("\t%s is in tech group muslim.\n", (*countryItr)->getSrcCountry()->getTitleString().c_str());
		}
		else if ((*countryItr)->getLearningScore() <= 0.8 * highestLearningScore)
		{
			(*countryItr)->setTechGroup("ottoman");
			log("\t%s is in tech group ottoman.\n", (*countryItr)->getSrcCountry()->getTitleString().c_str());
		}
		else if ((*countryItr)->getLearningScore() <= 0.85 * highestLearningScore)
		{
			(*countryItr)->setTechGroup("eastern");
			log("\t%s is in tech group eastern.\n", (*countryItr)->getSrcCountry()->getTitleString().c_str());
		}
		else
		{
			(*countryItr)->setTechGroup("western");
			log("\t%s is in tech group western.\n", (*countryItr)->getSrcCountry()->getTitleString().c_str());
		}

		(*countryItr)->determineTechLevels(avgTechLevels, techData);
	}

	for(map<string, EU3Country*>::iterator countryItr = countries.begin(); countryItr != countries.end(); countryItr++)
	{
		countryItr->second->determineTechInvestment(techData, startDate);
	}
}


void EU3World::convertGovernments(const religionGroupMapping& religionGroupMap)
{
	for (vector<EU3Country*>::iterator itr = convertedCountries.begin(); itr < convertedCountries.end(); itr++)
	{
		(*itr)->determineGovernment(religionGroupMap);
	}
}


void EU3World::convertEconomies(const cultureGroupMapping& cultureGroups, const tradeGoodMapping& tradeGoodMap)
{
	// get goods supply and demand
	map<string, double> goodsSupply;
	map<string, double> goodsDemand;
	for (map<int, EU3Province*>::iterator provItr = provinces.begin(); provItr != provinces.end(); provItr++)
	{
		provItr->second->addSupplyContribution(goodsSupply);
		provItr->second->addDemandContribution(goodsDemand);
	}
	for (map<string, double>::iterator goodItr = goodsSupply.begin(); goodItr != goodsSupply.end(); goodItr++)
	{
		log("\tSupply of %s: %f\n", goodItr->first.c_str(), goodItr->second);
		if (goodItr->second < 0.01)
		{
			goodItr->second = 0.01;
		}
		if (goodItr->second > 2.00)
		{
			goodItr->second = 2.00;
		}
	}
	for (map<string, double>::iterator goodItr = goodsDemand.begin(); goodItr != goodsDemand.end(); goodItr++)
	{
		log("\tDemand for %s: %f\n", goodItr->first.c_str(), goodItr->second);
		if (goodItr->second < 0.01)
		{
			goodItr->second = 0.01;
		}
		if (goodItr->second > 2.00)
		{
			goodItr->second = 2.00;
		}
	}

	// find prices for various goods
	map<string, double> unitPrices;
	for (tradeGoodMapping::const_iterator tradeItr = tradeGoodMap.begin(); tradeItr != tradeGoodMap.end(); tradeItr++)
	{
		map<string, double>::iterator supplyItr = goodsSupply.find(tradeItr->first);
		if (supplyItr == goodsSupply.end())
		{
			log("Error: no supply for trade good %s.\n", tradeItr->first);
			continue;
		}
		map<string, double>::iterator demandItr = goodsDemand.find(tradeItr->first);
		if (demandItr == goodsDemand.end())
		{
			log("Error: no demand for trade good %s.\n", tradeItr->first);
			continue;
		}
		double price = tradeItr->second.basePrice * (2.25 - supplyItr->second) * demandItr->second;
		unitPrices.insert(make_pair(tradeItr->first, price));
	}

	// determine economies
	for (map<string, EU3Country*>::iterator countryItr = countries.begin(); countryItr != countries.end(); countryItr++)
	{
		if (countryItr != countries.end())
		{
			countryItr->second->determineEconomy(cultureGroups, unitPrices);
		}
	}
}


int EU3World::assignTags(Object* rulesObj, vector<string>& blockedNations, const provinceMapping& provinceMap)
{
	// get CK2 Titles
	map<string, CK2Title*> CK2Titles;
	for (vector<EU3Country*>::iterator countryItr = convertedCountries.begin(); countryItr != convertedCountries.end(); countryItr++)
	{
		CK2Title* srcTitle = (*countryItr)->getSrcCountry();
		CK2Titles.insert( make_pair(srcTitle->getTitleString(), srcTitle) );
	}

	// get EU3 tags
	set<string> EU3tags;
	for (map<string, EU3Country*>::iterator countryItr =	countries.begin(); countryItr != countries.end(); countryItr++)
	{
		EU3tags.insert(countryItr->first);
	}

	// get rules
	vector<Object*> leaves = rulesObj->getLeaves();
	if (leaves.size() < 1)
	{
		log ("Error: No country mapping definitions loaded.\n");
		printf("Error: No country mapping definitions loaded.\n");
		return -1;
	}
	vector<Object*> rules = leaves[0]->getLeaves();

	// match titles and nations
	for (unsigned int i = 0; i < rules.size(); ++i)
	{
		vector<Object*> rule = rules[i]->getLeaves();
		string			rCK2Title;
		vector<string>	rEU3Tags;
		for (unsigned int j = 0; j < rule.size(); j++)
		{
			if (rule[j]->getKey().compare("CK2") == 0)
			{		 
				rCK2Title = rule[j]->getLeaf();
			}
			else if (rule[j]->getKey().compare("EU3") == 0)
			{
				rEU3Tags.push_back(rule[j]->getLeaf());
			}
			else
			{
				log("Warning: unknown data while mapping countries.\n");
			}
		}

		//find CK2 title from the rule
		map<string, CK2Title*>::iterator CK2TitlesPos = CK2Titles.find(rCK2Title);
		if (CK2TitlesPos == CK2Titles.end())
		{
			continue;
		}
		if (rCK2Title == Configuration::getHRETitle())
		{
			CK2Titles.erase(CK2TitlesPos);
			continue;
		}

		//find EU3 tag from the rule
		set<string>::iterator EU3CountryPos;
		unsigned int distance = 0;
		for (vector<string>::reverse_iterator j = rEU3Tags.rbegin(); j != rEU3Tags.rend(); ++j)
		{
			++distance;
			EU3CountryPos = EU3tags.find(*j);
			if (EU3CountryPos != EU3tags.end())
			{
				break;
			}
		}
		if (EU3CountryPos == EU3tags.end())
		{
			continue;
		}

		//map the countries
		EU3Country* convertedCountry	= CK2TitlesPos->second->getDstCountry();
		EU3Country* historicalCountry	= countries[*EU3CountryPos];
		historicalCountry->replaceWith(convertedCountry, provinceMap);
		log("\tMapped countries %s -> %s (#%d)\n", CK2TitlesPos->first.c_str(), EU3CountryPos->c_str() , distance);

		//remove tags from the lists
		CK2Titles.erase(CK2TitlesPos);
		EU3tags.erase(EU3CountryPos);
	}

	for (unsigned int j = 0; j < blockedNations.size(); ++j)
	{
		set<string>::iterator i = EU3tags.find(blockedNations[j]);
		if (i != EU3tags.end())
		{
			EU3tags.erase(i);
			continue;
		}
	}

	while ( (CK2Titles.size() > 0) && (EU3tags.size() > 0) )
	{
		map<string, CK2Title*>::iterator CK2TitlesPos = CK2Titles.begin();
		if (CK2TitlesPos->first == "e_rebels")
		{
			//mapping.insert(make_pair<string, string>(*CK2TitlesPos, "REB")); // TODO: map rebels nation
			CK2Titles.erase(CK2TitlesPos);
		}
		else if (CK2TitlesPos->first == "e_pirates")
		{
			//mapping.insert(make_pair<string, string>(*CK2TitlesPos, "PIR")); // TODO: map pirates nation
			CK2Titles.erase(CK2TitlesPos);
		}
		else if (CK2TitlesPos->first == Configuration::getHRETitle())
		{
			CK2Titles.erase(CK2TitlesPos);
		}
		else
		{
			set<string>::iterator EU3CountryPos = EU3tags.begin();
			EU3Country* convertedCountry	= CK2TitlesPos->second->getDstCountry();
			EU3Country* historicalCountry	= countries[*EU3CountryPos];
			historicalCountry->replaceWith(convertedCountry, provinceMap);
			log("\tMapped countries %s -> %s (fallback)\n", CK2TitlesPos->first.c_str(), EU3CountryPos->c_str());

			//remove tags from the lists
			CK2Titles.erase(CK2TitlesPos);
			EU3tags.erase(EU3CountryPos);
		}
	}

	// now that tags are known, can finish setting the map spread strings
	vector<string> converted;
	vector<string> nomadTech;
	vector<string> westernTech;
	vector<string> easternTech;
	vector<string> ottomanTech;
	vector<string> muslimTech;
	vector<string> indianTech;
	vector<string> chineseTech;
	vector<string> subSaharanTech;
	vector<string> newWorldTech;
	for (map<string, EU3Country*>::iterator i = countries.begin(); i != countries.end(); i++)
	{
		string techGroup = i->second->getTechGroup();
		if (i->second->getSrcCountry() != NULL)
		{
			converted.push_back( i->first );
		}
		else if (techGroup == "nomad_group")
		{
			nomadTech.push_back( i->first );
		}
		else if (techGroup == "western")
		{
			westernTech.push_back( i->first );
		}
		else if (techGroup == "eastern")
		{
			easternTech.push_back( i->first );
		}
		else if (techGroup == "ottoman")
		{
			ottomanTech.push_back( i->first );
		}
		else if (techGroup == "muslim")
		{
			muslimTech.push_back( i->first );
		}
		else if (techGroup == "indian")
		{
			indianTech.push_back( i->first );
		}
		else if (techGroup == "chinese")
		{
			chineseTech.push_back( i->first );
		}
		else if (techGroup == "sub_saharan")
		{
			subSaharanTech.push_back( i->first );
		}
		else if (techGroup == "new_world")
		{
			newWorldTech.push_back( i->first );
		}

		vector<string> selfString;
		selfString.push_back( i->first );
		mapSpreadStrings.insert(  make_pair( i->first, selfString )  );
	}

	mapSpreadStrings.insert( make_pair("converted", converted) );
	mapSpreadStrings.insert( make_pair("nomad_group", nomadTech) );
	mapSpreadStrings.insert( make_pair("western", westernTech) );
	mapSpreadStrings.insert( make_pair("eastern", easternTech) );
	mapSpreadStrings.insert( make_pair("ottoman", ottomanTech) );
	mapSpreadStrings.insert( make_pair("muslim", muslimTech) );
	mapSpreadStrings.insert( make_pair("indian", indianTech) );
	mapSpreadStrings.insert( make_pair("chinese", chineseTech) );
	mapSpreadStrings.insert( make_pair("sub_saharan", subSaharanTech) );
	mapSpreadStrings.insert( make_pair("new_world", newWorldTech) );

	for (map<int, EU3Province*>::iterator provItr = provinces.begin(); provItr != provinces.end(); provItr++)
	{
		provItr->second->setDiscoverers(mapSpreadStrings);
	}

	return CK2Titles.size();
}