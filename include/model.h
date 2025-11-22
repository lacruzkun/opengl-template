#pragma once
#include <cglm/cglm.h>

typedef struct {
    vec3 Vertices;
    vec3 Normal;
    vec2 TexCoord;
} Vertex;

typedef struct {
    Vertex *data;
    size_t size;
    size_t count;
} Vertices;

typedef struct {
    unsigned int *data;
    size_t size;
    size_t count;
} Indices;

typedef struct {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    char material[32];
    float shininess;
    int (*update)(int x);
} Texture;

typedef struct Textures{
    Texture data[128];
    size_t count;
    size_t size;
    //void (*update)(struct Textures *texture);
} Textures;

typedef struct {
    Indices *data;
    size_t size;
} Faces;

typedef struct {
    Faces face;
    Textures tex;
    Vertices vertices;
} Mesh;

void (dtarray_update)(Textures *texture);
int parse_obj(char *path, Vertices *vertices, Indices *indices, Textures *texture);
int parse_mtl(char *path, Textures *texture);
