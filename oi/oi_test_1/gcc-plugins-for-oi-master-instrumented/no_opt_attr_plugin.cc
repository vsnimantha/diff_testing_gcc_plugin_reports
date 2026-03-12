#include <ctime>
#include <iostream>
#include <fstream>
/*
 * Copyright 2021 by Xidian University Programming Contest Training Base
 * Licensed under the GPL v2
 *
 * Author: Xi Ruoyao <xry111@mengyan1223.wang>
 */

#include "gcc-common.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    std::ofstream log_file("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/oi-plugin/gcc-plugins-for-oi-master-instrumented/trace_no_opt_attr_plugin.cc.log", std::ios::app);
    log_file << "{\"ts\":\"" << time_buf
             << "\",\"file\":\"no_opt_attr_plugin.cc\",\"msg\":" << msg
             << "}" << std::endl;
}

__visible int plugin_is_GPL_compatible;

static struct plugin_info no_opt_attr_plugin_info = {
	.version = "20210915",
	.help = "",
};

static unsigned int no_opt_attr_execute(void)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/oi-plugin/gcc-plugins-for-oi-master-instrumented/no_opt_attr_plugin.cc","function":"no_opt_attr_execute","func_id":1,"num_params":0,"start_line":17,"end_line":24,"metrics":{"num_params":0,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[]})JSON");
	if (lookup_attribute("optimize", DECL_ATTRIBUTES(cfun->decl))) {
		error(G_("optimize attribute or pragma is not allowed"));
    log_with_timestamp(R"JSON({"event":"RETURN","function":"no_opt_attr_execute","func_id":1,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/oi-plugin/gcc-plugins-for-oi-master-instrumented/no_opt_attr_plugin.cc","line":21,"ret_type":"unsigned int","expr":"1"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"no_opt_attr_execute","func_id":1})JSON");
		return 1;
	}
    log_with_timestamp(R"JSON({"event":"RETURN","function":"no_opt_attr_execute","func_id":1,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/oi-plugin/gcc-plugins-for-oi-master-instrumented/no_opt_attr_plugin.cc","line":23,"ret_type":"unsigned int","expr":"0"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"no_opt_attr_execute","func_id":1})JSON");
	return 0;
}

#define PASS_NAME no_opt_attr
#define NO_GATE
#include "gcc-generate-gimple-pass.h"

__visible int plugin_init(struct plugin_name_args *plugin_info,
                          struct plugin_gcc_version *version)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/oi-plugin/gcc-plugins-for-oi-master-instrumented/no_opt_attr_plugin.cc","function":"plugin_init","func_id":0,"num_params":2,"start_line":30,"end_line":46,"metrics":{"num_params":2,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"plugin_info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false},{"name":"version","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"plugin_info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"version","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false})JSON");
	const char * const plugin_name = plugin_info->base_name;

	PASS_INFO(no_opt_attr, "optimized", 1, PASS_POS_INSERT_BEFORE);

	if (!plugin_default_version_check(version, &gcc_version)) {
		error(G_("incompatible gcc/plugin versions"));
    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/oi-plugin/gcc-plugins-for-oi-master-instrumented/no_opt_attr_plugin.cc","line":39,"ret_type":"int","expr":"1"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
		return 1;
	}

	register_callback(plugin_name, PLUGIN_PASS_MANAGER_SETUP, NULL,
	                  &no_opt_attr_pass_info);

    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/oi-plugin/gcc-plugins-for-oi-master-instrumented/no_opt_attr_plugin.cc","line":45,"ret_type":"int","expr":"0"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
	return 0;
}
