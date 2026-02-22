#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cglm/cglm.h>
#include <glad/glad.h>

#include "camera/camera.h"
#include "clock/clock.h"
#include "shaders/shader.h"
#include "vertices.h"

// Used to handle joystick drifting. (My controller suffers terribly with it
// >~< )
const Sint16 DRIF_THRESHOLD = 20000;

const Sint16 GAMEPAD_MAX_AXIS = 32767 - DRIF_THRESHOLD;

const float ROTATION_SPEED = 10.0f;

// Callback function to handle mouse movement.
void handle_mouse(void *user_data, Uint64 timestamp, SDL_Window *window,
                  SDL_MouseID mouse_id, float *x, float *y);

// Used to draw a traingle.
void draw_triangle(vec3 top, float scale, unsigned int uniform_loc);

/**
 * Draw Serpinski's traingle.
 *
 * `top`, `bottom_left`, `bottom_right` are the coordinates of the triangle to
 * draw.
 *
 * `subdivide` indicates whether the traingle should be divided using recursion.
 *
 * `uniform_loc` is the location of the transform's uniform in the shader
 * program.
 *
 * Note: this assumes the correct shader program has been loaded and the
 * uniforms have been setup properly.
 */
void draw_serpinskis_triangle(vec3 top, int subdivide, float scale,
                              unsigned int uniform_loc);

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);

	// Define OpenGL aatributes for SDL
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
	                    SDL_GL_CONTEXT_PROFILE_CORE);

	// Setup the window and GL context

	SDL_Window *window =
	    SDL_CreateWindow("Sierpinski's Triangle", 800, 800, SDL_WINDOW_OPENGL);

	SDL_GLContext context = SDL_GL_CreateContext(window);
	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
	glViewport(0, 0, 800, 800);
	glEnable(GL_DEPTH_TEST);

	// Copy the vertex data to the GPU for OpenGL

	unsigned int vbo, vao;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6,
	                      (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6,
	                      (void *)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	// Setup the shader program
	ShaderProgram *program = LoadShaderProgram("shader.vert", "shader.frag");
	UseShaderProgram(program);

	mat4 transform = GLM_MAT4_IDENTITY_INIT;
	glm_scale(transform, (vec3){0.5, 0.5, 0.0});

	unsigned int model_uniform = glGetUniformLocation(*program, "model");
	unsigned int view_uniform = glGetUniformLocation(*program, "view");
	unsigned int perspective_uniform =
	    glGetUniformLocation(*program, "perspective");

	// Camera and clock setup

	Camera *camera = CreateCamera((vec3){0.0, 0.0, 3.0}, (vec3){0.0, 0.0, 0.0},
	                              (vec3){0.0, 1.0, 0.0});

	Clock *clock = CreateClock(60);

	// Mouse callback setup
	SDL_SetWindowRelativeMouseMode(window, true);
	SDL_SetRelativeMouseTransform(handle_mouse, camera);

	// Gamepad setup
	int num_joysticks;
	SDL_JoystickID *joysticks = SDL_GetGamepads(&num_joysticks);
	SDL_JoystickID joystick_id;
	// If `NULL`, then no gamepad has been connected.
	SDL_Gamepad *gamepad = NULL;
	if (num_joysticks > 0) {
		joystick_id = joysticks[0]; // Select only one joystick.
		gamepad = SDL_OpenGamepad(joystick_id);
	}
	free(joysticks);

	printf("%s\n",
	       (gamepad == NULL) ? "Gamepad not connected!" : "Gamepad connected!");

	int subdivide = 0;

	bool running = true;
	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_QUIT:
				running = false;
				break;
			case SDL_EVENT_KEY_DOWN:
				switch (event.key.key) {
				case SDLK_ESCAPE:
					running = false;
					break;
				case SDLK_DOWN:
					subdivide--;
					if (subdivide < 0)
						subdivide = 0;
					break;
				case SDLK_UP:
					subdivide++;
					break;
				}
				break;
			}
		}

		const bool *keys = SDL_GetKeyboardState(NULL);
		vec3 direction = {0.0, 0.0, 0.0};

		if (keys[SDL_SCANCODE_A]) {
			direction[0] -= camera_speed;
		} else if (keys[SDL_SCANCODE_D]) {
			direction[0] += camera_speed;
		}

		if (keys[SDL_SCANCODE_W]) {
			direction[2] -= camera_speed;
		} else if (keys[SDL_SCANCODE_S]) {
			direction[2] += camera_speed;
		}

		if (keys[SDL_SCANCODE_LSHIFT]) {
			direction[1] += camera_speed;
		} else if (keys[SDL_SCANCODE_LCTRL]) {
			direction[1] -= camera_speed;
		}

		if (gamepad != NULL) {
			Sint16 x_axis = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
			Sint16 y_axis = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);

			// Movement
			if (x_axis > DRIF_THRESHOLD) {
				direction[0] +=
				    camera_speed * ((float)(x_axis - DRIF_THRESHOLD) /
				                    (float)GAMEPAD_MAX_AXIS);
			} else if (x_axis < -DRIF_THRESHOLD) {
				direction[0] -=
				    camera_speed * ((float)((-x_axis) - DRIF_THRESHOLD) /
				                    (float)GAMEPAD_MAX_AXIS);
			}

			if (y_axis > DRIF_THRESHOLD) {
				direction[2] +=
				    camera_speed * ((float)(y_axis - DRIF_THRESHOLD) /
				                    (float)GAMEPAD_MAX_AXIS);
			} else if (y_axis < -DRIF_THRESHOLD) {
				direction[2] -=
				    camera_speed * (((float)((-y_axis) - DRIF_THRESHOLD) /
				                     (float)GAMEPAD_MAX_AXIS));
			}

			// Move up/down
			if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH)) {
				direction[1] += camera_speed;
			} else if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_EAST)) {
				direction[1] -= camera_speed;
			}

			// Handle looking around
			x_axis = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTX);
			y_axis = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTY);

			float pitch = 0.0f, yaw = 0.0f;
			if (x_axis > DRIF_THRESHOLD) {
				yaw = ROTATION_SPEED * ((float)(x_axis - DRIF_THRESHOLD) /
				              (float)GAMEPAD_MAX_AXIS);
			} else if (x_axis < -DRIF_THRESHOLD) {
				yaw = -ROTATION_SPEED * ((float)((-x_axis) - DRIF_THRESHOLD) /
				               (float)GAMEPAD_MAX_AXIS);
			}

			if (y_axis > DRIF_THRESHOLD) {
				pitch = -ROTATION_SPEED * ((float)(y_axis - DRIF_THRESHOLD) /
				                 (float)GAMEPAD_MAX_AXIS);
			} else if (y_axis < -DRIF_THRESHOLD) {
				pitch = ROTATION_SPEED * (((float)((-y_axis) - DRIF_THRESHOLD) /
				                 (float)GAMEPAD_MAX_AXIS));
			}

			pitch *= camera_sensitivity;
			yaw *= camera_sensitivity;
			RotateCamera(camera, pitch, yaw);
		}

		MoveCamera(camera, direction);

		mat4 view;
		GetCameraViewMatrix(camera, view);

		mat4 perspective;
		glm_perspective(glm_rad(45.0f), 1.0f, 0.1f, 100.0f, perspective);

		glUniformMatrix4fv(view_uniform, 1, GL_FALSE, (float *)view);
		glUniformMatrix4fv(perspective_uniform, 1, GL_FALSE,
		                   (float *)perspective);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		draw_serpinskis_triangle((vec3){0.0, 0.5, 0.0}, subdivide, 1.0,
		                         model_uniform);

		SDL_GL_SwapWindow(window);

		TickClock(clock);
	}

	SDL_CloseGamepad(gamepad);
	DestroyClock(clock);
	DestroyCamera(camera);
	DeleteShaderProgram(program);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	SDL_GL_DestroyContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

void handle_mouse(void *user_data, Uint64 timestamp, SDL_Window *window,
                  SDL_MouseID mouse_id, float *x, float *y) {
	// Ignore unused variable warnings
	(void)timestamp;
	(void)window;
	(void)mouse_id;

	Camera *camera = (Camera *)user_data;

	float pitch = *y * camera_sensitivity;
	float yaw = *x * camera_sensitivity;
	RotateCamera(camera, -pitch, yaw);
}

void draw_triangle(vec3 top, float scale, unsigned int uniform_loc) {
	vec3 center = {top[0], (top[1] - (0.5f * scale)), top[2]};

	mat4 model = GLM_MAT4_IDENTITY_INIT;
	glm_translate(model, center);
	glm_scale(model, (vec3){scale, scale, scale});

	glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, (float *)model);
	glDrawArrays(GL_TRIANGLES, 0, 18);
}

void draw_serpinskis_triangle(vec3 top, int subdivide, float scale,
                              unsigned int uniform_loc) {
	if (subdivide <= 0) { // Base case
		draw_triangle(top, scale, uniform_loc);
		return;
	}

	subdivide--;

	/**
	 *         top
	 *          ^
	 *         / \
	 *        /   \
	 * mid1  *     *  mid2
	 *      /       \
	 *     /         \
	 * bl /_____*_____\  br
	 *         mid3
	 *
	 */

	vec3 mid_lf, mid_lb, mid_rf, mid_rb;
	vec3 base_center;

	float *corners[] = {mid_rf, mid_rb, mid_lb, mid_lf};

	vec3 translations[] = {
	    {0.5, -1.0, 0.5},
	    {0.5, -1.0, -0.5},
	    {-0.5, -1.0, -0.5},
	    {-0.5, -1.0, 0.5},
	};

	// Rotates from right-front, right-back, left-back, and left-front.
	scale *= 0.5f;
	for (int i = 0; i < 4; i++) {
		glm_vec3_scale(translations[i], scale, translations[i]);
		glm_vec3_add(top, translations[i], corners[i]);
	}

	glm_vec3_add(mid_lf, mid_rb, base_center);
	glm_vec3_divs(base_center, 2.0f, base_center);

	draw_serpinskis_triangle(top, subdivide, scale, uniform_loc);
	draw_serpinskis_triangle(mid_lf, subdivide, scale, uniform_loc);
	draw_serpinskis_triangle(mid_lb, subdivide, scale, uniform_loc);
	draw_serpinskis_triangle(mid_rf, subdivide, scale, uniform_loc);
	draw_serpinskis_triangle(mid_rb, subdivide, scale, uniform_loc);

	return;
}
