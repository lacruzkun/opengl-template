#include "model.h"
#include <string.h>
#include <stdlib.h>
/*
void (dtarray_update)(Textures *texture) {
    if (texture->count == texture->size) {
        Texture *temp = (Texture *)malloc(sizeof(Texture) * texture->size * 2);
        for (size_t i = 0; i < texture->size; i++) {
            temp[i] = texture->data[i];
        }
        free(texture->data);
        texture->data = temp;
    }
}
*/

int parse_obj(char *path, Vertices *vertices, Indices *indices, Textures *texture)
{
    FILE *file;

    if (!(file = fopen(path, "r"))){
        printf("couldn't open '%s'\n", path);
    }

    
    char buffer[512]; //buffer to hold each line of the file
    char mtllib[128];
    vertices->size = 0;
    indices->size = 0;
    // first iteration to get the number of vertex, faces etc
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strncmp(buffer, "v ", 2) == 0) {
            vertices->size += 1;
        }
        if (strncmp(buffer, "f ", 2) == 0) {
            indices->size += 6;
        }
    }

    //printf("%zu\n", vertices->size);
    //printf("%zu\n", indices->size);
    // allocating memory for the datas
    vertices->data = (Vertex *) malloc(sizeof(Vertex) * vertices->size);
    indices->data = (unsigned int *) malloc(sizeof(unsigned int) * indices->size);
    
    rewind(file);
    unsigned int total_vertex = 0;
    unsigned int total_indices = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        if (strncmp(buffer, "v ", 2) == 0) {
            int i = 0;
            float token = atof(strtok(buffer+2, " ")); // +2 to offset the string
            vertices->data[total_vertex].Vertices[i] = token;
            i++;
            for (; i < 3; i++) {
                token = atof(strtok(NULL, " "));
                vertices->data[total_vertex].Vertices[i] = token;
            }
            total_vertex++;
        } 
        else if (strncmp(buffer, "f ", 2) == 0) {
            int i = 0;
            char *tokens[5] = {NULL}; // a pointer to an array of 4 strings
                             // first of all get all the tokens in a line and store them in an array
            char *first_token = strtok(buffer+1, " ");
            tokens[i] = first_token;
            i++;
            for (; i < 5; i++) {
                first_token = strtok(NULL, " ");
                tokens[i] = first_token;
            }
            // then go through the array and split by / 
            int j;
            for (j = 0; j < 4; j++) {
                char *second_token = strtok(tokens[j], "/");
                if (second_token)
                    indices->data[total_indices] = atoi(second_token) - 1;
                for (int k = 0; k < 2; k++) {
                    // continue split cos i haven't implemented the face data for vertex and texture i think
                    second_token = strtok(NULL, "/");
                }
                total_indices++;
            }
            indices->data[total_indices] = indices->data[total_indices - 1];
            indices->data[total_indices - 1] = indices->data[total_indices - 2];
            indices->data[total_indices + 1] = indices->data[total_indices - 4];
            total_indices += 2;
        }
        else if (strncmp(buffer, "mtllib ", strlen("mtllib ")) == 0) {
            // split the mtllib from the line and store the file name in mtllib
            char *token = buffer+ strlen("mtllib ");
            strcpy(mtllib, token);

            strcpy(token, path);

            // all this sinanegans just to split the main path by / and the replace the last part with the text in mtllib
            char *mtllib_path = strtok(token, "/");
            char temp[128] = {0};
            int mtllib_path_len = strlen(mtllib_path);
            strcpy(temp, mtllib_path);
            for (int i = mtllib_path_len; mtllib_path != NULL; i+=mtllib_path_len) {
                temp[i] = '/';
                i++;
                strcpy(temp+i, mtllib_path);
                mtllib_path = strtok(NULL, "/");
                if (!mtllib_path) {
                    strcpy(temp+i-mtllib_path_len-1, mtllib);
                    temp[i-mtllib_path_len-2+strlen(mtllib)] = '\0';
                    break;
                }
                mtllib_path_len = strlen(mtllib_path);
            }

            printf("%s\n", temp);


            parse_mtl(temp, texture);
        }
            

    }
    /*for (int i = 0; i < vertices->size; i++) {
        printf("%f, %f, %f\n", vertices->data[i].Vertices[0], vertices->data[i].Vertices[1], vertices->data[i].Vertices[2]);
    }

    for (int i = 0; i < total_indices; i++) {
        printf("%u\n", indices->data[i]);
    }

    printf("%lu\n", indices->size);*/
    return 0;
}


int parse_mtl(char *path, Textures *texture)
{
    FILE *file;
    if (!(file = fopen(path, "r"))) {
        printf("coulnd't open %s\n", path);
        return -1;
    }

    char buffer[512];
    //texture->update = dtarray_update;
    //texture->data = (Texture *)malloc(sizeof(Texture) * 4);
    texture->size = 4;
    texture->count = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        if (strncmp(buffer, "newmtl ", strlen("newmtl ")) == 0) {
            texture->count++;
            //texture->update(texture);
            char *token = strtok(buffer+strlen("newmtl "), " ");
            strcpy(texture->data[texture->count - 1].material, token);
        }
        if (strncmp(buffer, "Ka ", 3) == 0) {
            int i = 0;
            float token = atof(strtok(buffer+3, " "));
            texture->data[texture->count - 1].ambient[i] = token;

            i++;
            for (; i < 3; i++) {
                token = atof(strtok(NULL, " "));
                texture->data[texture->count - 1].ambient[i] = token;
            }

        }

        if (strncmp(buffer, "Kd ", 3) == 0) {
            int i = 0;
            float token = atof(strtok(buffer+3, " "));
            texture->data[texture->count - 1].diffuse[i] = token;

            i++;
            for (; i < 3; i++) {
                token = atof(strtok(NULL, " "));
                texture->data[texture->count - 1].diffuse[i] = token;
            }

        }

        if (strncmp(buffer, "Ks ", 3) == 0) {
            int i = 0;
            float token = atof(strtok(buffer+3, " "));
            texture->data[texture->count - 1].specular[i] = token;

            i++;
            for (; i < 3; i++) {
                token = atof(strtok(NULL, " "));
                texture->data[texture->count - 1].specular[i] = token;
            }

        }

        if (strncmp(buffer, "Ns ", 3) == 0) {
            float token = atof(strtok(buffer+3, " "));
            texture->data[texture->count - 1].shininess = token;
        }
    }
    return 0;
}
