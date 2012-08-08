/*
 * trafo.cpp - trafo class implementation
 *

void trafo::initTR (void) {
  initAC ();

}

// properties
PROP_REQ [] = {
  { "T", PROP_REAL, { 1, PROP_NO_STR }, PROP_POS_RANGE }, PROP_NO_PROP };
PROP_OPT [] = {
  PROP_NO_PROP };
struct define_t trafo::cirdef =
  { "Tr", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
