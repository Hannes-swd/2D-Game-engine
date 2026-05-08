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
    
    
    
    camera.offset.x = GetScreenWidth() / 2.0f;
    camera.offset.y = GetScreenHeight() / 2.0f;
}