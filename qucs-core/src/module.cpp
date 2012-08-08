/*
 * module.cpp - module class implementation
 *
 * Copyright (C) 2008, 2009, 2010 Stefan Jahn <stefan@lkcc.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.  
 *

  REGISTER_CIRCUIT (hicumL0V1p3);

  REGISTER_CIRCUIT (hicumL2V2p23);
  REGISTER_CIRCUIT (hicumL2V2p24);
  REGISTER_CIRCUIT (photodiode);
  REGISTER_CIRCUIT (phototransistor);
  REGISTER_CIRCUIT (nigbt);
  REGISTER_CIRCUIT (dff_SR);
  REGISTER_CIRCUIT (tff_SR);
  REGISTER_CIRCUIT (jkff_SR);
  REGISTER_CIRCUIT (gatedDlatch);
  REGISTER_CIRCUIT (logic_1);
  REGISTER_CIRCUIT (logic_0);
  REGISTER_CIRCUIT (mux2to1);
  REGISTER_CIRCUIT (mux4to1);
  REGISTER_CIRCUIT (mux8to1);
  REGISTER_CIRCUIT (DLS_nto1);
  REGISTER_CIRCUIT (DLS_1ton);
  REGISTER_CIRCUIT (andor4x2);
  REGISTER_CIRCUIT (andor4x3);
  REGISTER_CIRCUIT (andor4x4);
  REGISTER_CIRCUIT (dmux2to4);
  REGISTER_CIRCUIT (dmux3to8);
  REGISTER_CIRCUIT (dmux4to16);
  REGISTER_CIRCUIT (ha1b);
  REGISTER_CIRCUIT (fa1b);
  REGISTER_CIRCUIT (fa2b);
  REGISTER_CIRCUIT (pad2bit);
  REGISTER_CIRCUIT (pad3bit);
  REGISTER_CIRCUIT (pad4bit);
  REGISTER_CIRCUIT (binarytogrey4bit);
  REGISTER_CIRCUIT (greytobinary4bit);
  REGISTER_CIRCUIT (comp_1bit);
  REGISTER_CIRCUIT (comp_2bit);
  REGISTER_CIRCUIT (comp_4bit);
  REGISTER_CIRCUIT (hpribin4bit);

  // analyses
  REGISTER_ANALYSIS (dcsolver);
  REGISTER_ANALYSIS (acsolver);
  REGISTER_ANALYSIS (spsolver);
  REGISTER_ANALYSIS (trsolver);
  REGISTER_ANALYSIS (hbsolver);
  REGISTER_ANALYSIS (parasweep);
}

// Global module unregistration.
void module::unregisterModules (void) {
  qucs::hashiterator<module> it;
  for (it = qucs::hashiterator<module> (modules); *it; ++it) {
    delete it.currentVal ();
  }
  modules.clear ();
}

#if DEBUG
// header prefix
static const char * def_prefix =
"/*\n"
" * qucsdefs.h - netlist definitions for the Qucs netlists\n"
" *\n"
" * This is free software; you can redistribute it and/or modify\n"
" * it under the terms of the GNU General Public License as published by\n"
" * the Free Software Foundation; either version 2, or (at your option)\n"
" * any later version.\n"
" * \n"
" */\n"
"\n"
"#ifndef __QUCSDEFS_H__\n"
"#define __QUCSDEFS_H__\n";

// header suffix
static const char * def_suffix =
"\n"
"#endif /* __QUCSDEFS_H__ */\n";

// start of list of definitions
static const char * def_start =
"\n"
"// List of available components.\n"
"struct define_t qucs_definition_available[] =\n";

// end of list entry
static const char * def_stop =
"\n"
"static struct define_t def_End = {\n"
"  ((char *) 0), -1, 1, 0, 0, req_Def, opt_Def };\n";

// Returns a compilable C-code string made from the given string.
static char * printstr (const char * str) {
  static char txt[256];
  int nostr = (str == PROP_NO_STR);
  sprintf (txt, "%s%s%s",
	   (str && !nostr) ? "\"" : "",
	   str ? nostr ? "((char *) -1)" : str : "((char *) 0)",
	   (str && !nostr) ? "\"" : "");
  return txt;
}

// Prints a property list as compilable C-code.
static void printprop (const char * type, const char * prefix,
		       struct property_t * prop) {
  const char * key;
  const char ** str;
  const char * txt;
  fprintf (stdout, "static struct property_t %s_%s[] = {\n", prefix, type);
  do {
    key = prop->key;
    fprintf (stdout, "  { %s, %d, ", printstr (key), prop->type);
    fprintf (stdout, "{ %g, %s }, ", prop->defaultval.d,
	     printstr (prop->defaultval.s));
    fprintf (stdout, "{ '%c', %g, %g, '%c',\n",
	     prop->range.il, prop->range.l, prop->range.h, prop->range.ih);
    fprintf (stdout, "    {");
    str = prop->range.str;
    do {
      txt = *str;
      fprintf (stdout, " %s", printstr (txt));
      if (txt) fprintf (stdout, ",");
      str++;
    }
    while (txt != NULL);
    fprintf (stdout, " } } }");
    if (key) fprintf (stdout, ",");
    fprintf (stdout, "\n");
    prop++;
  }
  while (key != NULL);
  fprintf (stdout, "};\n");
}

/* The function emits a complete list of the registered component
   definitions as compilable C-code. */
void module::print (void) {
  fprintf (stdout, def_prefix);
  qucs::hashiterator<module> it;
  for (it = qucs::hashiterator<module> (modules); *it; ++it) {
    module * m = it.currentVal ();
    struct define_t * def = m->definition;
    fprintf (stdout, "\n");
    printprop (def->type, "req", def->required);
    fprintf (stdout, "\n");
    printprop (def->type, "opt", def->optional);
    fprintf (stdout, "\n");
    fprintf (stdout, "static struct define_t def_%s = {\n", def->type);
    fprintf (stdout, "  %s, %d, %d, %d, %d, req_%s, opt_%s };\n",
	     printstr (def->type), def->nodes, def->action, def->substrate,
	     def->nonlinear, def->type, def->type);
  }
  fprintf (stdout, def_stop);
  fprintf (stdout, def_start);
  fprintf (stdout, "{\n");
  for (it = qucs::hashiterator<module> (modules); *it; ++it) {
    module * m = it.currentVal ();
    struct define_t * def = m->definition;
    fprintf (stdout, "  def_%s,\n", def->type);
  }
  fprintf (stdout, "  def_End\n");
  fprintf (stdout, "};\n");
  fprintf (stdout, def_suffix);
}
#endif /* DEBUG */
