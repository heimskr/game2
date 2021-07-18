#pragma once

#include <string>
#include "platform.h"

namespace FS {
	void init();
	bool fileExists(const char *);
	bool dirExists(const char *);
	std::string readFile(const char *);
	void writeFile(const char *, const std::string &);
}
