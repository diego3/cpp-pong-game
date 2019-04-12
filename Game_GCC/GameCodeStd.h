#ifndef GAMECODESTD_H_INCLUDED
#define GAMECODESTD_H_INCLUDED

#include <cctype>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <list>
#include <queue>
#include <memory>
#include <time.h>
#include "tinyxml.h"

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>

#if defined(_DEBUG)
#	define GCC_NEW new(_NORMAL_BLOCK,__FILE__, __LINE__)
#else
#	define GCC_NEW new
#endif

#if !defined(SAFE_DELETE)
	#define SAFE_DELETE(x) if(x) delete x; x=NULL;
#endif

#if !defined(SAFE_DELETE_ARRAY)
	#define SAFE_DELETE_ARRAY(x) if (x) delete [] x; x=NULL;
#endif

#if !defined(SAFE_RELEASE)
	#define SAFE_RELEASE(x) if(x) x->Release(); x=NULL;
#endif


#define TOSTRING( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

#define STRING2INT(str) std::atoi(str.c_str())

#define POINTS_TO_WIN 5
#define BALL_MAX_VEL 22
#define BALL_INITIAL_VEL -5
#define SCREEN_W 800
#define SCREEN_H 600
#define LOGINSCREEN_W 300
#define LOGINSCREEN_H 200


inline std::string trim(const std::string &s){
   auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
   auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
   return (wsback<=wsfront ? std::string() : std::string(wsfront,wsback));
}


template<class Type>
std::shared_ptr<Type> MakeStrongPrt(std::weak_ptr<Type> pWeakType){
    if(!pWeakType.expired()){
        return std::shared_ptr<Type>(pWeakType);
    }
    return std::shared_ptr<Type>();
}

template <class BaseType, class SubType>
BaseType* GenericObjectCreationFunction(void) { return GCC_NEW SubType; }

template <class BaseClass, class IdType>
class GenericObjectFactory
{
    typedef BaseClass* (*ObjectCreationFunction)(void);
    std::map<IdType, ObjectCreationFunction> m_creationFunctions;
public:
    /*
        Register usage example:
        m_componentFactory.Register<TransformComponent>(ActorComponent::GetIdFromName(TransformComponent::s_Name));
    */
    template <class SubClass>
    bool Register(IdType id){
        auto findIt = m_creationFunctions.find(id);
        if (findIt == m_creationFunctions.end()){
            m_creationFunctions[id] = &GenericObjectCreationFunction<BaseClass, SubClass>;  // insert() is giving me compiler errors
            return true;
        }
        return false;
    }

    BaseClass* Create(IdType id){
        auto findIt = m_creationFunctions.find(id);
        if (findIt != m_creationFunctions.end()){
            ObjectCreationFunction pFunc = findIt->second;
            return pFunc();
        }
        return nullptr;
    }
};




#endif // GAMECODESTD_H_INCLUDED
