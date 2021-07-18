#pragma once

#include <stdio.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"

namespace Logger {
	template <typename ...Args>
	void info(Args &&...args) {
		fprintf(stderr, "\e[2m[\e[22;36mi\e[39;2m]\e[22m \e[39m");
		fprintf(stderr, std::forward<Args>(args)...);
		fprintf(stderr, "\n");
		fflush(stderr);
	}

	template <typename ...Args>
	void error(Args &&...args) {
		fprintf(stderr, "\e[2m[\e[22;31m!\e[39;2m]\e[22m \e[39m");
		fprintf(stderr, std::forward<Args>(args)...);
		fprintf(stderr, "\n");
		fflush(stderr);
	}

	template <typename ...Args>
	void warn(Args &&...args) {
		fprintf(stderr, "\e[2m[\e[22;33m!\e[39;2m]\e[22m \e[39m");
		fprintf(stderr, std::forward<Args>(args)...);
		fprintf(stderr, "\n");
		fflush(stderr);
	}

	template <typename ...Args>
	void success(Args &&...args) {
		fprintf(stderr, "\e[2m[\e[22;32m\x02\e[39;2m]\e[22m \e[39m");
		fprintf(stderr, std::forward<Args>(args)...);
		fprintf(stderr, "\n");
		fflush(stderr);
	}
}

#pragma GCC diagnostic pop
