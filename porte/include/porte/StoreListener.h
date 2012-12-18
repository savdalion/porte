#pragma once

#include "AEngine.h"
#include <list>
#include <memory>


namespace porte {


/**
* Учит родителя хранить слушателей событий класса L.
* Предоставляет итератор по хранимым событиям.
* Предоставляет возможность обнаружить, когда слушатель пропал
* без вызова removeListener().
*/
template< class L >
class StoreListener {
public:
    typedef std::shared_ptr< L >       ListenerPtr;
    typedef std::weak_ptr< L >         ListenerWPtr;

    /**
    * Структура для хранения слушателей и связанных с ними движков.
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
    * # Помимо слушателя, запоминаем участников (движки кто слушает и кого
    *   слушают): это позволит получать доступ ко всем структурам мира и
    *   возможностям движков без необходимости передавать их в каждом
    *   событии.
    */
    void addListener( const std::shared_ptr< L >,  AEngine::Ptr whose,  AEngine::Ptr who );

    void removeListener( const std::shared_ptr< L > );



    /**
    * @return Первый существующий слушатель.
    *         nullptr, если слушателей нет.
    */
    store_t* begin();



    /**
    * @return Следующий существующий слушатель.
    *         nullptr, если слушателей нет.
    */
    store_t* next();



    /**
    * @return Слушатель, на который указывает итератор.
    *         nullptr, если итератор указывает на конец списка или
    *         несуществующего слушателя.
    *         Итератор не перемещается, список не меняется.
    */
    store_t* current() const;




protected:
    listener_t  mListener;
    typename listener_t::iterator  itr;
};


} // porte






#include "StoreListener.inl"
