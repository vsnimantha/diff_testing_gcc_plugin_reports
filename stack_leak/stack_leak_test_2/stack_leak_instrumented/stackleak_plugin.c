#include <time.h>
#include <stdio.h>
// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2011-2017 by the PaX Team <pageexec@freemail.hu>
 * Modified by Alexander Popov <alex.popov@linux.com>
 *
 * Note: the choice of the license means that the compilation process is
 * NOT 'eligible' as defined by gcc's library exception to the GPL v3,
 * but for the kernel it doesn't matter since it doesn't link against
 * any of the gcc libraries
 *
 * This gcc plugin is needed for tracking the lowest border of the kernel stack.
 * It instruments the kernel code inserting stackleak_track_stack() calls:
 *  - after alloca();
 *  - for the functions with a stack frame size greater than or equal
 *     to the "track-min-size" plugin parameter.
 *
 * This plugin is ported from grsecurity/PaX. For more information see:
 *   https://grsecurity.net/
 *   https://pax.grsecurity.net/
 *
 * Debugging:
 *  - use fprintf() to stderr, debug_generic_expr(), debug_gimple_stmt(),
 *     print_rtl_single() and debug_rtx();
 *  - add "-fdump-tree-all -fdump-rtl-all" to the plugin CFLAGS in
 *     Makefile.gcc-plugins to see the verbose dumps of the gcc passes;
 *  - use gcc -E to understand the preprocessing shenanigans;
 *  - use gcc with enabled CFG/GIMPLE/SSA verification (--enable-checking).
 */

#include "gcc-common.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/trace_stackleak_plugin.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"stackleak_plugin.c\",\"msg\":%s}\n",
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


__visible int plugin_is_GPL_compatible;

static int track_frame_size = -1;
static bool build_for_x86 = false;
static const char track_function[] = "stackleak_track_stack";
static bool disable = false;
static bool verbose = false;

/*
 * Mark these global variables (roots) for gcc garbage collector since
 * they point to the garbage-collected memory.
 */
static GTY(()) tree track_function_decl;

static struct plugin_info stackleak_plugin_info = {
	.version = PLUGIN_VERSION,
	.help = "track-min-size=nn\ttrack stack for functions with a stack frame size >= nn bytes\n"
		"arch=target_arch\tspecify target build arch\n"
		"disable\t\tdo not activate the plugin\n"
		"verbose\t\tprint info about the instrumentation\n"
};

static void add_stack_tracking_gcall(gimple_stmt_iterator *gsi, bool after)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"function\":\"add_stack_tracking_gcall\",\"func_id\":15,\"num_params\":2,\"start_line\":65,\"end_line\":87,\"metrics\":{\"num_params\":2,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"gsi\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"after\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"add_stack_tracking_gcall\",\"func_id\":15,\"name\":\"gsi\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] gsi = %d\n", gsi);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"add_stack_tracking_gcall\",\"func_id\":15,\"name\":\"after\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] after = %d\n", after);
	gimple stmt;
	gcall *gimple_call;
	cgraph_node_ptr node;
	basic_block bb;

	/* Insert calling stackleak_track_stack() */
	stmt = gimple_build_call(track_function_decl, 0);
	gimple_call = as_a_gcall(stmt);
	if (after)
		gsi_insert_after(gsi, gimple_call, GSI_CONTINUE_LINKING);
	else
		gsi_insert_before(gsi, gimple_call, GSI_SAME_STMT);

	/* Update the cgraph */
	bb = gimple_bb(gimple_call);
	node = cgraph_get_create_node(track_function_decl);
	gcc_assert(node);
	cgraph_create_edge(cgraph_get_node(current_function_decl), node,
			gimple_call, bb->count,
			compute_call_stmt_bb_frequency(current_function_decl, bb));
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"add_stack_tracking_gcall\",\"func_id\":15}");
}

static bool is_alloca(gimple stmt)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"function\":\"is_alloca\",\"func_id\":14,\"num_params\":1,\"start_line\":89,\"end_line\":98,\"metrics\":{\"num_params\":1,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"stmt\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"is_alloca\",\"func_id\":14,\"name\":\"stmt\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] stmt = %d\n", stmt);
	if (gimple_call_builtin_p(stmt, BUILT_IN_ALLOCA)) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"is_alloca\",\"func_id\":14,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":95,\"ret_type\":\"int\",\"expr\":\"true\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"is_alloca\",\"func_id\":14}");
	    return true;
	}

	if (gimple_call_builtin_p(stmt, BUILT_IN_ALLOCA_WITH_ALIGN)) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"is_alloca\",\"func_id\":14,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":101,\"ret_type\":\"int\",\"expr\":\"true\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"is_alloca\",\"func_id\":14}");
	    return true;
	}

	log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"is_alloca\",\"func_id\":14,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":106,\"ret_type\":\"int\",\"expr\":\"false\"}");
	log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"is_alloca\",\"func_id\":14}");
	return false;
}

static tree get_current_stack_pointer_decl(void)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"function\":\"get_current_stack_pointer_decl\",\"func_id\":13,\"num_params\":0,\"start_line\":100,\"end_line\":122,\"metrics\":{\"num_params\":0,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
	varpool_node_ptr node;

	FOR_EACH_VARIABLE(node) {
		tree var = NODE_DECL(node);
		tree name = DECL_NAME(var);

		if (DECL_NAME_LENGTH(var) != sizeof("current_stack_pointer") - 1)
			continue;

		if (strcmp(IDENTIFIER_POINTER(name), "current_stack_pointer"))
			continue;

		log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"get_current_stack_pointer_decl\",\"func_id\":13,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":115,\"ret_type\":\"int\",\"expr\":\"var\"}");
		log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"get_current_stack_pointer_decl\",\"func_id\":13}");
		return var;
	}

	if (verbose) {
		fprintf(stderr, "stackleak: missing current_stack_pointer in %s()\n",
			DECL_NAME_POINTER(current_function_decl));
	}
	log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"get_current_stack_pointer_decl\",\"func_id\":13,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":124,\"ret_type\":\"int\",\"expr\":\"NULL_TREE\"}");
	log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"get_current_stack_pointer_decl\",\"func_id\":13}");
	return NULL_TREE;
}

static void add_stack_tracking_gasm(gimple_stmt_iterator *gsi, bool after)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"function\":\"add_stack_tracking_gasm\",\"func_id\":12,\"num_params\":2,\"start_line\":124,\"end_line\":159,\"metrics\":{\"num_params\":2,\"call_count\":2,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"gsi\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"after\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"add_stack_tracking_gasm\",\"func_id\":12,\"name\":\"gsi\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] gsi = %d\n", gsi);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"add_stack_tracking_gasm\",\"func_id\":12,\"name\":\"after\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] after = %d\n", after);
	gasm *asm_call = NULL;
	tree sp_decl, input;
	vec<tree, va_gc> *inputs = NULL;

	/* 'no_caller_saved_registers' is currently supported only for x86 */
	gcc_assert(build_for_x86);

	/*
	 * Insert calling stackleak_track_stack() in asm:
	 *   asm volatile("call stackleak_track_stack"
	 *		  :: "r" (current_stack_pointer))
	 * Use ASM_CALL_CONSTRAINT trick from arch/x86/include/asm/asm.h.
	 * This constraint is taken into account during gcc shrink-wrapping
	 * optimization. It is needed to be sure that stackleak_track_stack()
	 * call is inserted after the prologue of the containing function,
	 * when the stack frame is prepared.
	 */
	sp_decl = get_current_stack_pointer_decl();
	if (sp_decl == NULL_TREE) {
		add_stack_tracking_gcall(gsi, after);
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"add_stack_tracking_gasm\",\"func_id\":12,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":146,\"ret_type\":\"void\",\"expr\":\"\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"add_stack_tracking_gasm\",\"func_id\":12}");
		return;
	}
	input = build_tree_list(NULL_TREE, build_const_char_string(2, "r"));
	input = chainon(NULL_TREE, build_tree_list(input, sp_decl));
	vec_safe_push(inputs, input);
	asm_call = gimple_build_asm_vec("call stackleak_track_stack",
					inputs, NULL, NULL, NULL);
	gimple_asm_set_volatile(asm_call, true);
	if (after)
		gsi_insert_after(gsi, asm_call, GSI_CONTINUE_LINKING);
	else
		gsi_insert_before(gsi, asm_call, GSI_SAME_STMT);
	update_stmt(asm_call);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"add_stack_tracking_gasm\",\"func_id\":12}");
}

static void add_stack_tracking(gimple_stmt_iterator *gsi, bool after)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"function\":\"add_stack_tracking\",\"func_id\":11,\"num_params\":2,\"start_line\":161,\"end_line\":175,\"metrics\":{\"num_params\":2,\"call_count\":4,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"gsi\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"after\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"add_stack_tracking\",\"func_id\":11,\"name\":\"gsi\",\"type\":\"int *\",\"is_pointer\":true,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] gsi = %d\n", gsi);
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"add_stack_tracking\",\"func_id\":11,\"name\":\"after\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    printf("[LOG][PARAM_VALUE] after = %d\n", after);
	/*
	 * The 'no_caller_saved_registers' attribute is used for
	 * stackleak_track_stack(). If the compiler supports this attribute for
	 * the target arch, we can add calling stackleak_track_stack() in asm.
	 * That improves performance: we avoid useless operations with the
	 * caller-saved registers in the functions from which we will remove
	 * stackleak_track_stack() call during the stackleak_cleanup pass.
	 */
	if (lookup_attribute_spec(get_identifier("no_caller_saved_registers")))
		add_stack_tracking_gasm(gsi, after);
	else
		add_stack_tracking_gcall(gsi, after);
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"add_stack_tracking\",\"func_id\":11}");
}

/*
 * Work with the GIMPLE representation of the code. Insert the
 * stackleak_track_stack() call after alloca() and into the beginning
 * of the function if it is not instrumented.
 */
static unsigned int stackleak_instrument_execute(void)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"function\":\"stackleak_instrument_execute\",\"func_id\":10,\"num_params\":0,\"start_line\":182,\"end_line\":268,\"metrics\":{\"num_params\":0,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
	basic_block bb, entry_bb;
	bool prologue_instrumented = false, is_leaf = true;
	gimple_stmt_iterator gsi = { 0 };

	/*
	 * ENTRY_BLOCK_PTR is a basic block which represents possible entry
	 * point of a function. This block does not contain any code and
	 * has a CFG edge to its successor.
	 */
	gcc_assert(single_succ_p(ENTRY_BLOCK_PTR_FOR_FN(cfun)));
	entry_bb = single_succ(ENTRY_BLOCK_PTR_FOR_FN(cfun));

	/*
	 * Loop through the GIMPLE statements in each of cfun basic blocks.
	 * cfun is a global variable which represents the function that is
	 * currently processed.
	 */
	FOR_EACH_BB_FN(bb, cfun) {
		for (gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
			gimple stmt;

			stmt = gsi_stmt(gsi);

			/* Leaf function is a function which makes no calls */
			if (is_gimple_call(stmt))
				is_leaf = false;

			if (!is_alloca(stmt))
				continue;

			if (verbose) {
				fprintf(stderr, "stackleak: be careful, alloca() in %s()\n",
					DECL_NAME_POINTER(current_function_decl));
			}

			/* Insert stackleak_track_stack() call after alloca() */
			add_stack_tracking(&gsi, true);
			if (bb == entry_bb)
				prologue_instrumented = true;
		}
	}

	if (prologue_instrumented) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"stackleak_instrument_execute\",\"func_id\":10,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":228,\"ret_type\":\"unsigned int\",\"expr\":\"0\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"stackleak_instrument_execute\",\"func_id\":10}");
	    return 0;
	}

	/*
	 * Special cases to skip the instrumentation.
	 *
	 * Taking the address of static inline functions materializes them,
	 * but we mustn't instrument some of them as the resulting stack
	 * alignment required by the function call ABI will break other
	 * assumptions regarding the expected (but not otherwise enforced)
	 * register clobbering ABI.
	 *
	 * Case in point: native_save_fl on amd64 when optimized for size
	 * clobbers rdx if it were instrumented here.
	 *
	 * TODO: any more special cases?
	 */
	if (is_leaf &&
	    !TREE_PUBLIC(current_function_decl) &&
	    DECL_DECLARED_INLINE_P(current_function_decl)) {
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"stackleak_instrument_execute\",\"func_id\":10,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":246,\"ret_type\":\"unsigned int\",\"expr\":\"0\"}");
    printf("[LOG][RETURN_VALUE] %d\n", (0));
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"stackleak_instrument_execute\",\"func_id\":10}");
		return 0;
	}

	if (is_leaf &&
	    !strncmp(IDENTIFIER_POINTER(DECL_NAME(current_function_decl)),
		     "_paravirt_", 10)) {
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"stackleak_instrument_execute\",\"func_id\":10,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":252,\"ret_type\":\"unsigned int\",\"expr\":\"0\"}");
    printf("[LOG][RETURN_VALUE] %d\n", (0));
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"stackleak_instrument_execute\",\"func_id\":10}");
		return 0;
	}

	/* Insert stackleak_track_stack() call at the function beginning */
	bb = entry_bb;
	if (!single_pred_p(bb)) {
		/* gcc_assert(bb_loop_depth(bb) ||
				(bb->flags & BB_IRREDUCIBLE_LOOP)); */
		split_edge(single_succ_edge(ENTRY_BLOCK_PTR_FOR_FN(cfun)));
		gcc_assert(single_succ_p(ENTRY_BLOCK_PTR_FOR_FN(cfun)));
		bb = single_succ(ENTRY_BLOCK_PTR_FOR_FN(cfun));
	}
	gsi = gsi_after_labels(bb);
	add_stack_tracking(&gsi, false);

    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"stackleak_instrument_execute\",\"func_id\":10,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":267,\"ret_type\":\"unsigned int\",\"expr\":\"0\"}");
    printf("[LOG][RETURN_VALUE] %d\n", (0));
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"stackleak_instrument_execute\",\"func_id\":10}");
	return 0;
}

static bool large_stack_frame(void)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"function\":\"large_stack_frame\",\"func_id\":9,\"num_params\":0,\"start_line\":270,\"end_line\":277,\"metrics\":{\"num_params\":0,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
#if BUILDING_GCC_VERSION >= 8000
	log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"large_stack_frame\",\"func_id\":9,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":274,\"ret_type\":\"int\",\"expr\":\"maybe_ge(get_frame_size(), track_frame_size)\"}");
	log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"large_stack_frame\",\"func_id\":9}");
	return maybe_ge(get_frame_size(), track_frame_size);
#else
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"large_stack_frame\",\"func_id\":9,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":275,\"ret_type\":\"int\",\"expr\":\"(get_frame_size() >= track_frame_size)\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"large_stack_frame\",\"func_id\":9}");
	return (get_frame_size() >= track_frame_size);
#endif
}

static void remove_stack_tracking_gcall(void)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"function\":\"remove_stack_tracking_gcall\",\"func_id\":8,\"num_params\":0,\"start_line\":279,\"end_line\":343,\"metrics\":{\"num_params\":0,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
	rtx_insn *insn, *next;

	/*
	 * Find stackleak_track_stack() calls. Loop through the chain of insns,
	 * which is an RTL representation of the code for a function.
	 *
	 * The example of a matching insn:
	 *  (call_insn 8 4 10 2 (call (mem (symbol_ref ("stackleak_track_stack")
	 *  [flags 0x41] <function_decl 0x7f7cd3302a80 stackleak_track_stack>)
	 *  [0 stackleak_track_stack S1 A8]) (0)) 675 {*call} (expr_list
	 *  (symbol_ref ("stackleak_track_stack") [flags 0x41] <function_decl
	 *  0x7f7cd3302a80 stackleak_track_stack>) (expr_list (0) (nil))) (nil))
	 */
	for (insn = get_insns(); insn; insn = next) {
		rtx body;

		next = NEXT_INSN(insn);

		/* Check the expression code of the insn */
		if (!CALL_P(insn))
			continue;

		/*
		 * Check the expression code of the insn body, which is an RTL
		 * Expression (RTX) describing the side effect performed by
		 * that insn.
		 */
		body = PATTERN(insn);

		if (GET_CODE(body) == PARALLEL)
			body = XVECEXP(body, 0, 0);

		if (GET_CODE(body) != CALL)
			continue;

		/*
		 * Check the first operand of the call expression. It should
		 * be a mem RTX describing the needed subroutine with a
		 * symbol_ref RTX.
		 */
		body = XEXP(body, 0);
		if (GET_CODE(body) != MEM)
			continue;

		body = XEXP(body, 0);
		if (GET_CODE(body) != SYMBOL_REF)
			continue;

		if (SYMBOL_REF_DECL(body) != track_function_decl)
			continue;

		/* Delete the stackleak_track_stack() call */
		delete_insn_and_edges(insn);
#if BUILDING_GCC_VERSION < 8000
		if (GET_CODE(next) == NOTE &&
		    NOTE_KIND(next) == NOTE_INSN_CALL_ARG_LOCATION) {
			insn = next;
			next = NEXT_INSN(insn);
			delete_insn_and_edges(insn);
		}
#endif
	}
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"remove_stack_tracking_gcall\",\"func_id\":8}");
}

static bool remove_stack_tracking_gasm(void)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"function\":\"remove_stack_tracking_gasm\",\"func_id\":7,\"num_params\":0,\"start_line\":345,\"end_line\":399,\"metrics\":{\"num_params\":0,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
	bool removed = false;
	rtx_insn *insn, *next;

	/* 'no_caller_saved_registers' is currently supported only for x86 */
	gcc_assert(build_for_x86);

	/*
	 * Find stackleak_track_stack() asm calls. Loop through the chain of
	 * insns, which is an RTL representation of the code for a function.
	 *
	 * The example of a matching insn:
	 *  (insn 11 5 12 2 (parallel [ (asm_operands/v
	 *  ("call stackleak_track_stack") ("") 0
	 *  [ (reg/v:DI 7 sp [ current_stack_pointer ]) ]
	 *  [ (asm_input:DI ("r")) ] [])
	 *  (clobber (reg:CC 17 flags)) ]) -1 (nil))
	 */
	for (insn = get_insns(); insn; insn = next) {
		rtx body;

		next = NEXT_INSN(insn);

		/* Check the expression code of the insn */
		if (!NONJUMP_INSN_P(insn))
			continue;

		/*
		 * Check the expression code of the insn body, which is an RTL
		 * Expression (RTX) describing the side effect performed by
		 * that insn.
		 */
		body = PATTERN(insn);

		if (GET_CODE(body) != PARALLEL)
			continue;

		body = XVECEXP(body, 0, 0);

		if (GET_CODE(body) != ASM_OPERANDS)
			continue;

		if (strcmp(ASM_OPERANDS_TEMPLATE(body),
						"call stackleak_track_stack")) {
			continue;
		}

		delete_insn_and_edges(insn);
		gcc_assert(!removed);
		removed = true;
	}

	log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"remove_stack_tracking_gasm\",\"func_id\":7,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":399,\"ret_type\":\"int\",\"expr\":\"removed\"}");
	log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"remove_stack_tracking_gasm\",\"func_id\":7}");
	return removed;
}

/*
 * Work with the RTL representation of the code.
 * Remove the unneeded stackleak_track_stack() calls from the functions
 * which don't call alloca() and don't have a large enough stack frame size.
 */
static unsigned int stackleak_cleanup_execute(void)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"function\":\"stackleak_cleanup_execute\",\"func_id\":6,\"num_params\":0,\"start_line\":406,\"end_line\":441,\"metrics\":{\"num_params\":0,\"call_count\":2,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
	const char *fn = DECL_NAME_POINTER(current_function_decl);
	bool removed = false;

	/*
	 * Leave stack tracking in functions that call alloca().
	 * Additional case:
	 *   gcc before version 7 called allocate_dynamic_stack_space() from
	 *   expand_stack_vars() for runtime alignment of constant-sized stack
	 *   variables. That caused cfun->calls_alloca to be set for functions
	 *   that in fact don't use alloca().
	 *   For more info see gcc commit 7072df0aae0c59ae437e.
	 *   Let's leave such functions instrumented as well.
	 */
	if (cfun->calls_alloca) {
		if (verbose)
			fprintf(stderr, "stackleak: instrument %s(): calls_alloca\n", fn);
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"stackleak_cleanup_execute\",\"func_id\":6,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":424,\"ret_type\":\"unsigned int\",\"expr\":\"0\"}");
    printf("[LOG][RETURN_VALUE] %d\n", (0));
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"stackleak_cleanup_execute\",\"func_id\":6}");
		return 0;
	}

	/* Leave stack tracking in functions with large stack frame */
	if (large_stack_frame()) {
		if (verbose)
			fprintf(stderr, "stackleak: instrument %s()\n", fn);
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"stackleak_cleanup_execute\",\"func_id\":6,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":431,\"ret_type\":\"unsigned int\",\"expr\":\"0\"}");
    printf("[LOG][RETURN_VALUE] %d\n", (0));
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"stackleak_cleanup_execute\",\"func_id\":6}");
		return 0;
	}

	if (lookup_attribute_spec(get_identifier("no_caller_saved_registers")))
		removed = remove_stack_tracking_gasm();

	if (!removed)
		remove_stack_tracking_gcall();

    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"stackleak_cleanup_execute\",\"func_id\":6,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":440,\"ret_type\":\"unsigned int\",\"expr\":\"0\"}");
    printf("[LOG][RETURN_VALUE] %d\n", (0));
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"stackleak_cleanup_execute\",\"func_id\":6}");
	return 0;
}

/*
 * STRING_CST may or may not be NUL terminated:
 * https://gcc.gnu.org/onlinedocs/gccint/Constant-expressions.html
 */
static inline bool string_equal(tree node, const char *string, int length)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"function\":\"string_equal\",\"func_id\":5,\"num_params\":3,\"start_line\":447,\"end_line\":457,\"metrics\":{\"num_params\":3,\"call_count\":2,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"node\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false},{\"name\":\"string\",\"type\":\"const char *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"length\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"string_equal\",\"func_id\":5,\"name\":\"node\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"string_equal\",\"func_id\":5,\"name\":\"string\",\"type\":\"const char *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"string_equal\",\"func_id\":5,\"name\":\"length\",\"type\":\"int\",\"is_pointer\":false,\"is_const\":false}");
	if (TREE_STRING_LENGTH(node) < length) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"string_equal\",\"func_id\":5,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":454,\"ret_type\":\"int\",\"expr\":\"false\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"string_equal\",\"func_id\":5}");
	    return false;
	}
	if (TREE_STRING_LENGTH(node) > length + 1) {
	    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"string_equal\",\"func_id\":5,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":459,\"ret_type\":\"int\",\"expr\":\"false\"}");
	    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"string_equal\",\"func_id\":5}");
	    return false;
	}
	if (TREE_STRING_LENGTH(node) == length + 1 &&
	    TREE_STRING_POINTER(node)[length] != '\0')
		log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"string_equal\",\"func_id\":5,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":465,\"ret_type\":\"int\",\"expr\":\"false\"}");
		log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"string_equal\",\"func_id\":5}");
		return false;
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"string_equal\",\"func_id\":5,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":456,\"ret_type\":\"int\",\"expr\":\"!memcmp(TREE_STRING_POINTER(node), string, length)\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"string_equal\",\"func_id\":5}");
	return !memcmp(TREE_STRING_POINTER(node), string, length);
}
#define STRING_EQUAL(node, str)	string_equal(node, str, strlen(str))

static bool stackleak_gate(void)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"function\":\"stackleak_gate\",\"func_id\":4,\"num_params\":0,\"start_line\":460,\"end_line\":486,\"metrics\":{\"num_params\":0,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
	tree section;

	section = lookup_attribute("section",
				   DECL_ATTRIBUTES(current_function_decl));
	if (section && TREE_VALUE(section)) {
		section = TREE_VALUE(TREE_VALUE(section));

		if (STRING_EQUAL(section, ".init.text")) {
		    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"stackleak_gate\",\"func_id\":4,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":471,\"ret_type\":\"int\",\"expr\":\"false\"}");
		    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"stackleak_gate\",\"func_id\":4}");
		    return false;
		}
		if (STRING_EQUAL(section, ".devinit.text")) {
		    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"stackleak_gate\",\"func_id\":4,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":476,\"ret_type\":\"int\",\"expr\":\"false\"}");
		    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"stackleak_gate\",\"func_id\":4}");
		    return false;
		}
		if (STRING_EQUAL(section, ".cpuinit.text")) {
		    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"stackleak_gate\",\"func_id\":4,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":481,\"ret_type\":\"int\",\"expr\":\"false\"}");
		    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"stackleak_gate\",\"func_id\":4}");
		    return false;
		}
		if (STRING_EQUAL(section, ".meminit.text")) {
		    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"stackleak_gate\",\"func_id\":4,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":486,\"ret_type\":\"int\",\"expr\":\"false\"}");
		    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"stackleak_gate\",\"func_id\":4}");
		    return false;
		}
		if (STRING_EQUAL(section, ".noinstr.text")) {
		    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"stackleak_gate\",\"func_id\":4,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":491,\"ret_type\":\"int\",\"expr\":\"false\"}");
		    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"stackleak_gate\",\"func_id\":4}");
		    return false;
		}
		if (STRING_EQUAL(section, ".entry.text")) {
		    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"stackleak_gate\",\"func_id\":4,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":496,\"ret_type\":\"int\",\"expr\":\"false\"}");
		    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"stackleak_gate\",\"func_id\":4}");
		    return false;
		}
		if (STRING_EQUAL(section, ".head.text")) {
		    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"stackleak_gate\",\"func_id\":4,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":501,\"ret_type\":\"int\",\"expr\":\"false\"}");
		    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"stackleak_gate\",\"func_id\":4}");
		    return false;
		}
	}

    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"stackleak_gate\",\"func_id\":4,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":485,\"ret_type\":\"int\",\"expr\":\"track_frame_size >= 0\"}");
    printf("[LOG][RETURN_VALUE] %d\n", (track_frame_size >= 0));
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"stackleak_gate\",\"func_id\":4}");
	return track_frame_size >= 0;
}

/* Build the function declaration for stackleak_track_stack() */
static void stackleak_start_unit(void *gcc_data __unused,
				 void *user_data __unused)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"function\":\"stackleak_start_unit\",\"func_id\":3,\"num_params\":2,\"start_line\":489,\"end_line\":503,\"metrics\":{\"num_params\":2,\"call_count\":0,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"gcc_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"user_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"stackleak_start_unit\",\"func_id\":3,\"name\":\"gcc_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"stackleak_start_unit\",\"func_id\":3,\"name\":\"user_data\",\"type\":\"void *\",\"is_pointer\":true,\"is_const\":false}");
	tree fntype;

	/* void stackleak_track_stack(void) */
	fntype = build_function_type_list(void_type_node, NULL_TREE);
	track_function_decl = build_fn_decl(track_function, fntype);
	DECL_ASSEMBLER_NAME(track_function_decl); /* for LTO */
	TREE_PUBLIC(track_function_decl) = 1;
	TREE_USED(track_function_decl) = 1;
	DECL_EXTERNAL(track_function_decl) = 1;
	DECL_ARTIFICIAL(track_function_decl) = 1;
	DECL_PRESERVE_P(track_function_decl) = 1;
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"stackleak_start_unit\",\"func_id\":3}");
}

/*
 * Pass gate function is a predicate function that gets executed before the
 * corresponding pass. If the return value is 'true' the pass gets executed,
 * otherwise, it is skipped.
 */
static bool stackleak_instrument_gate(void)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"function\":\"stackleak_instrument_gate\",\"func_id\":2,\"num_params\":0,\"start_line\":510,\"end_line\":513,\"metrics\":{\"num_params\":0,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"stackleak_instrument_gate\",\"func_id\":2,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":512,\"ret_type\":\"int\",\"expr\":\"stackleak_gate()\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"stackleak_instrument_gate\",\"func_id\":2}");
	return stackleak_gate();
}

#define PASS_NAME stackleak_instrument
#define PROPERTIES_REQUIRED PROP_gimple_leh | PROP_cfg
#define TODO_FLAGS_START TODO_verify_ssa | TODO_verify_flow | TODO_verify_stmts
#define TODO_FLAGS_FINISH TODO_verify_ssa | TODO_verify_stmts | TODO_dump_func \
			| TODO_update_ssa | TODO_rebuild_cgraph_edges
#include "gcc-generate-gimple-pass.h"

static bool stackleak_cleanup_gate(void)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"function\":\"stackleak_cleanup_gate\",\"func_id\":1,\"num_params\":0,\"start_line\":522,\"end_line\":525,\"metrics\":{\"num_params\":0,\"call_count\":1,\"has_recursion\":false,\"has_loop\":false,\"has_if\":false,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[]}");
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"stackleak_cleanup_gate\",\"func_id\":1,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":524,\"ret_type\":\"int\",\"expr\":\"stackleak_gate()\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"stackleak_cleanup_gate\",\"func_id\":1}");
	return stackleak_gate();
}

#define PASS_NAME stackleak_cleanup
#define TODO_FLAGS_FINISH TODO_dump_func
#include "gcc-generate-rtl-pass.h"

/*
 * Every gcc plugin exports a plugin_init() function that is called right
 * after the plugin is loaded. This function is responsible for registering
 * the plugin callbacks and doing other required initialization.
 */
__visible int plugin_init(struct plugin_name_args *plugin_info,
			  struct plugin_gcc_version *version)
{
    log_with_timestamp("{\"event\":\"FUNC_ENTER\",\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"function\":\"plugin_init\",\"func_id\":0,\"num_params\":2,\"start_line\":536,\"end_line\":646,\"metrics\":{\"num_params\":2,\"call_count\":2,\"has_recursion\":false,\"has_loop\":true,\"has_if\":true,\"has_switch\":false,\"has_goto\":false,\"stmt_count\":0},\"flags\":{\"is_method\":false,\"is_static_method\":false,\"is_const_method\":false,\"is_virtual_method\":false,\"is_variadic\":false},\"params\":[{\"name\":\"plugin_info\",\"type\":\"struct plugin_name_args *\",\"is_pointer\":true,\"is_const\":false},{\"name\":\"version\",\"type\":\"struct plugin_gcc_version *\",\"is_pointer\":true,\"is_const\":false}]}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"plugin_init\",\"func_id\":0,\"name\":\"plugin_info\",\"type\":\"struct plugin_name_args *\",\"is_pointer\":true,\"is_const\":false}");
    log_with_timestamp("{\"event\":\"PARAM\",\"function\":\"plugin_init\",\"func_id\":0,\"name\":\"version\",\"type\":\"struct plugin_gcc_version *\",\"is_pointer\":true,\"is_const\":false}");
	const char * const plugin_name = plugin_info->base_name;
	const int argc = plugin_info->argc;
	const struct plugin_argument * const argv = plugin_info->argv;
	int i = 0;

	/* Extra GGC root tables describing our GTY-ed data */
	static const struct ggc_root_tab gt_ggc_r_gt_stackleak[] = {
		{
			.base = &track_function_decl,
			.nelt = 1,
			.stride = sizeof(track_function_decl),
			.cb = &gt_ggc_mx_tree_node,
			.pchw = &gt_pch_nx_tree_node
		},
		LAST_GGC_ROOT_TAB
	};

	/*
	 * The stackleak_instrument pass should be executed before the
	 * "optimized" pass, which is the control flow graph cleanup that is
	 * performed just before expanding gcc trees to the RTL. In former
	 * versions of the plugin this new pass was inserted before the
	 * "tree_profile" pass, which is currently called "profile".
	 */
	PASS_INFO(stackleak_instrument, "optimized", 1,
						PASS_POS_INSERT_BEFORE);

	/*
	 * The stackleak_cleanup pass should be executed before the "*free_cfg"
	 * pass. It's the moment when the stack frame size is already final,
	 * function prologues and epilogues are generated, and the
	 * machine-dependent code transformations are not done.
	 */
	PASS_INFO(stackleak_cleanup, "*free_cfg", 1, PASS_POS_INSERT_BEFORE);

	if (!plugin_default_version_check(version, &gcc_version)) {
		error(G_("incompatible gcc/plugin versions"));
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"plugin_init\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":576,\"ret_type\":\"int\",\"expr\":\"1\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"plugin_init\",\"func_id\":0}");
		return 1;
	}

	/* Parse the plugin arguments */
	for (i = 0; i < argc; i++) {
		if (!strcmp(argv[i].key, "track-min-size")) {
			if (!argv[i].value) {
				error(G_("no value supplied for option '-fplugin-arg-%s-%s'"),
					plugin_name, argv[i].key);
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"plugin_init\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":585,\"ret_type\":\"int\",\"expr\":\"1\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"plugin_init\",\"func_id\":0}");
				return 1;
			}

			track_frame_size = atoi(argv[i].value);
			if (track_frame_size < 0) {
				error(G_("invalid option argument '-fplugin-arg-%s-%s=%s'"),
					plugin_name, argv[i].key, argv[i].value);
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"plugin_init\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":592,\"ret_type\":\"int\",\"expr\":\"1\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"plugin_init\",\"func_id\":0}");
				return 1;
			}
		} else if (!strcmp(argv[i].key, "arch")) {
			if (!argv[i].value) {
				error(G_("no value supplied for option '-fplugin-arg-%s-%s'"),
					plugin_name, argv[i].key);
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"plugin_init\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":598,\"ret_type\":\"int\",\"expr\":\"1\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"plugin_init\",\"func_id\":0}");
				return 1;
			}

			if (!strcmp(argv[i].value, "x86"))
				build_for_x86 = true;
		} else if (!strcmp(argv[i].key, "disable")) {
			disable = true;
		} else if (!strcmp(argv[i].key, "verbose")) {
			verbose = true;
		} else {
			error(G_("unknown option '-fplugin-arg-%s-%s'"),
					plugin_name, argv[i].key);
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"plugin_init\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":610,\"ret_type\":\"int\",\"expr\":\"1\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"plugin_init\",\"func_id\":0}");
			return 1;
		}
	}

	if (disable) {
		if (verbose)
			fprintf(stderr, "stackleak: disabled for this translation unit\n");
    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"plugin_init\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":617,\"ret_type\":\"int\",\"expr\":\"0\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"plugin_init\",\"func_id\":0}");
		return 0;
	}

	/* Give the information about the plugin */
	register_callback(plugin_name, PLUGIN_INFO, NULL,
						&stackleak_plugin_info);

	/* Register to be called before processing a translation unit */
	register_callback(plugin_name, PLUGIN_START_UNIT,
					&stackleak_start_unit, NULL);

	/* Register an extra GCC garbage collector (GGC) root table */
	register_callback(plugin_name, PLUGIN_REGISTER_GGC_ROOTS, NULL,
					(void *)&gt_ggc_r_gt_stackleak);

	/*
	 * Hook into the Pass Manager to register new gcc passes.
	 *
	 * The stack frame size info is available only at the last RTL pass,
	 * when it's too late to insert complex code like a function call.
	 * So we register two gcc passes to instrument every function at first
	 * and remove the unneeded instrumentation later.
	 */
	register_callback(plugin_name, PLUGIN_PASS_MANAGER_SETUP, NULL,
					&stackleak_instrument_pass_info);
	register_callback(plugin_name, PLUGIN_PASS_MANAGER_SETUP, NULL,
					&stackleak_cleanup_pass_info);

    log_with_timestamp("{\"event\":\"RETURN\",\"function\":\"plugin_init\",\"func_id\":0,\"file\":\"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/stack_leak_instrumented/stackleak_plugin.c\",\"line\":645,\"ret_type\":\"int\",\"expr\":\"0\"}");
    log_with_timestamp("{\"event\":\"FUNC_EXIT\",\"function\":\"plugin_init\",\"func_id\":0}");
	return 0;
}
