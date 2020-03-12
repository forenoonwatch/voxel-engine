#pragma once

#include <algorithm>

#include <string>
#include <string_view>
#include <sstream>

#define StringStream std::stringstream
#define StringView std::string_view

class String : public std::string {
	public:
		inline String()
				: std::string() {}

		inline String(char* cStr, size_t len)
				: std::string(cStr, len) {}

		inline String(const char* cStr)
				: std::string(cStr) {}

		inline String(const std::string& str)
				: std::string(str) {}

		inline String(std::string&& str)
				: std::string(str) {}

		inline String to_lower() {
			String res(*this);
			std::transform(res.begin(), res.end(), res.begin(), ::tolower);

			return res;
		}

		inline String to_upper() {
			String res(*this);
			std::transform(res.begin(), res.end(), res.begin(), ::toupper);

			return res;
		}

		inline bool starts_with(const String& prefix) {
			return compare(0, prefix.length(), prefix) == 0;
		}
};

namespace std {
	/*template <>
	struct hash<String> {
		inline size_t operator()(const String& str) const {
			return std::hash<std::string>()((std::string)str);
		}
	};*/

	template <>
	struct hash<String> {
		inline size_t operator()(const String& str) const {
			uint64_t hval = (uint64_t)0xcbf29ce484222325ULL;
            const unsigned char *s = (const unsigned char*)str.c_str();

            while (*s) {
                hval *= (uint64_t)0x100000001b3ULL;
                hval ^= (uint64_t)(*s++);
            }

            return (size_t)hval;
		}
	};
};
