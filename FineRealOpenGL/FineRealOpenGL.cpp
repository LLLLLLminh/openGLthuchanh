#include "ModelLoader.h"
#include "Skybox.h"
#include "LightSource.h"
#include "Spawner.h"

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void processInput(GLFWwindow* window);

bool firstMouse = true;
bool isStopped = false;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX, lastY;
double cosTime, sinTime;
float simTime = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    SCR_WIDTH = mode->width; //800
    SCR_HEIGHT = mode->height; //600

    lastX = SCR_WIDTH / 2.0f;
    lastY = SCR_HEIGHT / 2.0f;
    

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "PhoCo", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //3D
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_FRAMEBUFFER_SRGB);

    ModelLoader scene = ModelLoader("asset/PhoCo/PhoCo.obj");
    scene.SetUpPos(glm::vec3(5.0f, 0.0f, 0.0f), 0.0f);
	ModelLoader bike = ModelLoader("asset/Veh/bike.obj");
	ModelLoader car = ModelLoader("asset/Veh/car.obj");

    //Seed random
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

	Spawner spawner = Spawner(glm::vec3(-45, 1, -17), 0);
	spawner.modelList.push_back(&bike);
    spawner.modelList.push_back(&car);

    Spawner spawner2 = Spawner(glm::vec3(-65, 1, 23), 180);
    spawner2.modelList.push_back(&car);
    spawner2.modelList.push_back(&bike);

    std::vector<std::string> facesDay
    {
        "asset/Skybox/Daylight Box_Right.bmp", //R
        "asset/Skybox/Daylight Box_Left.bmp",  //L
        "asset/Skybox/Daylight Box_Top.bmp",  //T
        "asset/Skybox/Daylight Box_Bottom.bmp",  //B
        "asset/Skybox/Daylight Box_Front.bmp",  //F
        "asset/Skybox/Daylight Box_Back.bmp"  //B
    };
    Skybox skyboxDay(facesDay);

    stbi_set_flip_vertically_on_load(true);
    std::vector<std::string> facesNight
    {
        "asset/Skybox/SkyboxNight_Right.png", //R
        "asset/Skybox/SkyboxNight_Left.png",  //L
        "asset/Skybox/SkyboxNight_Top.png",  //T
        "asset/Skybox/SkyboxNight_Bottom.png",  //B
        "asset/Skybox/SkyboxNight_Front.png",  //F
        "asset/Skybox/SkyboxNight_Back.png"  //B
    };
    Skybox skyboxNight(facesNight);
    stbi_set_flip_vertically_on_load(false);

    LightSource sunlight;
    std::vector<LightSource> streetlights(2);
    glm::vec3 noonColor = glm::vec3(1.0f, 1.0f, 0.95f);
    glm::vec3 sunsetColor = glm::vec3(1.0f, 0.7f, 0.45f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (!isStopped) {
            simTime += deltaTime;
        }

        cosTime = glm::cos(simTime / 5.0f);
        sinTime = glm::sin(simTime / 5.0f);
        
        glm::vec3 dirLightDirection = glm::normalize(
            glm::vec3(cosTime, sinTime, 0.5f)
        );
        float sunHeight = glm::clamp(dirLightDirection.y, -1.0f, 1.0f);

        float daylightFactor = glm::clamp((sunHeight + 0.2f) / 0.6f, 0.0f, 1.0f);
        glm::vec3 sunColor = glm::mix(sunsetColor, noonColor, daylightFactor);

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        bool isDay = (sunHeight >= 0);

        scene.RenderModel(&camera, dirLightDirection, streetlights, isDay);

		spawner.Update(&camera, dirLightDirection, streetlights, isDay, isStopped);
        spawner2.Update(&camera, dirLightDirection, streetlights, isDay, isStopped);

        sunlight.use(&camera, 200.0f * dirLightDirection, glm::vec3(10), glm::vec3(0));
		streetlights[0].use(&camera, glm::vec3(-10.2, 27.2, -33), glm::vec3(0.5, 0.1, 1.2), glm::vec3(1.0f, 0.022f, 0.0019f)); //dist: 200
        streetlights[1].use(&camera, glm::vec3(18.7, 14, 24.6), glm::vec3(0.2, 0.1, 0.2), glm::vec3(1.0f, 0.09f, 0.032f)); //dist: 50

        if (isDay) {
            skyboxDay.use(&camera, sunHeight, daylightFactor);
			scene.UpdateLightColor(sunColor);
            car.UpdateLightColor(sunColor);
            bike.UpdateLightColor(sunColor);
            sunlight.lightColor = sunColor;
        }
        else {
            skyboxNight.use(&camera, sunHeight, daylightFactor);
            scene.UpdateLightColor(glm::vec3(1.0f, 1.0f, 0.95f));
            car.UpdateLightColor(glm::vec3(1.0f, 1.0f, 0.95f));
            bike.UpdateLightColor(glm::vec3(1.0f, 1.0f, 0.95f));
        }
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    float cameraSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}
// glfw: whenever a key is pressed, this callback is called
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;

        case GLFW_KEY_LEFT_CONTROL:
            isStopped = !isStopped;   // toggle pause
            break;
        }
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}