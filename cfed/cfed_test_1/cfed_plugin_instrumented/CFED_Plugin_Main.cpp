#include <ctime>
#include <iostream>
#include <fstream>
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    std::ofstream log_file("/home/nimantha/FIRES/cfed_plugin_instrumented/trace_CFED_Plugin_Main.cpp.log", std::ios::app);
    log_file << "{\"ts\":\"" << time_buf
             << "\",\"file\":\"CFED_Plugin_Main.cpp\",\"msg\":" << msg
             << "}" << std::endl;
}
/*
 * This GCC Plugin has been developed during a research grant from the Baekeland program of the Flemish Agency for Innovation and Entrepreneurship (VLAIO) in cooperation with Televic Healthcare NV, under grant agreement IWT 150696.
 * Copyright (c) 2019 Jens Vankeirsbilck & KU Leuven LRD & Televic Healthcare NV.
 * Distributed under the MIT "Expat" License. (See accompanying file LICENSE.txt)
 */

/*
 * This is the main-file for the GCC plugin.
 * It defines necessary structs, our defined noProtection function attribute,
 * and adds the plugin to the PASS_MANAGER.
 */

#include <gcc-plugin.h>
#include <context.h>
#include <basic-block.h>
#include <rtl.h>
#include <tree-pass.h>
#include <tree.h>
#include <plugin.h>

#include <stdio.h>

#include "CFED_Plugin.h"


// Mandatory variable, indicates that a GPL compatible license is applied to this GCC plugin.
int plugin_is_GPL_compatible = 1;

static struct plugin_info myPlugin_info =
{
		.version = "3",
		.help = "GCC plugin for arm-none-eabi-7.3",
};

static struct plugin_gcc_version myPlugin_ver =
{
		.basever = "7.3",
};

// specify own noProtection attribute
static struct attribute_spec noProtection_attr =
{
		"noProtection", 0, 0, false, false, false, NULL, false
};

// Register all self-specified attributes
static void register_attributes(void *event_data, void *data){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/FIRES/cfed_plugin_instrumented/CFED_Plugin_Main.cpp","function":"register_attributes","func_id":1,"num_params":2,"start_line":47,"end_line":49,"metrics":{"num_params":2,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"event_data","type":"void *","is_pointer":true,"is_const":false},{"name":"data","type":"void *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"register_attributes","func_id":1,"name":"event_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"register_attributes","func_id":1,"name":"data","type":"void *","is_pointer":true,"is_const":false})JSON");
	register_attribute(&noProtection_attr);
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"register_attributes","func_id":1})JSON");
}


// Start point of the plugin
int plugin_init(struct plugin_name_args *info, struct plugin_gcc_version *ver){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/FIRES/cfed_plugin_instrumented/CFED_Plugin_Main.cpp","function":"plugin_init","func_id":0,"num_params":2,"start_line":53,"end_line":68,"metrics":{"num_params":2,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false},{"name":"ver","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"ver","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false})JSON");
	if(strncmp(ver->basever,myPlugin_ver.basever, strlen("7.3"))){
		return -1;
	}
    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/FIRES/cfed_plugin_instrumented/CFED_Plugin_Main.cpp","line":55,"ret_type":"int","expr":""})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");

	struct register_pass_info pass;
	pass.pass = new CFED_PLUGIN(g, info->argv, info->argc);
	pass.reference_pass_name = "*free_cfg";
	pass.ref_pass_instance_number = 1;
	pass.pos_op = PASS_POS_INSERT_AFTER;

	register_callback("myPlugin", PLUGIN_PASS_MANAGER_SETUP, NULL, &pass);
	register_callback("myPlugin", PLUGIN_ATTRIBUTES, register_attributes, NULL);

    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/FIRES/cfed_plugin_instrumented/CFED_Plugin_Main.cpp","line":67,"ret_type":"int","expr":"0"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
	return 0;
}
