#pragma once
#include "Common.h"

template<class T>
class Array
{
public:
	class Proxy {
	public:
		int index;
		Array& theArr;
	public:

		Proxy(Array& arr, int idx);

		Proxy& operator=(T val);
		Proxy& operator=(Array<T>::Proxy Proxy);
		T operator->();

		operator T() const;
	};
public:
	size_t capacity;
	size_t length;
	T* arr;
public:
	Array(size_t capacity);
	void push(T data);
	void splice(size_t idx, int count = -1);
	void clear();

	void deletePointer(size_t idx, int count = -1);
	Proxy operator[](int idx);
	~Array();
};
template<class T>
inline Array<T>::Proxy::Proxy(Array& arr, int idx) :theArr(arr), index(idx)
{
}

template<class T>
typename Array<T>::Proxy& Array<T>::Proxy::operator=(T val)
{
	if (index >= 16)
	{
		printError("参考帧索引最大16");
		exit(-1);
	}
	if (index >= theArr.length)
	{
		theArr.length = index + 1;
	}
	theArr.arr[index] = val;

	return *this;
}

template<class T>
typename Array<T>::Proxy& Array<T>::Proxy::operator=(const Array<T>::Proxy proxy) {

	int a = proxy.index;
	this->theArr[this->index] = proxy.theArr.arr[proxy.index];

	return *this;
}

template<class T>
T Array<T>::Proxy::operator->() {
	return theArr.arr[index];
}

template<class T>
Array<T>::Proxy::operator T() const
{
	return theArr.arr[index];
}


template<class T>
Array<T>::Array(size_t capacity)
{
	this->capacity = capacity;
	this->length = 0;
	printf("执行几次");
	this->arr = new T[this->capacity];
	memset(arr, NULL, sizeof(T) * capacity);


}

template<class T>
void Array<T>::push(T data)
{
	if (length >= capacity)
	{
		return;
	}
	else
	{
		arr[length] = data;
		length++;
	}
}
template<class T>
inline void Array<T>::deletePointer(size_t idx, int count)
{
	size_t size = 0;

	size_t deleteSize = 0;
	if (count == -1) {
		size = length;
	}
	else {
		size = idx + count;
	}

	for (size_t i = idx; i < length; i++)
	{
		if (i >= idx && i < size)
		{
			if (arr[i])
			{
				deleteSize++;
				arr[i] = nullptr;
			}
		}
		else if (i >= size)
		{
			arr[i - count] = arr[i];
			arr[i] = nullptr;
		}
	}

	length -= deleteSize;

}
template<class T>
void Array<T>::splice(size_t idx, int count)
{
	size_t size = 0;
	if (count == -1) {
		size = length;
	}
	else {
		size = idx + count;
	}


	size_t deleteSize = 0;

	for (size_t i = idx; i < length; i++)
	{
		if (i >= idx && i < size)
		{
			if (arr[i])
			{
				deleteSize++;
				delete arr[i];
				arr[i] = nullptr;
			}
		}
		else if (i >= size)
		{
			arr[i - count] = arr[i];
			arr[i] = nullptr;
		}
	}

	length -= deleteSize;
}

template<class T>
inline void Array<T>::clear()
{
	for (size_t i = 0; i < length; i++)
	{
		if (arr[i])
		{
			delete arr[i];
			arr[i] = nullptr;
		}
	}

	length = 0;
}



template<class T>
typename Array<T>::Proxy Array<T>::operator[](int idx)
{
	return Proxy(*this, idx);
}



template<class T>
Array<T>::~Array()
{
	if (arr)
	{
		for (size_t i = 0; i < length; i++)
		{
			delete arr[i];
			arr[i] = nullptr;
		}
		delete[] arr;
		arr = nullptr;
	}
}


