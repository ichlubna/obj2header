#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

struct Vertex {
	std::vector<float> position;
	std::vector<float> normal;
	std::vector<float> uv;

	bool operator==(const Vertex &a) const
	{
		return a.position == this->position && a.normal == this->normal && a.uv == this->uv;
		//constexpr float EPS = 0.000001;
		//return (fabs(a.position[0] - this->position[0]) < EPS) && (fabs(a.position[1] - this->position[1]) < EPS) && (fabs(a.position[2] - this->position[2]) < EPS); 
	}
};

void convert(std::string input, std::string output)
{
	tinyobj::attrib_t attributes;
    	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warning, error;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	
	if(!tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, input.c_str()))
		throw std::runtime_error("Obj loader:" + warning + "\n" + error);
	
	bool isNormal = attributes.normals.size() > 0;
	bool isUv = attributes.texcoords.size() > 0;

	for(const auto& shape : shapes)
		for(const auto& index : shape.mesh.indices)
		{
			Vertex vertex;
			int offset = 3*index.vertex_index;
			vertex.position = {attributes.vertices[offset], attributes.vertices[offset+1], attributes.vertices[offset+2]};
			if(isNormal)
			{	offset = 3*index.normal_index;
				vertex.normal = {attributes.normals[offset], attributes.normals[offset+1], attributes.normals[offset+2]};
			}
			if(isUv)
			{	
				offset = 2*index.texcoord_index;
				vertex.uv = {attributes.texcoords[offset], attributes.texcoords[offset+1]};
			}
			//TODO use map or set
			unsigned int uniqueIndex = vertices.size();
			bool found = false;
			for(int i=0; i<vertices.size(); i++)
				if(vertices[i] == vertex)
				{
					uniqueIndex = i;
					found = true;
					break;
				}
			if(!found)
				vertices.push_back(vertex);
			indices.push_back(uniqueIndex);
		}

	std::ofstream file;
	file.open(output);
	file << "struct ModelVertex {" << std::endl;
	file << "float position[3];" << std::endl;
	if(isNormal)
		file << "float normal[3];" << std::endl;
	if(isUv)
		file << "float uv[2];" << std::endl;
	file << "};" << std::endl << std::endl;

	file << "struct ModelVertex const modelVertices[" << vertices.size() << "] = {" << std::endl;
	file.unsetf ( std::ios::floatfield );      
	file.precision(5);
	file.setf( std::ios::fixed, std:: ios::floatfield );
	for(const auto& vertex : vertices)
	{
		file << "{ { " << vertex.position[0] << "f, " << vertex.position[1] << "f, " << vertex.position[2] << "f }, ";
		if(isNormal)
			file << "{ " << vertex.normal[0] << "f, " << vertex.normal[1] << "f, " << vertex.normal[2] << "f }, ";
		if(isUv)
			file << "{ " << vertex.uv[0] << "f, " << vertex.uv[1] << "f } ";
		file << " }," << std::endl;
	}
	file << " };" << std::endl << std::endl;

	file << "const unsigned int modelIndices[" << indices.size() << "] = {" << std::endl;
	unsigned int i = 0;
	for(const auto& index : indices)
	{
		file << index << ", ";
		i++;
		if((i % 3) == 0)
			file << std::endl;
	}	
	file << "};";	

	file.close();
}

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		std::cerr << "Wrong number of arguments, just give me the the name of input and output file. Example: stargate.obj stargate.h" << std::endl;
		return EXIT_FAILURE;
	}

	try
	{
		convert(std::string(argv[1]), std::string(argv[2]));
	}
	catch(const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
