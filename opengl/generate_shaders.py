#!/usr/bin/python
# script to generate shaders variables C from files
from sys import argv
from os import walk
from os import path

file = open(argv[1], "w")
file.write("#ifndef __SHADERS_SOURCE__\n")
file.write("#define __SHADERS_SOURCE__\n\n")

for (dirpath, dirnames, filenames) in walk(argv[2]):
	for filename in filenames:
		name = path.splitext(filename)[0]
		file.write("const char* __extern%sSource = " % (name))
		filehandle = open("%s/%s" % (dirpath, filename), "r")
		for line in filehandle:
			file.write('\n"%s"' % (line.replace("\n", "\\n")))
		filehandle.close()
		file.write(";\n")


file.write("\n#endif\n")

