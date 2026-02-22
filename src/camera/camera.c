#include "camera.h"

#include <stdlib.h>
#include <math.h>

const float camera_speed = 0.025f;

const float camera_sensitivity = 0.1f;

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

	camera->pitch = asin(front[1]);
	float x_yaw = acos(front[0]/cos(camera->pitch));
	float z_yaw = asin(front[2]/cos(camera->pitch));

	if ((x_yaw >= 0) && z_yaw >= 0) {
		camera->yaw = x_yaw;
	} else if ((x_yaw >= 0) && (z_yaw <= 0)) {
		camera->yaw = GLM_PI - x_yaw;
	} else if ((x_yaw <= 0) && (z_yaw >= 0)) {
		camera->yaw = x_yaw;
	} else {
		camera->yaw = GLM_PI - x_yaw;
	}

	printf("Pitch: %f Yaw: %f\n", camera->pitch, camera->yaw);

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

void RotateCamera(Camera *camera, float pitch, float yaw) {
	// vec3 direction;
	pitch = glm_rad(pitch);
	yaw = glm_rad(yaw);

	camera->pitch += pitch;
	camera->yaw += yaw;

	if (glm_deg(camera->pitch) > 89.0f) {
		camera->pitch = glm_rad(89.0f);
	} else if (glm_deg(camera->pitch) < -89.0f) {
		camera->pitch = glm_rad(-89.0f);
	}

	printf("(%f %f)\n", camera->pitch, camera->yaw);

	camera->front[0] = cos(camera->yaw)*cos(camera->pitch);
	camera->front[1] = sin(camera->pitch);
	camera->front[2] = sin(camera->yaw)*cos(camera->pitch);

	printf("Camera front: [%f, %f, %f]\n", camera->front[0], camera->front[1], camera->front[2]);
}

void GetCameraViewMatrix(Camera *camera, mat4 view) {
	vec3 target;
	glm_vec3_add(camera->pos, camera->front, target);
	glm_lookat(camera->pos, target, camera->up, view);
}