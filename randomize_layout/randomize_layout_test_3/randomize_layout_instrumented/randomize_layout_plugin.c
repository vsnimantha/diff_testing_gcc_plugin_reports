#include <time.h>
#include <stdio.h>
/*
 * Copyright 2014-2016 by Open Source Security, Inc., Brad Spengler <spender@grsecurity.net>
 *                   and PaX Team <pageexec@freemail.hu>
 * Licensed under the GPL v2
 *
 * Note: the choice of the license means that the compilation process is
 *       NOT 'eligible' as defined by gcc's library exception to the GPL v3,
 *       but for the kernel it doesn't matter since it doesn't link against
 *       any of the gcc libraries
 *
 * Usage:
 * $ # for 4.5/4.6/C based 4.7
 * $ gcc -I`gcc -print-file-name=plugin`/include -I`gcc -print-file-name=plugin`/include/c-family -fPIC -shared -O2 -o randomize_layout_plugin.so randomize_layout_plugin.c
 * $ # for C++ based 4.7/4.8+
 * $ g++ -I`g++ -print-file-name=plugin`/include -I`g++ -print-file-name=plugin`/include/c-family -fPIC -shared -O2 -o randomize_layout_plugin.so randomize_layout_plugin.c
 * $ gcc -fplugin=./randomize_layout_plugin.so test.c -O2
 */

#include "gcc-common.h"
#include "randomize_layout_seed.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/trace_randomize_layout_plugin.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"randomize_layout_plugin.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}

#if BUILDING_GCC_MAJOR < 4 || (BUILDING_GCC_MAJOR == 4 && BUILDING_GCC_MINOR < 7)
#error "The RANDSTRUCT plugin requires GCC 4.7 or newer."
#endif

/* For out-of-tree builds on newer GCC, define PLUGIN_VERSION as a string. */
#ifndef PLUGIN_VERSION
#ifdef GCCPLUGIN_VERSION
/* Turn the numeric GCCPLUGIN_VERSION into a string literal. */
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define PLUGIN_VERSION STR(GCCPLUGIN_VERSION)
#else
#define PLUGIN_VERSION "20200101"
#endif
#endif


#define ORIG_TYPE_NAME(node) \
	(TYPE_NAME(TYPE_MAIN_VARIANT(node)) != NULL_TREE ? ((const unsigned char *)IDENTIFIER_POINTER(TYPE_NAME(TYPE_MAIN_VARIANT(node)))) : (const unsigned char *)"anonymous")

#define INFORM(loc, msg, ...)	inform(loc, "randstruct: " msg, ##__VA_ARGS__)
#define MISMATCH(loc, how, ...)	INFORM(loc, "casting between randomized structure pointer types (" how "): %qT and %qT\n", __VA_ARGS__)

__visible int plugin_is_GPL_compatible;

static int performance_mode;

static struct plugin_info randomize_layout_plugin_info = {
	.version	= PLUGIN_VERSION,
	.help		= "disable\t\t\tdo not activate plugin\n"
			  "performance-mode\tenable cacheline-aware layout randomization\n"
};

/* from old Linux dcache.h */
static inline unsigned long
partial_name_hash(unsigned long c, unsigned long prevhash)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"partial_name_hash\",\"func_id\":26,\"num_params\":2,\"start_line\":56,\"end_line\":60,\"metrics\":{\"num_params\":2,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"c\",\"type\":\"unsigned long\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"prevhash\",\"type\":\"unsigned long\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"partial_name_hash\",\"func_id\":26,\"name\":\"c\",\"type\":\"unsigned long\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"partial_name_hash\",\"func_id\":26,\"name\":\"prevhash\",\"type\":\"unsigned long\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"partial_name_hash\",\"func_id\":26,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":59,\"ret_type\":\"unsigned long\",\"expr\":\"(prevhash + (c << 4) + (c >> 4)) * 11\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"partial_name_hash\",\"func_id\":26}");
	return (prevhash + (c << 4) + (c >> 4)) * 11;
}
static inline unsigned int
name_hash(const unsigned char *name)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"name_hash\",\"func_id\":25,\"num_params\":1,\"start_line\":61,\"end_line\":69,\"metrics\":{\"num_params\":1,\"call_count\":2,\"has_recursion\":false,\"has_loop\":true,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"name\",\"type\":\"const unsigned char *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"name_hash\",\"func_id\":25,\"name\":\"name\",\"type\":\"const unsigned char *\",\"is_pointer\":true,\"is_const\":false}");
	unsigned long hash = 0;
	unsigned int len = strlen((const char *)name);
	while (len--)
		hash = partial_name_hash(*name++, hash);
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"name_hash\",\"func_id\":25,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":68,\"ret_type\":\"unsigned int\",\"expr\":\"(unsigned int)hash\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"name_hash\",\"func_id\":25}");
	return (unsigned int)hash;
}

static tree handle_randomize_layout_attr(tree *node, tree name, tree args, int flags, bool *no_add_attrs)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"handle_randomize_layout_attr\",\"func_id\":24,\"num_params\":5,\"start_line\":71,\"end_line\":111,\"metrics\":{\"num_params\":5,\"call_count\":4,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"node\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"name\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"args\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"flags\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"no_add_attrs\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle_randomize_layout_attr\",\"func_id\":24,\"name\":\"node\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle_randomize_layout_attr\",\"func_id\":24,\"name\":\"name\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle_randomize_layout_attr\",\"func_id\":24,\"name\":\"args\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle_randomize_layout_attr\",\"func_id\":24,\"name\":\"flags\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle_randomize_layout_attr\",\"func_id\":24,\"name\":\"no_add_attrs\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}");
	tree type;

	*no_add_attrs = true;
	if (TREE_CODE(*node) == FUNCTION_DECL) {
		error("%qE attribute does not apply to functions (%qF)", name, *node);
		log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"handle_randomize_layout_attr\",\"func_id\":24,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":84,\"ret_type\":\"int\",\"expr\":\"NULL_TREE\"}");
		log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"handle_randomize_layout_attr\",\"func_id\":24}");
		return NULL_TREE;
	}

	if (TREE_CODE(*node) == PARM_DECL) {
		error("%qE attribute does not apply to function parameters (%qD)", name, *node);
		log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"handle_randomize_layout_attr\",\"func_id\":24,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":91,\"ret_type\":\"int\",\"expr\":\"NULL_TREE\"}");
		log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"handle_randomize_layout_attr\",\"func_id\":24}");
		return NULL_TREE;
	}

	if (TREE_CODE(*node) == VAR_DECL) {
		error("%qE attribute does not apply to variables (%qD)", name, *node);
		log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"handle_randomize_layout_attr\",\"func_id\":24,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":98,\"ret_type\":\"int\",\"expr\":\"NULL_TREE\"}");
		log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"handle_randomize_layout_attr\",\"func_id\":24}");
		return NULL_TREE;
	}

	if (TYPE_P(*node)) {
		type = *node;
	} else {
		gcc_assert(TREE_CODE(*node) == TYPE_DECL);
		type = TREE_TYPE(*node);
	}

	if (TREE_CODE(type) != RECORD_TYPE) {
		error("%qE attribute used on %qT applies to struct types only", name, type);
		log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"handle_randomize_layout_attr\",\"func_id\":24,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":112,\"ret_type\":\"int\",\"expr\":\"NULL_TREE\"}");
		log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"handle_randomize_layout_attr\",\"func_id\":24}");
		return NULL_TREE;
	}

	if (lookup_attribute(IDENTIFIER_POINTER(name), TYPE_ATTRIBUTES(type))) {
		error("%qE attribute is already applied to the type %qT", name, type);
		log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"handle_randomize_layout_attr\",\"func_id\":24,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":119,\"ret_type\":\"int\",\"expr\":\"NULL_TREE\"}");
		log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"handle_randomize_layout_attr\",\"func_id\":24}");
		return NULL_TREE;
	}

	*no_add_attrs = false;

	log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"handle_randomize_layout_attr\",\"func_id\":24,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":126,\"ret_type\":\"int\",\"expr\":\"NULL_TREE\"}");
	log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"handle_randomize_layout_attr\",\"func_id\":24}");
	return NULL_TREE;
}

/* set on complete types that we don't need to inspect further at all */
static tree handle_randomize_considered_attr(tree *node, tree name, tree args, int flags, bool *no_add_attrs)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"handle_randomize_considered_attr\",\"func_id\":23,\"num_params\":5,\"start_line\":114,\"end_line\":118,\"metrics\":{\"num_params\":5,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"node\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"name\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"args\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"flags\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"no_add_attrs\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle_randomize_considered_attr\",\"func_id\":23,\"name\":\"node\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] node = %d\n", node);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle_randomize_considered_attr\",\"func_id\":23,\"name\":\"name\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] name = %d\n", name);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle_randomize_considered_attr\",\"func_id\":23,\"name\":\"args\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] args = %d\n", args);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle_randomize_considered_attr\",\"func_id\":23,\"name\":\"flags\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] flags = %d\n", flags);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle_randomize_considered_attr\",\"func_id\":23,\"name\":\"no_add_attrs\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] no_add_attrs = %d\n", no_add_attrs);
	*no_add_attrs = false;
	log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"handle_randomize_considered_attr\",\"func_id\":23,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":128,\"ret_type\":\"int\",\"expr\":\"NULL_TREE\"}");
	log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"handle_randomize_considered_attr\",\"func_id\":23}");
	return NULL_TREE;
}

/*
 * set on types that we've performed a shuffle on, to prevent re-shuffling
 * this does not preclude us from inspecting its fields for potential shuffles
 */
static tree handle_randomize_performed_attr(tree *node, tree name, tree args, int flags, bool *no_add_attrs)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"handle_randomize_performed_attr\",\"func_id\":22,\"num_params\":5,\"start_line\":124,\"end_line\":128,\"metrics\":{\"num_params\":5,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"node\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"name\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"args\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"flags\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"no_add_attrs\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle_randomize_performed_attr\",\"func_id\":22,\"name\":\"node\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] node = %d\n", node);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle_randomize_performed_attr\",\"func_id\":22,\"name\":\"name\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] name = %d\n", name);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle_randomize_performed_attr\",\"func_id\":22,\"name\":\"args\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] args = %d\n", args);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle_randomize_performed_attr\",\"func_id\":22,\"name\":\"flags\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] flags = %d\n", flags);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle_randomize_performed_attr\",\"func_id\":22,\"name\":\"no_add_attrs\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] no_add_attrs = %d\n", no_add_attrs);
	*no_add_attrs = false;
	log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"handle_randomize_performed_attr\",\"func_id\":22,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":138,\"ret_type\":\"int\",\"expr\":\"NULL_TREE\"}");
	log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"handle_randomize_performed_attr\",\"func_id\":22}");
	return NULL_TREE;
}

/*
 * 64bit variant of Bob Jenkins' public domain PRNG
 * 256 bits of internal state
 */

typedef unsigned long long u64;

typedef struct ranctx { u64 a; u64 b; u64 c; u64 d; } ranctx;

#define rot(x,k) (((x)<<(k))|((x)>>(64-(k))))
static u64 ranval(ranctx *x) {
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"ranval\",\"func_id\":21,\"num_params\":1,\"start_line\":140,\"end_line\":147,\"metrics\":{\"num_params\":1,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"x\",\"type\":\"ranctx *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"ranval\",\"func_id\":21,\"name\":\"x\",\"type\":\"ranctx *\",\"is_pointer\":true,\"is_const\":false}");
	u64 e = x->a - rot(x->b, 7);
	x->a = x->b ^ rot(x->c, 13);
	x->b = x->c + rot(x->d, 37);
	x->c = x->d + e;
	x->d = e + x->a;
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"ranval\",\"func_id\":21,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":146,\"ret_type\":\"u64\",\"expr\":\"x->d\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"ranval\",\"func_id\":21}");
	return x->d;
}

static void raninit(ranctx *x, u64 *seed) {
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"raninit\",\"func_id\":20,\"num_params\":2,\"start_line\":149,\"end_line\":159,\"metrics\":{\"num_params\":2,\"call_count\":1,\"has_recursion\":false,\"has_loop\":true,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"x\",\"type\":\"ranctx *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"seed\",\"type\":\"u64 *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"raninit\",\"func_id\":20,\"name\":\"x\",\"type\":\"ranctx *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"raninit\",\"func_id\":20,\"name\":\"seed\",\"type\":\"u64 *\",\"is_pointer\":true,\"is_const\":false}");
	int i;

	x->a = seed[0];
	x->b = seed[1];
	x->c = seed[2];
	x->d = seed[3];

	for (i=0; i < 30; ++i)
		(void)ranval(x);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"raninit\",\"func_id\":20}");
}

static u64 shuffle_seed[4];

struct partition_group {
	tree tree_start;
	unsigned long start;
	unsigned long length;
};

static void partition_struct(tree *fields, unsigned long length, struct partition_group *size_groups, unsigned long *num_groups)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"partition_struct\",\"func_id\":19,\"num_params\":4,\"start_line\":169,\"end_line\":203,\"metrics\":{\"num_params\":4,\"call_count\":3,\"has_recursion\":false,\"has_loop\":true,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"fields\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"length\",\"type\":\"unsigned long\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"size_groups\",\"type\":\"struct partition_group *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"num_groups\",\"type\":\"unsigned long *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"partition_struct\",\"func_id\":19,\"name\":\"fields\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"partition_struct\",\"func_id\":19,\"name\":\"length\",\"type\":\"unsigned long\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"partition_struct\",\"func_id\":19,\"name\":\"size_groups\",\"type\":\"struct partition_group *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"partition_struct\",\"func_id\":19,\"name\":\"num_groups\",\"type\":\"unsigned long *\",\"is_pointer\":true,\"is_const\":false}");
	unsigned long i;
	unsigned long accum_size = 0;
	unsigned long accum_length = 0;
	unsigned long group_idx = 0;

	gcc_assert(length < INT_MAX);

	memset(size_groups, 0, sizeof(struct partition_group) * length);

	for (i = 0; i < length; i++) {
		if (size_groups[group_idx].tree_start == NULL_TREE) {
			size_groups[group_idx].tree_start = fields[i];
			size_groups[group_idx].start = i;
			accum_length = 0;
			accum_size = 0;
		}
		accum_size += (unsigned long)int_size_in_bytes(TREE_TYPE(fields[i]));
		accum_length++;
		if (accum_size >= 64) {
			size_groups[group_idx].length = accum_length;
			accum_length = 0;
			group_idx++;
		}
	}

	if (size_groups[group_idx].tree_start != NULL_TREE &&
	    !size_groups[group_idx].length) {
		size_groups[group_idx].length = accum_length;
		group_idx++;
	}

	*num_groups = group_idx;
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"partition_struct\",\"func_id\":19}");
}

static void performance_shuffle(tree *newtree, unsigned long length, ranctx *prng_state)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"performance_shuffle\",\"func_id\":18,\"num_params\":3,\"start_line\":205,\"end_line\":240,\"metrics\":{\"num_params\":3,\"call_count\":5,\"has_recursion\":false,\"has_loop\":true,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"newtree\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"length\",\"type\":\"unsigned long\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"prng_state\",\"type\":\"ranctx *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"performance_shuffle\",\"func_id\":18,\"name\":\"newtree\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"performance_shuffle\",\"func_id\":18,\"name\":\"length\",\"type\":\"unsigned long\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"performance_shuffle\",\"func_id\":18,\"name\":\"prng_state\",\"type\":\"ranctx *\",\"is_pointer\":true,\"is_const\":false}");
	unsigned long i, x, index;
	struct partition_group size_group[length];
	unsigned long num_groups = 0;
	unsigned long randnum;

	partition_struct(newtree, length, (struct partition_group *)&size_group, &num_groups);

	/* FIXME: this group shuffle is currently a no-op. */
	for (i = num_groups - 1; i > 0; i--) {
		struct partition_group tmp;
		randnum = ranval(prng_state) % (i + 1);
		tmp = size_group[i];
		size_group[i] = size_group[randnum];
		size_group[randnum] = tmp;
	}

	for (x = 0; x < num_groups; x++) {
		for (index = size_group[x].length - 1; index > 0; index--) {
			tree tmp;

			i = size_group[x].start + index;
			if (DECL_BIT_FIELD_TYPE(newtree[i]))
				continue;
			randnum = ranval(prng_state) % (index + 1);
			randnum += size_group[x].start;
			// we could handle this case differently if desired
			if (DECL_BIT_FIELD_TYPE(newtree[randnum]))
				continue;
			tmp = newtree[i];
			newtree[i] = newtree[randnum];
			newtree[randnum] = tmp;
		}
	}
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"performance_shuffle\",\"func_id\":18}");
}

static void full_shuffle(tree *newtree, unsigned long length, ranctx *prng_state)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"full_shuffle\",\"func_id\":17,\"num_params\":3,\"start_line\":242,\"end_line\":253,\"metrics\":{\"num_params\":3,\"call_count\":1,\"has_recursion\":false,\"has_loop\":true,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"newtree\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"length\",\"type\":\"unsigned long\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"prng_state\",\"type\":\"ranctx *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"full_shuffle\",\"func_id\":17,\"name\":\"newtree\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] newtree = %d\n", newtree);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"full_shuffle\",\"func_id\":17,\"name\":\"length\",\"type\":\"unsigned long\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"full_shuffle\",\"func_id\":17,\"name\":\"prng_state\",\"type\":\"ranctx *\",\"is_pointer\":true,\"is_const\":false}");
	unsigned long i, randnum;

	for (i = length - 1; i > 0; i--) {
		tree tmp;
		randnum = ranval(prng_state) % (i + 1);
		tmp = newtree[i];
		newtree[i] = newtree[randnum];
		newtree[randnum] = tmp;
	}
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"full_shuffle\",\"func_id\":17}");
}

/* modern in-place Fisher-Yates shuffle */
static void shuffle(const_tree type, tree *newtree, unsigned long length)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"shuffle\",\"func_id\":16,\"num_params\":3,\"start_line\":256,\"end_line\":288,\"metrics\":{\"num_params\":3,\"call_count\":4,\"has_recursion\":false,\"has_loop\":true,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"type\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"newtree\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"length\",\"type\":\"unsigned long\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"shuffle\",\"func_id\":16,\"name\":\"type\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"shuffle\",\"func_id\":16,\"name\":\"newtree\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"shuffle\",\"func_id\":16,\"name\":\"length\",\"type\":\"unsigned long\",\"is_pointer\":false,\"is_const\":false}");
	unsigned long i;
	u64 seed[4];
	ranctx prng_state;
	const unsigned char *structname;

	if (length == 0) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"shuffle\",\"func_id\":16,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":264,\"ret_type\":\"void\",\"expr\":\"\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"shuffle\",\"func_id\":16}");
	    return;
	}

	gcc_assert(TREE_CODE(type) == RECORD_TYPE);

	structname = ORIG_TYPE_NAME(type);

#ifdef __DEBUG_PLUGIN
	fprintf(stderr, "Shuffling struct %s %p\n", (const char *)structname, type);
#ifdef __DEBUG_VERBOSE
	debug_tree((tree)type);
#endif
#endif

	for (i = 0; i < 4; i++) {
		seed[i] = shuffle_seed[i];
		seed[i] ^= name_hash(structname);
	}

	raninit(&prng_state, (u64 *)&seed);

	if (performance_mode)
		performance_shuffle(newtree, length, &prng_state);
	else
		full_shuffle(newtree, length, &prng_state);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"shuffle\",\"func_id\":16}");
}

static bool is_flexible_array(const_tree field)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"is_flexible_array\",\"func_id\":15,\"num_params\":1,\"start_line\":290,\"end_line\":308,\"metrics\":{\"num_params\":1,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"field\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"is_flexible_array\",\"func_id\":15,\"name\":\"field\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
	const_tree fieldtype;
	const_tree typesize;

	fieldtype = TREE_TYPE(field);
	typesize = TYPE_SIZE(fieldtype);

	if (TREE_CODE(fieldtype) != ARRAY_TYPE) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"is_flexible_array\",\"func_id\":15,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":301,\"ret_type\":\"int\",\"expr\":\"false\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"is_flexible_array\",\"func_id\":15}");
	    return false;
	}

	/* size of type is represented in bits */

	if (typesize == NULL_TREE && TYPE_DOMAIN(fieldtype) != NULL_TREE &&
	    TYPE_MAX_VALUE(TYPE_DOMAIN(fieldtype)) == NULL_TREE)
		log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"is_flexible_array\",\"func_id\":15,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":310,\"ret_type\":\"int\",\"expr\":\"true\"}");
		log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"is_flexible_array\",\"func_id\":15}");
		return true;

	log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"is_flexible_array\",\"func_id\":15,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":314,\"ret_type\":\"int\",\"expr\":\"false\"}");
	log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"is_flexible_array\",\"func_id\":15}");
	return false;
}

static int relayout_struct(tree type)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"relayout_struct\",\"func_id\":14,\"num_params\":1,\"start_line\":310,\"end_line\":404,\"metrics\":{\"num_params\":1,\"call_count\":7,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"type\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"relayout_struct\",\"func_id\":14,\"name\":\"type\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
	unsigned long num_fields = (unsigned long)list_length(TYPE_FIELDS(type));
	unsigned long shuffle_length = num_fields;
	tree field;
	tree newtree[num_fields];
	unsigned long i;
	tree list;
	tree variant;
	tree main_variant;
	expanded_location xloc;
	bool has_flexarray = false;

	if (TYPE_FIELDS(type) == NULL_TREE) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"relayout_struct\",\"func_id\":14,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":324,\"ret_type\":\"int\",\"expr\":\"0\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"relayout_struct\",\"func_id\":14}");
	    return 0;
	}

	if (num_fields < 2) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"relayout_struct\",\"func_id\":14,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":327,\"ret_type\":\"int\",\"expr\":\"0\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"relayout_struct\",\"func_id\":14}");
	    return 0;
	}

	gcc_assert(TREE_CODE(type) == RECORD_TYPE);

	gcc_assert(num_fields < INT_MAX);

	if (lookup_attribute("randomize_performed", TYPE_ATTRIBUTES(type)) ||
	    lookup_attribute("no_randomize_layout", TYPE_ATTRIBUTES(TYPE_MAIN_VARIANT(type))))
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"relayout_struct\",\"func_id\":14,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":335,\"ret_type\":\"int\",\"expr\":\"0\"}");
    printf("[LOG][RETURN_VALUE] %d\n", (0));
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"relayout_struct\",\"func_id\":14}");
		return 0;

	/* Workaround for 3rd-party VirtualBox source that we can't modify ourselves */
	if (!strcmp((const char *)ORIG_TYPE_NAME(type), "INTNETTRUNKFACTORY") ||
	    !strcmp((const char *)ORIG_TYPE_NAME(type), "RAWPCIFACTORY"))
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"relayout_struct\",\"func_id\":14,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":340,\"ret_type\":\"int\",\"expr\":\"0\"}");
    printf("[LOG][RETURN_VALUE] %d\n", (0));
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"relayout_struct\",\"func_id\":14}");
		return 0;

	/* throw out any structs in uapi */
	xloc = expand_location(DECL_SOURCE_LOCATION(TYPE_FIELDS(type)));

	if (strstr(xloc.file, "/uapi/"))
		error(G_("attempted to randomize userland API struct %s"), ORIG_TYPE_NAME(type));

	for (field = TYPE_FIELDS(type), i = 0; field; field = TREE_CHAIN(field), i++) {
		gcc_assert(TREE_CODE(field) == FIELD_DECL);
		newtree[i] = field;
	}

	/*
	 * enforce that we don't randomize the layout of the last
	 * element of a struct if it's a proper flexible array
	 */
	if (is_flexible_array(newtree[num_fields - 1])) {
		has_flexarray = true;
		shuffle_length--;
	}

	shuffle(type, (tree *)newtree, shuffle_length);

	/*
	 * set up a bogus anonymous struct field designed to error out on unnamed struct initializers
	 * as gcc provides no other way to detect such code
	 */
	list = make_node(FIELD_DECL);
	TREE_CHAIN(list) = newtree[0];
	TREE_TYPE(list) = void_type_node;
	DECL_SIZE(list) = bitsize_zero_node;
	DECL_NONADDRESSABLE_P(list) = 1;
	DECL_FIELD_BIT_OFFSET(list) = bitsize_zero_node;
	DECL_SIZE_UNIT(list) = size_zero_node;
	DECL_FIELD_OFFSET(list) = size_zero_node;
	DECL_CONTEXT(list) = type;
	// to satisfy the constify plugin
	TREE_READONLY(list) = 1;

	for (i = 0; i < num_fields - 1; i++)
		TREE_CHAIN(newtree[i]) = newtree[i+1];
	TREE_CHAIN(newtree[num_fields - 1]) = NULL_TREE;

	main_variant = TYPE_MAIN_VARIANT(type);
	for (variant = main_variant; variant; variant = TYPE_NEXT_VARIANT(variant)) {
		TYPE_FIELDS(variant) = list;
		TYPE_ATTRIBUTES(variant) = copy_list(TYPE_ATTRIBUTES(variant));
		TYPE_ATTRIBUTES(variant) = tree_cons(get_identifier("randomize_performed"), NULL_TREE, TYPE_ATTRIBUTES(variant));
		TYPE_ATTRIBUTES(variant) = tree_cons(get_identifier("designated_init"), NULL_TREE, TYPE_ATTRIBUTES(variant));
		if (has_flexarray)
			TYPE_ATTRIBUTES(type) = tree_cons(get_identifier("has_flexarray"), NULL_TREE, TYPE_ATTRIBUTES(type));
	}

	/*
	 * force a re-layout of the main variant
	 * the TYPE_SIZE for all variants will be recomputed
	 * by finalize_type_size()
	 */
	TYPE_SIZE(main_variant) = NULL_TREE;
	layout_type(main_variant);
	gcc_assert(TYPE_SIZE(main_variant) != NULL_TREE);

    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"relayout_struct\",\"func_id\":14,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":403,\"ret_type\":\"int\",\"expr\":\"1\"}");
    printf("[LOG][RETURN_VALUE] %d\n", (1));
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"relayout_struct\",\"func_id\":14}");
	return 1;
}

/* from constify plugin */
static const_tree get_field_type(const_tree field)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"get_field_type\",\"func_id\":13,\"num_params\":1,\"start_line\":407,\"end_line\":410,\"metrics\":{\"num_params\":1,\"call_count\":2,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"field\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"get_field_type\",\"func_id\":13,\"name\":\"field\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"get_field_type\",\"func_id\":13,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":409,\"ret_type\":\"int\",\"expr\":\"strip_array_types(TREE_TYPE(field))\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"get_field_type\",\"func_id\":13}");
	return strip_array_types(TREE_TYPE(field));
}

/* from constify plugin */
static bool is_fptr(const_tree fieldtype)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"is_fptr\",\"func_id\":12,\"num_params\":1,\"start_line\":413,\"end_line\":419,\"metrics\":{\"num_params\":1,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"fieldtype\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"is_fptr\",\"func_id\":12,\"name\":\"fieldtype\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
	if (TREE_CODE(fieldtype) != POINTER_TYPE) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"is_fptr\",\"func_id\":12,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":418,\"ret_type\":\"int\",\"expr\":\"false\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"is_fptr\",\"func_id\":12}");
	    return false;
	}

	log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"is_fptr\",\"func_id\":12,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":423,\"ret_type\":\"int\",\"expr\":\"TREE_CODE(TREE_TYPE(fieldtype)) == FUNCTION_TYPE\"}");
	log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"is_fptr\",\"func_id\":12}");
	return TREE_CODE(TREE_TYPE(fieldtype)) == FUNCTION_TYPE;
}

/* derived from constify plugin */
static int is_pure_ops_struct(const_tree node)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"is_pure_ops_struct\",\"func_id\":11,\"num_params\":1,\"start_line\":422,\"end_line\":446,\"metrics\":{\"num_params\":1,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"node\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"is_pure_ops_struct\",\"func_id\":11,\"name\":\"node\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
	const_tree field;

	gcc_assert(TREE_CODE(node) == RECORD_TYPE || TREE_CODE(node) == UNION_TYPE);

	for (field = TYPE_FIELDS(node); field; field = TREE_CHAIN(field)) {
		const_tree fieldtype = get_field_type(field);
		enum tree_code code = TREE_CODE(fieldtype);

		if (node == fieldtype)
			continue;

		if (code == RECORD_TYPE || code == UNION_TYPE) {
			if (!is_pure_ops_struct(fieldtype)) {
			    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"is_pure_ops_struct\",\"func_id\":11,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":439,\"ret_type\":\"int\",\"expr\":\"0\"}");
			    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"is_pure_ops_struct\",\"func_id\":11}");
			    return 0;
			}
			continue;
		}

		if (!is_fptr(fieldtype)) {
		    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"is_pure_ops_struct\",\"func_id\":11,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":447,\"ret_type\":\"int\",\"expr\":\"0\"}");
		    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"is_pure_ops_struct\",\"func_id\":11}");
		    return 0;
		}
	}

    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"is_pure_ops_struct\",\"func_id\":11,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":445,\"ret_type\":\"int\",\"expr\":\"1\"}");
    printf("[LOG][RETURN_VALUE] %d\n", (1));
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"is_pure_ops_struct\",\"func_id\":11}");
	return 1;
}

static void randomize_type(tree type)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"randomize_type\",\"func_id\":10,\"num_params\":1,\"start_line\":448,\"end_line\":470,\"metrics\":{\"num_params\":1,\"call_count\":7,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"type\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"randomize_type\",\"func_id\":10,\"name\":\"type\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
	tree variant;

	gcc_assert(TREE_CODE(type) == RECORD_TYPE);

	if (lookup_attribute("randomize_considered", TYPE_ATTRIBUTES(type))) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"randomize_type\",\"func_id\":10,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":455,\"ret_type\":\"void\",\"expr\":\"\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"randomize_type\",\"func_id\":10}");
	    return;
	}

	if (lookup_attribute("randomize_layout", TYPE_ATTRIBUTES(TYPE_MAIN_VARIANT(type))) || is_pure_ops_struct(type))
		relayout_struct(type);

	for (variant = TYPE_MAIN_VARIANT(type); variant; variant = TYPE_NEXT_VARIANT(variant)) {
		TYPE_ATTRIBUTES(type) = copy_list(TYPE_ATTRIBUTES(type));
		TYPE_ATTRIBUTES(type) = tree_cons(get_identifier("randomize_considered"), NULL_TREE, TYPE_ATTRIBUTES(type));
	}
#ifdef __DEBUG_PLUGIN
	fprintf(stderr, "Marking randomize_considered on struct %s\n", ORIG_TYPE_NAME(type));
#ifdef __DEBUG_VERBOSE
	debug_tree(type);
#endif
#endif
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"randomize_type\",\"func_id\":10}");
}

static void update_decl_size(tree decl)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"update_decl_size\",\"func_id\":9,\"num_params\":1,\"start_line\":472,\"end_line\":514,\"metrics\":{\"num_params\":1,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"decl\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"update_decl_size\",\"func_id\":9,\"name\":\"decl\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
	tree lastval, lastidx, field, init, type, flexsize;
	unsigned HOST_WIDE_INT len;

	type = TREE_TYPE(decl);

	if (!lookup_attribute("has_flexarray", TYPE_ATTRIBUTES(type))) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"update_decl_size\",\"func_id\":9,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":480,\"ret_type\":\"void\",\"expr\":\"\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"update_decl_size\",\"func_id\":9}");
	    return;
	}

	init = DECL_INITIAL(decl);
	if (init == NULL_TREE || init == error_mark_node) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"update_decl_size\",\"func_id\":9,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":484,\"ret_type\":\"void\",\"expr\":\"\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"update_decl_size\",\"func_id\":9}");
	    return;
	}

	if (TREE_CODE(init) != CONSTRUCTOR) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"update_decl_size\",\"func_id\":9,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":487,\"ret_type\":\"void\",\"expr\":\"\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"update_decl_size\",\"func_id\":9}");
	    return;
	}

	len = CONSTRUCTOR_NELTS(init);
        if (!len) {
            log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"update_decl_size\",\"func_id\":9,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":491,\"ret_type\":\"void\",\"expr\":\"\"}");
            log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"update_decl_size\",\"func_id\":9}");
            return;
        }

	lastval = CONSTRUCTOR_ELT(init, CONSTRUCTOR_NELTS(init) - 1)->value;
	lastidx = CONSTRUCTOR_ELT(init, CONSTRUCTOR_NELTS(init) - 1)->index;

	for (field = TYPE_FIELDS(TREE_TYPE(decl)); TREE_CHAIN(field); field = TREE_CHAIN(field))
		;

	if (lastidx != field) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"update_decl_size\",\"func_id\":9,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":500,\"ret_type\":\"void\",\"expr\":\"\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"update_decl_size\",\"func_id\":9}");
	    return;
	}

	if (TREE_CODE(lastval) != STRING_CST) {
		error("Only string constants are supported as initializers "
		      "for randomized structures with flexible arrays");
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"update_decl_size\",\"func_id\":9,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":505,\"ret_type\":\"void\",\"expr\":\"\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"update_decl_size\",\"func_id\":9}");
		return;
	}

	flexsize = bitsize_int(TREE_STRING_LENGTH(lastval) *
		tree_to_uhwi(TYPE_SIZE(TREE_TYPE(TREE_TYPE(lastval)))));

	DECL_SIZE(decl) = size_binop(PLUS_EXPR, TYPE_SIZE(type), flexsize);

    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"update_decl_size\",\"func_id\":9,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":513,\"ret_type\":\"void\",\"expr\":\"\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"update_decl_size\",\"func_id\":9}");
	return;
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"update_decl_size\",\"func_id\":9}");
}


static void randomize_layout_finish_decl(void *event_data, void *data)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"randomize_layout_finish_decl\",\"func_id\":8,\"num_params\":2,\"start_line\":517,\"end_line\":543,\"metrics\":{\"num_params\":2,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"event_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"randomize_layout_finish_decl\",\"func_id\":8,\"name\":\"event_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"randomize_layout_finish_decl\",\"func_id\":8,\"name\":\"data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
	tree decl = (tree)event_data;
	tree type;

	if (decl == NULL_TREE || decl == error_mark_node) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"randomize_layout_finish_decl\",\"func_id\":8,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":523,\"ret_type\":\"void\",\"expr\":\"\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"randomize_layout_finish_decl\",\"func_id\":8}");
	    return;
	}

	type = TREE_TYPE(decl);

	if (TREE_CODE(decl) != VAR_DECL) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"randomize_layout_finish_decl\",\"func_id\":8,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":528,\"ret_type\":\"void\",\"expr\":\"\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"randomize_layout_finish_decl\",\"func_id\":8}");
	    return;
	}

	if (TREE_CODE(type) != RECORD_TYPE && TREE_CODE(type) != UNION_TYPE) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"randomize_layout_finish_decl\",\"func_id\":8,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":531,\"ret_type\":\"void\",\"expr\":\"\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"randomize_layout_finish_decl\",\"func_id\":8}");
	    return;
	}

	if (!lookup_attribute("randomize_performed", TYPE_ATTRIBUTES(type))) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"randomize_layout_finish_decl\",\"func_id\":8,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":534,\"ret_type\":\"void\",\"expr\":\"\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"randomize_layout_finish_decl\",\"func_id\":8}");
	    return;
	}

	DECL_SIZE(decl) = 0;
	DECL_SIZE_UNIT(decl) = 0;
	SET_DECL_ALIGN(decl, 0);
	SET_DECL_MODE (decl, VOIDmode);
	SET_DECL_RTL(decl, 0);
	update_decl_size(decl);
	layout_decl(decl, 0);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"randomize_layout_finish_decl\",\"func_id\":8}");
}

static void finish_type(void *event_data, void *data)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"finish_type\",\"func_id\":7,\"num_params\":2,\"start_line\":545,\"end_line\":570,\"metrics\":{\"num_params\":2,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"event_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"finish_type\",\"func_id\":7,\"name\":\"event_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"finish_type\",\"func_id\":7,\"name\":\"data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
	tree type = (tree)event_data;

	if (type == NULL_TREE || type == error_mark_node) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"finish_type\",\"func_id\":7,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":550,\"ret_type\":\"void\",\"expr\":\"\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"finish_type\",\"func_id\":7}");
	    return;
	}

	if (TREE_CODE(type) != RECORD_TYPE) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"finish_type\",\"func_id\":7,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":553,\"ret_type\":\"void\",\"expr\":\"\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"finish_type\",\"func_id\":7}");
	    return;
	}

	if (TYPE_FIELDS(type) == NULL_TREE) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"finish_type\",\"func_id\":7,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":556,\"ret_type\":\"void\",\"expr\":\"\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"finish_type\",\"func_id\":7}");
	    return;
	}

	if (lookup_attribute("randomize_considered", TYPE_ATTRIBUTES(type))) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"finish_type\",\"func_id\":7,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":559,\"ret_type\":\"void\",\"expr\":\"\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"finish_type\",\"func_id\":7}");
	    return;
	}

#ifdef __DEBUG_PLUGIN
	fprintf(stderr, "Calling randomize_type on %s\n", ORIG_TYPE_NAME(type));
#endif
#ifdef __DEBUG_VERBOSE
	debug_tree(type);
#endif
	randomize_type(type);

    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"finish_type\",\"func_id\":7,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":569,\"ret_type\":\"void\",\"expr\":\"\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"finish_type\",\"func_id\":7}");
	return;
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"finish_type\",\"func_id\":7}");
}

static struct attribute_spec randomize_layout_attr = { };
static struct attribute_spec no_randomize_layout_attr = { };
static struct attribute_spec randomize_considered_attr = { };
static struct attribute_spec randomize_performed_attr = { };

static void register_attributes(void *event_data, void *data)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"register_attributes\",\"func_id\":6,\"num_params\":2,\"start_line\":577,\"end_line\":601,\"metrics\":{\"num_params\":2,\"call_count\":4,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"event_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"register_attributes\",\"func_id\":6,\"name\":\"event_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] event_data = %p\n", event_data);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"register_attributes\",\"func_id\":6,\"name\":\"data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] data = %p\n", data);
	randomize_layout_attr.name		= "randomize_layout";
	randomize_layout_attr.type_required	= true;
	randomize_layout_attr.handler		= handle_randomize_layout_attr;
	randomize_layout_attr.affects_type_identity = true;

	no_randomize_layout_attr.name		= "no_randomize_layout";
	no_randomize_layout_attr.type_required	= true;
	no_randomize_layout_attr.handler	= handle_randomize_layout_attr;
	no_randomize_layout_attr.affects_type_identity = true;

	randomize_considered_attr.name		= "randomize_considered";
	randomize_considered_attr.type_required	= true;
	randomize_considered_attr.handler	= handle_randomize_considered_attr;

	randomize_performed_attr.name		= "randomize_performed";
	randomize_performed_attr.type_required	= true;
	randomize_performed_attr.handler	= handle_randomize_performed_attr;

	register_attribute(&randomize_layout_attr);
	register_attribute(&no_randomize_layout_attr);
	register_attribute(&randomize_considered_attr);
	register_attribute(&randomize_performed_attr);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"register_attributes\",\"func_id\":6}");
}

static void check_bad_casts_in_constructor(tree var, tree init)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"check_bad_casts_in_constructor\",\"func_id\":5,\"num_params\":2,\"start_line\":603,\"end_line\":642,\"metrics\":{\"num_params\":2,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"var\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"init\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"check_bad_casts_in_constructor\",\"func_id\":5,\"name\":\"var\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"check_bad_casts_in_constructor\",\"func_id\":5,\"name\":\"init\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
	unsigned HOST_WIDE_INT idx;
	tree field, val;
	tree field_type, val_type;

	FOR_EACH_CONSTRUCTOR_ELT(CONSTRUCTOR_ELTS(init), idx, field, val) {
		if (TREE_CODE(val) == CONSTRUCTOR) {
			check_bad_casts_in_constructor(var, val);
			continue;
		}

		/* pipacs' plugin creates franken-arrays that differ from those produced by
		   normal code which all have valid 'field' trees. work around this */
		if (field == NULL_TREE)
			continue;
		field_type = TREE_TYPE(field);
		val_type = TREE_TYPE(val);

		if (TREE_CODE(field_type) != POINTER_TYPE || TREE_CODE(val_type) != POINTER_TYPE)
			continue;

		if (field_type == val_type)
			continue;

		field_type = TYPE_MAIN_VARIANT(strip_array_types(TYPE_MAIN_VARIANT(TREE_TYPE(field_type))));
		val_type = TYPE_MAIN_VARIANT(strip_array_types(TYPE_MAIN_VARIANT(TREE_TYPE(val_type))));

		if (field_type == void_type_node)
			continue;
		if (field_type == val_type)
			continue;
		if (TREE_CODE(val_type) != RECORD_TYPE)
			continue;

		if (!lookup_attribute("randomize_performed", TYPE_ATTRIBUTES(val_type)))
			continue;
		MISMATCH(DECL_SOURCE_LOCATION(var), "constructor\n", TYPE_MAIN_VARIANT(field_type), TYPE_MAIN_VARIANT(val_type));
	}
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"check_bad_casts_in_constructor\",\"func_id\":5}");
}

/* derived from the constify plugin */
static void check_global_variables(void *event_data, void *data)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"check_global_variables\",\"func_id\":4,\"num_params\":2,\"start_line\":645,\"end_line\":661,\"metrics\":{\"num_params\":2,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"event_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"check_global_variables\",\"func_id\":4,\"name\":\"event_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"check_global_variables\",\"func_id\":4,\"name\":\"data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
	struct varpool_node *node;
	tree init;

	FOR_EACH_VARIABLE(node) {
		tree var = NODE_DECL(node);
		init = DECL_INITIAL(var);
		if (init == NULL_TREE)
			continue;

		if (TREE_CODE(init) != CONSTRUCTOR)
			continue;

		check_bad_casts_in_constructor(var, init);
	}
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"check_global_variables\",\"func_id\":4}");
}

static bool dominated_by_is_err(const_tree rhs, basic_block bb)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"dominated_by_is_err\",\"func_id\":3,\"num_params\":2,\"start_line\":663,\"end_line\":717,\"metrics\":{\"num_params\":2,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"rhs\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"bb\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"dominated_by_is_err\",\"func_id\":3,\"name\":\"rhs\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"dominated_by_is_err\",\"func_id\":3,\"name\":\"bb\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
	basic_block dom;
	gimple dom_stmt;
	gimple call_stmt;
	const_tree dom_lhs;
	const_tree poss_is_err_cond;
	const_tree poss_is_err_func;
	const_tree is_err_arg;

	dom = get_immediate_dominator(CDI_DOMINATORS, bb);
	if (!dom) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"dominated_by_is_err\",\"func_id\":3,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":678,\"ret_type\":\"int\",\"expr\":\"false\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"dominated_by_is_err\",\"func_id\":3}");
	    return false;
	}

	dom_stmt = last_stmt(dom);
	if (!dom_stmt) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"dominated_by_is_err\",\"func_id\":3,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":685,\"ret_type\":\"int\",\"expr\":\"false\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"dominated_by_is_err\",\"func_id\":3}");
	    return false;
	}

	if (gimple_code(dom_stmt) != GIMPLE_COND) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"dominated_by_is_err\",\"func_id\":3,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":691,\"ret_type\":\"int\",\"expr\":\"false\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"dominated_by_is_err\",\"func_id\":3}");
	    return false;
	}

	if (gimple_cond_code(dom_stmt) != NE_EXPR) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"dominated_by_is_err\",\"func_id\":3,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":697,\"ret_type\":\"int\",\"expr\":\"false\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"dominated_by_is_err\",\"func_id\":3}");
	    return false;
	}

	if (!integer_zerop(gimple_cond_rhs(dom_stmt))) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"dominated_by_is_err\",\"func_id\":3,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":703,\"ret_type\":\"int\",\"expr\":\"false\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"dominated_by_is_err\",\"func_id\":3}");
	    return false;
	}

	poss_is_err_cond = gimple_cond_lhs(dom_stmt);

	if (TREE_CODE(poss_is_err_cond) != SSA_NAME) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"dominated_by_is_err\",\"func_id\":3,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":711,\"ret_type\":\"int\",\"expr\":\"false\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"dominated_by_is_err\",\"func_id\":3}");
	    return false;
	}

	call_stmt = SSA_NAME_DEF_STMT(poss_is_err_cond);

	if (gimple_code(call_stmt) != GIMPLE_CALL) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"dominated_by_is_err\",\"func_id\":3,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":719,\"ret_type\":\"int\",\"expr\":\"false\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"dominated_by_is_err\",\"func_id\":3}");
	    return false;
	}

	dom_lhs = gimple_get_lhs(call_stmt);
	poss_is_err_func = gimple_call_fndecl(call_stmt);
	if (!poss_is_err_func) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"dominated_by_is_err\",\"func_id\":3,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":727,\"ret_type\":\"int\",\"expr\":\"false\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"dominated_by_is_err\",\"func_id\":3}");
	    return false;
	}
	if (dom_lhs != poss_is_err_cond) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"dominated_by_is_err\",\"func_id\":3,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":732,\"ret_type\":\"int\",\"expr\":\"false\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"dominated_by_is_err\",\"func_id\":3}");
	    return false;
	}
	if (strcmp(DECL_NAME_POINTER(poss_is_err_func), "IS_ERR")) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"dominated_by_is_err\",\"func_id\":3,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":737,\"ret_type\":\"int\",\"expr\":\"false\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"dominated_by_is_err\",\"func_id\":3}");
	    return false;
	}

	is_err_arg = gimple_call_arg(call_stmt, 0);
	if (!is_err_arg) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"dominated_by_is_err\",\"func_id\":3,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":744,\"ret_type\":\"int\",\"expr\":\"false\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"dominated_by_is_err\",\"func_id\":3}");
	    return false;
	}

	if (is_err_arg != rhs) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"dominated_by_is_err\",\"func_id\":3,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":750,\"ret_type\":\"int\",\"expr\":\"false\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"dominated_by_is_err\",\"func_id\":3}");
	    return false;
	}

	log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"dominated_by_is_err\",\"func_id\":3,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":755,\"ret_type\":\"int\",\"expr\":\"true\"}");
	log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"dominated_by_is_err\",\"func_id\":3}");
	return true;
}

static void handle_local_var_initializers(void)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"handle_local_var_initializers\",\"func_id\":2,\"num_params\":0,\"start_line\":719,\"end_line\":732,\"metrics\":{\"num_params\":0,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
	tree var;
	unsigned int i;

	FOR_EACH_LOCAL_DECL(cfun, i, var) {
		tree init = DECL_INITIAL(var);
		if (!init)
			continue;
		if (TREE_CODE(init) != CONSTRUCTOR)
			continue;
		check_bad_casts_in_constructor(var, init);
	}
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"handle_local_var_initializers\",\"func_id\":2}");
}

/*
 * iterate over all statements to find "bad" casts:
 * those where the address of the start of a structure is cast
 * to a pointer of a structure of a different type, or a
 * structure pointer type is cast to a different structure pointer type
 */
static unsigned int find_bad_casts_execute(void)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"find_bad_casts_execute\",\"func_id\":1,\"num_params\":0,\"start_line\":740,\"end_line\":849,\"metrics\":{\"num_params\":0,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
	basic_block bb;

	handle_local_var_initializers();

	FOR_EACH_BB_FN(bb, cfun) {
		gimple_stmt_iterator gsi;

		for (gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
			gimple stmt;
			const_tree lhs;
			const_tree lhs_type;
			const_tree rhs1;
			const_tree rhs_type;
			const_tree ptr_lhs_type;
			const_tree ptr_rhs_type;
			const_tree op0;
			const_tree op0_type;
			enum tree_code rhs_code;

			stmt = gsi_stmt(gsi);

#ifdef __DEBUG_PLUGIN
#ifdef __DEBUG_VERBOSE
			debug_gimple_stmt(stmt);
			debug_tree(gimple_get_lhs(stmt));
#endif
#endif

			if (gimple_code(stmt) != GIMPLE_ASSIGN)
				continue;

#ifdef __DEBUG_PLUGIN
#ifdef __DEBUG_VERBOSE
			debug_tree(gimple_assign_rhs1(stmt));
#endif
#endif


			rhs_code = gimple_assign_rhs_code(stmt);

			if (rhs_code != ADDR_EXPR && rhs_code != SSA_NAME)
				continue;

			lhs = gimple_get_lhs(stmt);
			lhs_type = TREE_TYPE(lhs);
			rhs1 = gimple_assign_rhs1(stmt);
			rhs_type = TREE_TYPE(rhs1);

			if (TREE_CODE(rhs_type) != POINTER_TYPE ||
			    TREE_CODE(lhs_type) != POINTER_TYPE)
				continue;

			ptr_lhs_type = TYPE_MAIN_VARIANT(strip_array_types(TYPE_MAIN_VARIANT(TREE_TYPE(lhs_type))));
			ptr_rhs_type = TYPE_MAIN_VARIANT(strip_array_types(TYPE_MAIN_VARIANT(TREE_TYPE(rhs_type))));

			if (ptr_rhs_type == void_type_node)
				continue;

			if (ptr_lhs_type == void_type_node)
				continue;

			if (dominated_by_is_err(rhs1, bb))
				continue;

			if (TREE_CODE(ptr_rhs_type) != RECORD_TYPE) {
#ifndef __DEBUG_PLUGIN
				if (lookup_attribute("randomize_performed", TYPE_ATTRIBUTES(ptr_lhs_type)))
#endif
				MISMATCH(gimple_location(stmt), "rhs", ptr_lhs_type, ptr_rhs_type);
				continue;
			}

			if (rhs_code == SSA_NAME && ptr_lhs_type == ptr_rhs_type)
				continue;

			if (rhs_code == ADDR_EXPR) {
				op0 = TREE_OPERAND(rhs1, 0);

				if (op0 == NULL_TREE)
					continue;

				if (TREE_CODE(op0) != VAR_DECL)
					continue;

				op0_type = TYPE_MAIN_VARIANT(strip_array_types(TYPE_MAIN_VARIANT(TREE_TYPE(op0))));
				if (op0_type == ptr_lhs_type)
					continue;

#ifndef __DEBUG_PLUGIN
				if (lookup_attribute("randomize_performed", TYPE_ATTRIBUTES(op0_type)))
#endif
				MISMATCH(gimple_location(stmt), "op0", ptr_lhs_type, op0_type);
			} else {
				const_tree ssa_name_var = SSA_NAME_VAR(rhs1);
				/* skip bogus type casts introduced by container_of */
				if (ssa_name_var != NULL_TREE && DECL_NAME(ssa_name_var) && 
				    !strcmp((const char *)DECL_NAME_POINTER(ssa_name_var), "__mptr"))
					continue;
#ifndef __DEBUG_PLUGIN
				if (lookup_attribute("randomize_performed", TYPE_ATTRIBUTES(ptr_rhs_type)))
#endif
				MISMATCH(gimple_location(stmt), "ssa", ptr_lhs_type, ptr_rhs_type);
			}

		}
	}
	log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"find_bad_casts_execute\",\"func_id\":1,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":849,\"ret_type\":\"unsigned int\",\"expr\":\"0\"}");
	log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"find_bad_casts_execute\",\"func_id\":1}");
	return 0;
}

#define PASS_NAME find_bad_casts
#define NO_GATE
#define TODO_FLAGS_FINISH TODO_dump_func
#include "gcc-generate-gimple-pass.h"

__visible int plugin_init(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"function\":\"plugin_init\",\"func_id\":0,\"num_params\":2,\"start_line\":856,\"end_line\":914,\"metrics\":{\"num_params\":2,\"call_count\":9,\"has_recursion\":false,\"has_loop\":true,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"plugin_info\",\"type\":\"struct plugin_name_args *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"version\",\"type\":\"struct plugin_gcc_version *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"plugin_init\",\"func_id\":0,\"name\":\"plugin_info\",\"type\":\"struct plugin_name_args *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"plugin_init\",\"func_id\":0,\"name\":\"version\",\"type\":\"struct plugin_gcc_version *\",\"is_pointer\":true,\"is_const\":false}");
	int i;
	const char * const plugin_name = plugin_info->base_name;
	const int argc = plugin_info->argc;
	const struct plugin_argument * const argv = plugin_info->argv;
	bool enable = true;
	int obtained_seed = 0;
	struct register_pass_info find_bad_casts_pass_info;

	find_bad_casts_pass_info.pass			= make_find_bad_casts_pass();
	find_bad_casts_pass_info.reference_pass_name	= "ssa";
	find_bad_casts_pass_info.ref_pass_instance_number	= 1;
	find_bad_casts_pass_info.pos_op			= PASS_POS_INSERT_AFTER;

	if (!plugin_default_version_check(version, &gcc_version)) {
		error(G_("incompatible gcc/plugin versions"));
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"plugin_init\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":873,\"ret_type\":\"int\",\"expr\":\"1\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"plugin_init\",\"func_id\":0}");
		return 1;
	}

	if (strncmp(lang_hooks.name, "GNU C", 5) && !strncmp(lang_hooks.name, "GNU C+", 6)) {
		inform(UNKNOWN_LOCATION, G_("%s supports C only, not %s"), plugin_name, lang_hooks.name);
		enable = false;
	}

	for (i = 0; i < argc; ++i) {
		if (!strcmp(argv[i].key, "disable")) {
			enable = false;
			continue;
		}
		if (!strcmp(argv[i].key, "performance-mode")) {
			performance_mode = 1;
			continue;
		}
		error(G_("unknown option '-fplugin-arg-%s-%s'"), plugin_name, argv[i].key);
	}

	if (strlen(randstruct_seed) != 64) {
		error(G_("invalid seed value supplied for %s plugin"), plugin_name);
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"plugin_init\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":895,\"ret_type\":\"int\",\"expr\":\"1\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"plugin_init\",\"func_id\":0}");
		return 1;
	}
	obtained_seed = sscanf(randstruct_seed, "%016llx%016llx%016llx%016llx",
		&shuffle_seed[0], &shuffle_seed[1], &shuffle_seed[2], &shuffle_seed[3]);
	if (obtained_seed != 4) {
		error(G_("Invalid seed supplied for %s plugin"), plugin_name);
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"plugin_init\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":901,\"ret_type\":\"int\",\"expr\":\"1\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"plugin_init\",\"func_id\":0}");
		return 1;
	}

	register_callback(plugin_name, PLUGIN_INFO, NULL, &randomize_layout_plugin_info);
	if (enable) {
		register_callback(plugin_name, PLUGIN_ALL_IPA_PASSES_START, check_global_variables, NULL);
		register_callback(plugin_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &find_bad_casts_pass_info);
		register_callback(plugin_name, PLUGIN_FINISH_TYPE, finish_type, NULL);
		register_callback(plugin_name, PLUGIN_FINISH_DECL, randomize_layout_finish_decl, NULL);
	}
	register_callback(plugin_name, PLUGIN_ATTRIBUTES, register_attributes, NULL);

    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"plugin_init\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/randomize_layout_instrumented/randomize_layout_plugin.c\",\"line\":913,\"ret_type\":\"int\",\"expr\":\"0\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"plugin_init\",\"func_id\":0}");
	return 0;
}
