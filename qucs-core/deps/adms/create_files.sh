cd admsXml
/usr/bin/perl ./mkelements.pl ..

for i in constants.vams disciplines.vams adms.implicit.xml dummy.va dummy.xml; do
/usr/bin/perl ./mkctext.pl $i ..
done

/usr/bin/perl ./mkgrammar.pl ./verilogaYacc.y.in

