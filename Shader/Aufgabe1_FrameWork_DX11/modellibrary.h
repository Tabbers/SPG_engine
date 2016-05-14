#pragma once
#include "globaldefinitions.h"
#include "model.h"

class ModelLibrary
{
public:
	ModelLibrary();
	~ModelLibrary();
	
	int CheckLibrary(char* path);
	bool LoadModel(char* path, int &id);

	Model* GetModel(int ID);

private:
	bool LoadModel(char* path);
	std::vector<Model> Models;
};

