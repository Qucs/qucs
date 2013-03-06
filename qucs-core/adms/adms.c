
/* ------- code automatically created by ./mkelements.pl -------------- */

#include "adms.h"
const double adms_dzero=0.0;
double adms_NAN;
FILE *stdadmstdbgimpl=NULL;
#undef TTK
#define TTK(tk) if(e==admse_##tk) return #tk;
char* ns_etostr (admse e)
{
  TK
  if(e==admse__zero) return "_zero";
  if(e==admse__any) return "_any";
  if(e==admse__constant) return "_constant";
  if(e==admse__p) return "_p";
  if(e==admse__s) return "_s";
  if(e==admse__path) return "_path";
  if(e==admse__arguments) return "_arguments";
  if(e==admse__croix) return "_croix";
  if(e==admse__croixcroix) return "_croixcroix";
  if(e==admse__ladms) return "_ladms";
  if(e==admse__ladmst) return "_ladmst";
  return NULL;
}
#undef TTK
#define TTK(tk) if(!strcmp(mytk,#tk)) return admse_##tk;
admse ns_strtoe (const char* mytk)
{
  TK
  return admse__any;
}
static char* adms_admsxml_uid (p_kadms myitem)
{
  if(!myitem)
    return adms_kclone("");
  if(myitem->_datatypename==admse_simulator)
    return adms_simulator_uid((p_simulator)myitem);
  if(myitem->_datatypename==admse_admsmain)
    return adms_admsmain_uid((p_admsmain)myitem);
  if(myitem->_datatypename==admse_list)
    return adms_list_uid((p_list)myitem);
  if(myitem->_datatypename==admse_nature)
    return adms_nature_uid((p_nature)myitem);
  if(myitem->_datatypename==admse_discipline)
    return adms_discipline_uid((p_discipline)myitem);
  if(myitem->_datatypename==admse_module)
    return adms_module_uid((p_module)myitem);
  if(myitem->_datatypename==admse_analogfunction)
    return adms_analogfunction_uid((p_analogfunction)myitem);
  if(myitem->_datatypename==admse_nodealias)
    return adms_nodealias_uid((p_nodealias)myitem);
  if(myitem->_datatypename==admse_node)
    return adms_node_uid((p_node)myitem);
  if(myitem->_datatypename==admse_branchalias)
    return adms_branchalias_uid((p_branchalias)myitem);
  if(myitem->_datatypename==admse_branch)
    return adms_branch_uid((p_branch)myitem);
  if(myitem->_datatypename==admse_variableprototype)
    return adms_variableprototype_uid((p_variableprototype)myitem);
  if(myitem->_datatypename==admse_source)
    return adms_source_uid((p_source)myitem);
  if(myitem->_datatypename==admse_range)
    return adms_range_uid((p_range)myitem);
  if(myitem->_datatypename==admse_jacobian)
    return adms_jacobian_uid((p_jacobian)myitem);
  if(myitem->_datatypename==admse_analog)
    return adms_analog_uid((p_analog)myitem);
  if(myitem->_datatypename==admse_math)
    return adms_math_uid((p_math)myitem);
  if(myitem->_datatypename==admse_string)
    return adms_string_uid((p_string)myitem);
  if(myitem->_datatypename==admse_subexpression)
    return adms_subexpression_uid((p_subexpression)myitem);
  if(myitem->_datatypename==admse_mapply_unary)
    return adms_mapply_unary_uid((p_mapply_unary)myitem);
  if(myitem->_datatypename==admse_mapply_binary)
    return adms_mapply_binary_uid((p_mapply_binary)myitem);
  if(myitem->_datatypename==admse_mapply_ternary)
    return adms_mapply_ternary_uid((p_mapply_ternary)myitem);
  if(myitem->_datatypename==admse_number)
    return adms_number_uid((p_number)myitem);
  if(myitem->_datatypename==admse_function)
    return adms_function_uid((p_function)myitem);
  if(myitem->_datatypename==admse_variable)
    return adms_variable_uid((p_variable)myitem);
  if(myitem->_datatypename==admse_array)
    return adms_array_uid((p_array)myitem);
  if(myitem->_datatypename==admse_probe)
    return adms_probe_uid((p_probe)myitem);
  if(myitem->_datatypename==admse_expression)
    return adms_expression_uid((p_expression)myitem);
  if(myitem->_datatypename==admse_instance)
    return adms_instance_uid((p_instance)myitem);
  if(myitem->_datatypename==admse_instanceparameter)
    return adms_instanceparameter_uid((p_instanceparameter)myitem);
  if(myitem->_datatypename==admse_instancenode)
    return adms_instancenode_uid((p_instancenode)myitem);
  if(myitem->_datatypename==admse_nilled)
    return adms_nilled_uid((p_nilled)myitem);
  if(myitem->_datatypename==admse_assignment)
    return adms_assignment_uid((p_assignment)myitem);
  if(myitem->_datatypename==admse_contribution)
    return adms_contribution_uid((p_contribution)myitem);
  if(myitem->_datatypename==admse_conditional)
    return adms_conditional_uid((p_conditional)myitem);
  if(myitem->_datatypename==admse_forloop)
    return adms_forloop_uid((p_forloop)myitem);
  if(myitem->_datatypename==admse_whileloop)
    return adms_whileloop_uid((p_whileloop)myitem);
  if(myitem->_datatypename==admse_case)
    return adms_case_uid((p_case)myitem);
  if(myitem->_datatypename==admse_caseitem)
    return adms_caseitem_uid((p_caseitem)myitem);
  if(myitem->_datatypename==admse_blockvariable)
    return adms_blockvariable_uid((p_blockvariable)myitem);
  if(myitem->_datatypename==admse_block)
    return adms_block_uid((p_block)myitem);
  if(myitem->_datatypename==admse_callfunction)
    return adms_callfunction_uid((p_callfunction)myitem);
  if(myitem->_datatypename==admse_evaluation)
    return adms_evaluation_uid((p_evaluation)myitem);
  if(myitem->_datatypename==admse_text)
    return adms_text_uid((p_text)myitem);
  if(myitem->_datatypename==admse_path)
    return adms_path_uid((p_path)myitem);
  if(myitem->_datatypename==admse_ptraverse)
    return adms_ptraverse_uid((p_ptraverse)myitem);
  if(myitem->_datatypename==admse_pparse)
    return adms_pparse_uid((p_pparse)myitem);
  if(myitem->_datatypename==admse_admst)
    return adms_admst_uid((p_admst)myitem);
  if(myitem->_datatypename==admse_transform)
    return adms_transform_uid((p_transform)myitem);
  if(myitem->_datatypename==admse_itransform)
    return adms_itransform_uid((p_itransform)myitem);
  if(myitem->_datatypename==admse_return)
    return adms_return_uid((p_return)myitem);
  if(myitem->_datatypename==admse_attribute)
    return adms_attribute_uid((p_attribute)myitem);
  if(myitem->_datatypename==admse_admstvariable)
    return adms_admstvariable_uid((p_admstvariable)myitem);
  if(myitem->_datatypename==admse_lexval)
    return adms_lexval_uid((p_lexval)myitem);
  if(myitem->_datatypename==admse_yaccval)
    return adms_yaccval_uid((p_yaccval)myitem);
  adms_message_fatal_continue(("%s:%i: internal error - should not be reached\n",__FILE__,__LINE__))
  adms_message_fatal(("%s:%i: please report to r29173@gmail.com\n",__FILE__,__LINE__))
  return NULL;
}
char* aprintf (p_ktransform mytransform,p_kadmst myadmst)
{
  if(myadmst->_pseudo==admse__ladms)
  {
    adms_message_fatal_continue(("list not supported\n"))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
  }
  if(myadmst->_pseudo==admse_empty)
    return adms_kclone("");
  if(myadmst->_pseudo==admse_datatypename)
    return adms_kclone(ns_etostr((admse)myadmst->_item.i));
  if(myadmst->_pseudo==admse_basicenumeration)
    return adms_kclone(ns_etostr((admse)myadmst->_item.i));
  if(myadmst->_pseudo==admse_basicinteger)
    return adms_integertostring(myadmst->_item.i);
  if(myadmst->_pseudo==admse_basicreal)
    return adms_doubletostring(myadmst->_item.r);
  if(myadmst->_pseudo==admse_basicstring)
    return adms_kclone((char*)myadmst->_item.s);
  if(myadmst->_pseudo==admse__croix)
  {
    if(((p_attribute)myadmst->_item.p)->_value)
      return aprintf(mytransform,(p_admst)((p_attribute)myadmst->_item.p)->_value);
    else
      return adms_kclone("");
  }
  if(myadmst->_pseudo==admse_simulator)
    return adms_simulator_uid((p_simulator)myadmst->_item.p);
  if(myadmst->_pseudo==admse_admsmain)
    return adms_admsmain_uid((p_admsmain)myadmst->_item.p);
  if(myadmst->_pseudo==admse_list)
    return adms_list_uid((p_list)myadmst->_item.p);
  if(myadmst->_pseudo==admse_nature)
    return adms_nature_uid((p_nature)myadmst->_item.p);
  if(myadmst->_pseudo==admse_discipline)
    return adms_discipline_uid((p_discipline)myadmst->_item.p);
  if(myadmst->_pseudo==admse_module)
    return adms_module_uid((p_module)myadmst->_item.p);
  if(myadmst->_pseudo==admse_analogfunction)
    return adms_analogfunction_uid((p_analogfunction)myadmst->_item.p);
  if(myadmst->_pseudo==admse_nodealias)
    return adms_nodealias_uid((p_nodealias)myadmst->_item.p);
  if(myadmst->_pseudo==admse_node)
    return adms_node_uid((p_node)myadmst->_item.p);
  if(myadmst->_pseudo==admse_branchalias)
    return adms_branchalias_uid((p_branchalias)myadmst->_item.p);
  if(myadmst->_pseudo==admse_branch)
    return adms_branch_uid((p_branch)myadmst->_item.p);
  if(myadmst->_pseudo==admse_variableprototype)
    return adms_variableprototype_uid((p_variableprototype)myadmst->_item.p);
  if(myadmst->_pseudo==admse_source)
    return adms_source_uid((p_source)myadmst->_item.p);
  if(myadmst->_pseudo==admse_range)
    return adms_range_uid((p_range)myadmst->_item.p);
  if(myadmst->_pseudo==admse_jacobian)
    return adms_jacobian_uid((p_jacobian)myadmst->_item.p);
  if(myadmst->_pseudo==admse_analog)
    return adms_analog_uid((p_analog)myadmst->_item.p);
  if(myadmst->_pseudo==admse_math)
    return adms_math_uid((p_math)myadmst->_item.p);
  if(myadmst->_pseudo==admse_string)
    return adms_string_uid((p_string)myadmst->_item.p);
  if(myadmst->_pseudo==admse_subexpression)
    return adms_subexpression_uid((p_subexpression)myadmst->_item.p);
  if(myadmst->_pseudo==admse_mapply_unary)
    return adms_mapply_unary_uid((p_mapply_unary)myadmst->_item.p);
  if(myadmst->_pseudo==admse_mapply_binary)
    return adms_mapply_binary_uid((p_mapply_binary)myadmst->_item.p);
  if(myadmst->_pseudo==admse_mapply_ternary)
    return adms_mapply_ternary_uid((p_mapply_ternary)myadmst->_item.p);
  if(myadmst->_pseudo==admse_number)
    return adms_number_uid((p_number)myadmst->_item.p);
  if(myadmst->_pseudo==admse_function)
    return adms_function_uid((p_function)myadmst->_item.p);
  if(myadmst->_pseudo==admse_variable)
    return adms_variable_uid((p_variable)myadmst->_item.p);
  if(myadmst->_pseudo==admse_array)
    return adms_array_uid((p_array)myadmst->_item.p);
  if(myadmst->_pseudo==admse_probe)
    return adms_probe_uid((p_probe)myadmst->_item.p);
  if(myadmst->_pseudo==admse_expression)
    return adms_expression_uid((p_expression)myadmst->_item.p);
  if(myadmst->_pseudo==admse_instance)
    return adms_instance_uid((p_instance)myadmst->_item.p);
  if(myadmst->_pseudo==admse_instanceparameter)
    return adms_instanceparameter_uid((p_instanceparameter)myadmst->_item.p);
  if(myadmst->_pseudo==admse_instancenode)
    return adms_instancenode_uid((p_instancenode)myadmst->_item.p);
  if(myadmst->_pseudo==admse_nilled)
    return adms_nilled_uid((p_nilled)myadmst->_item.p);
  if(myadmst->_pseudo==admse_assignment)
    return adms_assignment_uid((p_assignment)myadmst->_item.p);
  if(myadmst->_pseudo==admse_contribution)
    return adms_contribution_uid((p_contribution)myadmst->_item.p);
  if(myadmst->_pseudo==admse_conditional)
    return adms_conditional_uid((p_conditional)myadmst->_item.p);
  if(myadmst->_pseudo==admse_forloop)
    return adms_forloop_uid((p_forloop)myadmst->_item.p);
  if(myadmst->_pseudo==admse_whileloop)
    return adms_whileloop_uid((p_whileloop)myadmst->_item.p);
  if(myadmst->_pseudo==admse_case)
    return adms_case_uid((p_case)myadmst->_item.p);
  if(myadmst->_pseudo==admse_caseitem)
    return adms_caseitem_uid((p_caseitem)myadmst->_item.p);
  if(myadmst->_pseudo==admse_blockvariable)
    return adms_blockvariable_uid((p_blockvariable)myadmst->_item.p);
  if(myadmst->_pseudo==admse_block)
    return adms_block_uid((p_block)myadmst->_item.p);
  if(myadmst->_pseudo==admse_callfunction)
    return adms_callfunction_uid((p_callfunction)myadmst->_item.p);
  if(myadmst->_pseudo==admse_evaluation)
    return adms_evaluation_uid((p_evaluation)myadmst->_item.p);
  if(myadmst->_pseudo==admse_text)
    return adms_text_uid((p_text)myadmst->_item.p);
  if(myadmst->_pseudo==admse_path)
    return adms_path_uid((p_path)myadmst->_item.p);
  if(myadmst->_pseudo==admse_ptraverse)
    return adms_ptraverse_uid((p_ptraverse)myadmst->_item.p);
  if(myadmst->_pseudo==admse_pparse)
    return adms_pparse_uid((p_pparse)myadmst->_item.p);
  if(myadmst->_pseudo==admse_admst)
    return adms_admst_uid((p_admst)myadmst->_item.p);
  if(myadmst->_pseudo==admse_transform)
    return adms_transform_uid((p_transform)myadmst->_item.p);
  if(myadmst->_pseudo==admse_itransform)
    return adms_itransform_uid((p_itransform)myadmst->_item.p);
  if(myadmst->_pseudo==admse_return)
    return adms_return_uid((p_return)myadmst->_item.p);
  if(myadmst->_pseudo==admse_attribute)
    return adms_attribute_uid((p_attribute)myadmst->_item.p);
  if(myadmst->_pseudo==admse_admstvariable)
    return adms_admstvariable_uid((p_admstvariable)myadmst->_item.p);
  if(myadmst->_pseudo==admse_lexval)
    return adms_lexval_uid((p_lexval)myadmst->_item.p);
  if(myadmst->_pseudo==admse_yaccval)
    return adms_yaccval_uid((p_yaccval)myadmst->_item.p);
  adms_message_fatal_continue(("%s:%i: internal error - should not be reached\n",__FILE__,__LINE__))
  adms_message_fatal_continue(("%s:%i: please report to r29173@gmail.com\n",__FILE__,__LINE__))
  adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
  return NULL;
}

const int minusone=-1;
void deref (p_admst myadmst)
{
  myadmst->_refd--;
  if(myadmst->_refd)
    return;
  if(myadmst->_original)
  {
    if(myadmst->_hasnewbasicstring==1)
    {
      myadmst->_original->_refd--;
      if(myadmst->_original->_refd==0)
      {
        free(myadmst->_original->_item.s);
        adms_admst_free(myadmst->_original);
      }
    }
    else if(myadmst->_hasnewbasicstring==2)
    {
      myadmst->_original->_refd--;
      if(myadmst->_original->_refd==0)
      {
        adms_return_free((p_return)myadmst->_original->_item.p);
        adms_admst_free(myadmst->_original);
      }
    else
      adms_message_fatal(("internal error"))
    }
  }
  if(myadmst->_hasnewbasicstring==1)
    free(myadmst->_item.s);
  else if(myadmst->_hasnewbasicstring==2)
    adms_return_free((p_return)myadmst->_item.p);
  adms_admst_free(myadmst);
}
#define p_basicstring char*
#define adms_basicstring_uid(e) ((char*)e)
static p_admst adms_basicenumeration_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(basicenumeration) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else if(adms_slist_length(myinputs[0])!=1)
  {
    adms_message_fatal_continue(("admst:new(basicenumeration) bad number of element in inputs[0] (found %i, expected 1)\n",adms_slist_length(myinputs[0])))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char*s=aprintf(mytransform,(p_admst)(myinputs[0]->data));
    /*xnew*/p_admst myadmst=adms_admst_newbi(dot,dot,adms_strtol(mytransform,s));
    free(s);
    deref((p_admst)myinputs[0]->data);
    adms_slist_free(myinputs[0]);
    return myadmst;
  }
}
static p_admst adms_basicinteger_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(basicinteger) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else if(adms_slist_length(myinputs[0])!=1)
  {
    adms_message_fatal_continue(("admst:new(basicinteger) bad number of element in inputs[0] (found %i, expected 1)\n",adms_slist_length(myinputs[0])))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char*s=aprintf(mytransform,(p_admst)(myinputs[0]->data));
    /*xnew*/p_admst myadmst=adms_admst_newbi(dot,dot,adms_strtol(mytransform,s));
    free(s);
    deref((p_admst)myinputs[0]->data);
    adms_slist_free(myinputs[0]);
    return myadmst;
  }
}
static p_admst adms_basicreal_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(basicreal) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else if(adms_slist_length(myinputs[0])!=1)
  {
    adms_message_fatal_continue(("admst:new(basicreal) bad number of element in inputs[0] (found %i, expected 1)\n",adms_slist_length(myinputs[0])))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char*s=aprintf(mytransform,(p_admst)(myinputs[0]->data));
    /*xnew*/p_admst myadmst=adms_admst_newbr(dot,dot,adms_strtod(mytransform,s));
    free(s);
    deref((p_admst)myinputs[0]->data);
    adms_slist_free(myinputs[0]);
    return myadmst;
  }
}
static p_admst adms_basicstring_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(basicstring) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else if(adms_slist_length(myinputs[0])!=1)
  {
    adms_message_fatal_continue(("admst:new(basicstring) bad number of element in inputs[0] (found %i, expected 1)\n",adms_slist_length(myinputs[0])))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char*s=aprintf(mytransform,(p_admst)(myinputs[0]->data));
    /*xnew*/p_admst myadmst=adms_admst_newbs(dot,dot,s);
    deref((p_admst)myinputs[0]->data);
    adms_slist_free(myinputs[0]);
    return myadmst;
  }
}
static int adms_basicenumeration_cmp (const char* mybasicenumeration,const char* refbasicenumeration)
{
  return (mybasicenumeration!=refbasicenumeration);
}
static int adms_basicinteger_cmp (const char* mybasicinteger,const char* refbasicinteger)
{
  return (mybasicinteger!=refbasicinteger);
}
static int adms_basicreal_cmp (const char* mybasicreal,const char* refbasicreal)
{
  return ((mybasicreal-refbasicreal)<0)||((mybasicreal-refbasicreal)>0);
}
static int adms_basicstring_cmp (const char* mybasicstring,const char* refbasicstring)
{
  return (strcmp(mybasicstring,refbasicstring));
}
static int adms_simulator_cmp (p_simulator mysimulator,p_simulator refsimulator)
{
  return
  (
    strcmp(mysimulator->_name,refsimulator->_name)
  );
}
static int adms_admsmain_cmp (p_admsmain myadmsmain,p_admsmain refadmsmain)
{
  return
  (
    strcmp(myadmsmain->_name,refadmsmain->_name)
  );
}
static int adms_list_cmp (p_list mylist,p_list reflist)
{
  return
  (
    strcmp(mylist->_datatype,reflist->_datatype)
  );
}
static int adms_nature_cmp (p_nature mynature,p_nature refnature)
{
  return
  (
    strcmp(mynature->_access,refnature->_access)
  );
}
static int adms_discipline_cmp (p_discipline mydiscipline,p_discipline refdiscipline)
{
  return
  (
    strcmp(mydiscipline->_name,refdiscipline->_name)
  );
}
static int adms_module_cmp (p_module mymodule,p_module refmodule)
{
  return
  (
    strcmp(mymodule->_name,refmodule->_name)
  );
}
static int adms_analogfunction_cmp (p_analogfunction myanalogfunction,p_analogfunction refanalogfunction)
{
  return
  (
    (myanalogfunction->_module != refanalogfunction->_module)
    ||
    strcmp(myanalogfunction->_lexval->_string,refanalogfunction->_lexval->_string)
  );
}
static int adms_nodealias_cmp (p_nodealias mynodealias,p_nodealias refnodealias)
{
  return
  (
    (mynodealias->_module != refnodealias->_module)
    ||
    strcmp(mynodealias->_name,refnodealias->_name)
  );
}
static int adms_node_cmp (p_node mynode,p_node refnode)
{
  return
  (
    (mynode->_module != refnode->_module)
    ||
    strcmp(mynode->_name,refnode->_name)
  );
}
static int adms_branchalias_cmp (p_branchalias mybranchalias,p_branchalias refbranchalias)
{
  return
  (
    (mybranchalias->_module != refbranchalias->_module)
    ||
    strcmp(mybranchalias->_name,refbranchalias->_name)
  );
}
static int adms_branch_cmp (p_branch mybranch,p_branch refbranch)
{
  return
  (
    (mybranch->_module != refbranch->_module)
    ||
    (mybranch->_pnode != refbranch->_pnode)
    ||
    (mybranch->_nnode != refbranch->_nnode)
  );
}
static int adms_variableprototype_cmp (p_variableprototype myvariableprototype,p_variableprototype refvariableprototype)
{
  return
  (
    (myvariableprototype->_module != refvariableprototype->_module)
    ||
    strcmp(myvariableprototype->_lexval->_string,refvariableprototype->_lexval->_string)
    ||
    (myvariableprototype->_block != refvariableprototype->_block)
  );
}
static int adms_source_cmp (p_source mysource,p_source refsource)
{
  return
  (
    (mysource->_module != refsource->_module)
    ||
    (mysource->_branch != refsource->_branch)
    ||
    (mysource->_nature != refsource->_nature)
  );
}
static int adms_range_cmp (p_range myrange,p_range refrange)
{
  return
  (
    (myrange->_module != refrange->_module)
    ||
    (myrange->_infexpr != refrange->_infexpr)
    ||
    (myrange->_supexpr != refrange->_supexpr)
  );
}
static int adms_jacobian_cmp (p_jacobian myjacobian,p_jacobian refjacobian)
{
  return
  (
    (myjacobian->_module != refjacobian->_module)
    ||
    (myjacobian->_row != refjacobian->_row)
    ||
    (myjacobian->_column != refjacobian->_column)
  );
}
static int adms_analog_cmp (p_analog myanalog,p_analog refanalog)
{
  return
  (
    (myanalog->_code != refanalog->_code)
  );
}
static int adms_math_cmp (p_math mymath,p_math refmath)
{
  return
  (
    strcmp(mymath->_name,refmath->_name)
  );
}
static int adms_string_cmp (p_string mystring,p_string refstring)
{
  return
  (
    strcmp(mystring->_value,refstring->_value)
  );
}
static int adms_subexpression_cmp (p_subexpression mysubexpression,p_subexpression refsubexpression)
{
  return
  (
    (mysubexpression->_expression != refsubexpression->_expression)
  );
}
static int adms_mapply_unary_cmp (p_mapply_unary mymapply_unary,p_mapply_unary refmapply_unary)
{
  return
  (
    (mymapply_unary->_name != refmapply_unary->_name)
    ||
    (mymapply_unary->_arg1 != refmapply_unary->_arg1)
  );
}
static int adms_mapply_binary_cmp (p_mapply_binary mymapply_binary,p_mapply_binary refmapply_binary)
{
  return
  (
    (mymapply_binary->_name != refmapply_binary->_name)
    ||
    (mymapply_binary->_arg1 != refmapply_binary->_arg1)
    ||
    (mymapply_binary->_arg2 != refmapply_binary->_arg2)
  );
}
static int adms_mapply_ternary_cmp (p_mapply_ternary mymapply_ternary,p_mapply_ternary refmapply_ternary)
{
  return
  (
    (mymapply_ternary->_name != refmapply_ternary->_name)
    ||
    (mymapply_ternary->_arg1 != refmapply_ternary->_arg1)
    ||
    (mymapply_ternary->_arg2 != refmapply_ternary->_arg2)
    ||
    (mymapply_ternary->_arg3 != refmapply_ternary->_arg3)
  );
}
static int adms_number_cmp (p_number mynumber,p_number refnumber)
{
  return
  (
    strcmp(mynumber->_lexval->_string,refnumber->_lexval->_string)
  );
}
static int adms_function_cmp (p_function myfunction,p_function reffunction)
{
  return
  (
    strcmp(myfunction->_lexval->_string,reffunction->_lexval->_string)
    ||
    (myfunction->_unique_id != reffunction->_unique_id)
  );
}
static int adms_variable_cmp (p_variable myvariable,p_variable refvariable)
{
  return
  (
    (myvariable->_prototype != refvariable->_prototype)
  );
}
static int adms_array_cmp (p_array myarray,p_array refarray)
{
  return
  (
    (myarray->_variable != refarray->_variable)
    ||
    (myarray->_index != refarray->_index)
  );
}
static int adms_probe_cmp (p_probe myprobe,p_probe refprobe)
{
  return
  (
    (myprobe->_module != refprobe->_module)
    ||
    (myprobe->_branch != refprobe->_branch)
    ||
    (myprobe->_nature != refprobe->_nature)
  );
}
static int adms_expression_cmp (p_expression myexpression,p_expression refexpression)
{
  return
  (
    (myexpression->_module != refexpression->_module)
    ||
    (myexpression->_tree != refexpression->_tree)
  );
}
static int adms_instance_cmp (p_instance myinstance,p_instance refinstance)
{
  return
  (
    (myinstance->_instantiator != refinstance->_instantiator)
    ||
    (myinstance->_module != refinstance->_module)
    ||
    strcmp(myinstance->_name,refinstance->_name)
  );
}
static int adms_instanceparameter_cmp (p_instanceparameter myinstanceparameter,p_instanceparameter refinstanceparameter)
{
  return
  (
    (myinstanceparameter->_parameter != refinstanceparameter->_parameter)
  );
}
static int adms_instancenode_cmp (p_instancenode myinstancenode,p_instancenode refinstancenode)
{
  return
  (
    (myinstancenode->_nodefrominstantiator != refinstancenode->_nodefrominstantiator)
    ||
    (myinstancenode->_nodefrommodule != refinstancenode->_nodefrommodule)
  );
}
static int adms_nilled_cmp (p_nilled mynilled,p_nilled refnilled)
{
  return
  (
    (mynilled->_module != refnilled->_module)
  );
}
static int adms_assignment_cmp (p_assignment myassignment,p_assignment refassignment)
{
  return
  (
    (myassignment->_module != refassignment->_module)
    ||
    (myassignment->_lhs != refassignment->_lhs)
    ||
    (myassignment->_rhs != refassignment->_rhs)
    ||
    strcmp(myassignment->_lexval->_string,refassignment->_lexval->_string)
  );
}
static int adms_contribution_cmp (p_contribution mycontribution,p_contribution refcontribution)
{
  return
  (
    (mycontribution->_module != refcontribution->_module)
    ||
    (mycontribution->_lhs != refcontribution->_lhs)
    ||
    (mycontribution->_rhs != refcontribution->_rhs)
    ||
    strcmp(mycontribution->_lexval->_string,refcontribution->_lexval->_string)
  );
}
static int adms_conditional_cmp (p_conditional myconditional,p_conditional refconditional)
{
  return
  (
    (myconditional->_module != refconditional->_module)
    ||
    (myconditional->_if != refconditional->_if)
    ||
    (myconditional->_then != refconditional->_then)
    ||
    (myconditional->_else != refconditional->_else)
  );
}
static int adms_forloop_cmp (p_forloop myforloop,p_forloop refforloop)
{
  return
  (
    (myforloop->_module != refforloop->_module)
    ||
    (myforloop->_initial != refforloop->_initial)
    ||
    (myforloop->_condition != refforloop->_condition)
    ||
    (myforloop->_update != refforloop->_update)
    ||
    (myforloop->_forblock != refforloop->_forblock)
  );
}
static int adms_whileloop_cmp (p_whileloop mywhileloop,p_whileloop refwhileloop)
{
  return
  (
    (mywhileloop->_module != refwhileloop->_module)
    ||
    (mywhileloop->_while != refwhileloop->_while)
    ||
    (mywhileloop->_whileblock != refwhileloop->_whileblock)
  );
}
static int adms_case_cmp (p_case mycase,p_case refcase)
{
  return
  (
    (mycase->_module != refcase->_module)
    ||
    (mycase->_case != refcase->_case)
  );
}
static int adms_caseitem_cmp (p_caseitem mycaseitem,p_caseitem refcaseitem)
{
  return
  (
    (mycaseitem->_code != refcaseitem->_code)
  );
}
static int adms_blockvariable_cmp (p_blockvariable myblockvariable,p_blockvariable refblockvariable)
{
  return
  (
    (myblockvariable->_block != refblockvariable->_block)
  );
}
static int adms_block_cmp (p_block myblock,p_block refblock)
{
  return
  (
    (myblock->_module != refblock->_module)
    ||
    strcmp(myblock->_lexval->_string,refblock->_lexval->_string)
    ||
    (myblock->_block != refblock->_block)
    ||
    (myblock->_item != refblock->_item)
  );
}
static int adms_callfunction_cmp (p_callfunction mycallfunction,p_callfunction refcallfunction)
{
  return
  (
    (mycallfunction->_module != refcallfunction->_module)
    ||
    (mycallfunction->_function != refcallfunction->_function)
  );
}
static int adms_evaluation_cmp (p_evaluation myevaluation,p_evaluation refevaluation)
{
  return
  (
    1
  );
}
static int adms_attribute_cmp (p_attribute myattribute,p_attribute refattribute)
{
  return
  (
    strcmp(myattribute->_name,refattribute->_name)
  );
}
static int adms_lexval_cmp (p_lexval mylexval,p_lexval reflexval)
{
  return
  (
    strcmp(mylexval->_string,reflexval->_string)
    ||
    strcmp(mylexval->_f,reflexval->_f)
    ||
    (mylexval->_l != reflexval->_l)
    ||
    (mylexval->_c != reflexval->_c)
  );
}

/* ------- adms -------------- */
void adms_adms_free (p_adms myadms)
{
  if(!myadms)
    return;
  adms_slist_free(myadms->_attribute);
  adms_slist_free(myadms->_variable);
  free(myadms);
}

void adms_adms_valueto_datatypename (p_adms myadms,int _datatypename)
{
  myadms->_datatypename=_datatypename;
}

void adms_adms_valueto_attribute (p_adms myadms,p_slist _attribute)
{
  myadms->_attribute=_attribute;
}

void adms_adms_valueto_variable (p_adms myadms,p_slist _variable)
{
  myadms->_variable=_variable;
}

/* ------- simulator -------------- */
void adms_simulator_free (p_simulator mysimulator)
{
  if(!mysimulator)
    return;
  free(mysimulator->_name);
  free(mysimulator->_currentdate);
  free(mysimulator->_developer);
  free(mysimulator->_fullname);
  free(mysimulator->_package_name);
  free(mysimulator->_package_tarname);
  free(mysimulator->_package_version);
  free(mysimulator->_package_string);
  free(mysimulator->_package_bugreport);
  free(mysimulator->_tmp);
  free(mysimulator->_tmp1);
  free(mysimulator->_ddx);
  free(mysimulator);
}

void adms_simulator_valueto_name (p_simulator mysimulator,const char* _name)
{
  mysimulator->_name=adms_kclone(_name);
  mysimulator->_name_isdefault=0;
}

void adms_simulator_valueto_currentdate (p_simulator mysimulator,const char* _currentdate)
{
  mysimulator->_currentdate=adms_kclone(_currentdate);
  mysimulator->_currentdate_isdefault=0;
}

void adms_simulator_valueto_developer (p_simulator mysimulator,const char* _developer)
{
  mysimulator->_developer=adms_kclone(_developer);
  mysimulator->_developer_isdefault=0;
}

void adms_simulator_valueto_fullname (p_simulator mysimulator,const char* _fullname)
{
  mysimulator->_fullname=adms_kclone(_fullname);
  mysimulator->_fullname_isdefault=0;
}

void adms_simulator_valueto_package_name (p_simulator mysimulator,const char* _package_name)
{
  mysimulator->_package_name=adms_kclone(_package_name);
  mysimulator->_package_name_isdefault=0;
}

void adms_simulator_valueto_package_tarname (p_simulator mysimulator,const char* _package_tarname)
{
  mysimulator->_package_tarname=adms_kclone(_package_tarname);
  mysimulator->_package_tarname_isdefault=0;
}

void adms_simulator_valueto_package_version (p_simulator mysimulator,const char* _package_version)
{
  mysimulator->_package_version=adms_kclone(_package_version);
  mysimulator->_package_version_isdefault=0;
}

void adms_simulator_valueto_package_string (p_simulator mysimulator,const char* _package_string)
{
  mysimulator->_package_string=adms_kclone(_package_string);
  mysimulator->_package_string_isdefault=0;
}

void adms_simulator_valueto_package_bugreport (p_simulator mysimulator,const char* _package_bugreport)
{
  mysimulator->_package_bugreport=adms_kclone(_package_bugreport);
  mysimulator->_package_bugreport_isdefault=0;
}

void adms_simulator_valueto_probe (p_simulator mysimulator,p_probe _probe)
{
  mysimulator->_probe=_probe;
  mysimulator->_probe_isdefault=0;
}

void adms_simulator_valueto_tmp (p_simulator mysimulator,const char* _tmp)
{
  mysimulator->_tmp=adms_kclone(_tmp);
  mysimulator->_tmp_isdefault=0;
}

void adms_simulator_valueto_tmp1 (p_simulator mysimulator,const char* _tmp1)
{
  mysimulator->_tmp1=adms_kclone(_tmp1);
  mysimulator->_tmp1_isdefault=0;
}

void adms_simulator_valueto_br1 (p_simulator mysimulator,p_branch _br1)
{
  mysimulator->_br1=_br1;
  mysimulator->_br1_isdefault=0;
}

void adms_simulator_valueto_br2 (p_simulator mysimulator,p_branch _br2)
{
  mysimulator->_br2=_br2;
  mysimulator->_br2_isdefault=0;
}

void adms_simulator_valueto_sr1 (p_simulator mysimulator,p_source _sr1)
{
  mysimulator->_sr1=_sr1;
  mysimulator->_sr1_isdefault=0;
}

void adms_simulator_valueto_ddx (p_simulator mysimulator,const char* _ddx)
{
  mysimulator->_ddx=adms_kclone(_ddx);
  mysimulator->_ddx_isdefault=0;
}
p_simulator adms_simulator_new (const char* myname)
{
  p_simulator mynewsimulator=(p_simulator) calloc(1,(size_t)sizeof(t_simulator));
  mynewsimulator->_adms._datatypename=admse_simulator;
  mynewsimulator->_name=adms_kclone(myname);
  mynewsimulator->_name_isdefault=1;
  mynewsimulator->_currentdate_isdefault=-1;
  mynewsimulator->_developer_isdefault=-1;
  mynewsimulator->_fullname_isdefault=-1;
  mynewsimulator->_package_name_isdefault=-1;
  mynewsimulator->_package_tarname_isdefault=-1;
  mynewsimulator->_package_version_isdefault=-1;
  mynewsimulator->_package_string_isdefault=-1;
  mynewsimulator->_package_bugreport_isdefault=-1;
  mynewsimulator->_probe_isdefault=-1;
  mynewsimulator->_tmp=adms_kclone("");
  mynewsimulator->_tmp_isdefault=1;
  mynewsimulator->_tmp1=adms_kclone("");
  mynewsimulator->_tmp1_isdefault=1;
  mynewsimulator->_br1_isdefault=-1;
  mynewsimulator->_br2_isdefault=-1;
  mynewsimulator->_sr1_isdefault=-1;
  mynewsimulator->_ddx=adms_kclone("");
  mynewsimulator->_ddx_isdefault=1;
  return mynewsimulator;
}
char*adms_simulator_uid (p_simulator mynewsimulator)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,mynewsimulator->_name);
  return myuid;
}
static p_admst adms_simulator_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(simulator) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char* myname=aprintf(mytransform,(p_admst)myinputs[0]->data);
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_simulator_new(myname));
    free(myname);
    return myadmst;
  }
}

/* ------- admsmain -------------- */
void adms_admsmain_free (p_admsmain myadmsmain)
{
  if(!myadmsmain)
    return;
  free(myadmsmain->_name);
  free(myadmsmain->_fullfilename);
  free(myadmsmain->_filename);
  free(myadmsmain->_curfilename);
  adms_slist_free(myadmsmain->_module);
  adms_slist_free(myadmsmain->_discipline);
  adms_slist_free(myadmsmain->_nature);
  adms_slist_free(myadmsmain->_variable);
  adms_slist_free(myadmsmain->_argv);
  adms_slist_free(myadmsmain->_transform);
  adms_slist_free(myadmsmain->_invtransform);
  adms_slist_free(myadmsmain->_itransform);
  adms_slist_free(myadmsmain->_itransforminsideapplytemplate);
  adms_slist_free(myadmsmain->_valueof);
  free(myadmsmain);
}

void adms_admsmain_valueto_name (p_admsmain myadmsmain,const char* _name)
{
  myadmsmain->_name=adms_kclone(_name);
  myadmsmain->_name_isdefault=0;
}

void adms_admsmain_valueto_argc (p_admsmain myadmsmain,int _argc)
{
  myadmsmain->_argc=_argc;
  myadmsmain->_argc_isdefault=0;
}

void adms_admsmain_valueto_fullfilename (p_admsmain myadmsmain,const char* _fullfilename)
{
  myadmsmain->_fullfilename=adms_kclone(_fullfilename);
  myadmsmain->_fullfilename_isdefault=0;
}

void adms_admsmain_valueto_filename (p_admsmain myadmsmain,const char* _filename)
{
  myadmsmain->_filename=adms_kclone(_filename);
  myadmsmain->_filename_isdefault=0;
}

void adms_admsmain_valueto_curfilename (p_admsmain myadmsmain,const char* _curfilename)
{
  myadmsmain->_curfilename=adms_kclone(_curfilename);
  myadmsmain->_curfilename_isdefault=0;
}

void adms_admsmain_valueto_curline (p_admsmain myadmsmain,int _curline)
{
  myadmsmain->_curline=_curline;
  myadmsmain->_curline_isdefault=0;
}

void adms_admsmain_valueto_fpos (p_admsmain myadmsmain,int _fpos)
{
  myadmsmain->_fpos=_fpos;
  myadmsmain->_fpos_isdefault=0;
}

void adms_admsmain_valueto_simulator (p_admsmain myadmsmain,p_simulator _simulator)
{
  myadmsmain->_simulator=_simulator;
  myadmsmain->_simulator_isdefault=0;
}

void adms_admsmain_valueto_info (p_admsmain myadmsmain,admse _info)
{
  myadmsmain->_info=_info;
  myadmsmain->_info_isdefault=0;
}

void adms_admsmain_valueto_usage (p_admsmain myadmsmain,admse _usage)
{
  myadmsmain->_usage=_usage;
  myadmsmain->_usage_isdefault=0;
}

void adms_admsmain_valueto_verbose (p_admsmain myadmsmain,admse _verbose)
{
  myadmsmain->_verbose=_verbose;
  myadmsmain->_verbose_isdefault=0;
}

void adms_admsmain_valueto_hint (p_admsmain myadmsmain,admse _hint)
{
  myadmsmain->_hint=_hint;
  myadmsmain->_hint_isdefault=0;
}

void adms_admsmain_valueto_warning (p_admsmain myadmsmain,admse _warning)
{
  myadmsmain->_warning=_warning;
  myadmsmain->_warning_isdefault=0;
}

void adms_admsmain_valueto_obsolete (p_admsmain myadmsmain,admse _obsolete)
{
  myadmsmain->_obsolete=_obsolete;
  myadmsmain->_obsolete_isdefault=0;
}

void adms_admsmain_valueto_debug (p_admsmain myadmsmain,admse _debug)
{
  myadmsmain->_debug=_debug;
  myadmsmain->_debug_isdefault=0;
}

void adms_admsmain_valueto_dbg_vla (p_admsmain myadmsmain,admse _dbg_vla)
{
  myadmsmain->_dbg_vla=_dbg_vla;
  myadmsmain->_dbg_vla_isdefault=0;
}

void adms_admsmain_valueto_dbg_xml (p_admsmain myadmsmain,admse _dbg_xml)
{
  myadmsmain->_dbg_xml=_dbg_xml;
  myadmsmain->_dbg_xml_isdefault=0;
}

void adms_admsmain_valueto_error (p_admsmain myadmsmain,admse _error)
{
  myadmsmain->_error=_error;
  myadmsmain->_error_isdefault=0;
}

void adms_admsmain_valueto_fatal (p_admsmain myadmsmain,admse _fatal)
{
  myadmsmain->_fatal=_fatal;
  myadmsmain->_fatal_isdefault=0;
}

void adms_admsmain_valueto_dbg_mem (p_admsmain myadmsmain,admse _dbg_mem)
{
  myadmsmain->_dbg_mem=_dbg_mem;
  myadmsmain->_dbg_mem_isdefault=0;
}

void adms_admsmain_valueto_module (p_admsmain myadmsmain,p_slist _module)
{
  myadmsmain->_module=_module;
  myadmsmain->_module_isdefault=0;
}
p_module adms_admsmain_list_module_prepend_by_id_once_or_abort (p_admsmain mymyadmsmain,const char* myname)
{
  p_slist list=mymyadmsmain->_module;
  p_module refmodule=adms_module_new(myname);
  while(list)
  {
    if(!adms_module_cmp((p_module)list->data,refmodule))
    {
      adms_message_fatal(("admsmain=[%s] module=[%s] already defined\n",
        adms_admsmain_uid(mymyadmsmain),adms_module_uid(refmodule)))
      adms_module_free (refmodule);
    }
    list=list->next;
  }
  adms_slist_push(&mymyadmsmain->_module,(p_adms)refmodule);
  return refmodule;
}
p_module adms_admsmain_list_module_lookup_by_id (p_admsmain mymyadmsmain,const char* myname)
{
  p_slist list=mymyadmsmain->_module;
  while(list) {
    p_module refmodule=(p_module)list->data;
    if(!strcmp(refmodule->_name,myname))
      return refmodule;
    list=list->next;
  }
  return NULL;
}

void adms_admsmain_valueto_discipline (p_admsmain myadmsmain,p_slist _discipline)
{
  myadmsmain->_discipline=_discipline;
  myadmsmain->_discipline_isdefault=0;
}
void adms_admsmain_list_discipline_prepend_once_or_abort (p_admsmain mymyadmsmain,p_discipline mydiscipline)
{
  p_slist list=mymyadmsmain->_discipline;
  while(list)
  {
    if(!adms_discipline_cmp((p_discipline)list->data,mydiscipline))
    {
      adms_message_fatal(("admsmain=[%s] discipline=[%s] already defined\n",
        adms_admsmain_uid(mymyadmsmain),adms_discipline_uid(mydiscipline)))
      return;
    }
    list=list->next;
  }
  adms_slist_push(&mymyadmsmain->_discipline,(p_adms)mydiscipline);
}
p_discipline adms_admsmain_list_discipline_lookup_by_id (p_admsmain mymyadmsmain,const char* myname)
{
  p_slist list=mymyadmsmain->_discipline;
  while(list) {
    p_discipline refdiscipline=(p_discipline)list->data;
    if(!strcmp(refdiscipline->_name,myname))
      return refdiscipline;
    list=list->next;
  }
  return NULL;
}

void adms_admsmain_valueto_nature (p_admsmain myadmsmain,p_slist _nature)
{
  myadmsmain->_nature=_nature;
  myadmsmain->_nature_isdefault=0;
}
p_nature adms_admsmain_list_nature_prepend_by_id_once_or_abort (p_admsmain mymyadmsmain,const char* myaccess)
{
  p_slist list=mymyadmsmain->_nature;
  p_nature refnature=adms_nature_new(myaccess);
  while(list)
  {
    if(!adms_nature_cmp((p_nature)list->data,refnature))
    {
      adms_message_fatal(("admsmain=[%s] nature=[%s] already defined\n",
        adms_admsmain_uid(mymyadmsmain),adms_nature_uid(refnature)))
      adms_nature_free (refnature);
    }
    list=list->next;
  }
  adms_slist_push(&mymyadmsmain->_nature,(p_adms)refnature);
  return refnature;
}
p_nature adms_admsmain_list_nature_lookup_by_id (p_admsmain mymyadmsmain,const char* myaccess)
{
  p_slist list=mymyadmsmain->_nature;
  while(list) {
    p_nature refnature=(p_nature)list->data;
    if(!strcmp(refnature->_access,myaccess))
      return refnature;
    list=list->next;
  }
  return NULL;
}

void adms_admsmain_valueto_variable (p_admsmain myadmsmain,p_slist _variable)
{
  myadmsmain->_variable=_variable;
  myadmsmain->_variable_isdefault=0;
}

void adms_admsmain_valueto_argv (p_admsmain myadmsmain,p_slist _argv)
{
  myadmsmain->_argv=_argv;
  myadmsmain->_argv_isdefault=0;
}

void adms_admsmain_valueto_transform (p_admsmain myadmsmain,p_slist _transform)
{
  myadmsmain->_transform=_transform;
  myadmsmain->_transform_isdefault=0;
}

void adms_admsmain_valueto_invtransform (p_admsmain myadmsmain,p_slist _invtransform)
{
  myadmsmain->_invtransform=_invtransform;
  myadmsmain->_invtransform_isdefault=0;
}

void adms_admsmain_valueto_itransform (p_admsmain myadmsmain,p_slist _itransform)
{
  myadmsmain->_itransform=_itransform;
  myadmsmain->_itransform_isdefault=0;
}

void adms_admsmain_valueto_itransforminsideapplytemplate (p_admsmain myadmsmain,p_slist _itransforminsideapplytemplate)
{
  myadmsmain->_itransforminsideapplytemplate=_itransforminsideapplytemplate;
  myadmsmain->_itransforminsideapplytemplate_isdefault=0;
}

void adms_admsmain_valueto_valueof (p_admsmain myadmsmain,p_slist _valueof)
{
  myadmsmain->_valueof=_valueof;
  myadmsmain->_valueof_isdefault=0;
}
p_admsmain adms_admsmain_new (const char* myname)
{
  p_admsmain mynewadmsmain=(p_admsmain) calloc(1,(size_t)sizeof(t_admsmain));
  mynewadmsmain->_adms._datatypename=admse_admsmain;
  mynewadmsmain->_name=adms_kclone(myname);
  mynewadmsmain->_name_isdefault=1;
  mynewadmsmain->_argc_isdefault=-1;
  mynewadmsmain->_fullfilename_isdefault=-1;
  mynewadmsmain->_filename_isdefault=-1;
  mynewadmsmain->_curfilename_isdefault=-1;
  mynewadmsmain->_curline_isdefault=-1;
  mynewadmsmain->_fpos_isdefault=-1;
  mynewadmsmain->_simulator_isdefault=-1;
  mynewadmsmain->_info=admse_yes;
  mynewadmsmain->_info_isdefault=1;
  mynewadmsmain->_usage=admse_yes;
  mynewadmsmain->_usage_isdefault=1;
  mynewadmsmain->_verbose=admse_no;
  mynewadmsmain->_verbose_isdefault=1;
  mynewadmsmain->_hint=admse_no;
  mynewadmsmain->_hint_isdefault=1;
  mynewadmsmain->_warning=admse_yes;
  mynewadmsmain->_warning_isdefault=1;
  mynewadmsmain->_obsolete=admse_yes;
  mynewadmsmain->_obsolete_isdefault=1;
  mynewadmsmain->_debug=admse_no;
  mynewadmsmain->_debug_isdefault=1;
  mynewadmsmain->_dbg_vla=admse_no;
  mynewadmsmain->_dbg_vla_isdefault=1;
  mynewadmsmain->_dbg_xml=admse_no;
  mynewadmsmain->_dbg_xml_isdefault=1;
  mynewadmsmain->_error=admse_yes;
  mynewadmsmain->_error_isdefault=1;
  mynewadmsmain->_fatal=admse_yes;
  mynewadmsmain->_fatal_isdefault=1;
  mynewadmsmain->_dbg_mem=admse_no;
  mynewadmsmain->_dbg_mem_isdefault=1;
  mynewadmsmain->_module_isdefault=-1;
  mynewadmsmain->_discipline_isdefault=-1;
  mynewadmsmain->_nature_isdefault=-1;
  mynewadmsmain->_variable_isdefault=-1;
  mynewadmsmain->_argv_isdefault=-1;
  mynewadmsmain->_transform_isdefault=-1;
  mynewadmsmain->_invtransform_isdefault=-1;
  mynewadmsmain->_itransform_isdefault=-1;
  mynewadmsmain->_itransforminsideapplytemplate_isdefault=-1;
  mynewadmsmain->_valueof_isdefault=-1;
  return mynewadmsmain;
}
char*adms_admsmain_uid (p_admsmain mynewadmsmain)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,mynewadmsmain->_name);
  return myuid;
}
static p_admst adms_admsmain_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(admsmain) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char* myname=aprintf(mytransform,(p_admst)myinputs[0]->data);
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_admsmain_new(myname));
    free(myname);
    return myadmst;
  }
}

/* ------- list -------------- */
void adms_list_free (p_list mylist)
{
  if(!mylist)
    return;
  free(mylist->_datatype);
  adms_slist_free(mylist->_item);
  free(mylist);
}

void adms_list_valueto_datatype (p_list mylist,const char* _datatype)
{
  mylist->_datatype=adms_kclone(_datatype);
  mylist->_datatype_isdefault=0;
}

void adms_list_valueto_item (p_list mylist,p_slist _item)
{
  mylist->_item=_item;
  mylist->_item_isdefault=0;
}
p_list adms_list_new (const char* mydatatype)
{
  p_list mynewlist=(p_list) calloc(1,(size_t)sizeof(t_list));
  mynewlist->_adms._datatypename=admse_list;
  mynewlist->_datatype=adms_kclone(mydatatype);
  mynewlist->_datatype_isdefault=1;
  mynewlist->_item_isdefault=-1;
  return mynewlist;
}
char*adms_list_uid (p_list mynewlist)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,mynewlist->_datatype);
  return myuid;
}
static p_admst adms_list_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(list) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char* mydatatype=aprintf(mytransform,(p_admst)myinputs[0]->data);
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_list_new(mydatatype));
    free(mydatatype);
    return myadmst;
  }
}

/* ------- nature -------------- */
void adms_nature_free (p_nature mynature)
{
  if(!mynature)
    return;
  free(mynature->_access);
  free(mynature->_name);
  free(mynature->_ddt_name);
  free(mynature->_idt_name);
  free(mynature->_units);
  free(mynature);
}

void adms_nature_valueto_access (p_nature mynature,const char* _access)
{
  mynature->_access=adms_kclone(_access);
  mynature->_access_isdefault=0;
}

void adms_nature_valueto_name (p_nature mynature,const char* _name)
{
  mynature->_name=adms_kclone(_name);
  mynature->_name_isdefault=0;
}

void adms_nature_valueto_abstol (p_nature mynature,p_number _abstol)
{
  mynature->_abstol=_abstol;
  mynature->_abstol_isdefault=0;
}

void adms_nature_valueto_base (p_nature mynature,p_nature _base)
{
  mynature->_base=_base;
  mynature->_base_isdefault=0;
}

void adms_nature_valueto_ddt_name (p_nature mynature,const char* _ddt_name)
{
  mynature->_ddt_name=adms_kclone(_ddt_name);
  mynature->_ddt_name_isdefault=0;
}

void adms_nature_valueto_ddt_nature (p_nature mynature,p_nature _ddt_nature)
{
  mynature->_ddt_nature=_ddt_nature;
  mynature->_ddt_nature_isdefault=0;
}

void adms_nature_valueto_idt_name (p_nature mynature,const char* _idt_name)
{
  mynature->_idt_name=adms_kclone(_idt_name);
  mynature->_idt_name_isdefault=0;
}

void adms_nature_valueto_idt_nature (p_nature mynature,p_nature _idt_nature)
{
  mynature->_idt_nature=_idt_nature;
  mynature->_idt_nature_isdefault=0;
}

void adms_nature_valueto_units (p_nature mynature,const char* _units)
{
  mynature->_units=adms_kclone(_units);
  mynature->_units_isdefault=0;
}
p_nature adms_nature_new (const char* myaccess)
{
  p_nature mynewnature=(p_nature) calloc(1,(size_t)sizeof(t_nature));
  mynewnature->_adms._datatypename=admse_nature;
  mynewnature->_access=adms_kclone(myaccess);
  mynewnature->_access_isdefault=1;
  mynewnature->_name_isdefault=-1;
  mynewnature->_abstol_isdefault=-1;
  mynewnature->_base_isdefault=-1;
  mynewnature->_ddt_name_isdefault=-1;
  mynewnature->_ddt_nature_isdefault=-1;
  mynewnature->_idt_name_isdefault=-1;
  mynewnature->_idt_nature_isdefault=-1;
  mynewnature->_units_isdefault=-1;
  return mynewnature;
}
char*adms_nature_uid (p_nature mynewnature)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,mynewnature->_access);
  return myuid;
}
static p_admst adms_nature_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(nature) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char* myaccess=aprintf(mytransform,(p_admst)myinputs[0]->data);
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_nature_new(myaccess));
    free(myaccess);
    return myadmst;
  }
}

/* ------- discipline -------------- */
void adms_discipline_free (p_discipline mydiscipline)
{
  if(!mydiscipline)
    return;
  free(mydiscipline->_name);
  free(mydiscipline);
}

void adms_discipline_valueto_name (p_discipline mydiscipline,const char* _name)
{
  mydiscipline->_name=adms_kclone(_name);
  mydiscipline->_name_isdefault=0;
}

void adms_discipline_valueto_flow (p_discipline mydiscipline,p_nature _flow)
{
  mydiscipline->_flow=_flow;
  mydiscipline->_flow_isdefault=0;
}

void adms_discipline_valueto_potential (p_discipline mydiscipline,p_nature _potential)
{
  mydiscipline->_potential=_potential;
  mydiscipline->_potential_isdefault=0;
}

void adms_discipline_valueto_domain (p_discipline mydiscipline,admse _domain)
{
  mydiscipline->_domain=_domain;
  mydiscipline->_domain_isdefault=0;
}
p_discipline adms_discipline_new (const char* myname)
{
  p_discipline mynewdiscipline=(p_discipline) calloc(1,(size_t)sizeof(t_discipline));
  mynewdiscipline->_adms._datatypename=admse_discipline;
  mynewdiscipline->_name=adms_kclone(myname);
  mynewdiscipline->_name_isdefault=1;
  mynewdiscipline->_flow_isdefault=-1;
  mynewdiscipline->_potential_isdefault=-1;
  mynewdiscipline->_domain=admse_continuous;
  mynewdiscipline->_domain_isdefault=1;
  return mynewdiscipline;
}
char*adms_discipline_uid (p_discipline mynewdiscipline)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,mynewdiscipline->_name);
  return myuid;
}
static p_admst adms_discipline_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(discipline) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char* myname=aprintf(mytransform,(p_admst)myinputs[0]->data);
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_discipline_new(myname));
    free(myname);
    return myadmst;
  }
}

/* ------- module -------------- */
void adms_module_free (p_module mymodule)
{
  if(!mymodule)
    return;
  free(mymodule->_name);
  adms_slist_free(mymodule->_node);
  adms_slist_free(mymodule->_nodealias);
  adms_slist_free(mymodule->_branch);
  adms_slist_free(mymodule->_branchalias);
  adms_slist_free(mymodule->_analogfunction);
  adms_slist_free(mymodule->_instance);
  adms_slist_free(mymodule->_variable);
  adms_slist_free(mymodule->_block);
  adms_slist_free(mymodule->_blockvariable);
  adms_slist_free(mymodule->_assignment);
  adms_slist_free(mymodule->_callfunction);
  adms_slist_free(mymodule->_contribution);
  adms_slist_free(mymodule->_conditional);
  adms_slist_free(mymodule->_case);
  adms_slist_free(mymodule->_forloop);
  adms_slist_free(mymodule->_whileloop);
  adms_slist_free(mymodule->_instantiator);
  adms_slist_free(mymodule->_expression);
  adms_slist_free(mymodule->_jacobian);
  adms_slist_free(mymodule->_probe);
  adms_slist_free(mymodule->_source);
  adms_slist_free(mymodule->_range);
  adms_slist_free(mymodule->_attribute);
  free(mymodule);
}

void adms_module_valueto_name (p_module mymodule,const char* _name)
{
  mymodule->_name=adms_kclone(_name);
  mymodule->_name_isdefault=0;
}

void adms_module_valueto_node (p_module mymodule,p_slist _node)
{
  mymodule->_node=_node;
  mymodule->_node_isdefault=0;
}
p_node adms_module_list_node_prepend_by_id_once_or_ignore (p_module mymymodule,p_module mymodule,const char* myname)
{
  p_slist list=mymymodule->_node;
  p_node refnode=adms_node_new(mymodule,myname);
  while(list)
  {
    if(!adms_node_cmp((p_node)list->data,refnode))
    {
      adms_node_free (refnode);
      return (p_node)list->data;
    }
    list=list->next;
  }
  adms_slist_push(&mymymodule->_node,(p_adms)refnode);
  return refnode;
}
p_node adms_module_list_node_prepend_by_id_once_or_abort (p_module mymymodule,p_module mymodule,const char* myname)
{
  p_slist list=mymymodule->_node;
  p_node refnode=adms_node_new(mymodule,myname);
  while(list)
  {
    if(!adms_node_cmp((p_node)list->data,refnode))
    {
      adms_message_fatal(("module=[%s] node=[%s] already defined\n",
        adms_module_uid(mymymodule),adms_node_uid(refnode)))
      adms_node_free (refnode);
    }
    list=list->next;
  }
  adms_slist_push(&mymymodule->_node,(p_adms)refnode);
  return refnode;
}
p_node adms_module_list_node_lookup_by_id (p_module mymymodule,p_module mymodule,const char* myname)
{
  p_slist list=mymymodule->_node;
  while(list) {
    p_node refnode=(p_node)list->data;
    if((refnode->_module==mymodule)&&
       !strcmp(refnode->_name,myname))
      return refnode;
    list=list->next;
  }
  return NULL;
}

void adms_module_valueto_nodealias (p_module mymodule,p_slist _nodealias)
{
  mymodule->_nodealias=_nodealias;
  mymodule->_nodealias_isdefault=0;
}
p_nodealias adms_module_list_nodealias_prepend_by_id_once_or_ignore (p_module mymymodule,p_module mymodule,const char* myname)
{
  p_slist list=mymymodule->_nodealias;
  p_nodealias refnodealias=adms_nodealias_new(mymodule,myname);
  while(list)
  {
    if(!adms_nodealias_cmp((p_nodealias)list->data,refnodealias))
    {
      adms_nodealias_free (refnodealias);
      return (p_nodealias)list->data;
    }
    list=list->next;
  }
  adms_slist_push(&mymymodule->_nodealias,(p_adms)refnodealias);
  return refnodealias;
}
p_nodealias adms_module_list_nodealias_prepend_by_id_once_or_abort (p_module mymymodule,p_module mymodule,const char* myname)
{
  p_slist list=mymymodule->_nodealias;
  p_nodealias refnodealias=adms_nodealias_new(mymodule,myname);
  while(list)
  {
    if(!adms_nodealias_cmp((p_nodealias)list->data,refnodealias))
    {
      adms_message_fatal(("module=[%s] nodealias=[%s] already defined\n",
        adms_module_uid(mymymodule),adms_nodealias_uid(refnodealias)))
      adms_nodealias_free (refnodealias);
    }
    list=list->next;
  }
  adms_slist_push(&mymymodule->_nodealias,(p_adms)refnodealias);
  return refnodealias;
}
p_nodealias adms_module_list_nodealias_lookup_by_id (p_module mymymodule,p_module mymodule,const char* myname)
{
  p_slist list=mymymodule->_nodealias;
  while(list) {
    p_nodealias refnodealias=(p_nodealias)list->data;
    if((refnodealias->_module==mymodule)&&
       !strcmp(refnodealias->_name,myname))
      return refnodealias;
    list=list->next;
  }
  return NULL;
}

void adms_module_valueto_branch (p_module mymodule,p_slist _branch)
{
  mymodule->_branch=_branch;
  mymodule->_branch_isdefault=0;
}
p_branch adms_module_list_branch_prepend_by_id_once_or_ignore (p_module mymymodule,p_module mymodule,p_node mypnode,p_node mynnode)
{
  p_slist list=mymymodule->_branch;
  p_branch refbranch=adms_branch_new(mymodule,mypnode,mynnode);
  while(list)
  {
    if(!adms_branch_cmp((p_branch)list->data,refbranch))
    {
      adms_branch_free (refbranch);
      return (p_branch)list->data;
    }
    list=list->next;
  }
  adms_slist_push(&mymymodule->_branch,(p_adms)refbranch);
  return refbranch;
}

void adms_module_valueto_branchalias (p_module mymodule,p_slist _branchalias)
{
  mymodule->_branchalias=_branchalias;
  mymodule->_branchalias_isdefault=0;
}
p_branchalias adms_module_list_branchalias_prepend_by_id_once_or_ignore (p_module mymymodule,p_module mymodule,const char* myname)
{
  p_slist list=mymymodule->_branchalias;
  p_branchalias refbranchalias=adms_branchalias_new(mymodule,myname);
  while(list)
  {
    if(!adms_branchalias_cmp((p_branchalias)list->data,refbranchalias))
    {
      adms_branchalias_free (refbranchalias);
      return (p_branchalias)list->data;
    }
    list=list->next;
  }
  adms_slist_push(&mymymodule->_branchalias,(p_adms)refbranchalias);
  return refbranchalias;
}
p_branchalias adms_module_list_branchalias_prepend_by_id_once_or_abort (p_module mymymodule,p_module mymodule,const char* myname)
{
  p_slist list=mymymodule->_branchalias;
  p_branchalias refbranchalias=adms_branchalias_new(mymodule,myname);
  while(list)
  {
    if(!adms_branchalias_cmp((p_branchalias)list->data,refbranchalias))
    {
      adms_message_fatal(("module=[%s] branchalias=[%s] already defined\n",
        adms_module_uid(mymymodule),adms_branchalias_uid(refbranchalias)))
      adms_branchalias_free (refbranchalias);
    }
    list=list->next;
  }
  adms_slist_push(&mymymodule->_branchalias,(p_adms)refbranchalias);
  return refbranchalias;
}
p_branchalias adms_module_list_branchalias_lookup_by_id (p_module mymymodule,p_module mymodule,const char* myname)
{
  p_slist list=mymymodule->_branchalias;
  while(list) {
    p_branchalias refbranchalias=(p_branchalias)list->data;
    if((refbranchalias->_module==mymodule)&&
       !strcmp(refbranchalias->_name,myname))
      return refbranchalias;
    list=list->next;
  }
  return NULL;
}

void adms_module_valueto_analogfunction (p_module mymodule,p_slist _analogfunction)
{
  mymodule->_analogfunction=_analogfunction;
  mymodule->_analogfunction_isdefault=0;
}
void adms_module_list_analogfunction_prepend_once_or_abort (p_module mymymodule,p_analogfunction myanalogfunction)
{
  p_slist list=mymymodule->_analogfunction;
  while(list)
  {
    if(!adms_analogfunction_cmp((p_analogfunction)list->data,myanalogfunction))
    {
      adms_message_fatal(("module=[%s] analogfunction=[%s] already defined\n",
        adms_module_uid(mymymodule),adms_analogfunction_uid(myanalogfunction)))
      return;
    }
    list=list->next;
  }
  adms_slist_push(&mymymodule->_analogfunction,(p_adms)myanalogfunction);
}

void adms_module_valueto_instance (p_module mymodule,p_slist _instance)
{
  mymodule->_instance=_instance;
  mymodule->_instance_isdefault=0;
}
p_instance adms_module_list_instance_prepend_by_id_once_or_abort (p_module mymymodule,p_module myinstantiator,p_module mymodule,const char* myname)
{
  p_slist list=mymymodule->_instance;
  p_instance refinstance=adms_instance_new(myinstantiator,mymodule,myname);
  while(list)
  {
    if(!adms_instance_cmp((p_instance)list->data,refinstance))
    {
      adms_message_fatal(("module=[%s] instance=[%s] already defined\n",
        adms_module_uid(mymymodule),adms_instance_uid(refinstance)))
      adms_instance_free (refinstance);
    }
    list=list->next;
  }
  adms_slist_push(&mymymodule->_instance,(p_adms)refinstance);
  return refinstance;
}

void adms_module_valueto_variable (p_module mymodule,p_slist _variable)
{
  mymodule->_variable=_variable;
  mymodule->_variable_isdefault=0;
}
void adms_module_list_variable_prepend_once_or_abort (p_module mymymodule,p_variableprototype myvariable)
{
  p_slist list=mymymodule->_variable;
  while(list)
  {
    if(!adms_variableprototype_cmp((p_variableprototype)list->data,myvariable))
    {
      adms_message_fatal(("module=[%s] variable=[%s] already defined\n",
        adms_module_uid(mymymodule),adms_variableprototype_uid(myvariable)))
      return;
    }
    list=list->next;
  }
  adms_slist_push(&mymymodule->_variable,(p_adms)myvariable);
}
p_variableprototype adms_module_list_variable_lookup_by_id (p_module mymymodule,p_module mymodule,p_lexval mylexval,p_adms myblock)
{
  p_slist list=mymymodule->_variable;
  while(list) {
    p_variableprototype refvariableprototype=(p_variableprototype)list->data;
    if((refvariableprototype->_module==mymodule)&&
       !strcmp(refvariableprototype->_lexval->_string,mylexval->_string)&&
       (refvariableprototype->_block==myblock))
      return refvariableprototype;
    list=list->next;
  }
  return NULL;
}

void adms_module_valueto_analog (p_module mymodule,p_analog _analog)
{
  mymodule->_analog=_analog;
  mymodule->_analog_isdefault=0;
}

void adms_module_valueto_block (p_module mymodule,p_slist _block)
{
  mymodule->_block=_block;
  mymodule->_block_isdefault=0;
}

void adms_module_valueto_blockvariable (p_module mymodule,p_slist _blockvariable)
{
  mymodule->_blockvariable=_blockvariable;
  mymodule->_blockvariable_isdefault=0;
}
void adms_module_list_blockvariable_prepend_once_or_abort (p_module mymymodule,p_blockvariable myblockvariable)
{
  p_slist list=mymymodule->_blockvariable;
  while(list)
  {
    if(!adms_blockvariable_cmp((p_blockvariable)list->data,myblockvariable))
    {
      adms_message_fatal(("module=[%s] blockvariable=[%s] already defined\n",
        adms_module_uid(mymymodule),adms_blockvariable_uid(myblockvariable)))
      return;
    }
    list=list->next;
  }
  adms_slist_push(&mymymodule->_blockvariable,(p_adms)myblockvariable);
}
p_blockvariable adms_module_list_blockvariable_lookup_by_id (p_module mymymodule,p_block myblock)
{
  p_slist list=mymymodule->_blockvariable;
  while(list) {
    p_blockvariable refblockvariable=(p_blockvariable)list->data;
    if((refblockvariable->_block==myblock))
      return refblockvariable;
    list=list->next;
  }
  return NULL;
}

void adms_module_valueto_assignment (p_module mymodule,p_slist _assignment)
{
  mymodule->_assignment=_assignment;
  mymodule->_assignment_isdefault=0;
}

void adms_module_valueto_callfunction (p_module mymodule,p_slist _callfunction)
{
  mymodule->_callfunction=_callfunction;
  mymodule->_callfunction_isdefault=0;
}

void adms_module_valueto_contribution (p_module mymodule,p_slist _contribution)
{
  mymodule->_contribution=_contribution;
  mymodule->_contribution_isdefault=0;
}

void adms_module_valueto_conditional (p_module mymodule,p_slist _conditional)
{
  mymodule->_conditional=_conditional;
  mymodule->_conditional_isdefault=0;
}

void adms_module_valueto_case (p_module mymodule,p_slist _case)
{
  mymodule->_case=_case;
  mymodule->_case_isdefault=0;
}

void adms_module_valueto_forloop (p_module mymodule,p_slist _forloop)
{
  mymodule->_forloop=_forloop;
  mymodule->_forloop_isdefault=0;
}

void adms_module_valueto_whileloop (p_module mymodule,p_slist _whileloop)
{
  mymodule->_whileloop=_whileloop;
  mymodule->_whileloop_isdefault=0;
}

void adms_module_valueto_instantiator (p_module mymodule,p_slist _instantiator)
{
  mymodule->_instantiator=_instantiator;
  mymodule->_instantiator_isdefault=0;
}

void adms_module_valueto_expression (p_module mymodule,p_slist _expression)
{
  mymodule->_expression=_expression;
  mymodule->_expression_isdefault=0;
}

void adms_module_valueto_jacobian (p_module mymodule,p_slist _jacobian)
{
  mymodule->_jacobian=_jacobian;
  mymodule->_jacobian_isdefault=0;
}

void adms_module_valueto_probe (p_module mymodule,p_slist _probe)
{
  mymodule->_probe=_probe;
  mymodule->_probe_isdefault=0;
}
p_probe adms_module_list_probe_prepend_by_id_once_or_ignore (p_module mymymodule,p_module mymodule,p_branch mybranch,p_nature mynature)
{
  p_slist list=mymymodule->_probe;
  p_probe refprobe=adms_probe_new(mymodule,mybranch,mynature);
  while(list)
  {
    if(!adms_probe_cmp((p_probe)list->data,refprobe))
    {
      adms_probe_free (refprobe);
      return (p_probe)list->data;
    }
    list=list->next;
  }
  adms_slist_push(&mymymodule->_probe,(p_adms)refprobe);
  return refprobe;
}

void adms_module_valueto_source (p_module mymodule,p_slist _source)
{
  mymodule->_source=_source;
  mymodule->_source_isdefault=0;
}
p_source adms_module_list_source_prepend_by_id_once_or_ignore (p_module mymymodule,p_module mymodule,p_branch mybranch,p_nature mynature)
{
  p_slist list=mymymodule->_source;
  p_source refsource=adms_source_new(mymodule,mybranch,mynature);
  while(list)
  {
    if(!adms_source_cmp((p_source)list->data,refsource))
    {
      adms_source_free (refsource);
      return (p_source)list->data;
    }
    list=list->next;
  }
  adms_slist_push(&mymymodule->_source,(p_adms)refsource);
  return refsource;
}

void adms_module_valueto_range (p_module mymodule,p_slist _range)
{
  mymodule->_range=_range;
  mymodule->_range_isdefault=0;
}
p_range adms_module_list_range_prepend_by_id_once_or_abort (p_module mymymodule,p_module mymodule,p_expression myinfexpr,p_expression mysupexpr)
{
  p_slist list=mymymodule->_range;
  p_range refrange=adms_range_new(mymodule,myinfexpr,mysupexpr);
  while(list)
  {
    if(!adms_range_cmp((p_range)list->data,refrange))
    {
      adms_message_fatal(("module=[%s] range=[%s] already defined\n",
        adms_module_uid(mymymodule),adms_range_uid(refrange)))
      adms_range_free (refrange);
    }
    list=list->next;
  }
  adms_slist_push(&mymymodule->_range,(p_adms)refrange);
  return refrange;
}

void adms_module_valueto_attribute (p_module mymodule,p_slist _attribute)
{
  mymodule->_attribute=_attribute;
  mymodule->_attribute_isdefault=0;
}

void adms_module_valueto_evaluation (p_module mymodule,p_evaluation _evaluation)
{
  mymodule->_evaluation=_evaluation;
  mymodule->_evaluation_isdefault=0;
}
p_module adms_module_new (const char* myname)
{
  p_module mynewmodule=(p_module) calloc(1,(size_t)sizeof(t_module));
  mynewmodule->_adms._datatypename=admse_module;
  mynewmodule->_name=adms_kclone(myname);
  mynewmodule->_name_isdefault=1;
  mynewmodule->_node_isdefault=-1;
  mynewmodule->_nodealias_isdefault=-1;
  mynewmodule->_branch_isdefault=-1;
  mynewmodule->_branchalias_isdefault=-1;
  mynewmodule->_analogfunction_isdefault=-1;
  mynewmodule->_instance_isdefault=-1;
  mynewmodule->_variable_isdefault=-1;
  mynewmodule->_analog_isdefault=-1;
  mynewmodule->_block_isdefault=-1;
  mynewmodule->_blockvariable_isdefault=-1;
  mynewmodule->_assignment_isdefault=-1;
  mynewmodule->_callfunction_isdefault=-1;
  mynewmodule->_contribution_isdefault=-1;
  mynewmodule->_conditional_isdefault=-1;
  mynewmodule->_case_isdefault=-1;
  mynewmodule->_forloop_isdefault=-1;
  mynewmodule->_whileloop_isdefault=-1;
  mynewmodule->_instantiator_isdefault=-1;
  mynewmodule->_expression_isdefault=-1;
  mynewmodule->_jacobian_isdefault=-1;
  mynewmodule->_probe_isdefault=-1;
  mynewmodule->_source_isdefault=-1;
  mynewmodule->_range_isdefault=-1;
  mynewmodule->_attribute_isdefault=-1;
  mynewmodule->_evaluation_isdefault=-1;
  return mynewmodule;
}
char*adms_module_uid (p_module mynewmodule)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,mynewmodule->_name);
  return myuid;
}
static p_admst adms_module_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(module) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char* myname=aprintf(mytransform,(p_admst)myinputs[0]->data);
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_module_new(myname));
    free(myname);
    return myadmst;
  }
}

/* ------- analogfunction -------------- */
void adms_analogfunction_free (p_analogfunction myanalogfunction)
{
  if(!myanalogfunction)
    return;
  adms_slist_free(myanalogfunction->_variable);
  adms_slist_free(myanalogfunction->_attribute);
  free(myanalogfunction);
}

void adms_analogfunction_valueto_module (p_analogfunction myanalogfunction,p_module _module)
{
  myanalogfunction->_module=_module;
  myanalogfunction->_module_isdefault=0;
}

void adms_analogfunction_valueto_lexval (p_analogfunction myanalogfunction,p_lexval _lexval)
{
  myanalogfunction->_lexval=_lexval;
  myanalogfunction->_lexval_isdefault=0;
}

void adms_analogfunction_valueto_type (p_analogfunction myanalogfunction,admse _type)
{
  myanalogfunction->_type=_type;
  myanalogfunction->_type_isdefault=0;
}

void adms_analogfunction_valueto_tree (p_analogfunction myanalogfunction,p_adms _tree)
{
  myanalogfunction->_tree=_tree;
  myanalogfunction->_tree_isdefault=0;
}

void adms_analogfunction_valueto_variable (p_analogfunction myanalogfunction,p_slist _variable)
{
  myanalogfunction->_variable=_variable;
  myanalogfunction->_variable_isdefault=0;
}
void adms_analogfunction_list_variable_prepend_once_or_abort (p_analogfunction mymyanalogfunction,p_variableprototype myvariable)
{
  p_slist list=mymyanalogfunction->_variable;
  while(list)
  {
    if(!adms_variableprototype_cmp((p_variableprototype)list->data,myvariable))
    {
      adms_message_fatal(("analogfunction=[%s] variable=[%s] already defined\n",
        adms_analogfunction_uid(mymyanalogfunction),adms_variableprototype_uid(myvariable)))
      return;
    }
    list=list->next;
  }
  adms_slist_push(&mymyanalogfunction->_variable,(p_adms)myvariable);
}
p_variableprototype adms_analogfunction_list_variable_lookup_by_id (p_analogfunction mymyanalogfunction,p_module mymodule,p_lexval mylexval,p_adms myblock)
{
  p_slist list=mymyanalogfunction->_variable;
  while(list) {
    p_variableprototype refvariableprototype=(p_variableprototype)list->data;
    if((refvariableprototype->_module==mymodule)&&
       !strcmp(refvariableprototype->_lexval->_string,mylexval->_string)&&
       (refvariableprototype->_block==myblock))
      return refvariableprototype;
    list=list->next;
  }
  return NULL;
}

void adms_analogfunction_valueto_attribute (p_analogfunction myanalogfunction,p_slist _attribute)
{
  myanalogfunction->_attribute=_attribute;
  myanalogfunction->_attribute_isdefault=0;
}
p_analogfunction adms_analogfunction_new (p_module mymodule,p_lexval mylexval)
{
  p_analogfunction mynewanalogfunction=(p_analogfunction) calloc(1,(size_t)sizeof(t_analogfunction));
  mynewanalogfunction->_adms._datatypename=admse_analogfunction;
  mynewanalogfunction->_module=mymodule;
  mynewanalogfunction->_module_isdefault=1;
  mynewanalogfunction->_lexval=mylexval;
  mynewanalogfunction->_lexval_isdefault=1;
  mynewanalogfunction->_type=admse_real;
  mynewanalogfunction->_type_isdefault=1;
  mynewanalogfunction->_tree_isdefault=-1;
  mynewanalogfunction->_variable_isdefault=-1;
  mynewanalogfunction->_attribute_isdefault=-1;
  return mynewanalogfunction;
}
char*adms_analogfunction_uid (p_analogfunction mynewanalogfunction)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_module_uid(mynewanalogfunction->_module));
  adms_k2strconcat(&myuid,":");
  adms_strconcat(&myuid,adms_lexval_uid(mynewanalogfunction->_lexval));
  return myuid;
}
static p_admst adms_analogfunction_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=2)
  {
    adms_message_fatal_continue(("admst:new(analogfunction) bad number of inputs (found %i, expected 2)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_module mymodule=(p_module)((p_admst)myinputs[0]->data)->_item.p;
    char* mylexvals=aprintf(mytransform,(p_admst)myinputs[1]->data);
    p_lexval mylexval=adms_lexval_new(mylexvals,adms_transform_uid(mytransform),0,0);
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_analogfunction_new(mymodule,mylexval));
    free(mylexvals);
    return myadmst;
  }
}

/* ------- nodealias -------------- */
void adms_nodealias_free (p_nodealias mynodealias)
{
  if(!mynodealias)
    return;
  free(mynodealias->_name);
  free(mynodealias);
}

void adms_nodealias_valueto_module (p_nodealias mynodealias,p_module _module)
{
  mynodealias->_module=_module;
  mynodealias->_module_isdefault=0;
}

void adms_nodealias_valueto_name (p_nodealias mynodealias,const char* _name)
{
  mynodealias->_name=adms_kclone(_name);
  mynodealias->_name_isdefault=0;
}

void adms_nodealias_valueto_node (p_nodealias mynodealias,p_node _node)
{
  mynodealias->_node=_node;
  mynodealias->_node_isdefault=0;
}
p_nodealias adms_nodealias_new (p_module mymodule,const char* myname)
{
  p_nodealias mynewnodealias=(p_nodealias) calloc(1,(size_t)sizeof(t_nodealias));
  mynewnodealias->_adms._datatypename=admse_nodealias;
  mynewnodealias->_module=mymodule;
  mynewnodealias->_module_isdefault=1;
  mynewnodealias->_name=adms_kclone(myname);
  mynewnodealias->_name_isdefault=1;
  mynewnodealias->_node_isdefault=-1;
  return mynewnodealias;
}
char*adms_nodealias_uid (p_nodealias mynewnodealias)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,mynewnodealias->_name);
  return myuid;
}
static p_admst adms_nodealias_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=2)
  {
    adms_message_fatal_continue(("admst:new(nodealias) bad number of inputs (found %i, expected 2)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_module mymodule=(p_module)((p_admst)myinputs[0]->data)->_item.p;
    char* myname=aprintf(mytransform,(p_admst)myinputs[1]->data);
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_nodealias_new(mymodule,myname));
    free(myname);
    return myadmst;
  }
}

/* ------- node -------------- */
void adms_node_free (p_node mynode)
{
  if(!mynode)
    return;
  free(mynode->_name);
  adms_slist_free(mynode->_attribute);
  free(mynode);
}

void adms_node_valueto_module (p_node mynode,p_module _module)
{
  mynode->_module=_module;
  mynode->_module_isdefault=0;
}

void adms_node_valueto_name (p_node mynode,const char* _name)
{
  mynode->_name=adms_kclone(_name);
  mynode->_name_isdefault=0;
}

void adms_node_valueto_direction (p_node mynode,admse _direction)
{
  mynode->_direction=_direction;
  mynode->_direction_isdefault=0;
}

void adms_node_valueto_discipline (p_node mynode,p_discipline _discipline)
{
  mynode->_discipline=_discipline;
  mynode->_discipline_isdefault=0;
}

void adms_node_valueto_grounded (p_node mynode,admse _grounded)
{
  mynode->_grounded=_grounded;
  mynode->_grounded_isdefault=0;
}

void adms_node_valueto_location (p_node mynode,admse _location)
{
  mynode->_location=_location;
  mynode->_location_isdefault=0;
}

void adms_node_valueto_attribute (p_node mynode,p_slist _attribute)
{
  mynode->_attribute=_attribute;
  mynode->_attribute_isdefault=0;
}
p_node adms_node_new (p_module mymodule,const char* myname)
{
  p_node mynewnode=(p_node) calloc(1,(size_t)sizeof(t_node));
  mynewnode->_adms._datatypename=admse_node;
  mynewnode->_module=mymodule;
  mynewnode->_module_isdefault=1;
  mynewnode->_name=adms_kclone(myname);
  mynewnode->_name_isdefault=1;
  mynewnode->_direction=admse_inout;
  mynewnode->_direction_isdefault=1;
  mynewnode->_discipline_isdefault=-1;
  mynewnode->_grounded=admse_no;
  mynewnode->_grounded_isdefault=1;
  mynewnode->_location=admse_internal;
  mynewnode->_location_isdefault=1;
  mynewnode->_attribute_isdefault=-1;
  return mynewnode;
}
char*adms_node_uid (p_node mynewnode)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,mynewnode->_name);
  return myuid;
}
static p_admst adms_node_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=2)
  {
    adms_message_fatal_continue(("admst:new(node) bad number of inputs (found %i, expected 2)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_module mymodule=(p_module)((p_admst)myinputs[0]->data)->_item.p;
    char* myname=aprintf(mytransform,(p_admst)myinputs[1]->data);
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_node_new(mymodule,myname));
    free(myname);
    return myadmst;
  }
}

/* ------- branchalias -------------- */
void adms_branchalias_free (p_branchalias mybranchalias)
{
  if(!mybranchalias)
    return;
  free(mybranchalias->_name);
  free(mybranchalias);
}

void adms_branchalias_valueto_module (p_branchalias mybranchalias,p_module _module)
{
  mybranchalias->_module=_module;
  mybranchalias->_module_isdefault=0;
}

void adms_branchalias_valueto_name (p_branchalias mybranchalias,const char* _name)
{
  mybranchalias->_name=adms_kclone(_name);
  mybranchalias->_name_isdefault=0;
}

void adms_branchalias_valueto_branch (p_branchalias mybranchalias,p_branch _branch)
{
  mybranchalias->_branch=_branch;
  mybranchalias->_branch_isdefault=0;
}
p_branchalias adms_branchalias_new (p_module mymodule,const char* myname)
{
  p_branchalias mynewbranchalias=(p_branchalias) calloc(1,(size_t)sizeof(t_branchalias));
  mynewbranchalias->_adms._datatypename=admse_branchalias;
  mynewbranchalias->_module=mymodule;
  mynewbranchalias->_module_isdefault=1;
  mynewbranchalias->_name=adms_kclone(myname);
  mynewbranchalias->_name_isdefault=1;
  mynewbranchalias->_branch_isdefault=-1;
  return mynewbranchalias;
}
char*adms_branchalias_uid (p_branchalias mynewbranchalias)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,mynewbranchalias->_name);
  return myuid;
}
static p_admst adms_branchalias_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=2)
  {
    adms_message_fatal_continue(("admst:new(branchalias) bad number of inputs (found %i, expected 2)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_module mymodule=(p_module)((p_admst)myinputs[0]->data)->_item.p;
    char* myname=aprintf(mytransform,(p_admst)myinputs[1]->data);
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_branchalias_new(mymodule,myname));
    free(myname);
    return myadmst;
  }
}

/* ------- branch -------------- */
void adms_branch_free (p_branch mybranch)
{
  if(!mybranch)
    return;
  adms_slist_free(mybranch->_attribute);
  free(mybranch);
}

void adms_branch_valueto_module (p_branch mybranch,p_module _module)
{
  mybranch->_module=_module;
  mybranch->_module_isdefault=0;
}

void adms_branch_valueto_pnode (p_branch mybranch,p_node _pnode)
{
  mybranch->_pnode=_pnode;
  mybranch->_pnode_isdefault=0;
}

void adms_branch_valueto_nnode (p_branch mybranch,p_node _nnode)
{
  mybranch->_nnode=_nnode;
  mybranch->_nnode_isdefault=0;
}

void adms_branch_valueto_discipline (p_branch mybranch,p_discipline _discipline)
{
  mybranch->_discipline=_discipline;
  mybranch->_discipline_isdefault=0;
}

void adms_branch_valueto_grounded (p_branch mybranch,admse _grounded)
{
  mybranch->_grounded=_grounded;
  mybranch->_grounded_isdefault=0;
}

void adms_branch_valueto_attribute (p_branch mybranch,p_slist _attribute)
{
  mybranch->_attribute=_attribute;
  mybranch->_attribute_isdefault=0;
}
p_branch adms_branch_new (p_module mymodule,p_node mypnode,p_node mynnode)
{
  p_branch mynewbranch=(p_branch) calloc(1,(size_t)sizeof(t_branch));
  mynewbranch->_adms._datatypename=admse_branch;
  mynewbranch->_module=mymodule;
  mynewbranch->_module_isdefault=1;
  mynewbranch->_pnode=mypnode;
  mynewbranch->_pnode_isdefault=1;
  mynewbranch->_nnode=mynnode;
  mynewbranch->_nnode_isdefault=1;
  mynewbranch->_discipline_isdefault=-1;
  mynewbranch->_grounded=admse_no;
  mynewbranch->_grounded_isdefault=1;
  mynewbranch->_attribute_isdefault=-1;
  return mynewbranch;
}
char*adms_branch_uid (p_branch mynewbranch)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_node_uid(mynewbranch->_pnode));
  adms_k2strconcat(&myuid,",");
  adms_strconcat(&myuid,adms_node_uid(mynewbranch->_nnode));
  return myuid;
}
static p_admst adms_branch_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=3)
  {
    adms_message_fatal_continue(("admst:new(branch) bad number of inputs (found %i, expected 3)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_module mymodule=(p_module)((p_admst)myinputs[0]->data)->_item.p;
    p_node mypnode=(p_node)((p_admst)myinputs[1]->data)->_item.p;
    p_node mynnode=(p_node)((p_admst)myinputs[2]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_branch_new(mymodule,mypnode,mynnode));
    return myadmst;
  }
}

/* ------- variableprototype -------------- */
void adms_variableprototype_free (p_variableprototype myvariableprototype)
{
  if(!myvariableprototype)
    return;
  adms_slist_free(myvariableprototype->_arraydefault);
  adms_slist_free(myvariableprototype->_attribute);
  adms_slist_free(myvariableprototype->_alias);
  adms_slist_free(myvariableprototype->_range);
  adms_slist_free(myvariableprototype->_probe);
  adms_slist_free(myvariableprototype->_ddxprobe);
  adms_slist_free(myvariableprototype->_variable);
  adms_slist_free(myvariableprototype->_assignment);
  adms_slist_free(myvariableprototype->_instance);
  free(myvariableprototype);
}

void adms_variableprototype_valueto_module (p_variableprototype myvariableprototype,p_module _module)
{
  myvariableprototype->_module=_module;
  myvariableprototype->_module_isdefault=0;
}

void adms_variableprototype_valueto_lexval (p_variableprototype myvariableprototype,p_lexval _lexval)
{
  myvariableprototype->_lexval=_lexval;
  myvariableprototype->_lexval_isdefault=0;
}

void adms_variableprototype_valueto_block (p_variableprototype myvariableprototype,p_adms _block)
{
  myvariableprototype->_block=_block;
  myvariableprototype->_block_isdefault=0;
}

void adms_variableprototype_valueto_dependency (p_variableprototype myvariableprototype,admse _dependency)
{
  myvariableprototype->_dependency=_dependency;
  myvariableprototype->_dependency_isdefault=0;
}

void adms_variableprototype_valueto_static (p_variableprototype myvariableprototype,admse _static)
{
  myvariableprototype->_static=_static;
  myvariableprototype->_static_isdefault=0;
}

void adms_variableprototype_valueto_dynamic (p_variableprototype myvariableprototype,admse _dynamic)
{
  myvariableprototype->_dynamic=_dynamic;
  myvariableprototype->_dynamic_isdefault=0;
}

void adms_variableprototype_valueto_sizetype (p_variableprototype myvariableprototype,admse _sizetype)
{
  myvariableprototype->_sizetype=_sizetype;
  myvariableprototype->_sizetype_isdefault=0;
}

void adms_variableprototype_valueto_parametertype (p_variableprototype myvariableprototype,admse _parametertype)
{
  myvariableprototype->_parametertype=_parametertype;
  myvariableprototype->_parametertype_isdefault=0;
}

void adms_variableprototype_valueto_type (p_variableprototype myvariableprototype,admse _type)
{
  myvariableprototype->_type=_type;
  myvariableprototype->_type_isdefault=0;
}

void adms_variableprototype_valueto_maxsize (p_variableprototype myvariableprototype,p_expression _maxsize)
{
  myvariableprototype->_maxsize=_maxsize;
  myvariableprototype->_maxsize_isdefault=0;
}

void adms_variableprototype_valueto_minsize (p_variableprototype myvariableprototype,p_expression _minsize)
{
  myvariableprototype->_minsize=_minsize;
  myvariableprototype->_minsize_isdefault=0;
}

void adms_variableprototype_valueto_input (p_variableprototype myvariableprototype,admse _input)
{
  myvariableprototype->_input=_input;
  myvariableprototype->_input_isdefault=0;
}

void adms_variableprototype_valueto_output (p_variableprototype myvariableprototype,admse _output)
{
  myvariableprototype->_output=_output;
  myvariableprototype->_output_isdefault=0;
}

void adms_variableprototype_valueto_scope (p_variableprototype myvariableprototype,admse _scope)
{
  myvariableprototype->_scope=_scope;
  myvariableprototype->_scope_isdefault=0;
}

void adms_variableprototype_valueto_default (p_variableprototype myvariableprototype,p_expression _default)
{
  myvariableprototype->_default=_default;
  myvariableprototype->_default_isdefault=0;
}

void adms_variableprototype_valueto_derivate (p_variableprototype myvariableprototype,admse _derivate)
{
  myvariableprototype->_derivate=_derivate;
  myvariableprototype->_derivate_isdefault=0;
}

void adms_variableprototype_valueto_isstate (p_variableprototype myvariableprototype,admse _isstate)
{
  myvariableprototype->_isstate=_isstate;
  myvariableprototype->_isstate_isdefault=0;
}

void adms_variableprototype_valueto_usedinmodel (p_variableprototype myvariableprototype,admse _usedinmodel)
{
  myvariableprototype->_usedinmodel=_usedinmodel;
  myvariableprototype->_usedinmodel_isdefault=0;
}

void adms_variableprototype_valueto_usedininstance (p_variableprototype myvariableprototype,admse _usedininstance)
{
  myvariableprototype->_usedininstance=_usedininstance;
  myvariableprototype->_usedininstance_isdefault=0;
}

void adms_variableprototype_valueto_usedininitial_step (p_variableprototype myvariableprototype,admse _usedininitial_step)
{
  myvariableprototype->_usedininitial_step=_usedininitial_step;
  myvariableprototype->_usedininitial_step_isdefault=0;
}

void adms_variableprototype_valueto_usedinevaluate (p_variableprototype myvariableprototype,admse _usedinevaluate)
{
  myvariableprototype->_usedinevaluate=_usedinevaluate;
  myvariableprototype->_usedinevaluate_isdefault=0;
}

void adms_variableprototype_valueto_usedinnoise (p_variableprototype myvariableprototype,admse _usedinnoise)
{
  myvariableprototype->_usedinnoise=_usedinnoise;
  myvariableprototype->_usedinnoise_isdefault=0;
}

void adms_variableprototype_valueto_usedinfinal (p_variableprototype myvariableprototype,admse _usedinfinal)
{
  myvariableprototype->_usedinfinal=_usedinfinal;
  myvariableprototype->_usedinfinal_isdefault=0;
}

void adms_variableprototype_valueto_setinmodel (p_variableprototype myvariableprototype,admse _setinmodel)
{
  myvariableprototype->_setinmodel=_setinmodel;
  myvariableprototype->_setinmodel_isdefault=0;
}

void adms_variableprototype_valueto_setininstance (p_variableprototype myvariableprototype,admse _setininstance)
{
  myvariableprototype->_setininstance=_setininstance;
  myvariableprototype->_setininstance_isdefault=0;
}

void adms_variableprototype_valueto_setininitial_step (p_variableprototype myvariableprototype,admse _setininitial_step)
{
  myvariableprototype->_setininitial_step=_setininitial_step;
  myvariableprototype->_setininitial_step_isdefault=0;
}

void adms_variableprototype_valueto_setinevaluate (p_variableprototype myvariableprototype,admse _setinevaluate)
{
  myvariableprototype->_setinevaluate=_setinevaluate;
  myvariableprototype->_setinevaluate_isdefault=0;
}

void adms_variableprototype_valueto_setinnoise (p_variableprototype myvariableprototype,admse _setinnoise)
{
  myvariableprototype->_setinnoise=_setinnoise;
  myvariableprototype->_setinnoise_isdefault=0;
}

void adms_variableprototype_valueto_setinfinal (p_variableprototype myvariableprototype,admse _setinfinal)
{
  myvariableprototype->_setinfinal=_setinfinal;
  myvariableprototype->_setinfinal_isdefault=0;
}

void adms_variableprototype_valueto_setingetprev (p_variableprototype myvariableprototype,admse _setingetprev)
{
  myvariableprototype->_setingetprev=_setingetprev;
  myvariableprototype->_setingetprev_isdefault=0;
}

void adms_variableprototype_valueto_TemperatureDependent (p_variableprototype myvariableprototype,admse _TemperatureDependent)
{
  myvariableprototype->_TemperatureDependent=_TemperatureDependent;
  myvariableprototype->_TemperatureDependent_isdefault=0;
}

void adms_variableprototype_valueto_OPdependent (p_variableprototype myvariableprototype,admse _OPdependent)
{
  myvariableprototype->_OPdependent=_OPdependent;
  myvariableprototype->_OPdependent_isdefault=0;
}

void adms_variableprototype_valueto_insource (p_variableprototype myvariableprototype,admse _insource)
{
  myvariableprototype->_insource=_insource;
  myvariableprototype->_insource_isdefault=0;
}

void adms_variableprototype_valueto_vcount (p_variableprototype myvariableprototype,int _vcount)
{
  myvariableprototype->_vcount=_vcount;
  myvariableprototype->_vcount_isdefault=0;
}

void adms_variableprototype_valueto_vlast (p_variableprototype myvariableprototype,p_assignment _vlast)
{
  myvariableprototype->_vlast=_vlast;
  myvariableprototype->_vlast_isdefault=0;
}

void adms_variableprototype_valueto_arraydefault (p_variableprototype myvariableprototype,p_slist _arraydefault)
{
  myvariableprototype->_arraydefault=_arraydefault;
  myvariableprototype->_arraydefault_isdefault=0;
}

void adms_variableprototype_valueto_attribute (p_variableprototype myvariableprototype,p_slist _attribute)
{
  myvariableprototype->_attribute=_attribute;
  myvariableprototype->_attribute_isdefault=0;
}

void adms_variableprototype_valueto_alias (p_variableprototype myvariableprototype,p_slist _alias)
{
  myvariableprototype->_alias=_alias;
  myvariableprototype->_alias_isdefault=0;
}
void adms_variableprototype_list_alias_prepend_once_or_abort (p_variableprototype mymyvariableprototype,const char* myalias)
{
  p_slist list=mymyvariableprototype->_alias;
  while(list)
  {
    if(!adms_basicstring_cmp((p_basicstring)list->data,myalias))
    {
      adms_message_fatal(("variableprototype=[%s] alias=[%s] already defined\n",
        adms_variableprototype_uid(mymyvariableprototype),adms_basicstring_uid(myalias)))
      return;
    }
    list=list->next;
  }
  adms_slist_push(&mymyvariableprototype->_alias,(p_adms)myalias);
}

void adms_variableprototype_valueto_range (p_variableprototype myvariableprototype,p_slist _range)
{
  myvariableprototype->_range=_range;
  myvariableprototype->_range_isdefault=0;
}

void adms_variableprototype_valueto_probe (p_variableprototype myvariableprototype,p_slist _probe)
{
  myvariableprototype->_probe=_probe;
  myvariableprototype->_probe_isdefault=0;
}

void adms_variableprototype_valueto_ddxprobe (p_variableprototype myvariableprototype,p_slist _ddxprobe)
{
  myvariableprototype->_ddxprobe=_ddxprobe;
  myvariableprototype->_ddxprobe_isdefault=0;
}

void adms_variableprototype_valueto_variable (p_variableprototype myvariableprototype,p_slist _variable)
{
  myvariableprototype->_variable=_variable;
  myvariableprototype->_variable_isdefault=0;
}

void adms_variableprototype_valueto_assignment (p_variableprototype myvariableprototype,p_slist _assignment)
{
  myvariableprototype->_assignment=_assignment;
  myvariableprototype->_assignment_isdefault=0;
}

void adms_variableprototype_valueto_instance (p_variableprototype myvariableprototype,p_slist _instance)
{
  myvariableprototype->_instance=_instance;
  myvariableprototype->_instance_isdefault=0;
}
p_variableprototype adms_variableprototype_new (p_module mymodule,p_lexval mylexval,p_adms myblock)
{
  p_variableprototype mynewvariableprototype=(p_variableprototype) calloc(1,(size_t)sizeof(t_variableprototype));
  mynewvariableprototype->_adms._datatypename=admse_variableprototype;
  mynewvariableprototype->_module=mymodule;
  mynewvariableprototype->_module_isdefault=1;
  mynewvariableprototype->_lexval=mylexval;
  mynewvariableprototype->_lexval_isdefault=1;
  mynewvariableprototype->_block=myblock;
  mynewvariableprototype->_block_isdefault=1;
  mynewvariableprototype->_dependency=admse_constant;
  mynewvariableprototype->_dependency_isdefault=1;
  mynewvariableprototype->_static=admse_no;
  mynewvariableprototype->_static_isdefault=1;
  mynewvariableprototype->_dynamic=admse_no;
  mynewvariableprototype->_dynamic_isdefault=1;
  mynewvariableprototype->_sizetype=admse_scalar;
  mynewvariableprototype->_sizetype_isdefault=1;
  mynewvariableprototype->_parametertype=admse_model;
  mynewvariableprototype->_parametertype_isdefault=1;
  mynewvariableprototype->_type=admse_real;
  mynewvariableprototype->_type_isdefault=1;
  mynewvariableprototype->_maxsize_isdefault=-1;
  mynewvariableprototype->_minsize_isdefault=-1;
  mynewvariableprototype->_input=admse_no;
  mynewvariableprototype->_input_isdefault=1;
  mynewvariableprototype->_output=admse_no;
  mynewvariableprototype->_output_isdefault=1;
  mynewvariableprototype->_scope=admse_local;
  mynewvariableprototype->_scope_isdefault=1;
  mynewvariableprototype->_default_isdefault=-1;
  mynewvariableprototype->_derivate=admse_no;
  mynewvariableprototype->_derivate_isdefault=1;
  mynewvariableprototype->_isstate=admse_no;
  mynewvariableprototype->_isstate_isdefault=1;
  mynewvariableprototype->_usedinmodel=admse_no;
  mynewvariableprototype->_usedinmodel_isdefault=1;
  mynewvariableprototype->_usedininstance=admse_no;
  mynewvariableprototype->_usedininstance_isdefault=1;
  mynewvariableprototype->_usedininitial_step=admse_no;
  mynewvariableprototype->_usedininitial_step_isdefault=1;
  mynewvariableprototype->_usedinevaluate=admse_no;
  mynewvariableprototype->_usedinevaluate_isdefault=1;
  mynewvariableprototype->_usedinnoise=admse_no;
  mynewvariableprototype->_usedinnoise_isdefault=1;
  mynewvariableprototype->_usedinfinal=admse_no;
  mynewvariableprototype->_usedinfinal_isdefault=1;
  mynewvariableprototype->_setinmodel=admse_no;
  mynewvariableprototype->_setinmodel_isdefault=1;
  mynewvariableprototype->_setininstance=admse_no;
  mynewvariableprototype->_setininstance_isdefault=1;
  mynewvariableprototype->_setininitial_step=admse_no;
  mynewvariableprototype->_setininitial_step_isdefault=1;
  mynewvariableprototype->_setinevaluate=admse_no;
  mynewvariableprototype->_setinevaluate_isdefault=1;
  mynewvariableprototype->_setinnoise=admse_no;
  mynewvariableprototype->_setinnoise_isdefault=1;
  mynewvariableprototype->_setinfinal=admse_no;
  mynewvariableprototype->_setinfinal_isdefault=1;
  mynewvariableprototype->_setingetprev=admse_no;
  mynewvariableprototype->_setingetprev_isdefault=1;
  mynewvariableprototype->_TemperatureDependent=admse_no;
  mynewvariableprototype->_TemperatureDependent_isdefault=1;
  mynewvariableprototype->_OPdependent=admse_no;
  mynewvariableprototype->_OPdependent_isdefault=1;
  mynewvariableprototype->_insource=admse_no;
  mynewvariableprototype->_insource_isdefault=1;
  mynewvariableprototype->_vcount=0;
  mynewvariableprototype->_vcount_isdefault=1;
  mynewvariableprototype->_vlast_isdefault=-1;
  mynewvariableprototype->_arraydefault_isdefault=-1;
  mynewvariableprototype->_attribute_isdefault=-1;
  mynewvariableprototype->_alias_isdefault=-1;
  mynewvariableprototype->_range_isdefault=-1;
  mynewvariableprototype->_probe_isdefault=-1;
  mynewvariableprototype->_ddxprobe_isdefault=-1;
  mynewvariableprototype->_variable_isdefault=-1;
  mynewvariableprototype->_assignment_isdefault=-1;
  mynewvariableprototype->_instance_isdefault=-1;
  return mynewvariableprototype;
}
char*adms_variableprototype_uid (p_variableprototype mynewvariableprototype)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_lexval_uid(mynewvariableprototype->_lexval));
  return myuid;
}
static p_admst adms_variableprototype_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=3)
  {
    adms_message_fatal_continue(("admst:new(variableprototype) bad number of inputs (found %i, expected 3)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_module mymodule=(p_module)((p_admst)myinputs[0]->data)->_item.p;
    char* mylexvals=aprintf(mytransform,(p_admst)myinputs[1]->data);
    p_lexval mylexval=adms_lexval_new(mylexvals,adms_transform_uid(mytransform),0,0);
    p_adms myblock=(p_adms)((p_admst)myinputs[2]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_variableprototype_new(mymodule,mylexval,myblock));
    free(mylexvals);
    return myadmst;
  }
}

/* ------- source -------------- */
void adms_source_free (p_source mysource)
{
  if(!mysource)
    return;
  adms_slist_free(mysource->_probe);
  adms_slist_free(mysource->_attribute);
  free(mysource);
}

void adms_source_valueto_module (p_source mysource,p_module _module)
{
  mysource->_module=_module;
  mysource->_module_isdefault=0;
}

void adms_source_valueto_branch (p_source mysource,p_branch _branch)
{
  mysource->_branch=_branch;
  mysource->_branch_isdefault=0;
}

void adms_source_valueto_nature (p_source mysource,p_nature _nature)
{
  mysource->_nature=_nature;
  mysource->_nature_isdefault=0;
}

void adms_source_valueto_discipline (p_source mysource,p_discipline _discipline)
{
  mysource->_discipline=_discipline;
  mysource->_discipline_isdefault=0;
}

void adms_source_valueto_static (p_source mysource,admse _static)
{
  mysource->_static=_static;
  mysource->_static_isdefault=0;
}

void adms_source_valueto_dynamic (p_source mysource,admse _dynamic)
{
  mysource->_dynamic=_dynamic;
  mysource->_dynamic_isdefault=0;
}

void adms_source_valueto_whitenoise (p_source mysource,admse _whitenoise)
{
  mysource->_whitenoise=_whitenoise;
  mysource->_whitenoise_isdefault=0;
}

void adms_source_valueto_flickernoise (p_source mysource,admse _flickernoise)
{
  mysource->_flickernoise=_flickernoise;
  mysource->_flickernoise_isdefault=0;
}

void adms_source_valueto_grounded (p_source mysource,admse _grounded)
{
  mysource->_grounded=_grounded;
  mysource->_grounded_isdefault=0;
}

void adms_source_valueto_switch (p_source mysource,admse _switch)
{
  mysource->_switch=_switch;
  mysource->_switch_isdefault=0;
}

void adms_source_valueto_type (p_source mysource,admse _type)
{
  mysource->_type=_type;
  mysource->_type_isdefault=0;
}

void adms_source_valueto_probe (p_source mysource,p_slist _probe)
{
  mysource->_probe=_probe;
  mysource->_probe_isdefault=0;
}

void adms_source_valueto_attribute (p_source mysource,p_slist _attribute)
{
  mysource->_attribute=_attribute;
  mysource->_attribute_isdefault=0;
}
p_source adms_source_new (p_module mymodule,p_branch mybranch,p_nature mynature)
{
  p_source mynewsource=(p_source) calloc(1,(size_t)sizeof(t_source));
  mynewsource->_adms._datatypename=admse_source;
  mynewsource->_module=mymodule;
  mynewsource->_module_isdefault=1;
  mynewsource->_branch=mybranch;
  mynewsource->_branch_isdefault=1;
  mynewsource->_nature=mynature;
  mynewsource->_nature_isdefault=1;
  mynewsource->_discipline_isdefault=-1;
  mynewsource->_static=admse_no;
  mynewsource->_static_isdefault=1;
  mynewsource->_dynamic=admse_no;
  mynewsource->_dynamic_isdefault=1;
  mynewsource->_whitenoise=admse_no;
  mynewsource->_whitenoise_isdefault=1;
  mynewsource->_flickernoise=admse_no;
  mynewsource->_flickernoise_isdefault=1;
  mynewsource->_grounded=admse_no;
  mynewsource->_grounded_isdefault=1;
  mynewsource->_switch=admse_no;
  mynewsource->_switch_isdefault=1;
  mynewsource->_type=admse_fs;
  mynewsource->_type_isdefault=1;
  mynewsource->_probe_isdefault=-1;
  mynewsource->_attribute_isdefault=-1;
  return mynewsource;
}
char*adms_source_uid (p_source mynewsource)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_nature_uid(mynewsource->_nature));
  adms_k2strconcat(&myuid,"(");
  adms_strconcat(&myuid,adms_branch_uid(mynewsource->_branch));
  adms_k2strconcat(&myuid,")");
  return myuid;
}
static p_admst adms_source_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=3)
  {
    adms_message_fatal_continue(("admst:new(source) bad number of inputs (found %i, expected 3)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_module mymodule=(p_module)((p_admst)myinputs[0]->data)->_item.p;
    p_branch mybranch=(p_branch)((p_admst)myinputs[1]->data)->_item.p;
    p_nature mynature=(p_nature)((p_admst)myinputs[2]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_source_new(mymodule,mybranch,mynature));
    return myadmst;
  }
}

/* ------- range -------------- */
void adms_range_free (p_range myrange)
{
  if(!myrange)
    return;
  free(myrange->_name);
  free(myrange);
}

void adms_range_valueto_module (p_range myrange,p_module _module)
{
  myrange->_module=_module;
  myrange->_module_isdefault=0;
}

void adms_range_valueto_infexpr (p_range myrange,p_expression _infexpr)
{
  myrange->_infexpr=_infexpr;
  myrange->_infexpr_isdefault=0;
}

void adms_range_valueto_supexpr (p_range myrange,p_expression _supexpr)
{
  myrange->_supexpr=_supexpr;
  myrange->_supexpr_isdefault=0;
}

void adms_range_valueto_name (p_range myrange,const char* _name)
{
  myrange->_name=adms_kclone(_name);
  myrange->_name_isdefault=0;
}

void adms_range_valueto_infboundtype (p_range myrange,admse _infboundtype)
{
  myrange->_infboundtype=_infboundtype;
  myrange->_infboundtype_isdefault=0;
}

void adms_range_valueto_supboundtype (p_range myrange,admse _supboundtype)
{
  myrange->_supboundtype=_supboundtype;
  myrange->_supboundtype_isdefault=0;
}

void adms_range_valueto_type (p_range myrange,admse _type)
{
  myrange->_type=_type;
  myrange->_type_isdefault=0;
}
p_range adms_range_new (p_module mymodule,p_expression myinfexpr,p_expression mysupexpr)
{
  p_range mynewrange=(p_range) calloc(1,(size_t)sizeof(t_range));
  mynewrange->_adms._datatypename=admse_range;
  mynewrange->_module=mymodule;
  mynewrange->_module_isdefault=1;
  mynewrange->_infexpr=myinfexpr;
  mynewrange->_infexpr_isdefault=1;
  mynewrange->_supexpr=mysupexpr;
  mynewrange->_supexpr_isdefault=1;
  mynewrange->_name_isdefault=-1;
  mynewrange->_infboundtype_isdefault=-1;
  mynewrange->_supboundtype_isdefault=-1;
  mynewrange->_type=admse_include;
  mynewrange->_type_isdefault=1;
  return mynewrange;
}
char*adms_range_uid (p_range mynewrange)
{
  char*myuid=NULL;
  if(mynewrange->_type==admse_include)
  {
    adms_k2strconcat(&myuid,"from");
  }
  if(mynewrange->_type==admse_include_value)
  {
    adms_k2strconcat(&myuid,"from");
  }
  if(mynewrange->_type==admse_exclude)
  {
    adms_k2strconcat(&myuid,"exclude");
  }
  if(mynewrange->_type==admse_exclude_value)
  {
    adms_k2strconcat(&myuid,"exclude");
  }
  adms_k2strconcat(&myuid," ");
  if(mynewrange->_infboundtype==admse_range_bound_include)
  {
    adms_k2strconcat(&myuid,"[");
  }
  if(mynewrange->_infboundtype==admse_range_bound_exclude)
  {
    adms_k2strconcat(&myuid,"(");
  }
  if(mynewrange->_infboundtype==admse_range_bound_value)
  {
    adms_k2strconcat(&myuid,"[");
  }
  adms_strconcat(&myuid,adms_expression_uid(mynewrange->_infexpr));
  adms_k2strconcat(&myuid,":");
  adms_strconcat(&myuid,adms_expression_uid(mynewrange->_supexpr));
  if(mynewrange->_supboundtype==admse_range_bound_include)
  {
    adms_k2strconcat(&myuid,"]");
  }
  if(mynewrange->_supboundtype==admse_range_bound_exclude)
  {
    adms_k2strconcat(&myuid,")");
  }
  if(mynewrange->_supboundtype==admse_range_bound_value)
  {
    adms_k2strconcat(&myuid,"]");
  }
  return myuid;
}
static p_admst adms_range_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=3)
  {
    adms_message_fatal_continue(("admst:new(range) bad number of inputs (found %i, expected 3)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_module mymodule=(p_module)((p_admst)myinputs[0]->data)->_item.p;
    p_expression myinfexpr=(p_expression)((p_admst)myinputs[1]->data)->_item.p;
    p_expression mysupexpr=(p_expression)((p_admst)myinputs[2]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_range_new(mymodule,myinfexpr,mysupexpr));
    return myadmst;
  }
}

/* ------- jacobian -------------- */
void adms_jacobian_free (p_jacobian myjacobian)
{
  if(!myjacobian)
    return;
  free(myjacobian);
}

void adms_jacobian_valueto_module (p_jacobian myjacobian,p_module _module)
{
  myjacobian->_module=_module;
  myjacobian->_module_isdefault=0;
}

void adms_jacobian_valueto_row (p_jacobian myjacobian,p_node _row)
{
  myjacobian->_row=_row;
  myjacobian->_row_isdefault=0;
}

void adms_jacobian_valueto_column (p_jacobian myjacobian,p_node _column)
{
  myjacobian->_column=_column;
  myjacobian->_column_isdefault=0;
}

void adms_jacobian_valueto_diagonal (p_jacobian myjacobian,admse _diagonal)
{
  myjacobian->_diagonal=_diagonal;
  myjacobian->_diagonal_isdefault=0;
}

void adms_jacobian_valueto_static (p_jacobian myjacobian,admse _static)
{
  myjacobian->_static=_static;
  myjacobian->_static_isdefault=0;
}

void adms_jacobian_valueto_dynamic (p_jacobian myjacobian,admse _dynamic)
{
  myjacobian->_dynamic=_dynamic;
  myjacobian->_dynamic_isdefault=0;
}
p_jacobian adms_jacobian_new (p_module mymodule,p_node myrow,p_node mycolumn)
{
  p_jacobian mynewjacobian=(p_jacobian) calloc(1,(size_t)sizeof(t_jacobian));
  mynewjacobian->_adms._datatypename=admse_jacobian;
  mynewjacobian->_module=mymodule;
  mynewjacobian->_module_isdefault=1;
  mynewjacobian->_row=myrow;
  mynewjacobian->_row_isdefault=1;
  mynewjacobian->_column=mycolumn;
  mynewjacobian->_column_isdefault=1;
  mynewjacobian->_diagonal=admse_no;
  mynewjacobian->_diagonal_isdefault=1;
  mynewjacobian->_static=admse_no;
  mynewjacobian->_static_isdefault=1;
  mynewjacobian->_dynamic=admse_no;
  mynewjacobian->_dynamic_isdefault=1;
  return mynewjacobian;
}
char*adms_jacobian_uid (p_jacobian mynewjacobian)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_node_uid(mynewjacobian->_row));
  adms_k2strconcat(&myuid,":");
  adms_strconcat(&myuid,adms_node_uid(mynewjacobian->_column));
  return myuid;
}
static p_admst adms_jacobian_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=3)
  {
    adms_message_fatal_continue(("admst:new(jacobian) bad number of inputs (found %i, expected 3)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_module mymodule=(p_module)((p_admst)myinputs[0]->data)->_item.p;
    p_node myrow=(p_node)((p_admst)myinputs[1]->data)->_item.p;
    p_node mycolumn=(p_node)((p_admst)myinputs[2]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_jacobian_new(mymodule,myrow,mycolumn));
    return myadmst;
  }
}

/* ------- analog -------------- */
void adms_analog_free (p_analog myanalog)
{
  if(!myanalog)
    return;
  free(myanalog);
}

void adms_analog_valueto_code (p_analog myanalog,p_adms _code)
{
  myanalog->_code=_code;
  myanalog->_code_isdefault=0;
}
p_analog adms_analog_new (p_adms mycode)
{
  p_analog mynewanalog=(p_analog) calloc(1,(size_t)sizeof(t_analog));
  mynewanalog->_adms._datatypename=admse_analog;
  mynewanalog->_code=mycode;
  mynewanalog->_code_isdefault=1;
  return mynewanalog;
}
char*adms_analog_uid (p_analog mynewanalog)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,"analog ");
  adms_strconcat(&myuid,adms_admsxml_uid(mynewanalog->_code));
  return myuid;
}
static p_admst adms_analog_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(analog) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_adms mycode=(p_adms)((p_admst)myinputs[0]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_analog_new(mycode));
    return myadmst;
  }
}

/* ------- math -------------- */
void adms_math_free (p_math mymath)
{
  if(!mymath)
    return;
  free(mymath->_name);
  adms_slist_free(mymath->_attribute);
  free(mymath);
}

void adms_math_valueto_name (p_math mymath,const char* _name)
{
  mymath->_name=adms_kclone(_name);
  mymath->_name_isdefault=0;
}

void adms_math_valueto_value (p_math mymath,double _value)
{
  mymath->_value=_value;
  mymath->_value_isdefault=0;
}

void adms_math_valueto_static (p_math mymath,admse _static)
{
  mymath->_static=_static;
  mymath->_static_isdefault=0;
}

void adms_math_valueto_dynamic (p_math mymath,admse _dynamic)
{
  mymath->_dynamic=_dynamic;
  mymath->_dynamic_isdefault=0;
}

void adms_math_valueto_dependency (p_math mymath,admse _dependency)
{
  mymath->_dependency=_dependency;
  mymath->_dependency_isdefault=0;
}

void adms_math_valueto_attribute (p_math mymath,p_slist _attribute)
{
  mymath->_attribute=_attribute;
  mymath->_attribute_isdefault=0;
}
p_math adms_math_new (const char* myname)
{
  p_math mynewmath=(p_math) calloc(1,(size_t)sizeof(t_math));
  mynewmath->_adms._datatypename=admse_math;
  mynewmath->_name=adms_kclone(myname);
  mynewmath->_name_isdefault=1;
  mynewmath->_value=adms_NAN;
  mynewmath->_value_isdefault=1;
  mynewmath->_static=admse_no;
  mynewmath->_static_isdefault=1;
  mynewmath->_dynamic=admse_no;
  mynewmath->_dynamic_isdefault=1;
  mynewmath->_dependency=admse_constant;
  mynewmath->_dependency_isdefault=1;
  mynewmath->_attribute_isdefault=-1;
  return mynewmath;
}
char*adms_math_uid (p_math mynewmath)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,mynewmath->_name);
  return myuid;
}
static p_admst adms_math_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(math) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char* myname=aprintf(mytransform,(p_admst)myinputs[0]->data);
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_math_new(myname));
    free(myname);
    return myadmst;
  }
}

/* ------- string -------------- */
void adms_string_free (p_string mystring)
{
  if(!mystring)
    return;
  free(mystring->_value);
  free(mystring);
}

void adms_string_valueto_value (p_string mystring,const char* _value)
{
  mystring->_value=adms_kclone(_value);
  mystring->_value_isdefault=0;
}
p_string adms_string_new (const char* myvalue)
{
  p_string mynewstring=(p_string) calloc(1,(size_t)sizeof(t_string));
  mynewstring->_math._adms._datatypename=admse_string;
  mynewstring->_math._value=adms_NAN;
  mynewstring->_math._static=admse_no;
  mynewstring->_math._dynamic=admse_no;
  mynewstring->_math._dependency=admse_constant;
  mynewstring->_value=adms_kclone(myvalue);
  mynewstring->_value_isdefault=1;
  return mynewstring;
}
char*adms_string_uid (p_string mynewstring)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,"\"");
  adms_k2strconcat(&myuid,mynewstring->_value);
  adms_k2strconcat(&myuid,"\"");
  return myuid;
}
static p_admst adms_string_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(string) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char* myvalue=aprintf(mytransform,(p_admst)myinputs[0]->data);
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_string_new(myvalue));
    free(myvalue);
    return myadmst;
  }
}

/* ------- subexpression -------------- */
void adms_subexpression_free (p_subexpression mysubexpression)
{
  if(!mysubexpression)
    return;
  free(mysubexpression);
}

void adms_subexpression_valueto_expression (p_subexpression mysubexpression,p_expression _expression)
{
  mysubexpression->_expression=_expression;
  mysubexpression->_expression_isdefault=0;
}
p_subexpression adms_subexpression_new (p_expression myexpression)
{
  p_subexpression mynewsubexpression=(p_subexpression) calloc(1,(size_t)sizeof(t_subexpression));
  mynewsubexpression->_math._adms._datatypename=admse_subexpression;
  mynewsubexpression->_math._value=adms_NAN;
  mynewsubexpression->_math._static=admse_no;
  mynewsubexpression->_math._dynamic=admse_no;
  mynewsubexpression->_math._dependency=admse_constant;
  mynewsubexpression->_expression=myexpression;
  mynewsubexpression->_expression_isdefault=1;
  return mynewsubexpression;
}
char*adms_subexpression_uid (p_subexpression mynewsubexpression)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_expression_uid(mynewsubexpression->_expression));
  return myuid;
}
static p_admst adms_subexpression_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(subexpression) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_expression myexpression=(p_expression)((p_admst)myinputs[0]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_subexpression_new(myexpression));
    return myadmst;
  }
}

/* ------- mapply_unary -------------- */
void adms_mapply_unary_free (p_mapply_unary mymapply_unary)
{
  if(!mymapply_unary)
    return;
  free(mymapply_unary);
}

void adms_mapply_unary_valueto_name (p_mapply_unary mymapply_unary,admse _name)
{
  mymapply_unary->_name=_name;
  mymapply_unary->_name_isdefault=0;
}

void adms_mapply_unary_valueto_arg1 (p_mapply_unary mymapply_unary,p_adms _arg1)
{
  mymapply_unary->_arg1=_arg1;
  mymapply_unary->_arg1_isdefault=0;
}
p_mapply_unary adms_mapply_unary_new (admse myname,p_adms myarg1)
{
  p_mapply_unary mynewmapply_unary=(p_mapply_unary) calloc(1,(size_t)sizeof(t_mapply_unary));
  mynewmapply_unary->_subexpression._math._adms._datatypename=admse_mapply_unary;
  mynewmapply_unary->_subexpression._math._value=adms_NAN;
  mynewmapply_unary->_subexpression._math._static=admse_no;
  mynewmapply_unary->_subexpression._math._dynamic=admse_no;
  mynewmapply_unary->_subexpression._math._dependency=admse_constant;
  mynewmapply_unary->_name=myname;
  mynewmapply_unary->_name_isdefault=1;
  mynewmapply_unary->_arg1=myarg1;
  mynewmapply_unary->_arg1_isdefault=1;
  return mynewmapply_unary;
}
char*adms_mapply_unary_uid (p_mapply_unary mynewmapply_unary)
{
  char*myuid=NULL;
  if(mynewmapply_unary->_name==admse_plus)
  {
    adms_k2strconcat(&myuid,"(+");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_unary->_arg1));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_unary->_name==admse_minus)
  {
    adms_k2strconcat(&myuid,"(-");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_unary->_arg1));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_unary->_name==admse_not)
  {
    adms_k2strconcat(&myuid,"(!");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_unary->_arg1));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_unary->_name==admse_bw_not)
  {
    adms_k2strconcat(&myuid,"(~");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_unary->_arg1));
    adms_k2strconcat(&myuid,")");
  }
  return myuid;
}
static p_admst adms_mapply_unary_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=2)
  {
    adms_message_fatal_continue(("admst:new(mapply_unary) bad number of inputs (found %i, expected 2)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char* mynames=aprintf(mytransform,(p_admst)myinputs[0]->data);
    admse mynamee=ns_strtoe(mynames);
    admse myname=mynamee;
    p_adms myarg1=(p_adms)((p_admst)myinputs[1]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_mapply_unary_new(myname,myarg1));
    if(!mynamee)
    {
      adms_message_fatal_continue(("cannot convert '%s' to a valid value\n",mynames))
      adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    }
    free(mynames);
    return myadmst;
  }
}

/* ------- mapply_binary -------------- */
void adms_mapply_binary_free (p_mapply_binary mymapply_binary)
{
  if(!mymapply_binary)
    return;
  free(mymapply_binary);
}

void adms_mapply_binary_valueto_name (p_mapply_binary mymapply_binary,admse _name)
{
  mymapply_binary->_name=_name;
  mymapply_binary->_name_isdefault=0;
}

void adms_mapply_binary_valueto_arg1 (p_mapply_binary mymapply_binary,p_adms _arg1)
{
  mymapply_binary->_arg1=_arg1;
  mymapply_binary->_arg1_isdefault=0;
}

void adms_mapply_binary_valueto_arg2 (p_mapply_binary mymapply_binary,p_adms _arg2)
{
  mymapply_binary->_arg2=_arg2;
  mymapply_binary->_arg2_isdefault=0;
}
p_mapply_binary adms_mapply_binary_new (admse myname,p_adms myarg1,p_adms myarg2)
{
  p_mapply_binary mynewmapply_binary=(p_mapply_binary) calloc(1,(size_t)sizeof(t_mapply_binary));
  mynewmapply_binary->_subexpression._math._adms._datatypename=admse_mapply_binary;
  mynewmapply_binary->_subexpression._math._value=adms_NAN;
  mynewmapply_binary->_subexpression._math._static=admse_no;
  mynewmapply_binary->_subexpression._math._dynamic=admse_no;
  mynewmapply_binary->_subexpression._math._dependency=admse_constant;
  mynewmapply_binary->_name=myname;
  mynewmapply_binary->_name_isdefault=1;
  mynewmapply_binary->_arg1=myarg1;
  mynewmapply_binary->_arg1_isdefault=1;
  mynewmapply_binary->_arg2=myarg2;
  mynewmapply_binary->_arg2_isdefault=1;
  return mynewmapply_binary;
}
char*adms_mapply_binary_uid (p_mapply_binary mynewmapply_binary)
{
  char*myuid=NULL;
  if(mynewmapply_binary->_name==admse_bw_equr)
  {
    adms_k2strconcat(&myuid,"(");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg1));
    adms_k2strconcat(&myuid,"^~");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg2));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_binary->_name==admse_bw_equl)
  {
    adms_k2strconcat(&myuid,"(");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg1));
    adms_k2strconcat(&myuid,"~^");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg2));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_binary->_name==admse_bw_xor)
  {
    adms_k2strconcat(&myuid,"(");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg1));
    adms_k2strconcat(&myuid,"^");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg2));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_binary->_name==admse_bw_or)
  {
    adms_k2strconcat(&myuid,"(");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg1));
    adms_k2strconcat(&myuid,"|");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg2));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_binary->_name==admse_bw_and)
  {
    adms_k2strconcat(&myuid,"(");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg1));
    adms_k2strconcat(&myuid,"&");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg2));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_binary->_name==admse_or)
  {
    adms_k2strconcat(&myuid,"(");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg1));
    adms_k2strconcat(&myuid,"||");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg2));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_binary->_name==admse_and)
  {
    adms_k2strconcat(&myuid,"(");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg1));
    adms_k2strconcat(&myuid,"&&");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg2));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_binary->_name==admse_equ)
  {
    adms_k2strconcat(&myuid,"(");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg1));
    adms_k2strconcat(&myuid,"==");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg2));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_binary->_name==admse_notequ)
  {
    adms_k2strconcat(&myuid,"(");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg1));
    adms_k2strconcat(&myuid,"!=");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg2));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_binary->_name==admse_lt)
  {
    adms_k2strconcat(&myuid,"(");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg1));
    adms_k2strconcat(&myuid,"<=");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg2));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_binary->_name==admse_lt_equ)
  {
    adms_k2strconcat(&myuid,"(");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg1));
    adms_k2strconcat(&myuid,"<=");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg2));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_binary->_name==admse_gt)
  {
    adms_k2strconcat(&myuid,"(");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg1));
    adms_k2strconcat(&myuid,">");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg2));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_binary->_name==admse_gt_equ)
  {
    adms_k2strconcat(&myuid,"(");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg1));
    adms_k2strconcat(&myuid,">=");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg2));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_binary->_name==admse_shiftr)
  {
    adms_k2strconcat(&myuid,"(");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg1));
    adms_k2strconcat(&myuid,">>");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg2));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_binary->_name==admse_shiftl)
  {
    adms_k2strconcat(&myuid,"(");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg1));
    adms_k2strconcat(&myuid,"<<");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg2));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_binary->_name==admse_addp)
  {
    adms_k2strconcat(&myuid,"(");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg1));
    adms_k2strconcat(&myuid,"+");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg2));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_binary->_name==admse_addm)
  {
    adms_k2strconcat(&myuid,"(");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg1));
    adms_k2strconcat(&myuid,"-");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg2));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_binary->_name==admse_multtime)
  {
    adms_k2strconcat(&myuid,"(");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg1));
    adms_k2strconcat(&myuid,"*");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg2));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_binary->_name==admse_multdiv)
  {
    adms_k2strconcat(&myuid,"(");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg1));
    adms_k2strconcat(&myuid,"/");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg2));
    adms_k2strconcat(&myuid,")");
  }
  if(mynewmapply_binary->_name==admse_multmod)
  {
    adms_k2strconcat(&myuid,"(");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg1));
    adms_k2strconcat(&myuid,"%");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_binary->_arg2));
    adms_k2strconcat(&myuid,")");
  }
  return myuid;
}
static p_admst adms_mapply_binary_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=3)
  {
    adms_message_fatal_continue(("admst:new(mapply_binary) bad number of inputs (found %i, expected 3)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char* mynames=aprintf(mytransform,(p_admst)myinputs[0]->data);
    admse mynamee=ns_strtoe(mynames);
    admse myname=mynamee;
    p_adms myarg1=(p_adms)((p_admst)myinputs[1]->data)->_item.p;
    p_adms myarg2=(p_adms)((p_admst)myinputs[2]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_mapply_binary_new(myname,myarg1,myarg2));
    if(!mynamee)
    {
      adms_message_fatal_continue(("cannot convert '%s' to a valid value\n",mynames))
      adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    }
    free(mynames);
    return myadmst;
  }
}

/* ------- mapply_ternary -------------- */
void adms_mapply_ternary_free (p_mapply_ternary mymapply_ternary)
{
  if(!mymapply_ternary)
    return;
  free(mymapply_ternary);
}

void adms_mapply_ternary_valueto_name (p_mapply_ternary mymapply_ternary,admse _name)
{
  mymapply_ternary->_name=_name;
  mymapply_ternary->_name_isdefault=0;
}

void adms_mapply_ternary_valueto_arg1 (p_mapply_ternary mymapply_ternary,p_adms _arg1)
{
  mymapply_ternary->_arg1=_arg1;
  mymapply_ternary->_arg1_isdefault=0;
}

void adms_mapply_ternary_valueto_arg2 (p_mapply_ternary mymapply_ternary,p_adms _arg2)
{
  mymapply_ternary->_arg2=_arg2;
  mymapply_ternary->_arg2_isdefault=0;
}

void adms_mapply_ternary_valueto_arg3 (p_mapply_ternary mymapply_ternary,p_adms _arg3)
{
  mymapply_ternary->_arg3=_arg3;
  mymapply_ternary->_arg3_isdefault=0;
}
p_mapply_ternary adms_mapply_ternary_new (admse myname,p_adms myarg1,p_adms myarg2,p_adms myarg3)
{
  p_mapply_ternary mynewmapply_ternary=(p_mapply_ternary) calloc(1,(size_t)sizeof(t_mapply_ternary));
  mynewmapply_ternary->_subexpression._math._adms._datatypename=admse_mapply_ternary;
  mynewmapply_ternary->_subexpression._math._value=adms_NAN;
  mynewmapply_ternary->_subexpression._math._static=admse_no;
  mynewmapply_ternary->_subexpression._math._dynamic=admse_no;
  mynewmapply_ternary->_subexpression._math._dependency=admse_constant;
  mynewmapply_ternary->_name=myname;
  mynewmapply_ternary->_name_isdefault=1;
  mynewmapply_ternary->_arg1=myarg1;
  mynewmapply_ternary->_arg1_isdefault=1;
  mynewmapply_ternary->_arg2=myarg2;
  mynewmapply_ternary->_arg2_isdefault=1;
  mynewmapply_ternary->_arg3=myarg3;
  mynewmapply_ternary->_arg3_isdefault=1;
  return mynewmapply_ternary;
}
char*adms_mapply_ternary_uid (p_mapply_ternary mynewmapply_ternary)
{
  char*myuid=NULL;
  if(mynewmapply_ternary->_name==admse_conditional)
  {
    adms_k2strconcat(&myuid,"(");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_ternary->_arg1));
    adms_k2strconcat(&myuid,"?");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_ternary->_arg2));
    adms_k2strconcat(&myuid,":");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewmapply_ternary->_arg3));
    adms_k2strconcat(&myuid,")");
  }
  return myuid;
}
static p_admst adms_mapply_ternary_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=4)
  {
    adms_message_fatal_continue(("admst:new(mapply_ternary) bad number of inputs (found %i, expected 4)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char* mynames=aprintf(mytransform,(p_admst)myinputs[0]->data);
    admse mynamee=ns_strtoe(mynames);
    admse myname=mynamee;
    p_adms myarg1=(p_adms)((p_admst)myinputs[1]->data)->_item.p;
    p_adms myarg2=(p_adms)((p_admst)myinputs[2]->data)->_item.p;
    p_adms myarg3=(p_adms)((p_admst)myinputs[3]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_mapply_ternary_new(myname,myarg1,myarg2,myarg3));
    if(!mynamee)
    {
      adms_message_fatal_continue(("cannot convert '%s' to a valid value\n",mynames))
      adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    }
    free(mynames);
    return myadmst;
  }
}

/* ------- number -------------- */
void adms_number_free (p_number mynumber)
{
  if(!mynumber)
    return;
  free(mynumber);
}

void adms_number_valueto_lexval (p_number mynumber,p_lexval _lexval)
{
  mynumber->_lexval=_lexval;
  mynumber->_lexval_isdefault=0;
}

void adms_number_valueto_scalingunit (p_number mynumber,admse _scalingunit)
{
  mynumber->_scalingunit=_scalingunit;
  mynumber->_scalingunit_isdefault=0;
}

void adms_number_valueto_cast (p_number mynumber,admse _cast)
{
  mynumber->_cast=_cast;
  mynumber->_cast_isdefault=0;
}
p_number adms_number_new (p_lexval mylexval)
{
  p_number mynewnumber=(p_number) calloc(1,(size_t)sizeof(t_number));
  mynewnumber->_subexpression._math._adms._datatypename=admse_number;
  mynewnumber->_subexpression._math._value=adms_NAN;
  mynewnumber->_subexpression._math._static=admse_no;
  mynewnumber->_subexpression._math._dynamic=admse_no;
  mynewnumber->_subexpression._math._dependency=admse_constant;
  mynewnumber->_lexval=mylexval;
  mynewnumber->_lexval_isdefault=1;
  mynewnumber->_scalingunit=admse_1;
  mynewnumber->_scalingunit_isdefault=1;
  mynewnumber->_cast=admse_d;
  mynewnumber->_cast_isdefault=1;
  return mynewnumber;
}
char*adms_number_uid (p_number mynewnumber)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_lexval_uid(mynewnumber->_lexval));
  return myuid;
}
static p_admst adms_number_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(number) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char* mylexvals=aprintf(mytransform,(p_admst)myinputs[0]->data);
    p_lexval mylexval=adms_lexval_new(mylexvals,adms_transform_uid(mytransform),0,0);
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_number_new(mylexval));
    free(mylexvals);
    return myadmst;
  }
}

/* ------- function -------------- */
void adms_function_free (p_function myfunction)
{
  if(!myfunction)
    return;
  adms_slist_free(myfunction->_arguments);
  free(myfunction);
}

void adms_function_valueto_lexval (p_function myfunction,p_lexval _lexval)
{
  myfunction->_lexval=_lexval;
  myfunction->_lexval_isdefault=0;
}

void adms_function_valueto_unique_id (p_function myfunction,int _unique_id)
{
  myfunction->_unique_id=_unique_id;
  myfunction->_unique_id_isdefault=0;
}

void adms_function_valueto_definition (p_function myfunction,p_analogfunction _definition)
{
  myfunction->_definition=_definition;
  myfunction->_definition_isdefault=0;
}

void adms_function_valueto_class (p_function myfunction,admse _class)
{
  myfunction->_class=_class;
  myfunction->_class_isdefault=0;
}

void adms_function_valueto_arguments (p_function myfunction,p_slist _arguments)
{
  myfunction->_arguments=_arguments;
  myfunction->_arguments_isdefault=0;
}
p_function adms_function_new (p_lexval mylexval,int myunique_id)
{
  p_function mynewfunction=(p_function) calloc(1,(size_t)sizeof(t_function));
  mynewfunction->_subexpression._math._adms._datatypename=admse_function;
  mynewfunction->_subexpression._math._value=adms_NAN;
  mynewfunction->_subexpression._math._static=admse_no;
  mynewfunction->_subexpression._math._dynamic=admse_no;
  mynewfunction->_subexpression._math._dependency=admse_constant;
  mynewfunction->_lexval=mylexval;
  mynewfunction->_lexval_isdefault=1;
  mynewfunction->_unique_id=myunique_id;
  mynewfunction->_unique_id_isdefault=1;
  mynewfunction->_definition_isdefault=-1;
  mynewfunction->_class=admse_analog;
  mynewfunction->_class_isdefault=1;
  mynewfunction->_arguments_isdefault=-1;
  return mynewfunction;
}
char*adms_function_uid (p_function mynewfunction)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_lexval_uid(mynewfunction->_lexval));
  adms_k2strconcat(&myuid,"(");
  {
    p_slist list=mynewfunction->_arguments;
    while(list)
    {
      adms_strconcat(&myuid,adms_admsxml_uid(list->data));
      if(list->next) adms_k2strconcat(&myuid,",");
      list=list->next;
    }
  }
  adms_k2strconcat(&myuid,")");
  return myuid;
}
static p_admst adms_function_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=2)
  {
    adms_message_fatal_continue(("admst:new(function) bad number of inputs (found %i, expected 2)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char* mylexvals=aprintf(mytransform,(p_admst)myinputs[0]->data);
    p_lexval mylexval=adms_lexval_new(mylexvals,adms_transform_uid(mytransform),0,0);
    char* myunique_ids=aprintf(mytransform,(p_admst)myinputs[1]->data);
    int myunique_id=adms_strtol(mytransform,myunique_ids);
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_function_new(mylexval,myunique_id));
    free(mylexvals);
    free(myunique_ids);
    return myadmst;
  }
}

/* ------- variable -------------- */
void adms_variable_free (p_variable myvariable)
{
  if(!myvariable)
    return;
  free(myvariable);
}

void adms_variable_valueto_prototype (p_variable myvariable,p_variableprototype _prototype)
{
  myvariable->_prototype=_prototype;
  myvariable->_prototype_isdefault=0;
}

void adms_variable_valueto_array (p_variable myvariable,p_array _array)
{
  myvariable->_array=_array;
  myvariable->_array_isdefault=0;
}
p_variable adms_variable_new (p_variableprototype myprototype)
{
  p_variable mynewvariable=(p_variable) calloc(1,(size_t)sizeof(t_variable));
  mynewvariable->_subexpression._math._adms._datatypename=admse_variable;
  mynewvariable->_subexpression._math._value=adms_NAN;
  mynewvariable->_subexpression._math._static=admse_no;
  mynewvariable->_subexpression._math._dynamic=admse_no;
  mynewvariable->_subexpression._math._dependency=admse_constant;
  mynewvariable->_prototype=myprototype;
  mynewvariable->_prototype_isdefault=1;
  mynewvariable->_array_isdefault=-1;
  return mynewvariable;
}
char*adms_variable_uid (p_variable mynewvariable)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_variableprototype_uid(mynewvariable->_prototype));
  return myuid;
}
static p_admst adms_variable_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size==3)
    return adms_variableprototype_xnew (mytransform,dot,myinputs,size);
  else  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(variable) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_variableprototype myprototype=(p_variableprototype)((p_admst)myinputs[0]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_variable_new(myprototype));
    return myadmst;
  }
}

/* ------- array -------------- */
void adms_array_free (p_array myarray)
{
  if(!myarray)
    return;
  free(myarray);
}

void adms_array_valueto_variable (p_array myarray,p_variable _variable)
{
  myarray->_variable=_variable;
  myarray->_variable_isdefault=0;
}

void adms_array_valueto_index (p_array myarray,p_adms _index)
{
  myarray->_index=_index;
  myarray->_index_isdefault=0;
}
p_array adms_array_new (p_variable myvariable,p_adms myindex)
{
  p_array mynewarray=(p_array) calloc(1,(size_t)sizeof(t_array));
  mynewarray->_subexpression._math._adms._datatypename=admse_array;
  mynewarray->_subexpression._math._value=adms_NAN;
  mynewarray->_subexpression._math._static=admse_no;
  mynewarray->_subexpression._math._dynamic=admse_no;
  mynewarray->_subexpression._math._dependency=admse_constant;
  mynewarray->_variable=myvariable;
  mynewarray->_variable_isdefault=1;
  mynewarray->_index=myindex;
  mynewarray->_index_isdefault=1;
  return mynewarray;
}
char*adms_array_uid (p_array mynewarray)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_variable_uid(mynewarray->_variable));
  adms_k2strconcat(&myuid,"[");
  adms_strconcat(&myuid,adms_admsxml_uid(mynewarray->_index));
  adms_k2strconcat(&myuid,"]");
  return myuid;
}
static p_admst adms_array_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=2)
  {
    adms_message_fatal_continue(("admst:new(array) bad number of inputs (found %i, expected 2)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_variable myvariable=(p_variable)((p_admst)myinputs[0]->data)->_item.p;
    p_adms myindex=(p_adms)((p_admst)myinputs[1]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_array_new(myvariable,myindex));
    return myadmst;
  }
}

/* ------- probe -------------- */
void adms_probe_free (p_probe myprobe)
{
  if(!myprobe)
    return;
  free(myprobe);
}

void adms_probe_valueto_module (p_probe myprobe,p_module _module)
{
  myprobe->_module=_module;
  myprobe->_module_isdefault=0;
}

void adms_probe_valueto_branch (p_probe myprobe,p_branch _branch)
{
  myprobe->_branch=_branch;
  myprobe->_branch_isdefault=0;
}

void adms_probe_valueto_nature (p_probe myprobe,p_nature _nature)
{
  myprobe->_nature=_nature;
  myprobe->_nature_isdefault=0;
}

void adms_probe_valueto_seen (p_probe myprobe,admse _seen)
{
  myprobe->_seen=_seen;
  myprobe->_seen_isdefault=0;
}

void adms_probe_valueto_discipline (p_probe myprobe,p_discipline _discipline)
{
  myprobe->_discipline=_discipline;
  myprobe->_discipline_isdefault=0;
}

void adms_probe_valueto_grounded (p_probe myprobe,admse _grounded)
{
  myprobe->_grounded=_grounded;
  myprobe->_grounded_isdefault=0;
}

void adms_probe_valueto_source (p_probe myprobe,p_source _source)
{
  myprobe->_source=_source;
  myprobe->_source_isdefault=0;
}
p_probe adms_probe_new (p_module mymodule,p_branch mybranch,p_nature mynature)
{
  p_probe mynewprobe=(p_probe) calloc(1,(size_t)sizeof(t_probe));
  mynewprobe->_subexpression._math._adms._datatypename=admse_probe;
  mynewprobe->_subexpression._math._value=adms_NAN;
  mynewprobe->_subexpression._math._static=admse_no;
  mynewprobe->_subexpression._math._dynamic=admse_no;
  mynewprobe->_subexpression._math._dependency=admse_constant;
  mynewprobe->_module=mymodule;
  mynewprobe->_module_isdefault=1;
  mynewprobe->_branch=mybranch;
  mynewprobe->_branch_isdefault=1;
  mynewprobe->_nature=mynature;
  mynewprobe->_nature_isdefault=1;
  mynewprobe->_seen=admse_no;
  mynewprobe->_seen_isdefault=1;
  mynewprobe->_discipline_isdefault=-1;
  mynewprobe->_grounded=admse_no;
  mynewprobe->_grounded_isdefault=1;
  mynewprobe->_source_isdefault=-1;
  return mynewprobe;
}
char*adms_probe_uid (p_probe mynewprobe)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_nature_uid(mynewprobe->_nature));
  adms_k2strconcat(&myuid,"(");
  adms_strconcat(&myuid,adms_branch_uid(mynewprobe->_branch));
  adms_k2strconcat(&myuid,")");
  return myuid;
}
static p_admst adms_probe_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=3)
  {
    adms_message_fatal_continue(("admst:new(probe) bad number of inputs (found %i, expected 3)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_module mymodule=(p_module)((p_admst)myinputs[0]->data)->_item.p;
    p_branch mybranch=(p_branch)((p_admst)myinputs[1]->data)->_item.p;
    p_nature mynature=(p_nature)((p_admst)myinputs[2]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_probe_new(mymodule,mybranch,mynature));
    return myadmst;
  }
}

/* ------- expression -------------- */
void adms_expression_free (p_expression myexpression)
{
  if(!myexpression)
    return;
  free(myexpression->_hasspecialnumber);
  adms_slist_free(myexpression->_function);
  adms_slist_free(myexpression->_probe);
  adms_slist_free(myexpression->_variable);
  free(myexpression);
}

void adms_expression_valueto_module (p_expression myexpression,p_module _module)
{
  myexpression->_module=_module;
  myexpression->_module_isdefault=0;
}

void adms_expression_valueto_tree (p_expression myexpression,p_adms _tree)
{
  myexpression->_tree=_tree;
  myexpression->_tree_isdefault=0;
}

void adms_expression_valueto_hasspecialnumber (p_expression myexpression,const char* _hasspecialnumber)
{
  myexpression->_hasspecialnumber=adms_kclone(_hasspecialnumber);
  myexpression->_hasspecialnumber_isdefault=0;
}

void adms_expression_valueto_hasVoltageDependentFunction (p_expression myexpression,admse _hasVoltageDependentFunction)
{
  myexpression->_hasVoltageDependentFunction=_hasVoltageDependentFunction;
  myexpression->_hasVoltageDependentFunction_isdefault=0;
}

void adms_expression_valueto_infinity (p_expression myexpression,admse _infinity)
{
  myexpression->_infinity=_infinity;
  myexpression->_infinity_isdefault=0;
}

void adms_expression_valueto_TemperatureDependent (p_expression myexpression,admse _TemperatureDependent)
{
  myexpression->_TemperatureDependent=_TemperatureDependent;
  myexpression->_TemperatureDependent_isdefault=0;
}

void adms_expression_valueto_value (p_expression myexpression,admse _value)
{
  myexpression->_value=_value;
  myexpression->_value_isdefault=0;
}

void adms_expression_valueto_function (p_expression myexpression,p_slist _function)
{
  myexpression->_function=_function;
  myexpression->_function_isdefault=0;
}

void adms_expression_valueto_probe (p_expression myexpression,p_slist _probe)
{
  myexpression->_probe=_probe;
  myexpression->_probe_isdefault=0;
}

void adms_expression_valueto_variable (p_expression myexpression,p_slist _variable)
{
  myexpression->_variable=_variable;
  myexpression->_variable_isdefault=0;
}
p_expression adms_expression_new (p_module mymodule,p_adms mytree)
{
  p_expression mynewexpression=(p_expression) calloc(1,(size_t)sizeof(t_expression));
  mynewexpression->_math._adms._datatypename=admse_expression;
  mynewexpression->_math._value=adms_NAN;
  mynewexpression->_math._static=admse_no;
  mynewexpression->_math._dynamic=admse_no;
  mynewexpression->_math._dependency=admse_constant;
  mynewexpression->_module=mymodule;
  mynewexpression->_module_isdefault=1;
  mynewexpression->_tree=mytree;
  mynewexpression->_tree_isdefault=1;
  mynewexpression->_hasspecialnumber=adms_kclone("NO");
  mynewexpression->_hasspecialnumber_isdefault=1;
  mynewexpression->_hasVoltageDependentFunction=admse_no;
  mynewexpression->_hasVoltageDependentFunction_isdefault=1;
  mynewexpression->_infinity=admse_no;
  mynewexpression->_infinity_isdefault=1;
  mynewexpression->_TemperatureDependent=admse_no;
  mynewexpression->_TemperatureDependent_isdefault=1;
  mynewexpression->_value=admse_is_any;
  mynewexpression->_value_isdefault=1;
  mynewexpression->_function_isdefault=-1;
  mynewexpression->_probe_isdefault=-1;
  mynewexpression->_variable_isdefault=-1;
  return mynewexpression;
}
char*adms_expression_uid (p_expression mynewexpression)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,"(");
  adms_strconcat(&myuid,adms_admsxml_uid(mynewexpression->_tree));
  adms_k2strconcat(&myuid,")");
  return myuid;
}
static p_admst adms_expression_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=2)
  {
    adms_message_fatal_continue(("admst:new(expression) bad number of inputs (found %i, expected 2)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_module mymodule=(p_module)((p_admst)myinputs[0]->data)->_item.p;
    p_adms mytree=(p_adms)((p_admst)myinputs[1]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_expression_new(mymodule,mytree));
    return myadmst;
  }
}

/* ------- instance -------------- */
void adms_instance_free (p_instance myinstance)
{
  if(!myinstance)
    return;
  free(myinstance->_name);
  adms_slist_free(myinstance->_terminal);
  adms_slist_free(myinstance->_parameterset);
  free(myinstance);
}

void adms_instance_valueto_instantiator (p_instance myinstance,p_module _instantiator)
{
  myinstance->_instantiator=_instantiator;
  myinstance->_instantiator_isdefault=0;
}

void adms_instance_valueto_module (p_instance myinstance,p_module _module)
{
  myinstance->_module=_module;
  myinstance->_module_isdefault=0;
}

void adms_instance_valueto_name (p_instance myinstance,const char* _name)
{
  myinstance->_name=adms_kclone(_name);
  myinstance->_name_isdefault=0;
}

void adms_instance_valueto_terminal (p_instance myinstance,p_slist _terminal)
{
  myinstance->_terminal=_terminal;
  myinstance->_terminal_isdefault=0;
}
void adms_instance_list_terminal_prepend_once_or_abort (p_instance mymyinstance,p_instancenode myterminal)
{
  p_slist list=mymyinstance->_terminal;
  while(list)
  {
    if(!adms_instancenode_cmp((p_instancenode)list->data,myterminal))
    {
      adms_message_fatal(("instance=[%s] terminal=[%s] already defined\n",
        adms_instance_uid(mymyinstance),adms_instancenode_uid(myterminal)))
      return;
    }
    list=list->next;
  }
  adms_slist_push(&mymyinstance->_terminal,(p_adms)myterminal);
}

void adms_instance_valueto_parameterset (p_instance myinstance,p_slist _parameterset)
{
  myinstance->_parameterset=_parameterset;
  myinstance->_parameterset_isdefault=0;
}
void adms_instance_list_parameterset_prepend_once_or_abort (p_instance mymyinstance,p_instanceparameter myparameterset)
{
  p_slist list=mymyinstance->_parameterset;
  while(list)
  {
    if(!adms_instanceparameter_cmp((p_instanceparameter)list->data,myparameterset))
    {
      adms_message_fatal(("instance=[%s] parameterset=[%s] already defined\n",
        adms_instance_uid(mymyinstance),adms_instanceparameter_uid(myparameterset)))
      return;
    }
    list=list->next;
  }
  adms_slist_push(&mymyinstance->_parameterset,(p_adms)myparameterset);
}
p_instance adms_instance_new (p_module myinstantiator,p_module mymodule,const char* myname)
{
  p_instance mynewinstance=(p_instance) calloc(1,(size_t)sizeof(t_instance));
  mynewinstance->_math._adms._datatypename=admse_instance;
  mynewinstance->_math._value=adms_NAN;
  mynewinstance->_math._static=admse_no;
  mynewinstance->_math._dynamic=admse_no;
  mynewinstance->_math._dependency=admse_constant;
  mynewinstance->_instantiator=myinstantiator;
  mynewinstance->_instantiator_isdefault=1;
  mynewinstance->_module=mymodule;
  mynewinstance->_module_isdefault=1;
  mynewinstance->_name=adms_kclone(myname);
  mynewinstance->_name_isdefault=1;
  mynewinstance->_terminal_isdefault=-1;
  mynewinstance->_parameterset_isdefault=-1;
  return mynewinstance;
}
char*adms_instance_uid (p_instance mynewinstance)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_module_uid(mynewinstance->_instantiator));
  adms_k2strconcat(&myuid,".");
  adms_strconcat(&myuid,adms_module_uid(mynewinstance->_module));
  adms_k2strconcat(&myuid,"#");
  adms_k2strconcat(&myuid,mynewinstance->_name);
  return myuid;
}
static p_admst adms_instance_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=3)
  {
    adms_message_fatal_continue(("admst:new(instance) bad number of inputs (found %i, expected 3)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_module myinstantiator=(p_module)((p_admst)myinputs[0]->data)->_item.p;
    p_module mymodule=(p_module)((p_admst)myinputs[1]->data)->_item.p;
    char* myname=aprintf(mytransform,(p_admst)myinputs[2]->data);
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_instance_new(myinstantiator,mymodule,myname));
    free(myname);
    return myadmst;
  }
}

/* ------- instanceparameter -------------- */
void adms_instanceparameter_free (p_instanceparameter myinstanceparameter)
{
  if(!myinstanceparameter)
    return;
  free(myinstanceparameter);
}

void adms_instanceparameter_valueto_parameter (p_instanceparameter myinstanceparameter,p_variableprototype _parameter)
{
  myinstanceparameter->_parameter=_parameter;
  myinstanceparameter->_parameter_isdefault=0;
}

void adms_instanceparameter_valueto_value (p_instanceparameter myinstanceparameter,p_expression _value)
{
  myinstanceparameter->_value=_value;
  myinstanceparameter->_value_isdefault=0;
}
p_instanceparameter adms_instanceparameter_new (p_variableprototype myparameter)
{
  p_instanceparameter mynewinstanceparameter=(p_instanceparameter) calloc(1,(size_t)sizeof(t_instanceparameter));
  mynewinstanceparameter->_math._adms._datatypename=admse_instanceparameter;
  mynewinstanceparameter->_math._value=adms_NAN;
  mynewinstanceparameter->_math._static=admse_no;
  mynewinstanceparameter->_math._dynamic=admse_no;
  mynewinstanceparameter->_math._dependency=admse_constant;
  mynewinstanceparameter->_parameter=myparameter;
  mynewinstanceparameter->_parameter_isdefault=1;
  mynewinstanceparameter->_value_isdefault=-1;
  return mynewinstanceparameter;
}
char*adms_instanceparameter_uid (p_instanceparameter mynewinstanceparameter)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_variableprototype_uid(mynewinstanceparameter->_parameter));
  adms_k2strconcat(&myuid,"#");
  adms_strconcat(&myuid,adms_expression_uid(mynewinstanceparameter->_value));
  return myuid;
}
static p_admst adms_instanceparameter_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(instanceparameter) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_variableprototype myparameter=(p_variableprototype)((p_admst)myinputs[0]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_instanceparameter_new(myparameter));
    return myadmst;
  }
}

/* ------- instancenode -------------- */
void adms_instancenode_free (p_instancenode myinstancenode)
{
  if(!myinstancenode)
    return;
  free(myinstancenode);
}

void adms_instancenode_valueto_nodefrominstantiator (p_instancenode myinstancenode,p_node _nodefrominstantiator)
{
  myinstancenode->_nodefrominstantiator=_nodefrominstantiator;
  myinstancenode->_nodefrominstantiator_isdefault=0;
}

void adms_instancenode_valueto_nodefrommodule (p_instancenode myinstancenode,p_node _nodefrommodule)
{
  myinstancenode->_nodefrommodule=_nodefrommodule;
  myinstancenode->_nodefrommodule_isdefault=0;
}
p_instancenode adms_instancenode_new (p_node mynodefrominstantiator,p_node mynodefrommodule)
{
  p_instancenode mynewinstancenode=(p_instancenode) calloc(1,(size_t)sizeof(t_instancenode));
  mynewinstancenode->_math._adms._datatypename=admse_instancenode;
  mynewinstancenode->_math._value=adms_NAN;
  mynewinstancenode->_math._static=admse_no;
  mynewinstancenode->_math._dynamic=admse_no;
  mynewinstancenode->_math._dependency=admse_constant;
  mynewinstancenode->_nodefrominstantiator=mynodefrominstantiator;
  mynewinstancenode->_nodefrominstantiator_isdefault=1;
  mynewinstancenode->_nodefrommodule=mynodefrommodule;
  mynewinstancenode->_nodefrommodule_isdefault=1;
  return mynewinstancenode;
}
char*adms_instancenode_uid (p_instancenode mynewinstancenode)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_node_uid(mynewinstancenode->_nodefrominstantiator));
  adms_k2strconcat(&myuid,"#");
  adms_strconcat(&myuid,adms_node_uid(mynewinstancenode->_nodefrommodule));
  return myuid;
}
static p_admst adms_instancenode_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=2)
  {
    adms_message_fatal_continue(("admst:new(instancenode) bad number of inputs (found %i, expected 2)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_node mynodefrominstantiator=(p_node)((p_admst)myinputs[0]->data)->_item.p;
    p_node mynodefrommodule=(p_node)((p_admst)myinputs[1]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_instancenode_new(mynodefrominstantiator,mynodefrommodule));
    return myadmst;
  }
}

/* ------- nilled -------------- */
void adms_nilled_free (p_nilled mynilled)
{
  if(!mynilled)
    return;
  free(mynilled->_name);
  free(mynilled);
}

void adms_nilled_valueto_module (p_nilled mynilled,p_module _module)
{
  mynilled->_module=_module;
  mynilled->_module_isdefault=0;
}

void adms_nilled_valueto_name (p_nilled mynilled,const char* _name)
{
  mynilled->_name=adms_kclone(_name);
  mynilled->_name_isdefault=0;
}
p_nilled adms_nilled_new (p_module mymodule)
{
  p_nilled mynewnilled=(p_nilled) calloc(1,(size_t)sizeof(t_nilled));
  mynewnilled->_math._adms._datatypename=admse_nilled;
  mynewnilled->_math._value=adms_NAN;
  mynewnilled->_math._static=admse_no;
  mynewnilled->_math._dynamic=admse_no;
  mynewnilled->_math._dependency=admse_constant;
  mynewnilled->_module=mymodule;
  mynewnilled->_module_isdefault=1;
  mynewnilled->_name=adms_kclone("");
  mynewnilled->_name_isdefault=1;
  return mynewnilled;
}
char*adms_nilled_uid (p_nilled mynewnilled)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,"nilled operator");
  return myuid;
}
static p_admst adms_nilled_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(nilled) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_module mymodule=(p_module)((p_admst)myinputs[0]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_nilled_new(mymodule));
    return myadmst;
  }
}

/* ------- assignment -------------- */
void adms_assignment_free (p_assignment myassignment)
{
  if(!myassignment)
    return;
  free(myassignment->_name);
  adms_slist_free(myassignment->_attribute);
  free(myassignment);
}

void adms_assignment_valueto_module (p_assignment myassignment,p_module _module)
{
  myassignment->_module=_module;
  myassignment->_module_isdefault=0;
}

void adms_assignment_valueto_lhs (p_assignment myassignment,p_adms _lhs)
{
  myassignment->_lhs=_lhs;
  myassignment->_lhs_isdefault=0;
}

void adms_assignment_valueto_rhs (p_assignment myassignment,p_expression _rhs)
{
  myassignment->_rhs=_rhs;
  myassignment->_rhs_isdefault=0;
}

void adms_assignment_valueto_lexval (p_assignment myassignment,p_lexval _lexval)
{
  myassignment->_lexval=_lexval;
  myassignment->_lexval_isdefault=0;
}

void adms_assignment_valueto_name (p_assignment myassignment,const char* _name)
{
  myassignment->_name=adms_kclone(_name);
  myassignment->_name_isdefault=0;
}

void adms_assignment_valueto_attribute (p_assignment myassignment,p_slist _attribute)
{
  myassignment->_attribute=_attribute;
  myassignment->_attribute_isdefault=0;
}
p_assignment adms_assignment_new (p_module mymodule,p_adms mylhs,p_expression myrhs,p_lexval mylexval)
{
  p_assignment mynewassignment=(p_assignment) calloc(1,(size_t)sizeof(t_assignment));
  mynewassignment->_math._adms._datatypename=admse_assignment;
  mynewassignment->_math._value=adms_NAN;
  mynewassignment->_math._static=admse_no;
  mynewassignment->_math._dynamic=admse_no;
  mynewassignment->_math._dependency=admse_constant;
  mynewassignment->_module=mymodule;
  mynewassignment->_module_isdefault=1;
  mynewassignment->_lhs=mylhs;
  mynewassignment->_lhs_isdefault=1;
  mynewassignment->_rhs=myrhs;
  mynewassignment->_rhs_isdefault=1;
  mynewassignment->_lexval=mylexval;
  mynewassignment->_lexval_isdefault=1;
  mynewassignment->_name=adms_kclone("");
  mynewassignment->_name_isdefault=1;
  mynewassignment->_attribute_isdefault=-1;
  return mynewassignment;
}
char*adms_assignment_uid (p_assignment mynewassignment)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_admsxml_uid(mynewassignment->_lhs));
  adms_k2strconcat(&myuid,"=");
  adms_strconcat(&myuid,adms_expression_uid(mynewassignment->_rhs));
  adms_k2strconcat(&myuid,";");
  return myuid;
}
static p_admst adms_assignment_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=4)
  {
    adms_message_fatal_continue(("admst:new(assignment) bad number of inputs (found %i, expected 4)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_module mymodule=(p_module)((p_admst)myinputs[0]->data)->_item.p;
    p_adms mylhs=(p_adms)((p_admst)myinputs[1]->data)->_item.p;
    p_expression myrhs=(p_expression)((p_admst)myinputs[2]->data)->_item.p;
    char* mylexvals=aprintf(mytransform,(p_admst)myinputs[3]->data);
    p_lexval mylexval=adms_lexval_new(mylexvals,adms_transform_uid(mytransform),0,0);
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_assignment_new(mymodule,mylhs,myrhs,mylexval));
    free(mylexvals);
    return myadmst;
  }
}

/* ------- contribution -------------- */
void adms_contribution_free (p_contribution mycontribution)
{
  if(!mycontribution)
    return;
  free(mycontribution->_name);
  adms_slist_free(mycontribution->_attribute);
  free(mycontribution);
}

void adms_contribution_valueto_module (p_contribution mycontribution,p_module _module)
{
  mycontribution->_module=_module;
  mycontribution->_module_isdefault=0;
}

void adms_contribution_valueto_lhs (p_contribution mycontribution,p_source _lhs)
{
  mycontribution->_lhs=_lhs;
  mycontribution->_lhs_isdefault=0;
}

void adms_contribution_valueto_rhs (p_contribution mycontribution,p_expression _rhs)
{
  mycontribution->_rhs=_rhs;
  mycontribution->_rhs_isdefault=0;
}

void adms_contribution_valueto_lexval (p_contribution mycontribution,p_lexval _lexval)
{
  mycontribution->_lexval=_lexval;
  mycontribution->_lexval_isdefault=0;
}

void adms_contribution_valueto_branchalias (p_contribution mycontribution,p_branchalias _branchalias)
{
  mycontribution->_branchalias=_branchalias;
  mycontribution->_branchalias_isdefault=0;
}

void adms_contribution_valueto_name (p_contribution mycontribution,const char* _name)
{
  mycontribution->_name=adms_kclone(_name);
  mycontribution->_name_isdefault=0;
}

void adms_contribution_valueto_whitenoise (p_contribution mycontribution,admse _whitenoise)
{
  mycontribution->_whitenoise=_whitenoise;
  mycontribution->_whitenoise_isdefault=0;
}

void adms_contribution_valueto_flickernoise (p_contribution mycontribution,admse _flickernoise)
{
  mycontribution->_flickernoise=_flickernoise;
  mycontribution->_flickernoise_isdefault=0;
}

void adms_contribution_valueto_attribute (p_contribution mycontribution,p_slist _attribute)
{
  mycontribution->_attribute=_attribute;
  mycontribution->_attribute_isdefault=0;
}
p_contribution adms_contribution_new (p_module mymodule,p_source mylhs,p_expression myrhs,p_lexval mylexval)
{
  p_contribution mynewcontribution=(p_contribution) calloc(1,(size_t)sizeof(t_contribution));
  mynewcontribution->_math._adms._datatypename=admse_contribution;
  mynewcontribution->_math._value=adms_NAN;
  mynewcontribution->_math._static=admse_no;
  mynewcontribution->_math._dynamic=admse_no;
  mynewcontribution->_math._dependency=admse_constant;
  mynewcontribution->_module=mymodule;
  mynewcontribution->_module_isdefault=1;
  mynewcontribution->_lhs=mylhs;
  mynewcontribution->_lhs_isdefault=1;
  mynewcontribution->_rhs=myrhs;
  mynewcontribution->_rhs_isdefault=1;
  mynewcontribution->_lexval=mylexval;
  mynewcontribution->_lexval_isdefault=1;
  mynewcontribution->_branchalias_isdefault=-1;
  mynewcontribution->_name=adms_kclone("");
  mynewcontribution->_name_isdefault=1;
  mynewcontribution->_whitenoise=admse_no;
  mynewcontribution->_whitenoise_isdefault=1;
  mynewcontribution->_flickernoise=admse_no;
  mynewcontribution->_flickernoise_isdefault=1;
  mynewcontribution->_attribute_isdefault=-1;
  return mynewcontribution;
}
char*adms_contribution_uid (p_contribution mynewcontribution)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_source_uid(mynewcontribution->_lhs));
  adms_k2strconcat(&myuid,"<+");
  adms_strconcat(&myuid,adms_expression_uid(mynewcontribution->_rhs));
  adms_k2strconcat(&myuid,";");
  return myuid;
}
static p_admst adms_contribution_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=4)
  {
    adms_message_fatal_continue(("admst:new(contribution) bad number of inputs (found %i, expected 4)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_module mymodule=(p_module)((p_admst)myinputs[0]->data)->_item.p;
    p_source mylhs=(p_source)((p_admst)myinputs[1]->data)->_item.p;
    p_expression myrhs=(p_expression)((p_admst)myinputs[2]->data)->_item.p;
    char* mylexvals=aprintf(mytransform,(p_admst)myinputs[3]->data);
    p_lexval mylexval=adms_lexval_new(mylexvals,adms_transform_uid(mytransform),0,0);
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_contribution_new(mymodule,mylhs,myrhs,mylexval));
    free(mylexvals);
    return myadmst;
  }
}

/* ------- conditional -------------- */
void adms_conditional_free (p_conditional myconditional)
{
  if(!myconditional)
    return;
  free(myconditional->_name);
  free(myconditional);
}

void adms_conditional_valueto_module (p_conditional myconditional,p_module _module)
{
  myconditional->_module=_module;
  myconditional->_module_isdefault=0;
}

void adms_conditional_valueto_if (p_conditional myconditional,p_expression _if)
{
  myconditional->_if=_if;
  myconditional->_if_isdefault=0;
}

void adms_conditional_valueto_then (p_conditional myconditional,p_adms _then)
{
  myconditional->_then=_then;
  myconditional->_then_isdefault=0;
}

void adms_conditional_valueto_else (p_conditional myconditional,p_adms _else)
{
  myconditional->_else=_else;
  myconditional->_else_isdefault=0;
}

void adms_conditional_valueto_name (p_conditional myconditional,const char* _name)
{
  myconditional->_name=adms_kclone(_name);
  myconditional->_name_isdefault=0;
}
p_conditional adms_conditional_new (p_module mymodule,p_expression myif,p_adms mythen,p_adms myelse)
{
  p_conditional mynewconditional=(p_conditional) calloc(1,(size_t)sizeof(t_conditional));
  mynewconditional->_math._adms._datatypename=admse_conditional;
  mynewconditional->_math._value=adms_NAN;
  mynewconditional->_math._static=admse_no;
  mynewconditional->_math._dynamic=admse_no;
  mynewconditional->_math._dependency=admse_constant;
  mynewconditional->_module=mymodule;
  mynewconditional->_module_isdefault=1;
  mynewconditional->_if=myif;
  mynewconditional->_if_isdefault=1;
  mynewconditional->_then=mythen;
  mynewconditional->_then_isdefault=1;
  mynewconditional->_else=myelse;
  mynewconditional->_else_isdefault=1;
  mynewconditional->_name=adms_kclone("");
  mynewconditional->_name_isdefault=1;
  return mynewconditional;
}
char*adms_conditional_uid (p_conditional mynewconditional)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,"if(");
  adms_strconcat(&myuid,adms_expression_uid(mynewconditional->_if));
  adms_k2strconcat(&myuid,") ");
  adms_strconcat(&myuid,adms_admsxml_uid(mynewconditional->_then));
  if(mynewconditional->_else)
  {
    adms_k2strconcat(&myuid," else ");
    adms_strconcat(&myuid,adms_admsxml_uid(mynewconditional->_else));
  }
  return myuid;
}
static p_admst adms_conditional_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=4)
  {
    adms_message_fatal_continue(("admst:new(conditional) bad number of inputs (found %i, expected 4)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_module mymodule=(p_module)((p_admst)myinputs[0]->data)->_item.p;
    p_expression myif=(p_expression)((p_admst)myinputs[1]->data)->_item.p;
    p_adms mythen=(p_adms)((p_admst)myinputs[2]->data)->_item.p;
    p_adms myelse=(p_adms)((p_admst)myinputs[3]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_conditional_new(mymodule,myif,mythen,myelse));
    return myadmst;
  }
}

/* ------- forloop -------------- */
void adms_forloop_free (p_forloop myforloop)
{
  if(!myforloop)
    return;
  free(myforloop->_name);
  free(myforloop);
}

void adms_forloop_valueto_module (p_forloop myforloop,p_module _module)
{
  myforloop->_module=_module;
  myforloop->_module_isdefault=0;
}

void adms_forloop_valueto_initial (p_forloop myforloop,p_assignment _initial)
{
  myforloop->_initial=_initial;
  myforloop->_initial_isdefault=0;
}

void adms_forloop_valueto_condition (p_forloop myforloop,p_expression _condition)
{
  myforloop->_condition=_condition;
  myforloop->_condition_isdefault=0;
}

void adms_forloop_valueto_update (p_forloop myforloop,p_assignment _update)
{
  myforloop->_update=_update;
  myforloop->_update_isdefault=0;
}

void adms_forloop_valueto_forblock (p_forloop myforloop,p_adms _forblock)
{
  myforloop->_forblock=_forblock;
  myforloop->_forblock_isdefault=0;
}

void adms_forloop_valueto_name (p_forloop myforloop,const char* _name)
{
  myforloop->_name=adms_kclone(_name);
  myforloop->_name_isdefault=0;
}
p_forloop adms_forloop_new (p_module mymodule,p_assignment myinitial,p_expression mycondition,p_assignment myupdate,p_adms myforblock)
{
  p_forloop mynewforloop=(p_forloop) calloc(1,(size_t)sizeof(t_forloop));
  mynewforloop->_math._adms._datatypename=admse_forloop;
  mynewforloop->_math._value=adms_NAN;
  mynewforloop->_math._static=admse_no;
  mynewforloop->_math._dynamic=admse_no;
  mynewforloop->_math._dependency=admse_constant;
  mynewforloop->_module=mymodule;
  mynewforloop->_module_isdefault=1;
  mynewforloop->_initial=myinitial;
  mynewforloop->_initial_isdefault=1;
  mynewforloop->_condition=mycondition;
  mynewforloop->_condition_isdefault=1;
  mynewforloop->_update=myupdate;
  mynewforloop->_update_isdefault=1;
  mynewforloop->_forblock=myforblock;
  mynewforloop->_forblock_isdefault=1;
  mynewforloop->_name=adms_kclone("");
  mynewforloop->_name_isdefault=1;
  return mynewforloop;
}
char*adms_forloop_uid (p_forloop mynewforloop)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,"for(");
  adms_strconcat(&myuid,adms_assignment_uid(mynewforloop->_initial));
  adms_k2strconcat(&myuid,";");
  adms_strconcat(&myuid,adms_expression_uid(mynewforloop->_condition));
  adms_k2strconcat(&myuid,";");
  adms_strconcat(&myuid,adms_assignment_uid(mynewforloop->_update));
  adms_k2strconcat(&myuid,")\n");
  adms_strconcat(&myuid,adms_admsxml_uid(mynewforloop->_forblock));
  return myuid;
}
static p_admst adms_forloop_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=5)
  {
    adms_message_fatal_continue(("admst:new(forloop) bad number of inputs (found %i, expected 5)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_module mymodule=(p_module)((p_admst)myinputs[0]->data)->_item.p;
    p_assignment myinitial=(p_assignment)((p_admst)myinputs[1]->data)->_item.p;
    p_expression mycondition=(p_expression)((p_admst)myinputs[2]->data)->_item.p;
    p_assignment myupdate=(p_assignment)((p_admst)myinputs[3]->data)->_item.p;
    p_adms myforblock=(p_adms)((p_admst)myinputs[4]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_forloop_new(mymodule,myinitial,mycondition,myupdate,myforblock));
    return myadmst;
  }
}

/* ------- whileloop -------------- */
void adms_whileloop_free (p_whileloop mywhileloop)
{
  if(!mywhileloop)
    return;
  free(mywhileloop->_name);
  free(mywhileloop);
}

void adms_whileloop_valueto_module (p_whileloop mywhileloop,p_module _module)
{
  mywhileloop->_module=_module;
  mywhileloop->_module_isdefault=0;
}

void adms_whileloop_valueto_while (p_whileloop mywhileloop,p_expression _while)
{
  mywhileloop->_while=_while;
  mywhileloop->_while_isdefault=0;
}

void adms_whileloop_valueto_whileblock (p_whileloop mywhileloop,p_adms _whileblock)
{
  mywhileloop->_whileblock=_whileblock;
  mywhileloop->_whileblock_isdefault=0;
}

void adms_whileloop_valueto_name (p_whileloop mywhileloop,const char* _name)
{
  mywhileloop->_name=adms_kclone(_name);
  mywhileloop->_name_isdefault=0;
}
p_whileloop adms_whileloop_new (p_module mymodule,p_expression mywhile,p_adms mywhileblock)
{
  p_whileloop mynewwhileloop=(p_whileloop) calloc(1,(size_t)sizeof(t_whileloop));
  mynewwhileloop->_math._adms._datatypename=admse_whileloop;
  mynewwhileloop->_math._value=adms_NAN;
  mynewwhileloop->_math._static=admse_no;
  mynewwhileloop->_math._dynamic=admse_no;
  mynewwhileloop->_math._dependency=admse_constant;
  mynewwhileloop->_module=mymodule;
  mynewwhileloop->_module_isdefault=1;
  mynewwhileloop->_while=mywhile;
  mynewwhileloop->_while_isdefault=1;
  mynewwhileloop->_whileblock=mywhileblock;
  mynewwhileloop->_whileblock_isdefault=1;
  mynewwhileloop->_name=adms_kclone("");
  mynewwhileloop->_name_isdefault=1;
  return mynewwhileloop;
}
char*adms_whileloop_uid (p_whileloop mynewwhileloop)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,"while(");
  adms_strconcat(&myuid,adms_expression_uid(mynewwhileloop->_while));
  adms_k2strconcat(&myuid,")\n");
  adms_strconcat(&myuid,adms_admsxml_uid(mynewwhileloop->_whileblock));
  return myuid;
}
static p_admst adms_whileloop_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=3)
  {
    adms_message_fatal_continue(("admst:new(whileloop) bad number of inputs (found %i, expected 3)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_module mymodule=(p_module)((p_admst)myinputs[0]->data)->_item.p;
    p_expression mywhile=(p_expression)((p_admst)myinputs[1]->data)->_item.p;
    p_adms mywhileblock=(p_adms)((p_admst)myinputs[2]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_whileloop_new(mymodule,mywhile,mywhileblock));
    return myadmst;
  }
}

/* ------- case -------------- */
void adms_case_free (p_case mycase)
{
  if(!mycase)
    return;
  free(mycase->_name);
  adms_slist_free(mycase->_caseitem);
  free(mycase);
}

void adms_case_valueto_module (p_case mycase,p_module _module)
{
  mycase->_module=_module;
  mycase->_module_isdefault=0;
}

void adms_case_valueto_case (p_case mycase,p_expression _case)
{
  mycase->_case=_case;
  mycase->_case_isdefault=0;
}

void adms_case_valueto_name (p_case mycase,const char* _name)
{
  mycase->_name=adms_kclone(_name);
  mycase->_name_isdefault=0;
}

void adms_case_valueto_caseitem (p_case mycase,p_slist _caseitem)
{
  mycase->_caseitem=_caseitem;
  mycase->_caseitem_isdefault=0;
}
p_case adms_case_new (p_module mymodule,p_expression mycase)
{
  p_case mynewcase=(p_case) calloc(1,(size_t)sizeof(t_case));
  mynewcase->_math._adms._datatypename=admse_case;
  mynewcase->_math._value=adms_NAN;
  mynewcase->_math._static=admse_no;
  mynewcase->_math._dynamic=admse_no;
  mynewcase->_math._dependency=admse_constant;
  mynewcase->_module=mymodule;
  mynewcase->_module_isdefault=1;
  mynewcase->_case=mycase;
  mynewcase->_case_isdefault=1;
  mynewcase->_name=adms_kclone("");
  mynewcase->_name_isdefault=1;
  mynewcase->_caseitem_isdefault=-1;
  return mynewcase;
}
char*adms_case_uid (p_case mynewcase)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,"case");
  return myuid;
}
static p_admst adms_case_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=2)
  {
    adms_message_fatal_continue(("admst:new(case) bad number of inputs (found %i, expected 2)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_module mymodule=(p_module)((p_admst)myinputs[0]->data)->_item.p;
    p_expression mycase=(p_expression)((p_admst)myinputs[1]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_case_new(mymodule,mycase));
    return myadmst;
  }
}

/* ------- caseitem -------------- */
void adms_caseitem_free (p_caseitem mycaseitem)
{
  if(!mycaseitem)
    return;
  adms_slist_free(mycaseitem->_condition);
  free(mycaseitem);
}

void adms_caseitem_valueto_code (p_caseitem mycaseitem,p_adms _code)
{
  mycaseitem->_code=_code;
  mycaseitem->_code_isdefault=0;
}

void adms_caseitem_valueto_defaultcase (p_caseitem mycaseitem,admse _defaultcase)
{
  mycaseitem->_defaultcase=_defaultcase;
  mycaseitem->_defaultcase_isdefault=0;
}

void adms_caseitem_valueto_condition (p_caseitem mycaseitem,p_slist _condition)
{
  mycaseitem->_condition=_condition;
  mycaseitem->_condition_isdefault=0;
}
p_caseitem adms_caseitem_new (p_adms mycode)
{
  p_caseitem mynewcaseitem=(p_caseitem) calloc(1,(size_t)sizeof(t_caseitem));
  mynewcaseitem->_math._adms._datatypename=admse_caseitem;
  mynewcaseitem->_math._value=adms_NAN;
  mynewcaseitem->_math._static=admse_no;
  mynewcaseitem->_math._dynamic=admse_no;
  mynewcaseitem->_math._dependency=admse_constant;
  mynewcaseitem->_code=mycode;
  mynewcaseitem->_code_isdefault=1;
  mynewcaseitem->_defaultcase=admse_no;
  mynewcaseitem->_defaultcase_isdefault=1;
  mynewcaseitem->_condition_isdefault=-1;
  return mynewcaseitem;
}
char*adms_caseitem_uid (p_caseitem mynewcaseitem)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,"caseitem");
  return myuid;
}
static p_admst adms_caseitem_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(caseitem) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_adms mycode=(p_adms)((p_admst)myinputs[0]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_caseitem_new(mycode));
    return myadmst;
  }
}

/* ------- blockvariable -------------- */
void adms_blockvariable_free (p_blockvariable myblockvariable)
{
  if(!myblockvariable)
    return;
  adms_slist_free(myblockvariable->_variable);
  free(myblockvariable);
}

void adms_blockvariable_valueto_block (p_blockvariable myblockvariable,p_block _block)
{
  myblockvariable->_block=_block;
  myblockvariable->_block_isdefault=0;
}

void adms_blockvariable_valueto_variable (p_blockvariable myblockvariable,p_slist _variable)
{
  myblockvariable->_variable=_variable;
  myblockvariable->_variable_isdefault=0;
}
void adms_blockvariable_list_variable_prepend_once_or_abort (p_blockvariable mymyblockvariable,p_variable myvariable)
{
  p_slist list=mymyblockvariable->_variable;
  while(list)
  {
    if(!adms_variable_cmp((p_variable)list->data,myvariable))
    {
      adms_message_fatal(("blockvariable=[%s] variable=[%s] already defined\n",
        adms_blockvariable_uid(mymyblockvariable),adms_variable_uid(myvariable)))
      return;
    }
    list=list->next;
  }
  adms_slist_push(&mymyblockvariable->_variable,(p_adms)myvariable);
}
p_blockvariable adms_blockvariable_new (p_block myblock)
{
  p_blockvariable mynewblockvariable=(p_blockvariable) calloc(1,(size_t)sizeof(t_blockvariable));
  mynewblockvariable->_math._adms._datatypename=admse_blockvariable;
  mynewblockvariable->_math._value=adms_NAN;
  mynewblockvariable->_math._static=admse_no;
  mynewblockvariable->_math._dynamic=admse_no;
  mynewblockvariable->_math._dependency=admse_constant;
  mynewblockvariable->_block=myblock;
  mynewblockvariable->_block_isdefault=1;
  mynewblockvariable->_variable_isdefault=-1;
  return mynewblockvariable;
}
char*adms_blockvariable_uid (p_blockvariable mynewblockvariable)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,"real ");
  {
    p_slist list=mynewblockvariable->_variable;
    while(list)
    {
      adms_strconcat(&myuid,adms_admsxml_uid(list->data));
      if(list->next) adms_k2strconcat(&myuid,",");
      list=list->next;
    }
  }
  adms_k2strconcat(&myuid,";");
  return myuid;
}
static p_admst adms_blockvariable_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(blockvariable) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_block myblock=(p_block)((p_admst)myinputs[0]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_blockvariable_new(myblock));
    return myadmst;
  }
}

/* ------- block -------------- */
void adms_block_free (p_block myblock)
{
  if(!myblock)
    return;
  adms_slist_free(myblock->_item);
  adms_slist_free(myblock->_attribute);
  adms_slist_free(myblock->_variable);
  adms_slist_free(myblock->_probe);
  free(myblock);
}

void adms_block_valueto_module (p_block myblock,p_module _module)
{
  myblock->_module=_module;
  myblock->_module_isdefault=0;
}

void adms_block_valueto_lexval (p_block myblock,p_lexval _lexval)
{
  myblock->_lexval=_lexval;
  myblock->_lexval_isdefault=0;
}

void adms_block_valueto_block (p_block myblock,p_block _block)
{
  myblock->_block=_block;
  myblock->_block_isdefault=0;
}

void adms_block_valueto_item (p_block myblock,p_slist _item)
{
  myblock->_item=_item;
  myblock->_item_isdefault=0;
}

void adms_block_valueto_attribute (p_block myblock,p_slist _attribute)
{
  myblock->_attribute=_attribute;
  myblock->_attribute_isdefault=0;
}

void adms_block_valueto_variable (p_block myblock,p_slist _variable)
{
  myblock->_variable=_variable;
  myblock->_variable_isdefault=0;
}
void adms_block_list_variable_prepend_once_or_abort (p_block mymyblock,p_variableprototype myvariable)
{
  p_slist list=mymyblock->_variable;
  while(list)
  {
    if(!adms_variableprototype_cmp((p_variableprototype)list->data,myvariable))
    {
      adms_message_fatal(("block=[%s] variable=[%s] already defined\n",
        adms_block_uid(mymyblock),adms_variableprototype_uid(myvariable)))
      return;
    }
    list=list->next;
  }
  adms_slist_push(&mymyblock->_variable,(p_adms)myvariable);
}

void adms_block_valueto_probe (p_block myblock,p_slist _probe)
{
  myblock->_probe=_probe;
  myblock->_probe_isdefault=0;
}
p_block adms_block_new (p_module mymodule,p_lexval mylexval,p_block myblock,p_slist myitem)
{
  p_block mynewblock=(p_block) calloc(1,(size_t)sizeof(t_block));
  mynewblock->_math._adms._datatypename=admse_block;
  mynewblock->_math._value=adms_NAN;
  mynewblock->_math._static=admse_no;
  mynewblock->_math._dynamic=admse_no;
  mynewblock->_math._dependency=admse_constant;
  mynewblock->_module=mymodule;
  mynewblock->_module_isdefault=1;
  mynewblock->_lexval=mylexval;
  mynewblock->_lexval_isdefault=1;
  mynewblock->_block=myblock;
  mynewblock->_block_isdefault=1;
  mynewblock->_item=myitem;
  mynewblock->_item_isdefault=1;
  mynewblock->_attribute_isdefault=-1;
  mynewblock->_variable_isdefault=-1;
  mynewblock->_probe_isdefault=-1;
  return mynewblock;
}
char*adms_block_uid (p_block mynewblock)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,"begin :");
  adms_strconcat(&myuid,adms_lexval_uid(mynewblock->_lexval));
  adms_k2strconcat(&myuid,"\n");
  {
    p_slist list=mynewblock->_item;
    while(list)
    {
      adms_strconcat(&myuid,adms_admsxml_uid(list->data));
      if(list->next) adms_k2strconcat(&myuid,"\n");
      list=list->next;
    }
  }
  adms_k2strconcat(&myuid,"end\n");
  return myuid;
}
static p_admst adms_block_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=4)
  {
    adms_message_fatal_continue(("admst:new(block) bad number of inputs (found %i, expected 4)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_module mymodule=(p_module)((p_admst)myinputs[0]->data)->_item.p;
    char* mylexvals=aprintf(mytransform,(p_admst)myinputs[1]->data);
    p_lexval mylexval=adms_lexval_new(mylexvals,adms_transform_uid(mytransform),0,0);
    p_block myblock=(p_block)((p_admst)myinputs[2]->data)->_item.p;
    p_slist myitem=myinputs[3];
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_block_new(mymodule,mylexval,myblock,myitem));
    free(mylexvals);
    return myadmst;
  }
}

/* ------- callfunction -------------- */
void adms_callfunction_free (p_callfunction mycallfunction)
{
  if(!mycallfunction)
    return;
  free(mycallfunction);
}

void adms_callfunction_valueto_module (p_callfunction mycallfunction,p_module _module)
{
  mycallfunction->_module=_module;
  mycallfunction->_module_isdefault=0;
}

void adms_callfunction_valueto_function (p_callfunction mycallfunction,p_function _function)
{
  mycallfunction->_function=_function;
  mycallfunction->_function_isdefault=0;
}
p_callfunction adms_callfunction_new (p_module mymodule,p_function myfunction)
{
  p_callfunction mynewcallfunction=(p_callfunction) calloc(1,(size_t)sizeof(t_callfunction));
  mynewcallfunction->_math._adms._datatypename=admse_callfunction;
  mynewcallfunction->_math._value=adms_NAN;
  mynewcallfunction->_math._static=admse_no;
  mynewcallfunction->_math._dynamic=admse_no;
  mynewcallfunction->_math._dependency=admse_constant;
  mynewcallfunction->_module=mymodule;
  mynewcallfunction->_module_isdefault=1;
  mynewcallfunction->_function=myfunction;
  mynewcallfunction->_function_isdefault=1;
  return mynewcallfunction;
}
char*adms_callfunction_uid (p_callfunction mynewcallfunction)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_function_uid(mynewcallfunction->_function));
  return myuid;
}
static p_admst adms_callfunction_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=2)
  {
    adms_message_fatal_continue(("admst:new(callfunction) bad number of inputs (found %i, expected 2)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    p_module mymodule=(p_module)((p_admst)myinputs[0]->data)->_item.p;
    p_function myfunction=(p_function)((p_admst)myinputs[1]->data)->_item.p;
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_callfunction_new(mymodule,myfunction));
    return myadmst;
  }
}

/* ------- evaluation -------------- */
void adms_evaluation_free (p_evaluation myevaluation)
{
  if(!myevaluation)
    return;
  adms_slist_free(myevaluation->_variable);
  free(myevaluation);
}

void adms_evaluation_valueto_variable (p_evaluation myevaluation,p_slist _variable)
{
  myevaluation->_variable=_variable;
  myevaluation->_variable_isdefault=0;
}
p_evaluation adms_evaluation_new (void)
{
  p_evaluation mynewevaluation=(p_evaluation) calloc(1,(size_t)sizeof(t_evaluation));
  mynewevaluation->_adms._datatypename=admse_evaluation;
  mynewevaluation->_variable_isdefault=-1;
  return mynewevaluation;
}
char*adms_evaluation_uid (p_evaluation mynewevaluation)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,"");
  return myuid;
}
static p_admst adms_evaluation_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=0)
  {
    adms_message_fatal_continue(("admst:new(evaluation) bad number of inputs (found %i, expected 0)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_evaluation_new());
    return myadmst;
  }
}

/* ------- text -------------- */
void adms_text_free (p_text mytext)
{
  if(!mytext)
    return;
  free(mytext->_value);
  free(mytext->_aname);
  adms_slist_free(mytext->_token);
  free(mytext);
}

void adms_text_valueto_transform (p_text mytext,p_transform _transform)
{
  mytext->_transform=_transform;
}

void adms_text_valueto_value (p_text mytext,const char* _value)
{
  mytext->_value=adms_kclone(_value);
}

void adms_text_valueto_aname (p_text mytext,const char* _aname)
{
  mytext->_aname=adms_kclone(_aname);
}

void adms_text_valueto_admse (p_text mytext,int _admse)
{
  mytext->_admse=_admse;
}

void adms_text_valueto_token (p_text mytext,p_slist _token)
{
  mytext->_token=_token;
}
p_text adms_text_new (p_transform mytransform,const char* myvalue)
{
  p_text mynewtext=(p_text) calloc(1,(size_t)sizeof(t_text));
  mynewtext->_adms._datatypename=admse_text;
  mynewtext->_transform=mytransform;
  mynewtext->_value=adms_kclone(myvalue);
  mynewtext->_admse=0;
  return mynewtext;
}
char*adms_text_uid (p_text mynewtext)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_transform_uid(mynewtext->_transform));
  adms_k2strconcat(&myuid,":");
  adms_k2strconcat(&myuid,mynewtext->_value);
  return myuid;
}

/* ------- path -------------- */
void adms_path_free (p_path mypath)
{
  if(!mypath)
    return;
  free(mypath->_aname);
  free(mypath->_avalue);
  free(mypath->_callbackname);
  adms_slist_free(mypath->_bar);
  adms_slist_free(mypath->_arg);
  adms_slist_free(mypath->_u);
  adms_slist_free(mypath->_b);
  adms_slist_free(mypath->_c);
  free(mypath);
}

void adms_path_valueto_transform (p_path mypath,p_transform _transform)
{
  mypath->_transform=_transform;
}

void adms_path_valueto_text (p_path mypath,p_text _text)
{
  mypath->_text=_text;
}

void adms_path_valueto_aname (p_path mypath,const char* _aname)
{
  mypath->_aname=adms_kclone(_aname);
}

void adms_path_valueto_avalue (p_path mypath,const char* _avalue)
{
  mypath->_avalue=adms_kclone(_avalue);
}

void adms_path_valueto_template (p_path mypath,p_transform _template)
{
  mypath->_template=_template;
}

void adms_path_valueto_keeplist (p_path mypath,int _keeplist)
{
  mypath->_keeplist=_keeplist;
}

void adms_path_valueto_callbackname (p_path mypath,const char* _callbackname)
{
  mypath->_callbackname=adms_kclone(_callbackname);
}

void adms_path_valueto_callback (p_path mypath,void* _callback)
{
  mypath->_callback=_callback;
}

void adms_path_valueto_bar (p_path mypath,p_slist _bar)
{
  mypath->_bar=_bar;
}

void adms_path_valueto_arg (p_path mypath,p_slist _arg)
{
  mypath->_arg=_arg;
}

void adms_path_valueto_u (p_path mypath,p_slist _u)
{
  mypath->_u=_u;
}

void adms_path_valueto_b (p_path mypath,p_slist _b)
{
  mypath->_b=_b;
}

void adms_path_valueto_c (p_path mypath,p_slist _c)
{
  mypath->_c=_c;
}
p_path adms_path_new (p_transform mytransform,p_text mytext)
{
  p_path mynewpath=(p_path) calloc(1,(size_t)sizeof(t_path));
  mynewpath->_adms._datatypename=admse_path;
  mynewpath->_transform=mytransform;
  mynewpath->_text=mytext;
  mynewpath->_keeplist=0;
  mynewpath->_bar=NULL;
  mynewpath->_arg=NULL;
  mynewpath->_u=NULL;
  mynewpath->_b=NULL;
  mynewpath->_c=NULL;
  return mynewpath;
}
char*adms_path_uid (p_path mynewpath)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_transform_uid(mynewpath->_transform));
  adms_k2strconcat(&myuid,":");
  adms_k2strconcat(&myuid,mynewpath->_aname);
  adms_k2strconcat(&myuid,".");
  adms_k2strconcat(&myuid,mynewpath->_avalue);
  return myuid;
}

/* ------- ptraverse -------------- */
void adms_ptraverse_free (p_ptraverse myptraverse)
{
  if(!myptraverse)
    return;
  adms_slist_free(myptraverse->_admst);
  adms_slist_free(myptraverse->_alladmst);
  adms_slist_free(myptraverse->_aliasadmst);
  free(myptraverse);
}

void adms_ptraverse_valueto_transform (p_ptraverse myptraverse,p_transform _transform)
{
  myptraverse->_transform=_transform;
}

void adms_ptraverse_valueto_admst (p_ptraverse myptraverse,p_slist _admst)
{
  myptraverse->_admst=_admst;
}

void adms_ptraverse_valueto_alladmst (p_ptraverse myptraverse,p_slist _alladmst)
{
  myptraverse->_alladmst=_alladmst;
}

void adms_ptraverse_valueto_aliasadmst (p_ptraverse myptraverse,p_slist _aliasadmst)
{
  myptraverse->_aliasadmst=_aliasadmst;
}

void adms_ptraverse_valueto_position (p_ptraverse myptraverse,int _position)
{
  myptraverse->_position=_position;
}

void adms_ptraverse_valueto_a1 (p_ptraverse myptraverse,p_admst _a1)
{
  myptraverse->_a1=_a1;
}

void adms_ptraverse_valueto_an (p_ptraverse myptraverse,p_admst _an)
{
  myptraverse->_an=_an;
}
p_ptraverse adms_ptraverse_new (p_transform mytransform)
{
  p_ptraverse mynewptraverse=(p_ptraverse) calloc(1,(size_t)sizeof(t_ptraverse));
  mynewptraverse->_adms._datatypename=admse_ptraverse;
  mynewptraverse->_transform=mytransform;
  return mynewptraverse;
}
char*adms_ptraverse_uid (p_ptraverse mynewptraverse)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_transform_uid(mynewptraverse->_transform));
  return myuid;
}

/* ------- pparse -------------- */
void adms_pparse_free (p_pparse mypparse)
{
  if(!mypparse)
    return;
  free(mypparse->_aname);
  free(mypparse->_value);
  free(mypparse);
}

void adms_pparse_valueto_transform (p_pparse mypparse,p_transform _transform)
{
  mypparse->_transform=_transform;
}

void adms_pparse_valueto_aname (p_pparse mypparse,const char* _aname)
{
  mypparse->_aname=adms_kclone(_aname);
}

void adms_pparse_valueto_value (p_pparse mypparse,const char* _value)
{
  mypparse->_value=adms_kclone(_value);
}

void adms_pparse_valueto_path (p_pparse mypparse,p_path _path)
{
  mypparse->_path=_path;
}

void adms_pparse_valueto_text (p_pparse mypparse,p_text _text)
{
  mypparse->_text=_text;
}

void adms_pparse_valueto_position (p_pparse mypparse,const char* _position)
{
  mypparse->_position=_position;
}

void adms_pparse_valueto_tkstart (p_pparse mypparse,const char* _tkstart)
{
  mypparse->_tkstart=_tkstart;
}

void adms_pparse_valueto_tklen (p_pparse mypparse,int _tklen)
{
  mypparse->_tklen=_tklen;
}

void adms_pparse_valueto_tkid (p_pparse mypparse,admse _tkid)
{
  mypparse->_tkid=_tkid;
}
p_pparse adms_pparse_new (p_transform mytransform,const char* myaname,const char* myvalue)
{
  p_pparse mynewpparse=(p_pparse) calloc(1,(size_t)sizeof(t_pparse));
  mynewpparse->_adms._datatypename=admse_pparse;
  mynewpparse->_transform=mytransform;
  mynewpparse->_aname=adms_kclone(myaname);
  mynewpparse->_value=adms_kclone(myvalue);
  return mynewpparse;
}
char*adms_pparse_uid (p_pparse mynewpparse)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_transform_uid(mynewpparse->_transform));
  adms_k2strconcat(&myuid,":");
  adms_k2strconcat(&myuid,mynewpparse->_aname);
  return myuid;
}

/* ------- admst -------------- */
void adms_admst_free (p_admst myadmst)
{
  if(!myadmst)
    return;
  adms_global_increment_nbadmstdestroy();
  free(myadmst);
}

void adms_admst_valueto_previous (p_admst myadmst,p_admst _previous)
{
  myadmst->_previous=_previous;
}

void adms_admst_valueto_previouspath (p_admst myadmst,p_admst _previouspath)
{
  myadmst->_previouspath=_previouspath;
}

void adms_admst_valueto_pseudo (p_admst myadmst,admse _pseudo)
{
  myadmst->_pseudo=_pseudo;
}

void adms_admst_valueto_item (p_admst myadmst,p_adms _item)
{
  /*fixme: myadmst->_item=_item;*/
}

void adms_admst_valueto_valueto (p_admst myadmst,void* _valueto)
{
  myadmst->_valueto=_valueto;
}

void adms_admst_valueto_position (p_admst myadmst,int _position)
{
  myadmst->_position=_position;
}

void adms_admst_valueto_count (p_admst myadmst,int _count)
{
  myadmst->_count=_count;
}

void adms_admst_valueto_hasnewbasicstring (p_admst myadmst,int _hasnewbasicstring)
{
  myadmst->_hasnewbasicstring=_hasnewbasicstring;
}

void adms_admst_valueto_original (p_admst myadmst,p_admst _original)
{
  myadmst->_original=_original;
}

void adms_admst_valueto_refd (p_admst myadmst,int _refd)
{
  myadmst->_refd=_refd;
}

void adms_admst_valueto_true (p_admst myadmst,int _true)
{
  myadmst->_true=_true;
}

void adms_admst_valueto_preva (p_admst myadmst,p_admst _preva)
{
  myadmst->_preva=_preva;
}

void adms_admst_valueto_nexta (p_admst myadmst,p_admst _nexta)
{
  myadmst->_nexta=_nexta;
}

void adms_admst_valueto_selected (p_admst myadmst,admse _selected)
{
  myadmst->_selected=_selected;
}
p_admst adms_admst_new (p_admst myprevious,p_admst mypreviouspath,admse mypseudo)
{
  p_admst mynewadmst=(p_admst) calloc(1,(size_t)sizeof(t_admst));
  adms_global_increment_nbadmstnew();
  mynewadmst->_adms._datatypename=admse_admst;
  mynewadmst->_previous=myprevious;
  mynewadmst->_previouspath=mypreviouspath;
  mynewadmst->_pseudo=mypseudo;
  mynewadmst->_hasnewbasicstring=0;
  mynewadmst->_refd=1;
  mynewadmst->_true=1;
  mynewadmst->_selected=admse_yes;
  return mynewadmst;
}
char*adms_admst_uid (p_admst mynewadmst)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,"admst");
  return myuid;
}

/* ------- transform -------------- */
void adms_transform_free (p_transform mytransform)
{
  if(!mytransform)
    return;
  free(mytransform->_fullfilename);
  free(mytransform->_basefilename);
  free(mytransform->_name);
  free(mytransform->_id);
  adms_slist_free(mytransform->_templates);
  adms_slist_free(mytransform->_textarguments);
  adms_slist_free(mytransform->_pathinputs);
  adms_slist_free(mytransform->_attribute);
  adms_slist_free(mytransform->_children);
  free(mytransform);
}

void adms_transform_valueto_fullfilename (p_transform mytransform,const char* _fullfilename)
{
  mytransform->_fullfilename=adms_kclone(_fullfilename);
}

void adms_transform_valueto_basefilename (p_transform mytransform,const char* _basefilename)
{
  mytransform->_basefilename=adms_kclone(_basefilename);
}

void adms_transform_valueto_l (p_transform mytransform,int _l)
{
  mytransform->_l=_l;
}

void adms_transform_valueto_name (p_transform mytransform,const char* _name)
{
  mytransform->_name=adms_kclone(_name);
}

void adms_transform_valueto_parent (p_transform mytransform,p_transform _parent)
{
  mytransform->_parent=_parent;
}

void adms_transform_valueto_callback (p_transform mytransform,void* _callback)
{
  mytransform->_callback=_callback;
}

void adms_transform_valueto_id (p_transform mytransform,const char* _id)
{
  mytransform->_id=adms_kclone(_id);
}

void adms_transform_valueto_pathselect (p_transform mytransform,p_path _pathselect)
{
  mytransform->_pathselect=_pathselect;
}

void adms_transform_valueto_pathpath (p_transform mytransform,p_path _pathpath)
{
  mytransform->_pathpath=_pathpath;
}

void adms_transform_valueto_pathinto (p_transform mytransform,p_path _pathinto)
{
  mytransform->_pathinto=_pathinto;
}

void adms_transform_valueto_pathtest (p_transform mytransform,p_path _pathtest)
{
  mytransform->_pathtest=_pathtest;
}

void adms_transform_valueto_pathlist (p_transform mytransform,p_path _pathlist)
{
  mytransform->_pathlist=_pathlist;
}

void adms_transform_valueto_textselect (p_transform mytransform,p_text _textselect)
{
  mytransform->_textselect=_textselect;
}

void adms_transform_valueto_textdatatype (p_transform mytransform,p_text _textdatatype)
{
  mytransform->_textdatatype=_textdatatype;
}

void adms_transform_valueto_textmatch (p_transform mytransform,p_text _textmatch)
{
  mytransform->_textmatch=_textmatch;
}

void adms_transform_valueto_textrequired (p_transform mytransform,p_text _textrequired)
{
  mytransform->_textrequired=_textrequired;
}

void adms_transform_valueto_textseparator (p_transform mytransform,p_text _textseparator)
{
  mytransform->_textseparator=_textseparator;
}

void adms_transform_valueto_textfrom (p_transform mytransform,p_text _textfrom)
{
  mytransform->_textfrom=_textfrom;
}

void adms_transform_valueto_textto (p_transform mytransform,p_text _textto)
{
  mytransform->_textto=_textto;
}

void adms_transform_valueto_textlist (p_transform mytransform,p_text _textlist)
{
  mytransform->_textlist=_textlist;
}

void adms_transform_valueto_textname (p_transform mytransform,p_text _textname)
{
  mytransform->_textname=_textname;
}

void adms_transform_valueto_textformat (p_transform mytransform,p_text _textformat)
{
  mytransform->_textformat=_textformat;
}

void adms_transform_valueto_textfile (p_transform mytransform,p_text _textfile)
{
  mytransform->_textfile=_textfile;
}

void adms_transform_valueto_textversion (p_transform mytransform,p_text _textversion)
{
  mytransform->_textversion=_textversion;
}

void adms_transform_valueto_textonduplicate (p_transform mytransform,p_text _textonduplicate)
{
  mytransform->_textonduplicate=_textonduplicate;
}

void adms_transform_valueto_textprobe (p_transform mytransform,p_text _textprobe)
{
  mytransform->_textprobe=_textprobe;
}

void adms_transform_valueto_textstring (p_transform mytransform,p_text _textstring)
{
  mytransform->_textstring=_textstring;
}

void adms_transform_valueto_templates (p_transform mytransform,p_slist _templates)
{
  mytransform->_templates=_templates;
}

void adms_transform_valueto_textarguments (p_transform mytransform,p_slist _textarguments)
{
  mytransform->_textarguments=_textarguments;
}

void adms_transform_valueto_pathoncompare (p_transform mytransform,p_path _pathoncompare)
{
  mytransform->_pathoncompare=_pathoncompare;
}

void adms_transform_valueto_pathinputs (p_transform mytransform,p_slist _pathinputs)
{
  mytransform->_pathinputs=_pathinputs;
}

void adms_transform_valueto_attribute (p_transform mytransform,p_slist _attribute)
{
  mytransform->_attribute=_attribute;
}

void adms_transform_valueto_children (p_transform mytransform,p_slist _children)
{
  mytransform->_children=_children;
}
p_transform adms_transform_new (const char* myfullfilename,const char* mybasefilename,int myl,const char* myname,p_transform myparent)
{
  p_transform mynewtransform=(p_transform) calloc(1,(size_t)sizeof(t_transform));
  mynewtransform->_adms._datatypename=admse_transform;
  mynewtransform->_fullfilename=adms_kclone(myfullfilename);
  mynewtransform->_basefilename=adms_kclone(mybasefilename);
  mynewtransform->_l=myl;
  mynewtransform->_name=adms_kclone(myname);
  mynewtransform->_parent=myparent;
  return mynewtransform;
}
char*adms_transform_uid (p_transform mynewtransform)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,"[");
  adms_k2strconcat(&myuid,mynewtransform->_fullfilename);
  adms_k2strconcat(&myuid,":");
  adms_strconcat(&myuid,adms_integertostring(mynewtransform->_l));
  adms_k2strconcat(&myuid,"]");
  return myuid;
}

/* ------- itransform -------------- */
void adms_itransform_free (p_itransform myitransform)
{
  if(!myitransform)
    return;
  adms_slist_free(myitransform->_variable);
  adms_slist_free(myitransform->_return);
  free(myitransform);
}

void adms_itransform_valueto_template (p_itransform myitransform,p_transform _template)
{
  myitransform->_template=_template;
}

void adms_itransform_valueto_originalcall (p_itransform myitransform,p_transform _originalcall)
{
  myitransform->_originalcall=_originalcall;
}

void adms_itransform_valueto_variable (p_itransform myitransform,p_slist _variable)
{
  myitransform->_variable=_variable;
}

void adms_itransform_valueto_return (p_itransform myitransform,p_slist _return)
{
  myitransform->_return=_return;
}
p_itransform adms_itransform_new (p_transform mytemplate)
{
  p_itransform mynewitransform=(p_itransform) calloc(1,(size_t)sizeof(t_itransform));
  mynewitransform->_adms._datatypename=admse_itransform;
  mynewitransform->_template=mytemplate;
  return mynewitransform;
}
char*adms_itransform_uid (p_itransform mynewitransform)
{
  char*myuid=NULL;
  adms_strconcat(&myuid,adms_transform_uid(mynewitransform->_template));
  return myuid;
}

/* ------- return -------------- */
void adms_return_free (p_return myreturn)
{
  if(!myreturn)
    return;
  free(myreturn->_name);
  free(myreturn->_value);
  free(myreturn);
}

void adms_return_valueto_name (p_return myreturn,const char* _name)
{
  myreturn->_name=adms_kclone(_name);
}

void adms_return_valueto_value (p_return myreturn,const char* _value)
{
  myreturn->_value=adms_kclone(_value);
}
p_return adms_return_new (const char* myname,const char* myvalue)
{
  p_return mynewreturn=(p_return) calloc(1,(size_t)sizeof(t_return));
  mynewreturn->_adms._datatypename=admse_return;
  mynewreturn->_name=adms_kclone(myname);
  mynewreturn->_value=adms_kclone(myvalue);
  return mynewreturn;
}
char*adms_return_uid (p_return mynewreturn)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,mynewreturn->_name);
  return myuid;
}

/* ------- attribute -------------- */
void adms_attribute_free (p_attribute myattribute)
{
  if(!myattribute)
    return;
  free(myattribute->_name);
  free(myattribute);
}

void adms_attribute_valueto_name (p_attribute myattribute,const char* _name)
{
  myattribute->_name=adms_kclone(_name);
  myattribute->_name_isdefault=0;
}

void adms_attribute_valueto_value (p_attribute myattribute,p_adms _value)
{
  myattribute->_value=_value;
  myattribute->_value_isdefault=0;
}
p_attribute adms_attribute_new (const char* myname)
{
  p_attribute mynewattribute=(p_attribute) calloc(1,(size_t)sizeof(t_attribute));
  mynewattribute->_adms._datatypename=admse_attribute;
  mynewattribute->_name=adms_kclone(myname);
  mynewattribute->_name_isdefault=1;
  mynewattribute->_value_isdefault=-1;
  return mynewattribute;
}
char*adms_attribute_uid (p_attribute mynewattribute)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,mynewattribute->_name);
  return myuid;
}
static p_admst adms_attribute_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(attribute) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char* myname=aprintf(mytransform,(p_admst)myinputs[0]->data);
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_attribute_new(myname));
    free(myname);
    return myadmst;
  }
}

/* ------- admstvariable -------------- */
void adms_admstvariable_free (p_admstvariable myadmstvariable)
{
  if(!myadmstvariable)
    return;
  free(myadmstvariable->_name);
  adms_slist_free(myadmstvariable->_value);
  free(myadmstvariable);
}

void adms_admstvariable_valueto_name (p_admstvariable myadmstvariable,const char* _name)
{
  myadmstvariable->_name=adms_kclone(_name);
}

void adms_admstvariable_valueto_value (p_admstvariable myadmstvariable,p_slist _value)
{
  myadmstvariable->_value=_value;
}
p_admstvariable adms_admstvariable_new (const char* myname)
{
  p_admstvariable mynewadmstvariable=(p_admstvariable) calloc(1,(size_t)sizeof(t_admstvariable));
  mynewadmstvariable->_adms._datatypename=admse_admstvariable;
  mynewadmstvariable->_name=adms_kclone(myname);
  return mynewadmstvariable;
}
char*adms_admstvariable_uid (p_admstvariable mynewadmstvariable)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,mynewadmstvariable->_name);
  return myuid;
}

/* ------- lexval -------------- */
void adms_lexval_free (p_lexval mylexval)
{
  if(!mylexval)
    return;
  free(mylexval->_string);
  free(mylexval->_f);
  free(mylexval);
}

void adms_lexval_valueto_string (p_lexval mylexval,const char* _string)
{
  mylexval->_string=adms_kclone(_string);
  mylexval->_string_isdefault=0;
}

void adms_lexval_valueto_f (p_lexval mylexval,const char* _f)
{
  mylexval->_f=adms_kclone(_f);
  mylexval->_f_isdefault=0;
}

void adms_lexval_valueto_l (p_lexval mylexval,int _l)
{
  mylexval->_l=_l;
  mylexval->_l_isdefault=0;
}

void adms_lexval_valueto_c (p_lexval mylexval,int _c)
{
  mylexval->_c=_c;
  mylexval->_c_isdefault=0;
}
p_lexval adms_lexval_new (const char* mystring,const char* myf,int myl,int myc)
{
  p_lexval mynewlexval=(p_lexval) calloc(1,(size_t)sizeof(t_lexval));
  mynewlexval->_adms._datatypename=admse_lexval;
  mynewlexval->_string=adms_kclone(mystring);
  mynewlexval->_string_isdefault=1;
  mynewlexval->_f=adms_kclone(myf);
  mynewlexval->_f_isdefault=1;
  mynewlexval->_l=myl;
  mynewlexval->_l_isdefault=1;
  mynewlexval->_c=myc;
  mynewlexval->_c_isdefault=1;
  return mynewlexval;
}
char*adms_lexval_uid (p_lexval mynewlexval)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,mynewlexval->_string);
  return myuid;
}
static p_admst adms_lexval_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=4)
  {
    adms_message_fatal_continue(("admst:new(lexval) bad number of inputs (found %i, expected 4)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char* mystring=aprintf(mytransform,(p_admst)myinputs[0]->data);
    char* myf=aprintf(mytransform,(p_admst)myinputs[1]->data);
    char* myls=aprintf(mytransform,(p_admst)myinputs[2]->data);
    int myl=adms_strtol(mytransform,myls);
    char* mycs=aprintf(mytransform,(p_admst)myinputs[3]->data);
    int myc=adms_strtol(mytransform,mycs);
    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_lexval_new(mystring,myf,myl,myc));
    free(mystring);
    free(myf);
    free(myls);
    free(mycs);
    return myadmst;
  }
}

/* ------- yaccval -------------- */
void adms_yaccval_free (p_yaccval myyaccval)
{
  if(!myyaccval)
    return;
  free(myyaccval->_filename);
  free(myyaccval);
}

void adms_yaccval_valueto_filename (p_yaccval myyaccval,const char* _filename)
{
  myyaccval->_filename=adms_kclone(_filename);
}

void adms_yaccval_valueto_usrdata (p_yaccval myyaccval,p_adms _usrdata)
{
  myyaccval->_usrdata=_usrdata;
}
p_yaccval adms_yaccval_new (const char* myfilename)
{
  p_yaccval mynewyaccval=(p_yaccval) calloc(1,(size_t)sizeof(t_yaccval));
  mynewyaccval->_adms._datatypename=admse_yaccval;
  mynewyaccval->_filename=adms_kclone(myfilename);
  return mynewyaccval;
}
char*adms_yaccval_uid (p_yaccval mynewyaccval)
{
  char*myuid=NULL;
  adms_k2strconcat(&myuid,mynewyaccval->_filename);
  return myuid;
}

p_new adms_admsxml_new (p_ktransform mytransform,const char* ename)
{
  if(0) {}
  else if(!strcmp(ename,"simulator")) return (p_new) adms_simulator_xnew;
  else if(!strcmp(ename,"admsmain")) return (p_new) adms_admsmain_xnew;
  else if(!strcmp(ename,"list")) return (p_new) adms_list_xnew;
  else if(!strcmp(ename,"nature")) return (p_new) adms_nature_xnew;
  else if(!strcmp(ename,"discipline")) return (p_new) adms_discipline_xnew;
  else if(!strcmp(ename,"module")) return (p_new) adms_module_xnew;
  else if(!strcmp(ename,"analogfunction")) return (p_new) adms_analogfunction_xnew;
  else if(!strcmp(ename,"nodealias")) return (p_new) adms_nodealias_xnew;
  else if(!strcmp(ename,"node")) return (p_new) adms_node_xnew;
  else if(!strcmp(ename,"branchalias")) return (p_new) adms_branchalias_xnew;
  else if(!strcmp(ename,"branch")) return (p_new) adms_branch_xnew;
  else if(!strcmp(ename,"variableprototype")) return (p_new) adms_variableprototype_xnew;
  else if(!strcmp(ename,"source")) return (p_new) adms_source_xnew;
  else if(!strcmp(ename,"range")) return (p_new) adms_range_xnew;
  else if(!strcmp(ename,"jacobian")) return (p_new) adms_jacobian_xnew;
  else if(!strcmp(ename,"analog")) return (p_new) adms_analog_xnew;
  else if(!strcmp(ename,"math")) return (p_new) adms_math_xnew;
  else if(!strcmp(ename,"string")) return (p_new) adms_string_xnew;
  else if(!strcmp(ename,"subexpression")) return (p_new) adms_subexpression_xnew;
  else if(!strcmp(ename,"mapply_unary")) return (p_new) adms_mapply_unary_xnew;
  else if(!strcmp(ename,"mapply_binary")) return (p_new) adms_mapply_binary_xnew;
  else if(!strcmp(ename,"mapply_ternary")) return (p_new) adms_mapply_ternary_xnew;
  else if(!strcmp(ename,"number")) return (p_new) adms_number_xnew;
  else if(!strcmp(ename,"function")) return (p_new) adms_function_xnew;
  else if(!strcmp(ename,"variable")) return (p_new) adms_variable_xnew;
  else if(!strcmp(ename,"array")) return (p_new) adms_array_xnew;
  else if(!strcmp(ename,"probe")) return (p_new) adms_probe_xnew;
  else if(!strcmp(ename,"expression")) return (p_new) adms_expression_xnew;
  else if(!strcmp(ename,"instance")) return (p_new) adms_instance_xnew;
  else if(!strcmp(ename,"instanceparameter")) return (p_new) adms_instanceparameter_xnew;
  else if(!strcmp(ename,"instancenode")) return (p_new) adms_instancenode_xnew;
  else if(!strcmp(ename,"nilled")) return (p_new) adms_nilled_xnew;
  else if(!strcmp(ename,"assignment")) return (p_new) adms_assignment_xnew;
  else if(!strcmp(ename,"contribution")) return (p_new) adms_contribution_xnew;
  else if(!strcmp(ename,"conditional")) return (p_new) adms_conditional_xnew;
  else if(!strcmp(ename,"forloop")) return (p_new) adms_forloop_xnew;
  else if(!strcmp(ename,"whileloop")) return (p_new) adms_whileloop_xnew;
  else if(!strcmp(ename,"case")) return (p_new) adms_case_xnew;
  else if(!strcmp(ename,"caseitem")) return (p_new) adms_caseitem_xnew;
  else if(!strcmp(ename,"blockvariable")) return (p_new) adms_blockvariable_xnew;
  else if(!strcmp(ename,"block")) return (p_new) adms_block_xnew;
  else if(!strcmp(ename,"callfunction")) return (p_new) adms_callfunction_xnew;
  else if(!strcmp(ename,"evaluation")) return (p_new) adms_evaluation_xnew;
  else if(!strcmp(ename,"attribute")) return (p_new) adms_attribute_xnew;
  else if(!strcmp(ename,"lexval")) return (p_new) adms_lexval_xnew;
  else if(!strcmp(ename,"basicenumeration")) return (p_new) adms_basicenumeration_xnew;
  else if(!strcmp(ename,"basicinteger")) return (p_new) adms_basicinteger_xnew;
  else if(!strcmp(ename,"basicreal")) return (p_new) adms_basicreal_xnew;
  else if(!strcmp(ename,"basicstring")) return (p_new) adms_basicstring_xnew;
  adms_message_fatal_continue(("adms element '%s' cannot be created using admst:new\n",ename))
  adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
  return NULL;
}

p_cmp adms_admsxml_cmp (p_kadmst myadmst)
{
  p_admst myadmstread=aread(myadmst);
  if(myadmstread->_pseudo==admse_basicenumeration) return (p_cmp)adms_basicenumeration_cmp;
  if(myadmstread->_pseudo==admse_basicinteger) return (p_cmp)adms_basicinteger_cmp;
  if(myadmstread->_pseudo==admse_basicreal) return (p_cmp)adms_basicreal_cmp;
  if(myadmstread->_pseudo==admse_basicstring) return (p_cmp)adms_basicstring_cmp;
  if(myadmstread->_pseudo==admse_simulator) return (p_cmp)adms_simulator_cmp;
  if(myadmstread->_pseudo==admse_admsmain) return (p_cmp)adms_admsmain_cmp;
  if(myadmstread->_pseudo==admse_list) return (p_cmp)adms_list_cmp;
  if(myadmstread->_pseudo==admse_nature) return (p_cmp)adms_nature_cmp;
  if(myadmstread->_pseudo==admse_discipline) return (p_cmp)adms_discipline_cmp;
  if(myadmstread->_pseudo==admse_module) return (p_cmp)adms_module_cmp;
  if(myadmstread->_pseudo==admse_analogfunction) return (p_cmp)adms_analogfunction_cmp;
  if(myadmstread->_pseudo==admse_nodealias) return (p_cmp)adms_nodealias_cmp;
  if(myadmstread->_pseudo==admse_node) return (p_cmp)adms_node_cmp;
  if(myadmstread->_pseudo==admse_branchalias) return (p_cmp)adms_branchalias_cmp;
  if(myadmstread->_pseudo==admse_branch) return (p_cmp)adms_branch_cmp;
  if(myadmstread->_pseudo==admse_variableprototype) return (p_cmp)adms_variableprototype_cmp;
  if(myadmstread->_pseudo==admse_source) return (p_cmp)adms_source_cmp;
  if(myadmstread->_pseudo==admse_range) return (p_cmp)adms_range_cmp;
  if(myadmstread->_pseudo==admse_jacobian) return (p_cmp)adms_jacobian_cmp;
  if(myadmstread->_pseudo==admse_analog) return (p_cmp)adms_analog_cmp;
  if(myadmstread->_pseudo==admse_math) return (p_cmp)adms_math_cmp;
  if(myadmstread->_pseudo==admse_string) return (p_cmp)adms_string_cmp;
  if(myadmstread->_pseudo==admse_subexpression) return (p_cmp)adms_subexpression_cmp;
  if(myadmstread->_pseudo==admse_mapply_unary) return (p_cmp)adms_mapply_unary_cmp;
  if(myadmstread->_pseudo==admse_mapply_binary) return (p_cmp)adms_mapply_binary_cmp;
  if(myadmstread->_pseudo==admse_mapply_ternary) return (p_cmp)adms_mapply_ternary_cmp;
  if(myadmstread->_pseudo==admse_number) return (p_cmp)adms_number_cmp;
  if(myadmstread->_pseudo==admse_function) return (p_cmp)adms_function_cmp;
  if(myadmstread->_pseudo==admse_variable) return (p_cmp)adms_variable_cmp;
  if(myadmstread->_pseudo==admse_array) return (p_cmp)adms_array_cmp;
  if(myadmstread->_pseudo==admse_probe) return (p_cmp)adms_probe_cmp;
  if(myadmstread->_pseudo==admse_expression) return (p_cmp)adms_expression_cmp;
  if(myadmstread->_pseudo==admse_instance) return (p_cmp)adms_instance_cmp;
  if(myadmstread->_pseudo==admse_instanceparameter) return (p_cmp)adms_instanceparameter_cmp;
  if(myadmstread->_pseudo==admse_instancenode) return (p_cmp)adms_instancenode_cmp;
  if(myadmstread->_pseudo==admse_nilled) return (p_cmp)adms_nilled_cmp;
  if(myadmstread->_pseudo==admse_assignment) return (p_cmp)adms_assignment_cmp;
  if(myadmstread->_pseudo==admse_contribution) return (p_cmp)adms_contribution_cmp;
  if(myadmstread->_pseudo==admse_conditional) return (p_cmp)adms_conditional_cmp;
  if(myadmstread->_pseudo==admse_forloop) return (p_cmp)adms_forloop_cmp;
  if(myadmstread->_pseudo==admse_whileloop) return (p_cmp)adms_whileloop_cmp;
  if(myadmstread->_pseudo==admse_case) return (p_cmp)adms_case_cmp;
  if(myadmstread->_pseudo==admse_caseitem) return (p_cmp)adms_caseitem_cmp;
  if(myadmstread->_pseudo==admse_blockvariable) return (p_cmp)adms_blockvariable_cmp;
  if(myadmstread->_pseudo==admse_block) return (p_cmp)adms_block_cmp;
  if(myadmstread->_pseudo==admse_callfunction) return (p_cmp)adms_callfunction_cmp;
  if(myadmstread->_pseudo==admse_evaluation) return (p_cmp)adms_evaluation_cmp;
  if(myadmstread->_pseudo==admse_attribute) return (p_cmp)adms_attribute_cmp;
  if(myadmstread->_pseudo==admse_lexval) return (p_cmp)adms_lexval_cmp;
  adms_message_fatal_continue(("%s:%i: internal error - should not be reached\n",__FILE__,__LINE__))
  adms_message_fatal(("%s:%i: please report to r29173@gmail.com\n",__FILE__,__LINE__))
  return NULL;
}
/*-- admst/new prototypes --*/
p_admst adms_admst_newbd (p_admst myprevious,p_admst mypreviouspath,const admse item)
{
  p_admst myadmst=adms_admst_new(myprevious,mypreviouspath,admse_datatypename);
  myadmst->_item.i=item;
  return myadmst;
}
p_admst adms_admst_newbe (p_admst myprevious,p_admst mypreviouspath,const admse item)
{
  p_admst myadmst=adms_admst_new(myprevious,mypreviouspath,admse_basicenumeration);
  myadmst->_item.i=item;
  return myadmst;
}
p_admst adms_admst_newbi (p_admst myprevious,p_admst mypreviouspath,const int item)
{
  p_admst myadmst=adms_admst_new(myprevious,mypreviouspath,admse_basicinteger);
  myadmst->_item.i=item;
  return myadmst;
}
p_admst adms_admst_newbr (p_admst myprevious,p_admst mypreviouspath,const double item)
{
  p_admst myadmst=adms_admst_new(myprevious,mypreviouspath,admse_basicreal);
  myadmst->_item.r=item;
  return myadmst;
}
p_admst adms_admst_newbs (p_admst myprevious,p_admst mypreviouspath,char* item)
{
  p_admst myadmst=adms_admst_new(myprevious,mypreviouspath,admse_basicstring);
  myadmst->_item.s=item;
  if(myprevious->_hasnewbasicstring==2)
  {
    myadmst->_original=myprevious;
    myprevious->_refd++;
  }
  return myadmst;
}
p_admst adms_admst_newks (char* item)
{
  p_admst myadmst=adms_admst_new(NULL,NULL,admse_basicstring);
  myadmst->_item.s=item;
  myadmst->_refd=0;
  return myadmst;
}
p_admst adms_admst_newns (p_admst myprevious,p_admst mypreviouspath,char* item)
{
  p_admst myadmst=adms_admst_new(myprevious,mypreviouspath,admse_basicstring);
  myadmst->_item.s=item;
  myadmst->_hasnewbasicstring=1;
  return myadmst;
}
p_admst adms_admst_newpa (p_admst myprevious,p_admst mypreviouspath,const p_adms item)
{
  p_admst myadmst=adms_admst_new(myprevious,mypreviouspath,item?item->_datatypename:admse_empty);
  myadmst->_item.p=item;
  return myadmst;
}
p_admst adms_admst_newpn (p_admst myprevious,p_admst mypreviouspath,p_admst item)
{
  p_admst myadmst=adms_admst_new(myprevious,mypreviouspath,item->_pseudo);
  if(item->_hasnewbasicstring==1)
  {
    myadmst->_item.s=item->_item.s;
    myadmst->_original=item;
    item->_refd++;
  }
  else
    myadmst->_item.p=item->_item.p;
  return myadmst;
}
p_admst adms_admst_newpc (p_admst myprevious,p_admst mypreviouspath,p_attribute item)
{
  p_admst myadmst=adms_admst_new(myprevious,mypreviouspath,admse__croix);
  myadmst->_item.p=(p_adms)item;
  return myadmst;
}
p_admst adms_admst_newpd (p_admst myprevious,p_admst mypreviouspath,p_admstvariable item)
{
  p_admst myadmst=adms_admst_new(myprevious,mypreviouspath,admse_basiclist);
  myadmst->_item.p=(p_adms)item;
  return myadmst;
}
p_admst adms_admst_newla (p_admst myprevious,p_admst mypreviouspath,p_slist item)
{
  p_admst myadmst=adms_admst_new(myprevious,mypreviouspath,admse__ladms);
  myadmst->_item.p=(p_adms)item;
  return myadmst;
}
p_admst adms_admst_newln (p_admst myprevious,p_admst mypreviouspath,p_slist item)
{
  p_admst myadmst=adms_admst_new(myprevious,mypreviouspath,admse__ladmst);
  myadmst->_item.p=(p_adms)item;
  return myadmst;
}
p_admst adms_admst_newpp (p_admst myprevious,p_admst mypreviouspath,admse item)
{
  p_admst myadmst=adms_admst_new(myprevious,mypreviouspath,item);
  myadmst->_item.p=myprevious->_item.p;
  return myadmst;
}

/*-- Miscellaneous routines --*/
p_admst aread (p_admst myadmst)
{
  p_admst myadmstread=(myadmst&&myadmst->_pseudo==admse__croix)?(p_admst)((p_attribute)myadmst->_item.p)->_value:myadmst;
  return myadmstread;
}
int adms_strtol (p_ktransform mytransform,const char* mynumber)
{
  int nb;
  char* tail;
  errno=0;
  nb=strtol(mynumber,&tail,10);
  if(*tail||errno)
  {
    adms_message_fatal_continue(("%s: cannot convert value to valid integer value\n",mynumber))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
  }
  return nb;
}
double adms_strtod (p_ktransform mytransform,const char* myreal)
{
  double nb;
  char* tail;
  errno=0;
  nb=strtod(myreal,&tail);
  if(*tail||errno)
    return adms_NAN;
  if(*tail||errno)
  {
    adms_message_fatal_continue(("%s: cannot convert value to valid real value\n",myreal))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
  }
  return nb;
}
void bp (void) {}
char*adms_integertostring (int value)
{
   char*string=malloc(sizeof(char)*50);
   sprintf(string,"%i",value);
   return string;
}
char*adms_doubletostring (double value)
{
   if(isnan(value))
   {
     char*string=malloc(sizeof(char)*4);
     sprintf(string,"nan");
     return string;
   }
   else
   {
     char*string=malloc(sizeof(char)*50);
     sprintf(string,"%e",value);
     return string;
   }
}
int adms_file_isregular (const char* myfilename)
{
  return((access(myfilename,ADMS_F_OK)==0));
}
int adms_file_isdirectory (const char* myfilename)
{
  struct stat s;
  return((stat(myfilename,&s)==0)&&(s.st_mode&ADMS_S_IFDIR));
}
int adms_setenv (const char* myname,const char* myvalue)
{
  int myres;
#if defined(HAVE_PUTENV)
  char*myassign=NULL;
  adms_k2strconcat(&myassign,myname);
  adms_k2strconcat(&myassign,"=");
  adms_k2strconcat(&myassign,myvalue);
  myres=putenv(myassign);
  free(myassign);
#else
  myres=setenv(myname,myvalue,1);
#endif
  return (myres==0);
}
p_slist adms_slist_last (p_slist l)
{
  if(l)
  {
    while(l->next)
      l=l->next;
  }
  return l;
}
p_slist adms_slist_new (p_adms d)
{
  p_slist newl=NULL;
  adms_slist_push(&newl,d);
  return newl;
}
p_slist adms_slist_copy (p_slist l)
{
  p_slist copiedl=NULL;
  while(l)
  {
    adms_slist_push(&copiedl,l->data);
    l=l->next;
  }
  return adms_slist_reverse(copiedl);
}
void adms_slist_push (p_slist* l,p_adms data)
{
  p_slist n=malloc(sizeof(t_slist));
  n->next=*l;
  n->data=data;
  *l=n;
}
p_adms adms_slist_pull (p_slist* l)
{
  if(*l)
  {
    p_slist n=*l;
    p_adms data=n->data;
    *l=(*l)->next;
    free(n);
    return data;
  }
  return NULL;
}
void adms_slist_concat (p_slist* l1,p_slist l2)
{
  if(l2)
  {
    if(*l1)
      adms_slist_last(*l1)->next=l2;
    else
      *l1=l2;
  }
}
unsigned int adms_slist_length (p_slist l)
{
  unsigned int length=0;
  while(l)
  {
    length++;
    l=l->next;
  }
  return length;
}
p_slist adms_slist_nth (p_slist l,unsigned int  n)
{
  while (n-->0 && l)
    l=l->next;
  return l;
}
p_adms adms_slist_nth_data (p_slist l,unsigned int n)
{
  while (n-->0 && l)
    l=l->next;
  return l ? l->data : ((p_adms)0);
}
p_slist adms_slist_find (p_slist l,p_kadms data)
{
  while(l)
  {
    if(l->data==data)
      break;
    l=l->next;
  }
  return l;
}
int adms_slist_index (p_slist l, p_kadms data)
{
  int i=0;
  while(l)
  {
    if(l->data==data)
      return i;
    i++;
    l=l->next;
  }
  return -1;
}
p_slist adms_slist_reverse (p_slist l)
{
  p_slist p=NULL;
  while(l)
  {
    p_slist n=l->next;
    l->next=p;
    p=l;
    l=n;
  }
  return p;
}
void adms_slist_inreverse (p_slist* l)
{
  *l=adms_slist_reverse(*l);
}
void adms_slist_free (p_slist l)
{
  while(l)
  {
    p_slist freed=l;
    l=l->next;
    free(freed);
  }
}
int globalnbadmstnew=0, globalnbadmstdestroy=0;
int adms_global_nbadmstnew (void) {return globalnbadmstnew;}
int adms_global_nbadmstdestroy (void) {return globalnbadmstdestroy;}
void adms_global_increment_nbadmstnew (void) {globalnbadmstnew++;}
void adms_global_increment_nbadmstdestroy (void) {globalnbadmstdestroy++;}
p_admsmain globaladmsmain;
p_admsmain root (void) {return globaladmsmain;}
void rootnew (p_admsmain myglobaladmsmain) {globaladmsmain=myglobaladmsmain;}

/*-- Messaging --*/
void adms_toupper (char*m)
{
  for(;*m;m++)
    *m=toupper(*m);
}
void adms_tolower (char*m)
{
  for(;*m;m++)
    *m=tolower(*m);
}
char*adms_kclone (const char* m)
{
  if(m)
  {
    int l=strlen(m);
    char*mycpy=(char*)malloc((l+1)*sizeof(char));
    memcpy(mycpy,m,l);
    mycpy[l]='\0';
    return mycpy;
  }
  else
    return NULL;
}
char*adms_knclone (const char* m,const int l)
{
  char*mycpy=(char*)malloc((l+1)*sizeof(char));
  memcpy(mycpy,m,l);
  mycpy[l]='\0';
  return mycpy;
}
/*clone array (m,n(: "abcdefghijkl\0" returns as "cdef\0"*/
/*                      m   n                            */
/* same as adms_knclone(m,n-m)                           */
char*adms_m2nclone (const char* m,const char* n)
{
  int l=(char*)n-(char*)m;
  char*mycpy=(char*)malloc((l+1)*sizeof(char));
  memcpy(mycpy,m,l);
  mycpy[l]='\0';
  return mycpy;
}
/*s1: realloced s2: constant ret: s1=s1s2*/
void adms_k2strconcat (char **s1,const char* s2)
{
  if(!s2)
    return;
  if(*s1)
  {
    int l1=strlen(*s1);
    int l2=strlen(s2);
    *s1=(char*)realloc(*s1,(l1+l2+1)*sizeof(char));
    memcpy(*s1+l1,s2,l2+1);
  }
  else
    *s1=strdup(s2);
}
/*s1: realloced s2: freed ret: s1=s1s2*/
void adms_strconcat (char **s1,char *s2)
{
  adms_k2strconcat(s1,s2);
  free(s2);
}
FILE* adms_file_open_read (const char* myfilename)
{
  FILE* fh=fopen(myfilename,"r");
  if(!fh)
    adms_message_fatal(("%s: failed to open file [read mode]\n",myfilename))
  return fh;
}

_t_message (adms_message_info_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  fputs("[info...] ",stdout);
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stdout); else fputs("NULL",stdout);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stdout,"%e",d);
          break;
        case 'c':
          c=va_arg (ap,int);
          fprintf(stdout,"%c",c);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stdout,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stdout,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stdout,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stdout,"%p",p);
          break;
        default:
          fputc(*format,stdout);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stdout);
      }
    }
  }
  va_end (ap);
  fflush(stdout);
}
_t_message (adms_message_info_continue_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stdout); else fputs("NULL",stdout);
          break;
        case 'c':
          c=va_arg (ap,int);
          fputc(c,stdout);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stdout,"%e",d);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stdout,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stdout,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stdout,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stdout,"%p",p);
          break;
        default:
          fputc(*format,stdout);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stdout);
      }
    }
  }
  va_end (ap);
  fflush(stdout);
}
_t_message (adms_message_usage_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  fputs("[usage..] ",stdout);
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stdout); else fputs("NULL",stdout);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stdout,"%e",d);
          break;
        case 'c':
          c=va_arg (ap,int);
          fprintf(stdout,"%c",c);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stdout,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stdout,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stdout,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stdout,"%p",p);
          break;
        default:
          fputc(*format,stdout);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stdout);
      }
    }
  }
  va_end (ap);
  fflush(stdout);
}
_t_message (adms_message_usage_continue_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stdout); else fputs("NULL",stdout);
          break;
        case 'c':
          c=va_arg (ap,int);
          fputc(c,stdout);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stdout,"%e",d);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stdout,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stdout,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stdout,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stdout,"%p",p);
          break;
        default:
          fputc(*format,stdout);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stdout);
      }
    }
  }
  va_end (ap);
  fflush(stdout);
}
_t_message (adms_message_verbose_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  fputs("[verbose] ",stdout);
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stdout); else fputs("NULL",stdout);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stdout,"%e",d);
          break;
        case 'c':
          c=va_arg (ap,int);
          fprintf(stdout,"%c",c);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stdout,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stdout,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stdout,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stdout,"%p",p);
          break;
        default:
          fputc(*format,stdout);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stdout);
      }
    }
  }
  va_end (ap);
  fflush(stdout);
}
_t_message (adms_message_verbose_continue_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stdout); else fputs("NULL",stdout);
          break;
        case 'c':
          c=va_arg (ap,int);
          fputc(c,stdout);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stdout,"%e",d);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stdout,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stdout,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stdout,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stdout,"%p",p);
          break;
        default:
          fputc(*format,stdout);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stdout);
      }
    }
  }
  va_end (ap);
  fflush(stdout);
}
_t_message (adms_message_debug_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  fputs("[debug..] ",stdout);
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stdout); else fputs("NULL",stdout);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stdout,"%e",d);
          break;
        case 'c':
          c=va_arg (ap,int);
          fprintf(stdout,"%c",c);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stdout,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stdout,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stdout,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stdout,"%p",p);
          break;
        default:
          fputc(*format,stdout);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stdout);
      }
    }
  }
  va_end (ap);
  fflush(stdout);
}
_t_message (adms_message_debug_continue_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stdout); else fputs("NULL",stdout);
          break;
        case 'c':
          c=va_arg (ap,int);
          fputc(c,stdout);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stdout,"%e",d);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stdout,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stdout,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stdout,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stdout,"%p",p);
          break;
        default:
          fputc(*format,stdout);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stdout);
      }
    }
  }
  va_end (ap);
  fflush(stdout);
}
_t_message (adms_message_dbg_vla_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  fputs("[dbg_vla] ",stdout);
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stdout); else fputs("NULL",stdout);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stdout,"%e",d);
          break;
        case 'c':
          c=va_arg (ap,int);
          fprintf(stdout,"%c",c);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stdout,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stdout,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stdout,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stdout,"%p",p);
          break;
        default:
          fputc(*format,stdout);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stdout);
      }
    }
  }
  va_end (ap);
  fflush(stdout);
}
_t_message (adms_message_dbg_vla_continue_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stdout); else fputs("NULL",stdout);
          break;
        case 'c':
          c=va_arg (ap,int);
          fputc(c,stdout);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stdout,"%e",d);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stdout,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stdout,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stdout,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stdout,"%p",p);
          break;
        default:
          fputc(*format,stdout);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stdout);
      }
    }
  }
  va_end (ap);
  fflush(stdout);
}
_t_message (adms_message_dbg_xml_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  fputs("[dbg_xml] ",stdout);
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stdout); else fputs("NULL",stdout);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stdout,"%e",d);
          break;
        case 'c':
          c=va_arg (ap,int);
          fprintf(stdout,"%c",c);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stdout,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stdout,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stdout,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stdout,"%p",p);
          break;
        default:
          fputc(*format,stdout);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stdout);
      }
    }
  }
  va_end (ap);
  fflush(stdout);
}
_t_message (adms_message_dbg_xml_continue_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stdout); else fputs("NULL",stdout);
          break;
        case 'c':
          c=va_arg (ap,int);
          fputc(c,stdout);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stdout,"%e",d);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stdout,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stdout,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stdout,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stdout,"%p",p);
          break;
        default:
          fputc(*format,stdout);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stdout);
      }
    }
  }
  va_end (ap);
  fflush(stdout);
}
_t_message (adms_message_dbg_mem_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  fputs("[dbg_mem] ",stdout);
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stdout); else fputs("NULL",stdout);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stdout,"%e",d);
          break;
        case 'c':
          c=va_arg (ap,int);
          fprintf(stdout,"%c",c);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stdout,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stdout,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stdout,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stdout,"%p",p);
          break;
        default:
          fputc(*format,stdout);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stdout);
      }
    }
  }
  va_end (ap);
  fflush(stdout);
}
_t_message (adms_message_dbg_mem_continue_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stdout); else fputs("NULL",stdout);
          break;
        case 'c':
          c=va_arg (ap,int);
          fputc(c,stdout);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stdout,"%e",d);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stdout,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stdout,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stdout,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stdout,"%p",p);
          break;
        default:
          fputc(*format,stdout);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stdout);
      }
    }
  }
  va_end (ap);
  fflush(stdout);
}
_t_message (adms_message_hint_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  fputs("[hint...] ",stderr);
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stderr); else fputs("NULL",stderr);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stderr,"%e",d);
          break;
        case 'c':
          c=va_arg (ap,int);
          fprintf(stderr,"%c",c);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stderr,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stderr,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stderr,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stderr,"%p",p);
          break;
        default:
          fputc(*format,stderr);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stderr);
      }
    }
  }
  va_end (ap);
  fflush(stderr);
}
_t_message (adms_message_hint_continue_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stderr); else fputs("NULL",stderr);
          break;
        case 'c':
          c=va_arg (ap,int);
          fputc(c,stderr);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stderr,"%e",d);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stderr,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stderr,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stderr,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stderr,"%p",p);
          break;
        default:
          fputc(*format,stderr);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stderr);
      }
    }
  }
  va_end (ap);
  fflush(stderr);
}
_t_message (adms_message_warning_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  fputs("[warning] ",stderr);
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stderr); else fputs("NULL",stderr);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stderr,"%e",d);
          break;
        case 'c':
          c=va_arg (ap,int);
          fprintf(stderr,"%c",c);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stderr,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stderr,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stderr,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stderr,"%p",p);
          break;
        default:
          fputc(*format,stderr);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stderr);
      }
    }
  }
  va_end (ap);
  fflush(stderr);
}
_t_message (adms_message_warning_continue_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stderr); else fputs("NULL",stderr);
          break;
        case 'c':
          c=va_arg (ap,int);
          fputc(c,stderr);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stderr,"%e",d);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stderr,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stderr,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stderr,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stderr,"%p",p);
          break;
        default:
          fputc(*format,stderr);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stderr);
      }
    }
  }
  va_end (ap);
  fflush(stderr);
}
_t_message (adms_message_obsolete_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  fputs("[obsolete] ",stderr);
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stderr); else fputs("NULL",stderr);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stderr,"%e",d);
          break;
        case 'c':
          c=va_arg (ap,int);
          fprintf(stderr,"%c",c);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stderr,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stderr,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stderr,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stderr,"%p",p);
          break;
        default:
          fputc(*format,stderr);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stderr);
      }
    }
  }
  va_end (ap);
  fflush(stderr);
}
_t_message (adms_message_obsolete_continue_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stderr); else fputs("NULL",stderr);
          break;
        case 'c':
          c=va_arg (ap,int);
          fputc(c,stderr);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stderr,"%e",d);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stderr,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stderr,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stderr,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stderr,"%p",p);
          break;
        default:
          fputc(*format,stderr);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stderr);
      }
    }
  }
  va_end (ap);
  fflush(stderr);
}
_t_message (adms_message_error_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  fputs("[error..] ",stderr);
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stderr); else fputs("NULL",stderr);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stderr,"%e",d);
          break;
        case 'c':
          c=va_arg (ap,int);
          fprintf(stderr,"%c",c);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stderr,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stderr,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stderr,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stderr,"%p",p);
          break;
        default:
          fputc(*format,stderr);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stderr);
      }
    }
  }
  va_end (ap);
  fflush(stderr);
  bp();
}
_t_message (adms_message_error_continue_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  fputs("[error..] ",stderr);
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stderr); else fputs("NULL",stderr);
          break;
        case 'c':
          c=va_arg (ap,int);
          fputc(c,stderr);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stderr,"%e",d);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stderr,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stderr,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stderr,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stderr,"%p",p);
          break;
        default:
          fputc(*format,stderr);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stderr);
      }
    }
  }
  va_end (ap);
  fflush(stderr);
}
_t_message (adms_message_fatal_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  fputs("[fatal..] ",stderr);
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stderr); else fputs("NULL",stderr);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stderr,"%e",d);
          break;
        case 'c':
          c=va_arg (ap,int);
          fprintf(stderr,"%c",c);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stderr,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stderr,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stderr,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stderr,"%p",p);
          break;
        default:
          fputc(*format,stderr);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stderr);
      }
    }
  }
  va_end (ap);
  fflush(stderr);
  bp(),exit(1);
}
_t_message (adms_message_fatal_continue_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  fputs("[fatal..] ",stderr);
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stderr); else fputs("NULL",stderr);
          break;
        case 'c':
          c=va_arg (ap,int);
          fputc(c,stderr);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stderr,"%e",d);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stderr,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stderr,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stderr,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stderr,"%p",p);
          break;
        default:
          fputc(*format,stderr);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stderr);
      }
    }
  }
  va_end (ap);
  fflush(stderr);
}
_t_message (adms_message_admstdbg_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stdadmstdbg); else fputs("NULL",stdadmstdbg);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stdadmstdbg,"%e",d);
          break;
        case 'c':
          c=va_arg (ap,int);
          fprintf(stdadmstdbg,"%c",c);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stdadmstdbg,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stdadmstdbg,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stdadmstdbg,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stdadmstdbg,"%p",p);
          break;
        default:
          fputc(*format,stdadmstdbg);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stdadmstdbg);
      }
    }
  }
  va_end (ap);
  fflush(stdadmstdbg);
}
_t_message (adms_message_admstdbg_continue_impl)
{
  va_list ap;
  int insideformat=0;
  int i;
  char* s;
  char c;
  double d;
  void* p;
  va_start(ap, format);
  for(;*format;format++)
  {
    if(insideformat)
    {
      insideformat=0;
      switch(*format) 
      {
        case 's':
          s=va_arg (ap,char*);
          if(s) fputs(s,stdadmstdbg); else fputs("NULL",stdadmstdbg);
          break;
        case 'c':
          c=va_arg (ap,int);
          fputc(c,stdadmstdbg);
          break;
        case 'e':
          d=va_arg (ap,double);
          fprintf(stdadmstdbg,"%e",d);
          break;
        case 'g':
          d=va_arg (ap,double);
          fprintf(stdadmstdbg,"%g",d);
          break;
        case 'f':
          d=va_arg (ap,double);
          fprintf(stdadmstdbg,"%f",d);
          break;
        case 'i':
          i=va_arg (ap,int);
          fprintf(stdadmstdbg,"%i",i);
          break;
        case 'p':
          p=va_arg (ap,void*);
          fprintf(stdadmstdbg,"%p",p);
          break;
        default:
          fputc(*format,stdadmstdbg);
      }
    }
    else
    {
      switch(*format) 
      {
        case '%':
          insideformat=1;
          break;
        default:
          fputc(*format,stdadmstdbg);
      }
    }
  }
  va_end (ap);
  fflush(stdadmstdbg);
}
