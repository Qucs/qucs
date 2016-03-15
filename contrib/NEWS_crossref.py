
'''
  NEWS_crossref.py
  This script creates a cross reference for issues, pull requests and bugs.
  Contributions are annotated with the ticket number.
  Similar to [#123], [PR#123] and [SF#123].

'''
# TODO, should we modify the NEWS file in place?

import argparse
import re

# Parse arguments, expect an input and output files.
parser = argparse.ArgumentParser(
    description=
    'This script appends cross-references to our NEWS file.\n'+
    'It looks for markers similar to [#123], [PR#123] and [SF#123].\n'+
    'The markers are added as full links to either GitHub or Sourceforge',
    formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('-i','--input', help='Input file name', required=True)
parser.add_argument('-o','--output',help='Output file name', required=True)
args = parser.parse_args()

# Footer note
footer = '<!-- Cross reference generated with NEWS_crossref.py -->\n'

# Read input NEWS file
# Avoid adding footer and references twice
f_in = open(args.input)
infile = ""
for line in f_in:
    if line.strip() == footer.strip():
        break
    else:
        infile += line
f_in.close()

# Parse issues and pull requests (GitHub), and bugs (SourceForge)
re_issue = re.compile(r'\[#[0-9]+\]')
re_pull  = re.compile(r'\[PR#[0-9]+\]')
re_bug   = re.compile(r'\[SF#[0-9]+\]')

# Turn into set to remove duplicates
issue = set(re_issue.findall(infile))
pull  = set(re_pull.findall(infile))
bug   = set(re_bug.findall(infile))


# Collect references
references = ''

# References for issues on GitHub
for ref in issue:
    number = ref[1:-1].split('#')[1] # drop quotes, keep number only
    link = '%s: https://github.com/Qucs/qucs/issues/%s\n' %(ref, str(number))
    references += link

# References for pull-requests on Github
for ref in pull:
    number = ref[1:-1].split('#')[1]
    link = '%s: https://github.com/Qucs/qucs/pull/%s\n' %(ref, str(number))
    references += link

# References for bugs on SourceForge
for ref in bug:
    number = ref[1:-1].split('#')[1]
    link = '%s: https://sourceforge.net/p/qucs/bugs/%s\n' %(ref, str(number))
    references += link

# Write annotated NEWS file
f_out = open(args.output, 'w')
f_out.write(infile)
f_out.write(footer)
f_out.write(references)
f_out.close()

