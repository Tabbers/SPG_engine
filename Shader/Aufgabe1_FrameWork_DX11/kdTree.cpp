#include "kdtree.h"
#include <string>
#include <algorithm>

using namespace DirectX;
KdTree::KdTree() 
{
	coll = new Collision();
}

KdTree::~KdTree() 
{
	if (this->root != 0) delete this->root;
}

KdNode::~KdNode() 
{
	if (this->children[0] != nullptr) delete this->children[0];
	if (this->children[1] != nullptr) delete this->children[1];
	if (this->triangles != nullptr) delete this->triangles;
}
void KdTree::Init(std::vector<Triangle*>* triangles, ID3D11Device* device, XMFLOAT3 color)
{
	this->RenderingTriangles = triangles;
	root = createTree(*RenderingTriangles, 16,32);
	InitBuffers(device,color);
}
void KdTree::Render(ID3D11DeviceContext *devCon)
{
	RenderBuffers(devCon);
	return;
}
int irand(int a, int b)
{
	double r = b - a + 1;
	return a + (int)(r * rand() / (RAND_MAX + 1.0));
}

inline void vector_int_swap(std::vector<Triangle*>& numbers, int a, int b) {
	Triangle* tmp = numbers[a];
	numbers[a] = numbers[b];
	numbers[b] = tmp;
}

int explicit_partition(std::vector<Triangle*>& numbers, int first, int last, int pivot, int splitdirection) {
	Triangle* pivotValue = numbers[pivot];
	vector_int_swap(numbers, pivot, last);
	int storeIndex = first;
	for (int i = first; i < last; ++i) {
		if (numbers[i]->center[splitdirection] < pivotValue->center[splitdirection]) {
			vector_int_swap(numbers, storeIndex, i);
			++storeIndex;
		}
	}
	vector_int_swap(numbers, last, storeIndex);
	return storeIndex;
}

int randomized_partition(std::vector<Triangle*>& numbers, int first, int last, int splitdirection) {
	// select random element (pivot), throw all smaller left and all bigger right, return index of pivot
	int pivot = irand(first, last);
	//int pivot = rand() % (last - first) + first;
	return explicit_partition(numbers, first, last, pivot, splitdirection);
}

int randomized_median_select(std::vector<Triangle*>& numbers, int first, int last, int xthSmallest, int splitdirection) {
	if (first == last) return first;
	int pivot = randomized_partition(numbers, first, last, splitdirection);
	int count = (pivot - first) + 1;
	if (xthSmallest == count) return pivot;
	else if (xthSmallest < count) return randomized_median_select(numbers, first, pivot - 1, xthSmallest, splitdirection);
	else return randomized_median_select(numbers, pivot + 1, last, xthSmallest - count, splitdirection);
}

int get_median_via_median_select(std::vector<Triangle*>& numbers, int splitdirection, int start, int end) {
	return randomized_median_select(numbers, start, end, (end - start + 1) / 2, splitdirection);
}


bool SortTrianglesByXPredicate(const Triangle* t1, const Triangle* t2)
{
	return t1->center[0] < t2->center[0];
}
bool SortTrianglesByYPredicate(const Triangle* t1, const Triangle* t2)
{
	return t1->center[1] < t2->center[1];
}
bool SortTrianglesByZPredicate(const Triangle* t1, const Triangle* t2)
{
	return t1->center[2] < t2->center[2];
}

// startpoint = origin of ray
// direction = normalized direction of ray
// tmax = length of ray (ideally == far plane)
void KdTree::rayCast(KdNode* node, Ray ray, float &length, float tmax, float tdiff_to_original)
{
	if (node == 0) return;

	if (node->dimension == -1) { // leaf node
								 // ray intersection here
		for (int i = 0; i < node->triangles->size(); ++i) {
			if (!(*node->triangles)[i]->intersection) {
				float intersectDistance = 0;
				bool intersect = coll->RayTriangleIntersection(ray, *(*node->triangles)[i], intersectDistance);
				if (intersect) 
				{
					(*node->triangles)[i]->intersection = true;
					length = intersectDistance + tdiff_to_original;
					(*node->triangles)[i]->distance = length;
					(*node->triangles)[i]->PointIntersect = ray.m_Origin + ray.m_Direction * intersectDistance;					
				}
			}
		}
	}
	else 
	{
		int dim = node->dimension;
		int first = ray.m_Origin.m128_f32[dim] > node->plane;

		if (ray.m_Direction.m128_f32[dim] == 0.0f) {
			// line segment parallel to splitting plane, visit near side only
			rayCast(node->children[first],ray,length, tmax, tdiff_to_original);
		}
		else {
			//find t value for intersection
			float t = (node->plane - ray.m_Origin.m128_f32[dim]) / ray.m_Direction.m128_f32[dim];

			if (0.0f <= t && t < tmax) 
			{
				rayCast(node->children[first], ray, length, t, tdiff_to_original);
				Ray subray(ray.m_Origin + t * ray.m_Direction,ray.m_Direction);
				rayCast(node->children[first ^ 1], subray, length, tmax - t, tdiff_to_original + t);
			}
			else 
			{
				rayCast(node->children[first], ray, length,t, tdiff_to_original);
			}
		}
	}
}

KdNode* KdTree::createTree(std::vector<Triangle*>& triangles, int depth, int numberofTriangles) {
	if (triangles.empty()) return 0;

	if (depth == 0 || triangles.size() <= 5) {
		KdNode* node = new KdNode();
		node->dimension = -1;

		node->triangles = new std::vector<Triangle*>(0);
		node->triangles->reserve(triangles.size());
		for (int i = 0; i < triangles.size(); ++i) {
			node->triangles->push_back(triangles[i]);
		}

		node->children[0] = 0;
		node->children[1] = 0;
		node->plane = 0;
		return node;
	}

	int splitdir;

	// --- find biggest expansion ---
	float smallest[3] = { FLT_MAX, FLT_MAX, FLT_MAX };
	float biggest[3] = { FLT_MIN, FLT_MIN, FLT_MIN };

	for (int i = 0; i < triangles.size(); ++i) {
		for (int dim = 0; dim < 3; ++dim) {
			if (triangles[i]->lowerCorner[dim] < smallest[dim]) {
				smallest[dim] = triangles[i]->lowerCorner[dim];
				//smallestidx[dim] = i;
			}
			if (triangles[i]->upperCorner[dim] > biggest[dim]) {
				biggest[dim] = triangles[i]->upperCorner[dim];
				//biggestidx[dim] = i;
			}
		}
	}

	float expansion[3];
	for (int dim = 0; dim < 3; ++dim) {
		expansion[dim] = biggest[dim] - smallest[dim];
	}
	if (expansion[0] > expansion[1]) {
		if (expansion[0] > expansion[2]) {
			splitdir = 0;
		}
		else { // 0 <= 2
			splitdir = 2;
		}
	}
	else { // 0 <= 1
		if (expansion[1] > expansion[2]) {
			splitdir = 1;
		}
		else { // 1 <= 2
			splitdir = 2;
		}
	}
	// --- find biggest expansion end ---

	// Generate Verticies
	//front 
	m_vertices.push_back({ smallest[0],biggest[1], smallest[2] }); //left down
	m_vertices.push_back({ biggest[0], biggest[1], smallest[2] });	//right down
	m_vertices.push_back({ biggest[0], smallest[1], smallest[2] });	//right up
	m_vertices.push_back({ smallest[0], smallest[1], smallest[2] });  //left Up
																	  //back
	m_vertices.push_back({ smallest[0], biggest[1], biggest[2] });  // left down
	m_vertices.push_back({ biggest[0], biggest[1], biggest[2] });	// right down
	m_vertices.push_back({ biggest[0], smallest[1], biggest[2] });	// right up
	m_vertices.push_back({ smallest[0], smallest[1], biggest[2] });	// left up

	int medianindex = get_median_via_median_select(triangles, splitdir, 0, triangles.size() - 1);

	KdNode* node = new KdNode();
	node->dimension = splitdir;
	node->triangles = 0;
	Triangle* mediantri = triangles[medianindex];

	// put split plane on the side fewer units away from the center of the triangle
	float left_to_center = abs(mediantri->center[splitdir] - mediantri->lowerCorner[splitdir]);
	float center_to_right = abs(mediantri->lowerCorner[splitdir] - mediantri->center[splitdir]);
	if (left_to_center > center_to_right) {
		node->plane = mediantri->upperCorner[splitdir] - 0.001f;
	}
	else {
		node->plane = mediantri->lowerCorner[splitdir] + 0.001f;
	}

	std::vector<Triangle*> left(0);
	left.reserve(triangles.size());
	std::vector<Triangle*> right(0);
	right.reserve(triangles.size());

	// insert all left of split plane to the left
	for (int i = 0; i < triangles.size(); ++i) {
		if (triangles[i]->lowerCorner[splitdir] <= node->plane) {
			left.push_back(triangles[i]);
		}
	}
	// insert all right of split plane to the right
	for (int i = 0; i < triangles.size(); ++i) {
		if (triangles[i]->upperCorner[splitdir] > node->plane) { // >= ?
			right.push_back(triangles[i]);
		}
	}

	// ---- create tree recursively on child nodes
	// if one side contains all triangles, we can't split it again using the same algo, as that would give us the same result again
	// so in that case, force the next node to be a leaf node by setting depth to 0
	if (left.size() == triangles.size()) {
		// if the other side ALSO contains all triangles, it wouldn't make sense to create two child nodes that contain the same triangles
		// so in that case, transform the current node into a leaf node and return it
		if (right.size() == triangles.size()) {
			node->dimension = -1;

			node->triangles = new std::vector<Triangle*>(0);
			node->triangles->reserve(triangles.size());
			for (int i = 0; i < triangles.size(); ++i) {
				node->triangles->push_back(triangles[i]);
			}

			node->children[0] = 0;
			node->children[1] = 0;
			node->plane = 0;
			return node;
		}
		else {
			node->children[0] = createTree(left, 0,numberofTriangles);
		}
	}
	else {
		node->children[0] = createTree(left, depth - 1, numberofTriangles);
	}

	// see comment for left, same applies to right
	if (right.size() == triangles.size()) {
		node->children[1] = createTree(right, 0, numberofTriangles);
	}
	else {
		node->children[1] = createTree(right, depth - 1, numberofTriangles);
	}

	return node;
}

KdNode* KdTree::getRoot() {
	return root;
}

bool KdTree::InitBuffers(ID3D11Device* device, XMFLOAT3 color)
{
	HRESULT result;
	VertexType* vertices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	unsigned int boxCount = m_vertices.size() / 8u;
	m_indexCount = boxCount * 24u;
	unsigned long* indices = new unsigned long[m_indexCount];

	for (unsigned int i = 0u; i < boxCount; ++i)
	{
		indices[i * 24u] = static_cast<unsigned long>(i * 8u);
		indices[i * 24u + 1u] = static_cast<unsigned long>(i * 8u + 1u);
		indices[i * 24u + 2u] = static_cast<unsigned long>(i * 8u + 1u);
		indices[i * 24u + 3u] = static_cast<unsigned long>(i * 8u + 2u);
		indices[i * 24u + 4u] = static_cast<unsigned long>(i * 8u + 2u);
		indices[i * 24u + 5u] = static_cast<unsigned long>(i * 8u + 3u);
		indices[i * 24u + 6u] = static_cast<unsigned long>(i * 8u + 3u);
		indices[i * 24u + 7u] = static_cast<unsigned long>(i * 8u);
		indices[i * 24u + 8u] = static_cast<unsigned long>(i * 8u + 4u);
		indices[i * 24u + 9u] = static_cast<unsigned long>(i * 8u + 5u);
		indices[i * 24u + 10u] = static_cast<unsigned long>(i * 8u + 5u);
		indices[i * 24u + 11u] = static_cast<unsigned long>(i * 8u + 6u);
		indices[i * 24u + 12u] = static_cast<unsigned long>(i * 8u + 6u);
		indices[i * 24u + 13u] = static_cast<unsigned long>(i * 8u + 7u);
		indices[i * 24u + 14u] = static_cast<unsigned long>(i * 8u + 7u);
		indices[i * 24u + 15u] = static_cast<unsigned long>(i * 8u + 4u);
		indices[i * 24u + 16u] = static_cast<unsigned long>(i * 8u);
		indices[i * 24u + 17u] = static_cast<unsigned long>(i * 8u + 4u);
		indices[i * 24u + 18u] = static_cast<unsigned long>(i * 8u + 1u);
		indices[i * 24u + 19u] = static_cast<unsigned long>(i * 8u + 5u);
		indices[i * 24u + 20u] = static_cast<unsigned long>(i * 8u + 2u);
		indices[i * 24u + 21u] = static_cast<unsigned long>(i * 8u + 6u);
		indices[i * 24u + 22u] = static_cast<unsigned long>(i * 8u + 3u);
		indices[i * 24u + 23u] = static_cast<unsigned long>(i * 8u + 7u);
	}

	// Array for vertices
	vertices = new VertexType[m_vertices.size()];
	if (!vertices) return false;
	for (int i = 0; i < m_vertices.size(); i++)
	{
		vertices[i].color = color;
		vertices[i].position = m_vertices[i];
	}
	// Setup dec vor vert buffer with size of vertextype*count
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// pointer to vertiues
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// create vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result)) return false;


	// Set up desc for static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// pointer to index Data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result)) return false;

	// Release arrays
	delete[] vertices;
	vertices = 0;
	m_vertices.clear();
	return true;
}

void KdTree::ShutdownBuffers()
{
	// Release the index and vert buffer
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
	return;
}

void KdTree::RenderBuffers(ID3D11DeviceContext *devCon)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	devCon->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	devCon->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	return;
}
