#include "OptionsFile.h"
#include <stdio.h>
#include <string.h>
#include <cctype>

OptionsFile::OptionsFile() {
#ifdef __APPLE__
	settingsPath = "./Documents/options.txt";
#elif defined(ANDROID)
	settingsPath = "options.txt";
#else
	settingsPath = "options.txt";
#endif
}

void OptionsFile::save(const StringVector& settings) {
	FILE* pFile = fopen(settingsPath.c_str(), "w");
	if(pFile != NULL) {
		for(StringVector::const_iterator it = settings.begin(); it != settings.end(); ++it) {
			fprintf(pFile, "%s\n", it->c_str());
		}
		fclose(pFile);
	}
}

StringVector OptionsFile::getOptionStrings() {
	StringVector returnVector;
	FILE* pFile = fopen(settingsPath.c_str(), "r");
	if(pFile != NULL) {
		char lineBuff[128];
		while(fgets(lineBuff, sizeof lineBuff, pFile)) {
			if(strlen(lineBuff) > 2) {
				// Remove newline character
				char* newline = strchr(lineBuff, '\n');
				if (newline) *newline = '\0';
				
				// Split line into key=value or key = value
				char* equals = strchr(lineBuff, '=');
				if (equals) {
					*equals = '\0';
					// Trim whitespace from key
					char* key = lineBuff;
					while(*key && isspace(*key)) key++;
					char* keyEnd = key + strlen(key) - 1;
					while(keyEnd > key && isspace(*keyEnd)) keyEnd--;
					*(keyEnd + 1) = '\0';
					
					// Trim whitespace from value
					char* value = equals + 1;
					while(*value && isspace(*value)) value++;
					char* valueEnd = value + strlen(value) - 1;
					while(valueEnd > value && isspace(*valueEnd)) valueEnd--;
					*(valueEnd + 1) = '\0';
					
					returnVector.push_back(std::string(key));
					returnVector.push_back(std::string(value));
				}
			}
		}
		fclose(pFile);
	}
	return returnVector;
}
