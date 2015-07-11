#pragma once

#include <string>
#include <fstream>

struct ScoreEntry
{
	unsigned int points;
	unsigned int seed;
	std::wstring name;
};

inline std::wostream& operator<<(std::wostream& ostr, const ScoreEntry& e)
{
	return ostr << e.points << L" " << e.seed << L" " << e.name;
}

inline std::wistream& operator>>(std::wistream& istr, ScoreEntry& e)
{
	return istr >> e.points >> e.seed >> e.name;
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
		static_assert(std::is_same<std::iterator_traits<OutIter>::value_type, ScoreEntry>::value, "wrong value type, must be 'ScoreEntry'");

		std::wifstream file(m_fileName);

		if (file) {
			std::wstring line;
			std::wstringstream wss;
			while (std::getline(file, line)) {
				wss = line;
				wss >> *outputIter;
				++outputIter;
			}
		}
	}
};