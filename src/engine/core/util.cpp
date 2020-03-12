#include "engine/core/util.hpp"

#include <fstream>
#include <cctype>

void Util::split(ArrayList<String>& elems, const String& s, char delim) {
    const char* cstr = s.c_str();
    size_t strLength = (size_t)s.length();
    size_t start = 0;
    size_t end = 0;
        
    while (end <= strLength) {
        while (end <= strLength) {
            if (cstr[end] == delim) {
                break;
			}

            ++end;
        }
            
        elems.push_back(s.substr(start, end - start));

        start = end + 1;
        end = start;
    }
}

ArrayList<String> Util::split(const String& s, char delim) {
	ArrayList<String> elems;
	Util::split(elems, s, delim);

	return elems;
}

String Util::getFilePath(const String& fileName) {
	const char* cstr = fileName.c_str();
	size_t strLength = (size_t)fileName.length();
	size_t end = strLength - 1;
	
	while (end != 0) {
		if (cstr[end] == '/' || cstr[end] == '\\') {
			break;
		}

		--end;
	}

	if (end == 0) {
		return fileName;
	}
	
	return fileName.substr(0, end + 1);
}

String Util::getFileExtension(const String& fileName) {
	const char* cstr = fileName.c_str();
	size_t strLength = (size_t)fileName.length();
	size_t end = strLength - 1;

	while (end > 0 && !std::isalnum(cstr[end])) {
		--end;
	}

	size_t start = end;

	while (start > 0) {
		if (cstr[start] == '.') {
			return fileName.substr(start + 1, end - start);
		}
		else if (cstr[start] == '/' || cstr[start] == '\\') {
			return "";
		}

		--start;
	}

	return "";
}

bool Util::loadFileWithLinking(StringStream& out, const String& fileName,
		const String& linkKeyword) {
	std::ifstream file;
	file.open(fileName.c_str());

	String filePath = getFilePath(fileName);
	String line;

	if (file.is_open()) {
		while (file.good()) {
			std::getline(file, line);
			
			if (line.find(linkKeyword) == String::npos) {
				out << line << "\n";
			}
			else {
				String linkFileName = Util::split(line, ' ')[1];
				linkFileName = linkFileName.substr(1, linkFileName.length() - 2);

				loadFileWithLinking(out, filePath + linkFileName,
						linkKeyword);
				out << "\n";
			}
		}
	}
	else {
		DEBUG_LOG(LOG_ERROR, "File IO", "Failed to load included file: %s",
				fileName.c_str());

		return false;
	}

	return true;
}
