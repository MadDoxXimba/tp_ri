#pragma once
#include <vector>
#include <iostream>
#include <utility>
void IndexData();

std::vector<std::vector<int>> getLinks(std::string path);

/*
on admet que le nombre d'iteration est toujours positif ou nul
num of col = num of doc voir fichier links.txt (39)
num of ligne = num of iteration
*/
std::vector<std::vector<float>> getPageRank(int numOfIterations, std::vector<std::vector<int>> link);

bool compare(const std::pair<int, float>& i, const std::pair<int, float>& j);

/*
return ranking of a specific iteration of the pagerankmatrix
numOfIteration must be greater than the number of column in the pagerankmatrix
*/
std::vector<std::pair<int, float>> getRankings(int numOfIteration, std::vector<std::vector<float>>& pageRankMatrix);

/*one doc*/
int calculateOutputArc(int colIndex, std::vector<std::vector<int>> link);

/*one doc*/
int calculateInputArc(int rowIndex, std::vector<std::vector<int>> link);

std::vector<int> listOfInputArc(int rowIndex, std::vector<std::vector<int>> link);

void debugMatrix(std::vector<std::vector<int>> toPrint);

void debugMatrix_f(std::vector<std::vector<float>> toPrint);

void debugVector_pair(std::vector<std::pair<int, float>> toPrint);

void readOneWordinFile(std::string pathBase);

void insertInTableWord(std::string word, int index);

void insertInTableWordPage(int id_word, int page_id);

void insertInTablePage(int page_id, std::string url, std::string summary, float pr);

/*Returns 1 if exist and 0 if not exist*/
bool checkIfExistWord(std::string word);

bool checkIfExistWordPage(int word_id, int page_id);