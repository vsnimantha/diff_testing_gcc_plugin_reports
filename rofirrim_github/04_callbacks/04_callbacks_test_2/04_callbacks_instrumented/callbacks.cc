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
    std::ofstream log_file("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/04_callbacks_instrumented/trace_callbacks.cc.log", std::ios::app);
    log_file << "{\"ts\":\"" << time_buf
             << "\",\"file\":\"callbacks.cc\",\"msg\":" << msg
             << "}" << std::endl;
}

// We must assert that this plugin is GPL compatible
int plugin_is_GPL_compatible;

static struct plugin_info my_gcc_plugin_info = { "1.0", "This is a very simple plugin" };

static void callback_finish_type(void *gcc_data, void *user_data)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/04_callbacks_instrumented/callbacks.cc","function":"callback_finish_type","func_id":11,"num_params":2,"start_line":12,"end_line":15,"metrics":{"num_params":2,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false},{"name":"user_data","type":"void *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_finish_type","func_id":11,"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_finish_type","func_id":11,"name":"user_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    std::cerr << " *** A type has been finished\n";
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"callback_finish_type","func_id":11})JSON");
}

static void callback_finish_declaration(void *gcc_data, void *user_data)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/04_callbacks_instrumented/callbacks.cc","function":"callback_finish_declaration","func_id":10,"num_params":2,"start_line":17,"end_line":20,"metrics":{"num_params":2,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false},{"name":"user_data","type":"void *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_finish_declaration","func_id":10,"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_finish_declaration","func_id":10,"name":"user_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    std::cerr << " *** A declaration has been finished\n";
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"callback_finish_declaration","func_id":10})JSON");
}

static void callback_finish_unit(void *gcc_data, void *user_data)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/04_callbacks_instrumented/callbacks.cc","function":"callback_finish_unit","func_id":9,"num_params":2,"start_line":22,"end_line":25,"metrics":{"num_params":2,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false},{"name":"user_data","type":"void *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_finish_unit","func_id":9,"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_finish_unit","func_id":9,"name":"user_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    std::cerr << " *** A translation unit has been finished\n";
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"callback_finish_unit","func_id":9})JSON");
}

static void callback_pre_genericize(void *gcc_data, void *user_data)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/04_callbacks_instrumented/callbacks.cc","function":"callback_pre_genericize","func_id":8,"num_params":2,"start_line":27,"end_line":30,"metrics":{"num_params":2,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false},{"name":"user_data","type":"void *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_pre_genericize","func_id":8,"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_pre_genericize","func_id":8,"name":"user_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    std::cerr << " *** We are about to emit GENERIC after parsing C/C++\n";
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"callback_pre_genericize","func_id":8})JSON");
}

static void callback_finish(void *gcc_data, void *user_data)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/04_callbacks_instrumented/callbacks.cc","function":"callback_finish","func_id":7,"num_params":2,"start_line":32,"end_line":35,"metrics":{"num_params":2,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false},{"name":"user_data","type":"void *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_finish","func_id":7,"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_finish","func_id":7,"name":"user_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    std::cerr << " *** GCC is exiting\n";
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"callback_finish","func_id":7})JSON");
}

static void callback_register_attribute(void *gcc_data, void *user_data)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/04_callbacks_instrumented/callbacks.cc","function":"callback_register_attribute","func_id":6,"num_params":2,"start_line":37,"end_line":40,"metrics":{"num_params":2,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false},{"name":"user_data","type":"void *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_register_attribute","func_id":6,"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_register_attribute","func_id":6,"name":"user_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    std::cerr << " *** Registering attributes\n";
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"callback_register_attribute","func_id":6})JSON");
}

static void callback_start_unit(void *gcc_data, void *user_data)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/04_callbacks_instrumented/callbacks.cc","function":"callback_start_unit","func_id":5,"num_params":2,"start_line":42,"end_line":45,"metrics":{"num_params":2,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false},{"name":"user_data","type":"void *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_start_unit","func_id":5,"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_start_unit","func_id":5,"name":"user_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    std::cerr << " *** We are about to process a translation unit\n";
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"callback_start_unit","func_id":5})JSON");
}

static void callback_registering_pragmas(void *gcc_data, void *user_data)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/04_callbacks_instrumented/callbacks.cc","function":"callback_registering_pragmas","func_id":4,"num_params":2,"start_line":47,"end_line":50,"metrics":{"num_params":2,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false},{"name":"user_data","type":"void *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_registering_pragmas","func_id":4,"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_registering_pragmas","func_id":4,"name":"user_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    std::cerr << " *** Registering pragmas\n";
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"callback_registering_pragmas","func_id":4})JSON");
}

static void callback_all_passes_start(void *gcc_data, void *user_data)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/04_callbacks_instrumented/callbacks.cc","function":"callback_all_passes_start","func_id":3,"num_params":2,"start_line":52,"end_line":55,"metrics":{"num_params":2,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false},{"name":"user_data","type":"void *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_all_passes_start","func_id":3,"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_all_passes_start","func_id":3,"name":"user_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    std::cerr << " *** About to start all passes\n";
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"callback_all_passes_start","func_id":3})JSON");
}

static void callback_all_passes_end(void *gcc_data, void *user_data)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/04_callbacks_instrumented/callbacks.cc","function":"callback_all_passes_end","func_id":2,"num_params":2,"start_line":57,"end_line":60,"metrics":{"num_params":2,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false},{"name":"user_data","type":"void *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_all_passes_end","func_id":2,"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_all_passes_end","func_id":2,"name":"user_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    std::cerr << " *** All passes ended\n";
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"callback_all_passes_end","func_id":2})JSON");
}

static void callback_pass_execution(void *gcc_data, void *user_data)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/04_callbacks_instrumented/callbacks.cc","function":"callback_pass_execution","func_id":1,"num_params":2,"start_line":62,"end_line":65,"metrics":{"num_params":2,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false},{"name":"user_data","type":"void *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_pass_execution","func_id":1,"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"callback_pass_execution","func_id":1,"name":"user_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    std::cerr << " *** About to execute a pass\n";
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"callback_pass_execution","func_id":1})JSON");
}

int plugin_init (struct plugin_name_args *plugin_info,
		struct plugin_gcc_version *version)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/04_callbacks_instrumented/callbacks.cc","function":"plugin_init","func_id":0,"num_params":2,"start_line":67,"end_line":127,"metrics":{"num_params":2,"call_count":0,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"plugin_info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false},{"name":"version","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"plugin_info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"version","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false})JSON");
	// We check the current gcc loading this plugin against the gcc we used to
	// created this plugin
	if (!plugin_default_version_check (version, &gcc_version))
    {
        std::cerr << "This GCC plugin is for version " << GCCPLUGIN_VERSION_MAJOR << "." << GCCPLUGIN_VERSION_MINOR << "\n";
    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/04_callbacks_instrumented/callbacks.cc","line":75,"ret_type":"int","expr":"1"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
		return 1;
    }

    register_callback(plugin_info->base_name,
            /* event */ PLUGIN_INFO,
            /* callback */ NULL, /* user_data */ &my_gcc_plugin_info);

    register_callback(plugin_info->base_name,
            PLUGIN_FINISH_TYPE,
            callback_finish_type, /* user_data */ NULL);

    register_callback(plugin_info->base_name,
            PLUGIN_FINISH_DECL,
            callback_finish_declaration, /* user_data */ NULL);

    register_callback(plugin_info->base_name,
            PLUGIN_FINISH_UNIT,
            callback_finish_unit, /* user_data */ NULL);

    register_callback(plugin_info->base_name,
            PLUGIN_PRE_GENERICIZE,
            callback_pre_genericize, /* user_data */ NULL);

    register_callback(plugin_info->base_name,
            PLUGIN_FINISH,
            callback_finish, /* user_data */ NULL);

    register_callback(plugin_info->base_name,
            PLUGIN_ATTRIBUTES,
            callback_register_attribute, /* user_data */ NULL);

    register_callback(plugin_info->base_name,
            PLUGIN_START_UNIT,
            callback_start_unit, /* user_data */ NULL);

    register_callback(plugin_info->base_name,
            PLUGIN_PRAGMAS,
            callback_registering_pragmas, /* user_data */ NULL);

    register_callback(plugin_info->base_name,
            PLUGIN_ALL_PASSES_START,
            callback_all_passes_start, /* user_data */ NULL);

    register_callback(plugin_info->base_name,
            PLUGIN_ALL_PASSES_END,
            callback_all_passes_end, /* user_data */ NULL);

    register_callback(plugin_info->base_name,
            PLUGIN_PASS_EXECUTION,
            callback_pass_execution, /* user_data */ NULL);

    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/04_callbacks_instrumented/callbacks.cc","line":126,"ret_type":"int","expr":"0"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
    return 0;
}
