#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>

#include <gcc-plugin.h>
#include <plugin-version.h>

#include "log.h"
#include "printfun.h"
#include "gcc_hell.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    std::ofstream log_file("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/trace_cprintf.cpp.log", std::ios::app);
    log_file << "{\"ts\":\"" << time_buf
             << "\",\"file\":\"cprintf.cpp\",\"msg\":" << msg
             << "}" << std::endl;
}

int plugin_is_GPL_compatible = 1;

typedef void (*arg_parse)(const char*);

static std::map<std::string, arg_parse> cprintf_args_create(void)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/cprintf.cpp","function":"cprintf_args_create","func_id":2,"num_params":0,"start_line":16,"end_line":24,"metrics":{"num_params":0,"call_count":0,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[]})JSON");
	std::map<std::string, arg_parse> ret;

	ret["log_level"] = &log::set_log_level;
	ret["printf"] = &printfun::add_printfun;

    log_with_timestamp(R"JSON({"event":"RETURN","function":"cprintf_args_create","func_id":2,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/cprintf.cpp","line":23,"ret_type":"int","expr":"ret"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"cprintf_args_create","func_id":2})JSON");
	return ret;
}
const static std::map<std::string, arg_parse>
cprintf_args = cprintf_args_create();

static int parse_parameters(struct plugin_name_args *info)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/cprintf.cpp","function":"parse_parameters","func_id":1,"num_params":1,"start_line":28,"end_line":52,"metrics":{"num_params":1,"call_count":4,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"parse_parameters","func_id":1,"name":"info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false})JSON");
	for (int i = 0; i < info->argc; i++) {
		try {
			arg_parse f = cprintf_args.at(info->argv[i].key);
			f(info->argv[i].value);
		} catch (const std::out_of_range &oor) {
			log::err << "Unknown parameter `" << info->argv[i].key
				<< "', terminating\n";
    log_with_timestamp(R"JSON({"event":"RETURN","function":"parse_parameters","func_id":1,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/cprintf.cpp","line":37,"ret_type":"int","expr":"1"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"parse_parameters","func_id":1})JSON");
			return 1;
		} catch (const std::logic_error &le) {
			log::err << "Parse parameter `" << info->argv[i].key
				<< "' failed:\n" << le.what() << std::endl;
    log_with_timestamp(R"JSON({"event":"RETURN","function":"parse_parameters","func_id":1,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/cprintf.cpp","line":41,"ret_type":"int","expr":"1"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"parse_parameters","func_id":1})JSON");
			return 1;
		}
	}

	if (printfun::printfuns.size() == 0) {
		/* no printf arg */
		log::err << "Specify `printf' argument with function specification\n";
    log_with_timestamp(R"JSON({"event":"RETURN","function":"parse_parameters","func_id":1,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/cprintf.cpp","line":48,"ret_type":"int","expr":"1"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"parse_parameters","func_id":1})JSON");
		return 1;
	}

    log_with_timestamp(R"JSON({"event":"RETURN","function":"parse_parameters","func_id":1,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/cprintf.cpp","line":51,"ret_type":"int","expr":"0"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"parse_parameters","func_id":1})JSON");
	return 0;
}

int plugin_init(struct plugin_name_args *info,
		struct plugin_gcc_version *version)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/cprintf.cpp","function":"plugin_init","func_id":0,"num_params":2,"start_line":54,"end_line":91,"metrics":{"num_params":2,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false},{"name":"version","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"version","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false})JSON");
	struct register_pass_info pass_info;
	int ret;

	/*
	 * Check the current gcc loading this plugin against the gcc,
	 * used to compile this plugin.
	 */
	if (!plugin_default_version_check(version, &gcc_version)) {
		log::err << "This GCC plugin is for version " <<
			GCCPLUGIN_VERSION_MAJOR << "." <<
			GCCPLUGIN_VERSION_MINOR << std::endl;
    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/cprintf.cpp","line":68,"ret_type":"int","expr":"1"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
		return 1;
	}

	ret = parse_parameters(info);
	if (ret) {
		log::err << "Failed to parse plugin parameters: "
			<< ret << std::endl;
    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/cprintf.cpp","line":75,"ret_type":"int","expr":"ret"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
		return ret;
	}

	/*
	 * Register cprintf pass before building CFG, otherwise
	 * fun->gimple_body is not accessible anymore.
	 */
	pass_info.pass = new gcc_hell::cprintf_pass(g);
	pass_info.reference_pass_name = "cfg";
	pass_info.ref_pass_instance_number = 1;
	pass_info.pos_op = PASS_POS_INSERT_BEFORE;

	register_callback(info->base_name, PLUGIN_PASS_MANAGER_SETUP,
			NULL, &pass_info);

    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/cprintf.cpp","line":90,"ret_type":"int","expr":"0"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
	return 0;
}
