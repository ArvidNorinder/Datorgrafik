#pragma once

#include "core/node.hpp"
#include <glm/glm.hpp>

class Box {
public:
	// Constructor initializes the box, sets up geometry, and shaders
	Box(float elapsed_time, FPSCameraf& mCamera, GLuint& box_shader, std::function<void(GLuint)> const& set_uniforms, int max_boxes_in_width, float radius);

	// Update box's position over time and check if it's destroyed or passed the player
	void update(float elapsed_time, float object_speed);

	// Render the box
	void render(glm::mat4 const& view_projection_matrix);

	// Check if the box is destroyed
	bool isDestroyed() const;

	// Check if the box has passed the player
	bool hasPassedPlayer() const;

	// Getter for the box's current position
	glm::vec3 getPosition() const;

	// Destroy the box
	void destroy();

	void takeHit();

	void loadTexture(std::string const& texture_path);

	void loadDamagedTexture(std::string const& texture_path);

private:
	void setup_geometry(float box_width);
	void setup_shaders();

	Node _node;  // Node to represent the box geometry
	glm::vec3 _position;  // Current position of the box
	bool _destroyed;  // True if the box has been destroyed
	bool _passed_player;  // True if the box has passed the player
	float _initial_time;  // The initial time when the box was created
	GLuint _shader_program;  // Shader program for the box
	FPSCameraf _mCamera; //The camera
	GLuint _box_shader;
	std::function<void(GLuint)> _set_uniforms;
	int _hit_points;  // current hit points
	int _max_hit_points;
	GLuint _box_texture;  // Texture ID
	GLuint _damaged_texture; //texture when box is half hp
};
