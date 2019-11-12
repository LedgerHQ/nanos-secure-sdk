import sys

# This is not required if you've installed pycparser into
# your site-packages/ with setup.py
#
sys.path.insert(0, '..')

from pycparser import c_parser, c_ast, parse_file
from pycparser.portability import printme

import hashlib
import re
import os
import string
import getopt
import binascii
import argparse

def _explain_type(decl):
    """ Recursively explains a type decl node
    """
    typ = type(decl)
    
    if typ == c_ast.TypeDecl:
        quals = ' '.join(decl.quals) + ' ' if decl.quals else ''
        return quals + str(_explain_type(decl.type))
    elif typ == c_ast.Typename:
        return str(_explain_type(decl.type))
    elif typ == c_ast.Decl:
        if (len(decl.funcspec) > 0):
          return ' '.join(decl.funcspec) + ' ' + str(_explain_type(decl.type))        
        return str(_explain_type(decl.type))
    elif typ == c_ast.IdentifierType:
        return ' '.join(decl.names)
    elif typ == c_ast.PtrDecl:
        quals = ' '.join(decl.quals) + ' ' if decl.quals else ''
        return quals + _explain_type(decl.type) + " *"
    elif typ == c_ast.ArrayDecl:
        arr = 'array'
        try:
          if decl.dim: arr += '[%s]' % decl.dim.value
        except:
          pass
        
        return arr + " of " + _explain_type(decl.type)
        

    elif typ == c_ast.FuncDecl:
        if decl.args:
            print(decl.args.params)
            params = [_explain_type(param) for param in decl.args.params]
            try:
              if params:
                args = ', '.join(params)
            except:
              args = ''
              pass
        else:
            args = ''
        
        return ('function(%s) returning ' % (args) +
                _explain_type(decl.type))


def func_parameters(node):
    params = []
    for param in node.type.args.params:
        if param.name:
            params.append([_explain_type(param), param.name, param.length])
        else:
            params.append(["void", ""])
    return params

def func_return_type(node):
    return _explain_type(node.type.type)

# return the prototype from a c_ast.FuncDecl parent
# TODO handle attribute definitions
def func_prototype(node, suffix=''):
    returntype = _explain_type(node.type.type)
    funcname = node.name + suffix
    args = ''
    try:
        params = []
        for param in node.type.args.params:
            if param.name:
                params.append(_explain_type(param) + ' ' + param.name)
            else:
                params.append("void")
        if params:
            args = ', '.join(params)
    except:
        pass

    return returntype + ' ' + funcname + ' ( ' + args + ' ) '


parser = argparse.ArgumentParser()
parser.add_argument("--libname", help="The name of the lib when generated")
parser.add_argument("--stub", help="The output file for generated stubs")
parser.add_argument("--defs", help="The output file for generated definitions (IDs and such)")
parser.add_argument("--disp", help="The output file for generated dispatcher")
parser.add_argument("-D", help="Additional argument to subcpp parsing", action='append')
parser.add_argument("-I", help="Additional argument to subcpp parsing", action='append')
parser.add_argument("--idsref", help="Filename to read and write the calls ID attributions (format is one funcname:id per line")
parser.add_argument('files', nargs='*')
parser.add_argument("--antifa", help="Enhance anti FA on the syscall barrier", action="store_true")

args = parser.parse_args()

cpp_args = []
mode_syscalls = args.syscalls
libname = args.libname
if args.D:
  for d in args.D:
    cpp_args.append("-D"+d)
if args.I:
  for i in args.I:
    cpp_args.append("-I"+i)

if libname is None:
  raise BaseException("Library name (application name when loading the library) is missing")
taskswitchs = []
shareds = []
libcalls = []
ids = {}

# first call has number 1 :), unless an ids ref file is provided
call_id = 1
ids = {}
if args.idsref:
  idsref_file = None
  try:
    idsref_file = open(args.idsref,'r+')
  except:
    print("WARNING: callgen ID reference file '{}' is not existing".format(args.idsref))
    pass
  # if the file was opened (don't ignore invalid format, just file not existing)
  if idsref_file:
    for line in idsref_file:
      funcprotohash, id = line.split(':')
      id = int(id, 0);
      ids[funcprotohash] = id
      if call_id < id:
        call_id = id
    idsref_file.close()

    #use the next one available
    call_id += 1

class FuncVisitor(c_ast.NodeVisitor):
  def visit_Decl(self, node):
    if (type(node.type) == c_ast.FuncDecl):
      if ("libcall" in node.funcspec):
        libcalls.append(node)


# ensure syscall/shared decl are kept by cpp
cpp_args.append("-DPERMISSION(x)=permission(__ ## x)")
cpp_args.append("-DCXPORT(x)=cxport(__ ## x)")
cpp_args.append("-DTASKLEVEL(x)=tasklevel(__ ## x)")
cpp_args.append("-DPLENGTH=plength")
cpp_args.append("-DLIBCALL=libcall")
# ensure on x86 processing of __attribute__ whatever is not problematic
cpp_args.append("-D__attribute__(...)=")
cpp_args.append("-D__extension__=")
cpp_args.append("-D__restrict=")
cpp_args.append("-D__inline=")
cpp_args.append("-D__asm__(...)=")
cpp_args.append("-D__builtin_va_list=int")
cpp_args.append("-D__builtin_offsetof(x,y)=0")
#cpp_args.append("-Dsizeof(x)=0")

# parse every files provided
  print("Parsing: " + filename)
  ast = parse_file(filename, use_cpp=True, cpp_args=cpp_args)
  v = FuncVisitor()
  v.visit(ast)


"""
generate a .h for syscall function declarations
generate a .c for syscall stubs (to be compiled in the userland)
generate a .c containing the syscalls dispatcher toward syscalled os functions (kernelmode)
"""

defs_file = open(args.defs, 'w+')
stub_file = open(args.stub,'w+')
dispatcher_file = open(args.disp,'w+')
print (".h defs: " + args.defs)
print (".c stub: " + args.stub)
print (".c disp: " + args.disp)

if 1:
  defs_file.write("""
    /* MACHINE GENERATED: DO NOT MODIFY */
#ifndef LIBCALL_DEFS_H
#define LIBCALL_DEFS_H
""")

  stub_file.write("""
  /* MACHINE GENERATED: DO NOT MODIFY */

  #include "os.h"

  // The name is forced to simplify generation
  #include \"""" + libname + """.h\"

  // include the syscall to use the os_lib_call trampoline.
  #include \"""" + libname + """_libcalls.h\"

  """)

  dispatcher_file.write("""
  /* MACHINE GENERATED: DO NOT MODIFY */

  #include \"""" + libname + """_libcalls.h\"

  // The name is forced to simplify generation
  #include \"""" + libname + """.h\"

  void libcall_enter(unsigned int libcall_index, unsigned int* params_array) __attribute__((weak));
  void libcall_exit(void) __attribute__((weak));

  /**
   * Library entry point. 
   */
  void libmain(int parameters_int) {
    unsigned int ret=0;
    unsigned int retid=0;

  // thanks LLVM for the error when first main argument is not an int.
  #define parameters ((unsigned int*)parameters_int)

    // don't initialize the exception context, we don't have the hand on the global

    BEGIN_TRY {
      TRY {
        // DESIGN NOTE: no need to ensure the syscall api level version is correct, it is checked on the app side.

        // mark syscall entry
        libcall_enter(parameters[0], &parameters[1]);

        switch(parameters[0]) {
""")
  # parameters[0] == libname
  # parameters[1] == libcall_id
  # parameters[2...] == call parameters ... // at least the return value
  # parameters[1] = return id
  # parameters[2] = return value
  OFFSET_FIRST_PARAM_STUB = 2
  OFFSET_FIRST_PARAM_ALLOC = 3
  OFFSET_FIRST_PARAM_DISPATCH = 1


class Error:
  pass


class ExprVisitor(c_ast.NodeVisitor):
    def visit_Constant(self, n):
        return n.value

    def visit_ID(self, n):
        return n.name

    def visit_str(self, s):
        return s

    def _parenthesize_if(self, n, condition):
        """ Visits 'n' and returns its string representation, parenthesized
            if the condition function applied to the node returns True.
        """
        s = self.visit(n)
        if condition(n):
            return '(' + s + ')'
        else:
            return s

    def _parenthesize_unless_simple(self, n):
        """ Common use case for _parenthesize_if
        """
        return self._parenthesize_if(n, lambda d: not self._is_simple_node(d))

    def _is_simple_node(self, n):
        """ Returns True for nodes that are "simple" - i.e. nodes that always
            have higher precedence than operators.
        """
        return isinstance(n,(   c_ast.Constant, c_ast.ID, c_ast.ArrayRef,
                                c_ast.StructRef, c_ast.FuncCall))

    def _generate_decl(self, n):
        """ Generation from a Decl node.
        """
        s = ''
        if n.funcspec: s = ' '.join(n.funcspec) + ' '
        if n.storage: s += ' '.join(n.storage) + ' '
        s += self._generate_type(n.type)
        return s

    def _generate_type(self, n, modifiers=[]):
        """ Recursive generation from a type node. n is the type node.
            modifiers collects the PtrDecl, ArrayDecl and FuncDecl modifiers
            encountered on the way down to a TypeDecl, to allow proper
            generation from it.
        """
        typ = type(n)
        #~ print(n, modifiers)

        if typ == c_ast.TypeDecl:
            s = ''
            if n.quals: s += ' '.join(n.quals) + ' '
            s += self.visit(n.type)

            nstr = n.declname if n.declname else ''
            # Resolve modifiers.
            # Wrap in parens to distinguish pointer to array and pointer to
            # function syntax.
            #
            for i, modifier in enumerate(modifiers):
                if isinstance(modifier, c_ast.ArrayDecl):
                    if (i != 0 and isinstance(modifiers[i - 1], c_ast.PtrDecl)):
                        nstr = '(' + nstr + ')'
                    nstr += '[' + self.visit(modifier.dim) + ']'
                elif isinstance(modifier, c_ast.FuncDecl):
                    if (i != 0 and isinstance(modifiers[i - 1], c_ast.PtrDecl)):
                        nstr = '(' + nstr + ')'
                    nstr += '(' + self.visit(modifier.args) + ')'
                elif isinstance(modifier, c_ast.PtrDecl):
                    if modifier.quals:
                        nstr = '* %s %s' % (' '.join(modifier.quals), nstr)
                    else:
                        nstr = '*' + nstr
            if nstr: s += ' ' + nstr
            return s
        elif typ == c_ast.Decl:
            return self._generate_decl(n.type)
        elif typ == c_ast.Typename:
            return self._generate_type(n.type)
        elif typ == c_ast.IdentifierType:
            return ' '.join(n.names) + ' '
        elif typ in (c_ast.ArrayDecl, c_ast.PtrDecl, c_ast.FuncDecl):
            return self._generate_type(n.type, modifiers + [n])
        else:
            return self.visit(n)

    def visit_IdentifierType(self, n):
        return ' '.join(n.names)

    def visit_Typename(self, n):
        return self._generate_type(n.type)

    def visit_UnaryOp(self, n):
        operand = self._parenthesize_unless_simple(n.expr)
        if n.op == 'p++':
            return '%s++' % operand
        elif n.op == 'p--':
            return '%s--' % operand
        elif n.op == 'sizeof':
            # Always parenthesize the argument of sizeof since it can be
            # a name.
            return 'sizeof(%s)' % self.visit(n.expr)
        else:            
            return '%s%s' % (n.op, operand)

   
    
    def visit_BinaryOp(self, n):
        lval_str = self._parenthesize_if(n.left,
                            lambda d: not self._is_simple_node(d))
        rval_str = self._parenthesize_if(n.right,
                            lambda d: not self._is_simple_node(d))
        return '%s %s %s' % (lval_str, n.op, rval_str)



    def generic_visit(self, node):
        print("Unsupported PLENGTH expression at " + str(node.coord) + ", parsing class: " + str(node.__class__.__name__))
        raise Error


protect_stubs = ""
defined_calls = []
calls = libcalls

dispatcher_file_cases = ""
parameter_array_max_len=OFFSET_FIRST_PARAM_ALLOC

for node in calls:
    func_name = node.name
    if not func_name in defined_calls:
      if 1:
        print("LIBCALL " + str(func_name) + " " + str(node.funcspec))

  
      # syscall byte dependent of the function prototype (deterministic build)
      func = func_prototype(node);
      funcinline = func_prototype(node, "_inline");

      # id lookup or attribution
      # hash the function name and prototype
      func_hash = hashlib.sha256(func.encode('utf-8')).hexdigest()
      
      if func_hash in ids:
        thiscallid = ids[func_hash]
      else:
        thiscallid = call_id

      idin_base = (int(func_hash[:2], 16)&0xFF) | (thiscallid<<8)
      idout_base = (int(func_hash[2:4], 16)&0xFF) | (thiscallid<<8)

      if not func_hash in ids:
        # store for persistence if required
        ids[func_hash] = call_id
        # prepare next allocation if required
        call_id += 1

      if 1:
        id_in = idin_base | 0x70000000
        id_out = idout_base | 0xA0000000
      #node.show(attrnames=True,nodenames=True)
      #print(func)
      params = func_parameters(node)
      returntype = func_return_type(node)

      ################## DEF
      if 1:
        const_id_in  = "LIBCALL_" + libname + "_" + func_name + "_ID_IN"
        const_id_out = "LIBCALL_" + libname + "_" + func_name + "_ID_OUT"
      defs_file.write("#define %s  0x%xUL\n"%(const_id_in,id_in))
      defs_file.write("#define %s  0x%xUL\n"%(const_id_out,id_out))
      
      # not required
      #defs_file.write(func + ";\n\n")

      ################## STUB
      stub=func + "\n"
      stub_no_ipsr=func + "\n"
      stubinline=funcinline + "\n"
      s="{\n"
      sparams=""
      # when the function has some parameters
      if params[0][0] != "void":
          sparams+="  volatile unsigned int parameters ["+str(OFFSET_FIRST_PARAM_ALLOC)+"+"+str(len(params))+"];\n"
          if OFFSET_FIRST_PARAM_ALLOC+len(params) > parameter_array_max_len:
            parameter_array_max_len = OFFSET_FIRST_PARAM_ALLOC+len(params)
      else:
          sparams+="  volatile unsigned int parameters ["+str(OFFSET_FIRST_PARAM_ALLOC)+"];\n"
      
      if 1:
        sparams+="  parameters[0] = (unsigned int)\""+libname+"\";\n"
        sparams+="  parameters[1] = (unsigned int)"+const_id_in+";\n"
      #temporary place to store the try context address
      i = OFFSET_FIRST_PARAM_STUB
      callargs=""
      j=0
      if returntype != "void" and ((params[0][0] == "void") or (len(params) < 2)):
          sparams += "#ifdef __clang_analyzer__\n"
          sparams += "  parameters[1] = 0;\n"
          sparams += "#endif\n"
      if params[0][0] != "void":
          # hope params is ordered
          for param in params:
              sparams+="  parameters["+str(i)+"] = (unsigned int)"+str(param[1])+";\n"
              callargs += param[1]
              j+=1
              if (j != len(params)):
                callargs += ", "
              i+=1
      if 1:
        stub+=s+"  os_lib_call(parameters);\n"
        if (args.antifa):
          stub+="  if (parameters[1] != "+const_id_out+") {\n"
          stub+="    THROW(EXCEPTION_SECURITY);\n"
          stub+="  }\n"
        if returntype != "void":
            stub+="  return ("+returntype+")parameters[2];\n"
        stub+="}\n\n"

      if 1:
        stub_file.write(stub)

      ################## DISPATCH
      # generate the dispatcher case
      if 1:
        dispatcher_case = "          case (("+const_id_in+">>8)&0xFFFFFUL):\n"


      # forge call with arguments (and prepare args check code as well)
      call = func_name

      call += "("
      i = 0
      if params[0][0] != "void":
          for param in params:
              if 1:
                call += "("+param[0]+") parameters["+str(i+OFFSET_FIRST_PARAM_DISPATCH)+"]"
              i+=1
              if (i < len(params)):
                  call += ", "
      call += ')'


      if returntype != "void":
          dispatcher_case+="            ret = (unsigned int)"+call+";\n"
      else:
          dispatcher_case+="            "+call+";\n"
      if (args.antifa):
        dispatcher_case+="            retid = "+const_id_out+";\n"
      dispatcher_case+="            break;\n"

      if 1:
        dispatcher_file_cases += dispatcher_case

    defined_calls.append(func_name)

if 1:
  defs_file.write("#endif // LIBCALL_DEFS_H\n")

  dispatcher_file.write(dispatcher_file_cases+"""
          default:
            // unknown libcall throws
            PRINTF("Unknown libcall identifier: 0x%08X\\n", parameters[0]);
            THROW(EXCEPTION);
            // make compiler happy
            break; 
        }
        // ensure exiting the syscall
        libcall_exit();

        // set the return identifier for end to end path validation, to avoid jumps
        // NOTE: the kernel shift the parameters array of 1 as it consumed the library name
        // return id
        parameters[0] = retid; 
        // return the call value
        parameters[1] = ret; 
        os_lib_end();
      }
      CATCH_OTHER(e) {
        libcall_exit();

        // special stack unwinding longjmp
        os_lib_throw(e);
      }
      FINALLY {
        // not reached after os_lib_throw
      }
    }
    END_TRY;
  }

  void libcall_enter(unsigned int libcall_index, unsigned int* params_array) /*__attribute__((weak))*/ {
    // nothing to be done by default, overwrite this function upon needs.
  }
  void libcall_exit(void) /*__attribute__((weak))*/ {
    // nothing to be done by default, overwrite this function upon needs.
  }

""")

# rewrite ids if changed
if args.idsref:
  idsref_file = open(args.idsref, "w+")
  idsref_file.truncate()
  for funcprotohash in ids:
    idsref_file.write(funcprotohash+":"+hex(ids[funcprotohash])+"\n")
  idsref_file.flush();
  idsref_file.close()

# flush and close files
defs_file.flush();
stub_file.flush();
dispatcher_file.flush();

defs_file.close();
stub_file.close();
dispatcher_file.close();
