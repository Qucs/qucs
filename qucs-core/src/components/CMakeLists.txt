PROJECT(component CXX)

# defines nr_double_t
ADD_DEFINITIONS( -DHAVE_CONFIG_H )

#INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/math

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ../../
                     ../math
                     ../ )


ADD_SUBDIRECTORY( microstrip )
ADD_SUBDIRECTORY( devices )
ADD_SUBDIRECTORY( digital )
ADD_SUBDIRECTORY( verilog )


SET(COMPONENT_SRC  attenuator.cpp dcblock.cpp open.cpp vccs.cpp
	biastee.cpp circulator.cpp dcfeed.cpp pac.cpp vcvs.cpp capacitor.cpp
	ground.cpp resistor.cpp cccs.cpp cross.cpp trafo.cpp inductor.cpp
	tee.cpp ccvs.cpp isolator.cpp itrafo.cpp strafo.cpp vdc.cpp idc.cpp
	vac.cpp phaseshifter.cpp gyrator.cpp tline.cpp iprobe.cpp iac.cpp
	spfile.cpp vnoise.cpp inoise.cpp vpulse.cpp ipulse.cpp vrect.cpp
	irect.cpp amplifier.cpp opamp.cpp iinoise.cpp mutual.cpp mutual2.cpp
	vvnoise.cpp ivnoise.cpp coupler.cpp coaxline.cpp vprobe.cpp vam.cpp
	vpm.cpp tswitch.cpp relais.cpp short.cpp twistedpair.cpp tline4p.cpp
	vexp.cpp iexp.cpp mutualx.cpp vfile.cpp ifile.cpp rfedd.cpp
	rectline.cpp rlcg.cpp hybrid.cpp ctline.cpp )

ADD_LIBRARY(component ${COMPONENT_SRC} )

#ok!

