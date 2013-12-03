
#
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#
#                      For use with the 'Temple' system
#
# When adding Component archetypes to the system, you should place them in a
# directory structure that mimics what you want to present to the user in the
# program's interface - i.e. when choosing a Component type to create from the
# context menu.  This script, when run from the root of said structure, will
# create a CPP file for inclusion in the program that automatically sets up a
# graph of the Component hierarchy.
#
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#

# NAME_TOK defines what this script looks for in Component .h files to find the
# visible name of the class - the one seen by users. The entirety of the line
# following this token is rstripped and then used as that name.
NAME_TOK = '//CMP_NAME='

# Note that a similar process finds the C++ class name: We look for a line that
# contains 'class' and doesn't end with ';', because that's good enough.

# HEADER is printed at the beginning of the output.
HEADER =\
"""
/*
 * - - - - - - - - - - - AUTO-PUKED COMPONENT HIERARCHY- - - - - - - - - - - -
 * This file was generated automatically for use with the 'Temple' system. It
 * includes all derivatives of the Component class and creates data structures
 * for using them within said system.  - - - - - - - - - - - - - - - - - - - -
 * - - - - - - - - - - - - - - - - - -
 */

#include "ComponentGraph.h"

#include <string>
"""

# FOOTER is printed at the end.
FOOTER = ""

# fillStruct defines, for a given Component class, the fields laid out in
# ComponentGraph.h.
def fillStruct(name, cname):
  return '  {\n' + ',\n'.join([

    '    %s::Create' % cname,	# Factory function
    '    %s::FromFile' % cname, # Deserializer
    '    "%s"' % name,		# Name
    '    "%s"' % cname.replace('Component', ''),  # Class name

  ]) + '\n  },'

import os

if __name__ == '__main__':
  allFiles = []
  output = HEADER

  for (roots, dirs, files) in os.walk('.'):
    if roots == '.':
      continue
    for f in files:
      # Skip over files that don't have 'Component' in their name
      if not (f.find('Component') > max(f.rfind('/'), f.rfind('\\'))):
        continue
      if f.find('.h') == len(f) - 2:
        allFiles.append(roots.replace('\\', '/') + '/' + f)
        # Add an include directive for each file
        output += '#include "%s"\n' % (allFiles[-1][2:])

  output += '\ncgNode cmpGraph[] =\n{\n'
  for f in allFiles:
    name = ""
    cname = ""
    with open(f) as cc:
      for l in cc:
        if l.find(NAME_TOK) > -1:
          name = l[l.find(NAME_TOK) + len(NAME_TOK):].rstrip()
          if len(cname) > 0: break
        if l.find('class ') > -1 and l.rstrip()[-1] != ';':
          cname = l[l.find('class ') + len('class '):l.find(':')].rstrip()
          if len(name) > 0: break

    output += fillStruct(name, cname) + '\n'
  output += '};\n'
  output += '\nint cmpTotal = %d;' % (len(allFiles))
  output += FOOTER

  print output
