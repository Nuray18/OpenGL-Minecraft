#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glad/glad.h>

using namespace glm;

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
    Camera(vec3 _position = vec3(0.0f, 0.0f, 0.0f), vec3 _up = vec3(0.0f, 1.0f, 0.0f), float _yaw = YAW, float _pitch = PITCH);

    Camera(float _posX, float _posY, float _posZ, float _upX, float _upY, float _upZ, float _yaw, float _pitch);

    // getting the view matrix
    mat4 getViewMatrix();

    void processKeyboard(const vec3 &direction, float deltaTime);
    void processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true);
    void processMouseScroll(float yOffeset);

    vec3 position; // Kameranın yeri
    vec3 front;    // Hangi yöne baktığı
    vec3 right;    // Sağ yön
    vec3 up;       // Yukarı yönü
    vec3 worldUp;

    // camera settings
    float yaw;
    float pitch;
    float mouseSensitivity;
    float zoom;

private:
    // updating camera so it can move
    void updateCamera();
};
