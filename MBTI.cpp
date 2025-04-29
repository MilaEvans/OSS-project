#include "MBTI.h"

#include <iostream>
#include <vector>
#include <string>

void User::addTag(const std::string& tag)
{
	tags.push_back(tag);
}

const std::vector<std::string>& User::getTags() const 
{
	return tags;
}
