/*
 * qucs_producer.cpp - the Qucs netlist producer
 *

    { "Type", "is", "it_mod", "mcf", "mcr", "vef", "ver", "aver", "iqf",
      "fiqf", "iqr", "iqfh", "tfh", "ahq", "ibes", "mbe", "ires", "mre",
      "ibcs", "mbc", "cje0", "vde", "ze", "aje", "vdedc", "zedc", "ajedc",
      "t0", "dt0h", "tbvl", "tef0", "gte", "thcs", "ahc", "tr", "rci0",
      "vlim", "vpt", "vces", "cjci0", "vdci", "zci", "vptci", "cjcx0", "vdcx",
      "zcx", "vptcx", "fbc", "rbi0", "vr0e", "vr0c", "fgeo", "rbx", "rcx",
      "re", "itss", "msf", "iscs", "msc", "cjs0", "vds", "zs", "vpts",
      "cbcpar", "cbepar", "eavl", "kavl", "kf", "af", "vgb", "vge", "vgc",
      "vgs", "f1vg", "f2vg", "alt0", "kt0", "zetact", "zetabet", "zetaci",
      "alvs", "alces", "zetarbi", "zetarbx", "zetarcx", "zetare", "zetaiqf",
      "alkav", "aleav", "zetarth", "tef_temp", "zetaver", "zetavgbe", "dvgbe",
      "aliqfh", "kiqfh", "flsh", "rth", "cth", "tnom", "dt", "Temp", NULL },

    NULL,
    "1 0 0 8 -26 0 0",
    NULL
  },
  /* hicum/l2 v2.22 bipolar transistor */
  { "hic2_full", "hic2_full", "Q", 5,
    { "c10", "qp0", "ich", "hfe", "hfc", "hjei", "hjci", "ibeis", "mbei",
      "ireis", "mrei", "ibeps", "mbep", "ireps", "mrep", "mcf", "tbhrec",
      "ibcis", "mbci", "ibcxs", "mbcx", "ibets", "abet", "tunode", "favl",
      "qavl", "alfav", "alqav", "rbi0", "rbx", "fgeo", "fdqr0", "fcrbi",
      "fqi", "re", "rcx", "itss", "msf", "iscs", "msc", "tsf", "rsu", "csu",
      "cjei0", "vdei", "zei", "ajei", "cjep0", "vdep", "zep", "ajep",
      "cjci0", "vdci", "zci", "vptci", "cjcx0", "vdcx", "zcx", "vptcx",
      "fbcpar", "fbepar", "cjs0", "vds", "zs", "vpts", "t0", "dt0h", "tbvl",
      "tef0", "gtfe", "thcs", "ahc", "fthc", "rci0", "vlim", "vces", "vpt",
      "tr", "cbepar", "cbcpar", "alqf", "alit", "flnqs", "kf", "af", "cfbe",
      "latb", "latl", "vgb", "alt0", "kt0", "zetaci", "alvs", "alces",
      "zetarbi", "zetarbx", "zetarcx", "zetare", "zetacx", "vge", "vgc",
      "vgs", "f1vg", "f2vg", "zetact", "zetabet", "alb", "flsh", "rth", "cth",
      "flcomp", "tnom", "dt", "Temp", NULL },
    NULL,
    "1 0 0 8 -26 0 0",
    NULL
  },
  /* hicum/l2 v2.1 bipolar transistor */
  { "hicumL2V2p1", "hicumL2V2p1", "Q", 5,
    { "c10", "qp0", "ich", "hfe", "hfc", "hjei", "hjci", "ibeis",
      "mbei", "ireis", "mrei", "ibeps", "mbep", "ireps", "mrep", "mcf",
      "ibcis", "mbci", "ibcxs", "mbcx", "ibets", "abet", "favl", "qavl",
      "alfav", "alqav", "rbi0", "rbx", "fgeo", "fdqr0", "fcrbi", "fqi", "re",
      "rcx", "itss", "msf", "iscs", "msc", "tsf", "rsu", "csu", "cjei0",
      "vdei", "zei", "aljei", "cjep0", "vdep", "zep", "aljep", "cjci0",
      "vdci", "zci", "vptci", "cjcx0", "vdcx", "zcx", "vptcx", "fbc", "cjs0",
      "vds", "zs", "vpts", "t0", "dt0h", "tbvl", "tef0", "gtfe", "thcs",
      "alhc", "fthc", "rci0", "vlim", "vces", "vpt", "tr", "ceox", "ccox",
      "alqf", "alit", "kf", "af", "krbi", "latb", "latl", "vgb", "alt0",
      "kt0", "zetaci", "zetacx", "alvs", "alces", "zetarbi", "zetarbx",
      "zetarcx", "zetare", "alb", "rth", "cth", "tnom", "dt", "Temp", NULL },
    NULL,
    "1 0 0 8 -26 0 0",
    NULL
  },
  /* hicum/l2 v2.23 bipolar transistor */
  { "hicumL2V2p23", "hicumL2V2p23", "Q", 5,
    { "c10", "qp0", "ich", "hfe", "hfc", "hjei", "hjci", "ibeis", "mbei",
      "ireis", "mrei", "ibeps", "mbep", "ireps", "mrep", "mcf", "tbhrec",
      "ibcis", "mbci", "ibcxs", "mbcx", "ibets", "abet", "tunode", "favl",
      "qavl", "alfav", "alqav", "rbi0", "rbx", "fgeo", "fdqr0", "fcrbi",
      "fqi", "re", "rcx", "itss", "msf", "iscs", "msc", "tsf", "rsu", "csu",
      "cjei0", "vdei", "zei", "ajei", "cjep0", "vdep", "zep", "ajep",
      "cjci0", "vdci", "zci", "vptci", "cjcx0", "vdcx", "zcx", "vptcx",
      "fbcpar", "fbepar", "cjs0", "vds", "zs", "vpts", "t0", "dt0h", "tbvl",
      "tef0", "gtfe", "thcs", "ahc", "fthc", "rci0", "vlim", "vces", "vpt",
      "tr", "cbepar", "cbcpar", "alqf", "alit", "flnqs", "kf", "af", "cfbe",
      "latb", "latl", "vgb", "alt0", "kt0", "zetaci", "alvs", "alces",
      "zetarbi", "zetarbx", "zetarcx", "zetare", "zetacx", "vge", "vgc",
      "vgs", "f1vg", "f2vg", "zetact", "zetabet", "alb", "flsh", "rth", "cth",
      "flcomp", "tnom", "dt", "Temp", NULL },
    NULL,
    "1 0 0 8 -26 0 0",
    NULL
  },
  /* hicum/l2 v2.24 bipolar transistor */
  { "hicumL2V2p24", "hicumL2V2p24", "Q", 5,
    { "c10", "qp0", "ich", "hfe", "hfc", "hjei", "hjci", "ibeis", "mbei",
      "ireis", "mrei", "ibeps", "mbep", "ireps", "mrep", "mcf", "tbhrec",
      "ibcis", "mbci", "ibcxs", "mbcx", "ibets", "abet", "tunode", "favl",
      "qavl", "alfav", "alqav", "rbi0", "rbx", "fgeo", "fdqr0", "fcrbi",
      "fqi", "re", "rcx", "itss", "msf", "iscs", "msc", "tsf", "rsu", "csu",
      "cjei0", "vdei", "zei", "ajei", "cjep0", "vdep", "zep", "ajep",
      "cjci0", "vdci", "zci", "vptci", "cjcx0", "vdcx", "zcx", "vptcx",
      "fbcpar", "fbepar", "cjs0", "vds", "zs", "vpts", "t0", "dt0h", "tbvl",
      "tef0", "gtfe", "thcs", "ahc", "fthc", "rci0", "vlim", "vces", "vpt",
      "tr", "cbepar", "cbcpar", "alqf", "alit", "flnqs", "kf", "af", "cfbe",
      "latb", "latl", "vgb", "alt0", "kt0", "zetaci", "alvs", "alces",
      "zetarbi", "zetarbx", "zetarcx", "zetare", "zetacx", "vge", "vgc",
      "vgs", "f1vg", "f2vg", "zetact", "zetabet", "alb", "flsh", "rth", "cth",
      "flcomp", "tnom", "dt", "Temp", NULL },
    NULL,
    "1 0 0 8 -26 0 0",
    NULL
  },
  /* junction FET */
  { "JFET", "JFET", "J", 3,
    { "Type", "Vt0", "Beta", "Lambda", "Rd", "Rs", "Is", "N", "Isr", "Nr",
      "Cgs", "Cgd", "Pb", "Fc", "M", "Kf", "Af", "Ffe", "Temp", "Xti",
      "Vt0tc", "Betatce", "Tnom", "Area", NULL },
    NULL,
    "1 190 140 30 -49 0 0",
    NULL
  },
  /* MOSFET */
  { "MOSFET", "_MOSFET", "M", 4,
    { "Type", "Vt0", "Kp", "Gamma", "Phi", "Lambda", "Rd", "Rs", "Rg", "Is",
      "N", "W", "L", "Ld", "Tox", "Cgso", "Cgdo", "Cgbo", "Cbd", "Cbs", "Pb",
      "Mj", "Fc", "Cjsw", "Mjsw", "Tt", "Nsub", "Nss", "Tpg", "Uo", "Rsh",
      "Nrd", "Nrs", "Cj", "Js", "Ad", "As", "Pd", "Ps", "Kf", "Af", "Ffe",
      "Temp", "Tnom", NULL },
    NULL,
    "1 0 0 8 -26 0 0",
    NULL
  },
  { NULL, NULL, NULL, 0, { NULL }, NULL, NULL, NULL }
};

/* Looks through the list of available Qucs devices.  Returns NULL if
   there is no such device. */
static struct device_t * qucslib_find_device (char * type) {
  struct device_t * dev;
  for (dev = qucs_devices; dev->ntype; dev++) {
    if (!strcmp (dev->ntype, type))
      return dev;
  }
  return NULL;
}

/* This function tries to find the given property in the key/value
   pairs of the given netlist entry and returns NULL if there is no
   such property. */
static struct pair_t *
qucslib_find_prop (struct definition_t * def, const char * key) {
  struct pair_t * pair;
  for (pair = def->pairs; pair; pair = pair->next) {
    if (!strcmp (pair->key, key))
      return pair;
  }
  return NULL;
}

/* The function outputs a device library entry in the Qucs schematic
   data format. */
static void qucslib_list_device (struct definition_t * def) {
  struct device_t * dev;
  if (!(dev = qucslib_find_device (def->type))) return;
  struct pair_t * pair;
  char txt[1024];
  
  sprintf (txt, "\n<Component %s>\n", def->instance[0] == dev->stype[0] ?
	   &def->instance[1] : def->instance);
  fprintf (qucs_out, txt);
  fprintf (qucs_out, "  <Description>\n");
  fprintf (qucs_out, "  </Description>\n");
  fprintf (qucs_out, "  <Model>\n");
  fprintf (qucs_out, "    <%s %s_ %s", dev->ltype,
	   netlist_instance (def->instance), dev->coords);
  for (int i = 0; dev->props[i]; i++) {
    if ((pair = qucslib_find_prop (def, dev->props[i])) != NULL) {
      fprintf (qucs_out, " \"");
      netlist_list_value (pair->value);
      fprintf (qucs_out, "\" 0");
    }
  }
  fprintf (qucs_out, ">\n");
  fprintf (qucs_out, "  </Model>\n");
  fprintf (qucs_out, "</Component>\n");
}

/* This function is the overall Qucs library producer. */
void qucslib_producer (void) {
  struct definition_t * def;
  fprintf (qucs_out, "<Qucs Library " PACKAGE_VERSION " \"Generic\">\n");
  for (def = device_root; def; def = def->next) {
    qucslib_list_device (def);
  }
}

/* This function is the Qucs dataset producer for VCD files. */
void qucsdata_producer_vcd (void) {
  struct dataset_variable * ds;
  struct dataset_value * dv;
  fprintf (qucs_out, "<Qucs Dataset " PACKAGE_VERSION ">\n");
  for (ds = dataset_root; ds; ds = ds->next) {
    if (!ds->output || ds->type == DATA_UNKNOWN)
      continue;
    if (ds->type == DATA_INDEPENDENT)
      fprintf (qucs_out, "<indep %s %d>\n", ds->ident, ds->size);
    else if (ds->type == DATA_DEPENDENT)
      fprintf (qucs_out, "<dep %s.%s %s>\n", ds->ident, ds->isreal ? "R" : "X",
	       ds->dependencies);
    for (dv = ds->values; dv; dv = dv->next) {
      fprintf (qucs_out, "  %s\n", dv->value);
    }
    if (ds->type == DATA_INDEPENDENT)
      fprintf (qucs_out, "</indep>\n");
    else if (ds->type == DATA_DEPENDENT)
      fprintf (qucs_out, "</dep>\n");
  }
}

/* This function is the Qucs dataset producer. */
void qucsdata_producer (dataset * data) {
  data->print ();
}
