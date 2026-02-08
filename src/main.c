/**
 * @file main.c
 * @brief Main entry point for the C² compiler
 * @details Implements the main function that orchestrates the complete
 * compilation pipeline: source loading, lexical analysis, parsing,
 * optimization, and code generation. Handles command-line argument parsing and
 * manages resource cleanup.
 */

#include <codegen/x86/gen.h>
#include <core/diag.h>
#include <csquare/lexer.h>
#include <csquare/logger.h>
#include <csquare/opt-common.h>
#include <csquare/source.h>
#include <middle/optimizer.h>
#include <parser/parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/ast_printer.h>
#include <util/linker.h>
/**
 * @brief Main compiler entry point.
 * @details Orchestrates the complete compilation pipeline: loads source files,
 * performs lexical analysis, parsing, optimization, code generation, and
 * linking. Handles error reporting and resource cleanup throughout the process.
 * @param argc Argument count from command line
 * @param argv Argument vector containing command-line arguments
 * @return Exit status (0 for success, 1 for failure)
 */
int main(int argc, char **argv) {
  /* C² flags */
  csq_options *opts = options_parse(argc, argv);
  if (!opts)
    return 1;
  char *filepath = opts->input_file;

  csq_debug_enabled = opts->debug_lexer || opts->debug_ast;

  RVN_INFO("Starting C² compiler...");

  csq_source *src = source_load(filepath);
  if (!src)
    return 1;

  DiagReporter *diag = diag_reporter_create();
  if (!diag) {
    source_free(src);
    return 1;
  }

  csq_lexer *lexer = lexer_create(src, diag);
  if (!lexer) {
    diag_reporter_free(diag);
    source_free(src);
    return 1;
  }

  if (opts->debug_lexer) {
    csq_token token;
    do {
      token = lexer_next(lexer);
      lexer_print_token(&token);
    } while (token.type != TOKEN_EOF);

    lexer_free(lexer);
    lexer = lexer_create(src, diag);
    if (!lexer) {
      diag_reporter_free(diag);
      source_free(src);
      return 1;
    }
  }

  csq_parser *parser = parser_create(lexer, diag);
  if (!parser) {
    lexer_free(lexer);
    diag_reporter_free(diag);
    source_free(src);
    return 1;
  }

  ast_context *ast = parser_parse(parser);

  if (opts->debug_ast && ast && ast->root)
    ast_print(ast, stdout);

  if (diag_has_errors(diag)) {
    printf("\n");
    diag_print_all(diag, src->buffer);
    parser_free(parser);
    lexer_free(lexer);
    diag_reporter_free(diag);
    source_free(src);
    options_free(opts);
    return 1;
  }

  if (ast && ast->root) {
    optimizer_state *opt = optimizer_create(ast);
    if (opt) {
      optimizer_configure(opt, 2);
      optimizer_run(opt);
      optimizer_free(opt);
    }

    gen_ctx *gen = gen_create(ast);
    if (gen) {
      gen->diag = diag;
      gen->source_path = filepath;
      gen->source_buffer = src->buffer;
      gen_set_asm_mode(gen, 1);
      if (opts->asm_backend == ASM_BACKEND_INTEL) {
        gen_set_syntax(gen, ASM_SYNTAX_INTEL);
      }

      char *temp_dir = NULL;
      char *asm_file = NULL;

      if (!opts->emit_asm) {
        temp_dir = linker_get_temp_dir();
        if (!temp_dir) {
          gen_free(gen);
          parser_free(parser);
          lexer_free(lexer);
          diag_reporter_free(diag);
          source_free(src);
          options_free(opts);
          return 1;
        }

        const char *basename = strrchr(filepath, '/');
#ifdef _WIN32
        if (!basename)
          basename = strrchr(filepath, '\\');
#endif
        if (!basename)
          basename = filepath;
        else
          basename++;
        size_t basename_len = strlen(basename);
        size_t asm_file_len = strlen(temp_dir) + basename_len + 10;
        asm_file = malloc(asm_file_len);
        if (!asm_file) {
          free(temp_dir);
          gen_free(gen);
          parser_free(parser);
          lexer_free(lexer);
          diag_reporter_free(diag);
          source_free(src);
          options_free(opts);
          return 1;
        }

        snprintf(asm_file, asm_file_len, "%s/%s", temp_dir, basename);
        char *dot = strrchr(asm_file, '.');
        if (dot)
          snprintf(dot, 3, ".s");
        else
          snprintf(asm_file + strlen(asm_file), 3, ".s");

        gen_set_output(gen, asm_file);
      } else {
        if (opts->output_file) {
          gen_set_output(gen, opts->output_file);
        } else {
          size_t filepath_len = strlen(filepath);
          asm_file = malloc(filepath_len + 3);
          if (asm_file) {
            snprintf(asm_file, filepath_len + 3, "%s", filepath);
            char *dot = strrchr(asm_file, '.');
            if (dot) {
              snprintf(dot, 3, ".s");
            } else {
              snprintf(asm_file + filepath_len, 3, ".s");
            }
            gen_set_output(gen, asm_file);
          }
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
        options_free(opts);
        free(asm_file);
        free(temp_dir);
        return 1;
      }

      if (!opts->emit_asm && asm_file) {
        char *output_path = NULL;
        if (opts->output_file)
          output_path = (char *)opts->output_file;
        else {
          size_t filepath_len = strlen(filepath);
          output_path = malloc(filepath_len + 1);
          if (output_path) {
            snprintf(output_path, filepath_len + 1, "%s", filepath);
            char *dot = strrchr(output_path, '.');
            if (dot)
              *dot = '\0';
          }
        }

        if (output_path) {
          linker_ctx *linker = linker_create(asm_file, output_path);
          if (linker) {
            int link_result = linker_assemble_and_link(linker);
            linker_free(linker);

            if (link_result != 0) {
              gen_free(gen);
              parser_free(parser);
              lexer_free(lexer);
              diag_reporter_free(diag);
              source_free(src);
              options_free(opts);
              free(asm_file);
              free(temp_dir);
              if (!opts->output_file)
                free(output_path);

              return 1;
            }
          }
          if (!opts->output_file)
            free(output_path);
        }
      }

      free(asm_file);
      free(temp_dir);

      gen_free(gen);
    }
  }

  parser_free(parser);
  lexer_free(lexer);
  diag_reporter_free(diag);
  source_free(src);
  options_free(opts);
  return 0;
}
