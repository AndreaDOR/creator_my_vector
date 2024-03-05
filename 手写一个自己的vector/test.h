#pragma once
#include <iostream>
#include "MyVector.h"
#include "myVector2.h"
void testMyVector()
{
	My::myVector<std::string> string_vec;
	string_vec.push_back("¹þ¹þ");
	string_vec.emplace_back();
	string_vec.emplace_back("hello2",4);
	for (auto& t : string_vec)
	{
		std::cout << t << std::endl;
	}
}

void testMyVector2()
{

}

