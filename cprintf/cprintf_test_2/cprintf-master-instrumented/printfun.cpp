#include <ctime>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include "log.h"
#include "printfun.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    std::ofstream log_file("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/trace_printfun.cpp.log", std::ios::app);
    log_file << "{\"ts\":\"" << time_buf
             << "\",\"file\":\"printfun.cpp\",\"msg\":" << msg
             << "}" << std::endl;
}

namespace printfun {

std::map<std::string, printfun_t> printfuns;

static const char *parse_get_fmt_pos(const char *printfun_def,
		unsigned int *out, std::string &func)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/printfun.cpp","function":"parse_get_fmt_pos","func_id":3,"num_params":3,"start_line":10,"end_line":41,"metrics":{"num_params":3,"call_count":5,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"printfun_def","type":"const char *","is_pointer":true,"is_const":false},{"name":"out","type":"unsigned int *","is_pointer":true,"is_const":false},{"name":"func","type":"std::string &","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"parse_get_fmt_pos","func_id":3,"name":"printfun_def","type":"const char *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"parse_get_fmt_pos","func_id":3,"name":"out","type":"unsigned int *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"parse_get_fmt_pos","func_id":3,"name":"func","type":"std::string &","is_pointer":false,"is_const":false})JSON");
	size_t fmt_pos_len;

	if (*printfun_def != '(') {
		std::string err("Fmt-string argument position is unknown in function `");
		throw std::logic_error(err + func + "'");
	}
	printfun_def++;
	try {
		*out = std::stoul(printfun_def, &fmt_pos_len, 10);
	} catch(...) {
		std::string err("Invalid format position in `");
		throw std::logic_error(err + func + "' function");
	}
	if (fmt_pos_len == 0) {
		std::string err("Failed to parse format position of `");
		throw std::logic_error(err + func + "' function");
	}
	printfun_def += fmt_pos_len;
	if (*printfun_def != ')') {
		std::string err("Can't find closing brace for fmt-string position of `");
		throw std::logic_error(err + func + "' function");
	}
	printfun_def++;
	if (*printfun_def == '\0') {
		std::string err("Unexpected line end after `");
		throw std::logic_error(err + func + "' function");
	}
    log_with_timestamp(R"JSON({"event":"RETURN","function":"parse_get_fmt_pos","func_id":3,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/printfun.cpp","line":40,"ret_type":"const char *","expr":"printfun_def"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"parse_get_fmt_pos","func_id":3})JSON");
	return printfun_def;
}

static const char *parse_get_function(const char *printfun_def,
		std::string *out)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/printfun.cpp","function":"parse_get_function","func_id":2,"num_params":2,"start_line":43,"end_line":63,"metrics":{"num_params":2,"call_count":5,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"printfun_def","type":"const char *","is_pointer":true,"is_const":false},{"name":"out","type":"std::string *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"parse_get_function","func_id":2,"name":"printfun_def","type":"const char *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"parse_get_function","func_id":2,"name":"out","type":"std::string *","is_pointer":true,"is_const":false})JSON");
	while (ISBLANK(*printfun_def)) printfun_def++;
	if (*printfun_def == '\0')
		throw std::logic_error("No function name specified");
	if (!ISALPHA(*printfun_def) && *printfun_def != '_') {
		std::string err("Function name should start with character or underscore, not with: `");
		err += *printfun_def++;
		while (ISALPHA(*printfun_def) ||
				ISDIGIT(*printfun_def) ||
				*printfun_def == '_')
			err += *printfun_def++;
		throw std::logic_error(err + "'");
	}

	while (ISALPHA(*printfun_def) || ISDIGIT(*printfun_def) ||
			*printfun_def == '_')
		*out += *printfun_def++;
    log_with_timestamp(R"JSON({"event":"RETURN","function":"parse_get_function","func_id":2,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/printfun.cpp","line":62,"ret_type":"const char *","expr":"printfun_def"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"parse_get_function","func_id":2})JSON");
	return printfun_def;
}

static const char *parse_get_specifier(const char *printfun_def,
		std::string *out)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/printfun.cpp","function":"parse_get_specifier","func_id":1,"num_params":2,"start_line":65,"end_line":89,"metrics":{"num_params":2,"call_count":6,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"printfun_def","type":"const char *","is_pointer":true,"is_const":false},{"name":"out","type":"std::string *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"parse_get_specifier","func_id":1,"name":"printfun_def","type":"const char *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"parse_get_specifier","func_id":1,"name":"out","type":"std::string *","is_pointer":true,"is_const":false})JSON");
	while (ISBLANK(*printfun_def)) printfun_def++;
	if (*printfun_def == '\0') {
	    log_with_timestamp(R"JSON({"event":"RETURN","function":"parse_get_specifier","func_id":1,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/printfun.cpp","line":70,"ret_type":"const char *","expr":"printfun_def"})JSON");
	    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"parse_get_specifier","func_id":1})JSON");
	    return printfun_def;
	}
	if (*printfun_def != '%') {
		std::string err("Expected %-specifier, but got: `");
		while (*printfun_def != '\0' && !ISBLANK(*printfun_def))
			err += *printfun_def++;
		throw std::logic_error(err + "'");
	}
	printfun_def++;
	if (ISBLANK(*printfun_def))
		throw std::logic_error("Got empty %-specifier");
	while (!ISBLANK(*printfun_def)) {
		if (*printfun_def == '\0') {
			std::string err("Unexpected %-specifier end, got: `");
			err += "%";
			throw std::logic_error(err + *out + "'");
		}
		*out += *printfun_def++;
	}
    log_with_timestamp(R"JSON({"event":"RETURN","function":"parse_get_specifier","func_id":1,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/printfun.cpp","line":88,"ret_type":"const char *","expr":"printfun_def"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"parse_get_specifier","func_id":1})JSON");
	return printfun_def;
}

void add_printfun(const char *printfun_def)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/cprintf/cprintf-master-instrumented/printfun.cpp","function":"add_printfun","func_id":0,"num_params":1,"start_line":91,"end_line":151,"metrics":{"num_params":1,"call_count":27,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"printfun_def","type":"const char *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"add_printfun","func_id":0,"name":"printfun_def","type":"const char *","is_pointer":true,"is_const":false})JSON");
	printfun_t pf;
	std::string fun_name;
	unsigned int i;

	printfun_def = parse_get_function(printfun_def, &fun_name);
	printfun_def = parse_get_fmt_pos(printfun_def,
			&pf.fmt_pos, fun_name);
	printfun_def++; /* skip function delimiter */

	for (i = 0;;i++) {
		std::string spec, func;

		printfun_def = parse_get_specifier(printfun_def, &spec);
		if (*printfun_def == '\0')
			break;
		printfun_def = parse_get_function(printfun_def, &func);
		printfun_def++; /* skip function delimiter */

		if (pf.spec_to_func.find(spec) != pf.spec_to_func.end()) {
			std::string err("%-Specifier `");
			err += spec;
			throw std::logic_error(err + "' found twice");
		}
		pf.spec_to_func[spec] = func;
		if (spec[0] == '%') { /* it's %% specifier really */
			if (spec.length() > 1) {
				std::string err("Found `%");
				err += spec;
				err += "' specifier for `";
				err += func;
				err += "', can handle only `%%'";
				throw std::logic_error(err);
			}
			log::info << "Reserved %% specifier for `"
				<< func << "'\n";
		}
	}

	if (i == 0) {
		std::string err("Found no %-specifiers for `");
		err += fun_name;
		throw std::logic_error(err + "' function");
	}

	if (printfuns.find(fun_name) != printfuns.end()) {
		std::string err("Function `");
		err += fun_name;
		throw std::logic_error(err + "' defined twice");
	}

	printfuns[fun_name] = pf;

	log::info << "Specifier handlers for `"
		<< fun_name << "(" << pf.fmt_pos << ")':\n";
	std::map<std::string, std::string>::const_iterator s;
	for (s = pf.spec_to_func.cbegin(); s != pf.spec_to_func.cend(); ++s)
		log::debug << "\t%" << (*s).first
			<< "\t" << (*s).second << std::endl;
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"add_printfun","func_id":0})JSON");
}

}; /* namespace printfun */

