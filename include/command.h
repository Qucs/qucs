#ifndef QUCS_COMMAND_H
#define QUCS_COMMAND_H

#include "object.h"
#include "element_list.h"

#define INTERFACE
#define CMD Command
// #define CARD_LIST qucs::ElementList // really?
#define CS istream_t

#include "c_comand.h"

#undef CMD
#undef CARD_LIST
#undef CS

#endif
