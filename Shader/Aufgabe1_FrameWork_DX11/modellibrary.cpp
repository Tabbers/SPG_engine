#include "modellibrary.h"



ModelLibrary::ModelLibrary()
{
	LoadModel("Data/sht/cube.sht");
}


ModelLibrary::~ModelLibrary()
{	
}

int ModelLibrary::CheckLibrary(char * path)
{
	for each (Model mod in Models)
	{
		if (mod.uid.path == path)
		{
			return mod.uid.ID;
		}
	}
	return -1;
}

bool ModelLibrary::LoadModel(char * path)
{
	bool result = true;

	Model model;
	result = model.LoadModel(path);
	if (!result) return result;

	model.uid.path = path;
	model.uid.ID = Models.size();

	Models.push_back(model);

	return result;
}
bool ModelLibrary::LoadModel(char * path, int &id)
{
	bool result = true;

	Model model;
	const std::string temppath = path;
	auto const pos = temppath.find_last_of('.');
	const auto leaf = temppath.substr(pos + 1);

	if (leaf == "obj")
	{
		result = model.loadObj(path);
		if (!result) return result;
	}
	else
	{
		result = model.LoadModel(path);
		if (!result) return result;
	}

	model.uid.path = path;
	model.uid.ID = Models.size();
	id = model.uid.ID;
	Models.push_back(model);
	
	return result;
}

Model* ModelLibrary::GetModel(int ID)
{
	return &Models[ID];
}
