#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include "lua/lua.hpp"

#ifdef _WIN32
#pragma comment(lib, "C:/Lib/lua54")
#endif

#define pi 3.14159265
#define DOWN GLFW_PRESS

namespace shorty {
    float square[] = {
        // positions         // texture coords
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f, // top right
         1.0f, -1.0f, -1.0f,  1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, // top left 
    };
    float cuboid[] = {
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,

        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,

        -1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,  1.0f, 0.0f,

         1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
         1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 0.0f,

        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
         1.0f, -1.0f, -1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,

        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f
    };
    struct shape {
        unsigned int VBO;
        unsigned int VAO;
        unsigned int VSIZE;
        shape(float* vertices, unsigned int vsize) {
            VSIZE = vsize;
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vsize, vertices, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
        }
        shape() {}
        void draw() {
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, VSIZE);
        }
    };

    int makeShader(GLenum type, const GLchar** source) {
        unsigned int shader;
        shader = glCreateShader(type);
        glShaderSource(shader, 1, source, NULL);
        glCompileShader(shader);
        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cout << "ShaderError:\n" << infoLog << std::endl;
            return -1;
        }
        return shader;
    }

    int makeProgram(unsigned int vertex, unsigned int fragment) {
        unsigned int program;
        program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
        int success;
        char infoLog[512];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(program, 512, NULL, infoLog);
            std::cout << "ProgramError:\n" << infoLog << std::endl;
            return -1;
        }
        return program;
    }
    int getUniform(unsigned int shader,const char* string) {
        return glGetUniformLocation(shader,string);
    }
    void dropShader(unsigned int shader) {
        glDeleteShader(shader);
    }
    void runProgram(unsigned int program) {
        glUseProgram(program);
    }
    void wipeWindow(/*window*/) {
        glClear(GL_COLOR_BUFFER_BIT);
    }
    void slapWindow(GLFWwindow* window) {
        glfwSwapBuffers(window);
    }
    bool keyDown(GLFWwindow* window,int key) {
        return glfwGetKey(window, key) == GLFW_PRESS;
    }
    void sleep(unsigned int time) {
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
    }
    namespace lua {
        void outstack(lua_State* L) {
            int i;
            int top = lua_gettop(L);
            for (i = 1; i <= top; i++) {  /* repeat for each level */
                int t = lua_type(L, i);
                std::cout << i << ": ";
                switch (t) {

                case LUA_TNIL:
                    printf("nil");

                case LUA_TSTRING:  /* strings */
                    printf("'%s'", lua_tostring(L, i));
                    break;

                case LUA_TBOOLEAN:  /* booleans */
                    printf(lua_toboolean(L, i) ? "true" : "false");
                    break;

                case LUA_TNUMBER:  /* numbers */
                    printf("%g", lua_tonumber(L, i));
                    break;

                case LUA_TLIGHTUSERDATA:
                    printf("lightuserdata");

                case LUA_TUSERDATA:
                    printf("userdata");

                default:  /* other values */
                    printf("%s", lua_typename(L, t));
                    break;

                }
                printf(",\n");  /* put a separator */
            }
            printf("\n");  /* end the listing */
        }
    }
}
namespace math {
    template<typename part> struct raw_vec3 {
        part x;
        part y;
        part z;
        raw_vec3(part nx, part ny, part nz) {
            x = nx;
            y = ny;
            z = nz;
        }
        raw_vec3 operator*(part v) {
            return raw_vec3<part>(v * x, v * y, v * z);
        }
        raw_vec3 operator/(part v) {
            return *this * (1 / v);
        }
        raw_vec3 operator+(raw_vec3<part> v) {
            return raw_vec3<part>(v.x + x, v.y + y, v.z + z);
        }
        raw_vec3 operator-(raw_vec3<part> v) {
            return raw_vec3<part>(v.x - x, v.y - y, v.z - z);
        }
        part mag() {
            return sqrt(x * x + y * y + z * z);
        }
        raw_vec3<part> norm() {
            return *this / mag();
        }
        raw_vec3<part> neg() {
            return raw_vec3<part>(-x, -y, -z);
        }
        void out() {
            std::cout << x << ", " << y << ", " << z << "\n";
        }
    };
    template<typename part> struct raw_vec4 {
        part x = 0;
        part y = 0;
        part z = 0;
        part w = 1;
        raw_vec4(part nx, part ny, part nz, part nw) {
            x = nx;
            y = ny;
            z = nz;
            w = nw;
        }
        raw_vec4(raw_vec3<part> xyz, part nw) {
            x = xyz.x;
            y = xyz.y;
            z = xyz.z;
            w = nw;
        }
        raw_vec4() {}
        raw_vec4 operator*(part v) {
            return raw_vec4<part>(v * x, v * y, v * z, v * w);
        }
        raw_vec4 operator/(part v) {
            return *this * (1 / v);
        }
        raw_vec4 operator+(raw_vec4<part> v) {
            return raw_vec4<part>(v.x + x, v.y + y, v.z + z, v.w + w);
        }
        part mag() {
            return sqrt(x * x + y * y + z * z + w * w);
        }
        raw_vec4 norm() {
            return *this / mag();
        }
        void out() {
            std::cout << x << ", " << y << ", " << z << ", " << w << "\n";
        }
        raw_vec3<part> vec3() {
            return raw_vec3<part>(x, y, z);
        }
    };
    template<typename part> struct raw_vec2 {
        part x;
        part y;
    };
    using vec4 = raw_vec4<float>;
    using vec3 = raw_vec3<float>;
    using vec2 = raw_vec2<float>;
    using f = float;
    struct mat4 {
        vec4 right = vec4(1,0,0,0);
        vec4 up    = vec4(0,1,0,0);
        vec4 look  = vec4(0,0,1,0);
        vec4 pos   = vec4(0,0,0,1);
        mat4(f a,f b,f c,f d,f e,f _f,f g,f h,f i,f j,f k,f l,f m,f n,f o,f p) {
            right = vec4(a, b, c, d);
            up = vec4(e, _f, g, h);
            look = vec4(i, j, k, l);
            pos = vec4(m, n, o, p);
        }
        mat4() {}
        mat4(vec4 nright,vec4 nup,vec4 nlook,vec4 npos) {
            right = nright;
            up = nup;
            look = nlook;
            pos = npos;
        }
        mat4(float px, float py, float pz,float rx,float ry,float rz) {
            pos.x = px;
            pos.y = py;
            pos.z = pz;

        }
        const float& operator[](const unsigned int x) const {
            return ((float*)this)[x];
        }
        float& operator[](const unsigned int x) {
            return ((float*)this)[x];
        }
        float i(int x, int y) {
            return ((float*)this)[x + y * 4];
        }
        mat4 operator*(mat4 b) {
            mat4& a = *this;
            mat4 r = mat4();
            r[0 ] = a[0 ] * b[0] + a[1 ] * b[4] + a[2 ] * b[8 ] + a[3 ] * b[12];
            r[4 ] = a[4 ] * b[0] + a[5 ] * b[4] + a[6 ] * b[8 ] + a[7 ] * b[12];
            r[8 ] = a[8 ] * b[0] + a[9 ] * b[4] + a[10] * b[8 ] + a[11] * b[12];
            r[12] = a[12] * b[0] + a[13] * b[4] + a[14] * b[8 ] + a[15] * b[12];
            
            r[1 ] = a[0 ] * b[1] + a[1 ] * b[5] + a[2 ] * b[9 ] + a[3 ] * b[13];
            r[5 ] = a[4 ] * b[1] + a[5 ] * b[5] + a[6 ] * b[9 ] + a[7 ] * b[13];
            r[9 ] = a[8 ] * b[1] + a[9 ] * b[5] + a[10] * b[9 ] + a[11] * b[13];
            r[13] = a[12] * b[1] + a[13] * b[5] + a[14] * b[9 ] + a[15] * b[13];

            r[2 ] = a[0 ] * b[2] + a[1 ] * b[6] + a[2 ] * b[10] + a[3 ] * b[14];
            r[6 ] = a[4 ] * b[2] + a[5 ] * b[6] + a[6 ] * b[10] + a[7 ] * b[14];
            r[10] = a[8 ] * b[2] + a[9 ] * b[6] + a[10] * b[10] + a[11] * b[14];
            r[14] = a[12] * b[2] + a[13] * b[6] + a[14] * b[10] + a[15] * b[14];

            r[3 ] = a[0 ] * b[3] + a[1 ] * b[7] + a[2 ] * b[11] + a[3 ] * b[15];
            r[7 ] = a[4 ] * b[3] + a[5 ] * b[7] + a[6 ] * b[11] + a[7 ] * b[15];
            r[11] = a[8 ] * b[3] + a[9 ] * b[7] + a[10] * b[11] + a[11] * b[15];
            r[15] = a[12] * b[3] + a[13] * b[7] + a[14] * b[11] + a[15] * b[15];
            return r;
        }
        vec4 operator*(vec4 p) {
            mat4& a = *this;
            vec4& r = p;
            r.x = r.x * a[0] + r.y * a[1] + r.z * a[2] + r.w * a[3];
            r.y = r.x * a[4] + r.y * a[5] + r.z * a[6] + r.w * a[7];
            r.z = r.x * a[8] + r.y * a[9] + r.z * a[10] + r.w * a[11];
            r.w = r.x * a[12] + r.y * a[13] + r.z * a[14] + r.w * a[15];
            return r;
        }
        mat4 operator*(float x) {
            mat4 r = mat4();
            r.look = look * x;
            r.up = up * x;
            r.right = right * x;
            r.pos = pos * x;
            return r;
        }
        float* data() {
            return (float*)this;
        }
        void out() {
            std::cout << "\n";
            right.out();
            up.out();
            look.out();
            pos.out();
            std::cout << "\n";
        }
        vec3 getLeft() {
            return vec3(look.z, look.y, look.x);
        }
        vec3 getRight() {
            return getLeft().neg();
        }
        vec3 getLook() {
            return vec3(right.z, right.y, right.x);
        }
        vec3 getUp() {
            return vec3(up.z, up.y, up.x);
        }
        mat4 inverse() {
            mat4& m = *this;
            float inv[16], det;
            int i;

            inv[0] = m[5] * m[10] * m[15] -
                m[5] * m[11] * m[14] -
                m[9] * m[6] * m[15] +
                m[9] * m[7] * m[14] +
                m[13] * m[6] * m[11] -
                m[13] * m[7] * m[10];

            inv[4] = -m[4] * m[10] * m[15] +
                m[4] * m[11] * m[14] +
                m[8] * m[6] * m[15] -
                m[8] * m[7] * m[14] -
                m[12] * m[6] * m[11] +
                m[12] * m[7] * m[10];

            inv[8] = m[4] * m[9] * m[15] -
                m[4] * m[11] * m[13] -
                m[8] * m[5] * m[15] +
                m[8] * m[7] * m[13] +
                m[12] * m[5] * m[11] -
                m[12] * m[7] * m[9];

            inv[12] = -m[4] * m[9] * m[14] +
                m[4] * m[10] * m[13] +
                m[8] * m[5] * m[14] -
                m[8] * m[6] * m[13] -
                m[12] * m[5] * m[10] +
                m[12] * m[6] * m[9];

            inv[1] = -m[1] * m[10] * m[15] +
                m[1] * m[11] * m[14] +
                m[9] * m[2] * m[15] -
                m[9] * m[3] * m[14] -
                m[13] * m[2] * m[11] +
                m[13] * m[3] * m[10];

            inv[5] = m[0] * m[10] * m[15] -
                m[0] * m[11] * m[14] -
                m[8] * m[2] * m[15] +
                m[8] * m[3] * m[14] +
                m[12] * m[2] * m[11] -
                m[12] * m[3] * m[10];

            inv[9] = -m[0] * m[9] * m[15] +
                m[0] * m[11] * m[13] +
                m[8] * m[1] * m[15] -
                m[8] * m[3] * m[13] -
                m[12] * m[1] * m[11] +
                m[12] * m[3] * m[9];

            inv[13] = m[0] * m[9] * m[14] -
                m[0] * m[10] * m[13] -
                m[8] * m[1] * m[14] +
                m[8] * m[2] * m[13] +
                m[12] * m[1] * m[10] -
                m[12] * m[2] * m[9];

            inv[2] = m[1] * m[6] * m[15] -
                m[1] * m[7] * m[14] -
                m[5] * m[2] * m[15] +
                m[5] * m[3] * m[14] +
                m[13] * m[2] * m[7] -
                m[13] * m[3] * m[6];

            inv[6] = -m[0] * m[6] * m[15] +
                m[0] * m[7] * m[14] +
                m[4] * m[2] * m[15] -
                m[4] * m[3] * m[14] -
                m[12] * m[2] * m[7] +
                m[12] * m[3] * m[6];

            inv[10] = m[0] * m[5] * m[15] -
                m[0] * m[7] * m[13] -
                m[4] * m[1] * m[15] +
                m[4] * m[3] * m[13] +
                m[12] * m[1] * m[7] -
                m[12] * m[3] * m[5];

            inv[14] = -m[0] * m[5] * m[14] +
                m[0] * m[6] * m[13] +
                m[4] * m[1] * m[14] -
                m[4] * m[2] * m[13] -
                m[12] * m[1] * m[6] +
                m[12] * m[2] * m[5];

            inv[3] = -m[1] * m[6] * m[11] +
                m[1] * m[7] * m[10] +
                m[5] * m[2] * m[11] -
                m[5] * m[3] * m[10] -
                m[9] * m[2] * m[7] +
                m[9] * m[3] * m[6];

            inv[7] = m[0] * m[6] * m[11] -
                m[0] * m[7] * m[10] -
                m[4] * m[2] * m[11] +
                m[4] * m[3] * m[10] +
                m[8] * m[2] * m[7] -
                m[8] * m[3] * m[6];

            inv[11] = -m[0] * m[5] * m[11] +
                m[0] * m[7] * m[9] +
                m[4] * m[1] * m[11] -
                m[4] * m[3] * m[9] -
                m[8] * m[1] * m[7] +
                m[8] * m[3] * m[5];

            inv[15] = m[0] * m[5] * m[10] -
                m[0] * m[6] * m[9] -
                m[4] * m[1] * m[10] +
                m[4] * m[2] * m[9] +
                m[8] * m[1] * m[6] -
                m[8] * m[2] * m[5];

            det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

            det = 1.0f / det;

            mat4 invOut = mat4();

            for (i = 0; i < 16; i++)
                invOut[i] = inv[i] * det;

            return invOut;
        }
    };
    mat4 matEulerY(float theta) {
        mat4 r = mat4();
        r.right = math::vec4(cos(theta), 0, -sin(theta), 0);
        r.look = math::vec4(sin(theta), 0, cos(theta), 0);
        return r;
    }
    mat4 matEulerX(float theta) {
        mat4 r = mat4();
        r.up = math::vec4(0,cos(theta), sin(theta), 0);
        r.look = math::vec4(0,-sin(theta), cos(theta), 0);
        return r;
    }
    mat4 matEulerZ(float theta) {
        mat4 r = mat4();
        r.right = math::vec4(cos(theta), -sin(theta), 0, 0);
        r.up = math::vec4(sin(theta), cos(theta), 0, 0);
        return r;
    }
    vec3 cross(vec3 a, vec3 b) {
        vec3 r = vec3(0, 0, 0);
        r.x = a.y * b.z - a.z * b.y;
        r.y = a.z * b.x - a.x * b.z;
        r.z = a.x * b.y - a.y * b.x;
        return r;
    }
    float dot(vec3 a, vec3 b) {
        float result = 0;
        result = a.x * b.x + a.y * b.y + a.z * b.z;
        return result;
    }
    mat4 matLookAt(vec3 start, vec3 end, vec3 ceiling) {
        /*vec3 forward = (end - start).norm();
        vec3 right = cross(ceiling.norm(),forward);
        vec3 up = cross(forward, right);
        mat4 r;
        r.right = vec4(right, 0.0f);
        r.up = vec4(up, 0.0f);
        r.look = vec4(forward, 0.0f);
        r.pos = vec4(start, 1.0f);
        return r;*/
        vec3 zaxis = (start - end).norm();
        vec3 xaxis = (cross(zaxis, ceiling)).norm();
        vec3 yaxis = cross(xaxis, zaxis);

        zaxis = zaxis.neg();

        mat4 viewMatrix = mat4(
          vec4(xaxis.x, xaxis.y, xaxis.z, 0),
          vec4(yaxis.x, yaxis.y, yaxis.z, 0),
          vec4(zaxis.x, zaxis.y, zaxis.z, 0),
          vec4(start, 1)
        );

        return viewMatrix;
    }
    vec3 vec3Lerp(vec3 first,vec3 second,float thing) {
        return vec3(
            first.x - (first.x - second.x) * thing,
            first.y - (first.y - second.y) * thing,
            first.z - (first.z - second.z) * thing
        );
    }
    template<typename T> T clamp(T x, T l, T h) {
        if (x > h) { return h; }
        else if (x < l) { return l; }
        else { return x; }
    }
}
namespace blocks {
    using namespace math;

    struct shaderUniforms {
        unsigned int camera = 0;
        unsigned int projection = 0;
        unsigned int transform = 0;
        unsigned int size = 0;
        unsigned int color = 0;
        shaderUniforms(unsigned int program) {
            camera = shorty::getUniform(program, "camera");
            projection = shorty::getUniform(program, "project");
            transform = shorty::getUniform(program, "transform");
            size = shorty::getUniform(program, "size");
            color = shorty::getUniform(program, "color");
        }
        shaderUniforms() {}
    };

    struct engineStuff { // eventually please migrate this to a Lua Table of stuff to allow us to get the data by a string name, keep references of stored light user data in C++ code thereafter
        const char* blockVertexShader;
        const char* blockFragmentShader; // please migrate shaderUniforms from renderer to here...
        engineStuff() {
            
        }
    };

    struct renderer {
        engineStuff* engine;
        unsigned int program;
        shaderUniforms uniforms;
        mat4 camera;
        mat4 project;
        shorty::shape box;
        renderer() {}
        void init(float ratio) {
            unsigned int vertexShader = shorty::makeShader(GL_VERTEX_SHADER, &engine->blockVertexShader);
            unsigned int fragmentShader = shorty::makeShader(GL_FRAGMENT_SHADER, &engine->blockFragmentShader);
            program = shorty::makeProgram(vertexShader, fragmentShader);
            shorty::dropShader(vertexShader);
            shorty::dropShader(fragmentShader);
            uniforms = shaderUniforms(program);
            box = shorty::shape(shorty::cuboid, sizeof(shorty::cuboid));
            glClearColor(0.4f, 0.7f, 0.9f, 0.0f);
            glEnable(GL_DEPTH_TEST);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glGetFloatv(GL_PROJECTION_MATRIX, (GLfloat*)&camera);
            glFrustum(-ratio, ratio, -1, 1, 1, 50);
            glGetFloatv(GL_PROJECTION_MATRIX, (GLfloat*)&project);
        }
        void render(mat4* transform,vec3 size,vec4 color) {
            glUniformMatrix4fv(uniforms.camera, 1, GL_FALSE,(GLfloat*)&camera);
            glUniformMatrix4fv(uniforms.projection, 1, GL_FALSE, (GLfloat*)&project);
            glUniform4fv(uniforms.color, 1, (GLfloat*)&color);
            glUniform3fv(uniforms.size, 1, (GLfloat*)&size);
            glUniformMatrix4fv(uniforms.transform, 1, GL_FALSE, (GLfloat*)transform);
        }
    };
    struct block {
        vec3 size = vec3(1, 1, 1);
        mat4 transform = mat4();
        vec4 color = vec4(1, 1, 1, 1);
        int goofy = 6968;
        block() {}
        void draw(renderer* system) {
            shorty::runProgram(system->program);
            system->render(&transform, size, color);
            system->box.draw();
        }
    };
}

const char* render_all_lua = R""""(
for _,object in pairs(world.inside) do
    print(object.name)
    object.draw()
end 
)"""";

lua_State* lua_main();
void render_all(lua_State* L) {
    int error = luaL_loadbuffer(L,render_all_lua,strlen(render_all_lua),"renderer") || lua_pcall(L, 0, 0, 0);
    if (error) {
        std::cout << "LuaError: " << lua_tostring(L, -1) << "\n";
        lua_pop(L, 1);
    }
}

namespace shorty {
    struct folder {
        std::string location = "C:/DuckEngine/version1/";
        folder(std::string newLocation) {
            location = newLocation;
            std::filesystem::create_directory(location);
        }
        std::string load(std::string name) {
            std::ifstream file(location + name);
            std::stringstream buffer;
            buffer << file.rdbuf();
            file.close();
            return buffer.str();
        }
        folder open(std::string name) {
            folder newFolder = folder(name);
            if (!std::filesystem::is_directory(name)) {
                std::filesystem::create_directory(name);
            }
            return newFolder;
        }
    };
}

namespace blocks {
    namespace lua {
        using namespace shorty::lua;
        int create_renderer(lua_State* L) {
            blocks::renderer* myNewRenderer = new blocks::renderer();
            lua_pushlightuserdata(L, myNewRenderer);
            luaL_getmetatable(L, "renderer");
            lua_setmetatable(L, -2);
            return 1;
        }
        int renderer_index(lua_State* L) { return 0; }
        int renderer_newindex(lua_State* L) { return 0; }
        int destroy_renderer(lua_State* L) { return 0; }
        int block_render(lua_State* L) {
            blocks::block* myBlock = (blocks::block*)lua_touserdata(L, 1);
            blocks::renderer* myRenderer = (blocks::renderer*)lua_touserdata(L, 2);
            myBlock->draw(myRenderer);
            //std::cout << "boi" << "\n";
            return 0;
        }
        int block_index(lua_State* L) {
            using namespace blocks;
            const char* name = lua_tostring(L, 2);
            block* myBlock = (block*)lua_touserdata(L, 1);
            if (name[0] == 'p') {
                lua_pushstring(L, "weeb, i haven't added VECTORS yet");
                return 1;
            }
            if (name[0] == 'r') {
                lua_pushcfunction(L,block_render);
                return 1;
            }
            if (name[0] == 'c') {
                lua_pushstring(L, "weeb, i haven't added COLORS yet");
                return 1;
            }
            if (name[0] == 'a') {
                lua_pushnumber(L, myBlock->goofy);
                return 1;
            }
            if (name[0] == 's') {
                lua_pushstring(L, "weeb, i haven't added VECTORS yet");
                return 1;
            }
            return 0;
        }
        int block_newindex(lua_State* L) {
            const char* name = lua_tostring(L, 2);
            blocks::block* myBlock = (blocks::block*)lua_touserdata(L, 1);
            if (name[0] == 'p' || name[0] == 'c' || name[0] == 's') {
                lua_geti(L, 3, 1);
                lua_geti(L, 3, 2);
                lua_geti(L, 3, 3);
                if (name[0] == 'p') {
                    myBlock->transform.pos = math::vec4(lua_tonumber(L, 4), lua_tonumber(L, 5), lua_tonumber(L, 6), 1);
                }
                else if (name[0] == 's') {
                    myBlock->size = math::vec3(lua_tonumber(L, 4), lua_tonumber(L, 5), lua_tonumber(L, 6));
                }
                else {
                    lua_geti(L, 3, 4);
                    myBlock->color = math::vec4(lua_tonumber(L, 4), lua_tonumber(L, 5), lua_tonumber(L, 6), lua_tonumber(L, 7));
                }
            }
            return 1;
        }
        int create_block(lua_State* L) { 
            blocks::block* myNewBlock = new blocks::block;
            lua_pushlightuserdata(L, myNewBlock);
            luaL_getmetatable(L, "block");
            lua_setmetatable(L, -2);
            return 1;
        }
        int destroy_block(lua_State* L) {
            block* myBlock = (block*)lua_touserdata(L, -1);
            delete[] myBlock;
            return 0;
        }

        int object_is(lua_State* L) {
            /*const char* string = lua_tostring(L, -1);
            lua_insert(L, 1);
            lua_pushstring(L, "real");
            lua_gettable(L, -2);
            lua_getmetatable(L, -1);
            luaL_getmetatable(L, string);
            outstack(L);
            int res = lua_rawequal(L, -1, -2);
            std::cout << res << std::endl;
            if (res) {
                lua_pushboolean(L, true);
            }
            else {
                lua_pushboolean(L, false);
            }
            return 1;*/
            //lua_insert(L, 1);
            lua_pushstring(L, "real");
            lua_gettable(L, -2);
            lua_getmetatable(L, -1);
            lua_pushstring(L, "name");
            lua_gettable(L, -2);
            outstack(L);
            return 0;
        }
        void load_object(lua_State* L, const char* name,
            int(*make)(lua_State* L),
            int(*destroy)(lua_State* L),
            int(*get)(lua_State* L),
            int(*set)(lua_State* L)
        ) {
            lua_getglobal(L, "new");
            lua_pushstring(L, name);
            lua_pushcfunction(L, make);
            lua_settable(L, -3);

            luaL_newmetatable(L, name);

            lua_pushstring(L, "__index");
            lua_pushcfunction(L,get);
            lua_settable(L, -3);

            lua_pushstring(L, "__newindex");
            lua_pushcfunction(L, set);
            lua_settable(L, -3);

            lua_pushstring(L, "name");
            lua_pushstring(L, name);
            lua_settable(L, -3);
        }
        void loadall(lua_State* L) {
            load_object(L, "block",
                create_block,
                destroy_block,
                block_index,
                block_newindex
            );
            load_object(L, "renderer",
                create_renderer,
                destroy_renderer,
                renderer_index,
                renderer_newindex
            );
            lua_getglobal(L, "_object");
            lua_pushstring(L, "is");
            lua_pushcfunction(L, object_is);
            lua_settable(L, -3);
        }
    }
}

int old_main()
{
    if (!glfwInit())
        return -1;
    float windowWidth = 1080;
    float windowHeight = 720;
    float windowRatio = (1080.0f / 720.0f);
    GLFWwindow* window = glfwCreateWindow((int)windowWidth, (int)windowHeight, "boi", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    lua_State* L = lua_main();

    double scale = 0;

    math::vec3 cameraOffset = math::vec3(0, 0, 0);

    lua_getglobal(L, "renderer");
    lua_pushstring(L, "real");
    lua_gettable(L, -2);

    /*blocks::renderer myRenderer = blocks::renderer(windowRatio);
    blocks::block myBlock = blocks::block();
    myBlock.transform.pos = math::vec4(0, 0, -4, 1);
    myBlock.color = math::vec4(1, 1, 1, 1);
    blocks::block otherBlock = blocks::block();
    otherBlock.transform.pos = math::vec4(8, 2, -4, 1);
    otherBlock.color = math::vec4(0, 1, 1, 1);*/

    shorty::folder stuffFolder = shorty::folder("E:/DuckEngine/Stuff/");

    blocks::engineStuff myEngine = blocks::engineStuff();
    std::string blockVertexShader = stuffFolder.load("blockvertexshader.txt").c_str();
    myEngine.blockVertexShader = blockVertexShader.c_str();
    std::string blockFragmentShader = stuffFolder.load("blockfragmentshader.txt").c_str();
    myEngine.blockFragmentShader = blockFragmentShader.c_str();

    blocks::renderer* myRenderer = (blocks::renderer*)lua_touserdata(L, -1);
    myRenderer->engine = &myEngine;
    myRenderer->init(windowRatio);

    double mouseX = 0;
    double mouseY = 0;

    float dMouseX = 0;
    float dMouseY = 0;

    float mouseS = 1.5;
    float walkS = 0.1;

    float lastTime = glfwGetTime();
    float delta;

    bool freeCam = true;

    float cameraPitch = 0;
    float cameraYaw = 0;
    math::vec3 cameraPos = math::vec3(0, 0, 0);

    myRenderer->camera.pos = math::vec4(0.0f, 0.0f, -8.0f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        shorty::sleep(14);
        glfwPollEvents();
        dMouseX = (float)mouseX;
        dMouseY = (float)mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        dMouseX -= (float)mouseX;
        dMouseX *= (float)mouseS;
        dMouseY -= (float)mouseY;
        dMouseY *= (float)mouseS;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        delta = (glfwGetTime() - lastTime) / 60;
        lastTime = glfwGetTime();
        if (shorty::keyDown(window, GLFW_KEY_W)) {
            cameraOffset.z = 1 * walkS;
        }
        else if (shorty::keyDown(window, GLFW_KEY_S)) {
            cameraOffset.z = -1 * walkS;
        }
        else {
            cameraOffset.z = 0;
        }
        if (shorty::keyDown(window, GLFW_KEY_A)) {
            cameraOffset.x = -1 * walkS;
        }
        else if (shorty::keyDown(window, GLFW_KEY_D)) {
            cameraOffset.x = 1 * walkS;
        }
        else {
            cameraOffset.x = 0;
        }
        if (shorty::keyDown(window, GLFW_KEY_E)) {
            cameraOffset.y = 1 * walkS;
        }
        else if (shorty::keyDown(window, GLFW_KEY_Q)) {
            cameraOffset.y = -1 * walkS;
        }
        else {
            cameraOffset.y = 0;
        }
        cameraYaw += dMouseX * delta * -mouseS;
        cameraPitch += dMouseY * delta * -mouseS;
        cameraPitch = math::clamp(cameraPitch, -1.0f, 1.0f);
        math::mat4 original = math::mat4();
        original.pos = math::vec4(cameraPos, 1.0f);
        original = original * math::matEulerY(cameraYaw);
        if (freeCam) {
            original = original * math::matEulerX(cameraPitch);
            cameraPos = cameraPos + original.getLook() * cameraOffset.z + original.getRight() * cameraOffset.x + original.getUp() * cameraOffset.y;
        }
        if (!freeCam) {
            cameraPos = cameraPos + math::vec3Lerp(
                original.getLook() * cameraOffset.z,
                original.getRight() * cameraOffset.x,
                0.5
            );
            original = original * math::matEulerX(cameraPitch);
        }
        if (shorty::keyDown(window, GLFW_KEY_SPACE)) { myRenderer->camera.out(); }
        myRenderer->camera = original;
        //myBlock.draw(myRenderer);
        //otherBlock.draw(myRenderer);

        lua_getglobal(L, "render_all");
        lua_pushnumber(L, delta);
        lua_pcall(L, 1, 0, 0);

        //otherBlock.transform = math::matEulerY(0.1) * otherBlock.transform;
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}

lua_State* lua_main() {
    std::string engineName("demo_engine_lua.lua");
    std::string worldName("demo_world_lua.lua");
    shorty::folder worldFolder = shorty::folder("E:/DuckEngine/Worlds/");
    shorty::folder stuffFolder = shorty::folder("E:/DuckEngine/Stuff/");
    std::string engine = stuffFolder.load(engineName);
    std::string world = worldFolder.load(worldName);

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    int error;

    error = luaL_loadbuffer(L, engine.c_str(), engine.length(), "engine") || lua_pcall(L, 0, 0, 0);
    if (error) {
        std::cout << "LuaError: " << lua_tostring(L, -1) << "\n";
        lua_pop(L, 1);
    }

    blocks::lua::loadall(L);

    lua_getglobal(L, "setup_all");
    error = lua_pcall(L,0,0,0);
    if (error) {
        std::cout << "LuaError: " << lua_tostring(L, -1) << "\n";
        lua_pop(L, 1);
    }

    error = luaL_loadbuffer(L, world.c_str(), world.length(), worldName.c_str()) || lua_pcall(L, 0, 0, 0);
    if (error) {
        std::cout << "LuaError: " << lua_tostring(L, -1) << "\n";
        lua_pop(L, 1);
    }

    return L;
}

int main() {
    return old_main();
}