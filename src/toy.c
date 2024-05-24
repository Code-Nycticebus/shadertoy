#include "claymore/entrypoint.h"

#include "toy.h"

#include <sys/stat.h>
#if defined(LINUX)
#define MTIME(s) (s).st_mtim.tv_sec
#elif defined(WINDOWS)
#define MTIME(s) (s).st_mtime
#endif

static const float interval = 1.f;

typedef struct {
  CmScene *child;
  Str filename;
  int mtime;
  float timer;
} Toy;

static void reload_shader(CmScene *scene) {
  Toy *toy = scene->data;
  if (toy->child != NULL) {
    cm_scene_delete(scene, toy->child);
  }
  Error error = ErrNew;
  toy->child = shader_init(scene, toy->filename, &error);
  error_context(&error, {
    cm_scene_delete(scene, toy->child);
    toy->child = error_display_init(scene, error_msg());
    error_except();
  });
}

static void init(CmScene *scene) {
  Toy *toy = cm_scene_alloc_data(scene, sizeof(Toy));
  toy->timer = interval;
  toy->filename = STR("toy.fs.glsl");
}

static void event(CmScene *scene, CmEvent *event) {
  (void)scene;
  cm_event_key(event, {
    if (key->action == RGFW_keyPressed) {
      if (key->code == RGFW_Escape) {
        cm_app_quit();
        return;
      }
    }
  });
}

static void update(CmScene *scene, double dt) {
  Toy *toy = scene->data;
  toy->timer += dt;
  if (interval < toy->timer) {
    toy->timer = 0;

    struct stat file;
    stat(toy->filename.data, &file);
    int mtime = MTIME(file);

    if (toy->mtime != mtime) {
      toy->mtime = mtime;
      reload_shader(scene);
    }
  }
}

static CmSceneInterface *toy(void) {
  static CmSceneInterface interface = {
      .init = init,
      .update = update,
      .event = event,
  };
  return &interface;
}

ClaymoreConfig *claymore_init(void) {
  static ClaymoreConfig config = {
      .window = {.width = 720, .height = 720, .title = "Shader Toy"},
      .main = toy,
  };
  return &config;
}
