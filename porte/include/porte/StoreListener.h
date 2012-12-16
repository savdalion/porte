#pragma once

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
    typedef std::list< ListenerWPtr >  listener_t;


public:
    StoreListener();

    ~StoreListener();


    void addListener( const std::shared_ptr< L > );

    void removeListener( const std::shared_ptr< L > );



    /**
    * @return ������ ������������ ���������.
    *         nullptr, ���� ���������� ���.
    */
    ListenerPtr begin();



    /**
    * @return ��������� ������������ ���������.
    *         nullptr, ���� ���������� ���.
    */
    ListenerPtr next();




protected:
    listener_t  mListener;
    mutable typename listener_t::iterator  itr;
};


} // porte






#include "StoreListener.inl"
