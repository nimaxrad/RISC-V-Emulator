# diff two files in python 3.0
# http://stackoverflow.com/questions/12654772/diff-two-files-in-python-3-0
#
# python 3.0: how to get the difference between two files?
# http://stackoverflow.com/questions/10660435/python-3-0-how-to-get-the-difference-between-two-files
#
# Python 3.0: how to get the difference between two files?
# http://stackoverflow.com/questions/10660435/python-3-0-how-to-get-the-difference-between-two-files

import difflib
import sys
import re

def printdiff():
    with open(sys.argv[1]) as file1, open(sys.argv[2]) as file2:
            diff = difflib.unified_diff(
            file1.readlines(),
            file2.readlines(),
            fromfile=sys.argv[1],
            tofile=sys.argv[2],
        )   
    print(''.join(diff))


def cmp_ignore_ws():
    with open(sys.argv[1]) as file1, open(sys.argv[2]) as file2:
        text1 = re.sub(r'\s+', '', file1.read()).strip()
        text2 = re.sub(r'\s+', '', file2.read()).strip()
        if (text1 == text2):
            return 0
        else:
            return 1
    
if __name__ == '__main__':
    if not cmp_ignore_ws():
        sys.exit(0)
    else:
        printdiff()
        sys.exit(1)
