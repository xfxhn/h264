#pragma once
#include "Common.h"

template<class T>
class Array
{
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
	T& operator[](int idx);
	~Array();
};



template<class T>
Array<T>::Array(size_t capacity)
{
	this->capacity = capacity;
	this->length = 0;
	printf("Ö´ÐÐ¼¸´Î");
	this->arr = new T[this->capacity];

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
T& Array<T>::operator[](int idx)
{
	return arr[idx];
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

