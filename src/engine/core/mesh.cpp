#include "mesh.h"


Mesh::Mesh() : filename("0"), va_handle(0), vbo_handle(0), ibo_handle(0)
{
}

Mesh::~Mesh()
{
	glBindVertexArray(va_handle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &ibo_handle);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glDeleteBuffers(1, &vbo_handle);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &va_handle);
}

Mesh::Mesh(const std::string fn) : filename(fn), va_handle(0), vbo_handle(0), ibo_handle(0)
{
}

bool Mesh::bufferDataFromArray(const Vertex_p *vertex_data, const GLuint *index_data, const GLsizei va_size, const GLsizei vi_size, GLenum mesh_type)
{
	if(va_handle == 0 || vbo_handle == 0 || ibo_handle == 0)
	{
		glGenVertexArrays(1, &va_handle);
		glGenBuffers(1, &vbo_handle);
		glGenBuffers(1, &ibo_handle);
	}

	glBindVertexArray(va_handle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
	glBufferData(GL_ARRAY_BUFFER, va_size, vertex_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_handle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vi_size, index_data, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	num_vertices = (vi_size/sizeof(GLuint));
	type = mesh_type;

	return true;
}

void Mesh::draw(int num_instances)
{
	glBindVertexArray(va_handle);
	/*	If stored correctly in the VAO, there is no need to rebind buffers again */
	//glDrawElements(type, num_vertices, GL_UNSIGNED_INT, nullptr);
	glDrawElementsInstanced(type, num_vertices, GL_UNSIGNED_INT, nullptr, num_instances);
}

void Mesh::setVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer)
{
	glBindVertexArray(va_handle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::setVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
	glBindVertexArray(va_handle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
	glEnableVertexAttribArray(index);
	glVertexAttribIPointer(index, size, type, stride, pointer);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::setVertexAttribLPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
	glBindVertexArray(va_handle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
	glEnableVertexAttribArray(index);
	glVertexAttribLPointer(index, size, type, stride, pointer);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

std::shared_ptr<Mesh> Mesh::loadFromFBX(const std::string &filename, FBX::OpenGL::BindAttribLocations locations)
{
	std::shared_ptr<Mesh> mesh(new Mesh(filename));

	std::shared_ptr<FBX::Geometry> geometry = FBX::Geometry::fbxLoadFirstGeometry(filename);
	FBX::OpenGL::GeometrySerialize serialize(geometry->features);

	uint8_t* memory;
	size_t memory_size;
	serialize.serialize(memory, memory_size, geometry->vertices);

	if (sizeof(unsigned int) == sizeof(GLuint)) {
		mesh->bufferDataFromArray(reinterpret_cast<Vertex_p*>(memory), reinterpret_cast<const GLuint*>(geometry->triangle_indices.data()), memory_size, geometry->triangle_indices.size() * sizeof(GLuint), GL_TRIANGLES);
	} else {
		std::vector<GLuint> gluint_indices(geometry->triangle_indices.begin(), geometry->triangle_indices.end());
		mesh->bufferDataFromArray(reinterpret_cast<Vertex_p*>(memory), gluint_indices.data(), memory_size, geometry->triangle_indices.size() * sizeof(GLuint), GL_TRIANGLES);
	}

	delete[] memory;

	const FBX::OpenGL::GeometrySerialize::Settings &s(serialize.settings());

	if (locations.ndx_position >= 0) {
		mesh->setVertexAttribPointer(locations.ndx_position, 3, GL_FLOAT        , GL_FALSE, s.stride, (void*) s.offset_position);
	}
	if (s.features & FBX::Geometry::NORMAL && locations.ndx_normal >= 0) {
		mesh->setVertexAttribPointer(locations.ndx_normal  , 3, GL_FLOAT        , GL_FALSE, s.stride, (void*) s.offset_normal);
	}
	if (s.features & FBX::Geometry::TANGENT && locations.ndx_tangent >= 0) {
		mesh->setVertexAttribPointer(locations.ndx_tangent , 3, GL_FLOAT        , GL_FALSE, s.stride, (void*) s.offset_tangent);
	}
	if (s.features & FBX::Geometry::COLOR && locations.ndx_color >= 0) {
		mesh->setVertexAttribPointer(locations.ndx_color   , 4, GL_UNSIGNED_BYTE, GL_TRUE , s.stride, (void*) s.offset_color);
		// no static color support in Mesh
		// ndx_static_color = -1;
	} else {
		// no static color support in Mesh
		// ndx_static_color = locations.ndx_color;
	}
	if (s.features & FBX::Geometry::UVCOORD && locations.ndx_uvcoord >= 0) {
		mesh->setVertexAttribPointer(locations.ndx_uvcoord , 2, GL_FLOAT        , GL_FALSE, s.stride, (void*) s.offset_uvcoord);
	}
	if (s.features & FBX::Geometry::BINORMAL && locations.ndx_binormal >= 0) {
		mesh->setVertexAttribPointer(locations.ndx_binormal, 3, GL_FLOAT        , GL_FALSE, s.stride, (void*) s.offset_binormal);
	}

	return mesh;
}
