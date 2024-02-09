#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>
#include "model.h"
#include "filesystem.h"
#include "camera.h"
#include "shader_m.h"
#include "GUI.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void LoadSkybox(Shader skyboxShader,unsigned int skyboxVAO,unsigned int cubemapTexture);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float rotationMat[3] = {0.0f,0.0f,0.0f};
//float rotationAngleY = 0.0f;
//float rotationAngleZ = 0.0f;
bool cameraDisabled = false;
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool isReflection = true;
bool Usequaternions = false;
bool useXAxis = false;
bool useYAxis = false;
bool useZAxis = false;
float angle = 0.0f;
float AxisUse[3] = {0.0f,0.0f,0.0f};
float eta = 0.1f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float skyboxVertices[] =
{
	//   Coordinates
	-1000.0f, -1000.0f,  1000.0f,//        7--------6
	 1000.0f, -1000.0f,  1000.0f,//       /|       /|
	 1000.0f, -1000.0f, -1000.0f,//      4--------5 |
	-1000.0f, -1000.0f, -1000.0f,//      | |      | |
	-1000.0f,  1000.0f,  1000.0f,//      | 3------|-2
	 1000.0f,  1000.0f,  1000.0f,//      |/       |/
	 1000.0f,  1000.0f, -1000.0f,//      0--------1
	-1000.0f,  1000.0f, -1000.0f
};

unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);



    // tell GLFW to capture our mouse
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);

    GUI guiManager;
    guiManager.gui_init(window);
    GLfloat backGroundColor[3] = {0.3f,0.2f,0.3f};
    // configure global opengl state
    // -----------------------------

    // build and compile shaders
    // -------------------------
    Shader ourShader("1.model_loading.vs", "1.model_loading.fs");
    Shader ourShader2("1.model_loading.vs", "1.model_loading.fs");
    Shader ourShader3("1.model_loading_dispersion.vs", "1.model_loading_dispersion.fs");
    // load models
    // -----------
    Model ourModel(FileSystem::getPath("Resources/teapot/teapot.obj"));
    Model ourModel2(FileSystem::getPath("Resources/teapot/teapot.obj"));
    Model ourModel3(FileSystem::getPath("Resources/teapot/teapot.obj"));
    glm::vec3 lightDir(-0.2f, 1.0f, -0.3f);


    Shader skyboxShader("skybox.vert", "skybox.frag");
    skyboxShader.use();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);
    // Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

//render loop
    //-----------
   unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// All the faces of the cubemap (make sure they are in this exact order)
	std::string facesCubemap[6] =
	{
		 FileSystem::getPath("Resources/Cubemap/right.jpg").c_str(),
		FileSystem::getPath("Resources/Cubemap/left.jpg").c_str(),
		FileSystem::getPath("Resources/Cubemap/top.jpg").c_str(),
		FileSystem::getPath("Resources/Cubemap/bottom.jpg").c_str(),
		FileSystem::getPath("Resources/Cubemap/front.jpg").c_str(),
		FileSystem::getPath("Resources/Cubemap/back.jpg").c_str()
	};
// Creates the cubemap texture object
	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// This might help with seams on some systems
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

         // input
        // -----
        processInput(window);
        if (cameraDisabled)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else{
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        guiManager.BeginFrame();
        ImGui::Begin("Settings");
        //ImGui::ColorEdit3("Background Color", backGroundColor);
        //ImGui::Checkbox("Use Quaternion", &Usequaternions);
        if(Usequaternions)
        {
            ImGui::Checkbox("X",&useXAxis);
            ImGui::Checkbox("Y",&useYAxis);
            ImGui::Checkbox("Z",&useZAxis);
            ImGui::SliderFloat("Angle", &angle,0.0f,360.0f);
        }
        ImGui::End();
        if(useXAxis)
        {
            AxisUse[0] = 1.0f;
        }
        if(useYAxis){
            AxisUse[1] =  1.0f;
        }
        if(useZAxis)
        {
            AxisUse[2] = 1.0f;
        }
        // render
        // ------
        glClearColor(backGroundColor[0],backGroundColor[1],backGroundColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        LoadSkybox(skyboxShader,skyboxVAO,cubemapTexture);
        // don't forget to enable shader before setting uniforms
        ourShader.use();
        //glm::quat rotation = glm::quat(1.0f,0.0f,0.0f,0.0f);
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setVec3("camPos",camera.Position);
        ourShader.setBool("isReflection",isReflection);
        ourShader.setFloat("eta",eta);
        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-5.0f, 0.0f, -3.0f));
        model = glm::rotate(model,rotationMat[0],glm::vec3(0.0f,1.0f,0.0f));
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);
        isReflection = false;

        ourShader2.use();
        //glm::quat rotation = glm::quat(1.0f,0.0f,0.0f,0.0f);
        // view/projection transformations
        glm::mat4 projection2 = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view2 = camera.GetViewMatrix();
        ourShader2.setMat4("projection", projection2);
        ourShader2.setMat4("view", view2);
        ourShader2.setVec3("camPos",camera.Position);
        ourShader2.setBool("isReflection",isReflection);
        ourShader2.setFloat("eta",eta);
        // render the loaded model
        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, glm::vec3(0.0f, 0.0f, -3.0f));
        model2 = glm::rotate(model2,rotationMat[0],glm::vec3(0.0f,1.0f,0.0f));
        ourShader2.setMat4("model", model2);
        ourShader2.setVec3("lightDir", lightDir);
        ourModel2.Draw(ourShader2);
        isReflection = false;


        ourShader3.use();
        //glm::quat rotation = glm::quat(1.0f,0.0f,0.0f,0.0f);
        // view/projection transformations
        glm::mat4 projection3 = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view3 = camera.GetViewMatrix();
        ourShader3.setMat4("projection", projection3);
        ourShader3.setMat4("view", view3);
        ourShader3.setVec3("camPos",camera.Position);
        // render the loaded model
        glm::mat4 model3 = glm::mat4(1.0f);
        model3 = glm::translate(model3, glm::vec3(5.0f, 0.0f, -3.0f));
        model3 = glm::rotate(model3,rotationMat[0],glm::vec3(0.0f,1.0f,0.0f));
        ourShader3.setMat4("model", model3);
        ourShader3.setVec3("lightDir", lightDir);
        ourModel3.Draw(ourShader3);
        guiManager.EndFrame();
        if(rotationMat[0]<360.0f)
        {
            rotationMat[0] += 0.005f;
        }
        else{
            rotationMat[0] = 0.0f;
        }
        isReflection = true;
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
     glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window,GLFW_KEY_C) == GLFW_PRESS)
    {
        if(cameraDisabled == true)
        {
            cameraDisabled = false;
        }
        else{
            cameraDisabled = true;
        }
    }
    // if(cameraDisabled == true)
    // {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    // }

    // Adjust the rotation angle based on user input (e.g., arrow keys)
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        rotationMat[1] += 0.005f;
    }

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        rotationMat[0] += 0.005f;


    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        rotationMat[2] += 0.005f;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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

    if(!cameraDisabled)
    {
         camera.ProcessMouseMovement(xoffset, yoffset);
    }

}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void LoadSkybox(Shader skyboxShader,unsigned int skyboxVAO,unsigned int cubemapTexture){
    // Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
    glDepthFunc(GL_LEQUAL);

    skyboxShader.use();
    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    glm::mat4 projection = glm::mat4(1.0f);
    // We make the mat4 into a mat3 and then a mat4 again in order to get rid of the last row and column
    // The last row and column affect the translation of the skybox (which we don't want to affect)
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Draws the cubemap as the last object so we can save a bit of performance by discarding all fragments
    // where an object is present (a depth of 1.0f will always fail against any object's depth value)
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Switch back to the normal depth function
    glDepthFunc(GL_LESS);

}
