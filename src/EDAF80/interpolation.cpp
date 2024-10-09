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
	float x2 = x * x;
	float x3 = x2 * x;
	return
		(0.0f * p0 + 1.0f * p1 + 0.0f * p2 + 0.0f * p3) * 1.0f +  // Constant term
		(-t * p0 + 0.0f * p1 + t * p2 + 0.0f * p3) * x +          // Linear term
		(2.0f * t * p0 + (t - 3.0f) * p1 + (3.0f - 2.0f * t) * p2 - t * p3) * x2 + // Quadratic term
		(-t * p0 + (2.0f - t) * p1 + (t - 2.0f) * p2 + t * p3) * x3; // Cubic term
}
