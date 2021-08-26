#pragma once
#include "Functions.h";
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <windows.h>
#include <vector>
static std::vector<float> MeshVertexDataContainer;
static std::vector<float> MeshColorDataContainer;
static std::vector<unsigned int> indices;
static int MeshCount = 0;

static int drawMode = 0;

struct Vector2
{
    float x;
    float y;
    Vector2(float x, float y)
    {
        this->x = x;
        this->y = y;
    }
    Vector2() {}
    Vector2 operator+(Vector2 other)
    {
        Vector2 result;
        result.x = this->x + other.x;
        result.y = this->y + other.y;
        return result;
    }
    Vector2 operator-(Vector2 other)
    {
        Vector2 result;
        result.x = this->x - other.x;
        result.y = this->y - other.y;
        return result;
    }
    bool operator==(Vector2 other)
    {
        return (this->x == other.x && this->y == other.y);
    }
};
struct Vector3
{
    float x;
    float y;
    float z;
    Vector3(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    Vector3() {}
    Vector3 operator+(Vector3 other)
    {
        Vector3 result;
        result.x = this->x + other.x;
        result.y = this->y + other.y;
        result.z = this->z + other.z;
        return result;
    }
    Vector3 operator-(Vector3 other)
    {
        Vector3 result;
        result.x = this->x - other.x;
        result.y = this->y - other.y;
        result.z = this->z - other.z;
        return result;
    }
    bool operator==(Vector3 other)
    {
        return (this->x == other.x && this->y == other.y && this->z == other.z);
    }
};
struct Mesh
{
    float vertex_positions[4*2] =
    {
     //   x      y
        -0.5f, -0.5f,//bottom left
        -0.5f,  0.5f,//top left
         0.5f, -0.5f,//bottom right
         0.5f,  0.5f //top right
    };
    float colors[12] =
    {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f
    };
    Vector2 Position = Vector2(0, 0);
    Vector2 Scale = Vector2(0, 0);
    int MeshId = -1;

    Mesh(const Vector2& Position, const Vector3& Colour, const Vector2& Scale = Vector2(1, 1))
    {
        this->Scale = Scale;
        SetScale();
        SetPosition(Position);
        SetColor(Colour.x, Colour.y, Colour.z);
        for (int j = 0; j < 8; j++)
            MeshVertexDataContainer.push_back(vertex_positions[j]);
        #pragma region Adding Mesh To The Vertex Index
        {
            int startindex = 0;
            if (MeshCount != 0)
                startindex = (MeshCount * 4);
            for (int j = startindex; j < startindex + 6; j++)
            {
                if (j >= (startindex + 3))
                    indices.push_back(j - 2);
                else
                    indices.push_back(j);
            }
        }
        #pragma endregion
        MeshId = MeshCount;
        MeshCount++;
    }
    void SetColor(const float r, const float g, const float b)
    {
        for (int j = 0; j < 10; j += 3)
        {
            colors[j] = r;
            colors[j + 1] = g;
            colors[j + 2] = b;
        }
        for (int j = 0; j < 12; j++)
        {
            MeshColorDataContainer.push_back(colors[j]);
        }
    }
    void SetPosition(Vector2 Position)
    {
        if (this->Position == Position)
            return;

        Vector2 Difference = (Position - this->Position);
        vertex_positions[0] += Difference.x; vertex_positions[2] += Difference.x; vertex_positions[4] += Difference.x; vertex_positions[6] += Difference.x;
        vertex_positions[1] += Difference.y; vertex_positions[3] += Difference.y; vertex_positions[5] += Difference.y; vertex_positions[7] += Difference.y;
        RefreshVertexPositions();
        this->Position = Position;
    }
    void SetScale()
    {                                
        vertex_positions[0] *= Scale.x; vertex_positions[2] *= Scale.x; vertex_positions[4] *= Scale.x; vertex_positions[6] *= Scale.x;
        vertex_positions[1] *= Scale.y; vertex_positions[3] *= Scale.y; vertex_positions[5] *= Scale.y; vertex_positions[7] *= Scale.y;
        RefreshVertexPositions();
    }
    void RemoveScale()
    {
        vertex_positions[0] /= Scale.x; vertex_positions[2] /= Scale.x; vertex_positions[4] /= Scale.x; vertex_positions[6] /= Scale.x;
        vertex_positions[1] /= Scale.y; vertex_positions[3] /= Scale.y; vertex_positions[5] /= Scale.y; vertex_positions[7] /= Scale.y;
        RefreshVertexPositions();
    }
    void RefreshVertexPositions()
    {
        if (MeshId != -1)//If Mesh is defined before
        {
            int start_index = 0;
            if (MeshId > 0)
                start_index = ((MeshId) * 7) + MeshId;
            MeshVertexDataContainer[start_index + 0] = vertex_positions[0];
            MeshVertexDataContainer[start_index + 1] = vertex_positions[1];
            MeshVertexDataContainer[start_index + 2] = vertex_positions[2];
            MeshVertexDataContainer[start_index + 3] = vertex_positions[3];
            MeshVertexDataContainer[start_index + 4] = vertex_positions[4];
            MeshVertexDataContainer[start_index + 5] = vertex_positions[5];
            MeshVertexDataContainer[start_index + 6] = vertex_positions[6];
            MeshVertexDataContainer[start_index + 7] = vertex_positions[7];

        }
    }
};