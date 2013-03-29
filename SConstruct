# vim:syntax=python

import os

env = Environment(ENV=os.environ)

if os.name == "nt":
    # lets assume that MSVC is used
    env.Append(CPPFLAGS=["/Z7"], LINKFLAGS=["/DEBUG"], LIBS=["advapi32"])
else:
    env.Append(LINKFLAGS=["-rdynamic"])

env.Program("example", ["example.cpp", "traceback.cpp", "StackWalker.cpp"])
