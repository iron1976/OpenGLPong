#define GLEW_STATIC
#include "Functions.h";
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp";
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <windows.h>
#include <vector>
#include "Mesh.h";

#pragma region shader
static const std::string vertexShader =
"#version 330 core\n"
"\n"
"layout(location = 0) in vec3 position;\n"
"layout(location = 1) in vec3 Color;\n"
"uniform mat4 view;\n"
"out vec4 rescolor;\n"
"\n" 
"void main()\n"
"{\n"
" gl_Position = view*vec4(position, 1.0f);\n"
" rescolor = vec4(Color, 1.0f);\n"
"}\n";
static const std::string fragmentShader =
"#version 330 core\n"
"\n"
"layout(location = 0) out vec4 color;\n"
"in vec4 rescolor;\n"
"\n"
"void main()\n"
"{\n"
" color = rescolor;\n"
"}\n";
#pragma endregion shader
static unsigned int CompileShader(unsigned int type, const std::string source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    
    int result = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == 0)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        print((std::string)"Failed to compile" + (std::string)(type == GL_VERTEX_SHADER ? "vertex" : "fragment"));
        print((std::string)"error:"+ (std::string)message);
        glDeleteShader(id);
        return 0;
    }
    return id;
}
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

#define WIDTH 640.0f
#define HEIGHT 480.0f

static const float cameraViewSpeed = 0.2f;

int main()
{
    #pragma region Creating & Initializing OpenGL
    GLFWwindow* window;
    if (!glfwInit())
        return -1;


    window = glfwCreateWindow(640, 480, "Pong by iron76", NULL, NULL);
    
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (glewInit() != GLEW_OK)
        return -2;
    #pragma endregion
    Mesh object_1(Vector2( 0.8f, 0.0f), Vector3(0, 1, 0), Vector2(0.01f, 0.6f));

    Mesh object_2(Vector2(-0.8f, 0.0f), Vector3(0, 0, 1), Vector2(0.01f, 0.6f));

    Mesh object_3(Vector2( 1.0f, 0.0f), Vector3(1, 0, 0), Vector2(0.05f * ((HEIGHT / WIDTH)), 0.05f ));

    for (int j = 0; j < MeshVertexDataContainer.size(); j++)
        print("MeshVertexDataContainer: " << MeshVertexDataContainer[j]);

    for (int j = 0; j < MeshColorDataContainer.size(); j++)
        print("MeshColorDataContainer: " << MeshColorDataContainer[j]);

    for (int j = 0; j < indices.size(); j++)
        print("indices: " << indices[j]);

    Vector2 ballPositionIncrement(0.01f, 0.01f);
    const float IncrementPerBounce = 0.01f;
    int timesCollided = 0;
    #pragma region Buffers&Attribs...
    unsigned int shader;
    unsigned int VertexBuffer;
    unsigned int ColorBuffer;
    unsigned int VertexIndexBuffer;
    glm::mat4 trans = glm::mat4(1.0f);
    {
        CheckErrorGL(glGenBuffers(1, &VertexBuffer));
        CheckErrorGL(glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer));
        CheckErrorGL(glNamedBufferData(VertexBuffer, 56 * sizeof(float) * 7, nullptr, GL_DYNAMIC_DRAW));
        glEnableVertexAttribArray(0);
        CheckErrorGL(glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), (void*)0));
        
        CheckErrorGL(glGenBuffers(2, &ColorBuffer));
        CheckErrorGL(glBindBuffer(GL_ARRAY_BUFFER, ColorBuffer));
        CheckErrorGL(glNamedBufferData(ColorBuffer, sizeof(float) * MeshColorDataContainer.size(), &MeshColorDataContainer[0], GL_STATIC_DRAW));
        glEnableVertexAttribArray(1);
        CheckErrorGL(glVertexAttribPointer(1, 3, GL_FLOAT, false, 3 * sizeof(float), (void*)0));

        CheckErrorGL(glGenBuffers(1, &VertexIndexBuffer));
        CheckErrorGL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexIndexBuffer));
        CheckErrorGL(glNamedBufferData(VertexIndexBuffer, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW));

        shader = CreateShader(vertexShader, fragmentShader);
        CheckErrorGL(glUseProgram(shader));
        trans = glm::rotate(trans, glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));
        trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false, glm::value_ptr(trans));
    }
    #pragma endregion
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

        glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * MeshVertexDataContainer.size(), &MeshVertexDataContainer[0]);
        glDrawElements(GL_TRIANGLES, indices.size()* sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        object_3.SetPosition(object_3.Position + ballPositionIncrement);

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * MeshVertexDataContainer.size(), &MeshVertexDataContainer[0]);
        #pragma region Camera Movement
        if (glfwGetKey(window, GLFW_KEY_SPACE))
        {
            if (glfwGetKey(window, GLFW_KEY_D))
                trans = glm::translate(trans, glm::vec3(-cameraViewSpeed, 0.0f, 0.0f));
            if (glfwGetKey(window, GLFW_KEY_A))
                trans = glm::translate(trans, glm::vec3( cameraViewSpeed, 0.0f, 0.0f));
            if (glfwGetKey(window, GLFW_KEY_W))
                trans = glm::translate(trans, glm::vec3(0.0f,-cameraViewSpeed, 0.0f));
            if (glfwGetKey(window, GLFW_KEY_S))
                trans = glm::translate(trans, glm::vec3(0.0f, cameraViewSpeed, 0.0f));
            
            glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false, glm::value_ptr(trans));
        }
        #pragma endregion
        #pragma region Keyboard Inputs
        else
        {
            if (glfwGetKey(window, GLFW_KEY_UP) && object_1.vertex_positions[3] < 0.9f)
                object_1.SetPosition(object_1.Position + Vector2( 0.0f, 0.1f));
            if (glfwGetKey(window, GLFW_KEY_DOWN) && object_1.vertex_positions[1] > -0.9f)
                object_1.SetPosition(object_1.Position + Vector2( 0.0f, -0.1f));

            if (glfwGetKey(window, GLFW_KEY_W) && object_2.vertex_positions[3] < 0.9f)
                object_2.SetPosition(object_2.Position + Vector2(0.0f, 0.1f));
            if (glfwGetKey(window, GLFW_KEY_S) && object_2.vertex_positions[1] > -0.9f)
                object_2.SetPosition(object_2.Position + Vector2(0.0f, -0.1f));

  /*          if (glfwGetKey(window, GLFW_KEY_A))        
                object_3.SetPosition(object_3.Position + Vector2(-0.1f,  0.0f));
            if (glfwGetKey(window, GLFW_KEY_D))
                object_3.SetPosition(object_3.Position + Vector2( 0.1f, -0.0f));
            if (glfwGetKey(window, GLFW_KEY_W))
                object_3.SetPosition(object_3.Position + Vector2( 0.0f,  0.1f));
            if (glfwGetKey(window, GLFW_KEY_S))
                object_3.SetPosition(object_3.Position + Vector2( 0.0f, -0.1f));*/

        }
        #pragma endregion
        #pragma region Collisions
        {
            #pragma region Collision Between Ball & BlueBar
            if (object_3.vertex_positions[0] < object_2.vertex_positions[4] && //bottom left x
                object_3.vertex_positions[1] > object_2.vertex_positions[5] && //bottom left y
                object_3.vertex_positions[0] > object_2.vertex_positions[2] &&
                object_3.vertex_positions[1] < object_2.vertex_positions[3] ||
                //Checking Collision For Fourth Corner
                object_3.vertex_positions[6] < object_2.vertex_positions[4] &&
                object_3.vertex_positions[7] > object_2.vertex_positions[5] &&
                object_3.vertex_positions[6] > object_2.vertex_positions[2] &&
                object_3.vertex_positions[7] < object_2.vertex_positions[3] &&
                true)
            {
                print("collided " << timesCollided);
                ballPositionIncrement.x *= -1;
                timesCollided++;
            }
            #pragma endregion
            #pragma region Collision Between Ball & RedBar
            else if (object_3.vertex_positions[0] < object_1.vertex_positions[4] && //bottom left x
                object_3.vertex_positions[1] > object_1.vertex_positions[5] && //bottom left y
                object_3.vertex_positions[0] > object_1.vertex_positions[2] &&
                object_3.vertex_positions[1] < object_1.vertex_positions[3] ||
                //Checking Collision For Fourth Corner
                object_3.vertex_positions[6] < object_1.vertex_positions[4] &&
                object_3.vertex_positions[7] > object_1.vertex_positions[5] &&
                object_3.vertex_positions[6] > object_1.vertex_positions[2] &&
                object_3.vertex_positions[7] < object_1.vertex_positions[3] &&
                true)
            {
                print("collided with red bar" << timesCollided);
                ballPositionIncrement.x *= -1;
                timesCollided++;
            }
            #pragma endregion
            #pragma region Collision Between Ball & WindowBorder
            else if ((object_3.vertex_positions[1] >= 1) 
                || (object_3.vertex_positions[3] >= 1) 
                || (object_3.vertex_positions[5] >= 1) 
                || (object_3.vertex_positions[7] >= 1))
            {
                ballPositionIncrement.y *= -1;
                timesCollided++;
            }
            else if ((object_3.vertex_positions[1] <= -1)
                || (object_3.vertex_positions[3] <= -1)
                || (object_3.vertex_positions[5] <= -1)
                || (object_3.vertex_positions[7] <= -1))
            {
                print("bounce off " << timesCollided);
                ballPositionIncrement.y *= -1;
                timesCollided++;                        
            }
            else if (object_3.vertex_positions[0] <= -1 ||
                object_3.vertex_positions[0] >= 1 ||
                object_3.vertex_positions[2] <= -1 ||
                object_3.vertex_positions[2] >= 1 ||
                object_3.vertex_positions[4] <= -1 ||
                object_3.vertex_positions[4] >= 1 ||
                object_3.vertex_positions[6] <= -1 ||
                object_3.vertex_positions[6] >= 1
                )
            {
                object_3.SetPosition(Vector2(0, 0));
                print("DEAD... " << timesCollided);
                timesCollided++;
            }
            #pragma endregion
        }
        #pragma endregion
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}