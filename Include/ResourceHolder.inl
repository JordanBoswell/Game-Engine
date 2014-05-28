#include "ResourceHolder.h"



template<typename T_Id, typename T_Resource>
void ResourceHolder<T_Id, T_Resource>::load(T_id id, std::string filename)
{
    std::unique_ptr<T_Resource> resourcePtr(new T_Resource());
    if(resourcePtr->loadFromFile(filename) == false)
        throw std::runtime_error("ResourceHolder::load failed to load " + filename);
    auto insertSuccess = mResourceMap.insert(id, std::move(resourcePtr)).second;
    assert(insertSuccess && "ResourceHolder::load failed to insert resource with filename " + filename);
}


template<typename T_Id, typename T_Resource>
template<typename T_SecondParameter>
void <T_Id, T_Resource>::load(T_id id, std::string filename, T_SecondParameter secondArgument)
{
    std::unique_ptr<T_Resource> resourcePtr(new T_Resource());
    if(resourcePtr->loadFromFile(filename, secondArgument) == false)
        throw std::runtime_error("ResourceHolder::load failed to load " + filename);
    auto insertSuccess = mResourceMap.insert(id, std::move(resourcePtr)).second;
    assert(insertSuccess && "ResourceHolder::load failed to insert resource with filename " + filename);
}


template<typename T_Id, typename T_Resource>
T_Resource& ResourceHolder<T_Id, T_Resource>::get(T_Id)
{
    auto searchResult = mResourceMap.find(T_Id);
    assert(searchResult != mResourceMap.end() && "ResourceHolder::get() failed to find requested resource.");
    return *searchResult->first;
}


template<typename T_Id, typename T_Resource>
const T_Resource& ResourceHolder<T_Id, T_Resource>::get(T_Id)const
{
    auto searchResult = mResourceMap.find(T_Id);
    assert(searchResult != mResourceMap.end() && "ResourceHolder::get() failed to find requested resource.");
    return *searchResult->first;
}