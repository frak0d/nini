#include <map>
#include <string>
#include <cstdio>
#include <cstdint>

namespace Ini
{

namespace Util
{
	// https://stackoverflow.com/a/1431206/14976549
	char* ltrim(char* s)
	{
		while(isspace(*s)) s++;
		return s;
	}
	
	char* rtrim(char* s)
	{
		char* back = s + strlen(s);
		while(isspace(*--back));
		*(back+1) = '\0';
		return s;
	}
	
	char* trim(char* s)
	{
		return rtrim(ltrim(s));
	}
}

class Section
{
friend class File;

private:
	std::map<std::string, std::string> entries;
	
public :
	std::string& operator [] (const std::string& key)
	{
		return entries[key];
	}
};

class File
{
private:
	std::string fpath;
	std::map<std::string, Section> sections;
	
public :
	Section& operator [] (const std::string& section)
	{
		return sections[section];
	}
	
	bool load(const std::string& path)
	{
		Section* current_section;
		FILE* fp = fopen(path.c_str(), "r");
		
		if (!fp) return false;
		
		char key[4096]{'\0'};
		char val[4096]{'\0'};
		char line[10000]{'\0'};
		char title[4096]{'\0'};
		
		using namespace Ini::Util;
		
		while(fgets(line, sizeof(line), fp))
		{
			if (line[0] == ';' or line[0] == '#'); // ignore comments
			
			else if (1 == sscanf(line, "[%[^]]]", title))
			{
				current_section = &sections[trim(title)];
			}
			else if (2 == sscanf(line, "%[^=]=%[^\n]", key, val))
			{
				(*current_section)[trim(key)] = trim(val);
			}
		}
		
		std::fclose(fp); return true;
	}
	
	bool save(const std::string& path)
	{
		FILE* fp = fopen(path.c_str(), "w");
		if (!fp) return false;
		
		std::string line;
		
		for (auto& [title, section] : sections)
		{
			line = "\n["+title+"]\n";
			std::fprintf(fp, "%s\n", line.c_str());
			
			for (auto& [key, value] : section.entries)
			{
				line = key+" = "+value;
				std::fprintf(fp, "%s\n", line.c_str());
			}
		}
		
		std::fclose(fp); return true;
	}
	
	File();
	
	File(const std::string& path) : fpath(path)
	{
		load(fpath);
	}
	
	~File()
	{
		if (!fpath.empty()) save(fpath);
	}
};

}