#include "linalg.h"
#include "geometry.h"

#include <vector>
#include <initializer_list>

class mesh {
public:
	mesh(std::initializer_list < mesh >);
	mesh(std::vector < int >, std::vector < float >);

	mesh translate(vec3);
	mesh transform(mat3);

	std::vector < tri > to_triangles();

	std::vector < int > ids;
	std::vector < float > vertices;

};
