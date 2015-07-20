# Qucs Patchset for BSIM 6.1.0

## Description

This directory contains the patches applied to the BSIM 6.1.0 Verilog-A model to make it useable with Qusc and Qucsator.

To simplify the build system the models added to the Qucs repository are already patched. The patches are provided here mostly to inform users of what has been done to the original model (in case only the tarball is available).

The original `bsim6.va` model can be downloaded from:

 - <http://www-device.eecs.berkeley.edu/bsim/?page=BSIM6>

## Scripts in this directory:

 - `apply_patches.sh` : apply the patches on top of the original `bsim6.va` file. The original file is ecpected to be in the same directory as the script (and the patches). Two files are generated: `bsim6v10nMOS.va` and a `bsim6v10pMOS.va`.

 - `convert_comments.py` : it reads `bsim6.va` and tries to promote ordinary code comments after `parameter` declarations into attributes. Attributes can be used as documentation strings in the simulator. This script was used to generate one of the patches in this set.
