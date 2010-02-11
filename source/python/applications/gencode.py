#!/usr/bin/env python
#
# Test driver for modulator
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

import sys
import modulator.parse as parse
import modulator.cpregen as cpregen
import modulator.ppregen as ppregen

#def create_ast(filename):
#    f = open(filename)
#    rv = parse.scan(f.read(), filename)
#    f.close()
#    ast = parse.parse(rv)
#    return ast

# import modulator.fpregen as fpregen
# ppregen.PreGenPython(pp)
# fpregen.GenFortran(pp)

verbose = 1

def read_ast(input):
    # create pdb only
    ast = parse.create_ast(input)
    # resolve types
    parse.typetool(ast)
    # generate C
    cpregen.pregen(ast)
    #    cpregen.gen(ast)
    # generate python
    ppregen.pregen(ast)
    return ast

# create pdb module
if verbose > 0:
    print "Generating PDB"
ast = read_ast('pdb.idl')
ppregen.gen(ast)

# create pgs module
if verbose > 0:
    print "Generating PGS"
pdb_ast = read_ast('pdb.idl')
# read existing pdb attributes
pdb_file_attrs = ppregen.read_old_files(pdb_ast, verbose)

# reread
pdb_ast = parse.create_ast('pdb.idl')
# mark all module methods and classes as 'modify=0'
for method in pdb_ast.method_nodes:
#    print method.name
    method.add_lang(parse.Lang('python', {'modify':0}))
for cls in pdb_ast.class_nodes:
#    print cls.name
    cls.add_lang(parse.Lang('python', {'modify':0,
                                       'file_name':''}))

pgs_ast = parse.create_ast('pgs.idl')
# create one tree
parse.updateast(pdb_ast, pgs_ast)
ast = pdb_ast
parse.typetool(ast)
#parse.printast(pdb_ast)

# generate C
cpregen.pregen(ast)
# cpregen.gen(ast)
# generate python
ppregen.pregen(ast)
#parse.printast(ast)
#print ppregen.print_type_map()
#raise SystemExit

# Take the method templates from pdb.idl
# and install into pgs.idl.
# also move pdb classes into pgs.
pgs_file_attrs = ppregen.read_old_files(ast, verbose)
pdb_attrs = pdb_file_attrs.get('pdb', {})
pgs_attrs = pgs_file_attrs.setdefault('pgs', {})

pdb_methods = pdb_attrs.get('method', {})
pgs_methods = pgs_attrs.setdefault('method', {})
pgs_methods.update(pdb_methods)

for cls in ('field', 'mesh_topology', 'set', 'mapping',
            'defstr', 'pdbdata', 'hashtab', 'assoc',
            'memdes', 'PDBfile'):
    pgs_attrs[cls] = pdb_attrs[cls]

visitor = ppregen.GenPython(pgs_file_attrs, verbose)
visitor.visit(ast)


