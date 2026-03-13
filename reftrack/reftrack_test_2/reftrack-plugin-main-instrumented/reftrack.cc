#include <ctime>
#include <fstream>
// GCC reftrack plugin
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    std::ofstream log_file("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/trace_reftrack.cc.log", std::ios::app);
    log_file << "{\"ts\":\"" << time_buf
             << "\",\"file\":\"reftrack.cc\",\"msg\":" << msg
             << "}" << std::endl;
}
// SPDX-License-Identifier: GPL-2.0-only
/************************************************************
Copyright (C) 2022-2023 Aravind Ceyardass (dev@aravind.cc)
************************************************************/

#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <cstring>
#include <functional>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include "gcc-plugin.h"
#include "basic-block.h"
#include "context.h"
#include "function.h"
#include "input.h"
#include "is-a.h"
#include "coretypes.h"
#include "dumpfile.h"
#include "plugin.h"
#include "timevar.h"
#include "tree-core.h"
#include "tree.h"
#include "tree-pass.h"
#include "context.h"
#include "gimple.h"
#include "gimple-expr.h"
#include "gimple-ssa.h"
#include "gimple-iterator.h"
#include "gimplify-me.h"
#include "tree-pass.h"
#include "stringpool.h"
#include "c-family/c-common.h"
#include "diagnostic-core.h"
#include "attribs.h"
#include "langhooks.h"

#include "reftrack.h"

using std::cout;
using std::endl;
using std::for_each;
using std::string;
using std::to_string;
using std::unordered_map;
using std::unordered_set;
using std::vector;


int plugin_is_GPL_compatible;

#define LOG(L, ...)                                                     \
    do {                                                                \
        if (logging(L))                                                 \
            log(L, __VA_ARGS__);                                        \
    } while(0)

namespace reftrack {

    // A helper class
    class xstring : public string {
    public:
        xstring(const char *p) : string(p ? p : "<null>") {}
        xstring(int i) : string(to_string(i)) {}
        xstring(size_t n) : string(to_string(n)) {}
        xstring(void *p) : string(to_string((long)p)) {}
        xstring(const string& s) :string(s){}
    };


    enum log_level {TRACE = 1, DEBUG, WARN, INFO, ERROR};

    static void log(log_level level, const std::vector<xstring>& v, const string delim);

    const char PLUGIN_NAME[] = "reftrack";

    const char REF_ATTR_NAME[] = "reftrack";

    const char REFTRACK_TMP_PREFIX[] = "reftrack_";

    const string horiz_line(60, '=');

    typedef struct {
        tree ref_add_fn;
        tree ref_remove_fn;
        tree pointer_type;
    } ref_info;


    typedef struct {
        enum gimple_code gcode;
        location_t loc;
        tree fn;
        tree var;
        bool use_tmp;
        tree tmp_type;
    } gimple_build_info;

    struct Globals {
        bool debug;
        struct plugin_info pinfo;
        vector<string> fn_list;
        int pass;

        // map from struct to ref function info
        unordered_map<const_tree, ref_info> ref_structs;

        // set of fields that have refcount attribute
        unordered_set<tree> refcount_fields;

        // skip reference tracking to function like malloc, alloc, free, etc.
        unordered_set<tree> ignored_fns;

        // list of destructor functions defined
        unordered_set<tree> destructor_fns;

        unordered_set<string> global_ignored_fns;

        log_level cur_log_level;

        // functions that needs to be replaced with reftrack version
        tree orig_alloc_fn, orig_free_fn;

        // name of the functions that needs to be replaced.
        string orig_alloc_fn_name, orig_free_fn_name;

        // functions that implement the reftrack malloc & free equivalents
        tree reftrack_alloc_fn, reftrack_free_fn;

        // name of the functions that implement the reftrack malloc & free equivalents

        string reftrack_alloc_fn_name, reftrack_free_fn_name;

        // default addref, removeref functions

        tree default_addref_fn, default_removeref_fn;

        // name of the default addref, removeref functions

        string default_addref_fn_name, default_removeref_fn_name;

        // replace malloc, free globally
        bool replace_memfn;

        // newline
        char nl;

        tree get_tree(const string& name){
            return lookup_name(get_identifier(name.c_str()));
        }

        tree get_orig_alloc_fn(){
            return orig_alloc_fn;
        }
        tree get_orig_free_fn(){
            return orig_free_fn;
        }
        tree get_reftrack_alloc_fn(){
            return reftrack_alloc_fn;
        }
        tree get_reftrack_free_fn(){
            return reftrack_free_fn;
        }
        tree get_default_addref_fn(){
            return default_addref_fn;
        }
        tree get_default_removeref_fn(){
            return default_removeref_fn;
        }

    };

    static struct Globals G = {
        .debug = true,
        .pinfo = {.version = "1.0.0",
            .help = "reftrack: list functions"
        },
        .pass = 0,
        .global_ignored_fns = {"memset", "free"},
        .cur_log_level = ERROR,
        .orig_alloc_fn_name = "malloc",
        .orig_free_fn_name = "free",
        .reftrack_alloc_fn_name = "rc_malloc_",
        .reftrack_free_fn_name = "rc_free_",
        .nl = '\n'
    };

    static const pass_data reftrack_data = {
        GIMPLE_PASS,
        "reftrack",
        OPTGROUP_NONE,
        TV_NONE,
        PROP_gimple_any,
        0,
        0,
        0,
        0
    };

    static const pass_data reftrack_cleanup_data = {
        GIMPLE_PASS,
        "reftrack_cleanup",
        OPTGROUP_NONE,
        TV_NONE,
        PROP_gimple_any,
        0,
        0,
        0,
        0
    };

    static inline string gimple_loc_str(const gimple *g){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"gimple_loc_str","func_id":49,"num_params":1,"start_line":218,"end_line":225,"metrics":{"num_params":1,"call_count":7,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"g","type":"const int *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"gimple_loc_str","func_id":49,"name":"g","type":"const int *","is_pointer":true,"is_const":false})JSON");
        xstring rv{gimple_filename(g)};
        rv+=':';
        rv+=xstring(gimple_lineno(g));
        rv+=":0";
    log_with_timestamp(R"JSON({"event":"RETURN","function":"gimple_loc_str","func_id":49,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":223,"ret_type":"string","expr":"rv"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"gimple_loc_str","func_id":49})JSON");
        return rv;

    }

    static inline bool logging(log_level l){ return l >= G.cur_log_level;}

    static inline void log(log_level level, const std::vector<xstring>& v, const string delim=" "){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"log","func_id":48,"num_params":3,"start_line":229,"end_line":236,"metrics":{"num_params":3,"call_count":5,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"level","type":"log_level","is_pointer":false,"is_const":false},{"name":"v","type":"const int &","is_pointer":false,"is_const":false},{"name":"delim","type":"const string","is_pointer":false,"is_const":true}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"log","func_id":48,"name":"level","type":"log_level","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"log","func_id":48,"name":"v","type":"const int &","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"log","func_id":48,"name":"delim","type":"const string","is_pointer":false,"is_const":true})JSON");

        if (logging(level)){

            for_each(cbegin(v), cend(v), [&](const auto& s){ cout << s << delim;});
            cout << G.nl;
        }
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"log","func_id":48})JSON");
    }

     static inline bool is_generated(const_tree var){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"is_generated","func_id":47,"num_params":1,"start_line":238,"end_line":243,"metrics":{"num_params":1,"call_count":2,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"var","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"is_generated","func_id":47,"name":"var","type":"int","is_pointer":false,"is_const":false})JSON");

         bool rv = var && (DECL_ARTIFICIAL(var)
                           || !DECL_NAME(var));
    log_with_timestamp(R"JSON({"event":"RETURN","function":"is_generated","func_id":47,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":242,"ret_type":"bool","expr":"rv"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_generated","func_id":47})JSON");
         return rv;
    }

    static inline string identifier_name(const_tree t){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"identifier_name","func_id":46,"num_params":1,"start_line":245,"end_line":250,"metrics":{"num_params":1,"call_count":2,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"t","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"identifier_name","func_id":46,"name":"t","type":"int","is_pointer":false,"is_const":false})JSON");
        string rv;
        if (get_name((tree)t))
            rv = get_name((tree)t);
    log_with_timestamp(R"JSON({"event":"RETURN","function":"identifier_name","func_id":46,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":249,"ret_type":"string","expr":"rv"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"identifier_name","func_id":46})JSON");
        return rv;
    }

    static inline string symbol_name(const_tree t) {
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"symbol_name","func_id":45,"num_params":1,"start_line":252,"end_line":268,"metrics":{"num_params":1,"call_count":4,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"t","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"symbol_name","func_id":45,"name":"t","type":"int","is_pointer":false,"is_const":false})JSON");
        string name = "?";

        if (!t) {
            log_with_timestamp(R"JSON({"event":"RETURN","function":"symbol_name","func_id":45,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":256,"ret_type":"string","expr":"\"<null>\""})JSON");
            log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"symbol_name","func_id":45})JSON");
            return "<null>";
        }

        if (is_generated(t)){
            name = "G."+ xstring((size_t)DECL_UID(t));
        }
        else if (!identifier_name(t).size()){
            name = "U."+ xstring((size_t)DECL_UID(t));
        }
        else{
            name = identifier_name(t);
        }
    log_with_timestamp(R"JSON({"event":"RETURN","function":"symbol_name","func_id":45,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":267,"ret_type":"string","expr":"name"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"symbol_name","func_id":45})JSON");
        return name;
    }


    static bool is_call_arg(tree arg, gcall *callee){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"is_call_arg","func_id":44,"num_params":2,"start_line":271,"end_line":279,"metrics":{"num_params":2,"call_count":2,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"arg","type":"int","is_pointer":false,"is_const":false},{"name":"callee","type":"int *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"is_call_arg","func_id":44,"name":"arg","type":"int","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"is_call_arg","func_id":44,"name":"callee","type":"int *","is_pointer":true,"is_const":false})JSON");
        auto arg_count = gimple_call_num_args(callee);
        for(unsigned ai = 0; ai < arg_count; ai++){
            auto call_arg = gimple_call_arg(callee, ai);
            if (call_arg == arg) {
                log_with_timestamp(R"JSON({"event":"RETURN","function":"is_call_arg","func_id":44,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":276,"ret_type":"bool","expr":"true"})JSON");
                log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_call_arg","func_id":44})JSON");
                return true;
            }
        }
    log_with_timestamp(R"JSON({"event":"RETURN","function":"is_call_arg","func_id":44,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":278,"ret_type":"bool","expr":"false"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_call_arg","func_id":44})JSON");
        return false;
    }

    static inline string type_name(const_tree t) {
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"type_name","func_id":43,"num_params":1,"start_line":281,"end_line":295,"metrics":{"num_params":1,"call_count":7,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"t","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"type_name","func_id":43,"name":"t","type":"int","is_pointer":false,"is_const":false})JSON");
        string name {'?'};

        auto tcc = TREE_CODE_CLASS(TREE_CODE(t));
        auto tname = TYPE_NAME(t);

        if (tcc == tcc_type && tname) {
            if (TREE_CODE(tname) == IDENTIFIER_NODE)
                name = IDENTIFIER_POINTER(tname);
            else if (TREE_CODE(tname) == TYPE_DECL && DECL_NAME(tname))
                name = IDENTIFIER_POINTER(DECL_NAME(tname));
        }

    log_with_timestamp(R"JSON({"event":"RETURN","function":"type_name","func_id":43,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":294,"ret_type":"string","expr":"name"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"type_name","func_id":43})JSON");
        return name;
    }

    static inline bool is_static(const_tree var){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"is_static","func_id":42,"num_params":1,"start_line":297,"end_line":299,"metrics":{"num_params":1,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"var","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"is_static","func_id":42,"name":"var","type":"int","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"RETURN","function":"is_static","func_id":42,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":298,"ret_type":"bool","expr":"TREE_STATIC(var)"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_static","func_id":42})JSON");
        return TREE_STATIC(var);
    }

    static inline bool is_array_elem(const_tree var){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"is_array_elem","func_id":41,"num_params":1,"start_line":301,"end_line":303,"metrics":{"num_params":1,"call_count":0,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"var","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"is_array_elem","func_id":41,"name":"var","type":"int","is_pointer":false,"is_const":false})JSON");
        log_with_timestamp(R"JSON({"event":"RETURN","function":"is_array_elem","func_id":41,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":304,"ret_type":"bool","expr":"TREE_CODE(var) == ARRAY_REF"})JSON");
        log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_array_elem","func_id":41})JSON");
        return TREE_CODE(var) == ARRAY_REF;
    }

    static inline bool is_component(const_tree var){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"is_component","func_id":40,"num_params":1,"start_line":305,"end_line":307,"metrics":{"num_params":1,"call_count":0,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"var","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"is_component","func_id":40,"name":"var","type":"int","is_pointer":false,"is_const":false})JSON");
        log_with_timestamp(R"JSON({"event":"RETURN","function":"is_component","func_id":40,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":308,"ret_type":"bool","expr":"TREE_CODE(var) == COMPONENT_REF"})JSON");
        log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_component","func_id":40})JSON");
        return TREE_CODE(var) == COMPONENT_REF;
    }


    static inline string attr_name(const_tree t){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"attr_name","func_id":39,"num_params":1,"start_line":310,"end_line":316,"metrics":{"num_params":1,"call_count":4,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"t","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"attr_name","func_id":39,"name":"t","type":"int","is_pointer":false,"is_const":false})JSON");
        string name;
        if (t){
            name = IDENTIFIER_POINTER(TREE_PURPOSE(t));
        }
    log_with_timestamp(R"JSON({"event":"RETURN","function":"attr_name","func_id":39,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":315,"ret_type":"string","expr":"name"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"attr_name","func_id":39})JSON");
        return name;
    }

    static inline bool null_value(const_tree var){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"null_value","func_id":38,"num_params":1,"start_line":318,"end_line":320,"metrics":{"num_params":1,"call_count":0,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"var","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"null_value","func_id":38,"name":"var","type":"int","is_pointer":false,"is_const":false})JSON");
        log_with_timestamp(R"JSON({"event":"RETURN","function":"null_value","func_id":38,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":321,"ret_type":"bool","expr":"TREE_CODE(var) == INTEGER_CST && int_cst_value(var) == 0"})JSON");
        log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"null_value","func_id":38})JSON");
        return TREE_CODE(var) == INTEGER_CST && int_cst_value(var) == 0;
    }

    static inline string function_name(const_tree t){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"function_name","func_id":37,"num_params":1,"start_line":322,"end_line":328,"metrics":{"num_params":1,"call_count":7,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"t","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"function_name","func_id":37,"name":"t","type":"int","is_pointer":false,"is_const":false})JSON");
        string name;
        if (DECL_NAME(t)){
            name = xstring(IDENTIFIER_POINTER(DECL_NAME(t)));
        }
    log_with_timestamp(R"JSON({"event":"RETURN","function":"function_name","func_id":37,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":327,"ret_type":"string","expr":"name"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"function_name","func_id":37})JSON");
        return name;
    }

    static inline bool is_ignored_function(tree fn){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"is_ignored_function","func_id":36,"num_params":1,"start_line":330,"end_line":332,"metrics":{"num_params":1,"call_count":2,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"fn","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"is_ignored_function","func_id":36,"name":"fn","type":"int","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"RETURN","function":"is_ignored_function","func_id":36,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":331,"ret_type":"bool","expr":"G.ignored_fns.find(fn) != G.ignored_fns.end()"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_ignored_function","func_id":36})JSON");
        return G.ignored_fns.find(fn) != G.ignored_fns.end();
    }

    static string get_enclosing_type(const tree & t){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"get_enclosing_type","func_id":35,"num_params":1,"start_line":334,"end_line":347,"metrics":{"num_params":1,"call_count":6,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"t","type":"const int &","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"get_enclosing_type","func_id":35,"name":"t","type":"const int &","is_pointer":false,"is_const":false})JSON");
        string rv {""};

        auto et = decl_type_context(t);

        if (0 && et && IS_TYPE_OR_DECL_P(et)){
            auto & etype_ID = DECL_NAME(et);

            const char *name =
                (etype_ID ? IDENTIFIER_POINTER(etype_ID) : "");
            rv = name;
        }
    log_with_timestamp(R"JSON({"event":"RETURN","function":"get_enclosing_type","func_id":35,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":346,"ret_type":"string","expr":"rv"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"get_enclosing_type","func_id":35})JSON");
        return rv;
    }

    // returns the type name of the given type
    static string c_type_name(const_tree ttype) {
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"c_type_name","func_id":34,"num_params":1,"start_line":350,"end_line":371,"metrics":{"num_params":1,"call_count":5,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":true,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"ttype","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"c_type_name","func_id":34,"name":"ttype","type":"int","is_pointer":false,"is_const":false})JSON");
        string tname;
        int tcode = TREE_CODE(ttype);

        switch (tcode) {

        case POINTER_TYPE:{

            auto pt = TREE_TYPE(ttype);
            if (pt) {
                tname = c_type_name(pt);
            }
            tname += "*";
            break;
        }

        default:
            tname = type_name(ttype);
            break;
        }
    log_with_timestamp(R"JSON({"event":"RETURN","function":"c_type_name","func_id":34,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":370,"ret_type":"string","expr":"tname"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"c_type_name","func_id":34})JSON");
        return tname;
    }

    // Returns the tree node of the ultimate type that a typedef refers to
    static const_tree get_ultimate_type(const_tree ttype){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"get_ultimate_type","func_id":33,"num_params":1,"start_line":374,"end_line":384,"metrics":{"num_params":1,"call_count":3,"has_recursion":false,"has_loop":true,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"ttype","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"get_ultimate_type","func_id":33,"name":"ttype","type":"int","is_pointer":false,"is_const":false})JSON");

        const_tree ut = ttype;

        while(typedef_variant_p(ut)){
            auto tname = TYPE_NAME(ut);
            ut = DECL_ORIGINAL_TYPE(tname);
        }

    log_with_timestamp(R"JSON({"event":"RETURN","function":"get_ultimate_type","func_id":33,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":383,"ret_type":"int","expr":"ut"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"get_ultimate_type","func_id":33})JSON");
        return ut;
    }

    static const char *tree_code_str(enum tree_code tc){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"tree_code_str","func_id":32,"num_params":1,"start_line":386,"end_line":388,"metrics":{"num_params":1,"call_count":2,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"tc","type":"enum tree_code","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"tree_code_str","func_id":32,"name":"tc","type":"enum tree_code","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"RETURN","function":"tree_code_str","func_id":32,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":387,"ret_type":"const char *","expr":"TREE_CODE_CLASS_STRING(TREE_CODE_CLASS(tc))"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"tree_code_str","func_id":32})JSON");
        return TREE_CODE_CLASS_STRING(TREE_CODE_CLASS(tc));
    }

    static const_tree get_pointee(const_tree ttype){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"get_pointee","func_id":31,"num_params":1,"start_line":390,"end_line":395,"metrics":{"num_params":1,"call_count":0,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"ttype","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"get_pointee","func_id":31,"name":"ttype","type":"int","is_pointer":false,"is_const":false})JSON");
        if (TREE_CODE(ttype) == POINTER_TYPE) {
            log_with_timestamp(R"JSON({"event":"RETURN","function":"get_pointee","func_id":31,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":394,"ret_type":"int","expr":"get_ultimate_type(TREE_TYPE(ttype))"})JSON");
            log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"get_pointee","func_id":31})JSON");
            return get_ultimate_type(TREE_TYPE(ttype));
        }
        else
            log_with_timestamp(R"JSON({"event":"RETURN","function":"get_pointee","func_id":31,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":399,"ret_type":"int","expr":"NULL_TREE"})JSON");
            log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"get_pointee","func_id":31})JSON");
            return NULL_TREE;
    }

    static bool get_fn_attr_value(const_tree fn, long& attr_value){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"get_fn_attr_value","func_id":30,"num_params":2,"start_line":397,"end_line":408,"metrics":{"num_params":2,"call_count":5,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"fn","type":"int","is_pointer":false,"is_const":false},{"name":"attr_value","type":"long &","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"get_fn_attr_value","func_id":30,"name":"fn","type":"int","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"get_fn_attr_value","func_id":30,"name":"attr_value","type":"long &","is_pointer":false,"is_const":false})JSON");
        bool rv = false;

        auto ref_attr = lookup_attribute(REF_ATTR_NAME, DECL_ATTRIBUTES(fn));
        if (ref_attr){
            ref_attr = TREE_VALUE(TREE_VALUE(ref_attr));
            rv = (TREE_CODE(ref_attr) == INTEGER_CST);
            if (rv)
                attr_value = int_cst_value(ref_attr);
        }
    log_with_timestamp(R"JSON({"event":"RETURN","function":"get_fn_attr_value","func_id":30,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":407,"ret_type":"bool","expr":"rv"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"get_fn_attr_value","func_id":30})JSON");
        return rv;
    }

    static bool is_heap_function(const_tree fn){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"is_heap_function","func_id":29,"num_params":1,"start_line":410,"end_line":420,"metrics":{"num_params":1,"call_count":1,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"fn","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"is_heap_function","func_id":29,"name":"fn","type":"int","is_pointer":false,"is_const":false})JSON");
        bool rv = false;
        long attr_value = 0;

        if (get_fn_attr_value(fn, attr_value) && (attr_value & REFTRACK_HEAP_FN_FLAG)){
            rv = true;
            LOG(TRACE, {function_name(fn), int(attr_value), ":heap function"});
        }

    log_with_timestamp(R"JSON({"event":"RETURN","function":"is_heap_function","func_id":29,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":419,"ret_type":"bool","expr":"rv"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_heap_function","func_id":29})JSON");
        return rv;
    }

    // Returns ref_info if the tree type is a pointer to one of the structs with ref attribute
    static const ref_info* is_tracked_struct(const_tree ttype){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"is_tracked_struct","func_id":28,"num_params":1,"start_line":423,"end_line":434,"metrics":{"num_params":1,"call_count":2,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"ttype","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"is_tracked_struct","func_id":28,"name":"ttype","type":"int","is_pointer":false,"is_const":false})JSON");
        const ref_info *refinfo = nullptr;
        auto pointee = get_pointee(ttype);

        if (pointee){
            auto it = G.ref_structs.find(pointee);
            if (it != G.ref_structs.cend())
                refinfo = &(it->second);
        }

    log_with_timestamp(R"JSON({"event":"RETURN","function":"is_tracked_struct","func_id":28,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":433,"ret_type":"const ref_info *","expr":"refinfo"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_tracked_struct","func_id":28})JSON");
        return refinfo;
    }

    static string symbol_info(tree s){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"symbol_info","func_id":27,"num_params":1,"start_line":436,"end_line":492,"metrics":{"num_params":1,"call_count":34,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"s","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"symbol_info","func_id":27,"name":"s","type":"int","is_pointer":false,"is_const":false})JSON");
        std::stringstream rv;
        string result;
        if (s){
            rv << '[';
            try{
                rv << symbol_name(s);
                rv << ':';
                string category;

                if (VAR_P(s)){
                    category = 'V';
                }
                if (DECL_P(s)){
                    category += 'D';
                }
                if (TYPE_P(s)){
                    category += 'T';
                }
                if (FUNC_OR_METHOD_TYPE_P(s)){
                    category += 'F';
                }
                if (EXPR_P(s)){
                    category += 'E';
                }

                rv << category;
                rv << ':' << type_name(TREE_TYPE(s));

                rv << (is_generated(s) ? ",G" : "");
                rv << (TREE_USED(s) ? ",USED" : "");
                rv << (DECL_READ_P(s) ? ",READ" : "");
                rv << (is_array_elem(s) ? ",[]" : "");
                rv << (is_component(s) ? ",." : "" );
                rv << (is_tracked_struct(TREE_TYPE(s)) ? ",T" : "");
                rv << ",#B:" <<  (BLOCK_NUMBER(s));
                rv << (is_gimple_val(s) ? ",GIMPLE:V":"");
                rv << (is_gimple_lvalue(s) ? ",GIMPLE:LHS" : "");

                if (VAR_P(s)){

                    if (SSA_NAME_VAR(s)){
                        rv << ",SSA:" << symbol_name(SSA_NAME_VAR(s));
                    }
                    auto fc = get_ultimate_context(s);
                    rv << ",CTX:" << symbol_name(fc);
                }

            }
            catch(std::exception& e){
                rv << " Exception:" << e.what();
            }
            rv << ']';
        }
        rv >> result;
    log_with_timestamp(R"JSON({"event":"RETURN","function":"symbol_info","func_id":27,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":491,"ret_type":"string","expr":"result"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"symbol_info","func_id":27})JSON");
        return result;
    }

    // Returns true if the type of a variable v is of T *v[n] where T is a tracked type
    bool is_array_of_tracked_struct(const_tree var_type){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"is_array_of_tracked_struct","func_id":26,"num_params":1,"start_line":495,"end_line":497,"metrics":{"num_params":1,"call_count":0,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"var_type","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"is_array_of_tracked_struct","func_id":26,"name":"var_type","type":"int","is_pointer":false,"is_const":false})JSON");
        log_with_timestamp(R"JSON({"event":"RETURN","function":"is_array_of_tracked_struct","func_id":26,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":498,"ret_type":"bool","expr":"TREE_CODE(var_type) == ARRAY_TYPE && is_tracked_struct(TREE_TYPE(var_type))"})JSON");
        log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_array_of_tracked_struct","func_id":26})JSON");
        return TREE_CODE(var_type) == ARRAY_TYPE && is_tracked_struct(TREE_TYPE(var_type));
    }

    // returns true if the tree type is a pointer to one of the structs with refcount field
    static bool is_pointer_to(const_tree ttype, const_tree target_type, int cvqual){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"is_pointer_to","func_id":25,"num_params":3,"start_line":500,"end_line":511,"metrics":{"num_params":3,"call_count":2,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"ttype","type":"int","is_pointer":false,"is_const":false},{"name":"target_type","type":"int","is_pointer":false,"is_const":false},{"name":"cvqual","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"is_pointer_to","func_id":25,"name":"ttype","type":"int","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"is_pointer_to","func_id":25,"name":"target_type","type":"int","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"is_pointer_to","func_id":25,"name":"cvqual","type":"int","is_pointer":false,"is_const":false})JSON");
        bool rv = false;

        if (TREE_CODE(ttype) == POINTER_TYPE){
            auto pointee = TREE_TYPE(ttype);
            if (pointee == target_type)
                rv = true;
            if (cvqual)
                rv = (TYPE_QUALS(pointee) & cvqual) != 0;
        }
    log_with_timestamp(R"JSON({"event":"RETURN","function":"is_pointer_to","func_id":25,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":510,"ret_type":"bool","expr":"rv"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_pointer_to","func_id":25})JSON");
        return rv;
    }

    // DFS traversal of all blocks starting from the given block
    ////////////////////////////////////////////////////////////
    static void for_each_block(tree block, tree parent,
                        std::function<void(tree block, tree pblock)> visitor){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"for_each_block","func_id":24,"num_params":3,"start_line":515,"end_line":531,"metrics":{"num_params":3,"call_count":8,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"block","type":"int","is_pointer":false,"is_const":false},{"name":"parent","type":"int","is_pointer":false,"is_const":false},{"name":"visitor","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"for_each_block","func_id":24,"name":"block","type":"int","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"for_each_block","func_id":24,"name":"parent","type":"int","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"for_each_block","func_id":24,"name":"visitor","type":"int","is_pointer":false,"is_const":false})JSON");
        visitor(block, parent);

        for (auto subblock = BLOCK_SUBBLOCKS(block); subblock;
             subblock = BLOCK_CHAIN(subblock)){
            LOG(TRACE, { "Subblock:"});

            for_each_block(subblock, block, visitor);
        }

        auto next_block = BLOCK_CHAIN(block);
        if (next_block){
            LOG(TRACE, { "block:"});
            for_each_block(next_block, parent, visitor);
        }
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"for_each_block","func_id":24})JSON");
    }

    ////////////////////////////////////////////////////////////
    static void for_each_block_var(tree block, std::function<void(tree var)> visitor){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"for_each_block_var","func_id":23,"num_params":2,"start_line":534,"end_line":539,"metrics":{"num_params":2,"call_count":3,"has_recursion":false,"has_loop":true,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"block","type":"int","is_pointer":false,"is_const":false},{"name":"visitor","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"for_each_block_var","func_id":23,"name":"block","type":"int","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"for_each_block_var","func_id":23,"name":"visitor","type":"int","is_pointer":false,"is_const":false})JSON");
        for (auto block_var = BLOCK_VARS(block); block_var;
             block_var = TREE_CHAIN(block_var)) {
            visitor(block_var);
        }
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"for_each_block_var","func_id":23})JSON");
    }

    static int process_block_vars(tree block){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"process_block_vars","func_id":22,"num_params":1,"start_line":541,"end_line":554,"metrics":{"num_params":1,"call_count":3,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"block","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"process_block_vars","func_id":22,"name":"block","type":"int","is_pointer":false,"is_const":false})JSON");
        LOG(TRACE, {"\nBlock variables:{"});
        auto var_count = 0;

        auto var_visitor = [&](auto block_var){
            LOG(TRACE, { symbol_name(block_var), ':', c_type_name(TREE_TYPE(block_var)), ' '});
            var_count++;
        };

        for_each_block_var(block, var_visitor);

        LOG(TRACE, { var_count, "}"});
    log_with_timestamp(R"JSON({"event":"RETURN","function":"process_block_vars","func_id":22,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":553,"ret_type":"int","expr":"var_count"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"process_block_vars","func_id":22})JSON");
        return var_count;
    }

    /*
     * Builds a sequence of call statements.
     * Expected: {(function, arg1), ...}
     */
    static gimple_seq build_refcall_block(const vector<gimple_build_info>& tracked_args, tree block=nullptr){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"build_refcall_block","func_id":21,"num_params":2,"start_line":560,"end_line":602,"metrics":{"num_params":2,"call_count":0,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"tracked_args","type":"const int &","is_pointer":false,"is_const":false},{"name":"block","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"build_refcall_block","func_id":21,"name":"tracked_args","type":"const int &","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"build_refcall_block","func_id":21,"name":"block","type":"int","is_pointer":false,"is_const":false})JSON");
        gimple_seq gseq = nullptr;

        for(const auto& entry : tracked_args){

            auto loc = entry.loc;
            switch(entry.gcode){

            case GIMPLE_CALL:
            {
                auto var = entry.var;
                if (entry.use_tmp){

                    auto tmp_var = create_tmp_var(entry.tmp_type, REFTRACK_TMP_PREFIX);
                    auto tmp_assign = gimple_build_assign(tmp_var, var);

                    gimple_set_location(tmp_assign, loc ? loc : UNKNOWN_LOCATION);
                    if (block) {
                        gimple_set_block(tmp_assign, block);
                    }
                    gimple_seq_add_stmt(&gseq, tmp_assign);

                    var = tmp_var;
                    LOG(TRACE, {"Temporary:", symbol_info(tmp_var)});
                }
                auto call = gimple_build_call(entry.fn, 1, var);

                gimple_set_location(call, loc ? loc : UNKNOWN_LOCATION);
                if (block) gimple_set_block(call, block);
                update_stmt(call);
                LOG(TRACE, {"Adding refcall at", ((loc ? LOCATION_LINE(loc) : -1)), symbol_info(entry.var)});

                gimple_seq_add_stmt(&gseq, call);
            }
            break;

            default:
                break;
            }
        }
    log_with_timestamp(R"JSON({"event":"RETURN","function":"build_refcall_block","func_id":21,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":600,"ret_type":"int","expr":"gseq"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"build_refcall_block","func_id":21})JSON");
        return gseq;

    }

    ////////////////////////////////////////////////////////////
    static bool is_valid_destructor_fn(const_tree fn){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"is_valid_destructor_fn","func_id":20,"num_params":1,"start_line":605,"end_line":634,"metrics":{"num_params":1,"call_count":9,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"fn","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"is_valid_destructor_fn","func_id":20,"name":"fn","type":"int","is_pointer":false,"is_const":false})JSON");

        if (TREE_CODE(fn) != FUNCTION_DECL){
            LOG(TRACE, {"Not a function"});
    log_with_timestamp(R"JSON({"event":"RETURN","function":"is_valid_destructor_fn","func_id":20,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":609,"ret_type":"bool","expr":"false"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_valid_destructor_fn","func_id":20})JSON");
            return false;
        }

        // return type must be void
        auto fn_result = TREE_TYPE(fn);

        if (!VOID_TYPE_P(TREE_TYPE(fn_result))){
            LOG(TRACE, {"Not void return type"});
    log_with_timestamp(R"JSON({"event":"RETURN","function":"is_valid_destructor_fn","func_id":20,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":617,"ret_type":"bool","expr":"false"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_valid_destructor_fn","func_id":20})JSON");
            return false;
        }

        auto param_count = 0;
        auto fnp = DECL_ARGUMENTS(fn);

        for(; fnp; fnp = DECL_CHAIN(fnp)) param_count++;

        if (param_count != 1){
            LOG(TRACE, {"params != 1", param_count});
    log_with_timestamp(R"JSON({"event":"RETURN","function":"is_valid_destructor_fn","func_id":20,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":627,"ret_type":"bool","expr":"false"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_valid_destructor_fn","func_id":20})JSON");
            return false;
        }

        fnp = DECL_ARGUMENTS(fn);

        log_with_timestamp(R"JSON({"event":"RETURN","function":"is_valid_destructor_fn","func_id":20,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":640,"ret_type":"bool","expr":"is_tracked_struct(TREE_TYPE(fnp)) != nullptr"})JSON");
        log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_valid_destructor_fn","func_id":20})JSON");
        return is_tracked_struct(TREE_TYPE(fnp)) != nullptr;

    }

    ////////////////////////////////////////////////////////////
    static bool is_valid_default_ref_fn(const_tree fn){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"is_valid_default_ref_fn","func_id":19,"num_params":1,"start_line":637,"end_line":659,"metrics":{"num_params":1,"call_count":5,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"fn","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"is_valid_default_ref_fn","func_id":19,"name":"fn","type":"int","is_pointer":false,"is_const":false})JSON");
        if (TREE_CODE(fn) != FUNCTION_DECL) {
            log_with_timestamp(R"JSON({"event":"RETURN","function":"is_valid_default_ref_fn","func_id":19,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":639,"ret_type":"bool","expr":"false"})JSON");
            log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_valid_default_ref_fn","func_id":19})JSON");
            return false;
        }

        auto param_count = 0, valid_param = 0;

        for (auto fnp = DECL_ARGUMENTS(fn); fnp; fnp = DECL_CHAIN(fnp)) {
            param_count++;
            if (is_pointer_to(TREE_TYPE(fnp), void_type_node, TYPE_QUAL_CONST))
                valid_param++;
            else
                LOG(TRACE, {c_type_name(TREE_TYPE(fnp)), "!=", c_type_name(void_type_node)});

        }

        auto fn_result = TREE_TYPE(fn);

        if (!VOID_TYPE_P(TREE_TYPE(fn_result))){
    log_with_timestamp(R"JSON({"event":"RETURN","function":"is_valid_default_ref_fn","func_id":19,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":655,"ret_type":"bool","expr":"false"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_valid_default_ref_fn","func_id":19})JSON");
            return false;
        }

    log_with_timestamp(R"JSON({"event":"RETURN","function":"is_valid_default_ref_fn","func_id":19,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":658,"ret_type":"bool","expr":"param_count == 1 && valid_param == 1"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_valid_default_ref_fn","func_id":19})JSON");
        return param_count == 1 && valid_param == 1;
    }

    ////////////////////////////////////////////////////////////
    static bool is_valid_ref_fn(const_tree fn, const_tree target_type){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"is_valid_ref_fn","func_id":18,"num_params":2,"start_line":662,"end_line":684,"metrics":{"num_params":2,"call_count":5,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"fn","type":"int","is_pointer":false,"is_const":false},{"name":"target_type","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"is_valid_ref_fn","func_id":18,"name":"fn","type":"int","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"is_valid_ref_fn","func_id":18,"name":"target_type","type":"int","is_pointer":false,"is_const":false})JSON");
        if (TREE_CODE(fn) != FUNCTION_DECL) {
            log_with_timestamp(R"JSON({"event":"RETURN","function":"is_valid_ref_fn","func_id":18,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":664,"ret_type":"bool","expr":"false"})JSON");
            log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_valid_ref_fn","func_id":18})JSON");
            return false;
        }

        auto param_count = 0, valid_param = 0;

        for (auto fnp = DECL_ARGUMENTS(fn); fnp; fnp = DECL_CHAIN(fnp)) {
            param_count++;
            if (is_pointer_to(TREE_TYPE(fnp), target_type, TYPE_QUAL_CONST))
                valid_param++;
            else
                LOG(TRACE, {c_type_name(TREE_TYPE(fnp)), "!=", c_type_name(target_type)});

        }

        auto fn_result = TREE_TYPE(fn);

        if (!VOID_TYPE_P(TREE_TYPE(fn_result))){
    log_with_timestamp(R"JSON({"event":"RETURN","function":"is_valid_ref_fn","func_id":18,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":680,"ret_type":"bool","expr":"false"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_valid_ref_fn","func_id":18})JSON");
            return false;
        }

    log_with_timestamp(R"JSON({"event":"RETURN","function":"is_valid_ref_fn","func_id":18,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":683,"ret_type":"bool","expr":"param_count == 1 && valid_param == 1"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_valid_ref_fn","func_id":18})JSON");
        return param_count == 1 && valid_param == 1;
    }

    ////////////////////////////////////////////////////////////
    static void capture_given_fn(tree fun){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"capture_given_fn","func_id":17,"num_params":1,"start_line":687,"end_line":724,"metrics":{"num_params":1,"call_count":17,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"fun","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"capture_given_fn","func_id":17,"name":"fun","type":"int","is_pointer":false,"is_const":false})JSON");
        if (fun) {
            auto fun_name = get_name(fun);
            // TODO check function signature for alloc, free

            if (G.reftrack_alloc_fn_name == fun_name){
                G.reftrack_alloc_fn = fun;
                G.orig_alloc_fn = G.get_tree(G.orig_alloc_fn_name);
                LOG(DEBUG, {"Alloc functions:", get_name(G.reftrack_alloc_fn),
                        get_name(G.orig_alloc_fn)});
            }
            else if (G.reftrack_free_fn_name == fun_name){
                G.reftrack_free_fn = fun;
                G.orig_free_fn = G.get_tree(G.orig_free_fn_name);
                LOG(DEBUG, {"Free functions:", get_name(G.reftrack_free_fn),
                        get_name(G.orig_free_fn)});
            }
            else if (G.default_addref_fn_name == fun_name){
                if (is_valid_default_ref_fn(fun)){
                    G.default_addref_fn = fun;
                    LOG(DEBUG, {"Given default addref function:", fun_name});
                }
                else{
                    error("Invalid default addref function");
                }
            }
            else if (G.default_removeref_fn_name == fun_name){
                if (is_valid_default_ref_fn(fun)){
                    G.default_removeref_fn = fun;
                    LOG(DEBUG, {"Given default removeref function:", fun_name});
                }
                else{
                    error("Invalid default removeref function");
                }
            }

        }
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"capture_given_fn","func_id":17})JSON");
    }

    ////////////////////////////////////////////////////////////
    static void check_fn_attributes(tree fn){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"check_fn_attributes","func_id":16,"num_params":1,"start_line":727,"end_line":736,"metrics":{"num_params":1,"call_count":2,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"fn","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"check_fn_attributes","func_id":16,"name":"fn","type":"int","is_pointer":false,"is_const":false})JSON");
        long attr_value = 0;
        if (get_fn_attr_value(fn, attr_value) && (attr_value & REFTRACK_DESTRUCTOR_FN_FLAG)){
            if (!is_valid_destructor_fn(fn))
                error("Invalid destructor function");
            else{
                G.destructor_fns.insert(fn);
            }
        }
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"check_fn_attributes","func_id":16})JSON");
    }

    ////////////////////////////////////////////////////////////
    static void pre_genericize_fn_cb(void *gcc_data, void *user_data) {
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"pre_genericize_fn_cb","func_id":15,"num_params":2,"start_line":739,"end_line":747,"metrics":{"num_params":2,"call_count":2,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false},{"name":"user_data","type":"void *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"pre_genericize_fn_cb","func_id":15,"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"pre_genericize_fn_cb","func_id":15,"name":"user_data","type":"void *","is_pointer":true,"is_const":false})JSON");

        tree fun = static_cast<tree>(gcc_data);
        if (fun){
            capture_given_fn(fun);
            check_fn_attributes(fun);
        }

    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"pre_genericize_fn_cb","func_id":15})JSON");
    }

    ////////////////////////////////////////////////////////////
    static void gcc_finish_cb(void *gcc_data, void *user_data) {
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"gcc_finish_cb","func_id":14,"num_params":2,"start_line":750,"end_line":756,"metrics":{"num_params":2,"call_count":1,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false},{"name":"user_data","type":"void *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"gcc_finish_cb","func_id":14,"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"gcc_finish_cb","func_id":14,"name":"user_data","type":"void *","is_pointer":true,"is_const":false})JSON");

        for (const auto &e : G.fn_list) {
            LOG(TRACE, {e});
        }
        LOG(TRACE, { "#pass:", G.pass});
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"gcc_finish_cb","func_id":14})JSON");
    }

    ////////////////////////////////////////////////////////////
    static void collect_struct_with_rc(tree t){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"collect_struct_with_rc","func_id":13,"num_params":1,"start_line":759,"end_line":777,"metrics":{"num_params":1,"call_count":5,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"t","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"collect_struct_with_rc","func_id":13,"name":"t","type":"int","is_pointer":false,"is_const":false})JSON");

        if (!TREE_CODE(t)) {
            log_with_timestamp(R"JSON({"event":"RETURN","function":"collect_struct_with_rc","func_id":13,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":762,"ret_type":"void","expr":""})JSON");
            log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"collect_struct_with_rc","func_id":13})JSON");
            return;
        }

        if (G.debug){
            LOG(TRACE, {"struct:", type_name(t)});
        }

        for (auto field = TYPE_FIELDS(t); field; field = TREE_CHAIN(field)){

            if (G.refcount_fields.find(field) != G.refcount_fields.end()){

                if (G.debug){
                    LOG(DEBUG, { c_type_name(t), "->", symbol_name(field)});
                }
            }
        }
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"collect_struct_with_rc","func_id":13})JSON");
    }

    ////////////////////////////////////////////////////////////
    static void gcc_parse_type_cb(void *gcc_data, void *user_data) {
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"gcc_parse_type_cb","func_id":12,"num_params":2,"start_line":780,"end_line":804,"metrics":{"num_params":2,"call_count":6,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false},{"name":"user_data","type":"void *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"gcc_parse_type_cb","func_id":12,"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"gcc_parse_type_cb","func_id":12,"name":"user_data","type":"void *","is_pointer":true,"is_const":false})JSON");

        auto t = static_cast<tree>(gcc_data);

        if (!TREE_CODE(t)) {
            log_with_timestamp(R"JSON({"event":"RETURN","function":"gcc_parse_type_cb","func_id":12,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":785,"ret_type":"void","expr":""})JSON");
            log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"gcc_parse_type_cb","func_id":12})JSON");
            return;
        }

        collect_struct_with_rc(t);

        if (G.debug){
            LOG(DEBUG, { "Parse type :", c_type_name(t), " : {"});

            for (auto field = TYPE_FIELDS(t); field; field = TREE_CHAIN(field)){
                LOG(DEBUG, {symbol_name(field), ':', c_type_name(TREE_TYPE(field))," @["});
                for(auto fa = DECL_ATTRIBUTES(field); fa; fa = TREE_CHAIN(fa)){
                    LOG(DEBUG, {attr_name(fa), ' '});
                }
                LOG(DEBUG, {"] "});

            }

            LOG(DEBUG, {'}'});
        }

    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"gcc_parse_type_cb","func_id":12})JSON");
    }

    static void gcc_early_gimple_cb(void *gcc_data, void *user_data) {
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"gcc_early_gimple_cb","func_id":11,"num_params":2,"start_line":806,"end_line":816,"metrics":{"num_params":2,"call_count":6,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false},{"name":"user_data","type":"void *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"gcc_early_gimple_cb","func_id":11,"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"gcc_early_gimple_cb","func_id":11,"name":"user_data","type":"void *","is_pointer":true,"is_const":false})JSON");

        auto cp = current_pass;
        auto tp = (tree) (gcc_data);
        if (tp) {
            LOG(DEBUG, {"Tree type:",TREE_TYPE(tp),',', TREE_CODE(tp)});

        }
        LOG(DEBUG, {"Early gimple:", (cp ? cp->name : "<null>")});

    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"gcc_early_gimple_cb","func_id":11})JSON");
    }

    static void gcc_pass_cb(void *gcc_data, void *user_data) {
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"gcc_pass_cb","func_id":10,"num_params":2,"start_line":818,"end_line":828,"metrics":{"num_params":2,"call_count":2,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false},{"name":"user_data","type":"void *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"gcc_pass_cb","func_id":10,"name":"gcc_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"gcc_pass_cb","func_id":10,"name":"user_data","type":"void *","is_pointer":true,"is_const":false})JSON");

        auto p = current_pass;
        auto gp = (struct Globals *) (user_data);
        gp->pass++;

        if (p) {
            LOG(DEBUG, {"Pass:", p->name,", type:",  p->type});
        }

    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"gcc_pass_cb","func_id":10})JSON");
    }


    ////////////////////////////////////////////////////////////
    static tree function_param(tree fn, unsigned arg_index){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"function_param","func_id":9,"num_params":2,"start_line":832,"end_line":846,"metrics":{"num_params":2,"call_count":2,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"fn","type":"int","is_pointer":false,"is_const":false},{"name":"arg_index","type":"unsigned int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"function_param","func_id":9,"name":"fn","type":"int","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"function_param","func_id":9,"name":"arg_index","type":"unsigned int","is_pointer":false,"is_const":false})JSON");
        auto param = NULL_TREE;
        unsigned i = 0;

        if (TREE_CODE(fn) != FUNCTION_DECL) {
            log_with_timestamp(R"JSON({"event":"RETURN","function":"function_param","func_id":9,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":837,"ret_type":"int","expr":"param"})JSON");
            log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"function_param","func_id":9})JSON");
            return param;
        }

        for (auto fnp = DECL_ARGUMENTS(fn); fnp; fnp = DECL_CHAIN(fnp)){
            if (i++ == arg_index){
                param = fnp;
                break;
            }
        }
    log_with_timestamp(R"JSON({"event":"RETURN","function":"function_param","func_id":9,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":845,"ret_type":"int","expr":"param"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"function_param","func_id":9})JSON");
        return param;
    }

    ////////////////////////////////////////////////////////////
    static tree handle_refcount_attribute(tree *node, tree name, tree args,
                                          int flags, bool *no_add_attrs){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"handle_refcount_attribute","func_id":8,"num_params":5,"start_line":849,"end_line":941,"metrics":{"num_params":5,"call_count":17,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":true,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"node","type":"int *","is_pointer":true,"is_const":false},{"name":"name","type":"int","is_pointer":false,"is_const":false},{"name":"args","type":"int","is_pointer":false,"is_const":false},{"name":"flags","type":"int","is_pointer":false,"is_const":false},{"name":"no_add_attrs","type":"bool *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"handle_refcount_attribute","func_id":8,"name":"node","type":"int *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"handle_refcount_attribute","func_id":8,"name":"name","type":"int","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"handle_refcount_attribute","func_id":8,"name":"args","type":"int","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"handle_refcount_attribute","func_id":8,"name":"flags","type":"int","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"handle_refcount_attribute","func_id":8,"name":"no_add_attrs","type":"bool *","is_pointer":true,"is_const":false})JSON");

        const auto& tcode = TREE_CODE(*node);

        vector<tree> attr_args;
        for(; args; args = TREE_CHAIN(args)){
            attr_args.push_back(TREE_VALUE(args));
        }

        switch(tcode){
        case RECORD_TYPE:
        case UNION_TYPE:
        {
            if (attr_args.size() == 0){
                const auto& afn = G.get_default_addref_fn();
                const auto& rfn = G.get_default_removeref_fn();

                if (afn == nullptr || rfn == nullptr)
                    error("Default addref/removeref functions not defined");
                else {
                    G.ref_structs[*node] = {afn, rfn, build_pointer_type(*node)};
                    if (G.debug){
                        LOG(INFO, {"Tracking struct", c_type_name(*node), "using", function_name(afn), ",",
                                function_name(rfn)});
                    }
                }
            }
            else if (attr_args.size() == 2){

                const auto& fn1 = attr_args[0];
                const auto& fn2 = attr_args[1];

                if (!is_valid_ref_fn(fn1, *node)
                    || !is_valid_ref_fn(fn2, *node)){
                    error("Invalid attribute arguments");
                }
                else{

                    G.ref_structs[*node] = {fn1, fn2, build_pointer_type(*node)};

                    G.ignored_fns.insert(fn1);
                    G.ignored_fns.insert(fn2);

                    if (G.debug){

                        LOG(INFO, {"Tracking struct",
                                c_type_name(*node), "using",
                                function_name(fn1), ",",  function_name(fn2)});

                    }
                }
            }
            else{
                error("struct requires 0 or 2 attribute arguments");
            }

        }
        break;
        case FUNCTION_DECL:
        {

            if (attr_args.size() != 1 || TREE_CODE(attr_args[0]) != INTEGER_CST)
                error("function requires a single attribute argument");

            auto arg_value = int_cst_value(attr_args[0]);

            switch(arg_value){
                /*
                 * If attribute argument value is REFTRACK_IGNORE_FLAG, then the body of function and
                 * calls to the function are NOT tracked, _but_ the return value from the function
                 * is still tracked if the type of the return value is a tracked type
                 *
                 */
            case REFTRACK_IGNORE_FLAG:

                G.ignored_fns.insert(*node);
                LOG(WARN, {"Ignoring function:", get_name(*node)});
                break;

            default:
                break;
            }

        }
        break;
        default:
            error("attribute can be applied only to structs/functions");
            break;
        }

        log_with_timestamp(R"JSON({"event":"RETURN","function":"handle_refcount_attribute","func_id":8,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":946,"ret_type":"int","expr":"NULL_TREE"})JSON");
        log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"handle_refcount_attribute","func_id":8})JSON");
        return NULL_TREE;
    }

    static struct attribute_spec refcount_attr = {
        REF_ATTR_NAME, 0, 2, false, false, false, false, handle_refcount_attribute, NULL
    };

    static void register_attributes(void *event_data, void *data){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"register_attributes","func_id":7,"num_params":2,"start_line":947,"end_line":949,"metrics":{"num_params":2,"call_count":1,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"event_data","type":"void *","is_pointer":true,"is_const":false},{"name":"data","type":"void *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"register_attributes","func_id":7,"name":"event_data","type":"void *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"register_attributes","func_id":7,"name":"data","type":"void *","is_pointer":true,"is_const":false})JSON");
        register_attribute(&refcount_attr);
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"register_attributes","func_id":7})JSON");
    }

    ////////////////////////////////////////////////////////////
    static bool skip_function(tree fn){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"skip_function","func_id":6,"num_params":1,"start_line":952,"end_line":954,"metrics":{"num_params":1,"call_count":0,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"fn","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"skip_function","func_id":6,"name":"fn","type":"int","is_pointer":false,"is_const":false})JSON");
        log_with_timestamp(R"JSON({"event":"RETURN","function":"skip_function","func_id":6,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":955,"ret_type":"bool","expr":"is_ignored_function(fn)"})JSON");
        log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"skip_function","func_id":6})JSON");
        return is_ignored_function(fn);
    }
    ////////////////////////////////////////////////////////////
    static bool is_destructor(tree fn){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"is_destructor","func_id":5,"num_params":1,"start_line":956,"end_line":958,"metrics":{"num_params":1,"call_count":2,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"fn","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"is_destructor","func_id":5,"name":"fn","type":"int","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"RETURN","function":"is_destructor","func_id":5,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":957,"ret_type":"bool","expr":"G.destructor_fns.find(fn) != G.destructor_fns.cend()"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"is_destructor","func_id":5})JSON");
        return G.destructor_fns.find(fn) != G.destructor_fns.cend();
    }
    ////////////////////////////////////////////////////////////
    static void traverse_dfs_gimple(gimple_seq gs, std::function<bool(gimple_stmt_iterator&)> visitor){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"traverse_dfs_gimple","func_id":4,"num_params":2,"start_line":960,"end_line":983,"metrics":{"num_params":2,"call_count":11,"has_recursion":false,"has_loop":true,"has_if":false,"has_switch":true,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"gs","type":"int","is_pointer":false,"is_const":false},{"name":"visitor","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"traverse_dfs_gimple","func_id":4,"name":"gs","type":"int","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"traverse_dfs_gimple","func_id":4,"name":"visitor","type":"int","is_pointer":false,"is_const":false})JSON");
        for (gimple_stmt_iterator gsi = gsi_start(gs); !gsi_end_p(gsi); gsi_next(&gsi)){
            auto stmt = gsi_stmt(gsi);
            auto gcode = gimple_code(stmt);

            switch(gcode){
            case GIMPLE_BIND:{
                auto gbp = dyn_cast<gbind*>(stmt);
                traverse_dfs_gimple(gimple_bind_body(gbp), visitor);
            }
                break;
            case GIMPLE_TRY:{
                auto gtp = dyn_cast<gtry*>(stmt);
                traverse_dfs_gimple(gimple_try_eval(gtp), visitor);
                traverse_dfs_gimple(gimple_try_cleanup(gtp), visitor);
            }
                break;
            default:
                break;
            }
            // TODO check return value of visitor and abort traversal
            visitor(gsi);
        }
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"traverse_dfs_gimple","func_id":4})JSON");
    }

    ////////////////////////////////////////////////////////////
    inline static const char* gimple_type_str(gimple_seq gs){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"gimple_type_str","func_id":3,"num_params":1,"start_line":986,"end_line":988,"metrics":{"num_params":1,"call_count":0,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"gs","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"gimple_type_str","func_id":3,"name":"gs","type":"int","is_pointer":false,"is_const":false})JSON");
        log_with_timestamp(R"JSON({"event":"RETURN","function":"gimple_type_str","func_id":3,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":989,"ret_type":"const char *","expr":"gimple_code_name[gimple_code(gs)]"})JSON");
        log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"gimple_type_str","func_id":3})JSON");
        return gimple_code_name[gimple_code(gs)];
    }

    ////////////////////////////////////////////////////////////
    // Wrap the body of the gimple bind with a try finally block
    //
    ////////////////////////////////////////////////////////////
    gimple_seq gbind_with_cleanup(gbind *gbind_stmt, gimple_seq cleanup_seq){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"gbind_with_cleanup","func_id":2,"num_params":2,"start_line":994,"end_line":1001,"metrics":{"num_params":2,"call_count":2,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"gbind_stmt","type":"int *","is_pointer":true,"is_const":false},{"name":"cleanup_seq","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"gbind_with_cleanup","func_id":2,"name":"gbind_stmt","type":"int *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"gbind_with_cleanup","func_id":2,"name":"cleanup_seq","type":"int","is_pointer":false,"is_const":false})JSON");

        auto bind_body = gimple_bind_body(gbind_stmt);

        gimple_seq try_stmt = gimple_build_try(bind_body, cleanup_seq, GIMPLE_TRY_FINALLY);
        gimple_bind_set_body(gbind_stmt, try_stmt);
    log_with_timestamp(R"JSON({"event":"RETURN","function":"gbind_with_cleanup","func_id":2,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":1000,"ret_type":"int","expr":"gbind_stmt"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"gbind_with_cleanup","func_id":2})JSON");
        return gbind_stmt;
    }

    ////////////////////////////////////////////////////////////
    struct reftrack_pass : gimple_opt_pass {

        reftrack_pass(gcc::context *ctx):gimple_opt_pass(reftrack_data, ctx) {
        }

        int instrument_gseq(function *);

        int handle_gimple_bind(function *, gimple_stmt_iterator&, gimple_seq);
        int handle_gimple_call(function *, gimple_stmt_iterator&, gimple_seq);
        int handle_gimple_assign(function *, gimple_stmt_iterator&, gimple_seq);
        int handle_gimple_return(function *, gimple_stmt_iterator&, gimple_seq);
        int transform_assign(tree, tree, gimple_stmt_iterator&);

        virtual unsigned int execute(function *fn) override{
            instrument_gseq(fn);
            return 0;
        }

        virtual reftrack_pass *clone() override {
            return this;
        }

        // tracks function variables and their last assigned value in a BB
        unordered_map<tree, tree> bb_assigned;

        unordered_set<const_tree> param_set;

        // tracks variables that are initialized in any BB
        unordered_set<const_tree> initialized;

        function *cur_fn;

        void cleanup_fn(){
            bb_assigned.clear();
            param_set.clear();
        }

        void init(function *fn){
            cleanup_fn();

            cur_fn = fn;

            for (auto param = DECL_ARGUMENTS(fn->decl); param; param = DECL_CHAIN(param)) {

                if (is_tracked_struct(TREE_TYPE(param))){
                    param_set.insert(param);
                    initialized.insert(param);
                }
            }

        }

        void bb_cleanup(basic_block bb){
            // empty
        }

        void bb_start_handler(basic_block bb){
            bb_cleanup(bb);
            // clear all assignments made in the BB.
            bb_assigned.clear();

        }

        void bb_end_handler(basic_block bb){
            // empty
        }

        bool is_param(const_tree p){ return param_set.find(p) != param_set.cend();}

        // Returns true if the given variable/field is local, or reachable from
        // a local variable or one of the parameters.

        bool is_local_var(tree var){
            bool rv = false;
            auto tcode = TREE_CODE(var);

            if (tcode == ARRAY_REF){
                return is_local_var(get_base_address(var));
            }

            if (DECL_CONTEXT(var) == cur_fn->decl){

                rv = true;
            }
            return rv;

        }

        bool is_initialized(const_tree var){
            return initialized.find(var) != initialized.cend();
        }

        // TODO handle variable of nested functions & extern/static global variables

        bool is_assigned(tree var){

            return bb_assigned.find(var) != bb_assigned.cend()
                || param_set.find(var) != param_set.cend()
                || is_initialized(var);
        }

        tree last_assigned_value(tree var){

            tree val = nullptr;

            /*
             * we consider parameters to be assigned as they are bound to arguments during
             * function invocation even though we won't know whether the argument is null or not.
             */
            if (param_set.find(var) != param_set.cend()
                && bb_assigned.find(var) == bb_assigned.cend()){
                val = var;
            }
            else {
                val = bb_assigned[var];
            }
            return val;
        }

        bool emit_removeref_p(tree var){
            bool rv = is_static(var);

            if (!rv)
                rv = !is_local_var(var);

            if (!rv && is_assigned(var)){
                if (last_assigned_value(var))
                    rv = !null_value(last_assigned_value(var));
                else
                    rv = is_initialized(var);
            }

            return rv;
        }

        void add_assignment(tree var, tree rhs){

            LOG(TRACE, {symbol_name(var),"=", symbol_name(rhs)});
            bb_assigned[var] = rhs;
            initialized.insert(var);
        }

        ////////////////////////////////////////////////////////////
        gimple_seq gen_local_var_cleanup(function *fn, location_t loc){
            gimple_seq rv = nullptr;
            vector<gimple_build_info> cleanup_list;

            for(auto it = bb_assigned.cbegin(); it != bb_assigned.cend(); ++it){
                const auto& var = it->first;

                if (is_static(var) || is_generated(var)){
                    continue;
                }

                const auto& refinfo = is_tracked_struct(TREE_TYPE(var));
                if (refinfo){

                    // TODO if last assignment was null, skip it
                    cleanup_list.push_back({GIMPLE_CALL, loc, refinfo->ref_remove_fn, var});
                }
            }

            if (cleanup_list.size()){
                    rv = build_refcall_block(cleanup_list);
            }

            return rv;

        }

    };


    ////////////////////////////////////////////////////////////
    int reftrack_pass::handle_gimple_bind(function *fn, gimple_stmt_iterator& gsi, gimple_seq stmt){

        LOG(TRACE, {"Gimple bind"});
        auto vars  = gimple_bind_vars(dyn_cast<gbind*>(stmt));

        for(; vars; vars = TREE_CHAIN(vars)){
            LOG(TRACE, {gimple_loc_str(stmt),"Symbol:[", symbol_name(vars),"] ", c_type_name(TREE_TYPE(vars))});
        }

        return 0;
    }
    ////////////////////////////////////////////////////////////
    int reftrack_pass::handle_gimple_return(function *fn, gimple_stmt_iterator& gsi, gimple_seq stmt){

        const auto& gseq = gen_local_var_cleanup(fn, gimple_location(stmt));
        if (gseq)
            gsi_insert_seq_before(&gsi, gseq, GSI_SAME_STMT);

        return 0;
    }

    /* Transforms the GIMPLE assign of form A=B+C+D where atmost 2 are pointers to the
     * the same tracked type.
     * The parameter gsi should be pointing to the GIMPLE assign
     * A=B+C+D
     * is transformed into
     *
     * T=B+C+D
     * addref(T)
     * removeref(A)
     * A=T
     *
     */
    ////////////////////////////////////////////////////////////
    int reftrack_pass::transform_assign(tree addref_fn, tree removeref_fn,
                                        gimple_stmt_iterator& gsi){
        vector<gimple_build_info> tracked_assign;
        gimple_seq stmt = gsi_stmt(gsi);
        tree lhs = gimple_assign_lhs(stmt);
        tree rhs1 = gimple_assign_rhs1(stmt);
        tree rhs2 = gimple_assign_rhs2(stmt);
        tree rhs3 = gimple_assign_rhs3(stmt);
        auto lineno = gimple_location(stmt);
        auto lhs_refinfo = is_tracked_struct(TREE_TYPE(lhs));

        if (EXPR_P(lhs)){
            auto old_lhs = lhs;
            lhs = force_gimple_operand_gsi(&gsi, lhs, true, nullptr, true, GSI_SAME_STMT);
            LOG(TRACE, {"Unfold LHS", gimple_loc_str(stmt), symbol_name(old_lhs), symbol_name(lhs)});
        }


        if (!null_value(rhs1) && (rhs2 || rhs3)){
            auto tmp_var = create_tmp_var(lhs_refinfo->pointer_type, REFTRACK_TMP_PREFIX);

            gimple_assign_set_lhs(stmt, tmp_var); // T=B+C+D
            update_stmt(stmt);
            if (addref_fn)
                tracked_assign.push_back({GIMPLE_CALL, lineno, addref_fn, tmp_var, false,
                        lhs_refinfo->pointer_type}); // addref(T)

            if (removeref_fn){ // removeref(A)
                tracked_assign.push_back({GIMPLE_CALL, lineno, removeref_fn, lhs, false,
                    lhs_refinfo->pointer_type});
            }
            auto new_stmts = build_refcall_block(tracked_assign, gimple_block(stmt)); // {addref(T), removeref(A)*}
            auto new_assign = gimple_build_assign(lhs, tmp_var); // A=T
            gimple_seq_add_stmt(&new_stmts, new_assign);
            gsi_insert_seq_after(&gsi, new_stmts, GSI_CONTINUE_LINKING);
            LOG(TRACE, {"Transform A=B+C+D for", gimple_loc_str(stmt), symbol_name(lhs)});
        }
        else{

            if (!null_value(rhs1) && addref_fn)
                tracked_assign.push_back({GIMPLE_CALL, lineno, addref_fn, rhs1, EXPR_P(rhs1),
                        lhs_refinfo->pointer_type});

            if (removeref_fn){
                tracked_assign.push_back({GIMPLE_CALL, lineno, removeref_fn, lhs, false,
                        lhs_refinfo->pointer_type});
            }
            auto new_stmts = build_refcall_block(tracked_assign, gimple_block(stmt));
            gsi_insert_seq_before(&gsi, new_stmts, GSI_SAME_STMT);

        }
        return 1;
    }
    ////////////////////////////////////////////////////////////
    int reftrack_pass::handle_gimple_assign(function *fn, gimple_stmt_iterator& gsi, gimple_seq stmt){

        auto loc = gimple_loc_str(stmt);
        tree rhs1 = gimple_assign_rhs1(stmt);
        tree rhs2 = gimple_assign_rhs2(stmt);
        tree rhs3 = gimple_assign_rhs3(stmt);
        tree lhs = gimple_assign_lhs(stmt);

        if (lhs == rhs1){
            return 0;
        }

        auto lhs_arginfo = is_tracked_struct(TREE_TYPE(lhs));

        if (!lhs_arginfo) {
            LOG(TRACE, {loc, "Skip", symbol_info(lhs)});
            return 0;
        }

        LOG(TRACE, {loc, "LHS:", symbol_info(lhs)});

        if (is_generated(lhs)
            && !is_component(lhs)
            && !EXPR_P(lhs)){ // case A.2/A.3
            LOG(TRACE, {loc, "A.2/A.3", symbol_info(lhs)});
            return 0;
        }

        if (is_array_elem(lhs)){
            LOG(INFO, {loc, "array types are not supported"});
            return 0;
        }

        vector<gimple_build_info> tracked_assign;

        tree rhs = is_tracked_struct(TREE_TYPE(rhs1)) ?
            rhs1 : (rhs2 && is_tracked_struct(TREE_TYPE(rhs2))
                    ? rhs2 : rhs3 && is_tracked_struct(TREE_TYPE(rhs3)) ? rhs3 : rhs1 );

        tree addref_fn = nullptr, removeref_fn = nullptr;
        if (!null_value(rhs)){
            auto rhs_arginfo = is_tracked_struct(TREE_TYPE(rhs));

            LOG(TRACE, {loc, "RHS:", "Code:", tree_code_str(gimple_assign_rhs_code(stmt)),
                    symbol_info(rhs), symbol_info(rhs2), symbol_info(rhs3)});

            if (rhs_arginfo){
                addref_fn = rhs_arginfo->ref_add_fn;
            }
            else if (!rhs_arginfo){ // assignment from typecast
                addref_fn = lhs_arginfo->ref_add_fn;

            }
        }

        if (emit_removeref_p(lhs)){
            removeref_fn = lhs_arginfo->ref_remove_fn;
        }

        add_assignment(lhs, rhs);

        if (addref_fn || removeref_fn){
            transform_assign(addref_fn, removeref_fn, gsi);
        }
        return 1;
    }

    ////////////////////////////////////////////////////////////
    int reftrack_pass::handle_gimple_call(function *fn, gimple_stmt_iterator& gsi, gimple_seq stmt){
        auto loc_str = gimple_loc_str(stmt);
        auto callee = gimple_call_fndecl(stmt);

        if (!callee){
            LOG(TRACE, {"Callee is null", function_name(fn), ":", gimple_loc_str(stmt)});
            return 1;
        }

        location_t loc = gimple_location(stmt);
        LOG(TRACE, {"call_fn:", callee, symbol_info(callee)});

        vector<gimple_build_info> tracked_args, call_lhs_epilog_args;

        auto arg_count = gimple_call_num_args(stmt);

        gcall *call = dyn_cast<gcall*>(stmt);

        auto call_lhs = gimple_call_lhs(call);
        const ref_info *lhs_arginfo =  call_lhs ? is_tracked_struct(TREE_TYPE(call_lhs)) : nullptr;


        if (!skip_function(callee) && !is_destructor(callee)){
            LOG(TRACE, {symbol_name(callee),
                    "#arg:",
                    (size_t)(arg_count)});

            for(unsigned ai = 0; ai < arg_count; ai++) {

                auto arg = gimple_call_arg(stmt, ai);

                auto arg_refinfo = is_tracked_struct(TREE_TYPE(arg));
                auto fn_param = function_param(callee, ai);
                if (arg_refinfo && fn_param){

                    auto param_refinfo = is_tracked_struct(
                        TREE_TYPE(fn_param));

                    // we don't skip even if arg is generated as the callee
                    // would decrement the refcount always.

                    if (arg_refinfo == param_refinfo)
                        tracked_args.push_back({GIMPLE_CALL, loc, arg_refinfo->ref_add_fn, arg, EXPR_P(arg),
                            arg_refinfo->pointer_type});
                }

            }

        }


        LOG(TRACE, {loc_str, symbol_info(call_lhs)});

        if (call_lhs && is_heap_function(callee) && is_call_arg(call_lhs, call))
            return 0;

        if (lhs_arginfo &&
            (!is_generated(call_lhs)
             || (is_generated(call_lhs) && is_component(call_lhs) ))){

            LOG(TRACE, {"CALL LHS:", symbol_info(call_lhs)});

            if (emit_removeref_p(call_lhs)){
                tracked_args.push_back({GIMPLE_CALL, loc,lhs_arginfo->ref_remove_fn, call_lhs,
                        EXPR_P(call_lhs), lhs_arginfo->pointer_type});
            }
            call_lhs_epilog_args.push_back({GIMPLE_CALL, loc, lhs_arginfo->ref_add_fn, call_lhs,
                    EXPR_P(call_lhs), lhs_arginfo->pointer_type});

        }

        if (tracked_args.size()){
            auto gseq = build_refcall_block(tracked_args, gimple_block(stmt));
                gsi_insert_seq_before(&gsi, gseq, GSI_SAME_STMT);
        }
        if (call_lhs_epilog_args.size()){
            auto gseq = build_refcall_block(call_lhs_epilog_args, gimple_block(stmt));
            gsi_insert_seq_after(&gsi, gseq, GSI_NEW_STMT);
        }

        // key value doesn't matter as we track only
        // assignments of non-null values
        if (call_lhs){
            add_assignment(call_lhs, call_lhs);
        }

        return 1;
    }

    ////////////////////////////////////////////////////////////
    int reftrack_pass::instrument_gseq(function * fn)  {

        if (skip_function(fn->decl))
            return 0;

        init(fn);

        if (G.debug){
            LOG(TRACE,{horiz_line,"\ninstrument_gseq():", function_name(fn->decl), "\n", horiz_line},"");
        }

        basic_block bb;

        gimple_stmt_iterator gsi;
        try{
            FOR_EACH_BB_FN(bb, fn){
                bb_start_handler(bb);
                LOG(TRACE, {"BLOCK START"});

                for (gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {

                    auto stmt = gsi_stmt(gsi);

                    if (gimple_clobber_p(stmt) || is_gimple_debug(stmt))
                        continue;

                    auto gcode = gimple_code(stmt);
                    LOG(TRACE, {"GCODE:", gcode, gimple_type_str(stmt)});
                    switch (gcode) {
                    case GIMPLE_BIND:

                        handle_gimple_bind(fn, gsi, stmt);
                        break;
                    case GIMPLE_ASSIGN:

                        handle_gimple_assign(fn, gsi, stmt);
                        break;
                    case GIMPLE_CALL:

                        // TODO handle return value of types X*

                        /* TODO gimple call might need to be processed first in order to
                         *  handle calls to identify functions like
                         *  T *id(T *x) { return x;}
                         *  T *p = T_create();
                         *  p = id(p);
                         */
                        handle_gimple_call(fn, gsi, stmt);
                        break;
                    case GIMPLE_RETURN:
                        //handle_gimple_return(fn, gsi, stmt);
                        break;
                    default:
                        break;
                    }

                }
                LOG(TRACE, {"BLOCK END"});
                bb_end_handler(bb);
            }
        }
        catch(std::exception& e){
            auto msg = e.what();
            LOG(ERROR, { "Exception:", (msg ? msg : "unknown")});

        }
        LOG(TRACE, {horiz_line});
        return 0;
    }

    ////////////////////////////////////////////////////////////
    struct reftrack_cleanup : gimple_opt_pass {

        reftrack_cleanup(gcc::context *ctx):gimple_opt_pass(reftrack_cleanup_data, ctx) {}

        unsigned int add_block_cleanup(function *);
        unsigned int add_arg_cleanup(function *);
        unsigned int replace_mem_fun(function *);
        unsigned int add_gimple_cleanup(gbind *bind_stmt, gimple_seq cleanup_seq);

        virtual unsigned int execute(function *fn) override {
            if (G.replace_memfn)
                replace_mem_fun(fn);

            add_arg_cleanup(fn);
            add_block_cleanup(fn);

            return 0;
        }

        virtual reftrack_cleanup *clone() override {
            return this;
        }
    };

    /**
     * Generates cleanup instructions for the given variable. Recursively descends into the type and
     * generates instructions for any tracked fields.
     * Returns the number of the fields that are tracked.
     */
    int collect_tracked_fields(tree var, vector<gimple_build_info>& tracked_list){
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"collect_tracked_fields","func_id":1,"num_params":2,"start_line":1524,"end_line":1545,"metrics":{"num_params":2,"call_count":4,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"var","type":"int","is_pointer":false,"is_const":false},{"name":"tracked_list","type":"int &","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"collect_tracked_fields","func_id":1,"name":"var","type":"int","is_pointer":false,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"collect_tracked_fields","func_id":1,"name":"tracked_list","type":"int &","is_pointer":false,"is_const":false})JSON");
        auto var_type = TREE_TYPE(var);
        auto refinfo = is_tracked_struct(var_type);
        int count = 0;

        if (refinfo){

            tracked_list.push_back({GIMPLE_CALL, 0, refinfo->ref_remove_fn, var, false});
            count++;
        }
        else {

            for(auto field = TYPE_FIELDS(var_type); field; field = TREE_CHAIN(field)){
                if (RECORD_OR_UNION_TYPE_P(var_type)){
                    auto mem_path = build3(COMPONENT_REF, TREE_TYPE(field), var, field, NULL_TREE);
                    count+=collect_tracked_fields(mem_path, tracked_list);
                }
            }
        }

    log_with_timestamp(R"JSON({"event":"RETURN","function":"collect_tracked_fields","func_id":1,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":1544,"ret_type":"int","expr":"count"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"collect_tracked_fields","func_id":1})JSON");
        return count;
    }

    ////////////////////////////////////////////////////////////
    unsigned int reftrack_cleanup::replace_mem_fun(function * fn){

        if (skip_function(fn->decl))
            return 0;

        traverse_dfs_gimple(fn->gimple_body, [&](auto& gsi)->bool{

            auto stmt = gsi_stmt(gsi);

            if (gimple_code(stmt) != GIMPLE_CALL)
                return false;

            auto call = dyn_cast<gcall*>(stmt);
            auto arg_count = gimple_call_num_args(call);
            auto callee = gimple_call_fndecl(stmt);
            auto call_lhs = gimple_call_lhs(call);

            const ref_info *lhs_arginfo =  call_lhs ? is_tracked_struct(TREE_TYPE(call_lhs)) : nullptr;

            /*
             * Replace original alloc function with reftrack equivalent
             * ONLY IF lhs is a tracked type.
             */
            if (lhs_arginfo){
                tree new_callee = nullptr;

                if (callee == G.get_orig_alloc_fn()){
                    new_callee = G.get_reftrack_alloc_fn();
                    LOG(TRACE, {gimple_loc_str(stmt), "replacing allocator"});

                    if (new_callee){
                        gimple_call_set_fndecl(call, new_callee);
                        update_stmt(call);
                        LOG(TRACE, {gimple_loc_str(stmt), "allocator replaced", get_name(new_callee)});
                    }
                    else{
                        LOG(ERROR, {gimple_loc_str(stmt),
                                "Replace flag given, but allocator function missing"});
                    }
                }

            }

            /*
             * Replace original free function with reftrack equivalent.
             */
            if (callee == G.get_orig_free_fn()){

                for(unsigned ai = 0; ai < arg_count; ai++){
                    auto arg = gimple_call_arg(stmt, ai);
                    if (is_tracked_struct(TREE_TYPE(arg))){
                        // Assumption: at least one arg is of tracked type.
                        auto new_callee = G.get_reftrack_free_fn();

                        if (new_callee){
                            gimple_call_set_fndecl(call, new_callee);
                            update_stmt(call);
                        }
                        else{
                            LOG(ERROR, {gimple_loc_str(stmt),
                                    "Replace flag given, but free function missing"});
                        }
                        break;
                    }
                }
            }
            return false;

        });

        return 0;

    }
    ////////////////////////////////////////////////////////////
    unsigned int reftrack_cleanup::add_gimple_cleanup(gbind *bind_stmt,
                                                      gimple_seq cleanup_seq){
        gimple_seq bind_body = gimple_bind_body(bind_stmt);
        auto try_stmt = bind_body;

        if (is_a<gtry*>(try_stmt)
            &&  gimple_try_kind(try_stmt) == GIMPLE_TRY_FINALLY){
            auto try_cleanup = gimple_try_cleanup(try_stmt);
            auto gsi = gsi_last(cleanup_seq);
            gsi_insert_seq_after(&gsi, try_cleanup, GSI_NEW_STMT);
            gimple_try_set_cleanup(dyn_cast<gtry*>(try_stmt), cleanup_seq);
            LOG(TRACE, {"Reusing try block at ", gimple_loc_str(bind_stmt) });
        }
        else{
            gbind_with_cleanup(bind_stmt, cleanup_seq);
        }

        return 0;

    }
    ////////////////////////////////////////////////////////////
    unsigned int reftrack_cleanup::add_block_cleanup(function *fn){

        if (skip_function(fn->decl))
            return 0;

        traverse_dfs_gimple(fn->gimple_body, [&](auto& gsi)->bool{

            auto stmt = gsi_stmt(gsi);
            auto gcode = gimple_code(stmt);
            if (gcode != GIMPLE_BIND)
                return false;

            LOG(TRACE,{horiz_line, "\ncleanup:", function_name(fn->decl),
                    "\n",horiz_line}, "");

            vector<gimple_build_info> tracked_vars;
            auto bind_stmt = dyn_cast<gbind*>(stmt);

            for (auto block_var = gimple_bind_vars(bind_stmt); block_var;
                 block_var = TREE_CHAIN(block_var)) {

                LOG(TRACE, {"Block var:", symbol_info(block_var)});

                if (is_generated(block_var) || TREE_STATIC(block_var) )
                    continue;

                if (is_array_of_tracked_struct(TREE_TYPE(block_var))){
                    LOG(TRACE, {"Array type unsupported:var:", symbol_name(block_var)});
                    continue;
                }

                int count = collect_tracked_fields(block_var, tracked_vars);

                LOG(TRACE, {"var traverse:",count});

            }

            if (tracked_vars.size()){

                LOG(TRACE, {"\nAdded", tracked_vars.size(), "cleanup statement(s)"});
                add_gimple_cleanup(bind_stmt, build_refcall_block(tracked_vars));
            }

            LOG(TRACE,{horiz_line});

            return false;
        });


        return 0;

    }
    ////////////////////////////////////////////////////////////
    unsigned int reftrack_cleanup::add_arg_cleanup(function *fn) {

        if (skip_function(fn->decl) || is_destructor(fn->decl))
            return 0;

        vector<gimple_build_info> tracked_args;

        for (auto param = DECL_ARGUMENTS(fn->decl); param; param = DECL_CHAIN(param)){
            auto refinfo = is_tracked_struct(TREE_TYPE(param));
            if (refinfo){
                tracked_args.push_back({GIMPLE_CALL, UNKNOWN_LOCATION, refinfo->ref_remove_fn, param, false});
            }
        }
        if (tracked_args.size()){
            auto top_bind_stmt = gimple_build_bind(NULL_TREE, fn->gimple_body, NULL_TREE);
            gbind_with_cleanup(top_bind_stmt, build_refcall_block(tracked_args));
            fn->gimple_body = top_bind_stmt;
        }

        return 0;
    }

} // END NAMESPACE reftrack

int plugin_init(struct plugin_name_args *plugin_info,
                struct plugin_gcc_version *version)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","function":"plugin_init","func_id":0,"num_params":2,"start_line":1720,"end_line":1824,"metrics":{"num_params":2,"call_count":25,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"plugin_info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false},{"name":"version","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"plugin_info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"version","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false})JSON");

    using namespace reftrack;

    int gcc_basever[] = { 12, 3 };

    int given_ver_comp[] = {0, 0};

    string given_ver = version->basever;

    auto first_pos = given_ver.find('.');
    if (first_pos != given_ver.npos){

        given_ver_comp[0] = atoi(given_ver.substr(0, first_pos).c_str());
        auto second_pos = given_ver.find('.', first_pos+1);
        if (second_pos != given_ver.npos)
            given_ver_comp[1] = atoi(given_ver.substr(first_pos+1, second_pos-first_pos).c_str());
        LOG(INFO, {"Given version:", xstring(given_ver_comp[0])+"."+ xstring(given_ver_comp[1])});
    }

    if ((given_ver_comp[0]*10+given_ver_comp[1]) < (gcc_basever[0]*10+gcc_basever[1])) {
        LOG(ERROR, {PLUGIN_NAME, ":version mismatch", "expected: >=",
                xstring(gcc_basever[0])+ "."+ xstring(gcc_basever[1]),
                ",received:", version->basever});
    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":1746,"ret_type":"int","expr":"1"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
        return 1;
    }

    if (!lang_GNU_C()){
        LOG(ERROR, {"Unsupported source language:", lang_hooks.name});
    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":1751,"ret_type":"int","expr":"1"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
        return 1;
    }

    for(int i = 0; i < plugin_info->argc; i++){
        string arg_key = plugin_info->argv[i].key;

        if (arg_key == "orig_alloc" ){
            G.orig_alloc_fn_name = plugin_info->argv[i].value;
            LOG(TRACE, {"Given original alloc function:", G.orig_alloc_fn_name});
        }
        else if (arg_key == "alloc" ){
            G.reftrack_alloc_fn_name = plugin_info->argv[i].value;
            LOG(TRACE, {"Given alloc function:", G.reftrack_alloc_fn_name});
        }
        else if (arg_key == "orig_free"){
            G.orig_free_fn_name = plugin_info->argv[i].value;
            LOG(TRACE, {"Given original free function:", G.orig_free_fn_name});
        }
        else if (arg_key == "free"){
            G.reftrack_free_fn_name = plugin_info->argv[i].value;
            LOG(TRACE, {"Given free function:", G.reftrack_free_fn_name});
        }
        else if (arg_key == "addref"){
            G.default_addref_fn_name = plugin_info->argv[i].value;
            LOG(TRACE, {"Given default addref function:", G.default_addref_fn_name});
        }
        else if (arg_key == "removeref"){
            G.default_removeref_fn_name = plugin_info->argv[i].value;
            LOG(TRACE, {"Given default removeref function:", G.default_removeref_fn_name});
        }
        else if (arg_key == "log_level"){
            G.cur_log_level = log_level(atoi(plugin_info->argv[i].value));
        }
        else if (arg_key == "replace"){
            G.replace_memfn = true;
        }
        else{
            error("Unknown plugin argument:%s", arg_key.c_str());
        }
    }

    register_callback(PLUGIN_NAME, PLUGIN_INFO, nullptr, &G.pinfo);
    register_callback(PLUGIN_NAME, PLUGIN_ATTRIBUTES, reftrack::register_attributes, NULL);


    register_callback(PLUGIN_NAME, PLUGIN_PRE_GENERICIZE,
                      pre_genericize_fn_cb, &G);

    register_callback(PLUGIN_NAME, PLUGIN_PASS_EXECUTION, gcc_pass_cb, &G);

    struct register_pass_info rpi = {
        .pass = new reftrack_pass(g),
        .reference_pass_name = "cfg",
        .ref_pass_instance_number = 1,
        .pos_op = PASS_POS_INSERT_AFTER
    };

    register_callback(PLUGIN_NAME, PLUGIN_PASS_MANAGER_SETUP, NULL,
      &rpi);

    struct register_pass_info cleanup_rpi = {
        .pass = new reftrack_cleanup(g),
        .reference_pass_name = "omplower",
        .ref_pass_instance_number = 1,
        .pos_op = PASS_POS_INSERT_AFTER
    };

    register_callback(PLUGIN_NAME, PLUGIN_PASS_MANAGER_SETUP, NULL,
                      &cleanup_rpi);

    LOG(INFO, {PLUGIN_NAME, "plugin initialized"});

    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/reftrack-plugin/reftrack-plugin-main-instrumented/reftrack.cc","line":1823,"ret_type":"int","expr":"0"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
    return 0;
}
