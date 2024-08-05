#pragma once
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <vector>

class CameraService
{
  Camera camera;
  float  aspectRatio;

  Quaternion orientation;

  float rotationSpeed = 0.02f;
  float zoomSpeed     = 0.5f;

  public:
  Matrix view;
  Matrix projection;
  
  // negative ar means that the aspect ratio will be calculated from the screen width and height
  CameraService(Vector3 position, Vector3 target, Vector3 up, float fovy, int type, float ar = -1.0f)
  {
    aspectRatio       = ar;
    camera.position   = position;
    camera.target     = target;
    camera.up         = up;
    camera.fovy       = fovy;
    camera.projection = type;
    orientation       = QuaternionIdentity();
    // Initialize view and projection matrices
    view = MatrixLookAt(position, target, up);
    // Update projection matrix
    UpdateProjectionMatrix();
  }

  void Rotate(float angle, Vector3 axis)
  {
    Quaternion q = QuaternionFromAxisAngle(axis, angle);
    orientation  = QuaternionMultiply(orientation, q);
    orientation  = QuaternionNormalize(orientation);
  }
  
  void Zoom(float amount)
  {
    Vector3 direction = Vector3Subtract(camera.target, camera.position);
    direction         = Vector3Normalize(direction);
    camera.position   = Vector3Add(camera.position, Vector3Scale(direction, amount));
  }
  
  void RotateAroundTarget(float dx, float dy)
  {
    // Calculate right and up vectors
    Vector3 forward = Vector3Subtract(camera.target, camera.position);
    Vector3 right   = Vector3Normalize(Vector3CrossProduct(camera.up, forward));
    Vector3 up      = Vector3Normalize(camera.up);

    // Calculate new angles based on mouse movement
    float angleX = dx * 0.05f; // Adjust sensitivity as needed
    float angleY = dy * 0.05f; // Adjust sensitivity as needed

    // Rotate around the Y axis (up vector)
    Quaternion qx = QuaternionFromAxisAngle(up, -angleX);
    forward       = Vector3Transform(forward, QuaternionToMatrix(qx));
    right         = Vector3Transform(right, QuaternionToMatrix(qx));

    // Rotate around the right vector
    Quaternion qy = QuaternionFromAxisAngle(right, -angleY);
    forward       = Vector3Transform(forward, QuaternionToMatrix(qy));

    // Update camera position
    camera.position = Vector3Subtract(camera.target, forward);
  }
  
  void Pan(float dx, float dy)
  {
    Vector3 right = Vector3CrossProduct(camera.up, Vector3Subtract(camera.target, camera.position));
    right         = Vector3Normalize(right);
    Vector3 up    = Vector3Normalize(camera.up);

    camera.position = Vector3Add(camera.position, Vector3Scale(right, -dx));
    camera.position = Vector3Add(camera.position, Vector3Scale(up, dy));

    camera.target = Vector3Add(camera.position, Vector3Subtract(camera.target, camera.position));
  }
  
  void Update()
  {
    Matrix  rotationMatrix = QuaternionToMatrix(orientation);
    Vector3 forward        = {0.0f, 0.0f, -1.0f};
    Vector3 up             = {0.0f, 1.0f, 0.0f};

    forward = Vector3Transform(forward, rotationMatrix);
    up      = Vector3Transform(up, rotationMatrix);

    // camera.target = Vector3Add(camera.position, forward);
    camera.up = up;

    // Update view matrix
    view = MatrixLookAt(camera.position, Vector3Subtract(camera.target, camera.position), camera.up);

    // Update projection matrix
    UpdateProjectionMatrix();
  }
  
  void BeginMode3D() { ::BeginMode3D(camera); }
  
  void EndMode3D() { ::EndMode3D(); }

  void SetTarget(Vector3 target)
  {
    camera.target = target;
    Update();
  }
  
  void SetPosition(Vector3 position)
  {
    camera.position = position;
    Update();
  }
  
  void Begin()
  {
    rlDrawRenderBatchActive();

    rlMatrixMode(RL_PROJECTION);
    rlPushMatrix();
    rlLoadIdentity();

    rlMultMatrixf(MatrixToFloat(projection));

    rlMatrixMode(RL_MODELVIEW);
    rlLoadIdentity();

    rlMultMatrixf(MatrixToFloat(view));

    rlEnableDepthTest();
  }

  void End() { EndMode3D(); }

  void UpdateProjectionMatrix()
  {
    int screenWidth  = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    float ar = aspectRatio < 0.0f ? (float)screenWidth / (float)screenHeight : aspectRatio;

    if(camera.projection == CAMERA_PERSPECTIVE)
      {
        projection = MatrixPerspective(camera.fovy * DEG2RAD, ar, 0.1f, 100.0f);
        ;
      }

    else
      {
        projection = MatrixOrtho(-10.0f * ar, 10.0f * ar, -10.0f, 10.0f, 0.1f, 100.0f);
        ;
      }
  }

  void AddActions()
  {
    float wheelMove = GetMouseWheelMove();

    if(wheelMove != 0.0f)
      {
        Zoom(wheelMove * zoomSpeed); // Adjust zoom speed as needed
      }

    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
      {
        Vector2 mouseDelta = GetMouseDelta();
        RotateAroundTarget(mouseDelta.x, -mouseDelta.y); // Adjust pan speed as needed
      }

    Update();
  }

  float   GetFovy() { return camera.fovy; }
  float   GetAspectRatio() { return aspectRatio; }
  Vector3 GetPosition() { return camera.position; }
  Vector3 GetTarget() { return camera.target; }
};
