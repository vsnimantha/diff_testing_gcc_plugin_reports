#include <ctime>
#include <fstream>
#include <iostream>
#include "gcc-plugin.h"
#include "plugin-version.h"
#include "tree-pass.h"
#include "context.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    std::ofstream log_file("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/05_my_first_pass_instrumented/trace_my_first_pass.cc.log", std::ios::app);
    log_file << "{\"ts\":\"" << time_buf
             << "\",\"file\":\"my_first_pass.cc\",\"msg\":" << msg
             << "}" << std::endl;
}

int plugin_is_GPL_compatible;

static struct plugin_info my_gcc_plugin_info = { "1.0", "This is a very simple plugin" };


namespace
{
    const pass_data my_first_pass_data = 
    {
        GIMPLE_PASS,
        "my_first_pass",        /* name */
        OPTGROUP_NONE,          /* optinfo_flags */
        TV_NONE,                /* tv_id */
        PROP_gimple_any,        /* properties_required */
        0,                      /* properties_provided */
        0,                      /* properties_destroyed */
        0,                      /* todo_flags_start */
        0                       /* todo_flags_finish */
    };

    struct my_first_pass : gimple_opt_pass
    {
        my_first_pass(gcc::context *ctx)
            : gimple_opt_pass(my_first_pass_data, ctx)
        {
        }

        virtual unsigned int execute(function *fun) override
        {
            std::cerr << "Running my first pass, OMG\n";
            return 0;
        }

        virtual my_first_pass* clone() override
        {
            // We do not clone ourselves
            return this;
        }
    };
}

int plugin_init (struct plugin_name_args *plugin_info,
		struct plugin_gcc_version *version)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/05_my_first_pass_instrumented/my_first_pass.cc","function":"plugin_init","func_id":0,"num_params":2,"start_line":48,"end_line":74,"metrics":{"num_params":2,"call_count":0,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"plugin_info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false},{"name":"version","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"plugin_info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"version","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false})JSON");
	// We check the current gcc loading this plugin against the gcc we used to
	// created this plugin
	if (!plugin_default_version_check (version, &gcc_version))
    {
        std::cerr << "This GCC plugin is for version " << GCCPLUGIN_VERSION_MAJOR << "." << GCCPLUGIN_VERSION_MINOR << "\n";
    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/05_my_first_pass_instrumented/my_first_pass.cc","line":56,"ret_type":"int","expr":"1"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
		return 1;
    }

    register_callback(plugin_info->base_name,
            /* event */ PLUGIN_INFO,
            /* callback */ NULL, /* user_data */ &my_gcc_plugin_info);

    struct register_pass_info pass_info;

    // Register the phase right before cfg
    pass_info.pass = new my_first_pass(g);
    pass_info.reference_pass_name = "cfg";
    pass_info.ref_pass_instance_number = 1;
    pass_info.pos_op = PASS_POS_INSERT_BEFORE;

    register_callback (plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &pass_info);

    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/rofirrim_github/gcc-plugins-master/05_my_first_pass_instrumented/my_first_pass.cc","line":73,"ret_type":"int","expr":"0"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
    return 0;
}
