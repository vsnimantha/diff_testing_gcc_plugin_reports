#include <ctime>
#include <fstream>
#include <iostream>
#include "gcc-plugin.h"
#include "plugin-version.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    std::ofstream log_file("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/check_conf/trace_plugin.cpp.log", std::ios::app);
    log_file << "{\"ts\":\"" << time_buf
             << "\",\"file\":\"plugin.cpp\",\"msg\":" << msg
             << "}" << std::endl;
}
int plugin_is_GPL_compatible;

int plugin_init(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/check_conf/plugin.cpp","function":"plugin_init","func_id":0,"num_params":2,"start_line":6,"end_line":14,"metrics":{"num_params":2,"call_count":2,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"plugin_info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false},{"name":"version","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"plugin_info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"version","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false})JSON");
       
	if(!plugin_default_version_check(version, &gcc_version)){
           std::cerr << "Error, version mismatching!" << std::endl;
    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/check_conf/plugin.cpp","line":10,"ret_type":"int","expr":"1"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
           return 1;
	}
       std::cout << "[+] Plugin Initializated" << std::endl;
    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/check_conf/plugin.cpp","line":13,"ret_type":"int","expr":"0"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
       return 0; 
}
