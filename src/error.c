#include "toy.h"

static const Str font = STR_STATIC("assets/Ubuntu.ttf");
static const float font_size = 20.f;
static const vec2 offset = {20.f, 20.f};

typedef struct {
  CmCamera2D camera;
  CmFont *font;
  Str message;
} ErrorDisplay;

static void init(CmScene *scene) {
  ErrorDisplay *display = cm_scene_alloc_data(scene, sizeof(ErrorDisplay));
  display->font = cm_font_init(&scene->gpu, font, font_size, ErrPanic);
  cm_camera2d_screen(&display->camera);
}

static void update(CmScene *scene, double UNUSED dt) {
  ErrorDisplay *display = scene->data;

  cm_renderer2d_begin(&display->camera);
  cm_font(display->font, offset, display->message);
  cm_renderer2d_end();
}

static CmSceneInterface *error_display(void) {
  static CmSceneInterface interface = {
      .init = init,
      .update = update,
  };
  return &interface;
}

CmScene *error_display_init(CmScene *parent, Str message) {
  CmScene *scene = cm_scene_push(parent, error_display);
  ErrorDisplay *display = scene->data;
  display->message = str_copy(message, &scene->arena);
  return scene;
}
