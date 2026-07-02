#include <ctime>
#include <fstream>
#include <iostream>

#include "gcc-plugin.h"
#include "coretypes.h"
#include "tree-core.h"
#include "tree.h"
#include "context.h"
#include "tree-pass.h"
#include "plugin-version.h"
#include "gimple-pretty-print.h" // for debug_gimple_stmt
#include "tree-pretty-print.h"   // for debug_generic_stmt
#include "print-tree.h"          // for debug_tree
#include "tree-cfg.h"            // for debug_function... seriously?!
#include "diagnostic.h"
#include "gimple.h"
#include "gimple-iterator.h"
#include "tree-ssa-operands.h"
#include "tree-ssa-propagate.h"
#include "stringpool.h"          // for gimple-ssa.h below
#include "gimple-ssa.h"          // for tree-ssanames.h below
#if GCC_VERSION >= 7000
#include "tree-vrp.h"
#endif
#include "tree-ssanames.h"       // for num_ssa_names
#include "tree-ssa-propagate.h"
#include "attribs.h"
static void log_with_timestamp(const char *msg);

static void log_with_timestamp(const char *msg) {
    time_t t = time(NULL);
    tm *tm_info = localtime(&t);
    char time_buf[26];
    strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    std::ofstream log_file("/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/trace_funcp-encrypt.cc.log", std::ios::app);
    log_file << "{\"ts\":\"" << time_buf
             << "\",\"file\":\"funcp-encrypt.cc\",\"msg\":" << msg
             << "}" << std::endl;
}

using namespace std;

extern "C" void *global = nullptr;
extern "C" void *global2 = nullptr;
extern "C" void *global3 = nullptr;

int plugin_is_GPL_compatible;

const pass_data funcp_pass_data = {
  GIMPLE_PASS,
  "funcp_plugin",
  OPTGROUP_NONE,
  TV_NONE,
  PROP_cfg, // properties_required
  0,        // properties_provided
  0,        // properties_destroyed
  0,        // properties_start
  0,        // properties_finish
};

class funcp_pass : public gimple_opt_pass
{
public:
  funcp_pass (gcc::context *ctxt)
    : gimple_opt_pass(funcp_pass_data, ctxt)
  {}
  virtual bool gate (function *) { return true; }
  virtual unsigned int execute (function *);
};

// A bitmap expressing which SSA variables are known to have been
// decrypted.
bitmap decrypted_ssa_bitmap;

// A lattice indexed by SSA name version to keep track of how SSA
// variables change throughout the propagator.  The values are -1 for
// uninitialized, otherwise they are the same as ssa_prop_result.
//
// We use this to recognize when subsequent analysis of the same SSA
// name yields nothing different.
static vec<int> lattice_values;

// Record the fact that SSA_VAR is known to be decrypted.

static void
mark_decrypted (tree ssa_var)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","function":"mark_decrypted","func_id":10,"num_params":1,"start_line":72,"end_line":77,"metrics":{"num_params":1,"call_count":2,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"ssa_var","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"mark_decrypted","func_id":10,"name":"ssa_var","type":"int","is_pointer":false,"is_const":false})JSON");
  gcc_assert (TREE_CODE (ssa_var) == SSA_NAME);
  bitmap_set_bit (decrypted_ssa_bitmap, SSA_NAME_VERSION (ssa_var));
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"mark_decrypted","func_id":10})JSON");
}

// Return TRUE if SSA_VAR is known to be decrypted.  Return FALSE if
// SSA_VAR is either not decrypted, or if it is not an SSA_VAR at all.

static bool
decrypted_p (tree ssa_var)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","function":"decrypted_p","func_id":9,"num_params":1,"start_line":82,"end_line":88,"metrics":{"num_params":1,"call_count":2,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"ssa_var","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"decrypted_p","func_id":9,"name":"ssa_var","type":"int","is_pointer":false,"is_const":false})JSON");
  if (TREE_CODE (ssa_var) == SSA_NAME) {
      log_with_timestamp(R"JSON({"event":"RETURN","function":"decrypted_p","func_id":9,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","line":86,"ret_type":"bool","expr":"bitmap_bit_p (decrypted_ssa_bitmap, SSA_NAME_VERSION (ssa_var))"})JSON");
      log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"decrypted_p","func_id":9})JSON");
      return bitmap_bit_p (decrypted_ssa_bitmap, SSA_NAME_VERSION (ssa_var));
  }
    log_with_timestamp(R"JSON({"event":"RETURN","function":"decrypted_p","func_id":9,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","line":87,"ret_type":"bool","expr":"false"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"decrypted_p","func_id":9})JSON");
  return false;
}

// Initialize the value propagation engine for function FUN.

static void
prop_init (function *fun)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","function":"prop_init","func_id":8,"num_params":1,"start_line":92,"end_line":132,"metrics":{"num_params":1,"call_count":13,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"fun","type":"int *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"prop_init","func_id":8,"name":"fun","type":"int *","is_pointer":true,"is_const":false})JSON");
  // Initialize lattice values to UNDEFINED.
  lattice_values.create (num_ssa_names);
  lattice_values.safe_grow_cleared (num_ssa_names);
  for (unsigned int i = 0; i < num_ssa_names; ++i)
    lattice_values[i] = -1;

  decrypted_ssa_bitmap = BITMAP_ALLOC (NULL);

  // Make a quick pass through the instructions and mark what's really
  // interesting to this pass.  The propagation engine will ignore
  // stmts/phis we mark with prop_set_simulate_again (x, false).
  basic_block bb;
  FOR_EACH_BB_FN (bb, fun)
    {
      for (gphi_iterator si = gsi_start_phis (bb); !gsi_end_p (si);
           gsi_next (&si))
        {
          gphi *phi = si.phi ();
          // All PHIs are interesting for now.
          prop_set_simulate_again (phi, true);
        }
      for (gimple_stmt_iterator si = gsi_start_bb (bb); !gsi_end_p (si);
           gsi_next (&si))
        {
          gimple *stmt = gsi_stmt (si);

          // If the statement is a control insn, then we do not want
          // to avoid simulating the statement once.  Failure to do so
          // means that those edges will never get added.
          if (stmt_ends_bb_p (stmt))
            prop_set_simulate_again (stmt, true);
          else
            prop_set_simulate_again (stmt,
                                     is_gimple_assign (stmt)
                                     || is_gimple_call (stmt));
        }
    }
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"prop_init","func_id":8})JSON");
}

// Return TRUE if a gimple call is a valid decryptor.

static bool
call_is_decryptor_p (gcall *call)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","function":"call_is_decryptor_p","func_id":7,"num_params":1,"start_line":136,"end_line":151,"metrics":{"num_params":1,"call_count":5,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"call","type":"int *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"call_is_decryptor_p","func_id":7,"name":"call","type":"int *","is_pointer":true,"is_const":false})JSON");
  // A valid decryptor works in place, not frobbing arguments passed
  // by reference.  Make sure we return something.
  if (!gimple_call_lhs (call)) {
      log_with_timestamp(R"JSON({"event":"RETURN","function":"call_is_decryptor_p","func_id":7,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","line":142,"ret_type":"bool","expr":"false"})JSON");
      log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"call_is_decryptor_p","func_id":7})JSON");
      return false;
  }

  tree fn = gimple_call_fn (call);
  while (fn && TREE_CODE (fn) == ADDR_EXPR)
    fn = TREE_OPERAND (fn, 0);

    log_with_timestamp(R"JSON({"event":"RETURN","function":"call_is_decryptor_p","func_id":7,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","line":148,"ret_type":"bool","expr":"fn && DECL_P (fn) && lookup_attribute (\"decryptor\", DECL_ATTRIBUTES (fn))"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"call_is_decryptor_p","func_id":7})JSON");
  return fn
    && DECL_P (fn)
    && lookup_attribute ("decryptor", DECL_ATTRIBUTES (fn));
}

// Callback for the SSA value propagation engine.

static enum ssa_prop_result
prop_visit_stmt (gimple *stmt, edge *taken_edge_p ATTRIBUTE_UNUSED,
                 tree *output_p)
{
  enum ssa_prop_result result = SSA_PROP_NOT_INTERESTING;

  // We only care about SSA temporaries, because anything living in
  // memory is assumed to be untrusted and encrypted.
  tree lhs = gimple_get_lhs (stmt);
  if (!lhs || TREE_CODE (lhs) != SSA_NAME)
    return SSA_PROP_VARYING;

  // If the statement changes the flow, mark it as VARYING so the
  // successor blocks get analyzed.
  if (stmt_ends_bb_p (stmt))
    return SSA_PROP_VARYING;

  int version = SSA_NAME_VERSION (lhs);
  int old_l = lattice_values[version];

  if (gassign *assign_stmt = dyn_cast <gassign *> (stmt))
    {
      if (decrypted_p (gimple_assign_rhs1 (assign_stmt)))
        {
          result = SSA_PROP_INTERESTING;
          goto prop_visit_stmt_exit;
        }
      if (gimple_assign_rhs_class (assign_stmt) == GIMPLE_BINARY_RHS)
        {
          if (decrypted_p (gimple_assign_rhs2 (assign_stmt)))
            {
              result = SSA_PROP_INTERESTING;
              goto prop_visit_stmt_exit;
            }
          tree type = TREE_TYPE (lhs);
          if (INTEGRAL_TYPE_P (type))
            {
              tree_code code = gimple_assign_rhs_code (assign_stmt);
              if (code == BIT_IOR_EXPR
                  || code == BIT_XOR_EXPR
                  || code == BIT_AND_EXPR)
                {
                  result = SSA_PROP_INTERESTING;
                  goto prop_visit_stmt_exit;
                }
            }
        }
    }
  else if (gcall *call_stmt = dyn_cast <gcall *> (stmt))
    {
      if (call_is_decryptor_p (call_stmt))
        {
          if (TREE_CODE (lhs) == SSA_NAME)
            {
              result = SSA_PROP_INTERESTING;
              goto prop_visit_stmt_exit;
            }
          else
            gcc_unreachable ();
        }
    }

 prop_visit_stmt_exit:

  if (old_l == result)
    return SSA_PROP_NOT_INTERESTING;

  if (result == SSA_PROP_INTERESTING)
    {
      mark_decrypted (lhs);
      *output_p = lhs;
    }

  lattice_values[version] = (int) result;
  return result;
}

// Propagate function pointer encryptness by evaluating all incoming
// arguments for PHI.

static enum ssa_prop_result
prop_visit_phi_node (gphi *phi)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","function":"prop_visit_phi_node","func_id":6,"num_params":1,"start_line":235,"end_line":262,"metrics":{"num_params":1,"call_count":4,"has_recursion":false,"has_loop":true,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"phi","type":"int *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"prop_visit_phi_node","func_id":6,"name":"phi","type":"int *","is_pointer":true,"is_const":false})JSON");
  tree lhs = PHI_RESULT (phi);
  int version = SSA_NAME_VERSION (lhs);
  int old_l = lattice_values[version];

  for (size_t i = 0; i < gimple_phi_num_args (phi); i++)
    {
      edge e = gimple_phi_arg_edge (phi, i);

      if (!(e->flags & EDGE_EXECUTABLE)) {
          log_with_timestamp(R"JSON({"event":"RETURN","function":"prop_visit_phi_node","func_id":6,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","line":249,"ret_type":"enum ssa_prop_result","expr":"SSA_PROP_NOT_INTERESTING"})JSON");
          log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"prop_visit_phi_node","func_id":6})JSON");
          return SSA_PROP_NOT_INTERESTING;
      }

      tree arg = PHI_ARG_DEF (phi, i);
      if (TREE_CODE (arg) != SSA_NAME
          || !decrypted_p (arg))
        log_with_timestamp(R"JSON({"event":"RETURN","function":"prop_visit_phi_node","func_id":6,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","line":257,"ret_type":"enum ssa_prop_result","expr":"SSA_PROP_NOT_INTERESTING"})JSON");
        log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"prop_visit_phi_node","func_id":6})JSON");
        return SSA_PROP_NOT_INTERESTING;
    }

  if (old_l == SSA_PROP_INTERESTING) {
      log_with_timestamp(R"JSON({"event":"RETURN","function":"prop_visit_phi_node","func_id":6,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","line":263,"ret_type":"enum ssa_prop_result","expr":"SSA_PROP_NOT_INTERESTING"})JSON");
      log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"prop_visit_phi_node","func_id":6})JSON");
      return SSA_PROP_NOT_INTERESTING;
  }

  lattice_values[version] = (int) SSA_PROP_INTERESTING;

  mark_decrypted (lhs);
  log_with_timestamp(R"JSON({"event":"RETURN","function":"prop_visit_phi_node","func_id":6,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","line":271,"ret_type":"enum ssa_prop_result","expr":"SSA_PROP_INTERESTING"})JSON");
  log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"prop_visit_phi_node","func_id":6})JSON");
  return SSA_PROP_INTERESTING;
}

// Unmodified PARM_DECLs can be called as a callback.

static bool
unmodified_parm_decl_p (tree arg)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","function":"unmodified_parm_decl_p","func_id":5,"num_params":1,"start_line":266,"end_line":273,"metrics":{"num_params":1,"call_count":0,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"arg","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"unmodified_parm_decl_p","func_id":5,"name":"arg","type":"int","is_pointer":false,"is_const":false})JSON");
  log_with_timestamp(R"JSON({"event":"RETURN","function":"unmodified_parm_decl_p","func_id":5,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","line":271,"ret_type":"bool","expr":"(TREE_CODE (arg) == PARM_DECL || (TREE_CODE (arg) == SSA_NAME && SSA_NAME_IS_DEFAULT_DEF (arg) && TREE_CODE (SSA_NAME_VAR (arg)) == PARM_DECL))"})JSON");
  log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"unmodified_parm_decl_p","func_id":5})JSON");
  return (TREE_CODE (arg) == PARM_DECL
          || (TREE_CODE (arg) == SSA_NAME
              && SSA_NAME_IS_DEFAULT_DEF (arg)
              && TREE_CODE (SSA_NAME_VAR (arg)) == PARM_DECL));
}

// Return true if TYPE is a pointer to a record that contains a
// function pointer.

static bool
pointer_to_record_contains_funcp_p (tree type)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","function":"pointer_to_record_contains_funcp_p","func_id":4,"num_params":1,"start_line":278,"end_line":298,"metrics":{"num_params":1,"call_count":1,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"type","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"pointer_to_record_contains_funcp_p","func_id":4,"name":"type","type":"int","is_pointer":false,"is_const":false})JSON");
  if (TREE_CODE (type) != POINTER_TYPE
      || TREE_CODE (TREE_TYPE (type)) != RECORD_TYPE)
    log_with_timestamp(R"JSON({"event":"RETURN","function":"pointer_to_record_contains_funcp_p","func_id":4,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","line":283,"ret_type":"bool","expr":"false"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"pointer_to_record_contains_funcp_p","func_id":4})JSON");
    return false;

  bool has_funcp = false;
  for (tree fld = TYPE_FIELDS (TREE_TYPE (type)); fld; fld = DECL_CHAIN (fld))
    {
      gcc_assert (TREE_CODE (fld) == FIELD_DECL);
      tree fld_type = TREE_TYPE (fld);
      if (TREE_CODE (fld_type) == POINTER_TYPE
          && TREE_CODE (TREE_TYPE (fld_type)) == FUNCTION_TYPE)
        {
          has_funcp = true;
          break;
        }
    }
    log_with_timestamp(R"JSON({"event":"RETURN","function":"pointer_to_record_contains_funcp_p","func_id":4,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","line":297,"ret_type":"bool","expr":"has_funcp"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"pointer_to_record_contains_funcp_p","func_id":4})JSON");
  return has_funcp;
}

// Now that we have propagated the encryptness of function pointers,
// perform a final pass over the statement sequence and warn on any
// suspicious behavior we may have found.

static void
prop_finalize (function *fun)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","function":"prop_finalize","func_id":3,"num_params":1,"start_line":304,"end_line":405,"metrics":{"num_params":1,"call_count":12,"has_recursion":false,"has_loop":true,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"fun","type":"int *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"prop_finalize","func_id":3,"name":"fun","type":"int *","is_pointer":true,"is_const":false})JSON");
  basic_block bb;
  FOR_EACH_BB_FN (bb, fun)
    {
      for (gimple_stmt_iterator si = gsi_start_bb (bb); !gsi_end_p (si);
           gsi_next (&si))
        {
          gimple *stmt = gsi_stmt (si);
          location_t loc = gimple_location (stmt);

          if (gimple_code (stmt) == GIMPLE_COND
              && TREE_CODE_CLASS (gimple_cond_code (stmt)) == tcc_comparison)
            {
              tree lhs = gimple_cond_lhs (stmt);
              tree rhs = gimple_cond_rhs (stmt);
              bool lhs_is_funcp = TREE_CODE (TREE_TYPE (lhs)) == POINTER_TYPE
                && TREE_CODE (TREE_TYPE (TREE_TYPE (lhs))) == FUNCTION_TYPE
                && !CONSTANT_CLASS_P (lhs);
              bool rhs_is_funcp = TREE_CODE (TREE_TYPE (rhs)) == POINTER_TYPE
                && TREE_CODE (TREE_TYPE (TREE_TYPE (rhs))) == FUNCTION_TYPE
                && !CONSTANT_CLASS_P (rhs);

              if (CONSTANT_CLASS_P (rhs)
                  && lhs_is_funcp
                  && !decrypted_p (lhs))
                warning_at (loc, 0,
                            "comparison between encrypted function pointer "
                            "and constant");
              else if (lhs_is_funcp && rhs_is_funcp)
                {
                  bool lhsd = decrypted_p (lhs);
                  bool rhsd = decrypted_p (rhs);
                  if ((lhsd && !rhsd) || (!lhsd && rhsd))
                    warning_at (loc, 0,
                                "comparison between different encrypted "
                                "function pointers");
                }
            }
          else if (gcall *call = dyn_cast <gcall *> (gsi_stmt (si)))
            {
              for (unsigned i = 0; i < gimple_call_num_args (call); ++i)
                {
                  tree arg = gimple_call_arg (call, i);
                  tree arg_type = TREE_TYPE (gimple_call_arg (call, i));
                  if (TREE_CODE (arg_type) == POINTER_TYPE
                      && TREE_CODE (TREE_TYPE (arg_type)) == FUNCTION_TYPE
                      && !decrypted_p (arg)
                      && (TREE_CODE (arg) != ADDR_EXPR
                          || TREE_CODE (TREE_OPERAND (arg,
                                                      0)) != FUNCTION_DECL)
                      && !unmodified_parm_decl_p (arg)
                      && !CONSTANT_CLASS_P (arg)
                      && !call_is_decryptor_p (call))
                    warning_at (loc, 0,
                                "passing non decrypted function pointer in "
                                "argument %d", i + 1);
                  else if (pointer_to_record_contains_funcp_p (arg_type))
                    warning_at (loc, 0,
                                "function pointer passed in object in memory");
                }

              tree fn = gimple_call_fn (call);
              if (!fn || TREE_CODE (fn) == ADDR_EXPR)
                continue;

              if (unmodified_parm_decl_p (fn))
                continue;

              if (!decrypted_p (fn))
                warning_at (loc, 0,
                            "possible use of non decrypted function pointer");
            }
          else if (gassign *asn = dyn_cast <gassign *> (gsi_stmt (si)))
            {
              tree lhs = gimple_assign_lhs (asn);
              tree type = TREE_TYPE (lhs);

              if (TREE_CODE (type) == POINTER_TYPE
                  && TREE_CODE (TREE_TYPE (type)) == FUNCTION_TYPE
                  && ((DECL_P (lhs)
                       && is_global_var (lhs))
                      || (TREE_CODE (lhs) == COMPONENT_REF
                          && TREE_CODE (TREE_OPERAND (lhs, 0)) == MEM_REF)))
                {
                  if (!gimple_assign_single_p (asn))
                    {
                      warning_at
                        (loc, 0,
                         "possible copy of function pointer to memory");
                      continue;
                    }
                  warning_at (loc, 0, "copy of a function pointer to memory");
                }
            }
        }
    }

  lattice_values.release ();
  BITMAP_FREE (decrypted_ssa_bitmap);
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"prop_finalize","func_id":3})JSON");
}

// --- GCC 10: custom propagation engine subclass ---

class funcp_propagation_engine : public ssa_propagation_engine
{
public:
  virtual enum ssa_prop_result
  visit_stmt (gimple *stmt, edge *taken_edge_p, tree *output_p)
  {
    return prop_visit_stmt (stmt, taken_edge_p, output_p);
  }

  virtual enum ssa_prop_result
  visit_phi (gphi *phi)
  {
    return prop_visit_phi_node (phi);
  }
};

unsigned int
funcp_pass::execute (function *fun)
{
  prop_init (fun);

  funcp_propagation_engine engine;
  engine.ssa_propagate ();

  prop_finalize (fun);
  return 0;
}

// Handle the "decryptor" attribute in the front-ends.

static tree
handle_decryptor_attribute (tree *node,
                            tree name ATTRIBUTE_UNUSED,
                            tree args ATTRIBUTE_UNUSED,
                            int flags ATTRIBUTE_UNUSED,
                            bool *no_add_attrs ATTRIBUTE_UNUSED)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","function":"handle_decryptor_attribute","func_id":2,"num_params":2,"start_line":439,"end_line":448,"metrics":{"num_params":2,"call_count":0,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"node","type":"int *","is_pointer":true,"is_const":false},{"name":"name","type":"int","is_pointer":false,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"handle_decryptor_attribute","func_id":2,"name":"node","type":"int *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"handle_decryptor_attribute","func_id":2,"name":"name","type":"int","is_pointer":false,"is_const":false})JSON");
  gcc_assert (TREE_CODE (*node) == FUNCTION_DECL);
    log_with_timestamp(R"JSON({"event":"RETURN","function":"handle_decryptor_attribute","func_id":2,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","line":447,"ret_type":"int","expr":"NULL"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"handle_decryptor_attribute","func_id":2})JSON");
  return NULL;
}

static struct attribute_spec decryptor_attr = {
    "decryptor",                // name
    0,                          // min_length
    0,                          // max_length
    true,                       // decl_required
    false,                      // type_required
    false,                      // affects_type_identity
    handle_decryptor_attribute  // handler
};

static void
register_plugin_attributes (void *event_data ATTRIBUTE_UNUSED,
                            void *data ATTRIBUTE_UNUSED)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","function":"register_plugin_attributes","func_id":1,"num_params":1,"start_line":460,"end_line":465,"metrics":{"num_params":1,"call_count":2,"has_recursion":false,"has_loop":false,"has_if":false,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"event_data","type":"void *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"register_plugin_attributes","func_id":1,"name":"event_data","type":"void *","is_pointer":true,"is_const":false})JSON");
  register_attribute (&decryptor_attr);
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"register_plugin_attributes","func_id":1})JSON");
}

int
plugin_init (struct plugin_name_args *plugin_info,
             struct plugin_gcc_version *version)
{
    log_with_timestamp(R"JSON({"event":"FUNC_ENTER","file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","function":"plugin_init","func_id":0,"num_params":2,"start_line":467,"end_line":487,"metrics":{"num_params":2,"call_count":0,"has_recursion":false,"has_loop":false,"has_if":true,"has_switch":false,"has_goto":false,"stmt_count":0},"flags":{"is_method":false,"is_static_method":false,"is_const_method":false,"is_virtual_method":false,"is_variadic":false},"params":[{"name":"plugin_info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false},{"name":"version","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false}]})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"plugin_info","type":"struct plugin_name_args *","is_pointer":true,"is_const":false})JSON");
    log_with_timestamp(R"JSON({"event":"PARAM","function":"plugin_init","func_id":0,"name":"version","type":"struct plugin_gcc_version *","is_pointer":true,"is_const":false})JSON");
  if (!plugin_default_version_check (version, &gcc_version)) {
      log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","line":472,"ret_type":"int","expr":"1"})JSON");
      log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
      return 1;
  }

  struct register_pass_info pass_info = {
    new funcp_pass (g),         // opt_pass
    "einline",                  // reference_pass_name
    1,                          // ref_pass_instance_number
    PASS_POS_INSERT_BEFORE,     // enum pass_positioning_ops
  };

  register_callback (plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP,
                     NULL, &pass_info);
  register_callback (plugin_info->base_name, PLUGIN_ATTRIBUTES,
                     register_plugin_attributes, NULL);

    log_with_timestamp(R"JSON({"event":"RETURN","function":"plugin_init","func_id":0,"file":"/home/nimantha/Desktop/KU_Leuven_App_Gen/Experimentation_Program_Gen/Web_Repos/funcp-encrypt-instrumented/funcp-encrypt.cc","line":486,"ret_type":"int","expr":"0"})JSON");
    log_with_timestamp(R"JSON({"event":"FUNC_EXIT","function":"plugin_init","func_id":0})JSON");
  return 0;
}
