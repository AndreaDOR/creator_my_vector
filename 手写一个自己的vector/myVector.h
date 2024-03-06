#pragma once
#include <iostream>
#include <cassert>
#include <stddef.h>
namespace My {
	template<typename T>
	class myVector
	{
	public:
		using value_type = T; //��������
		using reference = T&;//������������
		using const_reference = const T&; //const ����

		//������
		using iterator = T*;
		using const_iterator = const T*;

		//��С����
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t; //ָ����죬�з�����������

	private:
		T* m_data;
		std::size_t m_size;
		std::size_t m_capacity;//�����·�������¿����ɵ�����
 
	public:
		 
		constexpr myVector() noexcept
			:m_data(), m_size(), m_capacity()
		{
		}
		~myVector()
		{
			
			this->do_destruct();
		}
		//�������캯��
		myVector(const myVector& rhs)
		{
			this->m_data = static_cast<T*>(::operator new(rhs.m_capacity * sizeof(T)));
			//��������ڹ�������з������쳣��������Ҫ��¼�Ѿ��ɹ������Ԫ�أ�Ȼ���Ѿ������Ԫ��������
			this->m_size = 0;
			this->m_capacity = rhs->m_capacity;

			try {
				//ʹ���ˡ���λ new����������placement new�������ѷ�����ڴ��й������
				//����rhs�Ĺ��캯������m_data���ƹ���
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
				//Ȼ����ڴ��ͷŵ�
				::operator delete(this->m_data);

				throw;
			}
		}


		//�ƶ�����
		myVector(myVector&& rhs)noexcept
		{
			this->m_data = rhs.m_data;
			this->m_size = rhs.m_size;
			this->m_capacity = rhs.m_capacity;
			rhs.m_data = nullptr;
			rhs.m_size = 0;
			rhs.m_capacity = 0;
		}
		//	//���ظ�ֵ�����
		myVector& operator=(const myVector& rhs)
		{
			if (*this != &rhs)
			{
				this->m_data = static_cast<T*>(::operator new(rhs.m_capacity * sizeof(T)));
				//��������ڹ�������з������쳣��������Ҫ��¼�Ѿ��ɹ������Ԫ�أ�Ȼ���Ѿ������Ԫ��������
				this->m_size = 0;
				this->m_capacity = rhs->m_capacity;

				try {
					//ʹ���ˡ���λ new����������placement new�������ѷ�����ڴ��й������
					//����rhs�Ĺ��캯������m_data���ƹ���
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
					//Ȼ����ڴ��ͷŵ�
					::operator delete(this->m_data);
					this->m_size = 0;
					throw;
				}
			}
			return *this;
		}
		//	//"�ƶ�"��ֵ�����
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



		//������
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

		//data����
		value_type* data() noexcept {
			return this->m_data;
		}

		//data����
		const value_type* data()const noexcept {
			return this->m_data;
		}
		//size����
		size_type size() const noexcept
		{
			return this->m_size;
		}
		//capacity����
		size_type capacity() const noexcept
		{
			return this->m_capacity;
		}
		//empty����
		bool empty()const noexcept
		{
			return this->m_size == 0;
		}
		//clear����
		void clear() noexcept
		{
			//�����Լ������ڴ棬���Բ�ʹ��delete����ȡ���øö������������
			//���������Ѿ���ʼ����Ԫ��,Ȼ����ڴ��ͷŵ�
			for (std::size_t i = 0; i < this->m_size; ++i)
				this->m_data[i].~T();
			this->m_size = 0;
		}
		//pop_back����
		void pop_back() noexcept
		{
			assert(!this->empty());
			std::size_t k = this->m_size - 1;
			this->m_data[k].~T(); //�������һ�����ݵ�����
			this->m_size = k;
		}



		//emplace_back����
		//ֱ��������β�����������Ԫ��
		template<typename... ArgsT>
		reference emplace_back(ArgsT&&... args)
		{
			if (this->m_size < this->m_capacity)
			{
				std::size_t k = this->m_size;

				//���������е�ÿ����������ת����Ԫ�صĹ��캯�����������ǵ�ԭʼֵ��𲻱䡣
				// ���������Ա��ⲻ��Ҫ�Ŀ������ƶ��������Ӷ�������ܡ�
				new(&this->m_data[k]) T(std::forward<ArgsT>(args)...);
			}
			std::size_t new_capacity = this->m_size + 1;
			new_capacity |= this->m_size / 2; //* 1.5
			auto new_data = static_cast<T*>(operator new(new_capacity * sizeof(T)));
			std::size_t new_size = 0;

			try
			{
				//ʹ���ˡ���λ new����������placement new�������ѷ�����ڴ��й������
				//����rhs�Ĺ��캯������m_data���ƹ���
				for (std::size_t i = 0; i < this->m_size; ++i)
				{
					new(&new_data[i]) T(std::move(this->m_data[i]));
					++new_size;
				}
				//�ó������ټ�һ
				new(&new_data[new_size]) T(std::forward<ArgsT>(args)...);
				++new_size;
			}
			catch (...)
			{
				for (std::size_t i = 0; i < new_size; ++i)
					new_data[i].~T();
				//Ȼ����ڴ��ͷŵ�
				operator delete(new_data);
				throw;
			}
			this->do_destruct();

			this->m_data = new_data;
			this->m_size = new_size;
			this->m_capacity = new_capacity;
			return new_data[new_size];
		}

		//push_back����
		void push_back(const T& value)
		{
			this->emplace_back(value);
		}

		//push_back���� ��ֵ����
		void push_back(T&& value)
		{
			this->emplace_back(std::move(value));
		}

		//��������ȫ���������٣������ͷ��ڴ�
		void do_destruct() noexcept
		{
			//�����Լ������ڴ棬���Բ�ʹ��delete����ȡ���øö������������
			//���������Ѿ���ʼ����Ԫ��,Ȼ����ڴ��ͷŵ�
			this->clear();
			operator delete (this->m_data);
		}
	};


}

