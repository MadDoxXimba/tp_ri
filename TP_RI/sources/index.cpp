#include <string>
#include <iostream>
#include <vector>
#include "mysql.h"
#include "params.h"
#include "index.h"
#include <fstream>
#include<algorithm>
extern ParamIndex params;

void IndexData()
{
	MYSQL *conn = nullptr;

	//debugMatrix(getLinks("c:\\Users\\Jonathan\\Desktop\\TP_RI\\Données\\data\\links.txt"));
	//std::cout << calculateInputArc(1, getLinks("c:\\Users\\Jonathan\\Desktop\\TP_RI\\Données\\data\\links.txt")) << std::endl;
	//std::cout << calculateOutputArc(1, getLinks("c:\\Users\\Jonathan\\Desktop\\TP_RI\\Données\\data\\links.txt")) << std::endl;
	//listOfInputArc(0, getLinks("c:\\Users\\Jonathan\\Desktop\\TP_RI\\Données\\data\\links.txt"));
	//debugVector_pair(getRankings(5, getPageRank(10, getLinks("c:\\Users\\Jonathan\\Desktop\\TP_RI\\Données\\data\\links.txt"))));
	conn = mysql_init(conn);
	
	if (mysql_real_connect(conn, params.ServerName.c_str(), params.Login.c_str(), params.Password.c_str(), params.SchemeName.c_str(), 0, NULL, 0))
	{
		std::vector<std::string> tables = { "word_page","page","word" };

		for (auto &table : tables)
		{
			std::string sql = "TRUNCATE TABLE `";
			sql += table;
			sql += "`";
			if (!mysql_query(conn, sql.c_str())) std::cout << "Empty table '" << table << "'" << std::endl;
			else std::cerr << mysql_error(conn) << std::endl;
		}

		//TODO: à vous de jouer ...

		mysql_close(conn);
	}
	else std::cerr << mysql_error(conn) << std::endl;
}

std::vector<std::vector<int>> getLinks(std::string path)
{
	int size;
	std::ifstream in(path);
	if (!in) {
		std::cout << "Cannot open input file.\n";
	}
	std::vector<std::vector<int>> result = std::vector<std::vector<int>>();
	char str[255];
	bool firstLine = true;
	int currentLine1 = 0;
	int currentLine2 = 0;
	while (in) {
		in.getline(str, 255);
		if (in)
		{
			if (firstLine == true)
			{
				firstLine = false;
				size = atoi(str);
				result.resize(size);
				for (int i = 0; i < size; i++)
				{
					result[i].resize(size);
				}
				//initialitize matrix link with 0
				for (int j = 0; j < size; j++)
				{
					for (int i = 0; i < size; i++)
					{
						result[currentLine1].push_back(0);
					}
					currentLine1++;
				}
			}
			else
			{
				//increment a case by 1 if needed
				for (int i = 0; i < size * 2; i++)
				{
					if (str[i] == '1')
					{
						result[currentLine2][i / 2] += 1;
					}
				}
				currentLine2++;
			}
		}
	}
	in.close();
	return result;
}

std::vector<std::vector<float>> getPageRank(int numOfIterations, std::vector<std::vector<int>> link)
{
	std::vector<std::vector<float>> result(link.size(), std::vector<float>(numOfIterations));
	//initialization memory
	//result.resize(size);
	float initialPageRank = float(1) / float(link.size());
	//for (int i = 0; i < numOfIterations; i++) result[i].resize(numOfIterations);
	//initalize pagerank values for each documents
	//std::cout << result.size() << " " << result[0].size() << std::endl;
	for (int i = 0; i < link.size(); i++)
	{
		for (int j = 0; j < numOfIterations; j++)
		{
			result[i][j] = float(0);

			if (j == 0) result[i][j] += initialPageRank;
		}
		
	}
	if (numOfIterations < 1)
	{
		return result;
	}
	else
	{
		int numOfIteration = numOfIterations;
		while (numOfIteration > 1)
		{
			for (int rowIndex = 0; rowIndex < link.size(); rowIndex++)
			{
				for (int currentIteration = 1; currentIteration < numOfIterations; currentIteration++)
				{
					std::vector<int> listOfInputArcs(listOfInputArc(rowIndex, link));
					int numOfOutgoingArcs = calculateOutputArc(rowIndex, link);
					//for each outgoing arcs of this node
					for (int currOutgoinArc = 0; currOutgoinArc < listOfInputArcs.size() - 1; currOutgoinArc++)
					{
						result[rowIndex][currentIteration] += result[listOfInputArcs[currOutgoinArc]][currentIteration - 1];
					}
					
				}
			}
			numOfIteration--;
		}
		return result;
	}
}

bool compare(const std::pair<int, float>& i, const std::pair<int, float>& j)
{
	//change > to < for descending or ascending order
	return i.second > j.second;
}

std::vector<std::pair<int, float>> getRankings(int numOfIteration, std::vector<std::vector<float>>& pageRankMatrix)
{
	std::pair<int, float> result_pair;
	std::vector<std::pair<int, float>> result;
	for (int rowIndex = 0; rowIndex < pageRankMatrix.size(); rowIndex++)
	{
		result_pair.first = rowIndex;
		result_pair.second = pageRankMatrix[rowIndex][numOfIteration - 1];
		result.push_back(result_pair);
	}
	sort(result.begin(), result.end(), compare);
	return result;
}

int calculateOutputArc(int colIndex, std::vector<std::vector<int>> link)
{
	int result = 0;

	for (int rowIndex = 0; rowIndex < link.size(); rowIndex++)
	{
		if (link[rowIndex][colIndex] == 1)
		{
			result++;
		}
	}

	return result;
}

int calculateInputArc(int rowIndex, std::vector<std::vector<int>> link)
{
	int result = 0;

	for (int colIndex = 0; colIndex < link.size(); colIndex++)
	{
		if (link[rowIndex][colIndex] == 1)
		{
			result++;
		}
	}

	return result;
}

std::vector<int> listOfInputArc(int rowIndex, std::vector<std::vector<int>> link)
{
	std::vector<int> result;

	for (int colIndex = 0; colIndex < link.size(); colIndex++)
	{
		if (link[rowIndex][colIndex] == 1)
		{
			result.push_back(colIndex);
		}
	}
	return result;
}

void debugMatrix(std::vector<std::vector<int>> toPrint)
{
	for (int i = 0; i < toPrint.size(); i++)
	{
		for (int j = 0; j < toPrint[i].size(); j++)
		{
			std::cout << toPrint[i][j] << "";
		}
		std::cout << std::endl;
	}
}

void debugMatrix_f(std::vector<std::vector<float>> toPrint)
{
	for (int i = 0; i < toPrint.size(); i++)
	{
		for (int j = 0; j < toPrint[i].size(); j++)
		{
			std::cout << toPrint[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

void debugVector_pair(std::vector<std::pair<int, float>> toPrint) 
{
	for (std::vector<std::pair<int, float>>::iterator it = toPrint.begin(); it < toPrint.end(); it++)
	{
		std::cout << it->first << " " << it->second << " " << std::endl;
	}
}
