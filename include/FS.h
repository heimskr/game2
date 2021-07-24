#pragma once

#include <string>
#include "platform.h"

namespace FS {
	void init();
	bool fileExists(const char *);
	bool fileExists(const std::string &);
	bool dirExists(const char *);
	bool dirExists(const std::string &);
	std::string readFile(const char *);
	std::string readFile(const std::string &);
	void writeFile(const char *, const std::string &);
	void writeFile(const std::string &, const std::string &);
}
