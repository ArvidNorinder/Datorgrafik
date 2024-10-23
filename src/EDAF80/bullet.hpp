// bullet.hpp

#pragma once

#include "core/FPSCamera.h"
#include "core/node.hpp"
#include <functional>
#include <glm/glm.hpp>

class Bullet {
public:
	Bullet(float elapsed_time,
		FPSCameraf& mCamera,
		GLuint& bullet_shader,
		std::function<void(GLuint)> const& set_uniforms,
		glm::vec3 const& initial_position,
		glm::vec3 const& direction);

	void update(float elapsed_time, float bullet_speed);
	void render(glm::mat4 const& view_projection_matrix);

	bool isDestroyed() const;
	glm::vec3 getPosition() const;
	void destroy();

private:
	void setup_geometry(float bullet_radius);
	void setup_shaders();

	bool _destroyed;
	float _initial_time;
	FPSCameraf& _mCamera;
	Node _node;
	GLuint& _bullet_shader;
	std::function<void(GLuint)> const& _set_uniforms;
	glm::vec3 _position;
	glm::vec3 _direction;
};
