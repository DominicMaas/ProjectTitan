#include "Chunk.h"

Chunk::Chunk(glm::vec3 position, World* world)
{
    // Set chunk details
	_position = position;
	_world = world;

    // Convert the chunk position into physics coords
    reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion::identity();
    reactphysics3d::Transform transform(reactphysics3d::Vector3(_position.x, _position.y, _position.z), orientation);

    // Create a collision body for this world
    _collisionBody = _world->getPhysicsWorld()->createCollisionBody(transform);
}

Chunk::~Chunk()
{
	// Delete the blocks
	for (int i = 0; i < CHUNK_WIDTH; ++i)
	{
		for (int j = 0; j < CHUNK_HEIGHT; ++j)
		{
			delete[] _blocks[i][j];
		}

		delete[] _blocks[i];
	}
	delete[] _blocks;

	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vbo);
}

void Chunk::load()
{
	// Setup opengl buffers
	glGenBuffers(1, &_vbo);
	glGenVertexArrays(1, &_vao);

	// Create the blocks
	_blocks = new Block * *[CHUNK_WIDTH];
	for (int i = 0; i < CHUNK_WIDTH; i++)
	{
		_blocks[i] = new Block * [CHUNK_HEIGHT];

		for (int j = 0; j < CHUNK_HEIGHT; j++)
		{
			_blocks[i][j] = new Block[CHUNK_WIDTH];
		}
	}

	// Build height map
	for (int x = 0; x < CHUNK_WIDTH; x++)
		for (int y = 0; y < CHUNK_HEIGHT; y++)	
			for (int z = 0; z < CHUNK_WIDTH; z++)
			{
				_blocks[x][y][z].setMaterial(_world->getWorldGen()->getTheoreticalBlockType(_position.x + x, _position.y + y, _position.z + z));
			}

	_loaded = true;
}

void Chunk::render()
{
	// Quick check to make sure this chunk is loaded
	if (!_loaded)
		return;

	// Set the position of this chunk in the shader
	_world->getWorldShader()->setMat4("model", _modelMatrix);

	// Render
	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLES, 0, _vertices);
	glBindVertexArray(0);
}

bool Chunk::isTransparent(int x, int y, int z)
{
	if (y < 0) return false;

	if (getBlockType(x, y, z) == BlockManager::BLOCK_AIR)
		return true;

	return false;
}

unsigned int Chunk::getBlockType(int x, int y, int z)
{
	if ((y >= CHUNK_HEIGHT))
		return BlockManager::BLOCK_AIR;

	if ((x < 0) || (z < 0) || (x >= CHUNK_WIDTH) || (z >= CHUNK_WIDTH))
	{
		// Calculate world coordinates
		glm::vec3 worldPos(x, y, z);
		worldPos += _position;

		// Find the chunk that contains these coordinates
		Chunk* c = _world->findChunk(worldPos);

		// This is "air", render the side of the face
		if (c == NULL || !c->isLoaded())
			return _world->getWorldGen()->getTheoreticalBlockType(worldPos.x, worldPos.y, worldPos.z);

		// Calculate local space coordinates
		glm::vec3 cLocal = worldPos -= c->getPosition();
		return c->getBlockType(cLocal.x, cLocal.y, cLocal.z);
	}

	return _blocks[x][y][z].getMaterial();
}

void Chunk::rebuild()
{
	std::vector<ChunkVertex> vertices;
	std::vector<int> indices;
	int triangleCount = 0;

	for (int x = 0; x < CHUNK_WIDTH; x++) {
		for (int y = 0; y < CHUNK_HEIGHT; y++) {
			for (int z = 0; z < CHUNK_WIDTH; z++) {
				// Get the id at this position
				Block b = _blocks[x][y][z];

				// Don't render Air
				if (b.getMaterial() == BlockManager::BLOCK_AIR)
					continue;

				// Check all edges of the block
				int index = 0;

				if (isTransparent(x,     y,     z)) index |= 1;
				if (isTransparent(x + 1, y,     z)) index |= 2;
				if (isTransparent(x + 1, y + 1, z)) index |= 4;
				if (isTransparent(x,     y + 1, z)) index |= 8;
				if (isTransparent(x,     y    , z + 1)) index |= 16;
				if (isTransparent(x + 1, y,     z + 1)) index |= 32;		
				if (isTransparent(x + 1, y + 1, z + 1)) index |= 64;
				if (isTransparent(x,     y + 1, z + 1)) index |= 128;

				// Get block data
				glm::vec3 color = BlockManager::getColorFromId(b.getMaterial());

				// Front
				if (isTransparent(x, y, z - 1))
				{
					vertices.push_back(ChunkVertex(1 + x, 1 + y, 0 + z, 0, 0, -1, color));
					vertices.push_back(ChunkVertex(1 + x, 0 + y, 0 + z, 0, 0, -1, color));
					vertices.push_back(ChunkVertex(0 + x, 0 + y, 0 + z, 0, 0, -1, color));
                    triangleCount++;

					vertices.push_back(ChunkVertex(0 + x, 0 + y, 0 + z, 0, 0, -1, color));
					vertices.push_back(ChunkVertex(0 + x, 1 + y, 0 + z, 0, 0, -1, color));
					vertices.push_back(ChunkVertex(1 + x, 1 + y, 0 + z, 0, 0, -1, color));
                    triangleCount++;
				}

				// Back
				if (isTransparent(x, y, z + 1))
				{
					vertices.push_back(ChunkVertex(0 + x, 0 + y, 1 + z, 0, 0, 1, color));
					vertices.push_back(ChunkVertex(1 + x, 0 + y, 1 + z, 0, 0, 1, color));
					vertices.push_back(ChunkVertex(1 + x, 1 + y, 1 + z, 0, 0, 1, color));
                    triangleCount++;

					vertices.push_back(ChunkVertex(1 + x, 1 + y, 1 + z, 0, 0, 1, color));
					vertices.push_back(ChunkVertex(0 + x, 1 + y, 1 + z, 0, 0, 1, color));
					vertices.push_back(ChunkVertex(0 + x, 0 + y, 1 + z, 0, 0, 1, color));
                    triangleCount++;
				}

				// Right
				if (isTransparent(x - 1, y, z))
				{
					vertices.push_back(ChunkVertex(0 + x, 1 + y, 1 + z, -1, 0, 0, color));
					vertices.push_back(ChunkVertex(0 + x, 1 + y, 0 + z, -1, 0, 0, color));
					vertices.push_back(ChunkVertex(0 + x, 0 + y, 0 + z, -1, 0, 0, color));
                    triangleCount++;

					vertices.push_back(ChunkVertex(0 + x, 0 + y, 0 + z, -1, 0, 0, color));
					vertices.push_back(ChunkVertex(0 + x, 0 + y, 1 + z, -1, 0, 0, color));
					vertices.push_back(ChunkVertex(0 + x, 1 + y, 1 + z, -1, 0, 0, color));
                    triangleCount++;
				}

				// Left
				if (isTransparent(x + 1, y, z))
				{
					vertices.push_back(ChunkVertex(1 + x, 0 + y, 0 + z, 1, 0, 0, color));
					vertices.push_back(ChunkVertex(1 + x, 1 + y, 0 + z, 1, 0, 0, color));
					vertices.push_back(ChunkVertex(1 + x, 1 + y, 1 + z, 1, 0, 0, color));
                    triangleCount++;

					vertices.push_back(ChunkVertex(1 + x, 1 + y, 1 + z, 1, 0, 0, color));
					vertices.push_back(ChunkVertex(1 + x, 0 + y, 1 + z, 1, 0, 0, color));
					vertices.push_back(ChunkVertex(1 + x, 0 + y, 0 + z, 1, 0, 0, color));
                    triangleCount++;
				}

				// Down
				if (isTransparent(x, y - 1, z))
				{
					vertices.push_back(ChunkVertex(0 + x, 0 + y, 0 + z, 0, -1, 0, color));
					vertices.push_back(ChunkVertex(1 + x, 0 + y, 0 + z, 0, -1, 0, color));
					vertices.push_back(ChunkVertex(1 + x, 0 + y, 1 + z, 0, -1, 0, color));
                    triangleCount++;

					vertices.push_back(ChunkVertex(1 + x, 0 + y, 1 + z, 0, -1, 0, color));
					vertices.push_back(ChunkVertex(0 + x, 0 + y, 1 + z, 0, -1, 0, color));
					vertices.push_back(ChunkVertex(0 + x, 0 + y, 0 + z, 0, -1, 0, color));
                    triangleCount++;
				}

				if (isTransparent(x, y + 1, z))
				{
					vertices.push_back(ChunkVertex(1 + x, 1 + y, 1 + z, 0, 1, 0, color));
					vertices.push_back(ChunkVertex(1 + x, 1 + y, 0 + z, 0, 1, 0, color));
					vertices.push_back(ChunkVertex(0 + x, 1 + y, 0 + z, 0, 1, 0, color));
                    triangleCount++;

					vertices.push_back(ChunkVertex(0 + x, 1 + y, 0 + z, 0, 1, 0, color));
					vertices.push_back(ChunkVertex(0 + x, 1 + y, 1 + z, 0, 1, 0, color));
					vertices.push_back(ChunkVertex(1 + x, 1 + y, 1 + z, 0, 1, 0, color));
                    triangleCount++;
				}
			}
		}
	}

	// Set the number of vertices
	_vertices = vertices.size();

	// Bind Vertex Array Object
	glBindVertexArray(_vao);

	// Copy our vertices's array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ChunkVertex) * _vertices, vertices.data(), GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)offsetof(ChunkVertex, Position));
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)offsetof(ChunkVertex, Normal));
	glEnableVertexAttribArray(1);

	// color attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)offsetof(ChunkVertex, Color));
	glEnableVertexAttribArray(2);

	// Setup world position
	_modelMatrix = glm::translate(glm::mat4(1.0f), _position);

    /*if (_collider != nullptr) {

        float pVertices[3 * _vertices];
        int pIndices[3 * triangleCount];

        int pVerticesCount = 0;
        for (auto const& vertex : vertices) {
            pVertices[pVerticesCount] = vertex.Position.x;
            pVerticesCount++;
            pVertices[pVerticesCount] = vertex.Position.y;
            pVerticesCount++;
            pVertices[pVerticesCount] = vertex.Position.z;
            pVerticesCount++;
        }

        int pIndicesCount = 0;
        for (int i = 0; i <= _vertices; i++) {
            pIndices[pIndicesCount] = i;
            pIndicesCount++;
        }

        // Create the polygon vertex array
        auto* triangleArray = new reactphysics3d::TriangleVertexArray(
                pVerticesCount, pVertices, 3 * sizeof(float), triangleCount,
                pIndices, 3 * sizeof(int),
                reactphysics3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
                reactphysics3d::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE);

        reactphysics3d::TriangleMesh* triangleMesh = _world->getPhysicsCommon()->createTriangleMesh();
        triangleMesh->addSubpart(triangleArray);

        reactphysics3d::ConcaveMeshShape* concaveMesh = _world->getPhysicsCommon()->createConcaveMeshShape(triangleMesh);
        _collider = _collisionBody->addCollider(concaveMesh, reactphysics3d::Transform::identity());
    }*/

    // The chunk has been rebuilt
	_changed = false;
}