"""
Helper to extract APP_LOAD_PARAMS parameters values.
It takes as a first parameter the parameter name to be search and output the
corresponding values from the rest of the script parameters.
"""

from sys import argv

if __name__ == '__main__':

    assert len(argv) > 2
    searching = argv[1]

    res = []
    args = argv[2:]

    for i, arg in enumerate(args):
        if arg == searching and len(args) > i:
            res.append(repr(args[i + 1]))
    print(" ".join(res))
