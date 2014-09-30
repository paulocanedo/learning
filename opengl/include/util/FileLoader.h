#include <fstream>
#include <string>
using namespace std;

class FileLoader {

public:
	static string getFileContents(string filename);
	static char*  getFileContent(char* filename);

};