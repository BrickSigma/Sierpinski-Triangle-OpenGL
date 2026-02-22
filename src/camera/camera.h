#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

// Camera struct
typedef struct Camera {
	vec3 pos;   // Position of the camera in the world
	vec3 front; // Normalized direction of camera towards a target (represents
	            // the Z-axis)
	vec3 up;    // Up (Y-axis) vector
	float pitch;
	float yaw;
} Camera;

// Camera movement speed;
extern const float camera_speed;

// Camera rotation sensitivity
extern const float camera_sensitivity;

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
 * Rotate the camera around.
 * 
 * `pitch` and `yaw` don't set the camera's angle to a fixed point, they either add/subtract from it.
 * 
 * Both angles are specified in degrees, not radius.
 */
void RotateCamera(Camera *camera, float pitch, float yaw);

/**
Get the view matrix for the camera and place it in the `view` parameter.
*/
void GetCameraViewMatrix(Camera *camera, mat4 view);

#endif // CAMERA_H