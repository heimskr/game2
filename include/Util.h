#pragma once

#include <map>
#include <random>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Logger.h"

#define PRINTPOS do { fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); fflush(stderr); } while(0)

extern std::random_device rngdev;
extern std::mt19937 rng;

namespace Game2 {
	template <typename ...Args>
	void print(Args &&...args) {
		printf(std::forward<Args>(args)...);
	#ifdef __SWITCH__
		consoleUpdate(nullptr);
	#endif
	}

	template <typename T>
	std::string hex(T number) {
		std::stringstream ss;
		ss << std::hex << number;
		return ss.str();
	}

	std::vector<std::string> split(const std::string &str, const std::string &delimiter, bool condense = true);
	long parseLong(const std::string &, int base = 10);
	long parseLong(const std::string *, int base = 10);
	long parseLong(const char *, int base = 10);
	double parseDouble(const std::string &);
	double parseDouble(const std::string *);
	double parseDouble(const char *);
	unsigned long parseUlong(const std::string &, int base = 10);
	unsigned long parseUlong(const std::string *, int base = 10);
	unsigned long parseUlong(const char *, int base = 10);
	bool chance(double probability);
	size_t randomRange(size_t min, size_t max);
	double randomRangeDouble(double min, double max);
	std::string stringify(const std::map<std::string, double> &);
	std::map<std::string, double> parseMap(const std::string &);
	bool contains(const std::map<std::string, double> &, const std::map<std::string, double> &);
	bool compare(const std::unordered_set<std::string> &, const std::map<std::string, double> &);
	void shrink(std::map<std::string, double> &);
	void shrink(std::map<std::string, double> &, const std::string &);
	bool approx(double, double);
	bool lte(double, double);
	/** Less than and not approximate */
	bool ltna(double, double);
	std::string niceDouble(double);
	std::string makeUUID();

	template <typename T>
	bool compare(const std::unordered_set<T> &first, const std::unordered_set<T> &second) {
		if (first.size() != second.size())
			return false;

		for (const T &item: first)
			if (second.count(item) == 0)
				return false;

		return true;
	}
}
