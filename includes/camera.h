#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "common.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Default camera values
constexpr float YAW = -90.0f;
constexpr float PITCH = -50.0f;
constexpr float SPEED = 7.0f;
constexpr float SENSITIVITY = 0.1f;
constexpr float ZOOM = 45.0f;
constexpr glm::vec3 CAMERA_POSITION = glm::vec3(0.0f, 50.0f, 40.0f);
constexpr glm::vec3 CAMERA_UP = glm::vec3(0.0f, 1.0f, 0.0f);

class Camera
{
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    float yaw;
    float pitch;
    float movementSpeed;
    float mouseSensitivity;
    float zoom;

    Camera(
        glm::vec3 position = CAMERA_POSITION,
        glm::vec3 up = CAMERA_UP,
        float yaw = YAW,
        float pitch = PITCH
    );
    Camera(float posX, float posY, float posZ,
        float upX, float upY, float upZ,
        float yaw, float pitch
    );
    glm::mat4 getViewMatrix();
    void processKeyboard(Camera_Movement direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void processMouseScroll(float yoffset);
    void reset();
    void invertPitch();
    void rotateCamera(float theta);
private:
    void updateCameraVectors();
};
#endif  // __CAMERA_H__