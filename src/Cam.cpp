#include "raylib.h"
#include "Cam.h"

Camera2D camera = { 0 };
float cameraSpeed = 300.0f;

void initCamera() {
    camera.offset.x = GetScreenWidth() / 2.0f;
    camera.offset.y = GetScreenHeight() / 2.0f;
    camera.target.x = 0.0f;
    camera.target.y = 0.0f;
    camera.rotation = 0.0f;
    camera.zoom = 4.0f;
}
void kameramoovment() {
    float delta = GetFrameTime();
    
    if (IsKeyDown(KEY_W)) camera.target.y -= cameraSpeed * delta;
    if (IsKeyDown(KEY_S)) camera.target.y += cameraSpeed * delta;
    if (IsKeyDown(KEY_A)) camera.target.x -= cameraSpeed * delta;
    if (IsKeyDown(KEY_D)) camera.target.x += cameraSpeed * delta;
    
    camera.offset.x = GetScreenWidth() / 2.0f;
    camera.offset.y = GetScreenHeight() / 2.0f;
}