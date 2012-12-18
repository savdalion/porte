#pragma once

#include "AEngine.h"
#include <list>
#include <memory>


namespace porte {


/**
* ���� �������� ������� ���������� ������� ������ L.
* ������������� �������� �� �������� ��������.
* ������������� ����������� ����������, ����� ��������� ������
* ��� ������ removeListener().
*/
template< class L >
class StoreListener {
public:
    typedef std::shared_ptr< L >       ListenerPtr;
    typedef std::weak_ptr< L >         ListenerWPtr;

    /**
    * ��������� ��� �������� ���������� � ��������� � ���� �������.
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
    * # ������ ���������, ���������� ���������� (������ ��� ������� � ����
    *   �������): ��� �������� �������� ������ �� ���� ���������� ���� �
    *   ������������ ������� ��� ������������� ���������� �� � ������
    *   �������.
    */
    void addListener( const std::shared_ptr< L >,  AEngine::Ptr whose,  AEngine::Ptr who );

    void removeListener( const std::shared_ptr< L > );



    /**
    * @return ������ ������������ ���������.
    *         nullptr, ���� ���������� ���.
    */
    store_t* begin();



    /**
    * @return ��������� ������������ ���������.
    *         nullptr, ���� ���������� ���.
    */
    store_t* next();



    /**
    * @return ���������, �� ������� ��������� ��������.
    *         nullptr, ���� �������� ��������� �� ����� ������ ���
    *         ��������������� ���������.
    *         �������� �� ������������, ������ �� ��������.
    */
    store_t* current() const;




protected:
    listener_t  mListener;
    typename listener_t::iterator  itr;
};


} // porte






#include "StoreListener.inl"
