/*
 * Marble Maze - OpenGL Final Project
 * Controls: Arrows=Tilt, WASD=Pan, Q/E=Orbit, Scroll=Zoom, F=Reset, R=Restart,
 * N=Next
 */

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Ball.h"
#include "BoardGenerator.h"
#include "Camera.h"
#include "Config.h"
#include "Level.h"
#include "Mesh.h"
#include "Primitives.h"
#include "Shader.h"
#include "Texture.h"

int screenWidth = 1280, screenHeight = 720;
Camera camera(glm::vec3(0.0f), Config::CAMERA_INITIAL_DISTANCE);

enum class GamePhase { Playing, Won, Failed };
GamePhase gamePhase = GamePhase::Playing;

LevelManager levelManager;
Ball ball;
BoardGenerator::BoardMeshes boardMeshes;
glm::vec2 boardTilt = glm::vec2(0.0f);

float deltaTime = 0.0f, lastFrame = 0.0f;
bool keyW = false, keyA = false, keyS = false, keyD = false, keyQ = false,
     keyE = false;
bool keyUp = false, keyDown = false, keyLeft = false, keyRight = false;

void setupBoard() {
  boardMeshes.floor.cleanup();
  boardMeshes.walls.cleanup();
  boardMeshes.holeMarker.cleanup();
  boardMeshes.frame.cleanup();
  boardMeshes.startMarker.cleanup();
  boardMeshes.goalMarker.cleanup();
  boardMeshes = BoardGenerator::generateBoard(levelManager.getCurrentLevel());
}

void restartLevel() {
  ball.reset(levelManager.getCurrentLevel());
  boardTilt = glm::vec2(0.0f);
  gamePhase = GamePhase::Playing;
}

void framebufferSizeCallback(GLFWwindow *w, int width, int height) {
  screenWidth = width;
  screenHeight = height;
  glViewport(0, 0, width, height);
}

void scrollCallback(GLFWwindow *w, double x, double y) {
  if (ImGui::GetIO().WantCaptureMouse)
    return;
  camera.processZoom(static_cast<float>(y) * Config::CAMERA_ZOOM_SPEED);
}

void keyCallback(GLFWwindow *w, int key, int sc, int action, int mods) {
  if (ImGui::GetIO().WantCaptureKeyboard)
    return;
  bool pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
  if (key == GLFW_KEY_W)
    keyW = pressed;
  if (key == GLFW_KEY_A)
    keyA = pressed;
  if (key == GLFW_KEY_S)
    keyS = pressed;
  if (key == GLFW_KEY_D)
    keyD = pressed;
  if (key == GLFW_KEY_Q)
    keyQ = pressed;
  if (key == GLFW_KEY_E)
    keyE = pressed;
  if (key == GLFW_KEY_UP)
    keyUp = pressed;
  if (key == GLFW_KEY_DOWN)
    keyDown = pressed;
  if (key == GLFW_KEY_LEFT)
    keyLeft = pressed;
  if (key == GLFW_KEY_RIGHT)
    keyRight = pressed;

  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_F) {
      camera.reset();
      camera.Distance = Config::CAMERA_INITIAL_DISTANCE;
      camera.Pitch = Config::CAMERA_INITIAL_PITCH;
    }
    if (key == GLFW_KEY_R)
      restartLevel();
    if (key == GLFW_KEY_N && gamePhase == GamePhase::Won &&
        levelManager.nextLevel()) {
      setupBoard();
      restartLevel();
    }
    if (key == GLFW_KEY_ESCAPE)
      glfwSetWindowShouldClose(w, true);
  }
}

void processInput() {
  float panAmount = Config::CAMERA_PAN_SPEED * camera.Distance;
  if (keyW)
    camera.processPan(0, panAmount * 100);
  if (keyS)
    camera.processPan(0, -panAmount * 100);
  if (keyA)
    camera.processPan(panAmount * 100, 0);
  if (keyD)
    camera.processPan(-panAmount * 100, 0);

  float orbitAmount = Config::CAMERA_ORBIT_SPEED * deltaTime;
  if (keyQ)
    camera.processOrbit(-orbitAmount, 0);
  if (keyE)
    camera.processOrbit(orbitAmount, 0);

  if (gamePhase == GamePhase::Playing) {
    float maxTilt = glm::radians(Config::MAX_TILT_DEGREES);
    float tiltDelta = glm::radians(Config::TILT_SPEED_DEGREES) * deltaTime;
    float returnDelta =
        glm::radians(Config::TILT_RETURN_SPEED_DEGREES) * deltaTime;

    if (keyUp)
      boardTilt.y += tiltDelta;
    if (keyDown)
      boardTilt.y -= tiltDelta;
    if (keyLeft)
      boardTilt.x -= tiltDelta;
    if (keyRight)
      boardTilt.x += tiltDelta;

    boardTilt.x = glm::clamp(boardTilt.x, -maxTilt, maxTilt);
    boardTilt.y = glm::clamp(boardTilt.y, -maxTilt, maxTilt);

    if (!keyLeft && !keyRight) {
      if (boardTilt.x > 0)
        boardTilt.x = std::max(0.0f, boardTilt.x - returnDelta);
      else
        boardTilt.x = std::min(0.0f, boardTilt.x + returnDelta);
    }
    if (!keyUp && !keyDown) {
      if (boardTilt.y > 0)
        boardTilt.y = std::max(0.0f, boardTilt.y - returnDelta);
      else
        boardTilt.y = std::min(0.0f, boardTilt.y + returnDelta);
    }
  }
}

void updateGame() {
  if (gamePhase == GamePhase::Playing) {
    Level &level = levelManager.getCurrentLevel();
    ball.update(deltaTime, boardTilt, level);
    if (ball.hasFallenInHole())
      gamePhase = GamePhase::Failed;
    else if (level.isAtGoal(ball.position, ball.radius))
      gamePhase = GamePhase::Won;
  }
}

void renderUI() {
  ImGui::SetNextWindowPos(ImVec2(10, 10));
  ImGui::SetNextWindowBgAlpha(0.7f);
  ImGui::Begin("Game", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
  ImGui::Text("Level %d / %d", levelManager.currentLevelIndex + 1,
              (int)levelManager.levels.size());
  ImGui::Separator();
  if (gamePhase == GamePhase::Playing) {
    ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "Playing...");
    ImGui::Text("Arrow Keys to tilt");
  } else if (gamePhase == GamePhase::Won) {
    ImGui::TextColored(ImVec4(1.0f, 0.84f, 0.0f, 1.0f), "LEVEL COMPLETE!");
    ImGui::Text(levelManager.hasNextLevel() ? "Press N for next"
                                            : "All levels done!");
  } else {
    ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "FELL IN HOLE!");
    ImGui::Text("Press R to restart");
  }
  ImGui::Separator();
  ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);
  ImGui::End();

  ImGui::SetNextWindowPos(ImVec2(screenWidth - 180.0f, 10));
  ImGui::SetNextWindowBgAlpha(0.5f);
  ImGui::Begin("Controls", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
  ImGui::BulletText("Arrows: Tilt");
  ImGui::BulletText("WASD: Pan");
  ImGui::BulletText("Q/E: Orbit");
  ImGui::BulletText("Scroll: Zoom");
  ImGui::BulletText("F: Reset");
  ImGui::BulletText("R: Restart");
  ImGui::End();
}

int main() {
  if (!glfwInit())
    return -1;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight,
                                        "Marble Maze - PBR", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetScrollCallback(window, scrollCallback);
  glfwSetKeyCallback(window, keyCallback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    return -1;

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 410");

  Shader pbrShader;
  if (!pbrShader.load("assets/shaders/pbr.vert", "assets/shaders/pbr.frag"))
    return -1;

  Texture woodAlbedo, woodNormal, woodARM;
  Texture ballAlbedo, ballNormal, ballARM;
  bool woodTexturesLoaded = false;
  bool ballTexturesLoaded = false;
#ifdef USE_REAL_TEXTURES
  woodTexturesLoaded =
      woodAlbedo.loadFromFile("assets/textures/wood_albedo.png", true);
  woodTexturesLoaded =
      woodTexturesLoaded &&
      woodNormal.loadFromFile("assets/textures/wood_normal.png", false);
  woodTexturesLoaded =
      woodTexturesLoaded &&
      woodARM.loadFromFile("assets/textures/wood_arm.png", false);

  ballTexturesLoaded = ballAlbedo.loadFromFile(
      "assets/textures/green_metal_rust_albedo.png", true);
  ballTexturesLoaded =
      ballTexturesLoaded &&
      ballNormal.loadFromFile("assets/textures/green_metal_rust_normal.png",
                              false);
  ballTexturesLoaded =
      ballTexturesLoaded &&
      ballARM.loadFromFile("assets/textures/green_metal_rust_arm.png", false);
#endif

  levelManager.loadBuiltInLevels();
  setupBoard();
  restartLevel();

  Mesh ballMesh = Primitives::createSphere(Config::BALL_RADIUS, 48, 24);
  camera.Pitch = Config::CAMERA_INITIAL_PITCH;
  camera.Yaw = -90.0f;
  camera.Distance = Config::CAMERA_INITIAL_DISTANCE;

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = std::min(currentFrame - lastFrame, 0.1f);
    lastFrame = currentFrame;

    glfwPollEvents();
    processInput();
    updateGame();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    renderUI();

    glClearColor(0.15f, 0.15f, 0.18f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = (float)screenWidth / (float)screenHeight;
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix(aspect);

    glm::mat4 boardModel = glm::mat4(1.0f);
    boardModel = glm::rotate(boardModel, boardTilt.x, glm::vec3(0, 0, 1));
    boardModel = glm::rotate(boardModel, boardTilt.y, glm::vec3(1, 0, 0));

    pbrShader.use();
    pbrShader.setMat4("view", view);
    pbrShader.setMat4("projection", projection);
    pbrShader.setVec3("camPos", camera.getPosition());
    pbrShader.setVec3("ambientColor", glm::vec3(0.3f));

    pbrShader.setInt("numLights", 2);
    pbrShader.setVec3(
        "lightPositions[0]",
        glm::vec3(Config::LIGHT1_X, Config::LIGHT1_Y, Config::LIGHT1_Z));
    pbrShader.setVec3("lightColors[0]", glm::vec3(1.0f));
    pbrShader.setFloat("lightIntensities[0]", Config::LIGHT1_INTENSITY);
    pbrShader.setInt("lightTypes[0]", 0);
    pbrShader.setVec3(
        "lightPositions[1]",
        glm::vec3(Config::LIGHT2_X, Config::LIGHT2_Y, Config::LIGHT2_Z));
    pbrShader.setVec3("lightColors[1]", glm::vec3(1.0f, 0.95f, 0.9f));
    pbrShader.setFloat("lightIntensities[1]", Config::LIGHT2_INTENSITY);
    pbrShader.setInt("lightTypes[1]", 0);

#ifdef USE_REAL_TEXTURES
    if (woodTexturesLoaded) {
      pbrShader.setBool("useAlbedoMap", true);
      pbrShader.setBool("useNormalMap", true);
      pbrShader.setBool("useARMMap", true);
      woodAlbedo.bind(0);
      woodNormal.bind(1);
      woodARM.bind(2);
      pbrShader.setInt("albedoMap", 0);
      pbrShader.setInt("normalMap", 1);
      pbrShader.setInt("armMap", 2);
    } else
#endif
    {
      pbrShader.setBool("useAlbedoMap", false);
      pbrShader.setBool("useNormalMap", false);
      pbrShader.setBool("useARMMap", false);
    }

    // Floor
    pbrShader.setMat4("model", boardModel);
    pbrShader.setVec3("albedo", glm::vec3(0.6f, 0.45f, 0.28f));
    pbrShader.setFloat("metallic", Config::WOOD_METALLIC);
    pbrShader.setFloat("roughness", Config::WOOD_ROUGHNESS);
    pbrShader.setFloat("ao", 1.0f);
    boardMeshes.floor.draw();

    // Walls
    pbrShader.setVec3("albedo", glm::vec3(0.55f, 0.4f, 0.25f));
    boardMeshes.walls.draw();

    // Frame
    pbrShader.setVec3("albedo", glm::vec3(0.5f, 0.38f, 0.22f));
    // boardMeshes.frame.draw();

#ifdef USE_REAL_TEXTURES
    pbrShader.setBool("useAlbedoMap", false);
    pbrShader.setBool("useNormalMap", false);
    pbrShader.setBool("useARMMap", false);
#endif
    Level &level = levelManager.getCurrentLevel();

    // Holes - render each hole individually
    pbrShader.setVec3("albedo", glm::vec3(0.05f)); // Very dark for holes
    pbrShader.setFloat("metallic", 0.0f);
    pbrShader.setFloat("roughness", 0.95f);
    for (const auto &holePos : level.holePoss) {
      glm::vec3 holeWorldPos = level.gridToWorld(holePos);
      holeWorldPos.y = 0.02f; // Raise above floor
      glm::mat4 holeModel =
          boardModel * glm::translate(glm::mat4(1.0f), holeWorldPos);
      pbrShader.setMat4("model", holeModel);
      boardMeshes.holeMarker.draw();
    }

    // Start marker - slight Y offset to prevent z-fighting
    glm::vec3 startWorldPos = level.gridToWorld(level.startPos);
    startWorldPos.y = 0.02f; // Raise above floor
    glm::mat4 startModel =
        boardModel * glm::translate(glm::mat4(1.0f), startWorldPos);
    pbrShader.setMat4("model", startModel);
    pbrShader.setVec3("albedo", glm::vec3(0.2f, 0.8f, 0.3f));
    pbrShader.setFloat("metallic", 0.0f);
    pbrShader.setFloat("roughness", 0.5f);
    boardMeshes.startMarker.draw();

    // Goal marker - slight Y offset to prevent z-fighting
    glm::vec3 goalWorldPos = level.gridToWorld(level.goalPos);
    goalWorldPos.y = 0.02f; // Raise above floor
    glm::mat4 goalModel =
        boardModel * glm::translate(glm::mat4(1.0f), goalWorldPos);
    pbrShader.setMat4("model", goalModel);
    pbrShader.setVec3("albedo", glm::vec3(1.0f, 0.84f, 0.0f));
    pbrShader.setFloat("metallic", 0.9f);
    pbrShader.setFloat("roughness", 0.3f);
    boardMeshes.goalMarker.draw();

    // Ball - with PBR textures
    glm::mat4 ballModel =
        boardModel * glm::translate(glm::mat4(1.0f), ball.position);
    pbrShader.setMat4("model", ballModel);
#ifdef USE_REAL_TEXTURES
    if (ballTexturesLoaded) {
      pbrShader.setBool("useAlbedoMap", true);
      pbrShader.setBool("useNormalMap", true);
      pbrShader.setBool("useARMMap", true);
      ballAlbedo.bind(0);
      ballNormal.bind(1);
      ballARM.bind(2);
      pbrShader.setInt("albedoMap", 0);
      pbrShader.setInt("normalMap", 1);
      pbrShader.setInt("armMap", 2);
    } else
#endif
    {
      pbrShader.setBool("useAlbedoMap", false);
      pbrShader.setBool("useNormalMap", false);
      pbrShader.setBool("useARMMap", false);
    }
    pbrShader.setVec3("albedo", glm::vec3(0.95f));
    pbrShader.setFloat("metallic", Config::BALL_METALLIC);
    pbrShader.setFloat("roughness", Config::BALL_ROUGHNESS);
    pbrShader.setFloat("ao", 1.0f);
    ballMesh.draw();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }

  boardMeshes.floor.cleanup();
  boardMeshes.walls.cleanup();
  boardMeshes.holeMarker.cleanup();
  boardMeshes.frame.cleanup();
  boardMeshes.startMarker.cleanup();
  boardMeshes.goalMarker.cleanup();
  ballMesh.cleanup();
  woodAlbedo.cleanup();
  woodNormal.cleanup();
  woodARM.cleanup();
  ballAlbedo.cleanup();
  ballNormal.cleanup();
  ballARM.cleanup();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
