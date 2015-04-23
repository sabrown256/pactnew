;
; PDBVTST.SCM - pdbview test script
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; include "cpyright.h"
;

; note: pdbview.log will be empty because loaded commands are not logged
(unlink "pdbview.log" "pdbvtst.log")
(command-log on)
(command-log pdbvtst.log)
(command-log off)
(system "ls pdbview.log pdbvtst.log")
(unlink "pdbview.log" "pdbvtst.log")
(help)
(help menu)
(help log)
(lf)
(cf f0)
(file)
(file t)
(cf foo w)
(n-entries)
(lf)
(cf file.pdb)
(n-entries)
(lf)
(file)
(file t)
(menu)
(types)
(struct PM_mapping)
(ls)
(ls * Directory)
(ls /dir1)
(ls dir1 double)
(varprint x)
(pwd)
(cd dir1)
(pwd)
(menu)
(cd ..)
(pwd)
(cd /dir1)
(pwd)
(cd /)
(pwd)
(cd ./dir1)
(pwd)
(cd .)
(pwd)
(ls)
(ls ..)
(ls ../x*)
(ls i*)
(varset! /x[1] 2)
(format double %12.5e)
(varprint ../x[1])
(format default)
(varprint ../dir1/../x)
(format float %12.5e)
(format double %12.5e)
(format integer %12d)
(format long %12d)
(desc npts0)
(desc a)
(desc jkl)
(desc jkl.k)
(varprint jkl.k)
(varset! jkl.k 5)
(varprint /jkl_link)
(varset! jkl.k 2)
(varprint a)
(set line-length 1)
(set bits-precision 10)
(varprint a)
(set array-length 1)
(set decimal-precision 10)
(varprint a)
(ls-attr)
(cm foo 1 2)
(cm f1 3)
(cm f1 4 5)
(cm foo *)
(cd)
(cp x z foo)
(cd dir1)
(cp abc jkl foo)
(cd)
(cp * foo)
(cf foo)
(file)
(file t)
(types)
(ls)
(menu Domain*)
(ls-attr)
(n-entries)
(pl 1 Mapping1)
(dl 1 2)
(pl Image0)
(change-palette bw)
(wu)
(change-palette rainbow)
(wu)
(change-palette spectrum)
(wu)
(hcv color 8)
(dl 1)
(pl 4 curve0001)
(mkdir dir2)
(pl /curve0001)
(hc)
(clw)
(pm 1)
(mode indent)
(mode type)
(mode recursive)
(pm Image0)
(mode tree)
(mode no-type)
(mode iterative)
(pm 5)
(lf)
(close)
(lf)
(close f2)
(lf)
(cf file.pdb)
(lf)
(close file.pdb)
(lf)

(cf file.pdb)
(cd dir1)
(def foo (data-reference 3 file.pdb))
(cf f0)
(pl 1)

(cf foo w)
(cf file.pdb)
(cd)
(cp x foo)
(cd dir1)
(cm foo 1)
(cf foo)
(varprint x)
(clv)
(pl 1)
(close foo)

(cf foo w)
(cf file.pdb)
(cm foo 1)
(cf foo)
(mkdir dir2)
(menu)
(cd dir2)
(menu)
(close foo)

(cf foo w)
(mkdir dir2)
(cd dir2)
(cf file.pdb)
(cm foo 1)
(cf foo)
(menu)
(cd)
(menu)
(close foo)

(cf file.pdb)
(cf foo w)
(cf file.pdb)
(cp Mapping0 foo)
(cf foo)
(menu)
(close foo)

(cf foo w)
(cf file.pdb)
(cm foo 1)
(cp Image0 foo)
(cf foo)
(menu)

(cf nil)
(file t)
(n-entries)

(cf file.pdb)
(cm f0 3 4)
(cd)
(cp x f0)
(cf f0)
(clv)
(pl 1)
(menu)
(varprint x)

(end)