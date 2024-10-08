#include "interpolation.hpp"

glm::vec3
interpolation::evalLERP(glm::vec3 const& p0, glm::vec3 const& p1, float const x)
{
	// Linear interpolation formula: (1 - x) * p0 + x * p1
	return (1.0f - x) * p0 + x * p1;
}


glm::vec3
interpolation::evalCatmullRom(glm::vec3 const& p0, glm::vec3 const& p1,
	glm::vec3 const& p2, glm::vec3 const& p3,
	float const t, float const x)
{
	// Calculate the Catmull-Rom basis using the parameter x
	glm::vec3 a = 2.0f * p1;
	glm::vec3 b = -p0 + p2;
	glm::vec3 c = 2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3;
	glm::vec3 d = -p0 + 3.0f * p1 - 3.0f * p2 + p3;

	// Interpolated value
	return 0.5f * (a + b * x + c * (x * x) + d * (x * x * x));
}
