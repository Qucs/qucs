
#include "object.h"
#include "schematic_model.h"

#define INTERFACE
#define CMD Command
#define CKT_BASE Object
#define CARD_LIST SchematicModel // really?
#define CS istream_t

#include "c_comand.h"

#undef Cmd
#undef CKT_BASE
#undef CARD_LIST
#undef CS
