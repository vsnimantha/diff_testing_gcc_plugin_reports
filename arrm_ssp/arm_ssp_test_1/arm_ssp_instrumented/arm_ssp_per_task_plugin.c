#include <time.h>
#include <stdio.h>
// SPDX-License-Identifier: GPL-2.0

#include "gcc-common.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    FILE *fp = fopen("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/arm_ssp_instrumented/trace_arm_ssp_per_task_plugin.c.log", "a");
    if (fp) {
        fprintf(fp, "{\"ts\":\"%s\",\"file\":\"arm_ssp_per_task_plugin.c\",\"msg\":%s}\n",
                time_buf, msg);
        fclose(fp);
    }
}

__visible int plugin_is_GPL_compatible;

static unsigned int canary_offset;

static unsigned int arm_pertask_ssp_rtl_execute(void)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/arm_ssp_instrumented/arm_ssp_per_task_plugin.c","function":"arm_pertask_ssp_rtl_execute","func_id":1,"num_params":0,"start_line":9,"end_line":68,"metrics":{"num_params":0,"call_count":0,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[]})JSON");
    rtx_insn *insn;

    for (insn = get_insns(); insn; insn = NEXT_INSN(insn)) {
        const char *sym;
        rtx body;

        /*
         * Find a SET insn involving a SYMBOL_REF to __stack_chk_guard
         */
        if (!INSN_P(insn))
            continue;

        body = PATTERN(insn);
        if (GET_CODE(body) != SET ||
            GET_CODE(SET_SRC(body)) != SYMBOL_REF)
            continue;

        sym = XSTR(SET_SRC(body), 0);
        if (strcmp(sym, "__stack_chk_guard"))
            continue;

#if defined(THREAD_POINTER_REGNUM)
        /*
         * Real implementation: only compiled when the target backend
         * defines THREAD_POINTER_REGNUM (e.g. ARM).
         *
         * Replace the source of the SET insn with an expression that
         * produces the address of the current task's stack canary value.
         */
        {
            rtx current = gen_reg_rtx(Pmode);

            /* Load the thread pointer (TP) into 'current'. */
            rtx tp = gen_rtx_REG(Pmode, THREAD_POINTER_REGNUM);
            rtx set_tp = gen_rtx_SET(current, tp);

            emit_insn_before(set_tp, insn);

            /* Now compute current + canary_offset as the new source. */
            SET_SRC(body) = gen_rtx_PLUS(Pmode, current,
                             GEN_INT(canary_offset));
        }
#else
        /*
         * Non-ARM or targets without THREAD_POINTER_REGNUM:
         *
         * We leave the instruction unchanged. The plugin becomes a
         * no-op on this compiler/target combination, but it still
         * builds and loads without errors.
         */
        (void)sym;    /* avoid unused variable warnings */
        (void)body;
        (void)canary_offset;
#endif
    }

    log_with_timestamp(R"JSON({"event":"RETURN","function":"arm_pertask_ssp_rtl_execute","func_id":1,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/arm_ssp_instrumented/arm_ssp_per_task_plugin.c","line":67,"ret_type":"unsigned int","expr":"0"})JSON");
    printf("[LOG][RETURN_VALUE] %d\n", (0));
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"arm_pertask_ssp_rtl_execute","func_id":1})JSON");
    return 0;
}

#define PASS_NAME arm_pertask_ssp_rtl

#define NO_GATE
#include "gcc-generate-rtl-pass.h"

#if BUILDING_GCC_VERSION >= 9000
static bool no(void)
{
    return false;
}

static void arm_pertask_ssp_start_unit(void *gcc_data, void *user_data)
{
    targetm.have_stack_protect_combined_set = no;
    targetm.have_stack_protect_combined_test = no;
}
#endif

__visible int plugin_init(struct plugin_name_args *plugin_info,
              struct plugin_gcc_version *version)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/arm_ssp_instrumented/arm_ssp_per_task_plugin.c","function":"plugin_init","func_id":0,"num_params":2,"start_line":88,"end_line":137,"metrics":{"num_params":2,"call_count":2,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"plugin_info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false},{"name":"version","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"plugin_info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"version","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false})JSON");
    const char * const plugin_name = plugin_info->base_name;
    const int argc = plugin_info->argc;
    const struct plugin_argument *argv = plugin_info->argv;
    int i;

    if (!plugin_default_version_check(version, &gcc_version)) {
        error(G_("incompatible gcc/plugin versions"));
        return 1;
    }

    for (i = 0; i < argc; ++i) {
        if (!strcmp(argv[i].key, "disable"))
            return 0;

        /* all remaining options require a value */
    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/arm_ssp_instrumented/arm_ssp_per_task_plugin.c","line":98,"ret_type":"int","expr":""})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
        if (!argv[i].value) {
            /*
             * Keep original kernel-style messages.
    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/arm_ssp_instrumented/arm_ssp_per_task_plugin.c","line":103,"ret_type":"int","expr":""})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
             * GCC 10 may warn about the format, but it's harmless.
             */
            error(G_("no value supplied for option '-fplugin-arg-%s-%s'"),
                  plugin_name, argv[i].key);
            return 1;
        }

        if (!strcmp(argv[i].key, "offset")) {
    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/arm_ssp_instrumented/arm_ssp_per_task_plugin.c","line":113,"ret_type":"int","expr":""})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
            canary_offset = atoi(argv[i].value);
            continue;
        }

        error(G_("unknown option '-fplugin-arg-%s-%s'"),
              plugin_name, argv[i].key);
        return 1;
    }
    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/arm_ssp_instrumented/arm_ssp_per_task_plugin.c","line":123,"ret_type":"int","expr":""})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");

    PASS_INFO(arm_pertask_ssp_rtl, "expand", 1, PASS_POS_INSERT_AFTER);

    register_callback(plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP,
              NULL, &arm_pertask_ssp_rtl_pass_info);

#if BUILDING_GCC_VERSION >= 9000
    register_callback(plugin_info->base_name, PLUGIN_START_UNIT,
              arm_pertask_ssp_start_unit, NULL);
#endif

    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Linux_Kernal/arm_ssp_instrumented/arm_ssp_per_task_plugin.c","line":136,"ret_type":"int","expr":"0"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
    return 0;
}
