#include "cebus/core/error.h"
#include "toy.h"

#include "glad.h"

static const Str header = STR_STATIC( //
    "#version 430 core\n"
    "layout(location = 0) out vec4 f_color;\n"
    "uniform float iTime;\n"
    "uniform float iFrame;\n"
    "uniform vec2 iResolution;\n"
    "uniform vec4 iMouse;\n"
    "uniform float iTimeDelta;\n");

static const Str footer = STR_STATIC( //
    "void main() {\n"
    "  mainImage(f_color, gl_FragCoord.xy);\n"
    "}\n");

static const Str vs = STR_STATIC( //
    "#version 430 core\n"
    "void main() {\n"
    "  gl_Position = vec4(\n"
    "    (gl_VertexID >> 0 & 1) * 2 - 1,\n"
    "    (gl_VertexID >> 1 & 1) * 2 - 1,\n"
    "    0.0,\n"
    "    1.0\n"
    "  );\n"
    "}\n");

typedef struct {
  CmShader shader;
  float time;
  vec2 resolution;
  i32 frame;
  vec4 mouse;
} Shader;

static void init(CmScene *scene) {
  (void)cm_scene_alloc_data(scene, sizeof(Shader));
  Shader *shader = scene->data;

  RGFW_window *window = cm_app_window();
  shader->resolution[0] = window->r.w;
  shader->resolution[1] = window->r.h;

  shader->mouse[0] = window->event.point.x;
  shader->mouse[1] = window->event.point.y;
  shader->mouse[2] = 0;
  shader->mouse[3] = 1;
}

static void event(CmScene *scene, CmEvent *event) {
  Shader *shader = scene->data;
  cm_event_cursor(event, {
    shader->mouse[0] = cursor->pos[0];
    shader->mouse[1] = cursor->pos[1];
  });
}

static void update(CmScene *scene, double dt) {
  Shader *shader = scene->data;

  cm_shader_bind(&shader->shader);
  cm_shader_set_f32(&shader->shader, STR("iTime"), shader->time += dt);
  cm_shader_set_f32(&shader->shader, STR("iFrame"), shader->frame++);
  cm_shader_set_vec2(&shader->shader, STR("iResolution"), shader->resolution);
  cm_shader_set_vec4(&shader->shader, STR("iMouse"), shader->mouse);
  cm_shader_set_f32(&shader->shader, STR("iTimeDelta"), dt);

  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);
}

static CmSceneInterface *shader(void) {
  static CmSceneInterface interface = {
      .init = init,
      .update = update,
      .event = event,
  };
  return &interface;
}

CmScene *shader_init(CmScene *parent, Str filename, Error *error) {
  CmScene *scene = cm_scene_push(parent, shader);
  Shader *toy = scene->data;

  Arena arena = {0};
  Str content = file_read_str(filename, &arena, error);
  error_propagate(error, { goto defer; });

  StringBuilder sb = sb_init(&arena);
  sb_append_str(&sb, header);
  sb_append_str(&sb, content);
  sb_append_str(&sb, footer);

  Str fs = sb_to_str(&sb);
  toy->shader = cm_shader_from_memory(&scene->gpu, vs, fs, error);
  error_propagate(error, {
    Str msg = error_msg();
    (void)str_take(&msg, 2);
    int line = str_chop_i64(&msg) - str_count(header, STR("\n"));
    error_set_msg("%d" STR_FMT, line, STR_ARG(msg));
    goto defer;
  });

defer:
  arena_free(&arena);
  return scene;
}
