
/* ------- code automatically created by ./mkelements.pl -------------- */

#ifndef adms_h
#define adms_h

#define SVN "1229M" /* svn release version */

/*headers -- depend on compiler, OS, ...*/
#  if defined(_MSC_VER)
#    define PACKAGE_NAME "adms"
#    if ! defined(PACKAGE_VERSION)
#      pragma message(__FILE__ ": warning: #warning undefined macro 'PACKAGE_VERSION' - defaulted to 'unknown'")
#      define PACKAGE_VERSION "unknown"
#    endif
#    define PACKAGE_STRING PACKAGE_NAME " " PACKAGE_VERSION
#    define PACKAGE_TARNAME PACKAGE_NAME
#    define PACKAGE_BUGREPORT "r29173@gmail.com"
#    include <direct.h>
#    define isnan _isnan
#    define ADMS_PATH_SEPARATOR "\\"
#    define ADMS_F_OK 0
#    ifndef inline
#      define inline
#    endif
#    define HAVE_FLOAT_H 1
#    define HAVE_STDLIB_H 1 
#    define HAVE_PUTENV 1 
#    define HAVE_LOCALE 1 
#    define HAVE_STRING_H 1 
#    define HAVE_SYS_STAT_H 1
#    include <io.h>
#  else
#    include <config.h>
#    define ADMS_PATH_SEPARATOR "/"
#    include <unistd.h>
#    define ADMS_F_OK F_OK
#  endif
#  include <math.h>
#  if defined(HAVE_SYS_STAT_H)
#    include <sys/stat.h>
#  endif
#  if defined(HAVE_LOCALE_H)
#    include <locale.h>
#  endif
#  if defined(HAVE_FLOAT_H)
#    include <float.h>
#  endif
#  if defined(HAVE_UNISTD_H)
#    include <unistd.h>
#  endif
#  if defined(HAVE_STDLIB_H)
#    include <stdlib.h>
#  endif
#  if defined(HAVE_STRING_H)
#    include <string.h>
#  endif
#  include <assert.h>
#  include <signal.h>
#  include <stdarg.h>
#  include <stdio.h>
#  include <ctype.h>
#  include <errno.h>
#  include <time.h>
/* in case not Posix */
#  if defined(_S_IFDIR)
#    define ADMS_S_IFDIR _S_IFDIR
#  else 
#    define ADMS_S_IFDIR S_IFDIR
#  endif
/* check OS */
#  if defined(__CYGWIN__)
#    define ADMS_OS_MS
#    define ADMS_OS_MSCYGWIN
#    define ADMS_OS "MSCYGWIN"
#  elif defined(__MSDOS__)
#    define ADMS_OS_MS
#    define ADMS_OS_MSDOS
#    define ADMS_OS "MSDOS"
#  elif defined(_WIN64)
#    define ADMS_OS_MS
#    define ADMS_OS_MSWIN64
#    define ADMS_OS "MSWIN64"
#  elif defined(_WIN32)
#    define ADMS_OS_MS
#    define ADMS_OS_MSWIN32
#    define ADMS_OS "MSWIN32"
#  else
#    define ADMS_OS_UNKNOWN
#    define ADMS_OS "UNKNOWN"
#  endif
/* check compiler */
#  if defined(__MINGW32__)
#    ifndef WIN32
#      define WIN32
#    endif
#    define ADMS_COMPILER_MINGCC
#    define ADMS_COMPILER "MINGCC"
#  elif defined(__CYGWIN__)
#    ifndef WIN32
#      define WIN32
#    endif
#    define ADMS_COMPILER_GCC 
#    define ADMS_COMPILER "GCC" 
#  elif defined(_MSC_VER)
#    ifndef WIN32
#      define WIN32
#    endif
#    define ADMS_COMPILER_MSVC
#    define ADMS_COMPILER "MSVC"
#  else
#    define ADMS_COMPILER_CC
#    define ADMS_COMPILER "CC"
#  endif
#define E\
  EE(adms)\
  EE(simulator)\
  EE(admsmain)\
  EE(list)\
  EE(nature)\
  EE(discipline)\
  EE(module)\
  EE(analogfunction)\
  EE(nodealias)\
  EE(node)\
  EE(branchalias)\
  EE(branch)\
  EE(variableprototype)\
  EE(source)\
  EE(range)\
  EE(jacobian)\
  EE(analog)\
  EE(math)\
  EE(string)\
  EE(subexpression)\
  EE(mapply_unary)\
  EE(mapply_binary)\
  EE(mapply_ternary)\
  EE(number)\
  EE(function)\
  EE(variable)\
  EE(array)\
  EE(probe)\
  EE(expression)\
  EE(instance)\
  EE(instanceparameter)\
  EE(instancenode)\
  EE(nilled)\
  EE(assignment)\
  EE(contribution)\
  EE(conditional)\
  EE(forloop)\
  EE(whileloop)\
  EE(case)\
  EE(caseitem)\
  EE(blockvariable)\
  EE(block)\
  EE(callfunction)\
  EE(evaluation)\
  EE(text)\
  EE(path)\
  EE(ptraverse)\
  EE(pparse)\
  EE(admst)\
  EE(transform)\
  EE(itransform)\
  EE(return)\
  EE(attribute)\
  EE(admstvariable)\
  EE(lexval)\
  EE(yaccval)\
/*fin*/

#define A\
  AA(datatypename)\
  AA(attribute)\
  AA(variable)\
  AA(name)\
  AA(currentdate)\
  AA(developer)\
  AA(fullname)\
  AA(package_name)\
  AA(package_tarname)\
  AA(package_version)\
  AA(package_string)\
  AA(package_bugreport)\
  AA(probe)\
  AA(tmp)\
  AA(tmp1)\
  AA(br1)\
  AA(br2)\
  AA(sr1)\
  AA(ddx)\
  AA(name)\
  AA(argc)\
  AA(fullfilename)\
  AA(filename)\
  AA(curfilename)\
  AA(curline)\
  AA(fpos)\
  AA(simulator)\
  AA(info)\
  AA(usage)\
  AA(verbose)\
  AA(hint)\
  AA(warning)\
  AA(obsolete)\
  AA(debug)\
  AA(dbg_vla)\
  AA(dbg_xml)\
  AA(error)\
  AA(fatal)\
  AA(dbg_mem)\
  AA(module)\
  AA(discipline)\
  AA(nature)\
  AA(variable)\
  AA(argv)\
  AA(transform)\
  AA(invtransform)\
  AA(itransform)\
  AA(itransforminsideapplytemplate)\
  AA(valueof)\
  AA(datatype)\
  AA(item)\
  AA(access)\
  AA(name)\
  AA(abstol)\
  AA(base)\
  AA(ddt_name)\
  AA(ddt_nature)\
  AA(idt_name)\
  AA(idt_nature)\
  AA(units)\
  AA(name)\
  AA(flow)\
  AA(potential)\
  AA(domain)\
  AA(name)\
  AA(node)\
  AA(nodealias)\
  AA(branch)\
  AA(branchalias)\
  AA(analogfunction)\
  AA(instance)\
  AA(variable)\
  AA(analog)\
  AA(block)\
  AA(blockvariable)\
  AA(assignment)\
  AA(callfunction)\
  AA(contribution)\
  AA(conditional)\
  AA(case)\
  AA(forloop)\
  AA(whileloop)\
  AA(instantiator)\
  AA(expression)\
  AA(jacobian)\
  AA(probe)\
  AA(source)\
  AA(range)\
  AA(attribute)\
  AA(evaluation)\
  AA(module)\
  AA(lexval)\
  AA(type)\
  AA(tree)\
  AA(variable)\
  AA(attribute)\
  AA(module)\
  AA(name)\
  AA(node)\
  AA(module)\
  AA(name)\
  AA(direction)\
  AA(discipline)\
  AA(grounded)\
  AA(location)\
  AA(attribute)\
  AA(module)\
  AA(name)\
  AA(branch)\
  AA(module)\
  AA(pnode)\
  AA(nnode)\
  AA(discipline)\
  AA(grounded)\
  AA(attribute)\
  AA(module)\
  AA(lexval)\
  AA(block)\
  AA(dependency)\
  AA(static)\
  AA(dynamic)\
  AA(sizetype)\
  AA(parametertype)\
  AA(type)\
  AA(maxsize)\
  AA(minsize)\
  AA(input)\
  AA(output)\
  AA(scope)\
  AA(default)\
  AA(derivate)\
  AA(isstate)\
  AA(usedinmodel)\
  AA(usedininstance)\
  AA(usedininitial_step)\
  AA(usedinevaluate)\
  AA(usedinnoise)\
  AA(usedinfinal)\
  AA(setinmodel)\
  AA(setininstance)\
  AA(setininitial_step)\
  AA(setinevaluate)\
  AA(setinnoise)\
  AA(setinfinal)\
  AA(setingetprev)\
  AA(TemperatureDependent)\
  AA(OPdependent)\
  AA(insource)\
  AA(vcount)\
  AA(vlast)\
  AA(arraydefault)\
  AA(attribute)\
  AA(alias)\
  AA(range)\
  AA(probe)\
  AA(ddxprobe)\
  AA(variable)\
  AA(assignment)\
  AA(instance)\
  AA(module)\
  AA(branch)\
  AA(nature)\
  AA(discipline)\
  AA(static)\
  AA(dynamic)\
  AA(whitenoise)\
  AA(flickernoise)\
  AA(grounded)\
  AA(switch)\
  AA(type)\
  AA(probe)\
  AA(attribute)\
  AA(module)\
  AA(infexpr)\
  AA(supexpr)\
  AA(name)\
  AA(infboundtype)\
  AA(supboundtype)\
  AA(type)\
  AA(module)\
  AA(row)\
  AA(column)\
  AA(diagonal)\
  AA(static)\
  AA(dynamic)\
  AA(code)\
  AA(name)\
  AA(value)\
  AA(static)\
  AA(dynamic)\
  AA(dependency)\
  AA(attribute)\
  AA(value)\
  AA(expression)\
  AA(name)\
  AA(arg1)\
  AA(name)\
  AA(arg1)\
  AA(arg2)\
  AA(name)\
  AA(arg1)\
  AA(arg2)\
  AA(arg3)\
  AA(lexval)\
  AA(scalingunit)\
  AA(cast)\
  AA(lexval)\
  AA(unique_id)\
  AA(definition)\
  AA(class)\
  AA(arguments)\
  AA(prototype)\
  AA(array)\
  AA(variable)\
  AA(index)\
  AA(module)\
  AA(branch)\
  AA(nature)\
  AA(seen)\
  AA(discipline)\
  AA(grounded)\
  AA(source)\
  AA(module)\
  AA(tree)\
  AA(hasspecialnumber)\
  AA(hasVoltageDependentFunction)\
  AA(infinity)\
  AA(TemperatureDependent)\
  AA(value)\
  AA(function)\
  AA(probe)\
  AA(variable)\
  AA(instantiator)\
  AA(module)\
  AA(name)\
  AA(terminal)\
  AA(parameterset)\
  AA(parameter)\
  AA(value)\
  AA(nodefrominstantiator)\
  AA(nodefrommodule)\
  AA(module)\
  AA(name)\
  AA(module)\
  AA(lhs)\
  AA(rhs)\
  AA(lexval)\
  AA(name)\
  AA(attribute)\
  AA(module)\
  AA(lhs)\
  AA(rhs)\
  AA(lexval)\
  AA(branchalias)\
  AA(name)\
  AA(whitenoise)\
  AA(flickernoise)\
  AA(attribute)\
  AA(module)\
  AA(if)\
  AA(then)\
  AA(else)\
  AA(name)\
  AA(module)\
  AA(initial)\
  AA(condition)\
  AA(update)\
  AA(forblock)\
  AA(name)\
  AA(module)\
  AA(while)\
  AA(whileblock)\
  AA(name)\
  AA(module)\
  AA(case)\
  AA(name)\
  AA(caseitem)\
  AA(code)\
  AA(defaultcase)\
  AA(condition)\
  AA(block)\
  AA(variable)\
  AA(module)\
  AA(lexval)\
  AA(block)\
  AA(item)\
  AA(attribute)\
  AA(variable)\
  AA(probe)\
  AA(module)\
  AA(function)\
  AA(variable)\
  AA(transform)\
  AA(value)\
  AA(aname)\
  AA(admse)\
  AA(token)\
  AA(transform)\
  AA(text)\
  AA(aname)\
  AA(avalue)\
  AA(template)\
  AA(keeplist)\
  AA(callbackname)\
  AA(callback)\
  AA(bar)\
  AA(arg)\
  AA(u)\
  AA(b)\
  AA(c)\
  AA(transform)\
  AA(admst)\
  AA(alladmst)\
  AA(aliasadmst)\
  AA(position)\
  AA(a1)\
  AA(an)\
  AA(transform)\
  AA(aname)\
  AA(value)\
  AA(path)\
  AA(text)\
  AA(position)\
  AA(tkstart)\
  AA(tklen)\
  AA(tkid)\
  AA(previous)\
  AA(previouspath)\
  AA(pseudo)\
  AA(item)\
  AA(valueto)\
  AA(position)\
  AA(count)\
  AA(hasnewbasicstring)\
  AA(original)\
  AA(refd)\
  AA(true)\
  AA(preva)\
  AA(nexta)\
  AA(selected)\
  AA(fullfilename)\
  AA(basefilename)\
  AA(l)\
  AA(name)\
  AA(parent)\
  AA(callback)\
  AA(id)\
  AA(pathselect)\
  AA(pathpath)\
  AA(pathinto)\
  AA(pathtest)\
  AA(pathlist)\
  AA(textselect)\
  AA(textdatatype)\
  AA(textmatch)\
  AA(textrequired)\
  AA(textseparator)\
  AA(textfrom)\
  AA(textto)\
  AA(textlist)\
  AA(textname)\
  AA(textformat)\
  AA(textfile)\
  AA(textversion)\
  AA(textonduplicate)\
  AA(textprobe)\
  AA(textstring)\
  AA(templates)\
  AA(textarguments)\
  AA(pathoncompare)\
  AA(pathinputs)\
  AA(attribute)\
  AA(children)\
  AA(template)\
  AA(originalcall)\
  AA(variable)\
  AA(return)\
  AA(name)\
  AA(value)\
  AA(name)\
  AA(value)\
  AA(name)\
  AA(value)\
  AA(string)\
  AA(f)\
  AA(l)\
  AA(c)\
  AA(filename)\
  AA(usrdata)\
/*fin*/

#define EA\
  EEA(adms,datatypename)\
  EEA(adms,attribute)\
  EEA(adms,variable)\
  EEA(simulator,name)\
  EEA(simulator,currentdate)\
  EEA(simulator,developer)\
  EEA(simulator,fullname)\
  EEA(simulator,package_name)\
  EEA(simulator,package_tarname)\
  EEA(simulator,package_version)\
  EEA(simulator,package_string)\
  EEA(simulator,package_bugreport)\
  EEA(simulator,probe)\
  EEA(simulator,tmp)\
  EEA(simulator,tmp1)\
  EEA(simulator,br1)\
  EEA(simulator,br2)\
  EEA(simulator,sr1)\
  EEA(simulator,ddx)\
  EEA(admsmain,name)\
  EEA(admsmain,argc)\
  EEA(admsmain,fullfilename)\
  EEA(admsmain,filename)\
  EEA(admsmain,curfilename)\
  EEA(admsmain,curline)\
  EEA(admsmain,fpos)\
  EEA(admsmain,simulator)\
  EEA(admsmain,info)\
  EEA(admsmain,usage)\
  EEA(admsmain,verbose)\
  EEA(admsmain,hint)\
  EEA(admsmain,warning)\
  EEA(admsmain,obsolete)\
  EEA(admsmain,debug)\
  EEA(admsmain,dbg_vla)\
  EEA(admsmain,dbg_xml)\
  EEA(admsmain,error)\
  EEA(admsmain,fatal)\
  EEA(admsmain,dbg_mem)\
  EEA(admsmain,module)\
  EEA(admsmain,discipline)\
  EEA(admsmain,nature)\
  EEA(admsmain,variable)\
  EEA(admsmain,argv)\
  EEA(admsmain,transform)\
  EEA(admsmain,invtransform)\
  EEA(admsmain,itransform)\
  EEA(admsmain,itransforminsideapplytemplate)\
  EEA(admsmain,valueof)\
  EEA(list,datatype)\
  EEA(list,item)\
  EEA(nature,access)\
  EEA(nature,name)\
  EEA(nature,abstol)\
  EEA(nature,base)\
  EEA(nature,ddt_name)\
  EEA(nature,ddt_nature)\
  EEA(nature,idt_name)\
  EEA(nature,idt_nature)\
  EEA(nature,units)\
  EEA(discipline,name)\
  EEA(discipline,flow)\
  EEA(discipline,potential)\
  EEA(discipline,domain)\
  EEA(module,name)\
  EEA(module,node)\
  EEA(module,nodealias)\
  EEA(module,branch)\
  EEA(module,branchalias)\
  EEA(module,analogfunction)\
  EEA(module,instance)\
  EEA(module,variable)\
  EEA(module,analog)\
  EEA(module,block)\
  EEA(module,blockvariable)\
  EEA(module,assignment)\
  EEA(module,callfunction)\
  EEA(module,contribution)\
  EEA(module,conditional)\
  EEA(module,case)\
  EEA(module,forloop)\
  EEA(module,whileloop)\
  EEA(module,instantiator)\
  EEA(module,expression)\
  EEA(module,jacobian)\
  EEA(module,probe)\
  EEA(module,source)\
  EEA(module,range)\
  EEA(module,attribute)\
  EEA(module,evaluation)\
  EEA(analogfunction,module)\
  EEA(analogfunction,lexval)\
  EEA(analogfunction,type)\
  EEA(analogfunction,tree)\
  EEA(analogfunction,variable)\
  EEA(analogfunction,attribute)\
  EEA(nodealias,module)\
  EEA(nodealias,name)\
  EEA(nodealias,node)\
  EEA(node,module)\
  EEA(node,name)\
  EEA(node,direction)\
  EEA(node,discipline)\
  EEA(node,grounded)\
  EEA(node,location)\
  EEA(node,attribute)\
  EEA(branchalias,module)\
  EEA(branchalias,name)\
  EEA(branchalias,branch)\
  EEA(branch,module)\
  EEA(branch,pnode)\
  EEA(branch,nnode)\
  EEA(branch,discipline)\
  EEA(branch,grounded)\
  EEA(branch,attribute)\
  EEA(variableprototype,module)\
  EEA(variableprototype,lexval)\
  EEA(variableprototype,block)\
  EEA(variableprototype,dependency)\
  EEA(variableprototype,static)\
  EEA(variableprototype,dynamic)\
  EEA(variableprototype,sizetype)\
  EEA(variableprototype,parametertype)\
  EEA(variableprototype,type)\
  EEA(variableprototype,maxsize)\
  EEA(variableprototype,minsize)\
  EEA(variableprototype,input)\
  EEA(variableprototype,output)\
  EEA(variableprototype,scope)\
  EEA(variableprototype,default)\
  EEA(variableprototype,derivate)\
  EEA(variableprototype,isstate)\
  EEA(variableprototype,usedinmodel)\
  EEA(variableprototype,usedininstance)\
  EEA(variableprototype,usedininitial_step)\
  EEA(variableprototype,usedinevaluate)\
  EEA(variableprototype,usedinnoise)\
  EEA(variableprototype,usedinfinal)\
  EEA(variableprototype,setinmodel)\
  EEA(variableprototype,setininstance)\
  EEA(variableprototype,setininitial_step)\
  EEA(variableprototype,setinevaluate)\
  EEA(variableprototype,setinnoise)\
  EEA(variableprototype,setinfinal)\
  EEA(variableprototype,setingetprev)\
  EEA(variableprototype,TemperatureDependent)\
  EEA(variableprototype,OPdependent)\
  EEA(variableprototype,insource)\
  EEA(variableprototype,vcount)\
  EEA(variableprototype,vlast)\
  EEA(variableprototype,arraydefault)\
  EEA(variableprototype,attribute)\
  EEA(variableprototype,alias)\
  EEA(variableprototype,range)\
  EEA(variableprototype,probe)\
  EEA(variableprototype,ddxprobe)\
  EEA(variableprototype,variable)\
  EEA(variableprototype,assignment)\
  EEA(variableprototype,instance)\
  EEA(source,module)\
  EEA(source,branch)\
  EEA(source,nature)\
  EEA(source,discipline)\
  EEA(source,static)\
  EEA(source,dynamic)\
  EEA(source,whitenoise)\
  EEA(source,flickernoise)\
  EEA(source,grounded)\
  EEA(source,switch)\
  EEA(source,type)\
  EEA(source,probe)\
  EEA(source,attribute)\
  EEA(range,module)\
  EEA(range,infexpr)\
  EEA(range,supexpr)\
  EEA(range,name)\
  EEA(range,infboundtype)\
  EEA(range,supboundtype)\
  EEA(range,type)\
  EEA(jacobian,module)\
  EEA(jacobian,row)\
  EEA(jacobian,column)\
  EEA(jacobian,diagonal)\
  EEA(jacobian,static)\
  EEA(jacobian,dynamic)\
  EEA(analog,code)\
  EEA(math,name)\
  EEA(math,value)\
  EEA(math,static)\
  EEA(math,dynamic)\
  EEA(math,dependency)\
  EEA(math,attribute)\
  EEA(string,value)\
  EEA(subexpression,expression)\
  EEA(mapply_unary,name)\
  EEA(mapply_unary,arg1)\
  EEA(mapply_binary,name)\
  EEA(mapply_binary,arg1)\
  EEA(mapply_binary,arg2)\
  EEA(mapply_ternary,name)\
  EEA(mapply_ternary,arg1)\
  EEA(mapply_ternary,arg2)\
  EEA(mapply_ternary,arg3)\
  EEA(number,lexval)\
  EEA(number,scalingunit)\
  EEA(number,cast)\
  EEA(function,lexval)\
  EEA(function,unique_id)\
  EEA(function,definition)\
  EEA(function,class)\
  EEA(function,arguments)\
  EEA(variable,prototype)\
  EEA(variable,array)\
  EEA(array,variable)\
  EEA(array,index)\
  EEA(probe,module)\
  EEA(probe,branch)\
  EEA(probe,nature)\
  EEA(probe,seen)\
  EEA(probe,discipline)\
  EEA(probe,grounded)\
  EEA(probe,source)\
  EEA(expression,module)\
  EEA(expression,tree)\
  EEA(expression,hasspecialnumber)\
  EEA(expression,hasVoltageDependentFunction)\
  EEA(expression,infinity)\
  EEA(expression,TemperatureDependent)\
  EEA(expression,value)\
  EEA(expression,function)\
  EEA(expression,probe)\
  EEA(expression,variable)\
  EEA(instance,instantiator)\
  EEA(instance,module)\
  EEA(instance,name)\
  EEA(instance,terminal)\
  EEA(instance,parameterset)\
  EEA(instanceparameter,parameter)\
  EEA(instanceparameter,value)\
  EEA(instancenode,nodefrominstantiator)\
  EEA(instancenode,nodefrommodule)\
  EEA(nilled,module)\
  EEA(nilled,name)\
  EEA(assignment,module)\
  EEA(assignment,lhs)\
  EEA(assignment,rhs)\
  EEA(assignment,lexval)\
  EEA(assignment,name)\
  EEA(assignment,attribute)\
  EEA(contribution,module)\
  EEA(contribution,lhs)\
  EEA(contribution,rhs)\
  EEA(contribution,lexval)\
  EEA(contribution,branchalias)\
  EEA(contribution,name)\
  EEA(contribution,whitenoise)\
  EEA(contribution,flickernoise)\
  EEA(contribution,attribute)\
  EEA(conditional,module)\
  EEA(conditional,if)\
  EEA(conditional,then)\
  EEA(conditional,else)\
  EEA(conditional,name)\
  EEA(forloop,module)\
  EEA(forloop,initial)\
  EEA(forloop,condition)\
  EEA(forloop,update)\
  EEA(forloop,forblock)\
  EEA(forloop,name)\
  EEA(whileloop,module)\
  EEA(whileloop,while)\
  EEA(whileloop,whileblock)\
  EEA(whileloop,name)\
  EEA(case,module)\
  EEA(case,case)\
  EEA(case,name)\
  EEA(case,caseitem)\
  EEA(caseitem,code)\
  EEA(caseitem,defaultcase)\
  EEA(caseitem,condition)\
  EEA(blockvariable,block)\
  EEA(blockvariable,variable)\
  EEA(block,module)\
  EEA(block,lexval)\
  EEA(block,block)\
  EEA(block,item)\
  EEA(block,attribute)\
  EEA(block,variable)\
  EEA(block,probe)\
  EEA(callfunction,module)\
  EEA(callfunction,function)\
  EEA(evaluation,variable)\
  EEA(text,transform)\
  EEA(text,value)\
  EEA(text,aname)\
  EEA(text,admse)\
  EEA(text,token)\
  EEA(path,transform)\
  EEA(path,text)\
  EEA(path,aname)\
  EEA(path,avalue)\
  EEA(path,template)\
  EEA(path,keeplist)\
  EEA(path,callbackname)\
  EEA(path,callback)\
  EEA(path,bar)\
  EEA(path,arg)\
  EEA(path,u)\
  EEA(path,b)\
  EEA(path,c)\
  EEA(ptraverse,transform)\
  EEA(ptraverse,admst)\
  EEA(ptraverse,alladmst)\
  EEA(ptraverse,aliasadmst)\
  EEA(ptraverse,position)\
  EEA(ptraverse,a1)\
  EEA(ptraverse,an)\
  EEA(pparse,transform)\
  EEA(pparse,aname)\
  EEA(pparse,value)\
  EEA(pparse,path)\
  EEA(pparse,text)\
  EEA(pparse,position)\
  EEA(pparse,tkstart)\
  EEA(pparse,tklen)\
  EEA(pparse,tkid)\
  EEA(admst,previous)\
  EEA(admst,previouspath)\
  EEA(admst,pseudo)\
  EEA(admst,item)\
  EEA(admst,valueto)\
  EEA(admst,position)\
  EEA(admst,count)\
  EEA(admst,hasnewbasicstring)\
  EEA(admst,original)\
  EEA(admst,refd)\
  EEA(admst,true)\
  EEA(admst,preva)\
  EEA(admst,nexta)\
  EEA(admst,selected)\
  EEA(transform,fullfilename)\
  EEA(transform,basefilename)\
  EEA(transform,l)\
  EEA(transform,name)\
  EEA(transform,parent)\
  EEA(transform,callback)\
  EEA(transform,id)\
  EEA(transform,pathselect)\
  EEA(transform,pathpath)\
  EEA(transform,pathinto)\
  EEA(transform,pathtest)\
  EEA(transform,pathlist)\
  EEA(transform,textselect)\
  EEA(transform,textdatatype)\
  EEA(transform,textmatch)\
  EEA(transform,textrequired)\
  EEA(transform,textseparator)\
  EEA(transform,textfrom)\
  EEA(transform,textto)\
  EEA(transform,textlist)\
  EEA(transform,textname)\
  EEA(transform,textformat)\
  EEA(transform,textfile)\
  EEA(transform,textversion)\
  EEA(transform,textonduplicate)\
  EEA(transform,textprobe)\
  EEA(transform,textstring)\
  EEA(transform,templates)\
  EEA(transform,textarguments)\
  EEA(transform,pathoncompare)\
  EEA(transform,pathinputs)\
  EEA(transform,attribute)\
  EEA(transform,children)\
  EEA(itransform,template)\
  EEA(itransform,originalcall)\
  EEA(itransform,variable)\
  EEA(itransform,return)\
  EEA(return,name)\
  EEA(return,value)\
  EEA(attribute,name)\
  EEA(attribute,value)\
  EEA(admstvariable,name)\
  EEA(admstvariable,value)\
  EEA(lexval,string)\
  EEA(lexval,f)\
  EEA(lexval,l)\
  EEA(lexval,c)\
  EEA(yaccval,filename)\
  EEA(yaccval,usrdata)\
/*fin*/

#define TK\
  TTK(adms)\
  TTK(simulator)\
  TTK(admsmain)\
  TTK(list)\
  TTK(nature)\
  TTK(discipline)\
  TTK(module)\
  TTK(analogfunction)\
  TTK(nodealias)\
  TTK(node)\
  TTK(branchalias)\
  TTK(branch)\
  TTK(variableprototype)\
  TTK(source)\
  TTK(range)\
  TTK(jacobian)\
  TTK(analog)\
  TTK(math)\
  TTK(string)\
  TTK(subexpression)\
  TTK(mapply_unary)\
  TTK(mapply_binary)\
  TTK(mapply_ternary)\
  TTK(number)\
  TTK(function)\
  TTK(variable)\
  TTK(array)\
  TTK(probe)\
  TTK(expression)\
  TTK(instance)\
  TTK(instanceparameter)\
  TTK(instancenode)\
  TTK(nilled)\
  TTK(assignment)\
  TTK(contribution)\
  TTK(conditional)\
  TTK(forloop)\
  TTK(whileloop)\
  TTK(case)\
  TTK(caseitem)\
  TTK(blockvariable)\
  TTK(block)\
  TTK(callfunction)\
  TTK(evaluation)\
  TTK(text)\
  TTK(path)\
  TTK(ptraverse)\
  TTK(pparse)\
  TTK(admst)\
  TTK(transform)\
  TTK(itransform)\
  TTK(return)\
  TTK(attribute)\
  TTK(admstvariable)\
  TTK(lexval)\
  TTK(yaccval)\
  TTK(datatypename)\
  TTK(name)\
  TTK(currentdate)\
  TTK(developer)\
  TTK(fullname)\
  TTK(package_name)\
  TTK(package_tarname)\
  TTK(package_version)\
  TTK(package_string)\
  TTK(package_bugreport)\
  TTK(tmp)\
  TTK(tmp1)\
  TTK(br1)\
  TTK(br2)\
  TTK(sr1)\
  TTK(ddx)\
  TTK(argc)\
  TTK(fullfilename)\
  TTK(filename)\
  TTK(curfilename)\
  TTK(curline)\
  TTK(fpos)\
  TTK(info)\
  TTK(usage)\
  TTK(verbose)\
  TTK(hint)\
  TTK(warning)\
  TTK(obsolete)\
  TTK(debug)\
  TTK(dbg_vla)\
  TTK(dbg_xml)\
  TTK(error)\
  TTK(fatal)\
  TTK(dbg_mem)\
  TTK(argv)\
  TTK(invtransform)\
  TTK(itransforminsideapplytemplate)\
  TTK(valueof)\
  TTK(datatype)\
  TTK(item)\
  TTK(access)\
  TTK(abstol)\
  TTK(base)\
  TTK(ddt_name)\
  TTK(ddt_nature)\
  TTK(idt_name)\
  TTK(idt_nature)\
  TTK(units)\
  TTK(flow)\
  TTK(potential)\
  TTK(domain)\
  TTK(instantiator)\
  TTK(type)\
  TTK(tree)\
  TTK(direction)\
  TTK(grounded)\
  TTK(location)\
  TTK(pnode)\
  TTK(nnode)\
  TTK(dependency)\
  TTK(static)\
  TTK(dynamic)\
  TTK(sizetype)\
  TTK(parametertype)\
  TTK(maxsize)\
  TTK(minsize)\
  TTK(input)\
  TTK(output)\
  TTK(scope)\
  TTK(default)\
  TTK(derivate)\
  TTK(isstate)\
  TTK(usedinmodel)\
  TTK(usedininstance)\
  TTK(usedininitial_step)\
  TTK(usedinevaluate)\
  TTK(usedinnoise)\
  TTK(usedinfinal)\
  TTK(setinmodel)\
  TTK(setininstance)\
  TTK(setininitial_step)\
  TTK(setinevaluate)\
  TTK(setinnoise)\
  TTK(setinfinal)\
  TTK(setingetprev)\
  TTK(TemperatureDependent)\
  TTK(OPdependent)\
  TTK(insource)\
  TTK(vcount)\
  TTK(vlast)\
  TTK(arraydefault)\
  TTK(alias)\
  TTK(ddxprobe)\
  TTK(whitenoise)\
  TTK(flickernoise)\
  TTK(switch)\
  TTK(infexpr)\
  TTK(supexpr)\
  TTK(infboundtype)\
  TTK(supboundtype)\
  TTK(row)\
  TTK(column)\
  TTK(diagonal)\
  TTK(code)\
  TTK(value)\
  TTK(arg1)\
  TTK(arg2)\
  TTK(arg3)\
  TTK(scalingunit)\
  TTK(cast)\
  TTK(unique_id)\
  TTK(definition)\
  TTK(class)\
  TTK(arguments)\
  TTK(prototype)\
  TTK(index)\
  TTK(seen)\
  TTK(hasspecialnumber)\
  TTK(hasVoltageDependentFunction)\
  TTK(infinity)\
  TTK(terminal)\
  TTK(parameterset)\
  TTK(parameter)\
  TTK(nodefrominstantiator)\
  TTK(nodefrommodule)\
  TTK(lhs)\
  TTK(rhs)\
  TTK(if)\
  TTK(then)\
  TTK(else)\
  TTK(initial)\
  TTK(condition)\
  TTK(update)\
  TTK(forblock)\
  TTK(while)\
  TTK(whileblock)\
  TTK(defaultcase)\
  TTK(aname)\
  TTK(admse)\
  TTK(token)\
  TTK(avalue)\
  TTK(template)\
  TTK(keeplist)\
  TTK(callbackname)\
  TTK(callback)\
  TTK(bar)\
  TTK(arg)\
  TTK(u)\
  TTK(b)\
  TTK(c)\
  TTK(alladmst)\
  TTK(aliasadmst)\
  TTK(position)\
  TTK(a1)\
  TTK(an)\
  TTK(tkstart)\
  TTK(tklen)\
  TTK(tkid)\
  TTK(previous)\
  TTK(previouspath)\
  TTK(pseudo)\
  TTK(valueto)\
  TTK(count)\
  TTK(hasnewbasicstring)\
  TTK(original)\
  TTK(refd)\
  TTK(true)\
  TTK(preva)\
  TTK(nexta)\
  TTK(selected)\
  TTK(basefilename)\
  TTK(l)\
  TTK(parent)\
  TTK(id)\
  TTK(pathselect)\
  TTK(pathpath)\
  TTK(pathinto)\
  TTK(pathtest)\
  TTK(pathlist)\
  TTK(textselect)\
  TTK(textdatatype)\
  TTK(textmatch)\
  TTK(textrequired)\
  TTK(textseparator)\
  TTK(textfrom)\
  TTK(textto)\
  TTK(textlist)\
  TTK(textname)\
  TTK(textformat)\
  TTK(textfile)\
  TTK(textversion)\
  TTK(textonduplicate)\
  TTK(textprobe)\
  TTK(textstring)\
  TTK(templates)\
  TTK(textarguments)\
  TTK(pathoncompare)\
  TTK(pathinputs)\
  TTK(children)\
  TTK(originalcall)\
  TTK(f)\
  TTK(usrdata)\
  TTK(yes)\
  TTK(no)\
  TTK(continuous)\
  TTK(discrete)\
  TTK(real)\
  TTK(integer)\
  TTK(inout)\
  TTK(internal)\
  TTK(ground)\
  TTK(external)\
  TTK(constant)\
  TTK(noprobe)\
  TTK(linear)\
  TTK(nonlinear)\
  TTK(scalar)\
  TTK(model)\
  TTK(local)\
  TTK(global_model)\
  TTK(global_instance)\
  TTK(global_final)\
  TTK(global_past)\
  TTK(shot)\
  TTK(thermal)\
  TTK(flicker)\
  TTK(fs)\
  TTK(ps)\
  TTK(range_bound_include)\
  TTK(range_bound_exclude)\
  TTK(range_bound_value)\
  TTK(include)\
  TTK(include_value)\
  TTK(exclude)\
  TTK(exclude_value)\
  TTK(plus)\
  TTK(minus)\
  TTK(not)\
  TTK(bw_not)\
  TTK(bw_equr)\
  TTK(bw_equl)\
  TTK(bw_xor)\
  TTK(bw_or)\
  TTK(bw_and)\
  TTK(or)\
  TTK(and)\
  TTK(equ)\
  TTK(notequ)\
  TTK(lt)\
  TTK(lt_equ)\
  TTK(gt)\
  TTK(gt_equ)\
  TTK(shiftr)\
  TTK(shiftl)\
  TTK(addp)\
  TTK(addm)\
  TTK(multtime)\
  TTK(multdiv)\
  TTK(multmod)\
  TTK(E)\
  TTK(P)\
  TTK(T)\
  TTK(G)\
  TTK(M)\
  TTK(k)\
  TTK(h)\
  TTK(D)\
  TTK(1)\
  TTK(d)\
  TTK(m)\
  TTK(n)\
  TTK(A)\
  TTK(p)\
  TTK(a)\
  TTK(i)\
  TTK(builtin)\
  TTK(digital)\
  TTK(is_zero)\
  TTK(is_one)\
  TTK(is_neg_one)\
  TTK(is_any)\
  TTK(past_name)\
  TTK(datatypefamily)\
  TTK(absolute)\
  TTK(basicenumeration)\
  TTK(basicinteger)\
  TTK(basicreal)\
  TTK(basicstring)\
  TTK(basiclist)\
  TTK(empty)\
/*fin*/


#undef TTK
#define TTK(tk) admse_##tk,
typedef enum {
  admse__zero=0,
  admse__any,
  admse__constant,
  admse__p, /*%p*/
  admse__s, /*%s*/
  admse__path,
  admse__arguments,
  admse__croix,
  admse__croixcroix,
  admse__ladms,
  admse__ladmst,
  TK
  admse__
} admse;
#undef win32_interface
#if defined(WIN32)
#  if defined(insideElement)
#    define win32_interface __declspec(dllexport)
#  else
#    define win32_interface __declspec(dllimport)
#  endif
#else
#  define win32_interface extern
#endif

typedef struct s_adms  t_adms;
typedef t_adms*        p_adms;
typedef const p_adms   p_kadms;
typedef struct s_simulator  t_simulator;
typedef t_simulator*        p_simulator;
typedef const p_simulator   p_ksimulator;
typedef struct s_admsmain  t_admsmain;
typedef t_admsmain*        p_admsmain;
typedef const p_admsmain   p_kadmsmain;
typedef struct s_list  t_list;
typedef t_list*        p_list;
typedef const p_list   p_klist;
typedef struct s_nature  t_nature;
typedef t_nature*        p_nature;
typedef const p_nature   p_knature;
typedef struct s_discipline  t_discipline;
typedef t_discipline*        p_discipline;
typedef const p_discipline   p_kdiscipline;
typedef struct s_module  t_module;
typedef t_module*        p_module;
typedef const p_module   p_kmodule;
typedef struct s_analogfunction  t_analogfunction;
typedef t_analogfunction*        p_analogfunction;
typedef const p_analogfunction   p_kanalogfunction;
typedef struct s_nodealias  t_nodealias;
typedef t_nodealias*        p_nodealias;
typedef const p_nodealias   p_knodealias;
typedef struct s_node  t_node;
typedef t_node*        p_node;
typedef const p_node   p_knode;
typedef struct s_branchalias  t_branchalias;
typedef t_branchalias*        p_branchalias;
typedef const p_branchalias   p_kbranchalias;
typedef struct s_branch  t_branch;
typedef t_branch*        p_branch;
typedef const p_branch   p_kbranch;
typedef struct s_variableprototype  t_variableprototype;
typedef t_variableprototype*        p_variableprototype;
typedef const p_variableprototype   p_kvariableprototype;
typedef struct s_source  t_source;
typedef t_source*        p_source;
typedef const p_source   p_ksource;
typedef struct s_range  t_range;
typedef t_range*        p_range;
typedef const p_range   p_krange;
typedef struct s_jacobian  t_jacobian;
typedef t_jacobian*        p_jacobian;
typedef const p_jacobian   p_kjacobian;
typedef struct s_analog  t_analog;
typedef t_analog*        p_analog;
typedef const p_analog   p_kanalog;
typedef struct s_math  t_math;
typedef t_math*        p_math;
typedef const p_math   p_kmath;
typedef struct s_string  t_string;
typedef t_string*        p_string;
typedef const p_string   p_kstring;
typedef struct s_subexpression  t_subexpression;
typedef t_subexpression*        p_subexpression;
typedef const p_subexpression   p_ksubexpression;
typedef struct s_mapply_unary  t_mapply_unary;
typedef t_mapply_unary*        p_mapply_unary;
typedef const p_mapply_unary   p_kmapply_unary;
typedef struct s_mapply_binary  t_mapply_binary;
typedef t_mapply_binary*        p_mapply_binary;
typedef const p_mapply_binary   p_kmapply_binary;
typedef struct s_mapply_ternary  t_mapply_ternary;
typedef t_mapply_ternary*        p_mapply_ternary;
typedef const p_mapply_ternary   p_kmapply_ternary;
typedef struct s_number  t_number;
typedef t_number*        p_number;
typedef const p_number   p_knumber;
typedef struct s_function  t_function;
typedef t_function*        p_function;
typedef const p_function   p_kfunction;
typedef struct s_variable  t_variable;
typedef t_variable*        p_variable;
typedef const p_variable   p_kvariable;
typedef struct s_array  t_array;
typedef t_array*        p_array;
typedef const p_array   p_karray;
typedef struct s_probe  t_probe;
typedef t_probe*        p_probe;
typedef const p_probe   p_kprobe;
typedef struct s_expression  t_expression;
typedef t_expression*        p_expression;
typedef const p_expression   p_kexpression;
typedef struct s_instance  t_instance;
typedef t_instance*        p_instance;
typedef const p_instance   p_kinstance;
typedef struct s_instanceparameter  t_instanceparameter;
typedef t_instanceparameter*        p_instanceparameter;
typedef const p_instanceparameter   p_kinstanceparameter;
typedef struct s_instancenode  t_instancenode;
typedef t_instancenode*        p_instancenode;
typedef const p_instancenode   p_kinstancenode;
typedef struct s_nilled  t_nilled;
typedef t_nilled*        p_nilled;
typedef const p_nilled   p_knilled;
typedef struct s_assignment  t_assignment;
typedef t_assignment*        p_assignment;
typedef const p_assignment   p_kassignment;
typedef struct s_contribution  t_contribution;
typedef t_contribution*        p_contribution;
typedef const p_contribution   p_kcontribution;
typedef struct s_conditional  t_conditional;
typedef t_conditional*        p_conditional;
typedef const p_conditional   p_kconditional;
typedef struct s_forloop  t_forloop;
typedef t_forloop*        p_forloop;
typedef const p_forloop   p_kforloop;
typedef struct s_whileloop  t_whileloop;
typedef t_whileloop*        p_whileloop;
typedef const p_whileloop   p_kwhileloop;
typedef struct s_case  t_case;
typedef t_case*        p_case;
typedef const p_case   p_kcase;
typedef struct s_caseitem  t_caseitem;
typedef t_caseitem*        p_caseitem;
typedef const p_caseitem   p_kcaseitem;
typedef struct s_blockvariable  t_blockvariable;
typedef t_blockvariable*        p_blockvariable;
typedef const p_blockvariable   p_kblockvariable;
typedef struct s_block  t_block;
typedef t_block*        p_block;
typedef const p_block   p_kblock;
typedef struct s_callfunction  t_callfunction;
typedef t_callfunction*        p_callfunction;
typedef const p_callfunction   p_kcallfunction;
typedef struct s_evaluation  t_evaluation;
typedef t_evaluation*        p_evaluation;
typedef const p_evaluation   p_kevaluation;
typedef struct s_text  t_text;
typedef t_text*        p_text;
typedef const p_text   p_ktext;
typedef struct s_path  t_path;
typedef t_path*        p_path;
typedef const p_path   p_kpath;
typedef struct s_ptraverse  t_ptraverse;
typedef t_ptraverse*        p_ptraverse;
typedef const p_ptraverse   p_kptraverse;
typedef struct s_pparse  t_pparse;
typedef t_pparse*        p_pparse;
typedef const p_pparse   p_kpparse;
typedef struct s_admst  t_admst;
typedef t_admst*        p_admst;
typedef const p_admst   p_kadmst;
typedef struct s_transform  t_transform;
typedef t_transform*        p_transform;
typedef const p_transform   p_ktransform;
typedef struct s_itransform  t_itransform;
typedef t_itransform*        p_itransform;
typedef const p_itransform   p_kitransform;
typedef struct s_return  t_return;
typedef t_return*        p_return;
typedef const p_return   p_kreturn;
typedef struct s_attribute  t_attribute;
typedef t_attribute*        p_attribute;
typedef const p_attribute   p_kattribute;
typedef struct s_admstvariable  t_admstvariable;
typedef t_admstvariable*        p_admstvariable;
typedef const p_admstvariable   p_kadmstvariable;
typedef struct s_lexval  t_lexval;
typedef t_lexval*        p_lexval;
typedef const p_lexval   p_klexval;
typedef struct s_yaccval  t_yaccval;
typedef t_yaccval*        p_yaccval;
typedef const p_yaccval   p_kyaccval;

/*miscH*/
typedef struct s_slist t_slist;
typedef t_slist* p_slist;
win32_interface FILE *stdadmstdbgimpl;
win32_interface FILE *stdadmstdbgimpl;
#define stdadmstdbg (stdadmstdbgimpl?stdadmstdbgimpl:stderr)
win32_interface int admsmain(const char*xcode,const char*xflag,const char*vacode);
win32_interface int adms_global_nbadmstnew(void);
win32_interface int adms_global_nbadmstdestroy(void);
win32_interface void adms_global_increment_nbadmstnew(void);
win32_interface void adms_global_increment_nbadmstdestroy(void);
/*-- admst/new prototypes --*/
win32_interface p_admst adms_admst_newbd (p_admst myprevious,p_admst mypreviouspath,const admse item);
win32_interface p_admst adms_admst_newbe (p_admst myprevious,p_admst mypreviouspath,const admse item);
win32_interface p_admst adms_admst_newbi (p_admst myprevious,p_admst mypreviouspath,const int item);
win32_interface p_admst adms_admst_newbr (p_admst myprevious,p_admst mypreviouspath,const double item);
win32_interface p_admst adms_admst_newbs (p_admst myprevious,p_admst mypreviouspath,char* item);
win32_interface p_admst adms_admst_newns (p_admst myprevious,p_admst mypreviouspath,char* item);
win32_interface p_admst adms_admst_newks (char* item);
win32_interface p_admst adms_admst_newpa (p_admst myprevious,p_admst mypreviouspath,const p_adms item);
win32_interface p_admst adms_admst_newpn (p_admst myprevious,p_admst mypreviouspath,p_admst item);
win32_interface p_admst adms_admst_newpc (p_admst myprevious,p_admst mypreviouspath,p_attribute item);
win32_interface p_admst adms_admst_newpd (p_admst myprevious,p_admst mypreviouspath,p_admstvariable item);
win32_interface p_admst adms_admst_newla (p_admst myprevious,p_admst mypreviouspath,p_slist item);
win32_interface p_admst adms_admst_newln (p_admst myprevious,p_admst mypreviouspath,p_slist item);
win32_interface p_admst adms_admst_newpp (p_admst myprevious,p_admst mypreviouspath,admse item);

/*-- Miscellaneous routines --*/

win32_interface const double adms_dzero;
win32_interface double adms_NAN;
win32_interface p_admst aread (p_admst myadmst);
win32_interface void deref(p_admst mystack);
union admsu
{
  int i;
  double r;
  char* s;
  p_adms p;
};
win32_interface const int minusone;

win32_interface int adms_strtol (p_ktransform mytransform,const char* mynumber);
win32_interface double adms_strtod (p_ktransform mytransform,const char* myreal);
win32_interface void bp(void);
win32_interface char*adms_integertostring(int value);
win32_interface char*adms_doubletostring(double value);
win32_interface int adms_setenv(const char* myname,const char* myvalue);
win32_interface int adms_file_isregular(const char* myfilename);
win32_interface int adms_file_isdirectory(const char* myfilename);
win32_interface p_slist adms_slist_new (p_adms d);
win32_interface p_slist adms_slist_copy (p_slist l);
win32_interface void adms_slist_concat (p_slist* l1,p_slist l2);
win32_interface p_slist adms_slist_find (p_slist l,p_kadms data);
win32_interface void adms_slist_free (p_slist l);
win32_interface int adms_slist_index (p_slist l, p_kadms data);
win32_interface p_slist adms_slist_last (p_slist l);
win32_interface unsigned int adms_slist_length (p_slist l);
win32_interface p_slist adms_slist_nth (p_slist l,unsigned int  n);
win32_interface p_adms adms_slist_nth_data (p_slist l,unsigned int n);
win32_interface void adms_slist_print(const char* message,p_slist l);
win32_interface p_adms adms_slist_pull(p_slist* l);
win32_interface void adms_slist_push(p_slist* l,p_adms data);
win32_interface p_slist adms_slist_reverse (p_slist l);
win32_interface void adms_slist_inreverse (p_slist* l);
struct s_slist
{
  p_adms data;
  p_slist next;
};


win32_interface p_admsmain root(void);
win32_interface void rootnew(p_admsmain myglobaladmsmain);

typedef p_admst (t_new) (p_ktransform mytransform,p_admst dot,p_slist arguments[],const int size);
typedef p_admst (*p_new) (p_ktransform mytransform,p_admst dot,p_slist arguments[],const int size);
#define _t_new(function) p_admst (function) (p_ktransform mytransform,p_admst dot,p_slist arguments[],const int size)

typedef int (t_cmp) (p_adms myadms, p_adms myadmsref);
typedef int (*p_cmp) (p_adms myadms, p_adms myadmsref);
#define _t_cmp(function) int (function) (p_adms myadms, p_adms myadmsref)

/*-- Messaging --*/
typedef void (t_message) (const char*format, ...);
typedef void (*p_message) (const char*format, ...);
#define _t_message(function)  void (function) (const char*format, ...)

win32_interface void adms_toupper(char*m);
win32_interface void adms_tolower(char*m);
win32_interface char*adms_kclone(const char* m);
win32_interface char*adms_knclone(const char* m,const int l);
win32_interface char*adms_m2nclone(const char* m,const char* n);
win32_interface void adms_strconcat(char **s1,char *s2);
win32_interface void adms_k2strconcat(char **s1,const char* s2);
win32_interface FILE *adms_file_open_read (const char* fileName);
win32_interface t_message adms_message_info_impl;
win32_interface t_message adms_message_info_continue_impl;
#define adms_message_info(VAARGS)\
{\
  if((root())\
    &&(root()->_info==admse_yes))\
    adms_message_info_impl VAARGS;\
}
#define adms_message_info_continue(VAARGS)\
{\
  if((root())\
    &&(root()->_info==admse_yes))\
    adms_message_info_continue_impl VAARGS;\
}
win32_interface t_message adms_message_usage_impl;
win32_interface t_message adms_message_usage_continue_impl;
#define adms_message_usage(VAARGS)\
{\
  if((root())\
    &&(root()->_usage==admse_yes))\
    adms_message_usage_impl VAARGS;\
}
#define adms_message_usage_continue(VAARGS)\
{\
  if((root())\
    &&(root()->_usage==admse_yes))\
    adms_message_usage_continue_impl VAARGS;\
}
win32_interface t_message adms_message_verbose_impl;
win32_interface t_message adms_message_verbose_continue_impl;
#define adms_message_verbose(VAARGS)\
{\
  if((root())\
    &&(root()->_verbose==admse_yes))\
    adms_message_verbose_impl VAARGS;\
}
#define adms_message_verbose_continue(VAARGS)\
{\
  if((root())\
    &&(root()->_verbose==admse_yes))\
    adms_message_verbose_continue_impl VAARGS;\
}
win32_interface t_message adms_message_debug_impl;
win32_interface t_message adms_message_debug_continue_impl;
#define adms_message_debug(VAARGS)\
{\
  if((root())\
    &&(root()->_debug==admse_yes))\
    adms_message_debug_impl VAARGS;\
}
#define adms_message_debug_continue(VAARGS)\
{\
  if((root())\
    &&(root()->_debug==admse_yes))\
    adms_message_debug_continue_impl VAARGS;\
}
win32_interface t_message adms_message_dbg_vla_impl;
win32_interface t_message adms_message_dbg_vla_continue_impl;
#define adms_message_dbg_vla(VAARGS)\
{\
  if((root())\
    &&(root()->_dbg_vla==admse_yes))\
    adms_message_dbg_vla_impl VAARGS;\
}
#define adms_message_dbg_vla_continue(VAARGS)\
{\
  if((root())\
    &&(root()->_dbg_vla==admse_yes))\
    adms_message_dbg_vla_continue_impl VAARGS;\
}
win32_interface t_message adms_message_dbg_xml_impl;
win32_interface t_message adms_message_dbg_xml_continue_impl;
#define adms_message_dbg_xml(VAARGS)\
{\
  if((root())\
    &&(root()->_dbg_xml==admse_yes))\
    adms_message_dbg_xml_impl VAARGS;\
}
#define adms_message_dbg_xml_continue(VAARGS)\
{\
  if((root())\
    &&(root()->_dbg_xml==admse_yes))\
    adms_message_dbg_xml_continue_impl VAARGS;\
}
win32_interface t_message adms_message_dbg_mem_impl;
win32_interface t_message adms_message_dbg_mem_continue_impl;
#define adms_message_dbg_mem(VAARGS)\
{\
  if((root())\
    &&(root()->_dbg_mem==admse_yes))\
    adms_message_dbg_mem_impl VAARGS;\
}
#define adms_message_dbg_mem_continue(VAARGS)\
{\
  if((root())\
    &&(root()->_dbg_mem==admse_yes))\
    adms_message_dbg_mem_continue_impl VAARGS;\
}
win32_interface t_message adms_message_hint_impl;
win32_interface t_message adms_message_hint_continue_impl;
#define adms_message_hint(VAARGS)\
{\
  if((root())\
    &&(root()->_hint==admse_yes))\
    adms_message_hint_impl VAARGS;\
}
#define adms_message_hint_continue(VAARGS)\
{\
  if((root())\
    &&(root()->_hint==admse_yes))\
    adms_message_hint_continue_impl VAARGS;\
}
win32_interface t_message adms_message_warning_impl;
win32_interface t_message adms_message_warning_continue_impl;
#define adms_message_warning(VAARGS)\
{\
  if((root())\
    &&(root()->_warning==admse_yes))\
    adms_message_warning_impl VAARGS;\
}
#define adms_message_warning_continue(VAARGS)\
{\
  if((root())\
    &&(root()->_warning==admse_yes))\
    adms_message_warning_continue_impl VAARGS;\
}
win32_interface t_message adms_message_obsolete_impl;
win32_interface t_message adms_message_obsolete_continue_impl;
#define adms_message_obsolete(VAARGS)\
{\
  if((root())\
    &&(root()->_obsolete==admse_yes))\
    adms_message_obsolete_impl VAARGS;\
}
#define adms_message_obsolete_continue(VAARGS)\
{\
  if((root())\
    &&(root()->_obsolete==admse_yes))\
    adms_message_obsolete_continue_impl VAARGS;\
}
win32_interface t_message adms_message_error_impl;
win32_interface t_message adms_message_error_continue_impl;
#define adms_message_error(VAARGS)\
{\
  if((root())\
    &&(root()->_error==admse_yes))\
    adms_message_error_impl VAARGS;\
}
#define adms_message_error_continue(VAARGS)\
{\
  if((root())\
    &&(root()->_error==admse_yes))\
    adms_message_error_continue_impl VAARGS;\
}
win32_interface t_message adms_message_fatal_impl;
win32_interface t_message adms_message_fatal_continue_impl;
#define adms_message_fatal(VAARGS)\
{\
  if((root())\
    &&(root()->_fatal==admse_yes))\
    adms_message_fatal_impl VAARGS;\
}
#define adms_message_fatal_continue(VAARGS)\
{\
  if((root())\
    &&(root()->_fatal==admse_yes))\
    adms_message_fatal_continue_impl VAARGS;\
}
win32_interface t_message adms_message_admstdbg_impl;
win32_interface t_message adms_message_admstdbg_continue_impl;
#define adms_message_admstdbg(VAARGS)\
{\
  if((root())\
    &&(root()->_admstdbg==admse_yes))\
    adms_message_admstdbg_impl VAARGS;\
}
#define adms_message_admstdbg_continue(VAARGS)\
{\
  if((root())\
    &&(root()->_admstdbg==admse_yes))\
    adms_message_admstdbg_continue_impl VAARGS;\
}

win32_interface char* aprintf (p_ktransform mytransform,p_kadmst myadmst);

/* ------- adms -------------- */
#define adms_adms(member) ((p_adms)(member))
struct s_adms {
  admse _datatypename;
  p_slist _attribute;
  p_slist _variable;
};
win32_interface char*adms_adms_uid (p_adms myadms);
win32_interface void adms_adms_valueto_datatypename (p_adms myadms,int _datatypename);
win32_interface void adms_adms_valueto_attribute (p_adms myadms,p_slist _attribute);
win32_interface void adms_adms_valueto_variable (p_adms myadms,p_slist _variable);

/* ------- simulator -------------- */
#define adms_simulator(member) ((p_simulator)(member))
struct s_simulator {
  t_adms _adms;
  char* _name;
  int _name_isdefault : 2;
  char* _currentdate;
  int _currentdate_isdefault : 2;
  char* _developer;
  int _developer_isdefault : 2;
  char* _fullname;
  int _fullname_isdefault : 2;
  char* _package_name;
  int _package_name_isdefault : 2;
  char* _package_tarname;
  int _package_tarname_isdefault : 2;
  char* _package_version;
  int _package_version_isdefault : 2;
  char* _package_string;
  int _package_string_isdefault : 2;
  char* _package_bugreport;
  int _package_bugreport_isdefault : 2;
  p_probe _probe;
  int _probe_isdefault : 2;
  char* _tmp;
  int _tmp_isdefault : 2;
  char* _tmp1;
  int _tmp1_isdefault : 2;
  p_branch _br1;
  int _br1_isdefault : 2;
  p_branch _br2;
  int _br2_isdefault : 2;
  p_source _sr1;
  int _sr1_isdefault : 2;
  char* _ddx;
  int _ddx_isdefault : 2;
};
win32_interface char*adms_simulator_uid (p_simulator mysimulator);
win32_interface p_simulator adms_simulator_new (const char* myname);
win32_interface void adms_simulator_free(p_simulator mysimulator);
win32_interface void adms_simulator_valueto_name (p_simulator mysimulator,const char* _name);
win32_interface void adms_simulator_valueto_currentdate (p_simulator mysimulator,const char* _currentdate);
win32_interface void adms_simulator_valueto_developer (p_simulator mysimulator,const char* _developer);
win32_interface void adms_simulator_valueto_fullname (p_simulator mysimulator,const char* _fullname);
win32_interface void adms_simulator_valueto_package_name (p_simulator mysimulator,const char* _package_name);
win32_interface void adms_simulator_valueto_package_tarname (p_simulator mysimulator,const char* _package_tarname);
win32_interface void adms_simulator_valueto_package_version (p_simulator mysimulator,const char* _package_version);
win32_interface void adms_simulator_valueto_package_string (p_simulator mysimulator,const char* _package_string);
win32_interface void adms_simulator_valueto_package_bugreport (p_simulator mysimulator,const char* _package_bugreport);
win32_interface void adms_simulator_valueto_probe (p_simulator mysimulator,p_probe _probe);
win32_interface void adms_simulator_valueto_tmp (p_simulator mysimulator,const char* _tmp);
win32_interface void adms_simulator_valueto_tmp1 (p_simulator mysimulator,const char* _tmp1);
win32_interface void adms_simulator_valueto_br1 (p_simulator mysimulator,p_branch _br1);
win32_interface void adms_simulator_valueto_br2 (p_simulator mysimulator,p_branch _br2);
win32_interface void adms_simulator_valueto_sr1 (p_simulator mysimulator,p_source _sr1);
win32_interface void adms_simulator_valueto_ddx (p_simulator mysimulator,const char* _ddx);

/* ------- admsmain -------------- */
#define adms_admsmain(member) ((p_admsmain)(member))
struct s_admsmain {
  t_adms _adms;
  char* _name;
  int _name_isdefault : 2;
  int _argc;
  int _argc_isdefault : 2;
  char* _fullfilename;
  int _fullfilename_isdefault : 2;
  char* _filename;
  int _filename_isdefault : 2;
  char* _curfilename;
  int _curfilename_isdefault : 2;
  int _curline;
  int _curline_isdefault : 2;
  int _fpos;
  int _fpos_isdefault : 2;
  p_simulator _simulator;
  int _simulator_isdefault : 2;
  admse _info;
  int _info_isdefault : 2;
  admse _usage;
  int _usage_isdefault : 2;
  admse _verbose;
  int _verbose_isdefault : 2;
  admse _hint;
  int _hint_isdefault : 2;
  admse _warning;
  int _warning_isdefault : 2;
  admse _obsolete;
  int _obsolete_isdefault : 2;
  admse _debug;
  int _debug_isdefault : 2;
  admse _dbg_vla;
  int _dbg_vla_isdefault : 2;
  admse _dbg_xml;
  int _dbg_xml_isdefault : 2;
  admse _error;
  int _error_isdefault : 2;
  admse _fatal;
  int _fatal_isdefault : 2;
  admse _dbg_mem;
  int _dbg_mem_isdefault : 2;
  p_slist _module;
  int _module_isdefault : 2;
  p_slist _discipline;
  int _discipline_isdefault : 2;
  p_slist _nature;
  int _nature_isdefault : 2;
  p_slist _variable;
  int _variable_isdefault : 2;
  p_slist _argv;
  int _argv_isdefault : 2;
  p_slist _transform;
  int _transform_isdefault : 2;
  p_slist _invtransform;
  int _invtransform_isdefault : 2;
  p_slist _itransform;
  int _itransform_isdefault : 2;
  p_slist _itransforminsideapplytemplate;
  int _itransforminsideapplytemplate_isdefault : 2;
  p_slist _valueof;
  int _valueof_isdefault : 2;
};
win32_interface char*adms_admsmain_uid (p_admsmain myadmsmain);
win32_interface p_admsmain adms_admsmain_new (const char* myname);
win32_interface void adms_admsmain_free(p_admsmain myadmsmain);
win32_interface void adms_admsmain_valueto_name (p_admsmain myadmsmain,const char* _name);
win32_interface void adms_admsmain_valueto_argc (p_admsmain myadmsmain,int _argc);
win32_interface void adms_admsmain_valueto_fullfilename (p_admsmain myadmsmain,const char* _fullfilename);
win32_interface void adms_admsmain_valueto_filename (p_admsmain myadmsmain,const char* _filename);
win32_interface void adms_admsmain_valueto_curfilename (p_admsmain myadmsmain,const char* _curfilename);
win32_interface void adms_admsmain_valueto_curline (p_admsmain myadmsmain,int _curline);
win32_interface void adms_admsmain_valueto_fpos (p_admsmain myadmsmain,int _fpos);
win32_interface void adms_admsmain_valueto_simulator (p_admsmain myadmsmain,p_simulator _simulator);
win32_interface void adms_admsmain_valueto_info (p_admsmain myadmsmain,admse _info);
win32_interface void adms_admsmain_valueto_usage (p_admsmain myadmsmain,admse _usage);
win32_interface void adms_admsmain_valueto_verbose (p_admsmain myadmsmain,admse _verbose);
win32_interface void adms_admsmain_valueto_hint (p_admsmain myadmsmain,admse _hint);
win32_interface void adms_admsmain_valueto_warning (p_admsmain myadmsmain,admse _warning);
win32_interface void adms_admsmain_valueto_obsolete (p_admsmain myadmsmain,admse _obsolete);
win32_interface void adms_admsmain_valueto_debug (p_admsmain myadmsmain,admse _debug);
win32_interface void adms_admsmain_valueto_dbg_vla (p_admsmain myadmsmain,admse _dbg_vla);
win32_interface void adms_admsmain_valueto_dbg_xml (p_admsmain myadmsmain,admse _dbg_xml);
win32_interface void adms_admsmain_valueto_error (p_admsmain myadmsmain,admse _error);
win32_interface void adms_admsmain_valueto_fatal (p_admsmain myadmsmain,admse _fatal);
win32_interface void adms_admsmain_valueto_dbg_mem (p_admsmain myadmsmain,admse _dbg_mem);
win32_interface void adms_admsmain_valueto_module (p_admsmain myadmsmain,p_slist _module);
win32_interface p_module adms_admsmain_list_module_prepend_by_id_once_or_abort (p_admsmain mymyadmsmain,const char* myname);
win32_interface p_module adms_admsmain_list_module_lookup_by_id (p_admsmain mymyadmsmain,const char* myname);
win32_interface void adms_admsmain_valueto_discipline (p_admsmain myadmsmain,p_slist _discipline);
win32_interface void adms_admsmain_list_discipline_prepend_once_or_abort (p_admsmain mymyadmsmain,p_discipline mydiscipline);
win32_interface p_discipline adms_admsmain_list_discipline_lookup_by_id (p_admsmain mymyadmsmain,const char* myname);
win32_interface void adms_admsmain_valueto_nature (p_admsmain myadmsmain,p_slist _nature);
win32_interface p_nature adms_admsmain_list_nature_prepend_by_id_once_or_abort (p_admsmain mymyadmsmain,const char* myaccess);
win32_interface p_nature adms_admsmain_list_nature_lookup_by_id (p_admsmain mymyadmsmain,const char* myaccess);
win32_interface void adms_admsmain_valueto_variable (p_admsmain myadmsmain,p_slist _variable);
win32_interface void adms_admsmain_valueto_argv (p_admsmain myadmsmain,p_slist _argv);
win32_interface void adms_admsmain_valueto_transform (p_admsmain myadmsmain,p_slist _transform);
win32_interface void adms_admsmain_valueto_invtransform (p_admsmain myadmsmain,p_slist _invtransform);
win32_interface void adms_admsmain_valueto_itransform (p_admsmain myadmsmain,p_slist _itransform);
win32_interface void adms_admsmain_valueto_itransforminsideapplytemplate (p_admsmain myadmsmain,p_slist _itransforminsideapplytemplate);
win32_interface void adms_admsmain_valueto_valueof (p_admsmain myadmsmain,p_slist _valueof);

/* ------- list -------------- */
#define adms_list(member) ((p_list)(member))
struct s_list {
  t_adms _adms;
  char* _datatype;
  int _datatype_isdefault : 2;
  p_slist _item;
  int _item_isdefault : 2;
};
win32_interface char*adms_list_uid (p_list mylist);
win32_interface p_list adms_list_new (const char* mydatatype);
win32_interface void adms_list_free(p_list mylist);
win32_interface void adms_list_valueto_datatype (p_list mylist,const char* _datatype);
win32_interface void adms_list_valueto_item (p_list mylist,p_slist _item);

/* ------- nature -------------- */
#define adms_nature(member) ((p_nature)(member))
struct s_nature {
  t_adms _adms;
  char* _access;
  int _access_isdefault : 2;
  char* _name;
  int _name_isdefault : 2;
  p_number _abstol;
  int _abstol_isdefault : 2;
  p_nature _base;
  int _base_isdefault : 2;
  char* _ddt_name;
  int _ddt_name_isdefault : 2;
  p_nature _ddt_nature;
  int _ddt_nature_isdefault : 2;
  char* _idt_name;
  int _idt_name_isdefault : 2;
  p_nature _idt_nature;
  int _idt_nature_isdefault : 2;
  char* _units;
  int _units_isdefault : 2;
};
win32_interface char*adms_nature_uid (p_nature mynature);
win32_interface p_nature adms_nature_new (const char* myaccess);
win32_interface void adms_nature_free(p_nature mynature);
win32_interface void adms_nature_valueto_access (p_nature mynature,const char* _access);
win32_interface void adms_nature_valueto_name (p_nature mynature,const char* _name);
win32_interface void adms_nature_valueto_abstol (p_nature mynature,p_number _abstol);
win32_interface void adms_nature_valueto_base (p_nature mynature,p_nature _base);
win32_interface void adms_nature_valueto_ddt_name (p_nature mynature,const char* _ddt_name);
win32_interface void adms_nature_valueto_ddt_nature (p_nature mynature,p_nature _ddt_nature);
win32_interface void adms_nature_valueto_idt_name (p_nature mynature,const char* _idt_name);
win32_interface void adms_nature_valueto_idt_nature (p_nature mynature,p_nature _idt_nature);
win32_interface void adms_nature_valueto_units (p_nature mynature,const char* _units);

/* ------- discipline -------------- */
#define adms_discipline(member) ((p_discipline)(member))
struct s_discipline {
  t_adms _adms;
  char* _name;
  int _name_isdefault : 2;
  p_nature _flow;
  int _flow_isdefault : 2;
  p_nature _potential;
  int _potential_isdefault : 2;
  admse _domain;
  int _domain_isdefault : 2;
};
win32_interface char*adms_discipline_uid (p_discipline mydiscipline);
win32_interface p_discipline adms_discipline_new (const char* myname);
win32_interface void adms_discipline_free(p_discipline mydiscipline);
win32_interface void adms_discipline_valueto_name (p_discipline mydiscipline,const char* _name);
win32_interface void adms_discipline_valueto_flow (p_discipline mydiscipline,p_nature _flow);
win32_interface void adms_discipline_valueto_potential (p_discipline mydiscipline,p_nature _potential);
win32_interface void adms_discipline_valueto_domain (p_discipline mydiscipline,admse _domain);

/* ------- module -------------- */
#define adms_module(member) ((p_module)(member))
struct s_module {
  t_adms _adms;
  char* _name;
  int _name_isdefault : 2;
  p_slist _node;
  int _node_isdefault : 2;
  p_slist _nodealias;
  int _nodealias_isdefault : 2;
  p_slist _branch;
  int _branch_isdefault : 2;
  p_slist _branchalias;
  int _branchalias_isdefault : 2;
  p_slist _analogfunction;
  int _analogfunction_isdefault : 2;
  p_slist _instance;
  int _instance_isdefault : 2;
  p_slist _variable;
  int _variable_isdefault : 2;
  p_analog _analog;
  int _analog_isdefault : 2;
  p_slist _block;
  int _block_isdefault : 2;
  p_slist _blockvariable;
  int _blockvariable_isdefault : 2;
  p_slist _assignment;
  int _assignment_isdefault : 2;
  p_slist _callfunction;
  int _callfunction_isdefault : 2;
  p_slist _contribution;
  int _contribution_isdefault : 2;
  p_slist _conditional;
  int _conditional_isdefault : 2;
  p_slist _case;
  int _case_isdefault : 2;
  p_slist _forloop;
  int _forloop_isdefault : 2;
  p_slist _whileloop;
  int _whileloop_isdefault : 2;
  p_slist _instantiator;
  int _instantiator_isdefault : 2;
  p_slist _expression;
  int _expression_isdefault : 2;
  p_slist _jacobian;
  int _jacobian_isdefault : 2;
  p_slist _probe;
  int _probe_isdefault : 2;
  p_slist _source;
  int _source_isdefault : 2;
  p_slist _range;
  int _range_isdefault : 2;
  p_slist _attribute;
  int _attribute_isdefault : 2;
  p_evaluation _evaluation;
  int _evaluation_isdefault : 2;
};
win32_interface char*adms_module_uid (p_module mymodule);
win32_interface p_module adms_module_new (const char* myname);
win32_interface void adms_module_free(p_module mymodule);
win32_interface void adms_module_valueto_name (p_module mymodule,const char* _name);
win32_interface void adms_module_valueto_node (p_module mymodule,p_slist _node);
win32_interface p_node adms_module_list_node_prepend_by_id_once_or_ignore (p_module mymymodule,p_module mymodule,const char* myname);
win32_interface p_node adms_module_list_node_prepend_by_id_once_or_abort (p_module mymymodule,p_module mymodule,const char* myname);
win32_interface p_node adms_module_list_node_lookup_by_id (p_module mymymodule,p_module mymodule,const char* myname);
win32_interface void adms_module_valueto_nodealias (p_module mymodule,p_slist _nodealias);
win32_interface p_nodealias adms_module_list_nodealias_prepend_by_id_once_or_ignore (p_module mymymodule,p_module mymodule,const char* myname);
win32_interface p_nodealias adms_module_list_nodealias_prepend_by_id_once_or_abort (p_module mymymodule,p_module mymodule,const char* myname);
win32_interface p_nodealias adms_module_list_nodealias_lookup_by_id (p_module mymymodule,p_module mymodule,const char* myname);
win32_interface void adms_module_valueto_branch (p_module mymodule,p_slist _branch);
win32_interface p_branch adms_module_list_branch_prepend_by_id_once_or_ignore (p_module mymymodule,p_module mymodule,p_node mypnode,p_node mynnode);
win32_interface void adms_module_valueto_branchalias (p_module mymodule,p_slist _branchalias);
win32_interface p_branchalias adms_module_list_branchalias_prepend_by_id_once_or_ignore (p_module mymymodule,p_module mymodule,const char* myname);
win32_interface p_branchalias adms_module_list_branchalias_prepend_by_id_once_or_abort (p_module mymymodule,p_module mymodule,const char* myname);
win32_interface p_branchalias adms_module_list_branchalias_lookup_by_id (p_module mymymodule,p_module mymodule,const char* myname);
win32_interface void adms_module_valueto_analogfunction (p_module mymodule,p_slist _analogfunction);
win32_interface void adms_module_list_analogfunction_prepend_once_or_abort (p_module mymymodule,p_analogfunction myanalogfunction);
win32_interface void adms_module_valueto_instance (p_module mymodule,p_slist _instance);
win32_interface p_instance adms_module_list_instance_prepend_by_id_once_or_abort (p_module mymymodule,p_module myinstantiator,p_module mymodule,const char* myname);
win32_interface void adms_module_valueto_variable (p_module mymodule,p_slist _variable);
win32_interface void adms_module_list_variable_prepend_once_or_abort (p_module mymymodule,p_variableprototype myvariable);
win32_interface p_variableprototype adms_module_list_variable_lookup_by_id (p_module mymymodule,p_module mymodule,p_lexval mylexval,p_adms myblock);
win32_interface void adms_module_valueto_analog (p_module mymodule,p_analog _analog);
win32_interface void adms_module_valueto_block (p_module mymodule,p_slist _block);
win32_interface void adms_module_valueto_blockvariable (p_module mymodule,p_slist _blockvariable);
win32_interface void adms_module_list_blockvariable_prepend_once_or_abort (p_module mymymodule,p_blockvariable myblockvariable);
win32_interface p_blockvariable adms_module_list_blockvariable_lookup_by_id (p_module mymymodule,p_block myblock);
win32_interface void adms_module_valueto_assignment (p_module mymodule,p_slist _assignment);
win32_interface void adms_module_valueto_callfunction (p_module mymodule,p_slist _callfunction);
win32_interface void adms_module_valueto_contribution (p_module mymodule,p_slist _contribution);
win32_interface void adms_module_valueto_conditional (p_module mymodule,p_slist _conditional);
win32_interface void adms_module_valueto_case (p_module mymodule,p_slist _case);
win32_interface void adms_module_valueto_forloop (p_module mymodule,p_slist _forloop);
win32_interface void adms_module_valueto_whileloop (p_module mymodule,p_slist _whileloop);
win32_interface void adms_module_valueto_instantiator (p_module mymodule,p_slist _instantiator);
win32_interface void adms_module_valueto_expression (p_module mymodule,p_slist _expression);
win32_interface void adms_module_valueto_jacobian (p_module mymodule,p_slist _jacobian);
win32_interface void adms_module_valueto_probe (p_module mymodule,p_slist _probe);
win32_interface p_probe adms_module_list_probe_prepend_by_id_once_or_ignore (p_module mymymodule,p_module mymodule,p_branch mybranch,p_nature mynature);
win32_interface void adms_module_valueto_source (p_module mymodule,p_slist _source);
win32_interface p_source adms_module_list_source_prepend_by_id_once_or_ignore (p_module mymymodule,p_module mymodule,p_branch mybranch,p_nature mynature);
win32_interface void adms_module_valueto_range (p_module mymodule,p_slist _range);
win32_interface p_range adms_module_list_range_prepend_by_id_once_or_abort (p_module mymymodule,p_module mymodule,p_expression myinfexpr,p_expression mysupexpr);
win32_interface void adms_module_valueto_attribute (p_module mymodule,p_slist _attribute);
win32_interface void adms_module_valueto_evaluation (p_module mymodule,p_evaluation _evaluation);

/* ------- analogfunction -------------- */
#define adms_analogfunction(member) ((p_analogfunction)(member))
struct s_analogfunction {
  t_adms _adms;
  p_module _module;
  int _module_isdefault : 2;
  p_lexval _lexval;
  int _lexval_isdefault : 2;
  admse _type;
  int _type_isdefault : 2;
  p_adms _tree;
  int _tree_isdefault : 2;
  p_slist _variable;
  int _variable_isdefault : 2;
  p_slist _attribute;
  int _attribute_isdefault : 2;
};
win32_interface char*adms_analogfunction_uid (p_analogfunction myanalogfunction);
win32_interface p_analogfunction adms_analogfunction_new (p_module mymodule,p_lexval mylexval);
win32_interface void adms_analogfunction_free(p_analogfunction myanalogfunction);
win32_interface void adms_analogfunction_valueto_module (p_analogfunction myanalogfunction,p_module _module);
win32_interface void adms_analogfunction_valueto_lexval (p_analogfunction myanalogfunction,p_lexval _lexval);
win32_interface void adms_analogfunction_valueto_type (p_analogfunction myanalogfunction,admse _type);
win32_interface void adms_analogfunction_valueto_tree (p_analogfunction myanalogfunction,p_adms _tree);
win32_interface void adms_analogfunction_valueto_variable (p_analogfunction myanalogfunction,p_slist _variable);
win32_interface void adms_analogfunction_list_variable_prepend_once_or_abort (p_analogfunction mymyanalogfunction,p_variableprototype myvariable);
win32_interface p_variableprototype adms_analogfunction_list_variable_lookup_by_id (p_analogfunction mymyanalogfunction,p_module mymodule,p_lexval mylexval,p_adms myblock);
win32_interface void adms_analogfunction_valueto_attribute (p_analogfunction myanalogfunction,p_slist _attribute);

/* ------- nodealias -------------- */
#define adms_nodealias(member) ((p_nodealias)(member))
struct s_nodealias {
  t_adms _adms;
  p_module _module;
  int _module_isdefault : 2;
  char* _name;
  int _name_isdefault : 2;
  p_node _node;
  int _node_isdefault : 2;
};
win32_interface char*adms_nodealias_uid (p_nodealias mynodealias);
win32_interface p_nodealias adms_nodealias_new (p_module mymodule,const char* myname);
win32_interface void adms_nodealias_free(p_nodealias mynodealias);
win32_interface void adms_nodealias_valueto_module (p_nodealias mynodealias,p_module _module);
win32_interface void adms_nodealias_valueto_name (p_nodealias mynodealias,const char* _name);
win32_interface void adms_nodealias_valueto_node (p_nodealias mynodealias,p_node _node);

/* ------- node -------------- */
#define adms_node(member) ((p_node)(member))
struct s_node {
  t_adms _adms;
  p_module _module;
  int _module_isdefault : 2;
  char* _name;
  int _name_isdefault : 2;
  admse _direction;
  int _direction_isdefault : 2;
  p_discipline _discipline;
  int _discipline_isdefault : 2;
  admse _grounded;
  int _grounded_isdefault : 2;
  admse _location;
  int _location_isdefault : 2;
  p_slist _attribute;
  int _attribute_isdefault : 2;
};
win32_interface char*adms_node_uid (p_node mynode);
win32_interface p_node adms_node_new (p_module mymodule,const char* myname);
win32_interface void adms_node_free(p_node mynode);
win32_interface void adms_node_valueto_module (p_node mynode,p_module _module);
win32_interface void adms_node_valueto_name (p_node mynode,const char* _name);
win32_interface void adms_node_valueto_direction (p_node mynode,admse _direction);
win32_interface void adms_node_valueto_discipline (p_node mynode,p_discipline _discipline);
win32_interface void adms_node_valueto_grounded (p_node mynode,admse _grounded);
win32_interface void adms_node_valueto_location (p_node mynode,admse _location);
win32_interface void adms_node_valueto_attribute (p_node mynode,p_slist _attribute);

/* ------- branchalias -------------- */
#define adms_branchalias(member) ((p_branchalias)(member))
struct s_branchalias {
  t_adms _adms;
  p_module _module;
  int _module_isdefault : 2;
  char* _name;
  int _name_isdefault : 2;
  p_branch _branch;
  int _branch_isdefault : 2;
};
win32_interface char*adms_branchalias_uid (p_branchalias mybranchalias);
win32_interface p_branchalias adms_branchalias_new (p_module mymodule,const char* myname);
win32_interface void adms_branchalias_free(p_branchalias mybranchalias);
win32_interface void adms_branchalias_valueto_module (p_branchalias mybranchalias,p_module _module);
win32_interface void adms_branchalias_valueto_name (p_branchalias mybranchalias,const char* _name);
win32_interface void adms_branchalias_valueto_branch (p_branchalias mybranchalias,p_branch _branch);

/* ------- branch -------------- */
#define adms_branch(member) ((p_branch)(member))
struct s_branch {
  t_adms _adms;
  p_module _module;
  int _module_isdefault : 2;
  p_node _pnode;
  int _pnode_isdefault : 2;
  p_node _nnode;
  int _nnode_isdefault : 2;
  p_discipline _discipline;
  int _discipline_isdefault : 2;
  admse _grounded;
  int _grounded_isdefault : 2;
  p_slist _attribute;
  int _attribute_isdefault : 2;
};
win32_interface char*adms_branch_uid (p_branch mybranch);
win32_interface p_branch adms_branch_new (p_module mymodule,p_node mypnode,p_node mynnode);
win32_interface void adms_branch_free(p_branch mybranch);
win32_interface void adms_branch_valueto_module (p_branch mybranch,p_module _module);
win32_interface void adms_branch_valueto_pnode (p_branch mybranch,p_node _pnode);
win32_interface void adms_branch_valueto_nnode (p_branch mybranch,p_node _nnode);
win32_interface void adms_branch_valueto_discipline (p_branch mybranch,p_discipline _discipline);
win32_interface void adms_branch_valueto_grounded (p_branch mybranch,admse _grounded);
win32_interface void adms_branch_valueto_attribute (p_branch mybranch,p_slist _attribute);

/* ------- variableprototype -------------- */
#define adms_variableprototype(member) ((p_variableprototype)(member))
struct s_variableprototype {
  t_adms _adms;
  p_module _module;
  int _module_isdefault : 2;
  p_lexval _lexval;
  int _lexval_isdefault : 2;
  p_adms _block;
  int _block_isdefault : 2;
  admse _dependency;
  int _dependency_isdefault : 2;
  admse _static;
  int _static_isdefault : 2;
  admse _dynamic;
  int _dynamic_isdefault : 2;
  admse _sizetype;
  int _sizetype_isdefault : 2;
  admse _parametertype;
  int _parametertype_isdefault : 2;
  admse _type;
  int _type_isdefault : 2;
  p_expression _maxsize;
  int _maxsize_isdefault : 2;
  p_expression _minsize;
  int _minsize_isdefault : 2;
  admse _input;
  int _input_isdefault : 2;
  admse _output;
  int _output_isdefault : 2;
  admse _scope;
  int _scope_isdefault : 2;
  p_expression _default;
  int _default_isdefault : 2;
  admse _derivate;
  int _derivate_isdefault : 2;
  admse _isstate;
  int _isstate_isdefault : 2;
  admse _usedinmodel;
  int _usedinmodel_isdefault : 2;
  admse _usedininstance;
  int _usedininstance_isdefault : 2;
  admse _usedininitial_step;
  int _usedininitial_step_isdefault : 2;
  admse _usedinevaluate;
  int _usedinevaluate_isdefault : 2;
  admse _usedinnoise;
  int _usedinnoise_isdefault : 2;
  admse _usedinfinal;
  int _usedinfinal_isdefault : 2;
  admse _setinmodel;
  int _setinmodel_isdefault : 2;
  admse _setininstance;
  int _setininstance_isdefault : 2;
  admse _setininitial_step;
  int _setininitial_step_isdefault : 2;
  admse _setinevaluate;
  int _setinevaluate_isdefault : 2;
  admse _setinnoise;
  int _setinnoise_isdefault : 2;
  admse _setinfinal;
  int _setinfinal_isdefault : 2;
  admse _setingetprev;
  int _setingetprev_isdefault : 2;
  admse _TemperatureDependent;
  int _TemperatureDependent_isdefault : 2;
  admse _OPdependent;
  int _OPdependent_isdefault : 2;
  admse _insource;
  int _insource_isdefault : 2;
  int _vcount;
  int _vcount_isdefault : 2;
  p_assignment _vlast;
  int _vlast_isdefault : 2;
  p_slist _arraydefault;
  int _arraydefault_isdefault : 2;
  p_slist _attribute;
  int _attribute_isdefault : 2;
  p_slist _alias;
  int _alias_isdefault : 2;
  p_slist _range;
  int _range_isdefault : 2;
  p_slist _probe;
  int _probe_isdefault : 2;
  p_slist _ddxprobe;
  int _ddxprobe_isdefault : 2;
  p_slist _variable;
  int _variable_isdefault : 2;
  p_slist _assignment;
  int _assignment_isdefault : 2;
  p_slist _instance;
  int _instance_isdefault : 2;
};
win32_interface char*adms_variableprototype_uid (p_variableprototype myvariableprototype);
win32_interface p_variableprototype adms_variableprototype_new (p_module mymodule,p_lexval mylexval,p_adms myblock);
win32_interface void adms_variableprototype_free(p_variableprototype myvariableprototype);
win32_interface void adms_variableprototype_valueto_module (p_variableprototype myvariableprototype,p_module _module);
win32_interface void adms_variableprototype_valueto_lexval (p_variableprototype myvariableprototype,p_lexval _lexval);
win32_interface void adms_variableprototype_valueto_block (p_variableprototype myvariableprototype,p_adms _block);
win32_interface void adms_variableprototype_valueto_dependency (p_variableprototype myvariableprototype,admse _dependency);
win32_interface void adms_variableprototype_valueto_static (p_variableprototype myvariableprototype,admse _static);
win32_interface void adms_variableprototype_valueto_dynamic (p_variableprototype myvariableprototype,admse _dynamic);
win32_interface void adms_variableprototype_valueto_sizetype (p_variableprototype myvariableprototype,admse _sizetype);
win32_interface void adms_variableprototype_valueto_parametertype (p_variableprototype myvariableprototype,admse _parametertype);
win32_interface void adms_variableprototype_valueto_type (p_variableprototype myvariableprototype,admse _type);
win32_interface void adms_variableprototype_valueto_maxsize (p_variableprototype myvariableprototype,p_expression _maxsize);
win32_interface void adms_variableprototype_valueto_minsize (p_variableprototype myvariableprototype,p_expression _minsize);
win32_interface void adms_variableprototype_valueto_input (p_variableprototype myvariableprototype,admse _input);
win32_interface void adms_variableprototype_valueto_output (p_variableprototype myvariableprototype,admse _output);
win32_interface void adms_variableprototype_valueto_scope (p_variableprototype myvariableprototype,admse _scope);
win32_interface void adms_variableprototype_valueto_default (p_variableprototype myvariableprototype,p_expression _default);
win32_interface void adms_variableprototype_valueto_derivate (p_variableprototype myvariableprototype,admse _derivate);
win32_interface void adms_variableprototype_valueto_isstate (p_variableprototype myvariableprototype,admse _isstate);
win32_interface void adms_variableprototype_valueto_usedinmodel (p_variableprototype myvariableprototype,admse _usedinmodel);
win32_interface void adms_variableprototype_valueto_usedininstance (p_variableprototype myvariableprototype,admse _usedininstance);
win32_interface void adms_variableprototype_valueto_usedininitial_step (p_variableprototype myvariableprototype,admse _usedininitial_step);
win32_interface void adms_variableprototype_valueto_usedinevaluate (p_variableprototype myvariableprototype,admse _usedinevaluate);
win32_interface void adms_variableprototype_valueto_usedinnoise (p_variableprototype myvariableprototype,admse _usedinnoise);
win32_interface void adms_variableprototype_valueto_usedinfinal (p_variableprototype myvariableprototype,admse _usedinfinal);
win32_interface void adms_variableprototype_valueto_setinmodel (p_variableprototype myvariableprototype,admse _setinmodel);
win32_interface void adms_variableprototype_valueto_setininstance (p_variableprototype myvariableprototype,admse _setininstance);
win32_interface void adms_variableprototype_valueto_setininitial_step (p_variableprototype myvariableprototype,admse _setininitial_step);
win32_interface void adms_variableprototype_valueto_setinevaluate (p_variableprototype myvariableprototype,admse _setinevaluate);
win32_interface void adms_variableprototype_valueto_setinnoise (p_variableprototype myvariableprototype,admse _setinnoise);
win32_interface void adms_variableprototype_valueto_setinfinal (p_variableprototype myvariableprototype,admse _setinfinal);
win32_interface void adms_variableprototype_valueto_setingetprev (p_variableprototype myvariableprototype,admse _setingetprev);
win32_interface void adms_variableprototype_valueto_TemperatureDependent (p_variableprototype myvariableprototype,admse _TemperatureDependent);
win32_interface void adms_variableprototype_valueto_OPdependent (p_variableprototype myvariableprototype,admse _OPdependent);
win32_interface void adms_variableprototype_valueto_insource (p_variableprototype myvariableprototype,admse _insource);
win32_interface void adms_variableprototype_valueto_vcount (p_variableprototype myvariableprototype,int _vcount);
win32_interface void adms_variableprototype_valueto_vlast (p_variableprototype myvariableprototype,p_assignment _vlast);
win32_interface void adms_variableprototype_valueto_arraydefault (p_variableprototype myvariableprototype,p_slist _arraydefault);
win32_interface void adms_variableprototype_valueto_attribute (p_variableprototype myvariableprototype,p_slist _attribute);
win32_interface void adms_variableprototype_valueto_alias (p_variableprototype myvariableprototype,p_slist _alias);
win32_interface void adms_variableprototype_list_alias_prepend_once_or_abort (p_variableprototype mymyvariableprototype,const char* myalias);
win32_interface void adms_variableprototype_valueto_range (p_variableprototype myvariableprototype,p_slist _range);
win32_interface void adms_variableprototype_valueto_probe (p_variableprototype myvariableprototype,p_slist _probe);
win32_interface void adms_variableprototype_valueto_ddxprobe (p_variableprototype myvariableprototype,p_slist _ddxprobe);
win32_interface void adms_variableprototype_valueto_variable (p_variableprototype myvariableprototype,p_slist _variable);
win32_interface void adms_variableprototype_valueto_assignment (p_variableprototype myvariableprototype,p_slist _assignment);
win32_interface void adms_variableprototype_valueto_instance (p_variableprototype myvariableprototype,p_slist _instance);

/* ------- source -------------- */
#define adms_source(member) ((p_source)(member))
struct s_source {
  t_adms _adms;
  p_module _module;
  int _module_isdefault : 2;
  p_branch _branch;
  int _branch_isdefault : 2;
  p_nature _nature;
  int _nature_isdefault : 2;
  p_discipline _discipline;
  int _discipline_isdefault : 2;
  admse _static;
  int _static_isdefault : 2;
  admse _dynamic;
  int _dynamic_isdefault : 2;
  admse _whitenoise;
  int _whitenoise_isdefault : 2;
  admse _flickernoise;
  int _flickernoise_isdefault : 2;
  admse _grounded;
  int _grounded_isdefault : 2;
  admse _switch;
  int _switch_isdefault : 2;
  admse _type;
  int _type_isdefault : 2;
  p_slist _probe;
  int _probe_isdefault : 2;
  p_slist _attribute;
  int _attribute_isdefault : 2;
};
win32_interface char*adms_source_uid (p_source mysource);
win32_interface p_source adms_source_new (p_module mymodule,p_branch mybranch,p_nature mynature);
win32_interface void adms_source_free(p_source mysource);
win32_interface void adms_source_valueto_module (p_source mysource,p_module _module);
win32_interface void adms_source_valueto_branch (p_source mysource,p_branch _branch);
win32_interface void adms_source_valueto_nature (p_source mysource,p_nature _nature);
win32_interface void adms_source_valueto_discipline (p_source mysource,p_discipline _discipline);
win32_interface void adms_source_valueto_static (p_source mysource,admse _static);
win32_interface void adms_source_valueto_dynamic (p_source mysource,admse _dynamic);
win32_interface void adms_source_valueto_whitenoise (p_source mysource,admse _whitenoise);
win32_interface void adms_source_valueto_flickernoise (p_source mysource,admse _flickernoise);
win32_interface void adms_source_valueto_grounded (p_source mysource,admse _grounded);
win32_interface void adms_source_valueto_switch (p_source mysource,admse _switch);
win32_interface void adms_source_valueto_type (p_source mysource,admse _type);
win32_interface void adms_source_valueto_probe (p_source mysource,p_slist _probe);
win32_interface void adms_source_valueto_attribute (p_source mysource,p_slist _attribute);

/* ------- range -------------- */
#define adms_range(member) ((p_range)(member))
struct s_range {
  t_adms _adms;
  p_module _module;
  int _module_isdefault : 2;
  p_expression _infexpr;
  int _infexpr_isdefault : 2;
  p_expression _supexpr;
  int _supexpr_isdefault : 2;
  char* _name;
  int _name_isdefault : 2;
  admse _infboundtype;
  int _infboundtype_isdefault : 2;
  admse _supboundtype;
  int _supboundtype_isdefault : 2;
  admse _type;
  int _type_isdefault : 2;
};
win32_interface char*adms_range_uid (p_range myrange);
win32_interface p_range adms_range_new (p_module mymodule,p_expression myinfexpr,p_expression mysupexpr);
win32_interface void adms_range_free(p_range myrange);
win32_interface void adms_range_valueto_module (p_range myrange,p_module _module);
win32_interface void adms_range_valueto_infexpr (p_range myrange,p_expression _infexpr);
win32_interface void adms_range_valueto_supexpr (p_range myrange,p_expression _supexpr);
win32_interface void adms_range_valueto_name (p_range myrange,const char* _name);
win32_interface void adms_range_valueto_infboundtype (p_range myrange,admse _infboundtype);
win32_interface void adms_range_valueto_supboundtype (p_range myrange,admse _supboundtype);
win32_interface void adms_range_valueto_type (p_range myrange,admse _type);

/* ------- jacobian -------------- */
#define adms_jacobian(member) ((p_jacobian)(member))
struct s_jacobian {
  t_adms _adms;
  p_module _module;
  int _module_isdefault : 2;
  p_node _row;
  int _row_isdefault : 2;
  p_node _column;
  int _column_isdefault : 2;
  admse _diagonal;
  int _diagonal_isdefault : 2;
  admse _static;
  int _static_isdefault : 2;
  admse _dynamic;
  int _dynamic_isdefault : 2;
};
win32_interface char*adms_jacobian_uid (p_jacobian myjacobian);
win32_interface p_jacobian adms_jacobian_new (p_module mymodule,p_node myrow,p_node mycolumn);
win32_interface void adms_jacobian_free(p_jacobian myjacobian);
win32_interface void adms_jacobian_valueto_module (p_jacobian myjacobian,p_module _module);
win32_interface void adms_jacobian_valueto_row (p_jacobian myjacobian,p_node _row);
win32_interface void adms_jacobian_valueto_column (p_jacobian myjacobian,p_node _column);
win32_interface void adms_jacobian_valueto_diagonal (p_jacobian myjacobian,admse _diagonal);
win32_interface void adms_jacobian_valueto_static (p_jacobian myjacobian,admse _static);
win32_interface void adms_jacobian_valueto_dynamic (p_jacobian myjacobian,admse _dynamic);

/* ------- analog -------------- */
#define adms_analog(member) ((p_analog)(member))
struct s_analog {
  t_adms _adms;
  p_adms _code;
  int _code_isdefault : 2;
};
win32_interface char*adms_analog_uid (p_analog myanalog);
win32_interface p_analog adms_analog_new (p_adms mycode);
win32_interface void adms_analog_free(p_analog myanalog);
win32_interface void adms_analog_valueto_code (p_analog myanalog,p_adms _code);

/* ------- math -------------- */
#define adms_math(member) ((p_math)(member))
struct s_math {
  t_adms _adms;
  char* _name;
  int _name_isdefault : 2;
  double _value;
  int _value_isdefault : 2;
  admse _static;
  int _static_isdefault : 2;
  admse _dynamic;
  int _dynamic_isdefault : 2;
  admse _dependency;
  int _dependency_isdefault : 2;
  p_slist _attribute;
  int _attribute_isdefault : 2;
};
win32_interface char*adms_math_uid (p_math mymath);
win32_interface p_math adms_math_new (const char* myname);
win32_interface void adms_math_free(p_math mymath);
win32_interface void adms_math_valueto_name (p_math mymath,const char* _name);
win32_interface void adms_math_valueto_value (p_math mymath,double _value);
win32_interface void adms_math_valueto_static (p_math mymath,admse _static);
win32_interface void adms_math_valueto_dynamic (p_math mymath,admse _dynamic);
win32_interface void adms_math_valueto_dependency (p_math mymath,admse _dependency);
win32_interface void adms_math_valueto_attribute (p_math mymath,p_slist _attribute);

/* ------- string -------------- */
#define adms_string(member) ((p_string)(member))
struct s_string {
  t_math _math;
  char* _value;
  int _value_isdefault : 2;
};
win32_interface char*adms_string_uid (p_string mystring);
win32_interface p_string adms_string_new (const char* myvalue);
win32_interface void adms_string_free(p_string mystring);
win32_interface void adms_string_valueto_value (p_string mystring,const char* _value);

/* ------- subexpression -------------- */
#define adms_subexpression(member) ((p_subexpression)(member))
struct s_subexpression {
  t_math _math;
  p_expression _expression;
  int _expression_isdefault : 2;
};
win32_interface char*adms_subexpression_uid (p_subexpression mysubexpression);
win32_interface p_subexpression adms_subexpression_new (p_expression myexpression);
win32_interface void adms_subexpression_free(p_subexpression mysubexpression);
win32_interface void adms_subexpression_valueto_expression (p_subexpression mysubexpression,p_expression _expression);

/* ------- mapply_unary -------------- */
#define adms_mapply_unary(member) ((p_mapply_unary)(member))
struct s_mapply_unary {
  t_subexpression _subexpression;
  admse _name;
  int _name_isdefault : 2;
  p_adms _arg1;
  int _arg1_isdefault : 2;
};
win32_interface char*adms_mapply_unary_uid (p_mapply_unary mymapply_unary);
win32_interface p_mapply_unary adms_mapply_unary_new (admse myname,p_adms myarg1);
win32_interface void adms_mapply_unary_free(p_mapply_unary mymapply_unary);
win32_interface void adms_mapply_unary_valueto_name (p_mapply_unary mymapply_unary,admse _name);
win32_interface void adms_mapply_unary_valueto_arg1 (p_mapply_unary mymapply_unary,p_adms _arg1);

/* ------- mapply_binary -------------- */
#define adms_mapply_binary(member) ((p_mapply_binary)(member))
struct s_mapply_binary {
  t_subexpression _subexpression;
  admse _name;
  int _name_isdefault : 2;
  p_adms _arg1;
  int _arg1_isdefault : 2;
  p_adms _arg2;
  int _arg2_isdefault : 2;
};
win32_interface char*adms_mapply_binary_uid (p_mapply_binary mymapply_binary);
win32_interface p_mapply_binary adms_mapply_binary_new (admse myname,p_adms myarg1,p_adms myarg2);
win32_interface void adms_mapply_binary_free(p_mapply_binary mymapply_binary);
win32_interface void adms_mapply_binary_valueto_name (p_mapply_binary mymapply_binary,admse _name);
win32_interface void adms_mapply_binary_valueto_arg1 (p_mapply_binary mymapply_binary,p_adms _arg1);
win32_interface void adms_mapply_binary_valueto_arg2 (p_mapply_binary mymapply_binary,p_adms _arg2);

/* ------- mapply_ternary -------------- */
#define adms_mapply_ternary(member) ((p_mapply_ternary)(member))
struct s_mapply_ternary {
  t_subexpression _subexpression;
  admse _name;
  int _name_isdefault : 2;
  p_adms _arg1;
  int _arg1_isdefault : 2;
  p_adms _arg2;
  int _arg2_isdefault : 2;
  p_adms _arg3;
  int _arg3_isdefault : 2;
};
win32_interface char*adms_mapply_ternary_uid (p_mapply_ternary mymapply_ternary);
win32_interface p_mapply_ternary adms_mapply_ternary_new (admse myname,p_adms myarg1,p_adms myarg2,p_adms myarg3);
win32_interface void adms_mapply_ternary_free(p_mapply_ternary mymapply_ternary);
win32_interface void adms_mapply_ternary_valueto_name (p_mapply_ternary mymapply_ternary,admse _name);
win32_interface void adms_mapply_ternary_valueto_arg1 (p_mapply_ternary mymapply_ternary,p_adms _arg1);
win32_interface void adms_mapply_ternary_valueto_arg2 (p_mapply_ternary mymapply_ternary,p_adms _arg2);
win32_interface void adms_mapply_ternary_valueto_arg3 (p_mapply_ternary mymapply_ternary,p_adms _arg3);

/* ------- number -------------- */
#define adms_number(member) ((p_number)(member))
struct s_number {
  t_subexpression _subexpression;
  p_lexval _lexval;
  int _lexval_isdefault : 2;
  admse _scalingunit;
  int _scalingunit_isdefault : 2;
  admse _cast;
  int _cast_isdefault : 2;
};
win32_interface char*adms_number_uid (p_number mynumber);
win32_interface p_number adms_number_new (p_lexval mylexval);
win32_interface void adms_number_free(p_number mynumber);
win32_interface void adms_number_valueto_lexval (p_number mynumber,p_lexval _lexval);
win32_interface void adms_number_valueto_scalingunit (p_number mynumber,admse _scalingunit);
win32_interface void adms_number_valueto_cast (p_number mynumber,admse _cast);

/* ------- function -------------- */
#define adms_function(member) ((p_function)(member))
struct s_function {
  t_subexpression _subexpression;
  p_lexval _lexval;
  int _lexval_isdefault : 2;
  int _unique_id;
  int _unique_id_isdefault : 2;
  p_analogfunction _definition;
  int _definition_isdefault : 2;
  admse _class;
  int _class_isdefault : 2;
  p_slist _arguments;
  int _arguments_isdefault : 2;
};
win32_interface char*adms_function_uid (p_function myfunction);
win32_interface p_function adms_function_new (p_lexval mylexval,int myunique_id);
win32_interface void adms_function_free(p_function myfunction);
win32_interface void adms_function_valueto_lexval (p_function myfunction,p_lexval _lexval);
win32_interface void adms_function_valueto_unique_id (p_function myfunction,int _unique_id);
win32_interface void adms_function_valueto_definition (p_function myfunction,p_analogfunction _definition);
win32_interface void adms_function_valueto_class (p_function myfunction,admse _class);
win32_interface void adms_function_valueto_arguments (p_function myfunction,p_slist _arguments);

/* ------- variable -------------- */
#define adms_variable(member) ((p_variable)(member))
struct s_variable {
  t_subexpression _subexpression;
  p_variableprototype _prototype;
  int _prototype_isdefault : 2;
  p_array _array;
  int _array_isdefault : 2;
};
win32_interface char*adms_variable_uid (p_variable myvariable);
win32_interface p_variable adms_variable_new (p_variableprototype myprototype);
win32_interface void adms_variable_free(p_variable myvariable);
win32_interface void adms_variable_valueto_prototype (p_variable myvariable,p_variableprototype _prototype);
win32_interface void adms_variable_valueto_array (p_variable myvariable,p_array _array);

/* ------- array -------------- */
#define adms_array(member) ((p_array)(member))
struct s_array {
  t_subexpression _subexpression;
  p_variable _variable;
  int _variable_isdefault : 2;
  p_adms _index;
  int _index_isdefault : 2;
};
win32_interface char*adms_array_uid (p_array myarray);
win32_interface p_array adms_array_new (p_variable myvariable,p_adms myindex);
win32_interface void adms_array_free(p_array myarray);
win32_interface void adms_array_valueto_variable (p_array myarray,p_variable _variable);
win32_interface void adms_array_valueto_index (p_array myarray,p_adms _index);

/* ------- probe -------------- */
#define adms_probe(member) ((p_probe)(member))
struct s_probe {
  t_subexpression _subexpression;
  p_module _module;
  int _module_isdefault : 2;
  p_branch _branch;
  int _branch_isdefault : 2;
  p_nature _nature;
  int _nature_isdefault : 2;
  admse _seen;
  int _seen_isdefault : 2;
  p_discipline _discipline;
  int _discipline_isdefault : 2;
  admse _grounded;
  int _grounded_isdefault : 2;
  p_source _source;
  int _source_isdefault : 2;
};
win32_interface char*adms_probe_uid (p_probe myprobe);
win32_interface p_probe adms_probe_new (p_module mymodule,p_branch mybranch,p_nature mynature);
win32_interface void adms_probe_free(p_probe myprobe);
win32_interface void adms_probe_valueto_module (p_probe myprobe,p_module _module);
win32_interface void adms_probe_valueto_branch (p_probe myprobe,p_branch _branch);
win32_interface void adms_probe_valueto_nature (p_probe myprobe,p_nature _nature);
win32_interface void adms_probe_valueto_seen (p_probe myprobe,admse _seen);
win32_interface void adms_probe_valueto_discipline (p_probe myprobe,p_discipline _discipline);
win32_interface void adms_probe_valueto_grounded (p_probe myprobe,admse _grounded);
win32_interface void adms_probe_valueto_source (p_probe myprobe,p_source _source);

/* ------- expression -------------- */
#define adms_expression(member) ((p_expression)(member))
struct s_expression {
  t_math _math;
  p_module _module;
  int _module_isdefault : 2;
  p_adms _tree;
  int _tree_isdefault : 2;
  char* _hasspecialnumber;
  int _hasspecialnumber_isdefault : 2;
  admse _hasVoltageDependentFunction;
  int _hasVoltageDependentFunction_isdefault : 2;
  admse _infinity;
  int _infinity_isdefault : 2;
  admse _TemperatureDependent;
  int _TemperatureDependent_isdefault : 2;
  admse _value;
  int _value_isdefault : 2;
  p_slist _function;
  int _function_isdefault : 2;
  p_slist _probe;
  int _probe_isdefault : 2;
  p_slist _variable;
  int _variable_isdefault : 2;
};
win32_interface char*adms_expression_uid (p_expression myexpression);
win32_interface p_expression adms_expression_new (p_module mymodule,p_adms mytree);
win32_interface void adms_expression_free(p_expression myexpression);
win32_interface void adms_expression_valueto_module (p_expression myexpression,p_module _module);
win32_interface void adms_expression_valueto_tree (p_expression myexpression,p_adms _tree);
win32_interface void adms_expression_valueto_hasspecialnumber (p_expression myexpression,const char* _hasspecialnumber);
win32_interface void adms_expression_valueto_hasVoltageDependentFunction (p_expression myexpression,admse _hasVoltageDependentFunction);
win32_interface void adms_expression_valueto_infinity (p_expression myexpression,admse _infinity);
win32_interface void adms_expression_valueto_TemperatureDependent (p_expression myexpression,admse _TemperatureDependent);
win32_interface void adms_expression_valueto_value (p_expression myexpression,admse _value);
win32_interface void adms_expression_valueto_function (p_expression myexpression,p_slist _function);
win32_interface void adms_expression_valueto_probe (p_expression myexpression,p_slist _probe);
win32_interface void adms_expression_valueto_variable (p_expression myexpression,p_slist _variable);

/* ------- instance -------------- */
#define adms_instance(member) ((p_instance)(member))
struct s_instance {
  t_math _math;
  p_module _instantiator;
  int _instantiator_isdefault : 2;
  p_module _module;
  int _module_isdefault : 2;
  char* _name;
  int _name_isdefault : 2;
  p_slist _terminal;
  int _terminal_isdefault : 2;
  p_slist _parameterset;
  int _parameterset_isdefault : 2;
};
win32_interface char*adms_instance_uid (p_instance myinstance);
win32_interface p_instance adms_instance_new (p_module myinstantiator,p_module mymodule,const char* myname);
win32_interface void adms_instance_free(p_instance myinstance);
win32_interface void adms_instance_valueto_instantiator (p_instance myinstance,p_module _instantiator);
win32_interface void adms_instance_valueto_module (p_instance myinstance,p_module _module);
win32_interface void adms_instance_valueto_name (p_instance myinstance,const char* _name);
win32_interface void adms_instance_valueto_terminal (p_instance myinstance,p_slist _terminal);
win32_interface void adms_instance_list_terminal_prepend_once_or_abort (p_instance mymyinstance,p_instancenode myterminal);
win32_interface void adms_instance_valueto_parameterset (p_instance myinstance,p_slist _parameterset);
win32_interface void adms_instance_list_parameterset_prepend_once_or_abort (p_instance mymyinstance,p_instanceparameter myparameterset);

/* ------- instanceparameter -------------- */
#define adms_instanceparameter(member) ((p_instanceparameter)(member))
struct s_instanceparameter {
  t_math _math;
  p_variableprototype _parameter;
  int _parameter_isdefault : 2;
  p_expression _value;
  int _value_isdefault : 2;
};
win32_interface char*adms_instanceparameter_uid (p_instanceparameter myinstanceparameter);
win32_interface p_instanceparameter adms_instanceparameter_new (p_variableprototype myparameter);
win32_interface void adms_instanceparameter_free(p_instanceparameter myinstanceparameter);
win32_interface void adms_instanceparameter_valueto_parameter (p_instanceparameter myinstanceparameter,p_variableprototype _parameter);
win32_interface void adms_instanceparameter_valueto_value (p_instanceparameter myinstanceparameter,p_expression _value);

/* ------- instancenode -------------- */
#define adms_instancenode(member) ((p_instancenode)(member))
struct s_instancenode {
  t_math _math;
  p_node _nodefrominstantiator;
  int _nodefrominstantiator_isdefault : 2;
  p_node _nodefrommodule;
  int _nodefrommodule_isdefault : 2;
};
win32_interface char*adms_instancenode_uid (p_instancenode myinstancenode);
win32_interface p_instancenode adms_instancenode_new (p_node mynodefrominstantiator,p_node mynodefrommodule);
win32_interface void adms_instancenode_free(p_instancenode myinstancenode);
win32_interface void adms_instancenode_valueto_nodefrominstantiator (p_instancenode myinstancenode,p_node _nodefrominstantiator);
win32_interface void adms_instancenode_valueto_nodefrommodule (p_instancenode myinstancenode,p_node _nodefrommodule);

/* ------- nilled -------------- */
#define adms_nilled(member) ((p_nilled)(member))
struct s_nilled {
  t_math _math;
  p_module _module;
  int _module_isdefault : 2;
  char* _name;
  int _name_isdefault : 2;
};
win32_interface char*adms_nilled_uid (p_nilled mynilled);
win32_interface p_nilled adms_nilled_new (p_module mymodule);
win32_interface void adms_nilled_free(p_nilled mynilled);
win32_interface void adms_nilled_valueto_module (p_nilled mynilled,p_module _module);
win32_interface void adms_nilled_valueto_name (p_nilled mynilled,const char* _name);

/* ------- assignment -------------- */
#define adms_assignment(member) ((p_assignment)(member))
struct s_assignment {
  t_math _math;
  p_module _module;
  int _module_isdefault : 2;
  p_adms _lhs;
  int _lhs_isdefault : 2;
  p_expression _rhs;
  int _rhs_isdefault : 2;
  p_lexval _lexval;
  int _lexval_isdefault : 2;
  char* _name;
  int _name_isdefault : 2;
  p_slist _attribute;
  int _attribute_isdefault : 2;
};
win32_interface char*adms_assignment_uid (p_assignment myassignment);
win32_interface p_assignment adms_assignment_new (p_module mymodule,p_adms mylhs,p_expression myrhs,p_lexval mylexval);
win32_interface void adms_assignment_free(p_assignment myassignment);
win32_interface void adms_assignment_valueto_module (p_assignment myassignment,p_module _module);
win32_interface void adms_assignment_valueto_lhs (p_assignment myassignment,p_adms _lhs);
win32_interface void adms_assignment_valueto_rhs (p_assignment myassignment,p_expression _rhs);
win32_interface void adms_assignment_valueto_lexval (p_assignment myassignment,p_lexval _lexval);
win32_interface void adms_assignment_valueto_name (p_assignment myassignment,const char* _name);
win32_interface void adms_assignment_valueto_attribute (p_assignment myassignment,p_slist _attribute);

/* ------- contribution -------------- */
#define adms_contribution(member) ((p_contribution)(member))
struct s_contribution {
  t_math _math;
  p_module _module;
  int _module_isdefault : 2;
  p_source _lhs;
  int _lhs_isdefault : 2;
  p_expression _rhs;
  int _rhs_isdefault : 2;
  p_lexval _lexval;
  int _lexval_isdefault : 2;
  p_branchalias _branchalias;
  int _branchalias_isdefault : 2;
  char* _name;
  int _name_isdefault : 2;
  admse _whitenoise;
  int _whitenoise_isdefault : 2;
  admse _flickernoise;
  int _flickernoise_isdefault : 2;
  p_slist _attribute;
  int _attribute_isdefault : 2;
};
win32_interface char*adms_contribution_uid (p_contribution mycontribution);
win32_interface p_contribution adms_contribution_new (p_module mymodule,p_source mylhs,p_expression myrhs,p_lexval mylexval);
win32_interface void adms_contribution_free(p_contribution mycontribution);
win32_interface void adms_contribution_valueto_module (p_contribution mycontribution,p_module _module);
win32_interface void adms_contribution_valueto_lhs (p_contribution mycontribution,p_source _lhs);
win32_interface void adms_contribution_valueto_rhs (p_contribution mycontribution,p_expression _rhs);
win32_interface void adms_contribution_valueto_lexval (p_contribution mycontribution,p_lexval _lexval);
win32_interface void adms_contribution_valueto_branchalias (p_contribution mycontribution,p_branchalias _branchalias);
win32_interface void adms_contribution_valueto_name (p_contribution mycontribution,const char* _name);
win32_interface void adms_contribution_valueto_whitenoise (p_contribution mycontribution,admse _whitenoise);
win32_interface void adms_contribution_valueto_flickernoise (p_contribution mycontribution,admse _flickernoise);
win32_interface void adms_contribution_valueto_attribute (p_contribution mycontribution,p_slist _attribute);

/* ------- conditional -------------- */
#define adms_conditional(member) ((p_conditional)(member))
struct s_conditional {
  t_math _math;
  p_module _module;
  int _module_isdefault : 2;
  p_expression _if;
  int _if_isdefault : 2;
  p_adms _then;
  int _then_isdefault : 2;
  p_adms _else;
  int _else_isdefault : 2;
  char* _name;
  int _name_isdefault : 2;
};
win32_interface char*adms_conditional_uid (p_conditional myconditional);
win32_interface p_conditional adms_conditional_new (p_module mymodule,p_expression myif,p_adms mythen,p_adms myelse);
win32_interface void adms_conditional_free(p_conditional myconditional);
win32_interface void adms_conditional_valueto_module (p_conditional myconditional,p_module _module);
win32_interface void adms_conditional_valueto_if (p_conditional myconditional,p_expression _if);
win32_interface void adms_conditional_valueto_then (p_conditional myconditional,p_adms _then);
win32_interface void adms_conditional_valueto_else (p_conditional myconditional,p_adms _else);
win32_interface void adms_conditional_valueto_name (p_conditional myconditional,const char* _name);

/* ------- forloop -------------- */
#define adms_forloop(member) ((p_forloop)(member))
struct s_forloop {
  t_math _math;
  p_module _module;
  int _module_isdefault : 2;
  p_assignment _initial;
  int _initial_isdefault : 2;
  p_expression _condition;
  int _condition_isdefault : 2;
  p_assignment _update;
  int _update_isdefault : 2;
  p_adms _forblock;
  int _forblock_isdefault : 2;
  char* _name;
  int _name_isdefault : 2;
};
win32_interface char*adms_forloop_uid (p_forloop myforloop);
win32_interface p_forloop adms_forloop_new (p_module mymodule,p_assignment myinitial,p_expression mycondition,p_assignment myupdate,p_adms myforblock);
win32_interface void adms_forloop_free(p_forloop myforloop);
win32_interface void adms_forloop_valueto_module (p_forloop myforloop,p_module _module);
win32_interface void adms_forloop_valueto_initial (p_forloop myforloop,p_assignment _initial);
win32_interface void adms_forloop_valueto_condition (p_forloop myforloop,p_expression _condition);
win32_interface void adms_forloop_valueto_update (p_forloop myforloop,p_assignment _update);
win32_interface void adms_forloop_valueto_forblock (p_forloop myforloop,p_adms _forblock);
win32_interface void adms_forloop_valueto_name (p_forloop myforloop,const char* _name);

/* ------- whileloop -------------- */
#define adms_whileloop(member) ((p_whileloop)(member))
struct s_whileloop {
  t_math _math;
  p_module _module;
  int _module_isdefault : 2;
  p_expression _while;
  int _while_isdefault : 2;
  p_adms _whileblock;
  int _whileblock_isdefault : 2;
  char* _name;
  int _name_isdefault : 2;
};
win32_interface char*adms_whileloop_uid (p_whileloop mywhileloop);
win32_interface p_whileloop adms_whileloop_new (p_module mymodule,p_expression mywhile,p_adms mywhileblock);
win32_interface void adms_whileloop_free(p_whileloop mywhileloop);
win32_interface void adms_whileloop_valueto_module (p_whileloop mywhileloop,p_module _module);
win32_interface void adms_whileloop_valueto_while (p_whileloop mywhileloop,p_expression _while);
win32_interface void adms_whileloop_valueto_whileblock (p_whileloop mywhileloop,p_adms _whileblock);
win32_interface void adms_whileloop_valueto_name (p_whileloop mywhileloop,const char* _name);

/* ------- case -------------- */
#define adms_case(member) ((p_case)(member))
struct s_case {
  t_math _math;
  p_module _module;
  int _module_isdefault : 2;
  p_expression _case;
  int _case_isdefault : 2;
  char* _name;
  int _name_isdefault : 2;
  p_slist _caseitem;
  int _caseitem_isdefault : 2;
};
win32_interface char*adms_case_uid (p_case mycase);
win32_interface p_case adms_case_new (p_module mymodule,p_expression mycase);
win32_interface void adms_case_free(p_case mycase);
win32_interface void adms_case_valueto_module (p_case mycase,p_module _module);
win32_interface void adms_case_valueto_case (p_case mycase,p_expression _case);
win32_interface void adms_case_valueto_name (p_case mycase,const char* _name);
win32_interface void adms_case_valueto_caseitem (p_case mycase,p_slist _caseitem);

/* ------- caseitem -------------- */
#define adms_caseitem(member) ((p_caseitem)(member))
struct s_caseitem {
  t_math _math;
  p_adms _code;
  int _code_isdefault : 2;
  admse _defaultcase;
  int _defaultcase_isdefault : 2;
  p_slist _condition;
  int _condition_isdefault : 2;
};
win32_interface char*adms_caseitem_uid (p_caseitem mycaseitem);
win32_interface p_caseitem adms_caseitem_new (p_adms mycode);
win32_interface void adms_caseitem_free(p_caseitem mycaseitem);
win32_interface void adms_caseitem_valueto_code (p_caseitem mycaseitem,p_adms _code);
win32_interface void adms_caseitem_valueto_defaultcase (p_caseitem mycaseitem,admse _defaultcase);
win32_interface void adms_caseitem_valueto_condition (p_caseitem mycaseitem,p_slist _condition);

/* ------- blockvariable -------------- */
#define adms_blockvariable(member) ((p_blockvariable)(member))
struct s_blockvariable {
  t_math _math;
  p_block _block;
  int _block_isdefault : 2;
  p_slist _variable;
  int _variable_isdefault : 2;
};
win32_interface char*adms_blockvariable_uid (p_blockvariable myblockvariable);
win32_interface p_blockvariable adms_blockvariable_new (p_block myblock);
win32_interface void adms_blockvariable_free(p_blockvariable myblockvariable);
win32_interface void adms_blockvariable_valueto_block (p_blockvariable myblockvariable,p_block _block);
win32_interface void adms_blockvariable_valueto_variable (p_blockvariable myblockvariable,p_slist _variable);
win32_interface void adms_blockvariable_list_variable_prepend_once_or_abort (p_blockvariable mymyblockvariable,p_variable myvariable);

/* ------- block -------------- */
#define adms_block(member) ((p_block)(member))
struct s_block {
  t_math _math;
  p_module _module;
  int _module_isdefault : 2;
  p_lexval _lexval;
  int _lexval_isdefault : 2;
  p_block _block;
  int _block_isdefault : 2;
  p_slist _item;
  int _item_isdefault : 2;
  p_slist _attribute;
  int _attribute_isdefault : 2;
  p_slist _variable;
  int _variable_isdefault : 2;
  p_slist _probe;
  int _probe_isdefault : 2;
};
win32_interface char*adms_block_uid (p_block myblock);
win32_interface p_block adms_block_new (p_module mymodule,p_lexval mylexval,p_block myblock,p_slist myitem);
win32_interface void adms_block_free(p_block myblock);
win32_interface void adms_block_valueto_module (p_block myblock,p_module _module);
win32_interface void adms_block_valueto_lexval (p_block myblock,p_lexval _lexval);
win32_interface void adms_block_valueto_block (p_block myblock,p_block _block);
win32_interface void adms_block_valueto_item (p_block myblock,p_slist _item);
win32_interface void adms_block_valueto_attribute (p_block myblock,p_slist _attribute);
win32_interface void adms_block_valueto_variable (p_block myblock,p_slist _variable);
win32_interface void adms_block_list_variable_prepend_once_or_abort (p_block mymyblock,p_variableprototype myvariable);
win32_interface void adms_block_valueto_probe (p_block myblock,p_slist _probe);

/* ------- callfunction -------------- */
#define adms_callfunction(member) ((p_callfunction)(member))
struct s_callfunction {
  t_math _math;
  p_module _module;
  int _module_isdefault : 2;
  p_function _function;
  int _function_isdefault : 2;
};
win32_interface char*adms_callfunction_uid (p_callfunction mycallfunction);
win32_interface p_callfunction adms_callfunction_new (p_module mymodule,p_function myfunction);
win32_interface void adms_callfunction_free(p_callfunction mycallfunction);
win32_interface void adms_callfunction_valueto_module (p_callfunction mycallfunction,p_module _module);
win32_interface void adms_callfunction_valueto_function (p_callfunction mycallfunction,p_function _function);

/* ------- evaluation -------------- */
#define adms_evaluation(member) ((p_evaluation)(member))
struct s_evaluation {
  t_adms _adms;
  p_slist _variable;
  int _variable_isdefault : 2;
};
win32_interface char*adms_evaluation_uid (p_evaluation myevaluation);
win32_interface p_evaluation adms_evaluation_new (void);
win32_interface void adms_evaluation_free(p_evaluation myevaluation);
win32_interface void adms_evaluation_valueto_variable (p_evaluation myevaluation,p_slist _variable);

/* ------- text -------------- */
#define adms_text(member) ((p_text)(member))
struct s_text {
  t_adms _adms;
  p_transform _transform;
  char* _value;
  char* _aname;
  int _admse;
  p_slist _token;
};
win32_interface char*adms_text_uid (p_text mytext);
win32_interface p_text adms_text_new (p_transform mytransform,const char* myvalue);
win32_interface void adms_text_free(p_text mytext);
win32_interface void adms_text_valueto_transform (p_text mytext,p_transform _transform);
win32_interface void adms_text_valueto_value (p_text mytext,const char* _value);
win32_interface void adms_text_valueto_aname (p_text mytext,const char* _aname);
win32_interface void adms_text_valueto_admse (p_text mytext,int _admse);
win32_interface void adms_text_valueto_token (p_text mytext,p_slist _token);

/* ------- path -------------- */
#define adms_path(member) ((p_path)(member))
struct s_path {
  t_adms _adms;
  p_transform _transform;
  p_text _text;
  char* _aname;
  char* _avalue;
  p_transform _template;
  int _keeplist;
  char* _callbackname;
  void* _callback;
  p_slist _bar;
  p_slist _arg;
  p_slist _u;
  p_slist _b;
  p_slist _c;
};
win32_interface char*adms_path_uid (p_path mypath);
win32_interface p_path adms_path_new (p_transform mytransform,p_text mytext);
win32_interface void adms_path_free(p_path mypath);
win32_interface void adms_path_valueto_transform (p_path mypath,p_transform _transform);
win32_interface void adms_path_valueto_text (p_path mypath,p_text _text);
win32_interface void adms_path_valueto_aname (p_path mypath,const char* _aname);
win32_interface void adms_path_valueto_avalue (p_path mypath,const char* _avalue);
win32_interface void adms_path_valueto_template (p_path mypath,p_transform _template);
win32_interface void adms_path_valueto_keeplist (p_path mypath,int _keeplist);
win32_interface void adms_path_valueto_callbackname (p_path mypath,const char* _callbackname);
win32_interface void adms_path_valueto_callback (p_path mypath,void* _callback);
win32_interface void adms_path_valueto_bar (p_path mypath,p_slist _bar);
win32_interface void adms_path_valueto_arg (p_path mypath,p_slist _arg);
win32_interface void adms_path_valueto_u (p_path mypath,p_slist _u);
win32_interface void adms_path_valueto_b (p_path mypath,p_slist _b);
win32_interface void adms_path_valueto_c (p_path mypath,p_slist _c);

/* ------- ptraverse -------------- */
#define adms_ptraverse(member) ((p_ptraverse)(member))
struct s_ptraverse {
  t_adms _adms;
  p_transform _transform;
  p_slist _admst;
  p_slist _alladmst;
  p_slist _aliasadmst;
  int _position;
  p_admst _a1;
  p_admst _an;
};
win32_interface char*adms_ptraverse_uid (p_ptraverse myptraverse);
win32_interface p_ptraverse adms_ptraverse_new (p_transform mytransform);
win32_interface void adms_ptraverse_free(p_ptraverse myptraverse);
win32_interface void adms_ptraverse_valueto_transform (p_ptraverse myptraverse,p_transform _transform);
win32_interface void adms_ptraverse_valueto_admst (p_ptraverse myptraverse,p_slist _admst);
win32_interface void adms_ptraverse_valueto_alladmst (p_ptraverse myptraverse,p_slist _alladmst);
win32_interface void adms_ptraverse_valueto_aliasadmst (p_ptraverse myptraverse,p_slist _aliasadmst);
win32_interface void adms_ptraverse_valueto_position (p_ptraverse myptraverse,int _position);
win32_interface void adms_ptraverse_valueto_a1 (p_ptraverse myptraverse,p_admst _a1);
win32_interface void adms_ptraverse_valueto_an (p_ptraverse myptraverse,p_admst _an);

/* ------- pparse -------------- */
#define adms_pparse(member) ((p_pparse)(member))
struct s_pparse {
  t_adms _adms;
  p_transform _transform;
  char* _aname;
  char* _value;
  p_path _path;
  p_text _text;
  const char* _position;
  const char* _tkstart;
  int _tklen;
  admse _tkid;
};
win32_interface char*adms_pparse_uid (p_pparse mypparse);
win32_interface p_pparse adms_pparse_new (p_transform mytransform,const char* myaname,const char* myvalue);
win32_interface void adms_pparse_free(p_pparse mypparse);
win32_interface void adms_pparse_valueto_transform (p_pparse mypparse,p_transform _transform);
win32_interface void adms_pparse_valueto_aname (p_pparse mypparse,const char* _aname);
win32_interface void adms_pparse_valueto_value (p_pparse mypparse,const char* _value);
win32_interface void adms_pparse_valueto_path (p_pparse mypparse,p_path _path);
win32_interface void adms_pparse_valueto_text (p_pparse mypparse,p_text _text);
win32_interface void adms_pparse_valueto_position (p_pparse mypparse,const char* _position);
win32_interface void adms_pparse_valueto_tkstart (p_pparse mypparse,const char* _tkstart);
win32_interface void adms_pparse_valueto_tklen (p_pparse mypparse,int _tklen);
win32_interface void adms_pparse_valueto_tkid (p_pparse mypparse,admse _tkid);

/* ------- admst -------------- */
#define adms_admst(member) ((p_admst)(member))
struct s_admst {
  t_adms _adms;
  p_admst _previous;
  p_admst _previouspath;
  admse _pseudo;
  union admsu _item;
  void* _valueto;
  int _position;
  int _count;
  int _hasnewbasicstring;
  p_admst _original;
  int _refd;
  int _true;
  p_admst _preva;
  p_admst _nexta;
  admse _selected;
};
win32_interface char*adms_admst_uid (p_admst myadmst);
win32_interface p_admst adms_admst_new (p_admst myprevious,p_admst mypreviouspath,admse mypseudo);
win32_interface void adms_admst_free(p_admst myadmst);
win32_interface void adms_admst_valueto_previous (p_admst myadmst,p_admst _previous);
win32_interface void adms_admst_valueto_previouspath (p_admst myadmst,p_admst _previouspath);
win32_interface void adms_admst_valueto_pseudo (p_admst myadmst,admse _pseudo);
win32_interface void adms_admst_valueto_item (p_admst myadmst,p_adms _item);
win32_interface void adms_admst_valueto_valueto (p_admst myadmst,void* _valueto);
win32_interface void adms_admst_valueto_position (p_admst myadmst,int _position);
win32_interface void adms_admst_valueto_count (p_admst myadmst,int _count);
win32_interface void adms_admst_valueto_hasnewbasicstring (p_admst myadmst,int _hasnewbasicstring);
win32_interface void adms_admst_valueto_original (p_admst myadmst,p_admst _original);
win32_interface void adms_admst_valueto_refd (p_admst myadmst,int _refd);
win32_interface void adms_admst_valueto_true (p_admst myadmst,int _true);
win32_interface void adms_admst_valueto_preva (p_admst myadmst,p_admst _preva);
win32_interface void adms_admst_valueto_nexta (p_admst myadmst,p_admst _nexta);
win32_interface void adms_admst_valueto_selected (p_admst myadmst,admse _selected);

/* ------- transform -------------- */
#define adms_transform(member) ((p_transform)(member))
struct s_transform {
  t_adms _adms;
  char* _fullfilename;
  char* _basefilename;
  int _l;
  char* _name;
  p_transform _parent;
  void* _callback;
  char* _id;
  p_path _pathselect;
  p_path _pathpath;
  p_path _pathinto;
  p_path _pathtest;
  p_path _pathlist;
  p_text _textselect;
  p_text _textdatatype;
  p_text _textmatch;
  p_text _textrequired;
  p_text _textseparator;
  p_text _textfrom;
  p_text _textto;
  p_text _textlist;
  p_text _textname;
  p_text _textformat;
  p_text _textfile;
  p_text _textversion;
  p_text _textonduplicate;
  p_text _textprobe;
  p_text _textstring;
  p_slist _templates;
  p_slist _textarguments;
  p_path _pathoncompare;
  p_slist _pathinputs;
  p_slist _attribute;
  p_slist _children;
};
win32_interface char*adms_transform_uid (p_transform mytransform);
win32_interface p_transform adms_transform_new (const char* myfullfilename,const char* mybasefilename,int myl,const char* myname,p_transform myparent);
win32_interface void adms_transform_free(p_transform mytransform);
win32_interface void adms_transform_valueto_fullfilename (p_transform mytransform,const char* _fullfilename);
win32_interface void adms_transform_valueto_basefilename (p_transform mytransform,const char* _basefilename);
win32_interface void adms_transform_valueto_l (p_transform mytransform,int _l);
win32_interface void adms_transform_valueto_name (p_transform mytransform,const char* _name);
win32_interface void adms_transform_valueto_parent (p_transform mytransform,p_transform _parent);
win32_interface void adms_transform_valueto_callback (p_transform mytransform,void* _callback);
win32_interface void adms_transform_valueto_id (p_transform mytransform,const char* _id);
win32_interface void adms_transform_valueto_pathselect (p_transform mytransform,p_path _pathselect);
win32_interface void adms_transform_valueto_pathpath (p_transform mytransform,p_path _pathpath);
win32_interface void adms_transform_valueto_pathinto (p_transform mytransform,p_path _pathinto);
win32_interface void adms_transform_valueto_pathtest (p_transform mytransform,p_path _pathtest);
win32_interface void adms_transform_valueto_pathlist (p_transform mytransform,p_path _pathlist);
win32_interface void adms_transform_valueto_textselect (p_transform mytransform,p_text _textselect);
win32_interface void adms_transform_valueto_textdatatype (p_transform mytransform,p_text _textdatatype);
win32_interface void adms_transform_valueto_textmatch (p_transform mytransform,p_text _textmatch);
win32_interface void adms_transform_valueto_textrequired (p_transform mytransform,p_text _textrequired);
win32_interface void adms_transform_valueto_textseparator (p_transform mytransform,p_text _textseparator);
win32_interface void adms_transform_valueto_textfrom (p_transform mytransform,p_text _textfrom);
win32_interface void adms_transform_valueto_textto (p_transform mytransform,p_text _textto);
win32_interface void adms_transform_valueto_textlist (p_transform mytransform,p_text _textlist);
win32_interface void adms_transform_valueto_textname (p_transform mytransform,p_text _textname);
win32_interface void adms_transform_valueto_textformat (p_transform mytransform,p_text _textformat);
win32_interface void adms_transform_valueto_textfile (p_transform mytransform,p_text _textfile);
win32_interface void adms_transform_valueto_textversion (p_transform mytransform,p_text _textversion);
win32_interface void adms_transform_valueto_textonduplicate (p_transform mytransform,p_text _textonduplicate);
win32_interface void adms_transform_valueto_textprobe (p_transform mytransform,p_text _textprobe);
win32_interface void adms_transform_valueto_textstring (p_transform mytransform,p_text _textstring);
win32_interface void adms_transform_valueto_templates (p_transform mytransform,p_slist _templates);
win32_interface void adms_transform_valueto_textarguments (p_transform mytransform,p_slist _textarguments);
win32_interface void adms_transform_valueto_pathoncompare (p_transform mytransform,p_path _pathoncompare);
win32_interface void adms_transform_valueto_pathinputs (p_transform mytransform,p_slist _pathinputs);
win32_interface void adms_transform_valueto_attribute (p_transform mytransform,p_slist _attribute);
win32_interface void adms_transform_valueto_children (p_transform mytransform,p_slist _children);

/* ------- itransform -------------- */
#define adms_itransform(member) ((p_itransform)(member))
struct s_itransform {
  t_adms _adms;
  p_transform _template;
  p_transform _originalcall;
  p_slist _variable;
  p_slist _return;
};
win32_interface char*adms_itransform_uid (p_itransform myitransform);
win32_interface p_itransform adms_itransform_new (p_transform mytemplate);
win32_interface void adms_itransform_free(p_itransform myitransform);
win32_interface void adms_itransform_valueto_template (p_itransform myitransform,p_transform _template);
win32_interface void adms_itransform_valueto_originalcall (p_itransform myitransform,p_transform _originalcall);
win32_interface void adms_itransform_valueto_variable (p_itransform myitransform,p_slist _variable);
win32_interface void adms_itransform_valueto_return (p_itransform myitransform,p_slist _return);

/* ------- return -------------- */
#define adms_return(member) ((p_return)(member))
struct s_return {
  t_adms _adms;
  char* _name;
  char* _value;
};
win32_interface char*adms_return_uid (p_return myreturn);
win32_interface p_return adms_return_new (const char* myname,const char* myvalue);
win32_interface void adms_return_free(p_return myreturn);
win32_interface void adms_return_valueto_name (p_return myreturn,const char* _name);
win32_interface void adms_return_valueto_value (p_return myreturn,const char* _value);

/* ------- attribute -------------- */
#define adms_attribute(member) ((p_attribute)(member))
struct s_attribute {
  t_adms _adms;
  char* _name;
  int _name_isdefault : 2;
  p_adms _value;
  int _value_isdefault : 2;
};
win32_interface char*adms_attribute_uid (p_attribute myattribute);
win32_interface p_attribute adms_attribute_new (const char* myname);
win32_interface void adms_attribute_free(p_attribute myattribute);
win32_interface void adms_attribute_valueto_name (p_attribute myattribute,const char* _name);
win32_interface void adms_attribute_valueto_value (p_attribute myattribute,p_adms _value);

/* ------- admstvariable -------------- */
#define adms_admstvariable(member) ((p_admstvariable)(member))
struct s_admstvariable {
  t_adms _adms;
  char* _name;
  p_slist _value;
};
win32_interface char*adms_admstvariable_uid (p_admstvariable myadmstvariable);
win32_interface p_admstvariable adms_admstvariable_new (const char* myname);
win32_interface void adms_admstvariable_free(p_admstvariable myadmstvariable);
win32_interface void adms_admstvariable_valueto_name (p_admstvariable myadmstvariable,const char* _name);
win32_interface void adms_admstvariable_valueto_value (p_admstvariable myadmstvariable,p_slist _value);

/* ------- lexval -------------- */
#define adms_lexval(member) ((p_lexval)(member))
struct s_lexval {
  t_adms _adms;
  char* _string;
  int _string_isdefault : 2;
  char* _f;
  int _f_isdefault : 2;
  int _l;
  int _l_isdefault : 2;
  int _c;
  int _c_isdefault : 2;
};
win32_interface char*adms_lexval_uid (p_lexval mylexval);
win32_interface p_lexval adms_lexval_new (const char* mystring,const char* myf,int myl,int myc);
win32_interface void adms_lexval_free(p_lexval mylexval);
win32_interface void adms_lexval_valueto_string (p_lexval mylexval,const char* _string);
win32_interface void adms_lexval_valueto_f (p_lexval mylexval,const char* _f);
win32_interface void adms_lexval_valueto_l (p_lexval mylexval,int _l);
win32_interface void adms_lexval_valueto_c (p_lexval mylexval,int _c);

/* ------- yaccval -------------- */
#define adms_yaccval(member) ((p_yaccval)(member))
struct s_yaccval {
  t_adms _adms;
  char* _filename;
  p_adms _usrdata;
};
win32_interface char*adms_yaccval_uid (p_yaccval myyaccval);
win32_interface p_yaccval adms_yaccval_new (const char* myfilename);
win32_interface void adms_yaccval_free(p_yaccval myyaccval);
win32_interface void adms_yaccval_valueto_filename (p_yaccval myyaccval,const char* _filename);
win32_interface void adms_yaccval_valueto_usrdata (p_yaccval myyaccval,p_adms _usrdata);

win32_interface admse ns_strtoe (const char* tk);
win32_interface char* ns_etostr (admse e);
win32_interface p_new adms_admsxml_new (p_ktransform mytransform,const char* ename);
win32_interface p_cmp adms_admsxml_cmp (p_kadmst myadmst);
#endif /* adms_h */
