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
Box::Box(float elapsed_time, FPSCameraf& mCamera, GLuint& box_shader, std::function<void(GLuint)> const& set_uniforms, int max_boxes_in_width, float radius)
	: _destroyed(false),
	_passed_player(false),
	_initial_time(elapsed_time),
	_mCamera(mCamera),
	_box_shader(box_shader),
	_set_uniforms(set_uniforms),
	_hit_points(3 + rand() % 4),
	_max_hit_points(_hit_points)
{
	// Set up geometry
	float box_radius = radius;
	float box_width = 2.0f * box_radius;
	setup_geometry(box_radius);

	// Set up shaders
	setup_shaders();

	loadTexture("C:/Users/Arvid/OneDrive/Skrivbord/School/Datorgrafik/stone_texture.jpg");

	loadDamagedTexture("C:/Users/Arvid/OneDrive/Skrivbord/School/Datorgrafik/cracked_stone.jpg");

	int section = rand() % max_boxes_in_width;  // Get a random section from 0 to max_boxes_in_width - 1, so max_boxes_in_width number of sections
	float field_width = max_boxes_in_width * box_width;

	// Calculate the horizontal position: each section is box_width wide
	float object_horizontal_position = (section - (max_boxes_in_width / 2.0f)) * box_width + (box_width / 2.0f);

	std::cout << object_horizontal_position << std::endl;

	// Distance from the player
	float object_distance = 300.0f;

	// Initialize the box position with the calculated horizontal position
	_position = glm::vec3(object_horizontal_position, 0.0f, object_distance);

	// Set the initial position of the box in the node's transform
	_node.get_transform().SetTranslate(_position);
}


// Sets up the geometry for the box
void Box::setup_geometry(float box_width) {
	// Create sphere geometry for the box
	auto const box_geo = parametric_shapes::createSphere(box_width, 50u, 50u);
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

void Box::takeHit()
{
	_hit_points--;  // Decrease hit points

	// Check if HP is less than or equal to half of the max HP and switch to damaged texture
	if (_hit_points <= (_max_hit_points / 2.0f) && _damaged_texture != 0) {
		_node.add_texture("diffuse_texture", _damaged_texture, GL_TEXTURE_2D);  // Switch to damaged texture
	}

	// Destroy the box if hit points drop to 0 or less
	if (_hit_points <= 0) {
		destroy();
	}
}

void Box::loadTexture(std::string const& texture_path)
{
	_box_texture = bonobo::loadTexture2D(texture_path);  // Load the normal texture

	if (_box_texture == 0) {
		std::cerr << "Failed to load texture: " << texture_path << std::endl;
	}
	else {
		std::cout << "Successfully loaded normal texture: " << texture_path << std::endl;
	}

	_node.add_texture("diffuse_texture", _box_texture, GL_TEXTURE_2D);
}

void Box::loadDamagedTexture(std::string const& texture_path)
{
	_damaged_texture = bonobo::loadTexture2D(texture_path);  // Load the damaged texture

	if (_damaged_texture == 0) {
		std::cerr << "Failed to load damaged texture: " << texture_path << std::endl;
	}
	else {
		std::cout << "Successfully loaded damaged texture: " << texture_path << std::endl;
	}
}


