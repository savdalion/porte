#pragma once

#include <list>
#include <memory>


namespace porte {


/**
* ”чит родител€ хранить слушателей событий класса L.
* ѕредоставл€ет итератор по хранимым событи€м.
* ѕредоставл€ет возможность обнаружить, когда слушатель пропал
* без вызова removeListener().
*/
template< class L >
class StoreListener {
public:
    typedef std::shared_ptr< L >       ListenerPtr;
    typedef std::weak_ptr< L >         ListenerWPtr;
    typedef std::list< ListenerWPtr >  listener_t;


public:
    StoreListener();

    ~StoreListener();


    void addListener( const std::shared_ptr< L > );

    void removeListener( const std::shared_ptr< L > );



    /**
    * @return ѕервый существующий слушатель.
    *         nullptr, если слушателей нет.
    */
    ListenerPtr begin();



    /**
    * @return —ледующий существующий слушатель.
    *         nullptr, если слушателей нет.
    */
    ListenerPtr next();




protected:
    listener_t  mListener;
    mutable typename listener_t::iterator  itr;
};


} // porte






#include "StoreListener.inl"
