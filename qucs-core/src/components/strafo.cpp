/*
 * strafo.cpp - symmetrical trafo class implementation
 *

void strafo::initTR (void) {
  initAC ();

}

// properties
PROP_REQ [] = {
  { "T1", PROP_REAL, { 1, PROP_NO_STR }, PROP_POS_RANGE },
  { "T2", PROP_REAL, { 1, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  PROP_NO_PROP };
struct define_t strafo::cirdef =
  { "sTr", 6, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
