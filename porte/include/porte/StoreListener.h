#pragma once

#include "AEngine.h"
#include <list>
#include <memory>


namespace porte {


/**
* ”чит родител€ хранить слушателей событий класса L.
* ѕредоставл€ет итератор по хранимым событи€м.
* ѕредоставл€ет возможность обнаружить, когда слушатель пропал
* без необходимости €вно вызывать removeListener().
*/
template< class L >
class StoreListener {
public:
    typedef std::shared_ptr< L >       ListenerPtr;
    typedef std::weak_ptr< L >         ListenerWPtr;

    /**
    * —труктура дл€ хранени€ слушателей и св€занных с ними движков.
    */
    typedef struct {
        ListenerWPtr  listener;
        AEngine::Ptr  whose;
        AEngine::Ptr  who;
    } store_t;

    typedef std::list< store_t >  listener_t;




public:
    StoreListener();




    ~StoreListener();




    /**
    * # ѕомимо слушател€, запоминаем участников (движки кто слушает и кого
    *   слушают): это позволит получать доступ ко всем структурам мира и
    *   возможност€м движков без необходимости передавать их в каждом
    *   событии.
    */
    void addListener(
        const std::shared_ptr< L >,
        AEngine::Ptr whose,
        AEngine::Ptr who
    );




    void removeListener( const std::shared_ptr< L > );




    /**
    * @return —лушателей нет.
    */
    bool empty() const;




    /**
    * @return ѕервый существующий слушатель.
    *         nullptr, если слушателей нет.
    */
    store_t* begin();




    /**
    * @return —ледующий существующий слушатель.
    *         nullptr, если слушателей нет.
    */
    store_t* next();




    /**
    * @return —лушатель, на который указывает итератор.
    *         nullptr, если итератор указывает на конец списка или
    *         несуществующего слушател€.
    *         »тератор не перемещаетс€, список не мен€етс€.
    */
    store_t* current() const;




protected:
    listener_t  mListener;
    typename listener_t::iterator  itr;
};


} // porte






#include "StoreListener.inl"
