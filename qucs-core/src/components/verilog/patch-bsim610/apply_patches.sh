#!/bin/bash

##
# This script applies patches to the original BSIM 6.1.0 model.
# * the original 'bsim6.va' is expected in the current directory.
##

# Place original source in this directory
BSIM6=bsim6.va

# Patch model into NMOS and PMOS
NMOS=bsim6v10nMOS.va
PMOS=bsim6v10pMOS.va

for BSIM6mod in $NMOS $PMOS
do
  echo "==> Patching $BSIM6mod"

  # keep original
  cp $BSIM6 $BSIM6mod

  ## Patches common to NMOS and PMOS
  patch $BSIM6mod 0001-bsim6-Fix-T0-variable-name-collision.patch
  patch $BSIM6mod 0002-bsim6-Use-niminal-node-count.patch
  patch $BSIM6mod 0003-bsim6-Fix-sbulk-dbulk-defined.patch
  patch $BSIM6mod 0004-bsim6-Remove-function-from-default-value.patch
  patch $BSIM6mod 0005-bsim6-Add-modification-note.patch
  patch $BSIM6mod 0006-bsim6-Add-macro-for-parameter-attribute.patch
  patch $BSIM6mod 0007-bsim6-Promote-comments-to-parameter-attributes.patch

  ## Patches specific to NMOS
  if [ "$BSIM6mod" = "$NMOS" ]
  then
    patch $BSIM6mod 0008-bsim6NMOS-Name-module-bsim6v10nMOS.patch
    patch $BSIM6mod 0009-bsim6NMOS-Set-ternary-operator-default-to-NMOS.patch
    patch $BSIM6mod 0010-bsim6NMOS-Set-device-TYPE-to-nMOS.patch
  fi

  ## Patches specific to PMOS
  if [ "$BSIM6mod" = "$PMOS" ]
  then
    patch $BSIM6mod 0008-bsim6PMOS-Name-module-bsim6v10pMOS.patch
    patch $BSIM6mod 0009-bsim6PMOS-Set-ternary-operator-default-to-PMOS.patch
    patch $BSIM6mod 0010-bsim6PMOS-Set-device-TYPE-to-pMOS.patch
  fi

  echo "==> Patches applied to [$BSIM6], result in [$BSIM6mod]"

done








