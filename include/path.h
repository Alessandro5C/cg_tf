#ifndef __PATH_H__
#define __PATH_H__

#include <string>

class Path {
	std::string path;
public:
	Path(std::string path ="bin")
		: path(path){}
	
	std::string ruta(const std::string& name) {
		std::string str = path + "/";
		str += name;
		return str;
	}
};

#endif
