#include <ctime>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <stdexcept>

#include "log.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    std::ofstream log_file("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/trace_log.cpp.log", std::ios::app);
    log_file << "{\"ts\":\"" << time_buf
             << "\",\"file\":\"log.cpp\",\"msg\":" << msg
             << "}" << std::endl;
}

namespace log {

static std::map<std::string, log_level_t> log_level_map_create(void)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/log.cpp","function":"log_level_map_create","func_id":2,"num_params":0,"start_line":9,"end_line":38,"metrics":{"num_params":0,"call_count":48,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[]})JSON");
	std::map<std::string, log_level_t> ret;

	ret["quite"]	= LOG_NONE;
	ret["no"]	= LOG_NONE;
	ret["none"]	= LOG_NONE;
	ret["off"]	= LOG_NONE;
	ret["Quite"]	= LOG_NONE;
	ret["No"]	= LOG_NONE;
	ret["None"]	= LOG_NONE;
	ret["Off"]	= LOG_NONE;
	ret["error"]	= LOG_ERROR;
	ret["Error"]	= LOG_ERROR;
	ret["err"]	= LOG_ERROR;
	ret["Err"]	= LOG_ERROR;
	ret["warn"]	= LOG_WARN;
	ret["warning"]	= LOG_WARN;
	ret["Warn"]	= LOG_WARN;
	ret["Warning"]	= LOG_WARN;
	ret["info"]	= LOG_INFO;
	ret["Info"]	= LOG_INFO;
	ret["debug"]	= LOG_DEBUG;
	ret["Debug"]	= LOG_DEBUG;
	ret["all"]	= LOG_ALL;
	ret["All"]	= LOG_ALL;
	ret["ALL"]	= LOG_ALL;

    log_with_timestamp(R"JSON({"event":"RETURN","function":"log_level_map_create","func_id":2,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/log.cpp","line":37,"ret_type":"std::map<std::string, log_level_t>","expr":"ret"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"log_level_map_create","func_id":2})JSON");
	return ret;
}

const static std::map<std::string, log_level_t>
log_level_map = log_level_map_create();

static log_level_t log_level = LOG_WARN;

void set_log_level(const char *level)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/log.cpp","function":"set_log_level","func_id":1,"num_params":1,"start_line":45,"end_line":54,"metrics":{"num_params":1,"call_count":6,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"level","type":"const char *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"set_log_level","func_id":1,"name":"level","type":"const char *","is_pointer":true,"is_const":false})JSON");
	try {
		log_level = log_level_map.at(level);
	} catch (const std::out_of_range &oor) {
		std::string err("No such log level: `");
		err += level;
		throw std::logic_error(err + "'");
	}
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"set_log_level","func_id":1})JSON");
}

log_level_t curr_log_level(void)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/log.cpp","function":"curr_log_level","func_id":0,"num_params":0,"start_line":56,"end_line":59,"metrics":{"num_params":0,"call_count":0,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[]})JSON");
    log_with_timestamp(R"JSON({"event":"RETURN","function":"curr_log_level","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/log.cpp","line":58,"ret_type":"log_level_t","expr":"log_level"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"curr_log_level","func_id":0})JSON");
	return log_level;
}

log_stream err(LOG_ERROR), warn(LOG_WARN), info(LOG_INFO),
	   debug(LOG_DEBUG), all(LOG_ALL);

}; /* namespace log */
