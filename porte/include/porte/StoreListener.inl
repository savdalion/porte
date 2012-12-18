namespace porte {


template< class L >
inline StoreListener< L >::StoreListener() {
    itr = mListener.begin();
}



template< class L >
inline StoreListener< L >::~StoreListener() {
}




template< class L >
inline void StoreListener< L >::addListener(
    const std::shared_ptr< L >  lp,
    AEngine::Ptr whose,  AEngine::Ptr who
) {
    assert( lp    && "�� ������ ���������." );
    assert( whose && "�� ������ ������, �������� �������." );
    assert( who   && "�� ������ ������, ������� �������." );

    // ���������, ��� ����� ��������� ��� ��� � ������
    for (auto ftr = mListener.begin(); ftr != mListener.end(); ++ftr) {
        const ListenerWPtr f = ftr->listener;
        assert( ( f.expired() || (lp != f.lock()) )
            && "������� �������� �����-���������. ������ ������������������ ��� ���. ������." );
    }

    // ��������� ������ ���������
    const ListenerWPtr lwp( lp );
    const store_t store = { lwp, whose, who };
    mListener.push_back( store );
}




template< class L >
inline void StoreListener< L >::removeListener( const std::shared_ptr< L >  lp ) {
    for (auto ftr = mListener.begin(); ftr != mListener.end(); ++ftr) {
        const ListenerWPtr f = ftr->listener;
        if ( !f.expired() && (lp == f.lock()) ) {
            mListener.erase( ftr );
            // ��������� ��� ���� ������ ���� - ��. addListener()
            return;
        }
    }
}




template< class L >
inline typename StoreListener< L >::store_t* StoreListener< L >::begin() {
    itr = mListener.begin();
    if (itr == mListener.end()) {
        return nullptr;
    }

    // ���������� ������� ��� ���� ����. ������ ���������
    const ListenerWPtr f = itr->listener;
    return f.expired() ? next() : &( *itr );
}




template< class L >
inline typename StoreListener< L >::store_t* StoreListener< L >::next() {
    // @source http://schneide.wordpress.com/2008/12/08/observerlistener-structures-in-c-with-boosts-smart-pointers
    while (itr != mListener.end()) {
        const ListenerWPtr f = itr->listener;
        if ( f.expired() ) {
            // ������� ������������� ���������
            itr = mListener.erase( itr );
#ifdef _DEBUG
            std::cout << "(?) ��������� ��� ������� ��� ������ removeListener()." << std::endl;
#endif
        } else {
            ++itr;
            return (itr == mListener.end()) ? nullptr : &( *itr );
        }
    }
    return nullptr;
}




template< class L >
inline typename StoreListener< L >::store_t* StoreListener< L >::current() const {
    if (itr == mListener.cend()) {
        return nullptr;
    }

    // ���������� ������ ��������� ��� nullptr
    const ListenerWPtr f = itr->listener;
    return f.expired() ? nullptr : &( *itr );
}




} // porte
