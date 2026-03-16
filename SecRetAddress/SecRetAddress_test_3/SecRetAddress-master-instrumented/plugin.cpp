#include <ctime>
#include <fstream>
//#include "gimple-walk.h"
//#include "intl.h"
//#include "tree-ssa-alias.h"
//#include "basic-block.h"
//#include "gimple-expr.h"
//#include "function.h"


#include <iostream>
#include <string>

#include "gcc-plugin.h"
#include "plugin-version.h"

#include "context.h"
#include "tree.h"
#include "tree-pass.h"
#include "gimple.h"
#include "gimple-pretty-print.h"
#include "gimple-iterator.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    std::ofstream log_file("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/trace_plugin.cpp.log", std::ios::app);
    log_file << "{\"ts\":\"" << time_buf
             << "\",\"file\":\"plugin.cpp\",\"msg\":" << msg
             << "}" << std::endl;
}

#define RESET   "\033[0m"
#define RED		"\033[31m"
#define GREEN   "\033[32m" 


int plugin_is_GPL_compatible;


static inline tree build_const_char_string(int len, const char *str)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/plugin.cpp","function":"build_const_char_string","func_id":9,"num_params":2,"start_line":30,"end_line":43,"metrics":{"num_params":2,"call_count":3,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"len","type":"int","is_pointer":false,"is_const":false},{"name":"str","type":"const char *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"build_const_char_string","func_id":9,"name":"len","type":"int","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"build_const_char_string","func_id":9,"name":"str","type":"const char *","is_pointer":true,"is_const":false})JSON");
	tree cstr, elem, index, type;

	cstr = build_string(len, str);
	elem = build_type_variant(char_type_node, 1, 0);
	index = build_index_type(size_int(len - 1));
	type = build_array_type(elem, index);
	TREE_TYPE(cstr) = type;
	TREE_CONSTANT(cstr) = 1;
	TREE_READONLY(cstr) = 1;
	TREE_STATIC(cstr) = 1;
    log_with_timestamp(R"JSON({"event":"RETURN","function":"build_const_char_string","func_id":9,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/plugin.cpp","line":42,"ret_type":"int","expr":"cstr"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"build_const_char_string","func_id":9})JSON");
	return cstr;
}


static vec<tree, va_gc> * create_clobbers(std::string clobbers_str){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/plugin.cpp","function":"create_clobbers","func_id":8,"num_params":1,"start_line":46,"end_line":66,"metrics":{"num_params":1,"call_count":7,"has_recursion":false,"has_loop":true,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"clobbers_str","type":"std::string","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"create_clobbers","func_id":8,"name":"clobbers_str","type":"std::string","is_pointer":false,"is_const":false})JSON");
	std::string delimiter = ",";
	std::string token;
	int pos = 0;
	tree clob;
	vec<tree, va_gc> *clobbers = NULL;


	while ((pos = clobbers_str.find(delimiter)) != std::string::npos) {
		token = clobbers_str.substr(0, pos);
		//std::cout << token.length() << std::endl;
		clob = build_tree_list(NULL_TREE, build_const_char_string(token.length()+1, token.c_str()));
		vec_safe_push(clobbers, clob);
		clobbers_str.erase(0, pos + delimiter.length());
	}

	clob = build_tree_list(NULL_TREE, build_const_char_string(clobbers_str.length()+1, clobbers_str.c_str()));
	vec_safe_push(clobbers, clob);

    log_with_timestamp(R"JSON({"event":"RETURN","function":"create_clobbers","func_id":8,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/plugin.cpp","line":65,"ret_type":"int *","expr":"clobbers"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"create_clobbers","func_id":8})JSON");
	return clobbers;
}

static tree create_pointer(const char *str){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/plugin.cpp","function":"create_pointer","func_id":7,"num_params":1,"start_line":68,"end_line":75,"metrics":{"num_params":1,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"str","type":"const char *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"create_pointer","func_id":7,"name":"str","type":"const char *","is_pointer":true,"is_const":false})JSON");
	tree var;
	var = create_tmp_var(ptr_type_node, str);
	mark_addressable(var);
	SET_SSA_NAME_VAR_OR_IDENTIFIER(ptr_type_node, var);

    log_with_timestamp(R"JSON({"event":"RETURN","function":"create_pointer","func_id":7,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/plugin.cpp","line":74,"ret_type":"int","expr":"var"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"create_pointer","func_id":7})JSON");
	return var;
}

static void insert_asm_instr_before(const char *str, vec<tree, va_gc> *inputs, vec<tree, va_gc> *outputs, 
											vec<tree, va_gc> *clobbers, vec<tree, va_gc> *labels, bool _volatile, gimple_stmt_iterator * gsi){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/plugin.cpp","function":"insert_asm_instr_before","func_id":6,"num_params":7,"start_line":77,"end_line":85,"metrics":{"num_params":7,"call_count":0,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"str","type":"const char *","is_pointer":true,"is_const":false},{"name":"inputs","type":"int *","is_pointer":true,"is_const":false},{"name":"outputs","type":"int *","is_pointer":true,"is_const":false},{"name":"clobbers","type":"int *","is_pointer":true,"is_const":false},{"name":"labels","type":"int *","is_pointer":true,"is_const":false},{"name":"_volatile","type":"bool","is_pointer":false,"is_const":false},{"name":"gsi","type":"int *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"insert_asm_instr_before","func_id":6,"name":"str","type":"const char *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"insert_asm_instr_before","func_id":6,"name":"inputs","type":"int *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"insert_asm_instr_before","func_id":6,"name":"outputs","type":"int *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"insert_asm_instr_before","func_id":6,"name":"clobbers","type":"int *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"insert_asm_instr_before","func_id":6,"name":"labels","type":"int *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"insert_asm_instr_before","func_id":6,"name":"_volatile","type":"bool","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"insert_asm_instr_before","func_id":6,"name":"gsi","type":"int *","is_pointer":true,"is_const":false})JSON");

	gimple *asm_instr = gimple_build_asm_vec(str, inputs, outputs, clobbers, labels);
	gimple_asm_set_volatile((gasm *)asm_instr, _volatile);
	//print_gimple_stmt(stderr, asm_instr, TDF_NONE);
	gsi_insert_before(gsi, asm_instr, GSI_NEW_STMT);

    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"insert_asm_instr_before","func_id":6})JSON");
}

static void insert_asm_instr_after(const char *str, vec<tree, va_gc> *inputs, vec<tree, va_gc> *outputs, 
											vec<tree, va_gc> *clobbers, vec<tree, va_gc> *labels, bool _volatile, gimple_stmt_iterator * gsi){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/plugin.cpp","function":"insert_asm_instr_after","func_id":5,"num_params":7,"start_line":87,"end_line":95,"metrics":{"num_params":7,"call_count":0,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"str","type":"const char *","is_pointer":true,"is_const":false},{"name":"inputs","type":"int *","is_pointer":true,"is_const":false},{"name":"outputs","type":"int *","is_pointer":true,"is_const":false},{"name":"clobbers","type":"int *","is_pointer":true,"is_const":false},{"name":"labels","type":"int *","is_pointer":true,"is_const":false},{"name":"_volatile","type":"bool","is_pointer":false,"is_const":false},{"name":"gsi","type":"int *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"insert_asm_instr_after","func_id":5,"name":"str","type":"const char *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"insert_asm_instr_after","func_id":5,"name":"inputs","type":"int *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"insert_asm_instr_after","func_id":5,"name":"outputs","type":"int *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"insert_asm_instr_after","func_id":5,"name":"clobbers","type":"int *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"insert_asm_instr_after","func_id":5,"name":"labels","type":"int *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"insert_asm_instr_after","func_id":5,"name":"_volatile","type":"bool","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"insert_asm_instr_after","func_id":5,"name":"gsi","type":"int *","is_pointer":true,"is_const":false})JSON");

	gimple *asm_instr = gimple_build_asm_vec(str, inputs, outputs, clobbers, labels);
	gimple_asm_set_volatile((gasm *)asm_instr, _volatile);
	//print_gimple_stmt(stderr, asm_instr, TDF_NONE);
	gsi_insert_after(gsi, asm_instr, GSI_NEW_STMT);

    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"insert_asm_instr_after","func_id":5})JSON");
}

static void instrument_entry(function *fun, tree var){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/plugin.cpp","function":"instrument_entry","func_id":4,"num_params":2,"start_line":97,"end_line":144,"metrics":{"num_params":2,"call_count":8,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"fun","type":"int *","is_pointer":true,"is_const":false},{"name":"var","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"instrument_entry","func_id":4,"name":"fun","type":"int *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"instrument_entry","func_id":4,"name":"var","type":"int","is_pointer":false,"is_const":false})JSON");

	basic_block bb; 
	gimple_stmt_iterator gsi_entry_bb;
	tree output;
	vec<tree, va_gc> *outputs = NULL;
	vec<tree, va_gc> *clobbers = NULL;

	bb = single_succ(ENTRY_BLOCK_PTR_FOR_FN(fun));
	gsi_entry_bb = gsi_start_bb(bb);
	
	clobbers = create_clobbers("r8,r9,ecx,edx,esi,edi,rax");

	const char *asm_instr = "xor %%r9, %%r9\n\t"
							"mov $0xffffffff, %%r8d\n\t"
							"xor %%ecx, %%ecx\n\t"
							"mov $0x22, %%cl\n\t"
							"xor %%edx, %%edx\n\t"
							"mov $0x3, %%dl\n\t"
							"mov $0x8, %%esi\n\t"
							"xor %%edi, %%edi\n\t"
							"call mmap@plt\n\t";
	
	insert_asm_instr_before(asm_instr, NULL, NULL, clobbers, NULL, true, &gsi_entry_bb);

	output = build_tree_list(NULL_TREE, build_const_char_string(3, "=r")); 
	output = chainon(NULL_TREE, build_tree_list(output, var));
	vec_safe_push(outputs, output);

	clobbers = create_clobbers("memory");

	asm_instr = "mov %%rax, %0";
	insert_asm_instr_after(asm_instr,NULL,outputs,clobbers,NULL,true,&gsi_entry_bb);


	clobbers = create_clobbers("rcx,memory");
	asm_instr = "mov 0x8(%%rbp), %%rcx\n\t"
                "mov %%rcx, (%%rax)\n\t";
	insert_asm_instr_after(asm_instr,NULL,NULL,clobbers,NULL,true,&gsi_entry_bb);

	clobbers = create_clobbers("edx,rax,rdi");
	asm_instr = "mov $0x1, %%dl\n\t"
			    "mov %%rax, %%rdi\n\t"
			    "call mprotect@plt\n\t";
	insert_asm_instr_after(asm_instr,NULL,NULL,clobbers,NULL,true,&gsi_entry_bb);


    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"instrument_entry","func_id":4})JSON");
}

static void instrument_exit(function *fun, tree var){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/plugin.cpp","function":"instrument_exit","func_id":3,"num_params":2,"start_line":146,"end_line":171,"metrics":{"num_params":2,"call_count":5,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"fun","type":"int *","is_pointer":true,"is_const":false},{"name":"var","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"instrument_exit","func_id":3,"name":"fun","type":"int *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"instrument_exit","func_id":3,"name":"var","type":"int","is_pointer":false,"is_const":false})JSON");
	basic_block bb; 
	gimple_stmt_iterator gsi_exit_bb;
	tree input;
	vec<tree, va_gc> *inputs = NULL;
	vec<tree, va_gc> *clobbers = NULL;

	bb = single_pred(EXIT_BLOCK_PTR_FOR_FN(fun));
	gsi_exit_bb = gsi_last_bb(bb);

	clobbers = create_clobbers("rax,rdi,esi,memory");
	input = build_tree_list(NULL_TREE, build_const_char_string(2, "r"));
	input = chainon(NULL_TREE, build_tree_list(input, var));
	vec_safe_push(inputs, input);

	const char *asm_instr = "mov (%0), %%rax\n\t"
                            "xor 0x8(%%rbp), %%rax\n\t"
                            "jne __stack_chk_fail@plt\n\t"
							"mov %%rdx, %%rdi\n\t"
							"mov $0x8, %%esi\n\t"
							"call munmap@plt\n\t";
	
	insert_asm_instr_before(asm_instr,inputs,NULL,clobbers,NULL,true,&gsi_exit_bb);


    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"instrument_exit","func_id":3})JSON");
}

static bool to_instrument(function *fun){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/plugin.cpp","function":"to_instrument","func_id":2,"num_params":1,"start_line":173,"end_line":190,"metrics":{"num_params":1,"call_count":6,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"fun","type":"int *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"to_instrument","func_id":2,"name":"fun","type":"int *","is_pointer":true,"is_const":false})JSON");
	bool to_instr = false;
	std::string func_name = function_name(fun);
	std::cout << "[*] Checking function: '" << func_name << "'" << std::endl;

	vec<tree, va_gc> *local_declarations = fun->local_decls;
	if(local_declarations == NULL) {
	    log_with_timestamp(R"JSON({"event":"RETURN","function":"to_instrument","func_id":2,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/plugin.cpp","line":180,"ret_type":"bool","expr":"false"})JSON");
	    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"to_instrument","func_id":2})JSON");
	    return false;
	}

	for(tree *decl = local_declarations->begin(); decl != local_declarations->end(); decl++){
		if(TREE_CODE (TREE_TYPE (*decl)) == ARRAY_TYPE){
			std::cout << RED << "[-] Found Buffer: " << get_name(*decl) << std::endl << RESET;
			to_instr = true;
		}
	}

    log_with_timestamp(R"JSON({"event":"RETURN","function":"to_instrument","func_id":2,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/plugin.cpp","line":189,"ret_type":"bool","expr":"to_instr"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"to_instrument","func_id":2})JSON");
	return to_instr;
}

static unsigned int instrument_functions(function *fun){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/plugin.cpp","function":"instrument_functions","func_id":1,"num_params":1,"start_line":192,"end_line":213,"metrics":{"num_params":1,"call_count":8,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"fun","type":"int *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"instrument_functions","func_id":1,"name":"fun","type":"int *","is_pointer":true,"is_const":false})JSON");

	if(!to_instrument(fun)) {
	    log_with_timestamp(R"JSON({"event":"RETURN","function":"instrument_functions","func_id":1,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/plugin.cpp","line":195,"ret_type":"unsigned int","expr":"0"})JSON");
	    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"instrument_functions","func_id":1})JSON");
	    return 0;
	}
	

	std::string func_name = function_name(fun);
  	std::cout << "[!] Instrumenting function: '" << func_name << "' at: " 
	  << LOCATION_FILE(fun->function_start_locus) << ":" << LOCATION_LINE(fun->function_start_locus) << std::endl;

	
	tree m_addr = create_pointer("m_addr");

	instrument_entry(fun, m_addr);

	instrument_exit(fun, m_addr);

	std::cout << GREEN << "[+] Instrumented funtion: '" << func_name << "'" << std::endl << RESET;


    log_with_timestamp(R"JSON({"event":"RETURN","function":"instrument_functions","func_id":1,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/plugin.cpp","line":212,"ret_type":"unsigned int","expr":"0"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"instrument_functions","func_id":1})JSON");
	return 0;
}




struct plugin_info instr_plugin_info = {
	"1.0", 
	"Instrumentation code plugin"
};


namespace {
    const pass_data instrumentation_pass_data = {
        GIMPLE_PASS,
        "instr_pass2",           /* name */
        OPTGROUP_NONE,          /* optinfo_flags */
        TV_NONE,                /* tv_id */
        (PROP_ssa | PROP_cfg | PROP_gimple_leh),    /* properties_required */
        0,                      /* properties_provided */
        0,                      /* properties_destroyed */
        0,                      /* todo_flags_start */
        0                       /* todo_flags_finish */
    };

    struct instrumentation_pass : gimple_opt_pass {
        instrumentation_pass(gcc::context *ctx) : gimple_opt_pass(instrumentation_pass_data, ctx){}

			unsigned int execute(function *fun) {
				return instrument_functions(fun);
			}
    };	


}

int plugin_init(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/plugin.cpp","function":"plugin_init","func_id":0,"num_params":2,"start_line":248,"end_line":271,"metrics":{"num_params":2,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"plugin_info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false},{"name":"version","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"plugin_info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"version","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false})JSON");
  	if(!plugin_default_version_check(version, &gcc_version)){
		std::cerr << "Error, version mismatching!" << std::endl;
    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/plugin.cpp","line":251,"ret_type":"int","expr":"1"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
		return 1;
	}
	
	std::cout << std::endl;
	
	struct register_pass_info instr_pass_info;

	instr_pass_info.pass = new instrumentation_pass(g);
	instr_pass_info.reference_pass_name = "cfg";
	instr_pass_info.ref_pass_instance_number = 1;
  	instr_pass_info.pos_op = PASS_POS_INSERT_AFTER;


	register_callback(plugin_info->base_name, PLUGIN_INFO, NULL, &instr_plugin_info);
	register_callback(plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &instr_pass_info);
	

	

    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/SecRetAddress/SecRetAddress-master-instrumented/plugin.cpp","line":270,"ret_type":"int","expr":"0"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
	return 0;
}

