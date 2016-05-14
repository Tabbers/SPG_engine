#ifndef _MODEL_H_
#define _MODEL_H_

#include <DirectXMath.h>
#include "vertex.h"
#include <string>
#include <vector>

class Model
{
struct IDPathTupel
{
	int ID = 0;
	std::string path = " ";
};

public:
	Model();
	~Model();

	bool loadObj(char * filename);

	unsigned long AddVertex(Vertex vertex);

	bool LoadModel(char*);
	void ReleaseModel();

public:
	std::vector<Vertex> vertices;

	std::vector<unsigned long> indices;
	int m_vertexcount;
	int m_indexcount;

	IDPathTupel uid;


};
#endif

