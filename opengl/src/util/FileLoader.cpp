#include "util/FileLoader.h"
#include <sstream>

string FileLoader::getFileContents(string filename) {
	ifstream infile;
	infile.open(filename.c_str(), ifstream::in);

	stringstream sstream;
	sstream << infile.rdbuf();

	return sstream.str();
}