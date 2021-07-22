#include <cmath>
#include <list>
#include <stdexcept>

#include "Resource.h"
#include "Util.h"

std::random_device rngdev;
std::mt19937 rng(rngdev());

namespace Game2 {
	std::vector<std::string> split(const std::string &str, const std::string &delimiter, bool condense) {
		if (str.empty())
			return {};

		size_t next = str.find(delimiter);
		if (next == std::string::npos)
			return {str};

		std::vector<std::string> out {};
		const size_t delimiter_length = delimiter.size();
		size_t last = 0;

		out.push_back(str.substr(0, next));

		while (next != std::string::npos) {
			last = next;
			next = str.find(delimiter, last + delimiter_length);
			std::string sub = str.substr(last + delimiter_length, next - last - delimiter_length);
			if (!sub.empty() || !condense)
				out.push_back(std::move(sub));
		}

		return out;
	}

	std::string join(const std::vector<std::string> &pieces, const std::string &delimiter) {
		std::stringstream ss;
		bool first = true;
		for (const std::string &piece: pieces) {
			if (first)
				first = false;
			else
				ss << delimiter;
			ss << piece;
		}
		return ss.str();
	}

	long parseLong(const std::string &str, int base) {
		const char *c_str = str.c_str();
		char *end;
		long parsed = strtol(c_str, &end, base);
		if (c_str + str.length() != end)
			throw std::invalid_argument("Not an integer: \"" + str + "\"");
		return parsed;
	}

	long parseLong(const std::string *str, int base) {
		return parseLong(*str, base);
	}

	long parseLong(const char *str, int base) {
		return parseLong(std::string(str), base);
	}

	double parseDouble(const std::string &str) {
		const char *c_str = str.c_str();
		char *end;
		double parsed = strtod(c_str, &end);
		if (c_str + str.length() != end)
			throw std::invalid_argument("Not a double: \"" + str + "\"");
		return parsed;
	}

	double parseDouble(const std::string *str) {
		return parseDouble(*str);
	}

	double parseDouble(const char *str) {
		return parseDouble(std::string(str));
	}

	unsigned long parseUlong(const std::string &str, int base) {
		const char *c_str = str.c_str();
		char *end;
		unsigned long parsed = strtoul(c_str, &end, base);
		if (c_str + str.length() != end)
			throw std::invalid_argument("Not an integer: \"" + str + "\"");
		return parsed;
	}

	unsigned long parseUlong(const std::string *str, int base) {
		return parseUlong(*str, base);
	}

	unsigned long parseUlong(const char *str, int base) {
		return parseUlong(std::string(str), base);
	}

	bool chance(double probability) {
		return ((rand() % 100000000) / 100000000.) < probability;
	}

	size_t randomRange(size_t min, size_t max) {
		if (max <= min)
			return min;
		return std::uniform_int_distribution<size_t>(min, max)(rng);
	}

	double randomRangeDouble(double min, double max) {
		if (max <= min)
			return min;
		return std::uniform_real_distribution<double>(min, max)(rng);
	}

	std::string stringify(const std::map<std::string, double> &map) {
		std::stringstream out;
		out.imbue(std::locale::classic());
		bool first = true;
		for (const auto &[name, amount]: map) {
			if (first)
				first = false;
			else
				out << "/";
			out << name << "/" << amount;
		}
		return out.str();
	}

	std::map<std::string, double> parseMap(const std::string &str) {
		const std::vector<std::string> pieces = split(str, "/", false);
		std::map<std::string, double> out;
		for (size_t i = 0, max = pieces.size(); i < max; i += 2)
			out.emplace(pieces[i], parseDouble(pieces[i + 1]));
		return out;
	}

	bool contains(const std::map<std::string, double> &left, const std::map<std::string, double> &right) {
		for (const auto &[name, amount]: right)
			if (left.count(name) == 0 || ltna(left.at(name), amount))
				return false;
		return true;
	}

	bool compare(const std::unordered_set<std::string> &set, const std::map<std::string, double> &map) {
		if (set.size() != map.size())
			return false;

		for (const auto &[key, value]: map)
			if (!set.contains(key))
				return false;

		return true;
	}

	void shrink(Game2::Resource::Map &map) {
		std::list<const std::string *> to_remove;
		for (const auto &[name, amount]: map)
			if (amount < Game2::Resource::MIN_AMOUNT)
				to_remove.push_back(&name);
		for (const std::string *name: to_remove)
			map.erase(*name);
	}

	void shrink(Game2::Resource::Map &map, const std::string &key) {
		if (map[key] < Game2::Resource::MIN_AMOUNT)
			map.erase(key);
	}

	bool approx(double left, double right) {
		return std::fabs(left - right) < 0.000001;
	}

	bool lte(double left, double right) {
		return left <= right || approx(left, right);
	}

	bool ltna(double left, double right) {
		return (left < right) && !approx(left, right);
	}

	std::string niceDouble(double n) {
		std::string str = std::to_string(n);
		if (str.empty() || str.find('.') == std::string::npos)
			return str;
		while (!str.empty() && str.back() == '0')
			str.pop_back();
		if (!str.empty() && str.back() == '.')
			str.pop_back();
		return str;
	}

	std::string makeUUID() {
		// https://stackoverflow.com/a/58467162/227663
		static std::random_device dev;
		static std::mt19937 rng(dev());

		std::uniform_int_distribution<int> dist(0, 15);

		constexpr const char *v = "0123456789abcdef";
		constexpr bool dash[] = {0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0};

		std::string out;
		for (int i = 0; i < 16; ++i) {
			if (dash[i])
				out += "-";
			out += v[dist(rng)];
			out += v[dist(rng)];
		}
		return out;
	}
}
