#include "headers/Camera.h" // bu yazim kendi yazdigim kodlar icin

Camera::Camera(vec3 _position, vec3 _up, float _yaw, float _pitch)
{
    front = vec3(0.0f, 0.0f, -1.0f);
    mouseSensitivity = SENSITIVITY;
    zoom = ZOOM;

    position = _position;
    worldUp = _up;
    yaw = _yaw;
    pitch = _pitch;

    updateCamera();
}

Camera::Camera(float _posX, float _posY, float _posZ, float _upX, float _upY, float _upZ, float _yaw, float _pitch)
{
    front = vec3(0.0f, 0.0f, -1.0f);
    mouseSensitivity = SENSITIVITY;
    zoom = ZOOM;

    position = vec3(_posX, _posY, _posZ);
    worldUp = vec3(_upX, _upY, _upZ);
    yaw = _yaw;
    pitch = _pitch;

    updateCamera();
}

mat4 Camera::getViewMatrix()
{
    return lookAt(position, position + front, up);
}

void Camera::processKeyboard(const vec3 &direction, float deltaTime)
{
    float velocity = deltaTime;

    // Kameranın sağ ve ileri yönünü hesapla
    vec3 forwardDir = normalize(vec3(front.x, front.y, front.z)); // yukari gidemez biz w tusuna bastigimizda
    vec3 rightDir = normalize(cross(forwardDir, up));

    position += forwardDir * direction.z * velocity;
    position += rightDir * direction.x * velocity;
}

void Camera::processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch)
{
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (constrainPitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        updateCamera();
    }
}

void Camera::updateCamera()
{
    vec3 _front;
    _front.x = cos(radians(yaw)) * cos(radians(pitch));
    _front.y = sin(radians(pitch));
    _front.z = sin(radians(yaw)) * cos(radians(pitch));

    front = normalize(_front);
    right = normalize(cross(front, worldUp));
    up = normalize(cross(right, front));
}

void Camera::processMouseScroll(float yOffset)
{
    zoom -= yOffset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}