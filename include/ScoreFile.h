#pragma once

#include <string>
#include <sstream>
#include <fstream>

struct ScoreEntry
{
	unsigned int points;
	unsigned int seed;
	std::wstring name;
};

inline bool operator<(const ScoreEntry& lhs, const ScoreEntry& rhs)
{
	return lhs.points < rhs.points;
}

inline bool operator>(const ScoreEntry& lhs, const ScoreEntry& rhs)
{
	return lhs.points > rhs.points;
}

inline std::wostream& operator<<(std::wostream& ostr, const ScoreEntry& e)
{
	return ostr << e.points << L"," << e.seed << L"," << e.name;
}

inline std::wistream& operator>>(std::wistream& istr, ScoreEntry& e)
{
	istr >> e.points;
	istr.ignore(1, L',');
	istr >> e.seed;
	istr.ignore(1, L',');
	std::getline(istr, e.name);
	return istr;
}

class ScoreFile
{
private:
	std::wstring m_fileName;

public:
	ScoreFile(const std::wstring& fileName) : m_fileName(fileName) { }

	void saveEntry(const ScoreEntry& entry)
	{
		std::wofstream file(m_fileName, std::ios_base::app | std::ios_base::out);

		if (file) {
			file << entry << L"\n";
		}
	}

	template<typename OutIter>
	void readEntries(OutIter outputIter)
	{
		std::wifstream file(m_fileName);
		ScoreEntry e;

		if (file) {
			std::wstring line;
			while (std::getline(file, line)) {
				std::wstringstream wss(line);
				wss >> e;
				*outputIter = e;
			}
		}
	}
};