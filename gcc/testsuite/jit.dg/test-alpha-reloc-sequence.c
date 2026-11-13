/* { dg-do compile { target alpha*-*-linux* } } */

#include <stdlib.h>
#include <stdio.h>
#include "libgccjit.h"

#define TEST_COMPILING_TO_FILE
#define OUTPUT_KIND GCC_JIT_OUTPUT_KIND_OBJECT_FILE
#define OUTPUT_FILENAME "output-of-test-alpha-reloc-sequence.o"
#include "harness.h"

void
create_code (gcc_jit_context *ctxt, void *user_data)
{
  gcc_jit_context_add_command_line_option (ctxt, "-fPIC");
  gcc_jit_context_add_command_line_option (ctxt, "--param=ggc-min-expand=0");
  gcc_jit_context_add_command_line_option (ctxt, "--param=ggc-min-heapsize=0");

  gcc_jit_type *type = gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_LONG);
  gcc_jit_param *param = gcc_jit_context_new_param (ctxt, NULL, type, "x");
  gcc_jit_function *callee = gcc_jit_context_new_function (
    ctxt, NULL, GCC_JIT_FUNCTION_IMPORTED, type, "callee", 1, &param, 0);
  gcc_jit_lvalue *global = gcc_jit_context_new_global (
    ctxt, NULL, GCC_JIT_GLOBAL_IMPORTED, type, "global");
  param = gcc_jit_context_new_param (ctxt, NULL, type, "x");
  gcc_jit_function *func = gcc_jit_context_new_function (
    ctxt, NULL, GCC_JIT_FUNCTION_EXPORTED, type, "test_reloc", 1, &param, 0);
  gcc_jit_rvalue *arg = gcc_jit_param_as_rvalue (param);
  gcc_jit_rvalue *call = gcc_jit_context_new_call (ctxt, NULL, callee, 1, &arg);
  gcc_jit_rvalue *sum = gcc_jit_context_new_binary_op (
    ctxt, NULL, GCC_JIT_BINARY_OP_PLUS, type, call,
    gcc_jit_lvalue_as_rvalue (global));
  gcc_jit_block *block = gcc_jit_function_new_block (func, "entry");
  gcc_jit_block_end_with_return (block, NULL, sum);
}

/* { dg-final { jit-verify-output-file-was-created "" } } */
