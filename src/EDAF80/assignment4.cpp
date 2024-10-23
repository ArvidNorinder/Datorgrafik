#include "assignment4.hpp"
#include "parametric_shapes.hpp"

#include "config.hpp"
#include "core/Bonobo.h"
#include "core/FPSCamera.h"
#include "core/helpers.hpp"
#include "core/node.hpp"
#include "core/ShaderProgramManager.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <tinyfiledialogs.h>

#include <clocale>
#include <stdexcept>

#include <vector>  // For std::vector'
#include "box.hpp"
#include "bullet.hpp"

edaf80::Assignment4::Assignment4(WindowManager& windowManager) :
	mCamera(0.5f * glm::half_pi<float>(),
	        static_cast<float>(config::resolution_x) / static_cast<float>(config::resolution_y),
	        0.01f, 1000.0f),
	inputHandler(), mWindowManager(windowManager), window(nullptr)
{
	WindowManager::WindowDatum window_datum{ inputHandler, mCamera, config::resolution_x, config::resolution_y, 0, 0, 0, 0};

	window = mWindowManager.CreateGLFWWindow("EDAF80: Assignment 4", window_datum, config::msaa_rate);
	if (window == nullptr) {
		throw std::runtime_error("Failed to get a window: aborting!");
	}

	bonobo::init();
}

edaf80::Assignment4::~Assignment4()
{
	bonobo::deinit();
}


void
edaf80::Assignment4::run()
{

	GLuint cubemap_texture = bonobo::loadTextureCubeMap(
		config::resources_path("cubemaps/NissiBeach2/posx.jpg"),
		config::resources_path("cubemaps/NissiBeach2/negx.jpg"),
		config::resources_path("cubemaps/NissiBeach2/posy.jpg"),
		config::resources_path("cubemaps/NissiBeach2/negy.jpg"),
		config::resources_path("cubemaps/NissiBeach2/posz.jpg"),
		config::resources_path("cubemaps/NissiBeach2/negz.jpg"),
		true);

	GLuint water_normal_map = bonobo::loadTexture2D(config::resources_path("textures/waves.png"));

	// Set up the camera
	mCamera.mWorld.SetTranslate(glm::vec3(0.0f, 30.0f, -100.0f));
	mCamera.mWorld.LookAt(glm::vec3(0.0f));
	mCamera.mMouseSensitivity = glm::vec2(0.003f);
	mCamera.mMovementSpeed = glm::vec3(3.0f); // 3 m/s => 10.8 km/h

	auto camera_position = mCamera.mWorld.GetTranslation();

	// Create the shader programs
	ShaderProgramManager program_manager;
	GLuint fallback_shader = 0u;
	program_manager.CreateAndRegisterProgram("Fallback",
	                                         { { ShaderType::vertex, "common/fallback.vert" },
	                                           { ShaderType::fragment, "common/fallback.frag" } },
	                                         fallback_shader);
	if (fallback_shader == 0u) {
		LogError("Failed to load fallback shader");
		return;
	}

	GLuint skybox_shader = 0u;
	program_manager.CreateAndRegisterProgram("Skybox",
		{ { ShaderType::vertex, "EDAF80/skybox.vert" },
		  { ShaderType::fragment, "EDAF80/skybox.frag" } },
		skybox_shader);

	if (skybox_shader == 0u) {
		LogError("Failed to load skybox shader");
		return;
	}

	GLuint box_shader = 0u;
	program_manager.CreateAndRegisterProgram("Box",
		{ { ShaderType::vertex, "EDAF80/box.vert" },
		  { ShaderType::fragment, "EDAF80/box.frag" } },
		box_shader);

	GLuint player_shader = 0u;
	program_manager.CreateAndRegisterProgram("Player",
		{ { ShaderType::vertex, "EDAF80/player.vert" },
		  { ShaderType::fragment, "EDAF80/player.frag" } },
		player_shader);

	GLuint bullet_shader = 0u;
	program_manager.CreateAndRegisterProgram("Bullet",
		{ { ShaderType::vertex, "EDAF80/bullet.vert" },
		  { ShaderType::fragment, "EDAF80/bullet.frag" } },
		bullet_shader);

	if (bullet_shader == 0u) {
		LogError("Failed to load bullet shader");
		return;
	}


	auto light_position = glm::vec3(-2.0f, 4.0f, 2.0f);

	float elapsed_time_s = 0.0f;
	bool use_normal_mapping = false;
	auto const set_uniforms = [&use_normal_mapping, &light_position, &camera_position, &elapsed_time_s](GLuint program) {
		glUniform1i(glGetUniformLocation(program, "use_normal_mapping"), use_normal_mapping ? 1 : 0);
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));

		glUniform1f(glGetUniformLocation(program, "time"), elapsed_time_s);
		};


	auto const quad_shape = parametric_shapes::createQuad(100.0, 100.0, 1000, 1000);
	if (quad_shape.vao == 0u) {
		LogError("Failed to retrieve the mesh for the quad");
		return;
	}

	//TODO: We want to create our geometry. Probably a good approach is simulating the components as spheres.
	float player_width = 5.0f;
	auto const player_geo = parametric_shapes::createSphere(player_width, 50u, 50u);
	if (player_geo.vao == 0u) {
		LogError("Failed to retrieve the mesh for the player");
		return;
	}

	auto skybox_shape = parametric_shapes::createSphere(20.0f, 100u, 100u);

	//
	// Todo: Load your geometry
	//

	Node player;
	player.set_geometry(player_geo);
	player.set_program(&player_shader, set_uniforms);

	//TODO: If we use the same uniforms in both programs, we dont actually need to set uniforms twice, but the given
	//code expects us to set uniforms when setting a program. Maybe it is not necessary, investigate?

	Node skybox;
	skybox.set_geometry(skybox_shape);
	skybox.set_program(&skybox_shader, set_uniforms);
	skybox.add_texture("nissan_beach", cubemap_texture, GL_TEXTURE_CUBE_MAP);
	

	/*Node quad;
	quad.set_geometry(quad_shape);
	//quad.set_program(&fallback_shader, phong_set_uniforms);
	quad.set_program(&water_shader, phong_set_uniforms);
	quad.add_texture("nissan_beach", cubemap_texture, GL_TEXTURE_CUBE_MAP);
	quad.add_texture("water_normal_map", water_normal_map, GL_TEXTURE_2D);*/

	



	glClearDepthf(1.0f);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);


	auto lastTime = std::chrono::high_resolution_clock::now();

	bool pause_animation = true;
	bool use_orbit_camera = false;
	auto cull_mode = bonobo::cull_mode_t::disabled;
	auto polygon_mode = bonobo::polygon_mode_t::fill;
	bool show_logs = true;
	bool show_gui = true;
	bool shader_reload_failed = false;
	bool show_basis = false;
	float basis_thickness_scale = 1.0f;
	float basis_length_scale = 1.0f;

	changeCullMode(cull_mode);

	//Boxes variables
	std::vector<std::unique_ptr<Box>> active_boxes;
	glm::vec3 player_position(0.0f, 0.0f, 0.0f);
	float box_sphere_radius = 2.0f * player_width;
	int max_boxes_in_width = 4;

	//Field variables
	float left_bound = -max_boxes_in_width * box_sphere_radius + player_width / 2.0f;
	float right_bound = max_boxes_in_width * box_sphere_radius - player_width / 2.0f;

	//Bullet variables
	std::vector<std::unique_ptr<Bullet>> active_bullets;
	float bullet_sphere_radius = 1.0f;
	float last_bullet_time = 0.0f;
	float fire_rate = 1.0f / 5.0f;  // Fire 3 bullets every 1 second

	//Player variables
	int playerHealth = 100;
	int score = 0;
	int high_score = 0;
	bool gameOver = false;


	//TODO: Comment out
	//active_boxes.push_back(Box(elapsed_time_s, mCamera, &box_shader, set_uniforms));

	//Box test = Box(elapsed_time_s, mCamera, box_shader, set_uniforms);

	while (!glfwWindowShouldClose(window)) {
		auto const nowTime = std::chrono::high_resolution_clock::now();
		auto const deltaTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(nowTime - lastTime);
		elapsed_time_s += deltaTimeUs.count() / 1000000.0f;  // Convert microseconds to seconds
		lastTime = nowTime;

		// Poll and handle input events (keyboard, mouse, etc.)
		glfwPollEvents();
		inputHandler.Advance();

		// If the game is over, do not allow further updates
		if (!gameOver) {
			// Check if we need to pause the animation
			if (!pause_animation) {
				elapsed_time_s += std::chrono::duration<float>(deltaTimeUs).count();
			}

			// Check for key inputs to move the player
			if (inputHandler.GetKeycodeState(GLFW_KEY_A) & GLFW_PRESS) {
				player_position.x += box_sphere_radius * 8.0f * deltaTimeUs.count() / 1000000.0f;  // Move left
			}
			if (inputHandler.GetKeycodeState(GLFW_KEY_D) & GLFW_PRESS) {
				player_position.x -= box_sphere_radius * 8.0f * deltaTimeUs.count() / 1000000.0f;  // Move right
			}

			// Update the player's position
			player_position.x = glm::clamp(player_position.x, left_bound, right_bound);  // Keep player within bounds
			player.get_transform().SetTranslate(player_position);

			// Update the bullets and fire rate
			if (elapsed_time_s - last_bullet_time >= fire_rate) {
				glm::vec3 bullet_position = player_position;
				glm::vec3 bullet_direction = glm::vec3(0.0f, 0.0f, 1.0f);
				active_bullets.emplace_back(std::make_unique<Bullet>(elapsed_time_s, mCamera, bullet_shader, set_uniforms, bullet_position, bullet_direction));
				last_bullet_time = elapsed_time_s;  // Reset the timer after shooting a bullet
			}

			// Render all geometry
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			bonobo::changePolygonMode(polygon_mode);

			if (!shader_reload_failed) {
				// Update and render boxes first
				if (active_boxes.size() < 10 && (rand() / static_cast<float>(RAND_MAX)) < 0.02) {
					active_boxes.emplace_back(std::make_unique<Box>(elapsed_time_s, mCamera, box_shader, set_uniforms, max_boxes_in_width, box_sphere_radius));
				}

				for (auto& box_ptr : active_boxes) {
					box_ptr->update(elapsed_time_s, 1.0f);  // Example speed value
					box_ptr->render(mCamera.GetWorldToClipMatrix());

					// Check if the box has passed the player, and reduce health if true
					if (box_ptr->hasPassedPlayer()) {
						playerHealth -= 10;  // Reduce player's health when a box passes
						if (playerHealth <= 0) {
							playerHealth = 0;  // Ensure health doesn't go below 0
							gameOver = true;   // Set the game over flag
						}
					}
				}

				// Perform collision detection after rendering boxes
				for (auto& bullet_ptr : active_bullets) {
					for (auto& box_ptr : active_boxes) {
						float distance = glm::distance(bullet_ptr->getPosition(), box_ptr->getPosition());
						float collision_distance = bullet_sphere_radius + box_sphere_radius;
						if (distance <= collision_distance) {
							bullet_ptr->destroy();  // Mark bullet as destroyed
							box_ptr->takeHit();  // Decrease hit points for the box

							// Increment score if the box is destroyed
							if (box_ptr->isDestroyed()) {
								score += 10;  // Increment score by 10 for each destroyed box
							}
						}
					}
				}

				// Remove destroyed boxes after all updates and collisions are done
				active_boxes.erase(std::remove_if(active_boxes.begin(), active_boxes.end(),
					[](const std::unique_ptr<Box>& box_ptr) {
						return box_ptr->isDestroyed() || box_ptr->hasPassedPlayer();
					}), active_boxes.end());

				// Update and render bullets
				float bullet_speed = 100.0f;  // Adjust as needed
				for (auto& bullet_ptr : active_bullets) {
					bullet_ptr->update(elapsed_time_s, bullet_speed);
					bullet_ptr->render(mCamera.GetWorldToClipMatrix());
				}

				// Remove destroyed bullets after updates
				active_bullets.erase(std::remove_if(active_bullets.begin(), active_bullets.end(),
					[](const std::unique_ptr<Bullet>& bullet_ptr) {
						return bullet_ptr->isDestroyed();
					}), active_bullets.end());

				// Render the player
				player.render(mCamera.GetWorldToClipMatrix());
			}
		}

		// Start the ImGui frame (only once)
		mWindowManager.NewImGuiFrame();

		// Render Player Health ImGui window
		ImGui::Begin("Player Health");
		float healthPercentage = static_cast<float>(playerHealth) / 100.0f;  // Assuming max health is 100
		ImGui::Text("Health:");
		ImGui::ProgressBar(healthPercentage, ImVec2(200, 20));
		ImGui::Text("Health: %d / 100", playerHealth);
		ImGui::End();

		ImGui::Begin("Score");
		ImGui::Text("Score: %d", score);  // Display the score
		ImGui::Text("High Score: %d", high_score);
		ImGui::End();

		if (gameOver) {
			ImGui::Begin("Game Over");
			ImGui::Text("Game Over!");
			// Optionally, add a reset button
			if (ImGui::Button("Reset Game")) {
				playerHealth = 100;  // Reset player health
				gameOver = false;    // Restart the game
				active_boxes.clear(); // Clear existing boxes
				active_bullets.clear(); // Clear bullets

				if (score > high_score) {
					high_score = score;
				}

				score = 0;
			}
			ImGui::End();
		}

		// End the ImGui frame and render it
		mWindowManager.RenderImGuiFrame(show_gui);

		// Swap buffers for the next frame
		glfwSwapBuffers(window);
	}

}

int main()
{
	std::setlocale(LC_ALL, "");

	Bonobo framework;

	try {
		edaf80::Assignment4 assignment4(framework.GetWindowManager());
		assignment4.run();
	} catch (std::runtime_error const& e) {
		LogError(e.what());
	}
}
