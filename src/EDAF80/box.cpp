#include "assignment4.hpp"
#include "parametric_shapes.hpp"

#include "core/Bonobo.h"
#include "core/FPSCamera.h"
#include "core/helpers.hpp"
#include "core/node.hpp"
#include "core/ShaderProgramManager.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <clocale>
#include <stdexcept>

#include <vector>  // For std::vector
#include "box.hpp"

// Constructor initializes the box and sets up geometry and shaders
Box::Box(float elapsed_time, FPSCameraf& mCamera, GLuint& box_shader, std::function<void(GLuint)> const& set_uniforms)
	: _destroyed(false),
	_passed_player(false),
	_initial_time(elapsed_time),
	_mCamera(mCamera),
	_box_shader(box_shader),
	_set_uniforms(set_uniforms)
{
	//auto camera_position = mCamera.mWorld.GetTranslation();
	// Set up geometry (e.g., a sphere or a cube)
	setup_geometry();

	// Set up shaders
	setup_shaders();

	// Random horizontal position between -20 and 20
	float object_horizontal_position = ((rand() / (float)RAND_MAX) * 40.0f) - 20.0f;

	// Distance from the player
	float object_distance = 100.0f;

	// Initialize the box position with the random values
	_position = glm::vec3(object_horizontal_position, 0.0f, object_distance);

	// Set the initial position of the box in the node's transform
	_node.get_transform().SetTranslate(_position);
}

// Sets up the geometry for the box
void Box::setup_geometry() {
	// Create sphere geometry for the box
	auto const box_geo = parametric_shapes::createSphere(10.0f, 50u, 50u);
	if (box_geo.vao == 0u) {
		throw std::runtime_error("Failed to create box geometry!");
	}

	// Set the geometry in the node
	_node.set_geometry(box_geo);
}

// Sets up the shaders for the box
void Box::setup_shaders() {

	auto camera_position = _mCamera.mWorld.GetTranslation();

	//TODO: Change to box shaders

	_node.set_program(&_box_shader, _set_uniforms);
}

// Update the box's position over time
void Box::update(float elapsed_time, float object_speed) {
	// Move the box closer to the player over time
	_position.z -= object_speed * (elapsed_time - _initial_time);
	_node.get_transform().SetTranslate(_position);

	// Mark the box as "passed" if it's behind the player
	if (_position.z < 0.0f) {
		_passed_player = true;
	}
}

// Render the box
void Box::render(glm::mat4 const& view_projection_matrix) {
	_node.render(view_projection_matrix);
}

// Check if the box is destroyed
bool Box::isDestroyed() const {
	return _destroyed;
}

// Check if the box has passed the player
bool Box::hasPassedPlayer() const {
	return _passed_player;
}

// Get the box's current position
glm::vec3 Box::getPosition() const {
	return _position;
}

// Destroy the box
void Box::destroy() {
	_destroyed = true;
}
