#pragma once

#ifndef __CAM_H__
#define __CAM_H__
#include <typedef.h>

enum CameraMovement {
	FORWARD, BACKWARD, RIGTH, LEFT, UP, DOWN
};

class Cam {
	vec3 pos;
	vec3 lookat;
	vec3 up;
	vec3 rigth;
	vec3 worldUp;
	f32 yaw;
	f32 pitch;
	f32 zoom;
	f32 mouseSensitivity;
	f32 speed;
public:
	Cam(f32 px = 0.0f, f32 py = 0.0f, f32 pz = 0.0f,
		f32 fx = 0.0f, f32 fy = 0.0f, f32 fz = -1.0f,
		f32 ux = 0.0f, f32 uy = 1.0f, f32 uz = 0.0f,
		f32 yaw = -90.0f, f32 pitch = 0.0f, f32 zoom = 45.0f)
		: pos({ px, py, pz }), lookat({ fx, fy, fz }), up({ ux, uy, uz }),
		worldUp({ ux, uy, uz }), yaw(yaw), pitch(pitch), zoom(zoom),
		mouseSensitivity(0.1f), speed(2.5f) {
		updateVectors();
	}
	mat4 getViewM4() {
		return lookAt(pos, pos + lookat, up);
	}
	f32 getZoom() {
		return radians(zoom);
	}

	void processKeyboard(CameraMovement direction, f32 deltaTime) {
		f32 velocity = speed * deltaTime;

		if (direction == FORWARD)
			pos += lookat * velocity;
		else if (direction == BACKWARD)
			pos -= lookat * velocity;
		else if (direction == LEFT)
			pos -= rigth * velocity;
		else if (direction == RIGTH)
			pos += rigth * velocity;
		else if (direction == UP)
			pos += normalize(cross(rigth, lookat)) * velocity;
		else if (direction == DOWN)
			pos -= normalize(cross(rigth, lookat)) * velocity;

	}

	void processMouse(f32 xoffset, f32 yoffset, bool constraintPitch = true) {
		yaw += xoffset * mouseSensitivity;
		pitch += yoffset * mouseSensitivity;
		if (constraintPitch) {
			if (pitch > 89.0f)
				pitch = 89.0f;
			else if (pitch < -89.0f)
				pitch = -89.0f;
		}
		updateVectors();
	}

	void processScroll(f32 yoffset) {
		zoom -= (f32)yoffset;
		if (zoom < 1.0f)
			zoom = 1.0f;
		else if (zoom > 45.0f)
			zoom = 45.0f;
	}

	vec3 getPos() {
		return pos;
	}

	vec3 getFront() {
		return lookat;
	}

private:
	void updateVectors() {
		vec3 temp;

		temp.x = cos(radians(yaw)) * cos(radians(pitch));
		temp.y = sin(radians(pitch));
		temp.z = sin(radians(yaw)) * cos(radians(pitch));

		lookat = normalize(temp);
		rigth = normalize(cross(lookat, worldUp));
		up = normalize(cross(rigth, lookat));
	}
};

#endif 