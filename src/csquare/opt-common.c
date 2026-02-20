#include "csquare/opt-common.h"
#include "csquare/common_macros.h"
#include "info.h"
#include <stddef.h>
static void handle_info(csq_options *opts, const char *val);

static const opt_map_t opts[] = {
    {"--info", "-i", OPT_KIND_FUNC, offsetof(csq_options, show_info), handle_info}};

static void handle_info(csq_options *opts, const char *val) {
    (void)opts;
    printf("Csquared - %s (%s)\n", CSQ_VERSION, __DATE__);
    printf("Authors: %s\n", CSQ_AUTHORS);
    #ifdef CSQ_DEBUG
        printf("Build: Debug\n");
    #else
        printf("Build: Release\n");
    #endif
    opts->show_info = true;

}
csq_options *options_parse(int argc, char *argv[]) {
  csq_options *opt = calloc(1, sizeof(csq_options));
  if (!opt)
    return NULL;
  for (int i = 1; i < argc; ++i) {
    const char *arg = argv[i];
    bool opt_found = false;

    for (int j = 0; opts[j].long_name != NULL; ++j) {
      const opt_map_t *m = &opts[j];
      if (STRCMP(arg, m->long_name) == 0 ||
          (m->short_name && STRCMP(arg, m->short_name) == 0)) {
        opt_found = true;
        const char* val = NULL;
        if (m->kind == OPT_KIND_FLAG) {
          *(bool *)((char *)opt + m->offset) = true;
        } else if (m->kind == OPT_KIND_VAL) {
          if (i + 1 < argc)
            *(const char **)((char *)opt + m->offset) = argv[++i];
          else {
            fprintf(stderr, "Error: %s requires an argument\n", arg);
            goto error;
          }
        } else if(m->kind == OPT_KIND_FUNC && m->func) {
            m->func(opt, val);
        }
        break;
      }
    }
  }

  return opt;
error:
  options_free(opt);
  return NULL;
}

void options_free(csq_options *opt) { free(opt); }
