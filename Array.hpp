#pragma once

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

