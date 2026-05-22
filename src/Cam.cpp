#include "raylib.h"
#include "Cam.h"

Camera2D camera = { 0 };
float cameraSpeed = 300.0f;

static const float BASE_WIDTH  = 1280.0f;
static const float BASE_HEIGHT = 720.0f;
static const float BASE_ZOOM   = 4.0f;

void initCamera() {
    camera.offset.x = GetScreenWidth() / 2.0f;
    camera.offset.y = GetScreenHeight() / 2.0f;
    camera.target.x = 0.0f;
    camera.target.y = 0.0f;
    camera.rotation = 0.0f;
    camera.zoom = BASE_ZOOM;
}

void updateCamera() {
    camera.offset.x = GetScreenWidth()  / 2.0f;
    camera.offset.y = GetScreenHeight() / 2.0f;

    float scaleX = GetScreenWidth()  / BASE_WIDTH;
    float scaleY = GetScreenHeight() / BASE_HEIGHT;
    float scale  = (scaleX < scaleY) ? scaleX : scaleY; 
    camera.zoom  = BASE_ZOOM * scale;
}