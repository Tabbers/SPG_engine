#include "model.h"
#include <fstream>
#include <iostream>

Model::Model()
{

}


Model::~Model()
{

}
bool Model::loadObj(char * filename)
{
	std::ifstream ss(filename);
	if (!ss.is_open())
		return false;

	std::vector <DirectX::XMFLOAT3> Position;
	std::vector <DirectX::XMFLOAT2> TexCoord;
	std::vector <DirectX::XMFLOAT3> Normal;

	char cmd[256] = { 0 };
	while (TRUE)
	{
		ss >> cmd;
		if (ss.fail())
		{
			m_vertexcount = vertices.size();
			m_indexcount = indices.size();
			break;
		}
		else if (0 == strcmp(cmd, "v"))
		{
			float x, y, z;
			ss >> x >> y >> z;
			Position.push_back(DirectX::XMFLOAT3(x, y, z));
		}
		else if (0 == strcmp(cmd, "vt"))
		{
			float u, v, w;
			ss >> u >> v >> w;
			TexCoord.push_back(DirectX::XMFLOAT2(u, v));
		}
		else if (0 == strcmp(cmd, "vn"))
		{
			float x, y, z;
			ss >> x >> y >> z;
			Normal.push_back(DirectX::XMFLOAT3(x, y, z));
		}
		else if (0 == strcmp(cmd, "f"))
		{
			UINT Value; Vertex vertex;
			for (int iFace = 0; iFace < 3; iFace++)
			{
				ss >> Value;
				if(Position.size() > 0)
					vertex.m_position = Position[Value - 1];
				ss.ignore();

				ss >> Value;
				if (TexCoord.size() > 0)
					vertex.m_textcord = TexCoord[Value - 1];
				ss.ignore();

				ss >> Value;
				if(Normal.size() >0)
					vertex.m_normal = Normal[Value - 1];
				ss.ignore();

				ss >> Value;

				indices.push_back(AddVertex(vertex));
			}
		}
	}
	return true;
}
unsigned long Model::AddVertex(Vertex vertex)
{
	for (int a = 0; a < vertices.size(); a++)
	{
		if (vertices[a] == vertex)
			return a;
	}
	vertices.push_back(vertex);
	return vertices.size() - 1;
}
bool Model::LoadModel(char * filename)
{
	using namespace std;
	ifstream fin;
	char input;
	int i;

	// Open the model file.		
	fin.open(filename);

	// If it could not open the file then exit.
	if (!fin.is_open())
	{
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> m_vertexcount;

	// Set the number of indices to be the same as the vertex count.
	m_indexcount = m_vertexcount;

	// Read up to the beginning of the data.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (i = 0; i<m_vertexcount; i++)
	{
		Vertex vert;
		fin >> vert.m_position.x >> vert.m_position.y >> vert.m_position.z;
		fin >> vert.m_textcord.x >> vert.m_textcord.y;
		fin >> vert.m_normal.x >> vert.m_normal.y >> vert.m_normal.z;
		indices.push_back(i);
		vertices.push_back(vert);
	}

	// Close the model file.
	fin.close();

	return true;
}

void Model::ReleaseModel()
{
	vertices.clear();
	indices.clear();
	return;
}


