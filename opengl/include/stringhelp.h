#ifndef _STRINGHELP_H_
#define _STRINGHELP_H_

#include <string>
#include <vector>
#include <algorithm>

using namespace std;

namespace strhlp
{
	inline string toUpperCase(const string &s)
	{
		string res = s;
		for (int i = 0, n = s.size(); i < n; i++)
			res[i] = toupper(res[i]);
		return res;
	}

	inline string toLowerCase(const string &s)
	{
		string res = s;
		for (int i = 0, n = s.size(); i < n; i++)
			res[i] = tolower(res[i]);
		return res;
	}

	inline string trimLeft(const string &s)
	{
		int i = s.find_first_not_of(" \t\n\v\f\r");
		return i != -1 ? s.substr(i) : s;
	}

	inline string trimRight(const string &s)
	{
		int i = s.find_last_not_of(" \t\n\v\f\r");
		return i != -1 ? s.substr(0, i + 1) : s;
	}

	inline string trim(const string &s)
	{
		return trimRight(trimLeft(s));
	}

	inline vector<string> split(const string &s, char delim = ' ')
	{
		vector<string> arr;
		int prev = 0;
		for (int i = 0, n = s.size(); i < n; i++)
		{
			if (s[i] == delim)
			{
				arr.push_back(s.substr(prev, i - prev));
				prev = i + 1;
			}
		}
		arr.push_back(s.substr(prev));
		return arr;
	}
}

#endif // _STRINGHELP_H_