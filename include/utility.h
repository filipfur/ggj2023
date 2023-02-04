#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <iostream>

namespace utility
{
    glm::vec3 directionVector(float radians);

    float lerp(float a, float b, float f);

    glm::vec3 lerp(const glm::vec3 &a, const glm::vec3 &b, float t);

    float maxf(float a, float b);

    float minf(float a, float b);

    float clamp(float f, float a, float b);

    void printMatrix(const glm::mat4 &matrix);

    template <typename T>
    void printGLM(const T &vec, int n)
    {
        std::string delim{""};
        const float *pSource = (const float *)glm::value_ptr(vec);
        for (int i{0}; i < n; ++i)
        {
            std::cout << delim << pSource[i];
            delim = ", ";
        }
    }

    void printVector(const glm::vec2 &vec);
    void printVector(const glm::vec3 &vec);

    std::string hexStr(char *data, int len);

    float angle(const glm::vec3 &p0, const glm::vec3 &p1);

    static const float pi{3.14159265359f};
    static const float pi2{pi * 2.0f};
    static const float pih{pi * 0.5f};
}