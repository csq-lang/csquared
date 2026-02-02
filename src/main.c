#include <codegen/x86/gen.h>
#include <core/diag.h>
#include <parser/parser.h>
#include <raven/flag.h>
#include <raven/lexer.h>
#include <raven/logger.h>
#include <raven/source.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/ast_printer.h>
#include <version.h>

int main(int argc, char **argv) {
  bool debug_lexer = false;
  bool debug_ast = false;
  bool emit_asm = false;
  bool verbose = false;
  char *output_file = NULL;

  rvn_flag_ctx *flag_ctx = flags_create();
  if (!flag_ctx) {
    return 1;
  }

  flags_register_bool(flag_ctx, "debug-lexer", 'd', "Print lexer debug output",
                      &debug_lexer);
  flags_register_bool(flag_ctx, "debug-ast", 'a', "Print AST debug output",
                      &debug_ast);
  flags_register_bool(flag_ctx, "emit-asm", 's', "Emit assembly code",
                      &emit_asm);
  flags_register_bool(flag_ctx, "verbose", 'v', "Enable verbose output (logs)",
                      &verbose);
  flags_register_string(flag_ctx, "output", 'o', "Output file", &output_file);

  int file_arg = flags_parse(flag_ctx, argc, argv);
  if (file_arg < 0) {
    flags_destroy(flag_ctx);
    return 1;
  }

  if (file_arg >= argc) {
    RVN_ERROR("Usage: raven [flags] <filepath>");
    flags_print_help(flag_ctx, argv[0]);
    flags_destroy(flag_ctx);
    return 1;
  }

  char *filepath = argv[file_arg];
  size_t len = strlen(filepath);
  if (len < 4 || strcmp(filepath + len - 4, ".rvn") != 0) {
    RVN_ERROR("Error: File must have .rvn extension");
    flags_destroy(flag_ctx);
    return 1;
  }

  raven_debug_enabled = debug_lexer || debug_ast || verbose;

  RVN_INFO("Raven compiler version " RVN_VERSION ", built in " RVN_BUILD_DATE);
  RVN_INFO("Starting Raven compiler...");

  rvn_source *src = source_load(filepath);
  if (!src) {
    flags_destroy(flag_ctx);
    return 1;
  }

  DiagReporter *diag = diag_reporter_create();
  if (!diag) {
    source_free(src);
    flags_destroy(flag_ctx);
    return 1;
  }

  rvn_lexer *lexer = lexer_create(src, diag);
  if (!lexer) {
    diag_reporter_free(diag);
    source_free(src);
    flags_destroy(flag_ctx);
    return 1;
  }

  if (debug_lexer) {
    Token token;
    do {
      token = lexer_next(lexer);
      lexer_print_token(&token);
    } while (token.type != TOKEN_EOF);

    lexer_free(lexer);
    lexer = lexer_create(src, diag);
    if (!lexer) {
      diag_reporter_free(diag);
      source_free(src);
      flags_destroy(flag_ctx);
      return 1;
    }
  }

  rvn_parser *parser = parser_create(lexer, diag);
  if (!parser) {
    lexer_free(lexer);
    diag_reporter_free(diag);
    source_free(src);
    flags_destroy(flag_ctx);
    return 1;
  }

  ast_context *ast = parser_parse(parser);

  if (debug_ast && ast && ast->root) {
    ast_print(ast, stdout);
  }

  if (diag_has_errors(diag)) {
    printf("\n");
    diag_print_all(diag, src->buffer);
    parser_free(parser);
    lexer_free(lexer);
    diag_reporter_free(diag);
    source_free(src);
    flags_destroy(flag_ctx);
    return 1;
  }

  if (ast && ast->root) {
    gen_ctx *gen = gen_create(ast);
    if (gen) {
      gen->diag = diag;
      gen->source_path = filepath;
      gen->source_buffer = src->buffer;
      gen_set_asm_mode(gen, emit_asm);

      if (output_file) {
        gen_set_output(gen, output_file);
      } else if (emit_asm) {
        size_t filepath_len = strlen(filepath);
        char *asm_file = malloc(filepath_len + 3);
        if (asm_file) {
          strcpy(asm_file, filepath);
          char *dot = strrchr(asm_file, '.');
          if (dot) {
            strcpy(dot, ".s");
          } else {
            strcat(asm_file, ".s");
          }
          gen_set_output(gen, asm_file);
          free(asm_file);
        }
      }

      gen_generate(gen);

      if (diag_has_errors(diag)) {
        printf("\n");
        diag_print_all(diag, src->buffer);
        gen_free(gen);
        parser_free(parser);
        lexer_free(lexer);
        diag_reporter_free(diag);
        source_free(src);
        flags_destroy(flag_ctx);
        return 1;
      }

      gen_free(gen);
    }
  }

  parser_free(parser);
  lexer_free(lexer);
  diag_reporter_free(diag);
  source_free(src);
  flags_destroy(flag_ctx);
  return 0;
}
