#include <time.h>
// Copyright (C) 2019-2021 Christophe Bedard
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <stdio.h>
#include <assert.h>

#include "gcc-plugin.h"

#include "langhooks.h"
#include "plugin-version.h"
#include "stringpool.h"
#include "tree.h"
#include "attribs.h"

#include "utils.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/trace_instrument_attribute.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"instrument_attribute.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}

int plugin_is_GPL_compatible;

const char * ATTRIBUTE_NAME = "instrument_function";
const char * PLUGIN_NAME = "instrument_function attribute";
const char * LIST_DELIMITER = ",";
const char * ARG_DEBUG = "debug";
const char * ARG_INCLUDE_FILE_LIST = "include-file-list";
const char * ARG_INCLUDE_FUNCTION_LIST = "include-function-list";

bool is_verbose = false;
bool is_debug = false;
#define VERBOSE(...) \
  do { \
    if (is_verbose) { \
      printf(__VA_ARGS__); \
    } \
  } while (0)
#define DEBUG(...) \
  do { \
    if (is_debug) { \
      printf(__VA_ARGS__); \
    } \
  } while (0)

struct string_list include_files = {};
struct string_list include_functions = {};

static struct plugin_info info = {
  "0.2.0",  // version
  "This plugin provides the instrument_function attribute and"
    " other flags to enable instrumentation through allow-listing.",
};

static struct attribute_spec instrument_function_attr = {
  ATTRIBUTE_NAME,
  0,
  -1,
  false,
  false,
  false,
  NULL,  // No need for a handling function
};

static void register_attributes(void * event_data, void * data)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/instrument_attribute.c\",\"function\":\"register_attributes\",\"func_id\":7,\"num_params\":2,\"start_line\":72,\"end_line\":75,\"metrics\":{\"num_params\":2,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"event_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"register_attributes\",\"func_id\":7,\"name\":\"event_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] event_data = %p\n", event_data);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"register_attributes\",\"func_id\":7,\"name\":\"data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] data = %p\n", data);
  register_attribute(&instrument_function_attr);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"register_attributes\",\"func_id\":7}");
}

void print_list(struct string_list * list)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/instrument_attribute.c\",\"function\":\"print_list\",\"func_id\":6,\"num_params\":1,\"start_line\":77,\"end_line\":84,\"metrics\":{\"num_params\":1,\"call_count\":3,\"has_recursion\":false,\"has_loop\":true,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"list\",\"type\":\"struct string_list *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"print_list\",\"func_id\":6,\"name\":\"list\",\"type\":\"struct string_list *\",\"is_pointer\":true,\"is_const\":false}");
  printf("    list of size %ld: ", list->len);
  for (size_t i = 0; i < list->len; i++) {
    printf("%s, ", list->data[i]);
  }
  printf("\n");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"print_list\",\"func_id\":6}");
}

bool should_instrument_function(tree fndecl)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/instrument_attribute.c\",\"function\":\"should_instrument_function\",\"func_id\":5,\"num_params\":1,\"start_line\":86,\"end_line\":121,\"metrics\":{\"num_params\":1,\"call_count\":10,\"has_recursion\":false,\"has_loop\":true,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"fndecl\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"should_instrument_function\",\"func_id\":5,\"name\":\"fndecl\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
  // If the function has our attribute, enable instrumentation
  if (NULL_TREE != lookup_attribute(ATTRIBUTE_NAME, DECL_ATTRIBUTES(fndecl))) {
    DEBUG("    function instrumented from attribute: %s\n", get_name(fndecl));
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"should_instrument_function\",\"func_id\":5,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/instrument_attribute.c\",\"line\":93,\"ret_type\":\"int\",\"expr\":\"true\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"should_instrument_function\",\"func_id\":5}");
    return true;
  }

  // If the function's file is in the include-file-list, enable instrumentation
  if (include_files.len > 0) {
    const char * function_file = DECL_SOURCE_FILE(fndecl);

    // Check if an element in the list is a substring of the function's file's path
    DEBUG("    checking file: %s\n", function_file);
    const char * result = list_strstr(&include_files, function_file);
    if (NULL != result) {
      DEBUG("      function instrumented from file list: %s (%s)\n", result, get_name(fndecl));
      log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"should_instrument_function\",\"func_id\":5,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/instrument_attribute.c\",\"line\":107,\"ret_type\":\"int\",\"expr\":\"true\"}");
      log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"should_instrument_function\",\"func_id\":5}");
      return true;
    }
  }

  // If the function is in the include-function-list, enable instrumentation
  if (include_functions.len > 0) {
    const char * function_name = lang_hooks.decl_printable_name (fndecl, 1);

    // Check if the function name is in the list
    DEBUG("    checking function: %s\n", function_name);
    const char * result = list_strstr(&include_functions, function_name);
    if (NULL != result) {
      DEBUG("      function instrumented from function name list: %s\n", result);
      log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"should_instrument_function\",\"func_id\":5,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/instrument_attribute.c\",\"line\":122,\"ret_type\":\"int\",\"expr\":\"true\"}");
      log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"should_instrument_function\",\"func_id\":5}");
      return true;
    }
  }

  log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"should_instrument_function\",\"func_id\":5,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/instrument_attribute.c\",\"line\":128,\"ret_type\":\"int\",\"expr\":\"false\"}");
  log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"should_instrument_function\",\"func_id\":5}");
  return false;
}

void handle(void * event_data, void * data)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/instrument_attribute.c\",\"function\":\"handle\",\"func_id\":4,\"num_params\":2,\"start_line\":123,\"end_line\":150,\"metrics\":{\"num_params\":2,\"call_count\":0,\"has_recursion\":false,\"has_loop\":true,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"event_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle\",\"func_id\":4,\"name\":\"event_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle\",\"func_id\":4,\"name\":\"data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
  tree fndecl = (tree) event_data;

  // Make sure it's a function
  if (FUNCTION_DECL != TREE_CODE(fndecl)) {
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"handle\",\"func_id\":4,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/instrument_attribute.c\",\"line\":129,\"ret_type\":\"void\",\"expr\":\"\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"handle\",\"func_id\":4}");
    return;
  }

  // Check if the function should be instrumented
  if (should_instrument_function(fndecl)) {
    VERBOSE(
      "  instrumented function: (%s:%d) %s\n",
      DECL_SOURCE_FILE(fndecl),
      DECL_SOURCE_LINE(fndecl),
      get_name(fndecl));
    DECL_NO_INSTRUMENT_FUNCTION_ENTRY_EXIT(fndecl) = 0;
  }
  // Otherwise explicitly disable it
  else {
    DEBUG(
      "  NOT instrumented function: (%s:%d) %s\n",
      DECL_SOURCE_FILE(fndecl),
      DECL_SOURCE_LINE(fndecl),
      get_name(fndecl));
    DECL_NO_INSTRUMENT_FUNCTION_ENTRY_EXIT(fndecl) = 1;
  }
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"handle\",\"func_id\":4}");
}

void parse_plugin_args(struct plugin_name_args * plugin_info)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/instrument_attribute.c\",\"function\":\"parse_plugin_args\",\"func_id\":3,\"num_params\":1,\"start_line\":152,\"end_line\":185,\"metrics\":{\"num_params\":1,\"call_count\":6,\"has_recursion\":false,\"has_loop\":true,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"plugin_info\",\"type\":\"struct plugin_name_args *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"parse_plugin_args\",\"func_id\":3,\"name\":\"plugin_info\",\"type\":\"struct plugin_name_args *\",\"is_pointer\":true,\"is_const\":false}");
  const int argc = plugin_info->argc;
  struct plugin_argument * argv = plugin_info->argv;

  for (int i = 0; i < argc; ++i) {
    // Check for debug argument, and enable debug mode if found
    if (0 == strncmp(argv[i].key, ARG_DEBUG, strlen(ARG_DEBUG))) {
      is_debug = true;
      // Also enable verbose
      is_verbose = true;
    }
    // Check for file list
    else if (0 == strncmp(argv[i].key, ARG_INCLUDE_FILE_LIST, strlen(ARG_INCLUDE_FILE_LIST))) {
      char * include_file_list = argv[i].value;
      DEBUG("Plugin parameter:\n");
      DEBUG("  %s: %s\n", argv[i].key, include_file_list);
      split_str(include_file_list, LIST_DELIMITER, &include_files);
      if (is_debug) {
        print_list(&include_files);
      }
    }
    // Check for function list
    else if (0 == strncmp(argv[i].key, ARG_INCLUDE_FUNCTION_LIST, strlen(ARG_INCLUDE_FUNCTION_LIST))) {
      char * include_function_list = argv[i].value;
      DEBUG("Plugin parameter:\n");
      DEBUG("  %s: %s\n", argv[i].key, include_function_list);
      split_str(include_function_list, LIST_DELIMITER, &include_functions);
      if (is_debug) {
        print_list(&include_functions);
      }
    }
  }
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"parse_plugin_args\",\"func_id\":3}");
}

void check_verbose()
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/instrument_attribute.c\",\"function\":\"check_verbose\",\"func_id\":2,\"num_params\":0,\"start_line\":187,\"end_line\":193,\"metrics\":{\"num_params\":0,\"call_count\":2,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
  char * verbose_value = secure_getenv("VERBOSE");
  if (NULL != verbose_value && 0 == strncmp(verbose_value, "1", 1)) {
    is_verbose = true;
  }
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"check_verbose\",\"func_id\":2}");
}

void plugin_fini(void * gcc_data, void * user_data)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/instrument_attribute.c\",\"function\":\"plugin_fini\",\"func_id\":1,\"num_params\":2,\"start_line\":195,\"end_line\":199,\"metrics\":{\"num_params\":2,\"call_count\":2,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"gcc_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"user_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"plugin_fini\",\"func_id\":1,\"name\":\"gcc_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] gcc_data = %p\n", gcc_data);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"plugin_fini\",\"func_id\":1,\"name\":\"user_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] user_data = %p\n", user_data);
  free_list(&include_files);
  free_list(&include_functions);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"plugin_fini\",\"func_id\":1}");
}

int plugin_init(
  struct plugin_name_args * plugin_info,
  struct plugin_gcc_version * version)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/instrument_attribute.c\",\"function\":\"plugin_init\",\"func_id\":0,\"num_params\":2,\"start_line\":201,\"end_line\":235,\"metrics\":{\"num_params\":2,\"call_count\":3,\"has_recursion\":false,\"has_loop\":true,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"plugin_info\",\"type\":\"struct plugin_name_args *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"version\",\"type\":\"struct plugin_gcc_version *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"plugin_init\",\"func_id\":0,\"name\":\"plugin_info\",\"type\":\"struct plugin_name_args *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"plugin_init\",\"func_id\":0,\"name\":\"version\",\"type\":\"struct plugin_gcc_version *\",\"is_pointer\":true,\"is_const\":false}");
  check_verbose();
  parse_plugin_args(plugin_info);

  VERBOSE("Plugin: %s\n", PLUGIN_NAME);

  register_callback(
    plugin_info->base_name,
    PLUGIN_INFO,
    NULL,
    &info);

  register_callback(
    plugin_info->base_name,
    PLUGIN_ATTRIBUTES,
    register_attributes,
    NULL);

  register_callback(
    plugin_info->base_name,
    PLUGIN_FINISH_PARSE_FUNCTION,
    handle,
    NULL);

  register_callback(
    plugin_info->base_name,
    PLUGIN_FINISH,
    plugin_fini,
    NULL);

    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"plugin_init\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/instrument-attribute/instrument-attribute-gcc-plugin-main_instrumented/src/instrument_attribute.c\",\"line\":234,\"ret_type\":\"int\",\"expr\":\"0\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"plugin_init\",\"func_id\":0}");
  return 0;
}
