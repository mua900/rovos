#ifndef _DRAW_H
#define _DRAW_H

#include <SDL3/SDL.h>
#include "common.hpp"
#include "math_util.hpp"
#include "template.hpp"

struct RenderContext {
    vec2 render_size = {};
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* render_target = nullptr;

    DArray<SDL_Vertex> vertex_scratch;
    DArray<int> index_scratch;
};

struct Mesh {
    DArray<vec2> points;
    DArray<int> indices;

    ~Mesh() {
        points.reset();
        indices.reset();
    }
};

bool loadShader(RenderContext& context, const char* path);

void draw_circle(const RenderContext& context, vec2 center, float radius);
void draw_quadratic_bezier(const RenderContext& context, vec2 p0, vec2 p1, vec2 p2, float thick, ColorF color);
void draw_cubic_bezier(const RenderContext& context, vec2 p0, vec2 p1, vec2 p2, vec2 p3, float thick, ColorF color);

void draw_texture(const RenderContext& context, Rectangle area, SDL_Texture* texture);

#endif // _DRAW_H