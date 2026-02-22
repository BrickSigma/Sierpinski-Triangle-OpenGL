#include "camera.h"

#include <stdlib.h>

const float camera_speed = 0.05f;

Camera *CreateCamera(vec3 position, vec3 target, vec3 up) {
	Camera *camera = (Camera *)malloc(sizeof(Camera));
	if (camera == NULL) {
		perror("Could not allocate memory for camera");
		return NULL;
	}

	vec3 front;
	glm_vec3_sub(target, position, front);
	glm_vec3_normalize(front);

	vec3 up_normalized;
	glm_vec3_normalize_to(up, up_normalized);

	for (int i = 0; i < 3; i++) {
		camera->pos[i] = position[i];
		camera->up[i] = up_normalized[i];
		camera->front[i] = front[i];
	}

	return camera;
}

void DestroyCamera(Camera *camera) { free(camera); }

void MoveCamera(Camera *camera, vec3 direction) {
	vec3 temp; // Temporary variable for vector calculations

	// Move along the z-axis first
	glm_vec3_scale(camera->front, direction[2], temp);
	glm_vec3_sub(camera->pos, temp, camera->pos);

	// Move along the y-axis next
	glm_vec3_scale(camera->up, direction[1], temp);
	glm_vec3_add(camera->pos, temp, camera->pos);

	// Move along the x-axis last
	glm_vec3_crossn(camera->front, camera->up, temp);
	glm_vec3_scale(temp, direction[0], temp);
	glm_vec3_add(camera->pos, temp, camera->pos);
}

void GetCameraViewMatrix(Camera *camera, mat4 view) {
	vec3 target;
	glm_vec3_add(camera->pos, camera->front, target);
	glm_lookat(camera->pos, target, camera->up, view);
}