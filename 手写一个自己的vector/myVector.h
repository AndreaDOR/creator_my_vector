#pragma once
#include <iostream>
#include <cassert>
#include <stddef.h>
namespace My {
	template<typename T>
	class myVector
	{
	public:
		using value_type = T; //数据类型
		using reference = T&;//数据类型引用
		using const_reference = const T&; //const 引用

		//迭代器
		using iterator = T*;
		using const_iterator = const T*;

		//大小类型
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t; //指针差异，有符号整数类型

	private:
		T* m_data;
		std::size_t m_size;
		std::size_t m_capacity;//不重新分配情况下可容纳的数量
 
	public:
		 
		constexpr myVector() noexcept
			:m_data(), m_size(), m_capacity()
		{
		}
		~myVector()
		{
			
			this->do_destruct();
		}
		//拷贝构造函数
		myVector(const myVector& rhs)
		{
			this->m_data = static_cast<T*>(::operator new(rhs.m_capacity * sizeof(T)));
			//如果我们在构造过程中发生了异常，我们需要记录已经成功构造的元素，然后将已经构造的元素析构掉
			this->m_size = 0;
			this->m_capacity = rhs->m_capacity;

			try {
				//使用了“定位 new”操作符（placement new）来在已分配的内存中构造对象。
				//调用rhs的构造函数，将m_data复制过来
				for (int i = 0; i < rhs.m_size; i += 1)
				{
					::new(&this->m_data[i]) T(rhs.m_data[i]);
					this->m_size = this->m_size + 1;
				}
			}
			catch (...) {
				for (std::size_t i = 0; i < this->m_size; ++i)
				{
					this->m_data[i].~T();
				}
				//然后把内存释放掉
				::operator delete(this->m_data);

				throw;
			}
		}


		//移动构造
		myVector(myVector&& rhs)noexcept
		{
			this->m_data = rhs.m_data;
			this->m_size = rhs.m_size;
			this->m_capacity = rhs.m_capacity;
			rhs.m_data = nullptr;
			rhs.m_size = 0;
			rhs.m_capacity = 0;
		}
		//	//重载赋值运算符
		myVector& operator=(const myVector& rhs)
		{
			if (*this != &rhs)
			{
				this->m_data = static_cast<T*>(::operator new(rhs.m_capacity * sizeof(T)));
				//如果我们在构造过程中发生了异常，我们需要记录已经成功构造的元素，然后将已经构造的元素析构掉
				this->m_size = 0;
				this->m_capacity = rhs->m_capacity;

				try {
					//使用了“定位 new”操作符（placement new）来在已分配的内存中构造对象。
					//调用rhs的构造函数，将m_data复制过来
					for (std::size_t i = 0; i < rhs.m_size; ++i)
					{
						new(&this->m_data[i]) T(rhs.m_data[i]);
						this->m_size++;
					}
				}
				catch (...) {
					for (std::size_t i = 0; i < this->m_size; ++i)
					{
						this->m_data[i].~T();
					}
					//然后把内存释放掉
					::operator delete(this->m_data);
					this->m_size = 0;
					throw;
				}
			}
			return *this;
		}
		//	//"移动"赋值运算符
		myVector& operator=(myVector&& rhs)noexcept
		{
			if (*rhs != &this)
			{
				this->m_data = rhs.m_data;
				this->m_size = rhs.m_size;
				this->m_capacity = rhs.m_capacity;
				rhs.m_data = nullptr;
				rhs.m_size = 0;
				rhs.m_capacity = 0;
			}
			return *this;
		}



		//迭代器
		iterator begin() noexcept
		{
			return this->m_data;
		}
		const_iterator begin() const noexcept
		{
			return this->m_data;
		}
		iterator end() noexcept
		{
			return this->m_data + this->m_size;
		}
		const_iterator end() const noexcept
		{
			return this->m_data + this->m_size;
		}

		//data函数
		value_type* data() noexcept {
			return this->m_data;
		}

		//data函数
		const value_type* data()const noexcept {
			return this->m_data;
		}
		//size函数
		size_type size() const noexcept
		{
			return this->m_size;
		}
		//capacity函数
		size_type capacity() const noexcept
		{
			return this->m_capacity;
		}
		//empty函数
		bool empty()const noexcept
		{
			return this->m_size == 0;
		}
		//clear函数
		void clear() noexcept
		{
			//我们自己管理内存，所以不使用delete，采取调用该对象的析构函数
			//销毁所有已经初始化的元素,然后把内存释放掉
			for (std::size_t i = 0; i < this->m_size; ++i)
				this->m_data[i].~T();
			this->m_size = 0;
		}
		//pop_back函数
		void pop_back() noexcept
		{
			assert(!this->empty());
			std::size_t k = this->m_size - 1;
			this->m_data[k].~T(); //调用最后一个数据的析构
			this->m_size = k;
		}



		//emplace_back函数
		//直接在容器尾部构造插入新元素
		template<typename... ArgsT>
		reference emplace_back(ArgsT&&... args)
		{
			if (this->m_size < this->m_capacity)
			{
				std::size_t k = this->m_size;

				//将参数包中的每个参数完美转发给元素的构造函数，保持它们的原始值类别不变。
				// 这样做可以避免不必要的拷贝或移动操作，从而提高性能。
				new(&this->m_data[k]) T(std::forward<ArgsT>(args)...);
			}
			std::size_t new_capacity = this->m_size + 1;
			new_capacity |= this->m_size / 2; //* 1.5
			auto new_data = static_cast<T*>(operator new(new_capacity * sizeof(T)));
			std::size_t new_size = 0;

			try
			{
				//使用了“定位 new”操作符（placement new）来在已分配的内存中构造对象。
				//调用rhs的构造函数，将m_data复制过来
				for (std::size_t i = 0; i < this->m_size; ++i)
				{
					new(&new_data[i]) T(std::move(this->m_data[i]));
					++new_size;
				}
				//让长度最少加一
				new(&new_data[new_size]) T(std::forward<ArgsT>(args)...);
				++new_size;
			}
			catch (...)
			{
				for (std::size_t i = 0; i < new_size; ++i)
					new_data[i].~T();
				//然后把内存释放掉
				operator delete(new_data);
				throw;
			}
			this->do_destruct();

			this->m_data = new_data;
			this->m_size = new_size;
			this->m_capacity = new_capacity;
			return new_data[new_size];
		}

		//push_back函数
		void push_back(const T& value)
		{
			this->emplace_back(value);
		}

		//push_back函数 右值重载
		void push_back(T&& value)
		{
			this->emplace_back(std::move(value));
		}

		//将容器内全部对象销毁，并且释放内存
		void do_destruct() noexcept
		{
			//我们自己管理内存，所以不使用delete，采取调用该对象的析构函数
			//销毁所有已经初始化的元素,然后把内存释放掉
			this->clear();
			operator delete (this->m_data);
		}
	};


}

