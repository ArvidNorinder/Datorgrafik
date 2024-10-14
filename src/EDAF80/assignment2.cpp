#include "assignment2.hpp"
#include "interpolation.hpp"
#include "parametric_shapes.hpp"

#include "config.hpp"
#include "core/Bonobo.h"
#include "core/FPSCamera.h"
#include "core/node.hpp"
#include "core/ShaderProgramManager.hpp"
#include <imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <clocale>
#include <cstdlib>
#include <stdexcept>

edaf80::Assignment2::Assignment2(WindowManager& windowManager) :
	mCamera(0.5f * glm::half_pi<float>(),
	        static_cast<float>(config::resolution_x) / static_cast<float>(config::resolution_y),
	        0.01f, 1000.0f),
	inputHandler(), mWindowManager(windowManager), window(nullptr)
{
	WindowManager::WindowDatum window_datum{ inputHandler, mCamera, config::resolution_x, config::resolution_y, 0, 0, 0, 0};

	window = mWindowManager.CreateGLFWWindow("EDAF80: Assignment 2", window_datum, config::msaa_rate);
	if (window == nullptr) {
		throw std::runtime_error("Failed to get a window: aborting!");
	}

	bonobo::init();
}

edaf80::Assignment2::~Assignment2()
{
	bonobo::deinit();
}

void
edaf80::Assignment2::run()
{
	// Load the sphere geometry
	auto const shape = parametric_shapes::createSphere(0.15f, 10u, 10u);
	if (shape.vao == 0u)
		return;

	// Set up the camera
	mCamera.mWorld.SetTranslate(glm::vec3(0.0f, 0.0f, 0.5f));
	mCamera.mMouseSensitivity = glm::vec2(0.003f);
	mCamera.mMovementSpeed = glm::vec3(3.0f); // 3 m/s => 10.8 km/h

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

	GLuint diffuse_shader = 0u;
	program_manager.CreateAndRegisterProgram("Diffuse",
	                                         { { ShaderType::vertex, "EDAF80/diffuse.vert" },
	                                           { ShaderType::fragment, "EDAF80/diffuse.frag" } },
	                                         diffuse_shader);
	if (diffuse_shader == 0u)
		LogError("Failed to load diffuse shader");

	GLuint normal_shader = 0u;
	program_manager.CreateAndRegisterProgram("Normal",
	                                         { { ShaderType::vertex, "EDAF80/normal.vert" },
	                                           { ShaderType::fragment, "EDAF80/normal.frag" } },
	                                         normal_shader);
	if (normal_shader == 0u)
		LogError("Failed to load normal shader");

	GLuint tangent_shader = 0u;
	program_manager.CreateAndRegisterProgram("Tangent",
	                                         { { ShaderType::vertex, "EDAF80/tangent.vert" },
	                                           { ShaderType::fragment, "EDAF80/tangent.frag" } },
	                                         tangent_shader);
	if (tangent_shader == 0u)
		LogError("Failed to load tangent shader");

	GLuint binormal_shader = 0u;
	program_manager.CreateAndRegisterProgram("Bitangent",
	                                         { { ShaderType::vertex, "EDAF80/binormal.vert" },
	                                           { ShaderType::fragment, "EDAF80/binormal.frag" } },
	                                         binormal_shader);
	if (binormal_shader == 0u)
		LogError("Failed to load binormal shader");

	GLuint texcoord_shader = 0u;
	program_manager.CreateAndRegisterProgram("Texture coords",
	                                         { { ShaderType::vertex, "EDAF80/texcoord.vert" },
	                                           { ShaderType::fragment, "EDAF80/texcoord.frag" } },
	                                         texcoord_shader);
	if (texcoord_shader == 0u)
		LogError("Failed to load texcoord shader");

	auto const light_position = glm::vec3(-2.0f, 4.0f, 2.0f);
	auto const set_uniforms = [&light_position](GLuint program){
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
	};

	// Set the default tensions value; it can always be changed at runtime
	// through the "Scene Controls" window.
	float catmull_rom_tension = 0.5f;

	// Set whether the default interpolation algorithm should be the linear one;
	// it can always be changed at runtime through the "Scene Controls" window.
	bool use_linear = true;

	// Set whether to interpolate the position of an object or not; it can
	// always be changed at runtime through the "Scene Controls" window.
	bool interpolate = false;

	// Set whether to show the control points or not; it can always be changed
	// at runtime through the "Scene Controls" window.
	bool show_control_points = true;

	auto circle_rings = Node();
	circle_rings.set_geometry(shape);
	circle_rings.set_program(&fallback_shader, set_uniforms);
	TRSTransformf& circle_rings_transform_ref = circle_rings.get_transform();


	//! \todo Create a tesselated sphere and a tesselated torus


	glClearDepthf(1.0f);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);


	auto const control_point_sphere = parametric_shapes::createSphere(0.1f, 10u, 10u);
	std::array<glm::vec3, 9> control_point_locations = {
		glm::vec3( 0.0f,  0.0f,  0.0f),
		glm::vec3( 1.0f,  1.8f,  1.0f),
		glm::vec3( 2.0f,  1.2f,  2.0f),
		glm::vec3( 3.0f,  3.0f,  3.0f),
		glm::vec3( 3.0f,  0.0f,  3.0f),
		glm::vec3(-2.0f, -1.0f,  3.0f),
		glm::vec3(-3.0f, -3.0f, -3.0f),
		glm::vec3(-2.0f, -1.2f, -2.0f),
		glm::vec3(-1.0f, -1.8f, -1.0f)
	};
	std::array<Node, control_point_locations.size()> control_points;
	for (std::size_t i = 0; i < control_point_locations.size(); ++i) {
		auto& control_point = control_points[i];
		control_point.set_geometry(control_point_sphere);
		control_point.set_program(&diffuse_shader, set_uniforms);
		control_point.get_transform().SetTranslate(control_point_locations[i]);
	}


	auto lastTime = std::chrono::high_resolution_clock::now();

	std::int32_t program_index = 0;
	float elapsed_time_s = 0.0f;
	auto cull_mode = bonobo::cull_mode_t::disabled;
	auto polygon_mode = bonobo::polygon_mode_t::fill;
	bool show_logs = true;
	bool show_gui = true;
	bool show_basis = false;
	float basis_thickness_scale = 1.0f;
	float basis_length_scale = 1.0f;

	changeCullMode(cull_mode);

	while (!glfwWindowShouldClose(window)) {
		auto const nowTime = std::chrono::high_resolution_clock::now();
		auto const deltaTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(nowTime - lastTime);
		lastTime = nowTime;

		auto& io = ImGui::GetIO();
		inputHandler.SetUICapture(io.WantCaptureMouse, io.WantCaptureKeyboard);

		glfwPollEvents();
		inputHandler.Advance();
		mCamera.Update(deltaTimeUs, inputHandler);
		elapsed_time_s += std::chrono::duration<float>(deltaTimeUs).count();

		if (inputHandler.GetKeycodeState(GLFW_KEY_F3) & JUST_RELEASED)
			show_logs = !show_logs;
		if (inputHandler.GetKeycodeState(GLFW_KEY_F2) & JUST_RELEASED)
			show_gui = !show_gui;
		if (inputHandler.GetKeycodeState(GLFW_KEY_F11) & JUST_RELEASED)
			mWindowManager.ToggleFullscreenStatusForWindow(window);


		// Retrieve the actual framebuffer size: for HiDPI monitors,
		// you might end up with a framebuffer larger than what you
		// actually asked for. For example, if you ask for a 1920x1080
		// framebuffer, you might get a 3840x2160 one instead.
		// Also it might change as the user drags the window between
		// monitors with different DPIs, or if the fullscreen status is
		// being toggled.
		int framebuffer_width, framebuffer_height;
		glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
		glViewport(0, 0, framebuffer_width, framebuffer_height);

		mWindowManager.NewImGuiFrame();


		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		bonobo::changePolygonMode(polygon_mode);


		if (interpolate) {
			if (use_linear) {
				// Determine how long it should take to interpolate between control points
				float total_time = 5.0f;  // Adjust this value based on your desired speed

				// Calculate how far along the entire path you are, using elapsed_time_s
				float path_time = fmod(elapsed_time_s, total_time);

				// Compute which segment of control points we're currently in
				unsigned int current_segment = static_cast<unsigned int>(floor((path_time / total_time) * (control_point_locations.size() - 1)));

				// Ensure that the current segment doesn't go out of bounds
				if (current_segment > control_point_locations.size() - 1) {
					current_segment = control_point_locations.size() - 2;
				}

				// Get the control points to interpolate between
				glm::vec3 const& p0 = control_point_locations[current_segment];
				glm::vec3 const& p1 = control_point_locations[current_segment + 1];

				// Calculate the interpolation factor (x)
				float segment_duration = total_time / (control_point_locations.size() - 1);
				float segment_time = fmod(path_time, segment_duration);
				float x = segment_time / segment_duration;

				// Perform the LERP interpolation between the two control points
				glm::vec3 new_position = interpolation::evalLERP(p0, p1, x);

				// Set the position of the object
				circle_rings.get_transform().SetTranslate(new_position);

			}
			else {
				// Catmull-Rom interpolation logic

				float total_time = 5.0f;  // Total time for interpolation
				float path_time = fmod(elapsed_time_s, total_time);

				unsigned int current_segment = static_cast<unsigned int>(floor(path_time / total_time * (control_point_locations.size() - 1)));

				// Get the four control points for Catmull-Rom
				glm::vec3 const& p0 = control_point_locations[current_segment - 1];
				glm::vec3 const& p1 = control_point_locations[current_segment];
				glm::vec3 const& p2 = control_point_locations[current_segment + 1];
				glm::vec3 const& p3 = control_point_locations[current_segment + 2];

				float segment_duration = total_time / (control_point_locations.size() - 1);
				float segment_time = fmod(path_time, segment_duration);
				float x = segment_time / segment_duration;

				glm::vec3 new_position = interpolation::evalCatmullRom(p0, p1, p2, p3, catmull_rom_tension, x);

				circle_rings.get_transform().SetTranslate(new_position);
			}
		}


		circle_rings.render(mCamera.GetWorldToClipMatrix());
		if (show_control_points) {
			for (auto const& control_point : control_points) {
				//control_point.render(mCamera.GetWorldToClipMatrix());
			}
		}

		bool const opened = ImGui::Begin("Scene Controls", nullptr, ImGuiWindowFlags_None);
		if (opened) {
			auto const cull_mode_changed = bonobo::uiSelectCullMode("Cull mode", cull_mode);
			if (cull_mode_changed) {
				changeCullMode(cull_mode);
			}
			bonobo::uiSelectPolygonMode("Polygon mode", polygon_mode);
			auto selection_result = program_manager.SelectProgram("Shader", program_index);
			if (selection_result.was_selection_changed) {
				circle_rings.set_program(selection_result.program, set_uniforms);
			}
			ImGui::Separator();
			ImGui::Checkbox("Show control points", &show_control_points);
			ImGui::Checkbox("Enable interpolation", &interpolate);
			ImGui::Checkbox("Use linear interpolation", &use_linear);
			ImGui::SliderFloat("Catmull-Rom tension", &catmull_rom_tension, 0.0f, 1.0f);
			ImGui::Separator();
			ImGui::Checkbox("Show basis", &show_basis);
			ImGui::SliderFloat("Basis thickness scale", &basis_thickness_scale, 0.0f, 100.0f);
			ImGui::SliderFloat("Basis length scale", &basis_length_scale, 0.0f, 100.0f);
		}
		ImGui::End();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (show_basis)
			bonobo::renderBasis(basis_thickness_scale, basis_length_scale, mCamera.GetWorldToClipMatrix());
		if (show_logs)
			Log::View::Render();
		mWindowManager.RenderImGuiFrame(show_gui);

		glfwSwapBuffers(window);
	}
}

int main()
{
	std::setlocale(LC_ALL, "");

	Bonobo framework;

	try {
		edaf80::Assignment2 assignment2(framework.GetWindowManager());
		assignment2.run();
	} catch (std::runtime_error const& e) {
		LogError(e.what());
	}
}
