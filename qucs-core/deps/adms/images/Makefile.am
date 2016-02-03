generated_FILES= \
   adms-alias.png        \
   adms-element.png      \
   adms-enumeration.png  \
   adms-list.png         \
   adms-scalar.png       \
   adms-uid.png

EXTRA_DIST=mkimages.pl $(generated_FILES)

# Perl GD module only required for maintainers
if MAINTAINER_MODE
$(generated_FILES): ${srcdir}/mkimages.pl
	$(PERL) ${srcdir}/mkimages.pl
endif

CLEANFILES=core .*.swp *\~ $(generated_FILES)
