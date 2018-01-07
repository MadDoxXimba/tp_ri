#include <Windows.h>
#include <string>
#include <iostream>
#include <vector>
#include "mysql.h"
#include "params.h"
#include "index.h"
#include <fstream>
#include<algorithm>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
extern ParamIndex params;


void IndexData()
{
	MYSQL *conn = nullptr;

	//debugMatrix(getLinks("c:\\Users\\Jonathan\\Desktop\\TP_RI\\Données\\data\\links.txt"));
	//std::cout << calculateInputArc(1, getLinks("c:\\Users\\Jonathan\\Desktop\\TP_RI\\Données\\data\\links.txt")) << std::endl;
	//std::cout << calculateOutputArc(1, getLinks("c:\\Users\\Jonathan\\Desktop\\TP_RI\\Données\\data\\links.txt")) << std::endl;
	//listOfInputArc(0, getLinks("c:\\Users\\Jonathan\\Desktop\\TP_RI\\Données\\data\\links.txt"));
	//debugMatrix_f(getPageRank(10, getLinks("c:\\Users\\Jonathan\\Desktop\\TP_RI\\Données\\data\\links.txt")));
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

		//SELECT p.url, p.resume, p.pr FROM page as p LEFT JOIN word_page wp1 on wp1.id_page = p.id_page LEFT join word w1 on w1.id_word = wp1.id_word LEFT join word_page wp2 on wp2.id_page = p.id_page LEFT join word w2 on w2.id_word = wp2.id_word LEFT join word_page wp3 on wp3.id_page = p.id_page LEFT join word w3 on w3.id_word = wp3.id_word WHERE w1.word='page' AND w2.word='group' AND w3.word='models' ORDER BY pr DESC;

		readOneWordinFile("c:\\Users\\Jonathan\\Desktop\\TP_RI\\Données\\data\\");


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

bool checkIfExistWord(std::string word)
{
	//SELECT COUNT(`word`) FROM `word` WHERE `word` = 'test'

	MYSQL *conn = nullptr;
	MYSQL_RES *result;
	MYSQL_ROW row;
	conn = mysql_init(conn);
	if (mysql_real_connect(conn, params.ServerName.c_str(), params.Login.c_str(), params.Password.c_str(), params.SchemeName.c_str(), 0, NULL, 0))
	{
		std::string sql = "SELECT COUNT(`word`) FROM `word` WHERE `word` = '";
		sql += word;
		sql += "'";
		if (!mysql_query(conn, sql.c_str()))
		{
			result = mysql_store_result(conn);
			while ((row = mysql_fetch_row(result)) != NULL)
			{
				if (atoi(row[0]) == 1)
				{
					//std::cout << row[0] << " : testing word" << std::endl;
					mysql_free_result(result);
					mysql_close(conn);
					return 1;
				}
			}
			//std::cout << row[0] << std::endl;
		}
		else std::cerr << mysql_error(conn) << std::endl;
		mysql_close(conn);
	}
	else
	{
		std::cerr << mysql_error(conn) << std::endl;
	}
	return 0;
}

bool checkIfExistWordPage(int word_id, int page_id)
{
	MYSQL *conn = nullptr;
	MYSQL_RES *result;
	MYSQL_ROW row;
	conn = mysql_init(conn);
	if (mysql_real_connect(conn, params.ServerName.c_str(), params.Login.c_str(), params.Password.c_str(), params.SchemeName.c_str(), 0, NULL, 0))
	{
		//SELECT COUNT(`id_word`) FROM `word_page` WHERE `id_word` = 0 AND `id_page` = 1
		std::string sql = "SELECT COUNT(`id_word`) FROM `word_page` WHERE `id_word` = ";
		sql += std::to_string(word_id);
		sql += " AND `id_page` = ";
		sql += std::to_string(page_id);
		if (!mysql_query(conn, sql.c_str()))
		{
			result = mysql_store_result(conn);
			while ((row = mysql_fetch_row(result)) != NULL)
			{
				if (atoi(row[0]) == 1)
				{
					//std::cout << row[0] << " : testing word" << std::endl;
					mysql_free_result(result);
					mysql_close(conn);
					return 1;
				}
			}
			//std::cout << row[0] << std::endl;
		}
		else std::cerr << mysql_error(conn) << std::endl;
		mysql_close(conn);
	}
	else
	{
		std::cerr << mysql_error(conn) << std::endl;
	}
	return 0;
}

void readOneWordinFile(std::string pathBase)
{
	std::string links = pathBase + "\\links.txt";
	std::string filesDirectory = pathBase + "\\files\\";

	std::vector<std::pair<int, float>> pageRanks = getRankings(5, getPageRank(10, getLinks(links)));
	int numberOfFiles = getLinks(links).size();
	int dbID = 1;
	for (int currentFile = 0; currentFile < numberOfFiles; currentFile++)
	{
		std::string file = filesDirectory + std::to_string(currentFile) + ".txt";
		//std::cout << currentFile << std::endl;
		//std::cout << "-------------------------------------------------------------" << std::endl;
		std::string STRING;
		std::ifstream infile;

		std::string currentWord = "";
		std::string summary = "";

		infile.open(file);
		while (!infile.eof())
		{
			getline(infile, STRING);

			for (int i = 0; i < STRING.size(); i++)
			{
				if (i < 49)
				{
					summary += STRING[i];
				}
				if ((STRING[i] >= '0' && STRING[i] <= '9') || (STRING[i] >= 'a' && STRING[i] <= 'z') || (STRING[i] >= 'A' && STRING[i] <= 'Z'))
				{
					currentWord += STRING[i];
				}
				else
				{
					if (!currentWord.empty())
					{
						//std::cout << currentWord << std::endl;

						//insertion dans la base sql

						if (!checkIfExistWord(currentWord))
						{
							insertInTableWord(currentWord, dbID);
						}
						if (!checkIfExistWordPage(dbID, currentFile))
						{
							insertInTableWordPage(dbID, currentFile);
						}
						currentWord = "";
						dbID++;
					}
				}
			}
		}
		infile.close();
		for (std::vector<std::pair<int, float>>::iterator it = pageRanks.begin(); it < pageRanks.end(); it++)
		{
			if (it->first == currentFile)
			{
				float pr = it->second;
				insertInTablePage(currentFile, file, summary, pr);
				summary = "";
			}
		}
		//std::cout << std::endl;
		//std::cout << "-------------------------------------------------------------" << std::endl;
	}


}

void insertInTableWord(std::string word, int index)
{
	MYSQL *conn = nullptr;
	conn = mysql_init(conn);
	if (mysql_real_connect(conn, params.ServerName.c_str(), params.Login.c_str(), params.Password.c_str(), params.SchemeName.c_str(), 0, NULL, 0))
	{
		std::string sql = "INSERT INTO `word` VALUES(";
		sql += std::to_string(index);
		sql += ",'";
		sql += word;
		sql += "')";
		std::cout << sql << std::endl;
		if (!mysql_query(conn, sql.c_str())) std::cout << "Insertion dans la table page" << std::endl;
		else std::cerr << mysql_error(conn) << std::endl;
		mysql_close(conn);
	}
	else
	{
		std::cerr << mysql_error(conn) << std::endl;
	}
}

void insertInTableWordPage(int id_word, int page_id)
{
	MYSQL *conn = nullptr;
	conn = mysql_init(conn);
	if (mysql_real_connect(conn, params.ServerName.c_str(), params.Login.c_str(), params.Password.c_str(), params.SchemeName.c_str(), 0, NULL, 0))
	{
		std::string sql = "INSERT INTO `word_page` VALUES(";
		sql += std::to_string(id_word);
		sql += ",'";
		sql += std::to_string(page_id);
		sql += "')";
		std::cout << sql << std::endl;
		if (!mysql_query(conn, sql.c_str())) std::cout << "Insertion dans la table page_word" << std::endl;
		else std::cerr << mysql_error(conn) << std::endl;
		mysql_close(conn);
	}
	else
	{
		std::cerr << mysql_error(conn) << std::endl;
	}
}

void insertInTablePage(int page_id, std::string url, std::string summary, int pr)
{
	MYSQL *conn = nullptr;
	conn = mysql_init(conn);
	if (mysql_real_connect(conn, params.ServerName.c_str(), params.Login.c_str(), params.Password.c_str(), params.SchemeName.c_str(), 0, NULL, 0))
	{
		//INSERT INTO `page`(`id_page`, `url`, `pr`, `resume`) VALUES (0,'c:\Users\Jonathan\Desktop\TP_RI\DonnÚes\data\\files\0',1.00, 'Networks & New Technologies Research group')
		std::string sql = "INSERT INTO `page`(`id_page`, `url`, `pr`, `resume`) VALUES (";
		sql += std::to_string(page_id);
		sql += ",'";
		sql += url;
		sql += "',";
		sql += std::to_string(pr);
		sql += ",'";
		sql += summary;
		sql += "')";
		std::cout << sql << std::endl;
		if (!mysql_query(conn, sql.c_str())) std::cout << "Insertion dans la table page_word" << std::endl;
		else std::cerr << mysql_error(conn) << std::endl;
		mysql_close(conn);
	}
	else
	{
		std::cerr << mysql_error(conn) << std::endl;
	}
}