#pragma once

template<class T>
class Array
{
public:
	int capcity;
	int curLen;
	T* arr;
public:
	/*Array();*/
	Array(size_t capacity);
	void push(T data);
	T& operator[](int idx);
	~Array();
};





//template<class T>
//inline Array<T>::Array()
//{
//}

template<class T>
Array<T>::Array(size_t capacity)
{
	this->capcity = capcity;
	this->curLen = 0;
	this->arr = new T[this->capcity];
	for (size_t i = 0; i < capcity; i++)
	{
		this->arr[i]();
	}
}

template<class T>
void Array<T>::push(T data)
{
	if (curLen >= capcity)
	{
		return;
	}
	else
	{
		arr[curLen] = data;
		curLen++;
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
		delete arr;
		arr = nullptr;
	}
}

