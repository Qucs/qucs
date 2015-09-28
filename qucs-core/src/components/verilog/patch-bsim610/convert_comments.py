

'''
This script is used to convert BSIM6.1.0 comments into parameter attributes.

Original:
=========
  > parameter integer IGBMOD = 0 from [0:1]; // 0: Turn off Igb
  >				             // 1: Turn on Igb

Modified:
=========
  > parameter integer IGBMOD = 0 from [0:1]    `ATTR(info="0: Turn off Igb\n 1: Turn on Igb");


The modified version can be used with ADMS to generate documentation strings for the model parameters.

'''


def get_parameter_info(va, debug=False):
    '''
    Extract comments from parameter definitions.

    in va   : input verilog-a file
    in debug: print

    return  : dictionary
      * parameter name as key
      * parameter information, (concatenated with newline)
    '''
    with open(va) as f:
        parStart=False
        parNumber=0
        parLine=0
        doc={}
        for num, line in enumerate(f):
            # unset parameter start flag on blank lines
            if line.strip() == '\n':
                parStart=False
            # unset parameter start flag on normal comments
            if line.startswith('/'):
                parStart=False
            # look for start of parameter
            if line.strip().startswith('parameter'):
                parStart=True
                parNumber+=1
                parLine=num
                param=line.split()[2]
                # if there is a descrition
                if '//' in line:
                    if debug : print '[%i] %s' %(parNumber, param)
                    info=line.split('//')[1]
                    info=info.strip()
                    if debug: print '  ',info
                    # store info string
                    doc[param]=info
            # grap comments after a parameter line
            if parStart and  num > parLine :
                if '//' in line:
                    info=line.split('//')[1]
                    info=info.strip()
                    if debug: print '  ', info

                    # append further info strings
                    doc[param] += '\n'+info
    return doc


def convert_comments(params_info, va, vaout):
    '''
    in doc : dictionary containing parameters and its info strings
    in va  : bsim6 source file
    in vout: target file with modified
    '''
    import io
    fout = io.open(vaout, 'w', newline='\r\n')
    with io.open(va) as f:
        parLine=0
        for num, line in enumerate(f):
            # unset parameter start flag on blank lines
            if line.strip() == '\n':
                parStart=False
                fout.write(line)
            # unset parameter start flag on normal comments
            if line.startswith('/'):
                parStart=False
            # look for start of parameter
            if line.strip().startswith('parameter'):
                parStart=True
                parLine=num
                param=line.split()[2]
                # if found comment earlier, print new attribute
                if param in params_info.keys():
                    # grap parameter declaration, to the left of ;
                    declaration = line.split(';')[0]
                    # create new info attribute
                    attr = '\t`ATTR(info="%s");\n'  %(repr(params_info[param])[1:-1])
                    # write modified line
                    newParam= u'{0: <55} {1: <2}'.format(declaration, attr)
                    fout.write(newParam)
                # parameter without comment
                else:
                    fout.write(line)
            # remove comments on lines after a parameter
            elif parStart and  num > parLine :
                if '//' in line:
                    # do nothing, this comment was concatenated in the new attribute
                    pass
                else:
                    # print whatever comes after a parameter line which is not a comment
                    fout.write(line)
            # print all the rest
            else:
                fout.write(line)

import sys
if __name__ == "__main__":
    try:
        va=sys.argv[1]
        vaout=sys.argv[2]
    except IndexError:
        print "  Usage error: conver_comment.py [bsim6.va] [output.va]"
        sys.exit(1)


    ## Extract parameter comments.
    #va='bsim6.va'
    print 'Parsing %s' %(va)
    params_info = get_parameter_info(va, debug=False)

    # Rewrite BSIM6.va adding the parameter info as attributes.
    #vaout='vout2.va'
    print 'Modifications in %s' %(vaout)
    convert_comments(params_info, va, vaout)
