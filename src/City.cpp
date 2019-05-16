#include "City.h"

cml::City::City()
{

}

cml::City::~City()
{

}

void cml::City::parseCityDefinitionFile(std::string path)
{
	std::string data;
	std::fstream file;

	file.open(path.c_str());

	while (!file.eof())
	{
		data.clear();
		std::getline(file,data,'\n');

		//general section
		if (data == "general")
		{
			while (data != "}")
			{
				data.clear();
				std::getline(file,data,'\n');
				int pos;

				if ((pos = data.find("city_seed")) != std::string::npos)
				{
					data = data.substr(pos+9,data.length());
					cdf.citySeed = atoi(data.c_str());
				}

				if ((pos = data.find("initial_cell_boundary")) != std::string::npos)
				{
					data = data.substr(pos+22,data.length());
					cdf.initialCellBoundary = data;
				}

				if ((pos = data.find("initial_road_network")) != std::string::npos)
				{
					data = data.substr(pos+21,data.length());
					cdf.initialRoadNetwork = data;
				}
			}
		}

		if (data == "level")
		{
			std::vector< std::string > levelRuleConditionArray;
			std::vector< std::string > levelRuleActionArray;

			while (data != "}")
			{
				data.clear();
				std::getline(file,data,'\n');
				int pos;

				if ((pos = data.find("bounding_box")) != std::string::npos)
				{
					data = data.substr(pos+13,data.length());
					cdf.bboxVisibility.push_back(atoi(data.c_str()));
				}

				if ((pos = data.find("rand_val_limit")) != std::string::npos)
				{
					data = data.substr(pos+15,data.length());
					cdf.randValLimit.push_back(atoi(data.c_str()));
				}

				if ((pos = data.find("road_parameters")) != std::string::npos)
				{
					data = data.substr(pos+16,data.length());
					cdf.roadParameters.push_back(data);
				}

				if ((pos = data.find("->")) != std::string::npos)
				{
					levelRuleConditionArray.push_back(data.substr(0,pos-1));
					levelRuleActionArray.push_back(data.substr(pos+3,data.length()));					
				}
			}

			cdf.ruleActions.push_back(levelRuleActionArray);
			cdf.ruleConditions.push_back(levelRuleConditionArray);
		}
	}
}

bool cml::City::validCondition(std::string condition,int area, int randVal)
{
	std::stringstream ss;
	std::string temp;
	float value;

	ss<<condition;

	while (ss>>temp)
	{
		if (temp == "AREA")
		{
			ss>>temp;

			if (temp == "<")
			{
				ss>>value;
				if (!(area < value)) return false;
			}

			if (temp == ">")
			{
				ss>>value;
				if (!(area > value)) return false;
			}

			if (temp == "<=")
			{
				ss>>value;
				if (!(area <= value)) return false;
			}

			if (temp == ">=")
			{
				ss>>value;
				if (!(area >= value)) return false;
			}
		}

		if (temp == "RAND")
		{
			ss>>temp;

			if (temp == "<")
			{
				ss>>value;
				if (!(randVal < value)) return false;
			}

			if (temp == ">")
			{
				ss>>value;
				if (!(randVal > value)) return false;
			}

			if (temp == "<=")
			{
				ss>>value;
				if (!(randVal <= value)) return false;
			}

			if (temp == ">=")
			{
				ss>>value;
				if (!(randVal >= value)) return false;
			}
		}
	}

	return true;
}

void cml::City::generate()
{
	cml::CityCell *cell;
	std::vector<cml::CityCell *> cityLevel;
	IGenerator *gen1,*gen2,*gen3,*gen4,*gen5,*gen6,*generator;
	std::stringstream ss;
	std::string temp;
	int pos;
	
	cml::GeneratorManager* generatorManager = cml::GeneratorManager::instance();
/*
	gen1 = generatorManager->getGenerator("ConvexHullGenerator");	
	gen2 = generatorManager->getGenerator("CircularRayGenerator");
	gen3 = generatorManager->getGenerator("GridGenerator");	
	gen4 = generatorManager->getGenerator("QuadDivisionGenerator");
	gen5 = generatorManager->getGenerator("GeometryGenerator");
	gen6 = generatorManager->getGenerator("ParkGenerator");
*/
//	gen1 = new ConvexHullGenerator();
//	gen2 = new CircularRayGenerator();
//	gen3 = new GridGenerator();
//	gen4 = new QuadDivisionGenerator();
//	gen5 = new GeometryGenerator();
//	gen6 = new ParkGenerator();

	srand(cdf.citySeed);
	std::cout<<"Current seed: "<<cdf.citySeed<<std::endl;
	cdf.citySeed++;

//	std::cout<<"_ptr "<<gen1<<" "<<gen2<<"\n";

	//the initial level

	//create a new cell
	cell = new cml::CityCell();
	//choose the generator to use for initial cell boundary
	temp = cdf.initialCellBoundary;
	pos = temp.find(" ");
	generator = generatorManager->getGenerator(temp.substr(0,pos));
	generator->setTarget(cell);
	generator->setParameters(temp.substr(pos+1,temp.length()));
	generator->generate();

	//choose the generator to use for initial cell road network
	temp = cdf.initialRoadNetwork;
	pos = temp.find(" ");
	generator = generatorManager->getGenerator(temp.substr(0,pos));
	generator->setTarget(cell);
	generator->setParameters(temp.substr(pos+1,temp.length()));
	generator->generate();

	//procesarea lineGraph-ului generat
	cell->lineGraphMerge();
	//calcularea de subcelule pe baza MCB
	cell->calculateSubcells();
	//se adauga la nivelul curent celula (doar una aici)
	cityLevel.push_back(cell);
	//adaugat la oras primul nivel
	cityCells.push_back(cityLevel);
	//refolosim cityLevel
	cityLevel.clear();
	
	//generarea de celule pentru nivele
	//nr. de nivele = bboxVisibility.size = roadParameters.size = randValLimit.size
	//pentru fiecare nivel
	for (int i=1;i<cdf.bboxVisibility.size()+1;i++)
	{
		//date generale ale nivelului curent
		bool bboxVisible;
		float roadWidth, roadTexOffset;
		int randValLimit;
		//i-1 instead of i because this loop is used to access
		//the initial level, while the arrays in the cdf denote
		//only the levels after the initial one
		bboxVisible = cdf.bboxVisibility[i-1];
		randValLimit = cdf.randValLimit[i-1];

		temp = cdf.roadParameters[i-1];
		ss<<temp;
		ss>>roadWidth>>roadTexOffset;
		ss.clear();

		//pentru fiecare celula a nivelului precedent
		for (int j=0;j<cityCells[i-1].size();j++)
		{
			std::vector<cml::Subcell> subcells;
			//returneaza vectorul de subcelule al celulei j a nivelului i-1
			subcells = cityCells[i-1][j]->getSubcellData();
			//pentru fiecare subcelula a celulei j a nivelului i-1
			for (int k=0;k<subcells.size();k++)
			{
				Subcell& s = subcells[k];
				//creeaza o celula noua
				cell = new cml::CityCell();
				cell->setBoundingBoxVisible(bboxVisible);

				for (int j=0;j<s.edges.size();j++)
				{		
					cell->addEdge(s.edges[j]);
				}

				//TODO: de adaugat textura drumului ca parametru
				cell->constructRoadGeometry(roadWidth,roadTexOffset);

				float area;
				cell->calculateAreaAndCentroid(true);
				area = cell->getArea();

				int randVal;
				randVal = rand()%randValLimit;

				//iterate over condition/action set
				//only the rule with the first valid condition
				//will be activated
				int validConditionPos = -1;
				for (int condNr = 0;condNr<cdf.ruleConditions[i-1].size();condNr++)
				{
					if (validCondition(cdf.ruleConditions[i-1][condNr],area,randVal))
					{
						validConditionPos = condNr;
						break;
					}
				}
				
				//what rule to apply based on the first valid condition found
				if (validConditionPos != -1)
				{
					std::string action;
					int useMergeAndSubcells;
					action = cdf.ruleActions[i-1][validConditionPos];

					ss.clear();
					ss<<action;
					ss>>useMergeAndSubcells;

					action = action.substr(2,action.length());

					pos = action.find(" ");					
					generator = generatorManager->getGenerator(action.substr(0,pos));
					generator->setTarget(cell);
					generator->setParameters(action.substr(pos+1,action.length()));
					generator->generate();

					if (useMergeAndSubcells == 1)
					{
						cell->lineGraphMerge();
						cell->calculateSubcells();
					}
				}

				cityLevel.push_back(cell);
			}

		}
		cityCells.push_back(cityLevel);
		cityLevel.clear();		
	}
}

void cml::City::reset()
{
	for (int i=0;i<cityCells.size();i++)
	{
		for (int j=0;j<cityCells[i].size();j++)
		{
			delete cityCells[i][j];
		}
		cityCells[i].clear();
	}
	cityCells.clear();
}

int cml::City::getNumberOfCellsForLevel(int level)
{
	if (level > cityCells.size()-1)
	{
		return -1;
	}
	else
	{
		return cityCells[level].size();
	}
};