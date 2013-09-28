#
# PROGRAM-FIN.MP - define the actions to complete a
#                - multi-platform configuration of PACT
#
# include "cpyright.h"
#

Run {
   write/package         $Log $ScrDir
   write/cfg.db          $Log $ScrDir
   write/scconfig.h      $Log $ScrDir
}

Run {
   write/configured      $Log $ScrDir $CONFIG_FILE
   write/makefile.mp     $Log $ScrDir
}
