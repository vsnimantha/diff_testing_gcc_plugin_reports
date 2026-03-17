#include <time.h>
#include <stdio.h>
// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2013-2017 by PaX Team <pageexec@freemail.hu>
 *
 * Note: the choice of the license means that the compilation process is
 *       NOT 'eligible' as defined by gcc's library exception to the GPL v3,
 *       but for the kernel it doesn't matter since it doesn't link against
 *       any of the gcc libraries
 *
 * gcc plugin to forcibly initialize certain local variables that could
 * otherwise leak kernel stack to userland if they aren't properly initialized
 * by later code
 *
 * Homepage: https://pax.grsecurity.net/
 *
 * Options:
 * -fplugin-arg-structleak_plugin-disable
 * -fplugin-arg-structleak_plugin-verbose
 * -fplugin-arg-structleak_plugin-byref
 * -fplugin-arg-structleak_plugin-byref-all
 *
 * Usage:
 * $ # for 4.5/4.6/C based 4.7
 * $ gcc -I`gcc -print-file-name=plugin`/include -I`gcc -print-file-name=plugin`/include/c-family -fPIC -shared -O2 -o structleak_plugin.so structleak_plugin.c
 * $ # for C++ based 4.7/4.8+
 * $ g++ -I`g++ -print-file-name=plugin`/include -I`g++ -print-file-name=plugin`/include/c-family -fPIC -shared -O2 -o structleak_plugin.so structleak_plugin.c
 * $ gcc -fplugin=./structleak_plugin.so test.c -O2
 *
 * TODO: eliminate redundant initializers
 */

#include "gcc-common.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/trace_structleak_plugin.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"structleak_plugin.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}

#ifndef PLUGIN_VERSION
# ifdef GCCPLUGIN_VERSION
#  define STR_HELPER(x) #x
#  define STR(x) STR_HELPER(x)
#  define PLUGIN_VERSION STR(GCCPLUGIN_VERSION)
# else
#  define PLUGIN_VERSION "20200101"
# endif
#endif

/* unused C type flag in all versions 4.5-6 */
#define TYPE_USERSPACE(TYPE) TYPE_LANG_FLAG_5(TYPE)

__visible int plugin_is_GPL_compatible;

static struct plugin_info structleak_plugin_info = {
	.version	= PLUGIN_VERSION,
	.help		= "disable\tdo not activate plugin\n"
			  "byref\tinit structs passed by reference\n"
			  "byref-all\tinit anything passed by reference\n"
			  "verbose\tprint all initialized variables\n",
};

#define BYREF_STRUCT	1
#define BYREF_ALL	2

static bool verbose;
static int byref;

static tree handle_user_attribute(tree *node, tree name, tree args, int flags, bool *no_add_attrs)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/structleak_plugin.c\",\"function\":\"handle_user_attribute\",\"func_id\":7,\"num_params\":5,\"start_line\":63,\"end_line\":73,\"metrics\":{\"num_params\":5,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"node\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"name\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"args\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"flags\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"no_add_attrs\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle_user_attribute\",\"func_id\":7,\"name\":\"node\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle_user_attribute\",\"func_id\":7,\"name\":\"name\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle_user_attribute\",\"func_id\":7,\"name\":\"args\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle_user_attribute\",\"func_id\":7,\"name\":\"flags\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"handle_user_attribute\",\"func_id\":7,\"name\":\"no_add_attrs\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}");
	*no_add_attrs = true;

	/* check for types? for now accept everything linux has to offer */
	if (TREE_CODE(*node) != FIELD_DECL) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"handle_user_attribute\",\"func_id\":7,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/structleak_plugin.c\",\"line\":75,\"ret_type\":\"int\",\"expr\":\"NULL_TREE\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"handle_user_attribute\",\"func_id\":7}");
	    return NULL_TREE;
	}

	*no_add_attrs = false;
	log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"handle_user_attribute\",\"func_id\":7,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/structleak_plugin.c\",\"line\":81,\"ret_type\":\"int\",\"expr\":\"NULL_TREE\"}");
	log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"handle_user_attribute\",\"func_id\":7}");
	return NULL_TREE;
}

static struct attribute_spec user_attr = { };

static void register_attributes(void *event_data, void *data)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/structleak_plugin.c\",\"function\":\"register_attributes\",\"func_id\":6,\"num_params\":2,\"start_line\":77,\"end_line\":84,\"metrics\":{\"num_params\":2,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"event_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"register_attributes\",\"func_id\":6,\"name\":\"event_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] event_data = %p\n", event_data);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"register_attributes\",\"func_id\":6,\"name\":\"data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] data = %p\n", data);
	user_attr.name			= "user";
	user_attr.handler		= handle_user_attribute;
	user_attr.affects_type_identity	= true;

	register_attribute(&user_attr);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"register_attributes\",\"func_id\":6}");
}

static tree get_field_type(tree field)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/structleak_plugin.c\",\"function\":\"get_field_type\",\"func_id\":5,\"num_params\":1,\"start_line\":86,\"end_line\":89,\"metrics\":{\"num_params\":1,\"call_count\":2,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"field\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"get_field_type\",\"func_id\":5,\"name\":\"field\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"get_field_type\",\"func_id\":5,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/structleak_plugin.c\",\"line\":88,\"ret_type\":\"int\",\"expr\":\"strip_array_types(TREE_TYPE(field))\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"get_field_type\",\"func_id\":5}");
	return strip_array_types(TREE_TYPE(field));
}

static bool is_userspace_type(tree type)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/structleak_plugin.c\",\"function\":\"is_userspace_type\",\"func_id\":4,\"num_params\":1,\"start_line\":91,\"end_line\":107,\"metrics\":{\"num_params\":1,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"type\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"is_userspace_type\",\"func_id\":4,\"name\":\"type\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
	tree field;

	for (field = TYPE_FIELDS(type); field; field = TREE_CHAIN(field)) {
		tree fieldtype = get_field_type(field);
		enum tree_code code = TREE_CODE(fieldtype);

		if (code == RECORD_TYPE || code == UNION_TYPE)
			if (is_userspace_type(fieldtype)) {
			    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"is_userspace_type\",\"func_id\":4,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/structleak_plugin.c\",\"line\":103,\"ret_type\":\"int\",\"expr\":\"true\"}");
			    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"is_userspace_type\",\"func_id\":4}");
			    return true;
			}

		if (lookup_attribute("user", DECL_ATTRIBUTES(field))) {
		    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"is_userspace_type\",\"func_id\":4,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/structleak_plugin.c\",\"line\":109,\"ret_type\":\"int\",\"expr\":\"true\"}");
		    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"is_userspace_type\",\"func_id\":4}");
		    return true;
		}
	}
	log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"is_userspace_type\",\"func_id\":4,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/structleak_plugin.c\",\"line\":114,\"ret_type\":\"int\",\"expr\":\"false\"}");
	log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"is_userspace_type\",\"func_id\":4}");
	return false;
}

static void finish_type(void *event_data, void *data)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/structleak_plugin.c\",\"function\":\"finish_type\",\"func_id\":3,\"num_params\":2,\"start_line\":109,\"end_line\":124,\"metrics\":{\"num_params\":2,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"event_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"finish_type\",\"func_id\":3,\"name\":\"event_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"finish_type\",\"func_id\":3,\"name\":\"data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
	tree type = (tree)event_data;

	if (type == NULL_TREE || type == error_mark_node) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"finish_type\",\"func_id\":3,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/structleak_plugin.c\",\"line\":114,\"ret_type\":\"void\",\"expr\":\"\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"finish_type\",\"func_id\":3}");
	    return;
	}

	if (TREE_CODE(type) == ENUMERAL_TYPE) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"finish_type\",\"func_id\":3,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/structleak_plugin.c\",\"line\":117,\"ret_type\":\"void\",\"expr\":\"\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"finish_type\",\"func_id\":3}");
	    return;
	}

	if (TYPE_USERSPACE(type)) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"finish_type\",\"func_id\":3,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/structleak_plugin.c\",\"line\":120,\"ret_type\":\"void\",\"expr\":\"\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"finish_type\",\"func_id\":3}");
	    return;
	}

	if (is_userspace_type(type))
		TYPE_USERSPACE(type) = 1;
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"finish_type\",\"func_id\":3}");
}

static void initialize(tree var)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/structleak_plugin.c\",\"function\":\"initialize\",\"func_id\":2,\"num_params\":1,\"start_line\":126,\"end_line\":176,\"metrics\":{\"num_params\":1,\"call_count\":3,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"var\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"initialize\",\"func_id\":2,\"name\":\"var\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
	basic_block bb;
	gimple_stmt_iterator gsi;
	tree initializer;
	gimple init_stmt;
	tree type;

	/* this is the original entry bb before the forced split */
	bb = single_succ(ENTRY_BLOCK_PTR_FOR_FN(cfun));

	/* first check if variable is already initialized, warn otherwise */
	for (gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
		gimple stmt = gsi_stmt(gsi);
		tree rhs1;

		/* we're looking for an assignment of a single rhs... */
		if (!gimple_assign_single_p(stmt))
			continue;
		rhs1 = gimple_assign_rhs1(stmt);
		/* ... of a non-clobbering expression... */
		if (TREE_CLOBBER_P(rhs1))
			continue;
		/* ... to our variable... */
		if (gimple_get_lhs(stmt) != var)
			continue;
		/* if it's an initializer then we're good */
		if (TREE_CODE(rhs1) == CONSTRUCTOR) {
		    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"initialize\",\"func_id\":2,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/structleak_plugin.c\",\"line\":156,\"ret_type\":\"void\",\"expr\":\"\"}");
		    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"initialize\",\"func_id\":2}");
		    return;
		}
	}

	/* these aren't the 0days you're looking for */
	if (verbose)
		inform(DECL_SOURCE_LOCATION(var),
			"%s variable will be forcibly initialized",
			(byref && TREE_ADDRESSABLE(var)) ? "byref"
							 : "userspace");

	/* build the initializer expression */
	type = TREE_TYPE(var);
	if (AGGREGATE_TYPE_P(type))
		initializer = build_constructor(type, NULL);
	else
		initializer = fold_convert(type, integer_zero_node);

	/* build the initializer stmt */
	init_stmt = gimple_build_assign(var, initializer);
	gsi = gsi_after_labels(single_succ(ENTRY_BLOCK_PTR_FOR_FN(cfun)));
	gsi_insert_before(&gsi, init_stmt, GSI_NEW_STMT);
	update_stmt(init_stmt);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"initialize\",\"func_id\":2}");
}

static unsigned int structleak_execute(void)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/structleak_plugin.c\",\"function\":\"structleak_execute\",\"func_id\":1,\"num_params\":0,\"start_line\":178,\"end_line\":211,\"metrics\":{\"num_params\":0,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
	basic_block bb;
	tree var;
	unsigned int i;

	/* split the first bb where we can put the forced initializers */
	gcc_assert(single_succ_p(ENTRY_BLOCK_PTR_FOR_FN(cfun)));
	bb = single_succ(ENTRY_BLOCK_PTR_FOR_FN(cfun));
	if (!single_pred_p(bb)) {
		split_edge(single_succ_edge(ENTRY_BLOCK_PTR_FOR_FN(cfun)));
		gcc_assert(single_succ_p(ENTRY_BLOCK_PTR_FOR_FN(cfun)));
	}

	/* enumerate all local variables and forcibly initialize our targets */
	FOR_EACH_LOCAL_DECL(cfun, i, var) {
		tree type = TREE_TYPE(var);

		gcc_assert(DECL_P(var));
		if (!auto_var_in_fn_p(var, current_function_decl))
			continue;

		/* only care about structure types unless byref-all */
		if (byref != BYREF_ALL && TREE_CODE(type) != RECORD_TYPE && TREE_CODE(type) != UNION_TYPE)
			continue;

		/* if the type is of interest, examine the variable */
		if (TYPE_USERSPACE(type) ||
		    (byref && TREE_ADDRESSABLE(var)))
			initialize(var);
	}

	log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"structleak_execute\",\"func_id\":1,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/structleak_plugin.c\",\"line\":211,\"ret_type\":\"unsigned int\",\"expr\":\"0\"}");
	log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"structleak_execute\",\"func_id\":1}");
	return 0;
}

#define PASS_NAME structleak
#define NO_GATE
#define PROPERTIES_REQUIRED PROP_cfg
#define TODO_FLAGS_FINISH TODO_verify_il | TODO_verify_ssa | TODO_verify_stmts | TODO_dump_func | TODO_remove_unused_locals | TODO_update_ssa | TODO_ggc_collect | TODO_verify_flow
#include "gcc-generate-gimple-pass.h"

__visible int plugin_init(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/structleak_plugin.c\",\"function\":\"plugin_init\",\"func_id\":0,\"num_params\":2,\"start_line\":219,\"end_line\":267,\"metrics\":{\"num_params\":2,\"call_count\":2,\"has_recursion\":false,\"has_loop\":true,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"plugin_info\",\"type\":\"struct plugin_name_args *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"version\",\"type\":\"struct plugin_gcc_version *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"plugin_init\",\"func_id\":0,\"name\":\"plugin_info\",\"type\":\"struct plugin_name_args *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"plugin_init\",\"func_id\":0,\"name\":\"version\",\"type\":\"struct plugin_gcc_version *\",\"is_pointer\":true,\"is_const\":false}");
	int i;
	const char * const plugin_name = plugin_info->base_name;
	const int argc = plugin_info->argc;
	const struct plugin_argument * const argv = plugin_info->argv;
	bool enable = true;

	PASS_INFO(structleak, "early_optimizations", 1, PASS_POS_INSERT_BEFORE);

	if (!plugin_default_version_check(version, &gcc_version)) {
		error(G_("incompatible gcc/plugin versions"));
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"plugin_init\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/structleak_plugin.c\",\"line\":231,\"ret_type\":\"int\",\"expr\":\"1\"}");
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
		if (!strcmp(argv[i].key, "verbose")) {
			verbose = true;
			continue;
		}
		if (!strcmp(argv[i].key, "byref")) {
			byref = BYREF_STRUCT;
			continue;
		}
		if (!strcmp(argv[i].key, "byref-all")) {
			byref = BYREF_ALL;
			continue;
		}
		error(G_("unknown option '-fplugin-arg-%s-%s'"), plugin_name, argv[i].key);
	}

	register_callback(plugin_name, PLUGIN_INFO, NULL, &structleak_plugin_info);
	if (enable) {
		register_callback(plugin_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &structleak_pass_info);
		register_callback(plugin_name, PLUGIN_FINISH_TYPE, finish_type, NULL);
	}
	register_callback(plugin_name, PLUGIN_ATTRIBUTES, register_attributes, NULL);

    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"plugin_init\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/struck_Leak_instrumented/structleak_plugin.c\",\"line\":266,\"ret_type\":\"int\",\"expr\":\"0\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"plugin_init\",\"func_id\":0}");
	return 0;
}
