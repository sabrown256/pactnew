#!/usr/bin/env python
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

""" A python version of PDBview
"""

import cmd
import sys
import pact.pdb as pdb

class Viewer(cmd.Cmd):
    prompt = '-> '
    intro = '\n        pyPDBView 0.0\n'

    def __init__(self):
        self.cf = pdb.vif
        self.fdict = {'f0':pdb.vif}
        self.fd = 1
        cmd.Cmd.__init__(self)

    # Opening, Creating, Closing, and Controlling Files
    
    def do_cd(self, line):
        '''usage cd [<directory-name>]'''
        opts = line.split()
        dirname = None
        if len(opts) > 0:
            dirname = opts[0]
        self.cf.cd(dirname)

    def do_change_file(self, line):
        '''Usage: change-file [filename [mode [alias]]]'''
        opts = line.split()
        if len(opts) > 0:
            name = opts[0]
        else:
            name = 'aaa'
        if len(opts) > 1:
            mode = opts[1]
        else:
            mode = 'a'
        if len(opts) > 2:
            alias = opts[2]
        else:
            alias = 'f%d' % self.fd
            self.fd += 1

        try:
            fp = pdb.open(name, mode)
#            fp.as_raw = 1
            self.fdict[alias] = fp
            self.cf = fp
        except pdb.error, msg:
            print msg

    do_cf = do_change_file

    def do_close(self, line):
        if not self.cf == pdb.vif:
            self.cf.close()

        print "not implemented"

    def do_close_textfile(self, line):
        print "not implemented"
    do_clt = do_close_textfile
    
    def do_file(self, line):
        print "not implemented"

    def do_list_files(self, line):
        print "not implemented"
    do_lf = do_list_files

    def do_mkdir(self, line):
        '''usage" mkdir <directory-name>'''
        opts = line.split()
        if len(opts) == 0:
            print 'No directory name'
        else:
            self.cf.mkdir(opts[0])
        
    def do_n_entries(self, line):
        print "not implemented"
        
    def do_open_textfile(self, line):
        print "not implemented"
    do_ot = do_open_textfile

    def do_pwd(self, line):
        '''usage: pwd'''
        print self.cf.pwd()
    
    # Working with Data

    def do_array_nd_set(self, line):
        opts = line.split()
        print "not implemented"

    def do_change(self, line):
        opts = line.split()
        print "not implemented"

    def do_change_dimension(self, line):
        opts = line.split()
        print "not implemented"
    do_chdim = do_change_dimension

    def do_cmc(self, line):
        opts = line.split()
        print "not implemented"

    def do_copy(self, line):
        opts = line.split()
        print "not implemented"

    def do_copy_dir(self, line):
        opts = line.split()
        print "not implemented"

    def do_copy_mapping(self, line):
        opts = line.split()
        print "not implemented"
    do_cm = do_copy_mapping

    def do_def(self, line):
        opts = line.split()
        print "not implemented"

    def do_define_file_variable(self, line):
        opts = line.split()
        print "not implemented"

    def do_desc(self, line):
        opts = line.split()
        h = self.cf.host_chart
        dp = h[opts[0]]
        print dp

    def do_find(self, line):
        opts = line.split()
        print "not implemented"

    def do_ls(self, line):
        '''Usage: ls [pattern [type]]'''
        opts = line.split()
        pattern = None
        type = None
        if len(opts) > 0:
            pattern = opts[0]
        if len(opts) > 1:
            type = opts[1]
        a = self.cf.ls(pattern, type)
        print a

    def do_ls_attr(self, line):
        opts = line.split()
        print "not implemented"

    def do_lst(self, line):
        opts = line.split()
        print "not implemented"

    def do_make_ac_mapping_direct(self, line):
        opts = line.split()
        print "not implemented"
    do_ac = do_make_ac_mapping_direct

    def do_make_cp_set(self, line):
        opts = line.split()
        print "not implemented"
    do_cps = do_make_cp_set

    def do_make_lr_mapping_direct(self, line):
        opts = line.split()
        print "not implemented"
    do_lr = do_make_lr_mapping_direct

    def do_make_lr_mapping_synth(self, line):
        opts = line.split()
        print "not implemented"
    do_lrs = do_make_lr_mapping_synth

    def do_minmax(self, line):
        opts = line.split()
        print "not implemented"

    def do_pdb_copy(self, line):
        opts = line.split()
        print "not implemented"

    def do_print(self, line):
        opts = line.split()
        print "not implemented"

    def do_print_mapping(self, line):
        opts = line.split()
        print "not implemented"
    do_pm = do_print_mapping

    def do_save_mapping(self, line):
        opts = line.split()
        print "not implemented"
    do_sm = do_save_mapping

    def do_scale_file_variable(self, line):
        opts = line.split()
        print "not implemented"

    def do_struct(self, line):
        opts = line.split()
        print "not implemented"

    def do_table(self, line):
        opts = line.split()
        print "not implemented"

    def do_types(self, line):
        opts = line.split()
        print "not implemented"

    # Working with PDBview

    def do_command_log(self, line):
        opts = line.split()
        print "not implemented"

    def do_end(self, line):
        for fp in self.fdict.values():
            fp.close()
        sys.exit(0)
    do_quit = do_end

    def do_format(self, line):
        opts = line.split()
        print "not implemented"

    def do_ld(self, line):
        opts = line.split()
        print "not implemented"

    def do_mode(self, line):
        opts = line.split()
        print "not implemented"

    def do_set(self, line):
        opts = line.split()
        print "not implemented"

    def do_synonym(self, line):
        opts = line.split()
        print "not implemented"

    def do_EOF(self, line):
        print
        self.quit(line)


    def default(self, line):
        opts = line.split()
        try:
            v = self.cf.read(opts[0])
            print v
        except pdb.error:
            print "No such variable or command"

    #### help 

    def help_cd(self):
        print 'CD - Change the current file directory.'
        print 'Usage: cd [<directory-name>]'
        print 'Examples: cd mydir'
        print '          cd /zoo/mammals'
        print '          cd ../reptiles'
        print '          cd'


    def help_change_file(self):
        print 'CHANGE-FILE / CF - Change the current file to be the named one.'
        print '            If file is in the internal list of open files,'
        print '            make it current, otherwise, open it.'
        print '            If mode is "r", open file read only.'
        print '            If mode is "a", open file read/write.'
        print '            If mode is "w", overwrite any existing file.'
        print '            Mode defaults to "a".'
        print '            Alias defaults to "fd", d is an increasing decimal number.'
#        print '            Type is the file type (options are determined by output'
#        print '            spokes you have installed).'
        print 'Usage: change-file <filename> [<mode> [<alias> [<type>]]]'
        print 'Examples: change-file foo'
        print '          change-file foo.s00 r'
        print '          change-file foo.s00 w'
        print '          change-file foo.s00 w foo'
#        print '          change-file foo.s00 w bar pdb'
    help_cf = help_change_file



if __name__ == '__main__':
    v = Viewer()
    argv = sys.argv
    if len(argv) > 1:
        v.do_cf(argv[1])
    v.cmdloop();

