#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

#include <string.h>

#include <Utilities/Memory.h>
#include <Utilities/Logging.h>

namespace pn {

// ------------ TYPEDEFS ---------------

using string	= std::string;
using wstring	= std::wstring;

template<typename T>
using vector	= std::vector<T>;

template<typename K, typename V>
using map		= std::unordered_map<K, V>;

using bytes		= vector<char>;

// ------------ STRUCT/CLASS DEFINITIONS --------



// ------------ FUNCTIONS -------------

// --------- CONTAINER FUNCTIONS -----

template<typename Container>
auto	Size(const Container& container) -> decltype(container.size()) {
	return container.size();
}

template<typename Container>
void	Clear(Container& container) {
	container.clear();
}

// ------- STRING FUNCTIONS ----------

inline vector<string>	Split(const string& s, char delim) {
	std::stringstream ss(s);
	vector<string> elements;
	string element;
	while (std::getline(ss, element, delim)) {
		elements.emplace_back(element);
	}
	return elements;
}

inline void				Insert(string& s, char c) {
	s.push_back(c);
}

inline const char*		CString(const string& s) {
	return s.c_str();
}

// ------- VECTOR FUNCTIONS -----------

template<typename T, typename U>
void	PushBack(vector<T>& vec, const U& value) {
	vec.push_back(value);
}

template<typename T, typename U, typename... Us>
void	PushBack(vector<T>& vec, const U& value, const Us&... args) {
	vec.push_back(value);
	PushBack(vec, args...);
}

template<typename T, typename... Args>
void	EmplaceBack(vector<T>& vec, Args&&... args) {
	vec.emplace_back(std::forward<Args>(args)...);
}

template<typename T>
T		Pop(vector<T>& vec) {
	assert(Size(vec) > 0);
	T back = vec.back();
	vec.pop_back();
	return back;
}

template<typename T, typename SizeType>
void	Reserve(vector<T>& vec, const SizeType s) {
	vec.reserve(s);
}

template<typename T, typename SizeType>
void	Resize(vector<T>& vec, const SizeType s) {
	vec.resize(s);
}

template<typename T>
auto	Get(const vector<T>& v, size_t i) -> decltype(v[i]) {
	return v[i];
}

template<typename T>
auto	Get(vector<T>& v, size_t i) -> decltype(v[i]) {
	return v[i];
}

template<typename T>
auto	Erase(vector<T>& v, const T& el) {
	LogError("Vector erase not implemented yet");
}

// -------- MAP FUNCTIONS ------------

template<typename K, typename V>
bool	Contains(const map<K,V>& m, const K& key) {
	return m.find(key) != m.end();
}

template<typename K, typename V>
void	Insert(map<K,V>& m, const K key, const V& value) {
	if (Contains(m, key)) return;
	m.insert(std::make_pair(key, value));
}

template<typename K, typename V>
void	Remove(map<K, V>& m, const K& key) {
	if (!Contains(m, key)) return;
	m.erase(key);
}

template<typename K, typename V>
auto	Get(const map<K,V>& m, const K& key) {
	return m.at(key);	
}


} // namespace pn