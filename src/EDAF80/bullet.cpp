// bullet.cpp

#include "bullet.hpp"
#include "parametric_shapes.hpp"
#include <stdexcept>

Bullet::Bullet(float elapsed_time,
	FPSCameraf& mCamera,
	GLuint& bullet_shader,
	std::function<void(GLuint)> const& set_uniforms,
	glm::vec3 const& initial_position,
	glm::vec3 const& direction)
	: _destroyed(false),
	_initial_time(elapsed_time),
	_mCamera(mCamera),
	_bullet_shader(bullet_shader),
	_set_uniforms(set_uniforms),
	_position(initial_position),
	_direction(glm::normalize(direction))
{
	// Set up geometry
	float bullet_radius = 1.0f;  // Adjust as needed
	setup_geometry(bullet_radius);

	// Set up shaders
	setup_shaders();

	// Set the initial position of the bullet
	_node.get_transform().SetTranslate(_position);
}

void Bullet::setup_geometry(float bullet_radius) {
	auto const bullet_geo = parametric_shapes::createSphere(bullet_radius, 20u, 20u);
	if (bullet_geo.vao == 0u) {
		throw std::runtime_error("Failed to create bullet geometry!");
	}

	_node.set_geometry(bullet_geo);
}

void Bullet::setup_shaders() {
	_node.set_program(&_bullet_shader, _set_uniforms);
}

void Bullet::update(float elapsed_time, float bullet_speed) {
	// Move the bullet along its direction
	float delta_time = elapsed_time - _initial_time;
	_position += _direction * bullet_speed * delta_time;
	_node.get_transform().SetTranslate(_position);
	_initial_time = elapsed_time;  // Update initial time for the next frame

	// Optionally, you can set a maximum distance or check if the bullet is off-screen
	// and mark it as destroyed
}

void Bullet::render(glm::mat4 const& view_projection_matrix) {
	_node.render(view_projection_matrix);
}

bool Bullet::isDestroyed() const {
	return _destroyed;
}

glm::vec3 Bullet::getPosition() const {
	return _position;
}

void Bullet::destroy() {
	_destroyed = true;
}
