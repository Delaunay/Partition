#pragma once

#include <Graphics/DirectX.h>

namespace pn {

// ------ CLASS DEFINITIONS -------

template<typename T, int N>
struct cbuffer_array {
	T				data[N];
	pn::dx_buffer	buffer;
};

template<typename T>
struct cbuffer {
	T				data;
	pn::dx_buffer	buffer;
};

// ------- FUNCTIONS ------------

template<typename T, int N>
void InitializeCBuffer(pn::dx_device device, cbuffer_array<T, N>& uni) {
	uni.buffer = pn::CreateConstantBuffer(device, &uni.data, 1);
}

template<typename T>
void InitializeCBuffer(pn::dx_device device, cbuffer<T>& uni) {
	uni.buffer = pn::CreateConstantBuffer(device, &uni.data, 1);
}

template<typename T, int N>
void UpdateBuffer(pn::dx_context context, cbuffer_array<T, N>& uni) {
	context->UpdateSubresource(uni.buffer.Get(), 0, nullptr, &uni.data, 0, 0);
}

template<typename T>
void UpdateBuffer(pn::dx_context context, cbuffer<T>& uni) {
	context->UpdateSubresource(uni.buffer.Get(), 0, nullptr, &uni.data, 0, 0);
}

} // namespace pn