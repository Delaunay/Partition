#pragma once

#include <Utilities/Memory.h>

namespace pn {

// ----------- CLASS DEFINITION -------------

template<typename T>
struct basic_frame_string {
	static linear_allocator*	frame_alloc;
	T*							data;
	size_t						length;

	basic_frame_string() : data(nullptr), length(0) {}
	basic_frame_string(const basic_frame_string& f) : length(f.length) {
		data = _allocate(length);
		memcpy(data, f.data, sizeof(T)*length);
	}
	basic_frame_string(basic_frame_string&& f) : length(f.length), data(f.data) {
		f.data = nullptr;
		f.length = 0;
	}
	basic_frame_string(T* data, size_t length) : data(data), length(length) {}
	basic_frame_string(const char* str) : length(strlen(str)) {
		data = _allocate(length);
		memcpy(data, str, sizeof(T)*length);
	}

	T& operator[](size_t index) {
		return data[index];
	}
	T operator[](size_t index) const {
		return data[index];
	}

	bool operator==(const basic_frame_string& rhs) const {
		if (length != rhs.length) return false;
		if (length == 0) return true;

		auto cmp_result = memcmp(data, rhs.data, length);
		return cmp_result == 0;
	}
	bool operator!=(const basic_frame_string& rhs) const {
		return !operator==(rhs);
	}

	bool operator==(const std::basic_string<T>& rhs) const {
		if (length != rhs.length()) return false;
		if (length == 0) return true;

		auto cmp_result = memcmp(data, rhs.data(), length);
		return cmp_result == 0;
	}
	bool operator!=(const std::basic_string<T>& rhs) const {
		return !operator==(rhs);
	}

	bool operator==(const T* str) const {
		auto cmp_result = memcmp(data, str, length);
		return cmp_result == 0;
	}
	bool operator!=(const T* str) const {
		return !operator==(str);
	}

	basic_frame_string operator+(const basic_frame_string& rhs) const {
		auto new_length = length + rhs.length;
		auto new_data = _allocate(new_length);
		memcpy(new_data, data, length);
		memcpy(new_data + length, rhs.data, rhs.length);
		return std::move(basic_frame_string(new_data, new_length));
	}
	basic_frame_string operator+(const T* rhs) const {
		auto rhs_len = strlen(rhs);
		auto new_length = length + rhs_len;
		auto new_data = _allocate(new_length);
		memcpy(new_data, data, length);
		memcpy(new_data + length, rhs, rhs_len);
		return std::move(basic_frame_string(new_data, new_length));
	}
	basic_frame_string operator+(const std::basic_string<T>& rhs) const {
		return std::move(operator+(rhs.data()));
	}

	static void SetFrameAllocator(linear_allocator* fa) {
		frame_alloc = fa;
	}

	T* _allocate(size_t n) const {
		assert(frame_alloc != nullptr);
		return (T*) frame_alloc->Allocate(n);
	}

};

template<typename T>
linear_allocator* basic_frame_string<T>::frame_alloc = nullptr;

// -------- TYPEDEFS -------------

using frame_string = basic_frame_string<char>;

// ------- FUNCTIONS --------------

template<typename T>
bool operator==(const std::basic_string<T>& lhs, const basic_frame_string<T>& rhs) {
	return rhs.operator==(lhs);
}
template<typename T>
bool operator!=(const std::basic_string<T>& lhs, const basic_frame_string<T>& rhs) {
	return rhs.operator!=(lhs);
}

template<typename T>
bool operator==(const T* lhs, const basic_frame_string<T>& rhs) {
	return rhs.operator==(lhs);
}
template<typename T>
bool operator!=(const T* lhs, const basic_frame_string<T>& rhs) {
	return rhs.operator!=(lhs);
}

template<typename T>
basic_frame_string<T> operator+(const T* lhs, const basic_frame_string<T>& rhs) {
	auto lhs_len = strlen(lhs);
	auto new_length = rhs.length + lhs_len;
	auto new_data = rhs._allocate(new_length);
	memcpy(new_data, lhs, lhs_len);
	memcpy(new_data + lhs_len, rhs.data, rhs.length);
	return std::move(basic_frame_string<T>(new_data, new_length));
}
template<typename T>
basic_frame_string<T> operator+(const std::basic_string<T>& lhs, const basic_frame_string<T>& rhs) {
	return std::move(lhs.data() + rhs);
}

} // namespace pn