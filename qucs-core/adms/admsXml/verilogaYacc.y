/* File automatically created by ./mkgrammar.pl */

%{
#define YYDEBUG 1
#define YYPARSE_PARAM myadmsmain
#include "admsVeriloga.h"

#define NEWVARIABLE(l) p_variableprototype myvariableprototype=adms_variableprototype_new(gModule,l,(p_adms)gBlockList->data);

inline static void   Y (p_yaccval myyaccval,p_adms myusrdata) {myyaccval->_usrdata=myusrdata;}
inline static p_adms YY(p_yaccval myyaccval)                  {return myyaccval->_usrdata;}
static char* gNatureAccess=NULL;
static p_number gNatureAbsTol=NULL;
static char* gNatureUnits=NULL;
static char* gNatureidt=NULL;
static char* gNatureddt=NULL;
static p_discipline gDiscipline=NULL;
static p_module gModule=NULL;
static p_analogfunction gAnalogfunction=NULL;
static p_module gInstanceModule=NULL;
static p_node gGND=NULL;
static p_source gSource=NULL;
static p_lexval gLexval=NULL;
static p_contribution gContribution=NULL;
static admse gVariableType=admse_real;
static admse gNodeDirection;
int uid=0;
static p_slist gVariableDeclarationList=NULL;
static p_slist gInstanceVariableList=NULL;
static p_slist gTerminalList=NULL;
static p_slist gBranchAliasList=NULL;
static p_slist gRangeList=NULL;
static p_slist gNodeList=NULL;
static p_slist gAttributeList=NULL;
static p_slist gGlobalAttributeList=NULL;
static p_slist gBlockList=NULL;
static p_slist gBlockVariableList=NULL;
static p_branchalias gBranchAlias=NULL;

static void adms_veriloga_message_fatal_continue(const p_lexval mytoken)
{
  adms_message_fatal_continue(("[%s:%i:%i]: at '%s':\n",mytoken->_f,mytoken->_l,mytoken->_c,mytoken->_string))
}
static void adms_veriloga_message_fatal (const char* message,const p_lexval mytoken)
{
  adms_veriloga_message_fatal_continue(mytoken);
  adms_message_fatal((message))
}
/*
inline static p_variableprototype variableprototype_recursive_lookup_by_id (p_adms myadms,p_lexval mylexval)
{
  if(myadms==(p_adms)gModule)
    return adms_module_list_variable_lookup_by_id(gModule,gModule,mylexval,(p_adms)gModule);
  else if(myadms==(p_adms)gAnalogfunction)
    return adms_analogfunction_list_variable_lookup_by_id(gAnalogfunction,gModule,mylexval,(p_adms)gAnalogfunction);
  else
  {
    p_slist l;
    for(l=((p_block)myadms)->_variable;l;l=l->next)
      if(!strcmp(((p_variableprototype)l->data)->_lexval->_string,mylexval->_string))
        return (p_variableprototype)l->data;
    return variableprototype_recursive_lookup_by_id((p_adms)((p_block)myadms)->_block,mylexval);
  }
}
*/
inline static p_variable variable_recursive_lookup_by_id (p_adms myadms,p_lexval mylexval)
{
  if(myadms==(p_adms)gModule)
  {
    p_variable myvariable=NULL;
    p_variableprototype myvariableprototype;
    if((myvariableprototype=adms_module_list_variable_lookup_by_id(gModule,gModule,mylexval,(p_adms)gModule)))
    {
      myvariable=adms_variable_new(myvariableprototype);
      adms_slist_push(&myvariableprototype->_instance,(p_adms)myvariable);
    }
    return myvariable;
  }
  else if(myadms==(p_adms)gAnalogfunction)
  {
    p_variable myvariable=NULL;
    p_variableprototype myvariableprototype;
    if((myvariableprototype=adms_analogfunction_list_variable_lookup_by_id(gAnalogfunction,gModule,mylexval,(p_adms)gAnalogfunction)))
    {
      myvariable=adms_variable_new(myvariableprototype);
      adms_slist_push(&myvariableprototype->_instance,(p_adms)myvariable);
    }
    return myvariable;
  }
  else
  {
    p_slist l;
    for(l=((p_block)myadms)->_variable;l;l=l->next)
      if(!strcmp(((p_variableprototype)l->data)->_lexval->_string,mylexval->_string))
      {
        p_variableprototype myvariableprototype=(p_variableprototype)l->data;
        p_variable myvariable=adms_variable_new(myvariableprototype);
        adms_slist_push(&myvariableprototype->_instance,(p_adms)myvariable);
        return myvariable;
      }
    return variable_recursive_lookup_by_id((p_adms)((p_block)myadms)->_block,mylexval);
  }
}
static p_nature lookup_nature(const char *myname)
{
  p_slist l;
  for(l=root()->_nature;l;l=l->next)
    if(!strcmp(((p_nature)l->data)->_name,myname))
      return (p_nature)l->data;
  return NULL;
}

%}

%left PREC_IF_THEN
%left tk_else

%union
{
  p_lexval _lexval;
  p_yaccval _yaccval;
}

%token <_lexval> tk_from
%token <_lexval> tk_branch
%token <_lexval> tk_number
%token <_lexval> tk_nature
%token <_lexval> tk_aliasparameter
%token <_lexval> tk_output
%token <_lexval> tk_anystring
%token <_lexval> tk_dollar_ident
%token <_lexval> tk_or
%token <_lexval> tk_aliasparam
%token <_lexval> tk_if
%token <_lexval> tk_analog
%token <_lexval> tk_parameter
%token <_lexval> tk_discipline
%token <_lexval> tk_char
%token <_lexval> tk_anytext
%token <_lexval> tk_for
%token <_lexval> tk_while
%token <_lexval> tk_real
%token <_lexval> tk_op_shr
%token <_lexval> tk_case
%token <_lexval> tk_potential
%token <_lexval> tk_endcase
%token <_lexval> tk_inf
%token <_lexval> tk_exclude
%token <_lexval> tk_ground
%token <_lexval> tk_endmodule
%token <_lexval> tk_begin
%token <_lexval> tk_enddiscipline
%token <_lexval> tk_domain
%token <_lexval> tk_ident
%token <_lexval> tk_op_shl
%token <_lexval> tk_string
%token <_lexval> tk_integer
%token <_lexval> tk_module
%token <_lexval> tk_endattribute
%token <_lexval> tk_else
%token <_lexval> tk_end
%token <_lexval> tk_inout
%token <_lexval> tk_and
%token <_lexval> tk_bitwise_equr
%token <_lexval> tk_default
%token <_lexval> tk_function
%token <_lexval> tk_input
%token <_lexval> tk_beginattribute
%token <_lexval> tk_endnature
%token <_lexval> tk_endfunction
%token <_lexval> tk_flow

%type <_yaccval> R_admsParse
%type <_yaccval> R_l.admsParse
%type <_yaccval> R_s.admsParse
%type <_yaccval> R_discipline_member
%type <_yaccval> R_discipline_name
%type <_yaccval> R_l.discipline_assignment
%type <_yaccval> R_s.discipline_assignment
%type <_yaccval> R_discipline.naturename
%type <_yaccval> R_nature_member
%type <_yaccval> R_l.nature_assignment
%type <_yaccval> R_s.nature_assignment
%type <_yaccval> R_d.attribute.0
%type <_yaccval> R_d.attribute
%type <_yaccval> R_l.attribute
%type <_yaccval> R_s.attribute
%type <_yaccval> R_d.module
%type <_yaccval> R_modulebody
%type <_yaccval> R_netlist
%type <_yaccval> R_l.instance
%type <_yaccval> R_d.terminal
%type <_yaccval> R_l.terminal.0
%type <_yaccval> R_l.terminal
%type <_yaccval> R_s.terminal
%type <_yaccval> R_l.declaration
%type <_yaccval> R_s.declaration.withattribute
%type <_yaccval> R_d.attribute.global
%type <_yaccval> R_s.declaration
%type <_yaccval> R_d.node
%type <_yaccval> R_node.type
%type <_yaccval> R_l.terminalnode
%type <_yaccval> R_l.node
%type <_yaccval> R_s.terminalnode
%type <_yaccval> R_s.node
%type <_yaccval> R_d.branch
%type <_yaccval> R_l.branchalias
%type <_yaccval> R_s.branchalias
%type <_yaccval> R_s.branch
%type <_yaccval> R_d.analogfunction
%type <_yaccval> R_d.analogfunction.proto
%type <_yaccval> R_d.analogfunction.name
%type <_yaccval> R_l.analogfunction.declaration
%type <_yaccval> R_s.analogfunction.declaration
%type <_yaccval> R_l.analogfunction.input.variable
%type <_yaccval> R_l.analogfunction.output.variable
%type <_yaccval> R_l.analogfunction.inout.variable
%type <_yaccval> R_l.analogfunction.integer.variable
%type <_yaccval> R_l.analogfunction.real.variable
%type <_yaccval> R_variable.type
%type <_yaccval> R_d.variable.end
%type <_yaccval> R_l.parameter
%type <_yaccval> R_l.variable
%type <_yaccval> R_d.aliasparameter
%type <_yaccval> R_d.aliasparameter.token
%type <_yaccval> R_s.parameter
%type <_yaccval> R_s.variable
%type <_yaccval> R_s.parameter.name
%type <_yaccval> R_s.variable.name
%type <_yaccval> R_s.parameter.range
%type <_yaccval> R_l.interval
%type <_yaccval> R_s.interval
%type <_yaccval> R_d.interval
%type <_yaccval> R_interval.inf
%type <_yaccval> R_interval.sup
%type <_yaccval> R_analog
%type <_yaccval> R_analogcode
%type <_yaccval> R_l.expression
%type <_yaccval> R_analogcode.atomic
%type <_yaccval> R_analogcode.block
%type <_yaccval> R_analogcode.block.atevent
%type <_yaccval> R_l.analysis
%type <_yaccval> R_s.analysis
%type <_yaccval> R_d.block
%type <_yaccval> R_d.block.begin
%type <_yaccval> R_l.blockitem
%type <_yaccval> R_d.blockvariable
%type <_yaccval> R_l.blockvariable
%type <_yaccval> R_s.blockvariable
%type <_yaccval> R_d.contribution
%type <_yaccval> R_contribution
%type <_yaccval> R_source
%type <_yaccval> R_d.while
%type <_yaccval> R_d.for
%type <_yaccval> R_d.case
%type <_yaccval> R_l.case.item
%type <_yaccval> R_s.case.item
%type <_yaccval> R_s.instance
%type <_yaccval> R_instance.module.name
%type <_yaccval> R_l.instance.parameter
%type <_yaccval> R_s.instance.parameter
%type <_yaccval> R_s.assignment
%type <_yaccval> R_d.conditional
%type <_yaccval> R_s.expression
%type <_yaccval> R_l.enode
%type <_yaccval> R_s.function_expression
%type <_yaccval> R_expression
%type <_yaccval> R_e.conditional
%type <_yaccval> R_e.bitwise_equ
%type <_yaccval> R_e.bitwise_xor
%type <_yaccval> R_e.bitwise_or
%type <_yaccval> R_e.bitwise_and
%type <_yaccval> R_e.logical_or
%type <_yaccval> R_e.logical_and
%type <_yaccval> R_e.comp_equ
%type <_yaccval> R_e.comp
%type <_yaccval> R_e.bitwise_shift
%type <_yaccval> R_e.arithm_add
%type <_yaccval> R_e.arithm_mult
%type <_yaccval> R_e.unary
%type <_yaccval> R_e.atomic

%%
R_admsParse
        : R_l.admsParse
          {
          }
        ;
R_l.admsParse
        : R_s.admsParse
          {
          }
        | R_l.admsParse R_s.admsParse
          {
          }
        ;
R_s.admsParse
        : R_d.module
          {
          }
        | R_discipline_member
          {
          }
        | R_nature_member
          {
          }
        ;
R_discipline_member
        : tk_discipline R_discipline_name R_l.discipline_assignment tk_enddiscipline
          {
            adms_admsmain_list_discipline_prepend_once_or_abort(root(),gDiscipline);
            gDiscipline=NULL;
          }
        ;
R_discipline_name
        : tk_ident
          {
            char* mylexval1=((p_lexval)$1)->_string;
            gDiscipline=adms_discipline_new(mylexval1);
          }
        ;
R_l.discipline_assignment
        : R_s.discipline_assignment
          {
          }
        | R_l.discipline_assignment R_s.discipline_assignment
          {
          }
        ;
R_s.discipline_assignment
        : tk_potential R_discipline.naturename ';'
          {
            gDiscipline->_potential=(p_nature)YY($2);
          }
        | tk_flow R_discipline.naturename ';'
          {
            gDiscipline->_flow=(p_nature)YY($2);
          }
        | tk_domain tk_ident ';'
          {
            char* mylexval2=((p_lexval)$2)->_string;
            if(!strcmp(mylexval2,"discrete"))
              gDiscipline->_domain=admse_discrete;
            else if(!strcmp(mylexval2,"continuous"))
              gDiscipline->_domain=admse_continuous;
            else
             adms_veriloga_message_fatal("domain: bad value given - should be either 'discrete' or 'continuous'\n",$2);
          }
        ;
R_discipline.naturename
        : tk_ident
          {
            char* mylexval1=((p_lexval)$1)->_string;
            p_nature mynature=lookup_nature(mylexval1);
            if(!mynature)
              adms_veriloga_message_fatal("can't find nature definition\n",$1);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)mynature);
          }
        ;
R_nature_member
        : tk_nature tk_ident R_l.nature_assignment tk_endnature
          {
            char* mylexval2=((p_lexval)$2)->_string;
            p_nature mynature=NULL;
            if(gNatureAccess) 
              mynature=adms_admsmain_list_nature_prepend_by_id_once_or_abort(root(),gNatureAccess);
            else
             adms_veriloga_message_fatal("attribute 'access' in nature definition not found\n",$2);
            adms_nature_valueto_name(mynature,mylexval2);
            if(gNatureidt) 
              adms_nature_valueto_idt_name(mynature,gNatureidt);
            if(gNatureddt) 
              adms_nature_valueto_ddt_name(mynature,gNatureddt);
            if(gNatureUnits)
              mynature->_units=gNatureUnits;
            if(gNatureAbsTol)
              mynature->_abstol=gNatureAbsTol;
            gNatureAccess=NULL;
            gNatureAbsTol=NULL;
            gNatureUnits=NULL;
            gNatureidt=NULL;
            gNatureddt=NULL;
          }
        ;
R_l.nature_assignment
        : R_s.nature_assignment
          {
          }
        | R_l.nature_assignment R_s.nature_assignment
          {
          }
        ;
R_s.nature_assignment
        : tk_ident '=' tk_number ';'
          {
            if(!strcmp($1->_string,"abstol"))
            {
              if(gNatureAbsTol)
                adms_veriloga_message_fatal("nature attribute defined more than once\n",$1);
              gNatureAbsTol=adms_number_new($3);
            }
            else
             adms_veriloga_message_fatal("unknown nature attribute\n",$1);
          }
        | tk_ident '=' tk_number tk_ident ';'
          {
            char* mylexval4=((p_lexval)$4)->_string;
            admse myunit=admse_1;
            if(!strcmp($1->_string,"abstol"))
            {
              if(gNatureAbsTol)
                adms_veriloga_message_fatal("nature attribute defined more than once\n",$1);
              gNatureAbsTol=adms_number_new($3);
            }
            else
             adms_veriloga_message_fatal("unknown nature attribute\n",$1);
            if(0) {}
            else if(!strcmp(mylexval4,"E")) myunit=admse_E;
            else if(!strcmp(mylexval4,"P")) myunit=admse_P;
            else if(!strcmp(mylexval4,"T")) myunit=admse_T;
            else if(!strcmp(mylexval4,"G")) myunit=admse_G;
            else if(!strcmp(mylexval4,"M")) myunit=admse_M;
            else if(!strcmp(mylexval4,"k")) myunit=admse_k;
            else if(!strcmp(mylexval4,"h")) myunit=admse_h;
            else if(!strcmp(mylexval4,"D")) myunit=admse_D;
            else if(!strcmp(mylexval4,"d")) myunit=admse_d;
            else if(!strcmp(mylexval4,"c")) myunit=admse_c;
            else if(!strcmp(mylexval4,"m")) myunit=admse_m;
            else if(!strcmp(mylexval4,"u")) myunit=admse_u;
            else if(!strcmp(mylexval4,"n")) myunit=admse_n;
            else if(!strcmp(mylexval4,"A")) myunit=admse_A;
            else if(!strcmp(mylexval4,"p")) myunit=admse_p;
            else if(!strcmp(mylexval4,"f")) myunit=admse_f;
            else if(!strcmp(mylexval4,"a")) myunit=admse_a;
            else
              adms_veriloga_message_fatal("can not convert symbol to valid unit\n",$4);
            gNatureAbsTol->_scalingunit=myunit;
          }
        | tk_ident '=' tk_anystring ';'
          {
            char* mylexval3=((p_lexval)$3)->_string;
            if(!strcmp($1->_string,"units"))
            {
              if(gNatureUnits)
                adms_veriloga_message_fatal("nature attribute defined more than once\n",$1);
              gNatureUnits=adms_kclone(mylexval3);
            }
            else
             adms_veriloga_message_fatal("unknown nature attribute\n",$1);
          }
        | tk_ident '=' tk_ident ';'
          {
            char* mylexval3=((p_lexval)$3)->_string;
            if(!strcmp($1->_string,"access"))
            {
              if(gNatureAccess)
                adms_veriloga_message_fatal("nature attribute defined more than once\n",$1);
              gNatureAccess=adms_kclone(mylexval3);
            }
            else if(!strcmp($1->_string,"idt_nature"))
            {
              if(gNatureidt)
                adms_veriloga_message_fatal("idt_nature attribute defined more than once\n",$1);
              gNatureidt=adms_kclone(mylexval3);
            }
            else if(!strcmp($1->_string,"ddt_nature"))
            {
              if(gNatureddt)
                adms_veriloga_message_fatal("ddt_nature attribute defined more than once\n",$1);
              gNatureddt=adms_kclone(mylexval3);
            }
            else
             adms_veriloga_message_fatal("unknown nature attribute\n",$1);
          }
        ;
R_d.attribute.0
        : 
          {
          }
        | R_d.attribute
          {
          }
        ;
R_d.attribute
        : tk_beginattribute R_l.attribute tk_endattribute
          {
          }
        | tk_beginattribute tk_anytext
          {
            char* mylexval2=((p_lexval)$2)->_string;
            p_attribute myattribute=adms_attribute_new("ibm");
            p_admst myconstant=adms_admst_newks(adms_kclone(mylexval2));
            myattribute->_value=(p_adms)myconstant;
            adms_slist_push(&gAttributeList,(p_adms)myattribute);
          }
        | tk_beginattribute tk_endattribute
          {
          }
        ;
R_l.attribute
        : R_s.attribute
          {
          }
        | R_l.attribute R_s.attribute
          {
          }
        ;
R_s.attribute
        : tk_ident '=' tk_anystring
          {
            char* mylexval1=((p_lexval)$1)->_string;
            char* mylexval3=((p_lexval)$3)->_string;
            p_attribute myattribute=adms_attribute_new(mylexval1);
            p_admst myconstant=adms_admst_newks(adms_kclone(mylexval3));
            myattribute->_value=(p_adms)myconstant;
            adms_slist_push(&gAttributeList,(p_adms)myattribute);
          }
        ;
R_d.module
        : R_d.attribute.0 tk_module tk_ident
          {
            char* mylexval3=((p_lexval)$3)->_string;
            p_slist l;
            p_nodealias mynodealias;
            gModule=adms_admsmain_list_module_prepend_by_id_once_or_abort(root(),mylexval3); 
            adms_slist_push(&gBlockList,(p_adms)gModule);
            mynodealias=adms_module_list_nodealias_prepend_by_id_once_or_abort(gModule,gModule,"0"); 
            gGND=adms_module_list_node_prepend_by_id_once_or_abort(gModule,gModule,"GND"); 
            mynodealias->_node=gGND;
            gGND->_location=admse_ground;
            for(l=gAttributeList;l;l=l->next)
              adms_slist_push(&gModule->_attribute,l->data);
            adms_slist_free(gAttributeList); gAttributeList=NULL;
          }
         R_d.terminal R_modulebody tk_endmodule
          {
            adms_slist_pull(&gBlockList);
            adms_slist_inreverse(&gModule->_assignment);
          }
        ;
R_modulebody
        : 
          {
          }
        | R_l.declaration
          {
          }
        | R_netlist
          {
          }
        | R_l.declaration R_netlist
          {
          }
        ;
R_netlist
        : R_analog
          {
          }
        | R_l.instance
          {
          }
        | R_l.instance R_analog
          {
          }
        | R_analog R_l.instance
          {
          }
        | R_l.instance R_analog R_l.instance
          {
          }
        ;
R_l.instance
        : R_s.instance
          {
          }
        | R_l.instance R_s.instance
          {
          }
        ;
R_d.terminal
        : '(' R_l.terminal.0 ')' R_d.attribute.0 ';'
          {
            p_slist l;
            for(l=gAttributeList;l;l=l->next)
              adms_slist_push(&gModule->_attribute,l->data);
            adms_slist_free(gAttributeList); gAttributeList=NULL;
          }
        ;
R_l.terminal.0
        : 
          {
          }
        | R_l.terminal
          {
          }
        ;
R_l.terminal
        : R_s.terminal
          {
          }
        | R_l.terminal ',' R_s.terminal
          {
          }
        ;
R_s.terminal
        : tk_ident
          {
            char* mylexval1=((p_lexval)$1)->_string;
            p_nodealias mynodealias=adms_module_list_nodealias_prepend_by_id_once_or_abort(gModule,gModule,mylexval1); 
            p_node mynode=adms_module_list_node_prepend_by_id_once_or_abort(gModule,gModule,mylexval1); 
            mynodealias->_node=mynode;
            mynode->_location=admse_external;
          }
        ;
R_l.declaration
        : R_s.declaration.withattribute
          {
          }
        | R_l.declaration R_s.declaration.withattribute
          {
          }
        ;
R_s.declaration.withattribute
        : R_s.declaration
          {
          }
        | R_d.attribute.global R_s.declaration
          {
            adms_slist_free(gGlobalAttributeList); gGlobalAttributeList=NULL;
          }
        ;
R_d.attribute.global
        : R_d.attribute
          {
            gGlobalAttributeList=gAttributeList;
            gAttributeList=NULL;
          }
        ;
R_s.declaration
        : R_d.node
          {
          }
        | R_d.branch
          {
          }
        | tk_parameter R_variable.type R_l.parameter R_d.variable.end
          {
          }
        | tk_parameter R_l.parameter R_d.variable.end
          {
          }
        | R_variable.type R_l.variable R_d.variable.end
          {
          }
        | R_d.aliasparameter
          {
          }
        | R_d.analogfunction
          {
          }
        | ';'
          {
          }
        ;
R_d.node
        : R_node.type R_l.terminalnode ';'
          {
            p_slist l;
            for(l=gTerminalList;l;l=l->next)
              ((p_node)l->data)->_direction=gNodeDirection;
            adms_slist_free(gTerminalList); gTerminalList=NULL;
          }
        | tk_ground R_l.node ';'
          {
            p_slist l;
            for(l=gNodeList;l;l=l->next)
              ((p_node)l->data)->_location=admse_ground;
            adms_slist_free(gNodeList); gNodeList=NULL;
          }
        | tk_ident R_l.node ';'
          {
            char* mylexval1=((p_lexval)$1)->_string;
            char* mydisciplinename=mylexval1;
            p_discipline mydiscipline=adms_admsmain_list_discipline_lookup_by_id(root(),mydisciplinename);
            p_slist l;
            for(l=gNodeList;l;l=l->next)
              ((p_node)l->data)->_discipline=mydiscipline;
            adms_slist_free(gNodeList); gNodeList=NULL;
          }
        ;
R_node.type
        : tk_input
          {
            gNodeDirection=admse_input;
          }
        | tk_output
          {
            gNodeDirection=admse_output;
          }
        | tk_inout
          {
            gNodeDirection=admse_inout;
          }
        ;
R_l.terminalnode
        : R_s.terminalnode
          {
          }
        | R_l.terminalnode ',' R_s.terminalnode
          {
          }
        ;
R_l.node
        : R_s.node
          {
          }
        | R_l.node ',' R_s.node
          {
          }
        ;
R_s.terminalnode
        : tk_ident R_d.attribute.0
          {
            char* mylexval1=((p_lexval)$1)->_string;
            p_slist l;
            p_node mynode=adms_module_list_node_lookup_by_id(gModule,gModule,mylexval1);
            if(!mynode)
             adms_veriloga_message_fatal("terminal not found\n",$1);
            if(mynode->_location!=admse_external)
             adms_veriloga_message_fatal("node not a terminal\n",$1);
            adms_slist_push(&gTerminalList,(p_adms)mynode);
            for(l=gAttributeList;l;l=l->next)
              adms_slist_push(&mynode->_attribute,l->data);
            adms_slist_free(gAttributeList); gAttributeList=NULL;
            for(l=gGlobalAttributeList;l;l=l->next)
              adms_slist_push(&mynode->_attribute,l->data);
          }
        ;
R_s.node
        : tk_ident R_d.attribute.0
          {
            char* mylexval1=((p_lexval)$1)->_string;
            p_slist l;
            p_node mynode=adms_module_list_node_prepend_by_id_once_or_ignore(gModule,gModule,mylexval1);
            adms_slist_push(&gNodeList,(p_adms)mynode);
            for(l=gAttributeList;l;l=l->next)
              adms_slist_push(&mynode->_attribute,l->data);
            adms_slist_free(gAttributeList); gAttributeList=NULL;
            for(l=gGlobalAttributeList;l;l=l->next)
              adms_slist_push(&mynode->_attribute,l->data);
          }
        ;
R_d.branch
        : tk_branch R_s.branch ';'
          {
          }
        ;
R_l.branchalias
        : R_s.branchalias
          {
          }
        | R_l.branchalias ',' R_s.branchalias
          {
          }
        ;
R_s.branchalias
        : tk_ident
          {
            char* mylexval1=((p_lexval)$1)->_string;
            adms_slist_push(&gBranchAliasList,(p_adms)mylexval1);
          }
        ;
R_s.branch
        : '(' tk_ident ',' tk_ident ')' R_l.branchalias
          {
            char* mylexval2=((p_lexval)$2)->_string;
            char* mylexval4=((p_lexval)$4)->_string;
            p_slist l;
            p_branch mybranch; 
            p_node pnode=adms_module_list_node_lookup_by_id(gModule,gModule,mylexval2);
            p_node nnode=adms_module_list_node_lookup_by_id(gModule,gModule,mylexval4);
            mybranch=adms_module_list_branch_prepend_by_id_once_or_ignore(gModule,gModule,pnode,nnode); 
            if(!pnode)
             adms_veriloga_message_fatal("node never declared\n",$2);
            if(!nnode)
             adms_veriloga_message_fatal("node never declared\n",$4);
            for(l=gBranchAliasList;l;l=l->next)
            {
              char*aliasname=(char*)l->data;
              p_branchalias mybranchalias; 
              mybranchalias=adms_module_list_branchalias_prepend_by_id_once_or_abort(gModule,gModule,aliasname); 
              if(mybranchalias) mybranchalias->_branch=mybranch;
            }
            adms_slist_free(gBranchAliasList);
            gBranchAliasList=NULL;
            for(l=gGlobalAttributeList;l;l=l->next)
              adms_slist_push(&mybranch->_attribute,l->data);
          }
        | '(' tk_ident ')' R_l.branchalias
          {
            char* mylexval2=((p_lexval)$2)->_string;
            p_slist l;
            p_branch mybranch;
            p_node pnode=adms_module_list_node_lookup_by_id(gModule,gModule,mylexval2);
            if(!pnode)
             adms_veriloga_message_fatal("node never declared\n",$2);
            mybranch=adms_module_list_branch_prepend_by_id_once_or_ignore(gModule,gModule,pnode,gGND); 
            for(l=gBranchAliasList;l;l=l->next)
            {
              char*aliasname=(char*)l->data;
              p_branchalias mybranchalias; 
              mybranchalias=adms_module_list_branchalias_prepend_by_id_once_or_abort(gModule,gModule,aliasname); 
              if(mybranchalias) mybranchalias->_branch=mybranch;
            }
            adms_slist_free(gBranchAliasList);
            gBranchAliasList=NULL;
            for(l=gGlobalAttributeList;l;l=l->next)
              adms_slist_push(&mybranch->_attribute,l->data);
          }
        ;
R_d.analogfunction
        : R_d.analogfunction.proto R_l.analogfunction.declaration R_analogcode.block tk_endfunction
          {
            adms_slist_pull(&gBlockList);
            gAnalogfunction->_tree=YY($3);
            gAnalogfunction=NULL;
          }
        ;
R_d.analogfunction.proto
        : tk_analog tk_function R_d.analogfunction.name ';'
          {
            NEWVARIABLE(gAnalogfunction->_lexval)
            adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
            myvariableprototype->_output=admse_yes;
          }
        | tk_analog tk_function tk_integer R_d.analogfunction.name ';'
          {
            NEWVARIABLE(gAnalogfunction->_lexval)
            adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
            myvariableprototype->_output=admse_yes;
            myvariableprototype->_type=admse_integer;
            gAnalogfunction->_type=admse_integer; 
          }
        | tk_analog tk_function tk_real R_d.analogfunction.name ';'
          {
            NEWVARIABLE(gAnalogfunction->_lexval)
            adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
            myvariableprototype->_output=admse_yes;
          }
        ;
R_d.analogfunction.name
        : tk_ident
          {
            p_slist l;
            gAnalogfunction=adms_analogfunction_new(gModule,$1);
            adms_slist_push(&gBlockList,(p_adms)gAnalogfunction);
            adms_module_list_analogfunction_prepend_once_or_abort(gModule,gAnalogfunction); 
            for(l=gGlobalAttributeList;l;l=l->next)
              adms_slist_push(&gAnalogfunction->_attribute,l->data);
          }
        ;
R_l.analogfunction.declaration
        : R_s.analogfunction.declaration
          {
          }
        | R_l.analogfunction.declaration R_s.analogfunction.declaration
          {
          }
        ;
R_s.analogfunction.declaration
        : tk_input R_l.analogfunction.input.variable ';'
          {
          }
        | tk_output R_l.analogfunction.output.variable ';'
          {
          }
        | tk_inout R_l.analogfunction.inout.variable ';'
          {
          }
        | tk_integer R_l.analogfunction.integer.variable ';'
          {
          }
        | tk_real R_l.analogfunction.real.variable ';'
          {
          }
        ;
R_l.analogfunction.input.variable
        : tk_ident
          {
            NEWVARIABLE($1)
            adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
            myvariableprototype->_input=admse_yes;
            myvariableprototype->_parametertype=admse_analogfunction;
          }
        | R_l.analogfunction.input.variable ',' tk_ident
          {
            NEWVARIABLE($3)
            adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
            myvariableprototype->_input=admse_yes;
            myvariableprototype->_parametertype=admse_analogfunction;
          }
        ;
R_l.analogfunction.output.variable
        : tk_ident
          {
            NEWVARIABLE($1)
            adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
            myvariableprototype->_output=admse_yes;
            myvariableprototype->_parametertype=admse_analogfunction;
          }
        | R_l.analogfunction.output.variable ',' tk_ident
          {
            NEWVARIABLE($3)
            adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
            myvariableprototype->_output=admse_yes;
            myvariableprototype->_parametertype=admse_analogfunction;
          }
        ;
R_l.analogfunction.inout.variable
        : tk_ident
          {
            NEWVARIABLE($1)
            adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
            myvariableprototype->_input=admse_yes;
            myvariableprototype->_output=admse_yes;
            myvariableprototype->_parametertype=admse_analogfunction;
          }
        | R_l.analogfunction.inout.variable ',' tk_ident
          {
            NEWVARIABLE($3)
            adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
            myvariableprototype->_input=admse_yes;
            myvariableprototype->_output=admse_yes;
            myvariableprototype->_parametertype=admse_analogfunction;
          }
        ;
R_l.analogfunction.integer.variable
        : tk_ident
          {
            p_variableprototype myvariableprototype=adms_analogfunction_list_variable_lookup_by_id(gAnalogfunction,gModule,$1,(p_adms)gAnalogfunction);
            if(myvariableprototype)
              myvariableprototype->_type=admse_integer;
            else
            {
              NEWVARIABLE($1)
              adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
              myvariableprototype->_type=admse_integer;
            }
          }
        | R_l.analogfunction.integer.variable ',' tk_ident
          {
            p_variableprototype myvariableprototype=adms_analogfunction_list_variable_lookup_by_id(gAnalogfunction,gModule,$3,(p_adms)gAnalogfunction);
            if(myvariableprototype)
              myvariableprototype->_type=admse_integer;
            else
            {
              NEWVARIABLE($3)
              adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
              myvariableprototype->_type=admse_integer;
            }
          }
        ;
R_l.analogfunction.real.variable
        : tk_ident
          {
            p_variableprototype myvariableprototype=adms_analogfunction_list_variable_lookup_by_id(gAnalogfunction,gModule,$1,(p_adms)gAnalogfunction);
            if(myvariableprototype)
              myvariableprototype->_type=admse_real;
            else
            {
              NEWVARIABLE($1)
              adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
              myvariableprototype->_type=admse_real;
            }
          }
        | R_l.analogfunction.real.variable ',' tk_ident
          {
            p_variableprototype myvariableprototype=adms_analogfunction_list_variable_lookup_by_id(gAnalogfunction,gModule,$3,(p_adms)gAnalogfunction);
            if(myvariableprototype)
              myvariableprototype->_type=admse_real;
            else
            {
              NEWVARIABLE($3)
              adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
              myvariableprototype->_type=admse_real;
            }
          }
        ;
R_variable.type
        : tk_integer R_d.attribute.0
          {
            gVariableType=admse_integer;
            adms_slist_concat(&gGlobalAttributeList,gAttributeList);
            gAttributeList=NULL;
          }
        | tk_real R_d.attribute.0
          {
            gVariableType=admse_real;
            adms_slist_concat(&gGlobalAttributeList,gAttributeList);
            gAttributeList=NULL;
          }
        | tk_string R_d.attribute.0
          {
            gVariableType=admse_string;
            adms_slist_concat(&gGlobalAttributeList,gAttributeList);
            gAttributeList=NULL;
          }
        ;
R_d.variable.end
        : ';'
          {
            p_slist l;
            for(l=gVariableDeclarationList;l;l=l->next)
              ((p_variableprototype)l->data)->_type=gVariableType;
            adms_slist_free(gVariableDeclarationList); gVariableDeclarationList=NULL;
          }
        ;
R_l.parameter
        : R_s.parameter
          {
          }
        | R_l.parameter ',' R_s.parameter
          {
          }
        ;
R_l.variable
        : R_s.variable
          {
          }
        | R_l.variable ',' R_s.variable
          {
          }
        ;
R_d.aliasparameter
        : R_d.aliasparameter.token tk_ident '=' tk_ident R_d.attribute.0 ';'
          {
            char* mylexval2=((p_lexval)$2)->_string;
            p_slist l;
            p_variableprototype myvariableprototype=adms_module_list_variable_lookup_by_id(gModule,gModule,$4,(p_adms)gModule);
            if(!myvariableprototype)
             adms_veriloga_message_fatal("variable never declared\n",$4);
            adms_variableprototype_list_alias_prepend_once_or_abort(myvariableprototype,adms_kclone(mylexval2));
            for(l=gAttributeList;l;l=l->next)
              adms_slist_push(&myvariableprototype->_attribute,l->data);
            adms_slist_free(gAttributeList); gAttributeList=NULL;
            for(l=gGlobalAttributeList;l;l=l->next)
              adms_slist_push(&myvariableprototype->_attribute,l->data);
          }
        ;
R_d.aliasparameter.token
        : tk_aliasparameter
          {
          }
        | tk_aliasparam
          {
          }
        ;
R_s.parameter
        : R_s.parameter.name R_d.attribute.0
          {
            p_slist l;
            for(l=gAttributeList;l;l=l->next)
              adms_slist_push(&((p_variableprototype)gVariableDeclarationList->data)->_attribute,l->data);
            adms_slist_free(gAttributeList); gAttributeList=NULL;
            for(l=gGlobalAttributeList;l;l=l->next)
              adms_slist_push(&((p_variableprototype)gVariableDeclarationList->data)->_attribute,l->data);
          }
        ;
R_s.variable
        : R_s.variable.name R_d.attribute.0
          {
            p_slist l;
            for(l=gAttributeList;l;l=l->next)
              adms_slist_push(&((p_variableprototype)gVariableDeclarationList->data)->_attribute,l->data);
            adms_slist_free(gAttributeList); gAttributeList=NULL;
            for(l=gGlobalAttributeList;l;l=l->next)
              adms_slist_push(&((p_variableprototype)gVariableDeclarationList->data)->_attribute,l->data);
          }
        ;
R_s.parameter.name
        : R_s.variable.name '=' R_s.expression R_s.parameter.range
          {
            ((p_variableprototype)gVariableDeclarationList->data)->_input=admse_yes;
            ((p_variableprototype)gVariableDeclarationList->data)->_default=((p_expression)YY($3));
            ((p_variableprototype)gVariableDeclarationList->data)->_range=adms_slist_reverse(gRangeList);
            gRangeList=NULL;
          }
        | R_s.variable.name '=' '{' R_l.expression '}' R_s.parameter.range
          {
            p_slist myArgs=(p_slist)YY($4);
            adms_slist_inreverse(&myArgs);
            ((p_variableprototype)gVariableDeclarationList->data)->_input=admse_yes;
            ((p_variableprototype)gVariableDeclarationList->data)->_default=((p_expression)myArgs->data);
            ((p_variableprototype)gVariableDeclarationList->data)->_arraydefault=myArgs;
            ((p_variableprototype)gVariableDeclarationList->data)->_range=adms_slist_reverse(gRangeList);
            gRangeList=NULL;
          }
        ;
R_s.variable.name
        : tk_ident
          {
            char* mylexval1=((p_lexval)$1)->_string;
            NEWVARIABLE($1)
            if(adms_module_list_node_lookup_by_id(gModule,gModule,mylexval1))
             adms_veriloga_message_fatal("variable already defined as node\n",$1);
            adms_module_list_variable_prepend_once_or_abort(gModule,myvariableprototype); 
            adms_slist_push(&gVariableDeclarationList,(p_adms)myvariableprototype);
          }
        | tk_ident '[' tk_number ':' tk_number ']'
          {
            char* mylexval1=((p_lexval)$1)->_string;
            NEWVARIABLE($1)
            if(adms_module_list_node_lookup_by_id(gModule,gModule,mylexval1))
             adms_veriloga_message_fatal("variable already defined as node\n",$1);
            adms_module_list_variable_prepend_once_or_abort(gModule,myvariableprototype); 
            adms_slist_push(&gVariableDeclarationList,(p_adms)myvariableprototype);
            myvariableprototype->_sizetype=admse_array;
            myvariableprototype->_minsize=adms_number_new($3);
            myvariableprototype->_maxsize=adms_number_new($5);
          }
        ;
R_s.parameter.range
        : 
          {
          }
        | R_l.interval
          {
          }
        ;
R_l.interval
        : R_s.interval
          {
          }
        | R_l.interval R_s.interval
          {
          }
        ;
R_s.interval
        : tk_from R_d.interval
          {
            if(((p_range)YY($2))->_infboundtype==admse_range_bound_value)
              ((p_range)YY($2))->_type=admse_include_value;
            else
              ((p_range)YY($2))->_type=admse_include;
            adms_slist_push(&gRangeList,YY($2));
          }
        | tk_exclude R_d.interval
          {
            if(((p_range)YY($2))->_infboundtype==admse_range_bound_value)
              ((p_range)YY($2))->_type=admse_exclude_value;
            else
              ((p_range)YY($2))->_type=admse_exclude;
            adms_slist_push(&gRangeList,YY($2));
          }
        ;
R_d.interval
        : '(' R_interval.inf ':' R_interval.sup ')'
          {
            p_range myrange=adms_module_list_range_prepend_by_id_once_or_abort(gModule,gModule,(p_expression)YY($2),(p_expression)YY($4)); 
            $$=adms_yaccval_new("unknown source file");
            myrange->_infboundtype=admse_range_bound_exclude;
            myrange->_supboundtype=admse_range_bound_exclude;
            Y($$,(p_adms)myrange);
          }
        | '(' R_interval.inf ':' R_interval.sup ']'
          {
            p_range myrange=adms_module_list_range_prepend_by_id_once_or_abort(gModule,gModule,(p_expression)YY($2),(p_expression)YY($4)); 
            $$=adms_yaccval_new("unknown source file");
            myrange->_infboundtype=admse_range_bound_exclude;
            myrange->_supboundtype=admse_range_bound_include;
            Y($$,(p_adms)myrange);
          }
        | '[' R_interval.inf ':' R_interval.sup ')'
          {
            p_range myrange=adms_module_list_range_prepend_by_id_once_or_abort(gModule,gModule,(p_expression)YY($2),(p_expression)YY($4)); 
            $$=adms_yaccval_new("unknown source file");
            myrange->_infboundtype=admse_range_bound_include;
            myrange->_supboundtype=admse_range_bound_exclude;
            Y($$,(p_adms)myrange);
          }
        | '[' R_interval.inf ':' R_interval.sup ']'
          {
            p_range myrange=adms_module_list_range_prepend_by_id_once_or_abort(gModule,gModule,(p_expression)YY($2),(p_expression)YY($4)); 
            $$=adms_yaccval_new("unknown source file");
            myrange->_infboundtype=admse_range_bound_include;
            myrange->_supboundtype=admse_range_bound_include;
            Y($$,(p_adms)myrange);
          }
        | R_s.expression
          {
            p_range myrange=adms_module_list_range_prepend_by_id_once_or_abort(gModule,gModule,(p_expression)YY($1),(p_expression)YY($1)); 
            $$=adms_yaccval_new("unknown source file");
            myrange->_infboundtype=admse_range_bound_value;
            myrange->_supboundtype=admse_range_bound_value;
            Y($$,(p_adms)myrange);
          }
        ;
R_interval.inf
        : R_s.expression
          {
            $$=$1;
          }
        | '-' tk_inf
          {
            p_number mynumber=adms_number_new($2); 
            p_expression myexpression=adms_expression_new(gModule,(p_adms)mynumber); 
            mynumber->_lexval->_string=adms_kclone("-inf");
            adms_slist_push(&gModule->_expression,(p_adms)myexpression); 
            $$=adms_yaccval_new("unknown source file");
            myexpression->_infinity=admse_minus;
            myexpression->_hasspecialnumber=adms_kclone("YES");
            Y($$,(p_adms)myexpression);
          }
        ;
R_interval.sup
        : R_s.expression
          {
            $$=$1;
          }
        | tk_inf
          {
            p_number mynumber=adms_number_new($1); 
            p_expression myexpression=adms_expression_new(gModule,(p_adms)mynumber); 
            mynumber->_lexval->_string=adms_kclone("+inf");
            adms_slist_push(&gModule->_expression,(p_adms)myexpression); 
            $$=adms_yaccval_new("unknown source file");
            myexpression->_infinity=admse_plus;
            myexpression->_hasspecialnumber=adms_kclone("YES");
            Y($$,(p_adms)myexpression);
          }
        | '+' tk_inf
          {
            p_number mynumber=adms_number_new($2); 
            p_expression myexpression=adms_expression_new(gModule,(p_adms)mynumber); 
            mynumber->_lexval->_string=adms_kclone("+inf");
            adms_slist_push(&gModule->_expression,(p_adms)myexpression); 
            $$=adms_yaccval_new("unknown source file");
            myexpression->_infinity=admse_plus;
            myexpression->_hasspecialnumber=adms_kclone("YES");
            Y($$,(p_adms)myexpression);
          }
        ;
R_analog
        : tk_analog R_analogcode
          {
            gModule->_analog=adms_analog_new(YY($2));
          }
        ;
R_analogcode
        : R_analogcode.atomic
          {
            $$=$1;
          }
        | R_analogcode.block
          {
            $$=$1;
          }
        ;
R_l.expression
        : R_s.expression
          {
            p_slist myArgs=NULL;
            $$=adms_yaccval_new("unknown source file");
            adms_slist_push(&myArgs,YY($1));
            Y($$,(p_adms)myArgs);
          }
        | R_l.expression ',' R_s.expression
          {
            p_slist myArgs=(p_slist)YY($1);
            $$=$1;
            adms_slist_push(&myArgs,YY($3));
            Y($$,(p_adms)myArgs);
          }
        ;
R_analogcode.atomic
        : R_d.attribute.0 R_d.blockvariable
          {
            p_slist l;
            p_slist lv;
            for(l=gAttributeList;l;l=l->next)
              for(lv=((p_blockvariable)YY($2))->_variable;lv;lv=lv->next)
                adms_slist_push(&((p_variableprototype)lv->data)->_attribute,l->data);
            adms_slist_free(gAttributeList); gAttributeList=NULL;
            $$=$2;
          }
        | R_d.contribution
          {
            $$=$1;
          }
        | R_s.assignment ';'
          {
            $$=$1;
          }
        | R_d.conditional
          {
            $$=$1;
          }
        | R_d.while
          {
            $$=$1;
          }
        | R_d.case
          {
            $$=$1;
          }
        | R_d.for
          {
            $$=$1;
          }
        | tk_dollar_ident '(' R_l.expression ')' ';'
          {
            p_function myfunction=adms_function_new($1,uid++);
            p_slist myArgs=(p_slist)YY($3);
            p_callfunction mycallfunction=adms_callfunction_new(gModule,myfunction);
            adms_slist_push(&gModule->_callfunction,(p_adms)mycallfunction);
            $$=adms_yaccval_new("unknown source file");
            adms_slist_inreverse(&myArgs);
            myfunction->_arguments=myArgs;
            Y($$,(p_adms)mycallfunction);
          }
        | tk_dollar_ident '(' ')' ';'
          {
            p_function myfunction=adms_function_new($1,uid++);
            p_callfunction mycallfunction=adms_callfunction_new(gModule,myfunction);
            adms_slist_push(&gModule->_callfunction,(p_adms)mycallfunction);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)mycallfunction);
          }
        | tk_dollar_ident ';'
          {
            p_function myfunction=adms_function_new($1,uid++);
            p_callfunction mycallfunction=adms_callfunction_new(gModule,myfunction);
            adms_slist_push(&gModule->_callfunction,(p_adms)mycallfunction);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)mycallfunction);
          }
        | ';'
          {
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)adms_nilled_new(gModule));
          }
        ;
R_analogcode.block
        : R_d.block
          {
            $$=$1;
          }
        | R_analogcode.block.atevent R_d.block
          {
            $$=$2;
            adms_lexval_free(((p_block)YY($$))->_lexval);
            ((p_block)YY($$))->_lexval=(p_lexval)YY($1);
          }
        ;
R_analogcode.block.atevent
        : '@' '(' tk_ident '(' R_l.analysis ')' ')'
          {
            adms_veriloga_message_fatal("@ control not supported\n",$3);
          }
        | '@' tk_ident
          {
            char* mylexval2=((p_lexval)$2)->_string;
            char* mypartitionning=adms_kclone(mylexval2);
            if(strcmp(mypartitionning,"initial_model")
              && strcmp(mypartitionning,"initial_instance")
              && strcmp(mypartitionning,"noise")
              && strcmp(mypartitionning,"initial_step")
              && strcmp(mypartitionning,"final_step"))
              adms_veriloga_message_fatal(" @() control not supported\n",$2);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)$2);
          }
        | '@' '(' tk_ident ')'
          {
            char* mylexval3=((p_lexval)$3)->_string;
            char* mypartitionning=adms_kclone(mylexval3);
            if(strcmp(mypartitionning,"initial_model")
              && strcmp(mypartitionning,"initial_instance")
              && strcmp(mypartitionning,"noise")
              && strcmp(mypartitionning,"initial_step")
              && strcmp(mypartitionning,"final_step"))
              adms_veriloga_message_fatal(" @() control not supported\n",$3);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)$3);
          }
        ;
R_l.analysis
        : R_s.analysis
          {
          }
        | R_l.analysis ',' R_s.analysis
          {
          }
        ;
R_s.analysis
        : tk_anystring
          {
          }
        ;
R_d.block
        : R_d.block.begin tk_end
          {
            $$=adms_yaccval_new("unknown source file");
            Y($$,gBlockList->data);
            adms_slist_pull(&gBlockList);
          }
        | R_d.block.begin ':' tk_ident tk_end
          {
            $$=adms_yaccval_new("unknown source file");
            Y($$,gBlockList->data);
            adms_slist_pull(&gBlockList);
            ((p_block)YY($$))->_lexval->_string=$3->_string;
          }
        | R_d.block.begin R_l.blockitem tk_end
          {
            $$=adms_yaccval_new("unknown source file");
            Y($$,gBlockList->data);
            adms_slist_pull(&gBlockList);
          }
        | R_d.block.begin ':' tk_ident R_l.blockitem tk_end
          {
            $$=adms_yaccval_new("unknown source file");
            Y($$,gBlockList->data);
            adms_slist_pull(&gBlockList);
            ((p_block)YY($$))->_lexval->_string=$3->_string;
          }
        ;
R_d.block.begin
        : R_d.attribute.0 tk_begin
          {
            p_slist l;
            p_block myblock=adms_block_new(gModule,$2,gBlockList?((p_block)gBlockList->data):NULL,NULL);
            $$=adms_yaccval_new("unknown source file");
            myblock->_lexval->_string=adms_kclone("");
            adms_slist_push(&gBlockList,(p_adms)myblock);
            for(l=gAttributeList;l;l=l->next)
              adms_slist_push(&myblock->_attribute,l->data);
            adms_slist_free(gAttributeList); gAttributeList=NULL;
            adms_slist_push(&gModule->_block,gBlockList->data);
          }
        ;
R_l.blockitem
        : R_analogcode
          {
            adms_slist_push(&((p_block)gBlockList->data)->_item,YY($1));
          }
        | R_l.blockitem R_analogcode
          {
            adms_slist_push(&((p_block)gBlockList->data)->_item,YY($2));
          }
        ;
R_d.blockvariable
        : tk_integer R_l.blockvariable ';'
          {
            p_slist l;
            p_blockvariable myblockvariable=adms_blockvariable_new(((p_block)gBlockList->data)); 
            adms_slist_push(&gModule->_blockvariable,(p_adms)myblockvariable); 
            for(l=gBlockVariableList;l;l=l->next)
              ((p_variableprototype)l->data)->_type=admse_integer;
            $$=adms_yaccval_new("unknown source file");
            adms_slist_inreverse(&gBlockVariableList);
            myblockvariable->_variable=gBlockVariableList;
            gBlockVariableList=NULL;
            Y($$,(p_adms)myblockvariable);
          }
        | tk_real R_l.blockvariable ';'
          {
            p_slist l;
            p_blockvariable myblockvariable=adms_blockvariable_new(((p_block)gBlockList->data)); 
            adms_slist_push(&gModule->_blockvariable,(p_adms)myblockvariable); 
            for(l=gBlockVariableList;l;l=l->next)
              ((p_variableprototype)l->data)->_type=admse_real;
            $$=adms_yaccval_new("unknown source file");
            adms_slist_inreverse(&gBlockVariableList);
            myblockvariable->_variable=gBlockVariableList;
            gBlockVariableList=NULL;
            Y($$,(p_adms)myblockvariable);
          }
        | tk_string R_l.blockvariable ';'
          {
            p_slist l;
            p_blockvariable myblockvariable=adms_blockvariable_new(((p_block)gBlockList->data)); 
            adms_slist_push(&gModule->_blockvariable,(p_adms)myblockvariable); 
            for(l=gBlockVariableList;l;l=l->next)
              ((p_variableprototype)l->data)->_type=admse_string;
            $$=adms_yaccval_new("unknown source file");
            adms_slist_inreverse(&gBlockVariableList);
            myblockvariable->_variable=gBlockVariableList;
            gBlockVariableList=NULL;
            Y($$,(p_adms)myblockvariable);
          }
        ;
R_l.blockvariable
        : R_s.blockvariable
          {
          }
        | R_l.blockvariable ',' R_s.blockvariable
          {
          }
        ;
R_s.blockvariable
        : tk_ident
          {
            NEWVARIABLE($1)
            adms_block_list_variable_prepend_once_or_abort(((p_block)gBlockList->data),myvariableprototype); 
            adms_slist_push(&gBlockVariableList,(p_adms)myvariableprototype);
          }
        | tk_ident '[' tk_number ':' tk_number ']'
          {
            NEWVARIABLE($1)
            adms_block_list_variable_prepend_once_or_abort(((p_block)gBlockList->data),myvariableprototype); 
            adms_slist_push(&gVariableDeclarationList,(p_adms)myvariableprototype);
            myvariableprototype->_sizetype=admse_array;
            myvariableprototype->_minsize=adms_number_new($3);
            myvariableprototype->_maxsize=adms_number_new($5);
          }
        ;
R_d.contribution
        : R_contribution R_d.attribute.0 ';'
          {
            p_slist l;
            for(l=gAttributeList;l;l=l->next)
              adms_slist_push(&gContribution->_attribute,l->data);
            adms_slist_free(gAttributeList); gAttributeList=NULL;
            gContribution=NULL;
          }
        ;
R_contribution
        : R_source '<' '+' R_s.expression
          {
            p_source mysource=(p_source)YY($1);
            $$=adms_yaccval_new("unknown source file");
            gContribution=adms_contribution_new(gModule,mysource,(p_expression)YY($4),gLexval);
            adms_slist_push(&gModule->_contribution,(p_adms)gContribution);
            Y($$,(p_adms)gContribution);
            gContribution->_branchalias=gBranchAlias;
            gBranchAlias=NULL;
          }
        ;
R_source
        : tk_ident '(' tk_ident ',' tk_ident ')'
          {
            char* mylexval1=((p_lexval)$1)->_string;
            char* mylexval3=((p_lexval)$3)->_string;
            char* mylexval5=((p_lexval)$5)->_string;
            p_node Pnode=adms_module_list_node_lookup_by_id(gModule,gModule,mylexval3);
            p_node Nnode=adms_module_list_node_lookup_by_id(gModule,gModule,mylexval5);
            char* natureID=mylexval1;
            p_nature mynature=adms_admsmain_list_nature_lookup_by_id(root(),natureID);
            p_branch mybranch=adms_module_list_branch_prepend_by_id_once_or_ignore(gModule,gModule,Pnode,Nnode);
            $$=adms_yaccval_new("unknown source file");
            if(!mynature)
             adms_message_fatal(("[source:error] there is no nature with access %s, missing discipline.h file?\n",natureID))
            gSource=adms_module_list_source_prepend_by_id_once_or_ignore(gModule,gModule,mybranch,mynature);
            gLexval=$1;
            Y($$,(p_adms)gSource);
          }
        | tk_ident '(' tk_ident ')'
          {
            char* mylexval1=((p_lexval)$1)->_string;
            char* mylexval3=((p_lexval)$3)->_string;
            char* natureID=mylexval1;
            p_nature mynature=adms_admsmain_list_nature_lookup_by_id(root(),natureID);
            p_branchalias branchalias=adms_module_list_branchalias_lookup_by_id(gModule,gModule,mylexval3);
            p_node pnode=adms_module_list_node_lookup_by_id(gModule,gModule,mylexval3);
            p_branch mybranch=NULL;
            $$=adms_yaccval_new("unknown source file");
            if(!mynature)
             adms_message_fatal(("[source:error] there is no nature with access %s, please, include discipline.h file\n",natureID))
            if(pnode)
              mybranch=adms_module_list_branch_prepend_by_id_once_or_ignore(gModule,gModule,pnode,gGND);
            else if(branchalias)
              mybranch=branchalias->_branch;
            else
              adms_veriloga_message_fatal("undefined branch or node\n",$1);
            gSource=adms_module_list_source_prepend_by_id_once_or_ignore(gModule,gModule,mybranch,mynature);
            gLexval=$1;
            gBranchAlias=branchalias;
            Y($$,(p_adms)gSource);
          }
        ;
R_d.while
        : tk_while '(' R_s.expression ')' R_analogcode
          {
            p_whileloop mywhileloop=adms_whileloop_new(gModule,(p_expression)YY($3),YY($5));
            adms_slist_push(&gModule->_whileloop,(p_adms)mywhileloop);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)mywhileloop);
          }
        ;
R_d.for
        : tk_for '(' R_s.assignment ';' R_s.expression ';' R_s.assignment ')' R_analogcode
          {
            p_forloop myforloop=adms_forloop_new(gModule,(p_assignment)YY($3),(p_expression)YY($5),(p_assignment)YY($7),YY($9));
            adms_slist_push(&gModule->_forloop,(p_adms)myforloop);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myforloop);
          }
        ;
R_d.case
        : tk_case '(' R_s.expression ')' R_l.case.item tk_endcase
          {
            p_case mycase=adms_case_new(gModule,(p_expression)YY($3));
            adms_slist_push(&gModule->_case,(p_adms)mycase);
            $$=adms_yaccval_new("unknown source file");
            mycase->_caseitem=adms_slist_reverse((p_slist)YY($5));
            Y($$,(p_adms)mycase);
          }
        ;
R_l.case.item
        : R_s.case.item
          {
            p_slist myArgs=NULL;
            $$=adms_yaccval_new("unknown source file");
            adms_slist_push(&myArgs,YY($1));
            Y($$,(p_adms)myArgs);
          }
        | R_l.case.item R_s.case.item
          {
            p_slist myArgs=(p_slist)YY($1);
            $$=$1;
            adms_slist_push(&myArgs,YY($2));
            Y($$,(p_adms)myArgs);
          }
        ;
R_s.case.item
        : R_l.enode ':' R_analogcode
          {
            p_slist myArgs=(p_slist)YY($1);
            p_caseitem mycaseitem=adms_caseitem_new(YY($3));
            $$=adms_yaccval_new("unknown source file");
            mycaseitem->_condition=adms_slist_reverse(myArgs);
            Y($$,(p_adms)mycaseitem);
          }
        | tk_default ':' R_analogcode
          {
            p_caseitem mycaseitem=adms_caseitem_new(YY($3));
            $$=adms_yaccval_new("unknown source file");
            mycaseitem->_defaultcase=admse_yes;
            Y($$,(p_adms)mycaseitem);
          }
        | tk_default R_analogcode
          {
            p_caseitem mycaseitem=adms_caseitem_new(YY($2));
            $$=adms_yaccval_new("unknown source file");
            mycaseitem->_defaultcase=admse_yes;
            Y($$,(p_adms)mycaseitem);
          }
        ;
R_s.instance
        : R_instance.module.name '#' '(' R_l.instance.parameter ')' tk_ident '(' R_l.node ')' ';'
          {
            char* mylexval6=((p_lexval)$6)->_string;
            p_instance myinstance;
            p_slist l1;
            p_slist l2;
            myinstance=adms_module_list_instance_prepend_by_id_once_or_abort(gModule,gModule,gInstanceModule,mylexval6);
            adms_slist_inreverse(&gInstanceModule->_node);
            l2=gInstanceModule->_node;
            l2=l2->next; /*GND ignored*/
            for(l1=adms_slist_reverse(gNodeList);l1;l1=l1->next)
            {
              adms_instance_list_terminal_prepend_once_or_abort(myinstance,adms_instancenode_new(((p_node)l1->data),(p_node)l2->data));
              l2=l2->next;
            }
            for(l1=gInstanceVariableList;l1;l1=l1->next)
              adms_instance_list_parameterset_prepend_once_or_abort(myinstance,(p_instanceparameter)l1->data);
            adms_slist_inreverse(&gInstanceModule->_node);
            adms_slist_free(gNodeList);gNodeList=NULL;
            adms_slist_free(gInstanceVariableList);gInstanceVariableList=NULL;
          }
        ;
R_instance.module.name
        : tk_ident
          {
            char* mylexval1=((p_lexval)$1)->_string;
            gInstanceModule=adms_admsmain_list_module_lookup_by_id(root(),mylexval1);
            if(!gInstanceModule)
              adms_message_fatal(("module '%s' not found\n",mylexval1))
          }
        ;
R_l.instance.parameter
        : R_s.instance.parameter
          {
          }
        | R_l.instance.parameter ',' R_s.instance.parameter
          {
          }
        ;
R_s.instance.parameter
        : '.' tk_ident '(' R_s.expression ')'
          {
            char* mylexval2=((p_lexval)$2)->_string;
            p_variableprototype myvariableprototype=adms_module_list_variable_lookup_by_id(gInstanceModule,gInstanceModule,$2,(p_adms)gInstanceModule);
            if(myvariableprototype)
            {
              p_instanceparameter myinstanceparameter;
              myinstanceparameter=adms_instanceparameter_new(myvariableprototype);
              adms_slist_push(&gInstanceVariableList,(p_adms)myinstanceparameter);
              myinstanceparameter->_value=((p_expression)YY($4));
            }
            else
            {
              adms_veriloga_message_fatal_continue($2);
              adms_message_fatal(("[%s.%s.%s]: undefined variable (instance declaration)",
                adms_module_uid(gModule),adms_module_uid(gInstanceModule),mylexval2))
            }
          }
        ;
R_s.assignment
        : tk_ident '=' R_s.expression
          {
            p_assignment myassignment;
            p_variable myvariable=variable_recursive_lookup_by_id(gBlockList->data,$1);
            p_variableprototype myvariableprototype;
            if(!myvariable)
              adms_veriloga_message_fatal("undefined variable\n",$1);
            myvariableprototype=myvariable->_prototype;
            myassignment=adms_assignment_new(gModule,(p_adms)myvariable,(p_expression)YY($3),$1);
            adms_slist_push(&gModule->_assignment,(p_adms)myassignment);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myassignment);
            myvariableprototype->_vcount++;
            myvariableprototype->_vlast=myassignment;
          }
        | R_d.attribute tk_ident '=' R_s.expression
          {
            p_assignment myassignment;
            p_variable myvariable=variable_recursive_lookup_by_id(gBlockList->data,$2);
            p_variableprototype myvariableprototype;
            if(!myvariable)
              adms_veriloga_message_fatal("undefined variable\n",$2);
            myvariableprototype=myvariable->_prototype;
            myassignment=adms_assignment_new(gModule,(p_adms)myvariable,(p_expression)YY($4),$2);
            adms_slist_push(&gModule->_assignment,(p_adms)myassignment);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myassignment);
            {
              p_slist l;
              for(l=gAttributeList;l;l=l->next)
                adms_slist_push(&myassignment->_attribute,l->data);
            }
            adms_slist_free(gAttributeList); gAttributeList=NULL;
            myvariableprototype->_vcount++;
            myvariableprototype->_vlast=myassignment;
          }
        | tk_ident '[' R_expression ']' '=' R_s.expression
          {
            p_assignment myassignment;
            p_array myarray;
            p_variable myvariable=variable_recursive_lookup_by_id(gBlockList->data,$1);
            p_variableprototype myvariableprototype;
            if(!myvariable)
              adms_veriloga_message_fatal("undefined variable\n",$1);
            myvariableprototype=myvariable->_prototype;
            myarray=adms_array_new(myvariable,YY($3));
            myassignment=adms_assignment_new(gModule,(p_adms)myarray,(p_expression)YY($6),$1);
            adms_slist_push(&gModule->_assignment,(p_adms)myassignment);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myassignment);
            myvariableprototype->_vcount++;
            myvariableprototype->_vlast=myassignment;
          }
        | R_d.attribute tk_ident '[' R_expression ']' '=' R_s.expression
          {
            p_assignment myassignment;
            p_array myarray;
            p_variable myvariable=variable_recursive_lookup_by_id(gBlockList->data,$2);
            p_variableprototype myvariableprototype;
            if(!myvariable)
              adms_veriloga_message_fatal("undefined variable\n",$2);
            myvariableprototype=myvariable->_prototype;
            myarray=adms_array_new(myvariable,YY($4));
            myassignment=adms_assignment_new(gModule,(p_adms)myarray,(p_expression)YY($7),$2);
            adms_slist_push(&gModule->_assignment,(p_adms)myassignment);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myassignment);
            {
              p_slist l;
              for(l=gAttributeList;l;l=l->next)
                adms_slist_push(&myassignment->_attribute,l->data);
            }
            adms_slist_free(gAttributeList); gAttributeList=NULL;
            myvariableprototype->_vcount++;
            myvariableprototype->_vlast=myassignment;
          }
        ;
R_d.conditional
        : tk_if '(' R_s.expression ')' R_analogcode
          {
            p_expression myexpression=(p_expression)YY($3);
            p_adms mythen=YY($5);
            p_conditional myconditional=adms_conditional_new(gModule,myexpression,mythen,NULL);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myconditional);
          } %prec PREC_IF_THEN
        | tk_if '(' R_s.expression ')' R_analogcode tk_else R_analogcode
          {
            p_expression myexpression=(p_expression)YY($3);
            p_adms mythen=YY($5);
            p_adms myelse=YY($7);
            p_conditional myconditional=adms_conditional_new(gModule,myexpression,mythen,myelse);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myconditional);
          }
        ;
R_s.expression
        : R_expression
          {
            p_expression myexpression=adms_expression_new(gModule,YY($1)); 
            adms_slist_push(&gModule->_expression,(p_adms)myexpression); 
            $$=$1;
            Y($$,(p_adms)myexpression);
          }
        ;
R_l.enode
        : R_s.function_expression
          {
            p_slist myArgs=NULL;
            $$=adms_yaccval_new("unknown source file");
            adms_slist_push(&myArgs,YY($1));
            Y($$,(p_adms)myArgs);
          }
        | R_l.enode ',' R_s.function_expression
          {
            p_slist myArgs=(p_slist)YY($1);
            $$=$1;
            adms_slist_push(&myArgs,YY($3));
            Y($$,(p_adms)myArgs);
          }
        ;
R_s.function_expression
        : R_expression
          {
            $$=$1;
          }
        ;
R_expression
        : R_e.conditional
          {
            $$=$1;
          }
        ;
R_e.conditional
        : R_e.bitwise_equ
          {
            $$=$1;
          }
        | R_e.bitwise_equ '?' R_e.bitwise_equ ':' R_e.bitwise_equ
          {
            p_adms m1=YY($1);
            p_adms m2=YY($3);
            p_adms m3=YY($5);
            p_mapply_ternary myop=adms_mapply_ternary_new(admse_conditional,m1,m2,m3);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myop);
          }
        ;
R_e.bitwise_equ
        : R_e.bitwise_xor
          {
            $$=$1;
          }
        | R_e.bitwise_equ tk_bitwise_equr R_e.bitwise_xor
          {
            p_adms m1=YY($1);
            p_adms m2=YY($3);
            p_mapply_binary myop=adms_mapply_binary_new(admse_bw_equr,m1,m2);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myop);
          }
        | R_e.bitwise_equ '~' '^' R_e.bitwise_xor
          {
            p_adms m1=YY($1);
            p_adms m2=YY($4);
            p_mapply_binary myop=adms_mapply_binary_new(admse_bw_equl,m1,m2);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myop);
          }
        ;
R_e.bitwise_xor
        : R_e.bitwise_or
          {
            $$=$1;
          }
        | R_e.bitwise_xor '^' R_e.bitwise_or
          {
            p_adms m1=YY($1);
            p_adms m2=YY($3);
            p_mapply_binary myop=adms_mapply_binary_new(admse_bw_xor,m1,m2);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myop);
          }
        ;
R_e.bitwise_or
        : R_e.bitwise_and
          {
            $$=$1;
          }
        | R_e.bitwise_or '|' R_e.bitwise_and
          {
            p_adms m1=YY($1);
            p_adms m2=YY($3);
            p_mapply_binary myop=adms_mapply_binary_new(admse_bw_or,m1,m2);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myop);
          }
        ;
R_e.bitwise_and
        : R_e.logical_or
          {
            $$=$1;
          }
        | R_e.bitwise_and '&' R_e.logical_or
          {
            p_adms m1=YY($1);
            p_adms m2=YY($3);
            p_mapply_binary myop=adms_mapply_binary_new(admse_bw_and,m1,m2);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myop);
          }
        ;
R_e.logical_or
        : R_e.logical_and
          {
            $$=$1;
          }
        | R_e.logical_or tk_or R_e.logical_and
          {
            p_adms m1=YY($1);
            p_adms m2=YY($3);
            p_mapply_binary myop=adms_mapply_binary_new(admse_or,m1,m2);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myop);
          }
        ;
R_e.logical_and
        : R_e.comp_equ
          {
            $$=$1;
          }
        | R_e.logical_and tk_and R_e.comp_equ
          {
            p_adms m1=YY($1);
            p_adms m2=YY($3);
            p_mapply_binary myop=adms_mapply_binary_new(admse_and,m1,m2);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myop);
          }
        ;
R_e.comp_equ
        : R_e.comp
          {
            $$=$1;
          }
        | R_e.comp_equ '=' '=' R_e.comp
          {
            p_adms m1=YY($1);
            p_adms m2=YY($4);
            p_mapply_binary myop=adms_mapply_binary_new(admse_equ,m1,m2);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myop);
          }
        | R_e.comp_equ '!' '=' R_e.comp
          {
            p_adms m1=YY($1);
            p_adms m2=YY($4);
            p_mapply_binary myop=adms_mapply_binary_new(admse_notequ,m1,m2);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myop);
          }
        ;
R_e.comp
        : R_e.bitwise_shift
          {
            $$=$1;
          }
        | R_e.comp '<' R_e.bitwise_shift
          {
            p_adms m1=YY($1);
            p_adms m2=YY($3);
            p_mapply_binary myop=adms_mapply_binary_new(admse_lt,m1,m2);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myop);
          }
        | R_e.comp '<' '=' R_e.bitwise_shift
          {
            p_adms m1=YY($1);
            p_adms m2=YY($4);
            p_mapply_binary myop=adms_mapply_binary_new(admse_lt_equ,m1,m2);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myop);
          }
        | R_e.comp '>' R_e.bitwise_shift
          {
            p_adms m1=YY($1);
            p_adms m2=YY($3);
            p_mapply_binary myop=adms_mapply_binary_new(admse_gt,m1,m2);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myop);
          }
        | R_e.comp '>' '=' R_e.bitwise_shift
          {
            p_adms m1=YY($1);
            p_adms m2=YY($4);
            p_mapply_binary myop=adms_mapply_binary_new(admse_gt_equ,m1,m2);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myop);
          }
        ;
R_e.bitwise_shift
        : R_e.arithm_add
          {
            $$=$1;
          }
        | R_e.bitwise_shift tk_op_shr R_e.arithm_add
          {
            p_adms m1=YY($1);
            p_adms m2=YY($3);
            p_mapply_binary myop=adms_mapply_binary_new(admse_shiftr,m1,m2);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myop);
          }
        | R_e.bitwise_shift tk_op_shl R_e.arithm_add
          {
            p_adms m1=YY($1);
            p_adms m2=YY($3);
            p_mapply_binary myop=adms_mapply_binary_new(admse_shiftl,m1,m2);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myop);
          }
        ;
R_e.arithm_add
        : R_e.arithm_mult
          {
            $$=$1;
          }
        | R_e.arithm_add '+' R_e.arithm_mult
          {
            p_adms m1=YY($1);
            p_adms m2=YY($3);
            p_mapply_binary myop=adms_mapply_binary_new(admse_addp,m1,m2);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myop);
          }
        | R_e.arithm_add '-' R_e.arithm_mult
          {
            p_adms m1=YY($1);
            p_adms m2=YY($3);
            p_mapply_binary myop=adms_mapply_binary_new(admse_addm,m1,m2);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myop);
          }
        ;
R_e.arithm_mult
        : R_e.unary
          {
            $$=$1;
          }
        | R_e.arithm_mult '*' R_e.unary
          {
            p_adms m1=YY($1);
            p_adms m2=YY($3);
            p_mapply_binary myop=adms_mapply_binary_new(admse_multtime,m1,m2);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myop);
          }
        | R_e.arithm_mult '/' R_e.unary
          {
            p_adms m1=YY($1);
            p_adms m2=YY($3);
            p_mapply_binary myop=adms_mapply_binary_new(admse_multdiv,m1,m2);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myop);
          }
        | R_e.arithm_mult '%' R_e.unary
          {
            p_adms m1=YY($1);
            p_adms m2=YY($3);
            p_mapply_binary myop=adms_mapply_binary_new(admse_multmod,m1,m2);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myop);
          }
        ;
R_e.unary
        : R_e.atomic
          {
            $$=$1;
          }
        | '+' R_e.atomic
          {
            p_adms m=YY($2);
            p_mapply_unary mymathapply=adms_mapply_unary_new(admse_plus,m);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)mymathapply);
          }
        | '-' R_e.atomic
          {
            p_adms m=YY($2);
            p_mapply_unary mymathapply=adms_mapply_unary_new(admse_minus,m);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)mymathapply);
          }
        | '!' R_e.atomic
          {
            p_adms m=YY($2);
            p_mapply_unary mymathapply=adms_mapply_unary_new(admse_not,m);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)mymathapply);
          }
        | '~' R_e.atomic
          {
            p_adms m=YY($2);
            p_mapply_unary mymathapply=adms_mapply_unary_new(admse_bw_not,m);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)mymathapply);
          }
        ;
R_e.atomic
        : tk_number
          {
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)adms_number_new($1));
          }
        | tk_number tk_ident
          {
            char* mylexval2=((p_lexval)$2)->_string;
            p_number mynumber=adms_number_new($1);
            int myunit=admse_1;
            $$=adms_yaccval_new("unknown source file");
            if(0) {}
            else if(!strcmp(mylexval2,"E")) myunit=admse_E;
            else if(!strcmp(mylexval2,"P")) myunit=admse_P;
            else if(!strcmp(mylexval2,"T")) myunit=admse_T;
            else if(!strcmp(mylexval2,"G")) myunit=admse_G;
            else if(!strcmp(mylexval2,"M")) myunit=admse_M;
            else if(!strcmp(mylexval2,"k")) myunit=admse_k;
            else if(!strcmp(mylexval2,"h")) myunit=admse_h;
            else if(!strcmp(mylexval2,"D")) myunit=admse_D;
            else if(!strcmp(mylexval2,"d")) myunit=admse_d;
            else if(!strcmp(mylexval2,"c")) myunit=admse_c;
            else if(!strcmp(mylexval2,"m")) myunit=admse_m;
            else if(!strcmp(mylexval2,"u")) myunit=admse_u;
            else if(!strcmp(mylexval2,"n")) myunit=admse_n;
            else if(!strcmp(mylexval2,"A")) myunit=admse_A;
            else if(!strcmp(mylexval2,"p")) myunit=admse_p;
            else if(!strcmp(mylexval2,"f")) myunit=admse_f;
            else if(!strcmp(mylexval2,"a")) myunit=admse_a;
            else
              adms_veriloga_message_fatal(" can not convert symbol to valid unit\n",$2);
            mynumber->_scalingunit=myunit;
            Y($$,(p_adms)mynumber);
          }
        | tk_char
          {
            adms_veriloga_message_fatal("%s: character are not handled\n",$1);
          }
        | tk_anystring
          {
            char* mylexval1=((p_lexval)$1)->_string;
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)adms_string_new(mylexval1));
          }
        | tk_ident
          {
            char* mylexval1=((p_lexval)$1)->_string;
            $$=adms_yaccval_new("unknown source file");
            if(gAnalogfunction)
            {
              p_variable myvariable=variable_recursive_lookup_by_id(gBlockList->data,$1);
              if(myvariable)
                Y($$,(p_adms)myvariable);
            }
            else
            {
              p_variable myvariable=variable_recursive_lookup_by_id(gBlockList->data,$1);
              if(myvariable)
                Y($$,(p_adms)myvariable);
              else
              {
                p_branchalias mybranchalias=adms_module_list_branchalias_lookup_by_id(gModule,gModule,mylexval1);
                p_node mynode=adms_module_list_node_lookup_by_id(gModule,gModule,mylexval1);
                if(mynode) Y($$,(p_adms)mynode);
                if(mybranchalias)
                  Y($$,(p_adms)mybranchalias->_branch);
              }
            }
            if(!YY($$))
              adms_veriloga_message_fatal("identifier never declared\n",$1);
          }
        | tk_dollar_ident
          {
            p_function myfunction=adms_function_new($1,uid++);
            $$=adms_yaccval_new("unknown source file");
            Y($$,(p_adms)myfunction);
          }
        | tk_ident '[' R_expression ']'
          {
            $$=adms_yaccval_new("unknown source file");
            p_variable myvariable=variable_recursive_lookup_by_id(gBlockList->data,$1);
            if(!myvariable)
               adms_veriloga_message_fatal("undefined array variable\n",$1);
            Y($$,(p_adms)adms_array_new(myvariable,YY($3)));
          }
        | tk_dollar_ident '(' R_l.enode ')'
          {
            p_function myfunction=adms_function_new($1,uid++);
            p_slist myArgs=(p_slist)YY($3);
            $$=adms_yaccval_new("unknown source file");
            adms_slist_inreverse(&myArgs);
            myfunction->_arguments=myArgs;
            Y($$,(p_adms)myfunction);
          }
        | tk_ident '(' R_l.enode ')'
          {
            char* mylexval1=((p_lexval)$1)->_string;
            char* myfunctionname=mylexval1;
            p_slist myArgs=(p_slist)YY($3);
            int narg=adms_slist_length(myArgs);
            p_probe myprobe=NULL;
            p_nature mynature=adms_admsmain_list_nature_lookup_by_id(root(),myfunctionname);
            $$=adms_yaccval_new("unknown source file");
            if(mynature && narg==1)
            {
              p_adms mychild0=(p_adms)adms_slist_nth_data(myArgs,0);
              if(mychild0->_datatypename==admse_node)
              {
                p_branch mybranch=adms_module_list_branch_prepend_by_id_once_or_ignore(gModule,gModule,(p_node)mychild0,gGND);
                myprobe=adms_module_list_probe_prepend_by_id_once_or_ignore(gModule,gModule,mybranch,mynature);
              }
              else if(mychild0->_datatypename==admse_branch)
              {
                myprobe=adms_module_list_probe_prepend_by_id_once_or_ignore(gModule,gModule,(p_branch)mychild0,mynature);
              }
              else
                adms_veriloga_message_fatal("bad argument (expecting node or branch)\n",$1);
            }
            else if(mynature && narg==2)
            {
              p_adms mychild0=(p_adms)adms_slist_nth_data(myArgs,0);
              p_adms mychild1=(p_adms)adms_slist_nth_data(myArgs,1);
              p_branch mybranch;
              if(mychild0->_datatypename!=admse_node)
                adms_veriloga_message_fatal("second argument of probe is not a node\n",$1);
              if(mychild1->_datatypename!=admse_node)
                adms_veriloga_message_fatal("first argument of probe is not a node\n",$1);
              mybranch=adms_module_list_branch_prepend_by_id_once_or_ignore(gModule,gModule,(p_node)mychild1,((p_node)mychild0));
              myprobe=adms_module_list_probe_prepend_by_id_once_or_ignore(gModule,gModule,mybranch,mynature);
            }
            if(myprobe)
              Y($$,(p_adms)myprobe);
            else
            {
              p_slist l;
              p_function myfunction=adms_function_new($1,uid++);
              for(l=gModule->_analogfunction;l&&(myfunction->_definition==NULL);l=l->next)
              {
                p_analogfunction myanalogfunction=(p_analogfunction)l->data;
                if(!strcmp($1->_string,myanalogfunction->_lexval->_string))
                  myfunction->_definition=myanalogfunction;
              }
              myfunction->_arguments=adms_slist_reverse(myArgs);
              Y($$,(p_adms)myfunction);
            }
          }
        | '(' R_expression ')'
          {
            $$=$2;
          }
        ;
%%
void adms_veriloga_setint_yydebug(const int val)
{
  yydebug=val;
}
