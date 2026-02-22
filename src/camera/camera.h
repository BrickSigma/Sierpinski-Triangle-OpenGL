#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

// Camera struct
typedef struct Camera {
	vec3 pos;   // Position of the camera in the world
	vec3 front; // Normalized direction of camera towards a target (represents
	            // the Z-axis)
	vec3 up;    // Up (Y-axis) vector
} Camera;

extern const float camera_speed;

// Create a new camera object.
Camera *CreateCamera(vec3 position, vec3 target, vec3 up);

// Destroy the camera object
void DestroyCamera(Camera *camera);

/**
Move the camera around the world.

`direction` is relative to the camera's position, not the world position.
*/
void MoveCamera(Camera *camera, vec3 direction);

/**
Get the view matrix for the camera and place it in the `view` parameter.
*/
void GetCameraViewMatrix(Camera *camera, mat4 view);

#endif // CAMERA_H