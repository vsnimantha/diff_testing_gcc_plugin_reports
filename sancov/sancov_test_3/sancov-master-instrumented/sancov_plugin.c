#include <time.h>
#include <stdio.h>
/*
 * Copyright 2011-2017 by Emese Revfy <re.emese@gmail.com>
 * Licensed under the GPL v2, or (at your option) v3
 *
 * Homepage:
 * https://github.com/ephox-gcc-plugins/sancov
 *
 * This plugin inserts a __sanitizer_cov_trace_pc() call at the start of basic blocks.
 * It supports all gcc versions with plugin support (from gcc-4.5 on).
 * It is based on the commit "Add fuzzing coverage support" by Dmitry Vyukov <dvyukov@google.com>.
 *
 * You can read about it more here:
 *  https://gcc.gnu.org/viewcvs/gcc?limit_changes=0&view=revision&revision=231296
 *  http://lwn.net/Articles/674854/
 *  https://github.com/google/syzkaller
 *  https://lwn.net/Articles/677764/
 *
 * Usage:
 * make run
 */

#include "gcc-common.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/sancov-master-instrumented/trace_sancov_plugin.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"sancov_plugin.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}

__visible int plugin_is_GPL_compatible;

tree sancov_fndecl;

static struct plugin_info sancov_plugin_info = {
	.version	= "20170102",
	.help		= "sancov plugin\n",
};

static unsigned int sancov_execute(void)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/sancov-master-instrumented/sancov_plugin.c\",\"function\":\"sancov_execute\",\"func_id\":2,\"num_params\":0,\"start_line\":33,\"end_line\":55,\"metrics\":{\"num_params\":0,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
	basic_block bb;

	/* Remove this line when this plugin and kcov will be in the kernel. */
	if (!strcmp(DECL_NAME_POINTER(current_function_decl), DECL_NAME_POINTER(sancov_fndecl))) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"sancov_execute\",\"func_id\":2,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/sancov-master-instrumented/sancov_plugin.c\",\"line\":39,\"ret_type\":\"unsigned int\",\"expr\":\"0\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"sancov_execute\",\"func_id\":2}");
	    return 0;
	}

	FOR_EACH_BB_FN(bb, cfun) {
		const_gimple stmt;
		gcall *gcall;
		gimple_stmt_iterator gsi = gsi_after_labels(bb);

		if (gsi_end_p(gsi))
			continue;

		stmt = gsi_stmt(gsi);
		gcall = as_a_gcall(gimple_build_call(sancov_fndecl, 0));
		gimple_set_location(gcall, gimple_location(stmt));
		gsi_insert_before(&gsi, gcall, GSI_SAME_STMT);
	}
	log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"sancov_execute\",\"func_id\":2,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/sancov-master-instrumented/sancov_plugin.c\",\"line\":58,\"ret_type\":\"unsigned int\",\"expr\":\"0\"}");
	log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"sancov_execute\",\"func_id\":2}");
	return 0;
}

#define PASS_NAME sancov

#define NO_GATE
#define TODO_FLAGS_FINISH TODO_dump_func | TODO_verify_stmts | TODO_update_ssa_no_phi | TODO_verify_flow

#include "gcc-generate-gimple-pass.h"

#if BUILDING_GCC_VERSION < 6000
static void sancov_start_unit(void __unused *gcc_data, void __unused *user_data)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/sancov-master-instrumented/sancov_plugin.c\",\"function\":\"sancov_start_unit\",\"func_id\":1,\"num_params\":2,\"start_line\":65,\"end_line\":87,\"metrics\":{\"num_params\":2,\"call_count\":9,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"gcc_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"user_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"sancov_start_unit\",\"func_id\":1,\"name\":\"gcc_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"sancov_start_unit\",\"func_id\":1,\"name\":\"user_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
	tree leaf_attr, nothrow_attr;
	tree BT_FN_VOID = build_function_type_list(void_type_node, NULL_TREE);

	sancov_fndecl = build_fn_decl("__sanitizer_cov_trace_pc", BT_FN_VOID);

	DECL_ASSEMBLER_NAME(sancov_fndecl);
	TREE_PUBLIC(sancov_fndecl) = 1;
	DECL_EXTERNAL(sancov_fndecl) = 1;
	DECL_ARTIFICIAL(sancov_fndecl) = 1;
	DECL_PRESERVE_P(sancov_fndecl) = 1;
	DECL_UNINLINABLE(sancov_fndecl) = 1;
	TREE_USED(sancov_fndecl) = 1;

	nothrow_attr = tree_cons(get_identifier("nothrow"), NULL, NULL);
	decl_attributes(&sancov_fndecl, nothrow_attr, 0);
	gcc_assert(TREE_NOTHROW(sancov_fndecl));
#if BUILDING_GCC_VERSION > 4005
	leaf_attr = tree_cons(get_identifier("leaf"), NULL, NULL);
	decl_attributes(&sancov_fndecl, leaf_attr, 0);
#endif
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"sancov_start_unit\",\"func_id\":1}");
}
#endif

__visible int plugin_init(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/sancov-master-instrumented/sancov_plugin.c\",\"function\":\"plugin_init\",\"func_id\":0,\"num_params\":2,\"start_line\":90,\"end_line\":141,\"metrics\":{\"num_params\":2,\"call_count\":2,\"has_recursion\":false,\"has_loop\":true,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"plugin_info\",\"type\":\"struct plugin_name_args *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"version\",\"type\":\"struct plugin_gcc_version *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"plugin_init\",\"func_id\":0,\"name\":\"plugin_info\",\"type\":\"struct plugin_name_args *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"plugin_init\",\"func_id\":0,\"name\":\"version\",\"type\":\"struct plugin_gcc_version *\",\"is_pointer\":true,\"is_const\":false}");
	int i;
	const char * const plugin_name = plugin_info->base_name;
	const int argc = plugin_info->argc;
	const struct plugin_argument * const argv = plugin_info->argv;
	bool enable = true;

	static const struct ggc_root_tab gt_ggc_r_gt_sancov[] = {
		{
			.base = &sancov_fndecl,
			.nelt = 1,
			.stride = sizeof(sancov_fndecl),
			.cb = &gt_ggc_mx_tree_node,
			.pchw = &gt_pch_nx_tree_node
		},
		LAST_GGC_ROOT_TAB
	};

	/* BBs can be split afterwards?? */
#if BUILDING_GCC_VERSION >= 4009
	PASS_INFO(sancov, "asan", 0, PASS_POS_INSERT_BEFORE);
#else
	PASS_INFO(sancov, "nrv", 1, PASS_POS_INSERT_BEFORE);
#endif

	if (!plugin_default_version_check(version, &gcc_version)) {
		error(G_("incompatible gcc/plugin versions"));
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"plugin_init\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/sancov-master-instrumented/sancov_plugin.c\",\"line\":118,\"ret_type\":\"int\",\"expr\":\"1\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"plugin_init\",\"func_id\":0}");
		return 1;
	}

	for (i = 0; i < argc; ++i) {
		if (!strcmp(argv[i].key, "no-sancov")) {
			enable = false;
			continue;
		}
		error(G_("unkown option '-fplugin-arg-%s-%s'"), plugin_name, argv[i].key);
	}

	register_callback(plugin_name, PLUGIN_INFO, NULL, &sancov_plugin_info);

	if (!enable) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"plugin_init\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/sancov-master-instrumented/sancov_plugin.c\",\"line\":132,\"ret_type\":\"int\",\"expr\":\"0\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"plugin_init\",\"func_id\":0}");
	    return 0;
	}

#if BUILDING_GCC_VERSION < 6000
	register_callback(plugin_name, PLUGIN_START_UNIT, &sancov_start_unit, NULL);
	register_callback(plugin_name, PLUGIN_REGISTER_GGC_ROOTS, NULL, (void *)&gt_ggc_r_gt_sancov);
	register_callback(plugin_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &sancov_pass_info);
#endif

    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"plugin_init\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/sancov-master-instrumented/sancov_plugin.c\",\"line\":140,\"ret_type\":\"int\",\"expr\":\"0\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"plugin_init\",\"func_id\":0}");
	return 0;
}
