#ifndef ResourceHolder_h
#define ResourceHolder_h



#include <cassert>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>



//Basic generic resource holder with a std::map underlying container.
template<typename T_Id, typename T_Resource>
class ResourceHolder
{
public:
    void load(T_Id id, std::string filename);
    //Classes such as sf::Shader have a loadFromFile function with two arguments
    template<T_SecondParameter>
    void load(T_Id id, std::string filename, T_SecondParameter secondArgument);
    T_Resource& get(T_Id);
    const T_Resource& get(T_Id)const;
    
private:
    //Uses unique_ptr to store noncopyable objects and to ensure RAII
    std::map<T_Id, std::unique_ptr<T_Resource>> mResourceMap;  
};

#include "ResourceHolder.inl"


#endif