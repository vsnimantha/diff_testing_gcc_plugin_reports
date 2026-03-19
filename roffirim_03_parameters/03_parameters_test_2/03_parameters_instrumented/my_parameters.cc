#include <ctime>
#include <fstream>
#include <iostream>

// This is the first gcc header to be included
#include "gcc-plugin.h"
#include "plugin-version.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    std::ofstream log_file("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/03_parameters_instrumented/trace_my_parameters.cc.log", std::ios::app);
    log_file << "{\"ts\":\"" << time_buf
             << "\",\"file\":\"my_parameters.cc\",\"msg\":" << msg
             << "}" << std::endl;
}

// We must assert that this plugin is GPL compatible
int plugin_is_GPL_compatible;

static struct plugin_info my_gcc_plugin_info = { "1.0", "This is a very simple plugin" };

int plugin_init (struct plugin_name_args *plugin_info,
		struct plugin_gcc_version *version)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/03_parameters_instrumented/my_parameters.cc","function":"plugin_init","func_id":0,"num_params":2,"start_line":12,"end_line":35,"metrics":{"num_params":2,"call_count":4,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"plugin_info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false},{"name":"version","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"plugin_info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"version","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false})JSON");
	// We check the current gcc loading this plugin against the gcc we used to
	// created this plugin
	if (!plugin_default_version_check (version, &gcc_version))
    {
        std::cerr << "This GCC plugin is for version " << GCCPLUGIN_VERSION_MAJOR << "." << GCCPLUGIN_VERSION_MINOR << "\n";
    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/03_parameters_instrumented/my_parameters.cc","line":20,"ret_type":"int","expr":"1"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
		return 1;
    }

    register_callback(plugin_info->base_name,
            /* event */ PLUGIN_INFO,
            /* callback */ NULL, /* user_data */ &my_gcc_plugin_info);

    std::cerr << "Number of arguments of this plugin:" << plugin_info->argc << "\n";

    for (int i = 0; i < plugin_info->argc; i++)
    {
        std::cerr << "Argument " << i << ": Key: " << plugin_info->argv[i].key << ". Value: " << plugin_info->argv[i].value<< "\n";
    }

    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/03_parameters_instrumented/my_parameters.cc","line":34,"ret_type":"int","expr":"0"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
    return 0;
}
