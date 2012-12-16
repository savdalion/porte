namespace porte {


template< class L >
inline StoreListener< L >::StoreListener() {
    itr = mListener.begin();
}



template< class L >
inline StoreListener< L >::~StoreListener() {
}




template< class L >
inline void StoreListener< L >::addListener( const std::shared_ptr< L >  lp ) {
    // ���������, ��� ����� ��������� ��� ��� � ������
    for (auto ftr = mListener.begin(); ftr != mListener.end(); ++ftr) {
        assert( ( ( *ftr ).expired() || (lp != ( *ftr ).lock()) )
            && "������� �������� �����-���������. ������ ������������������ ��� ���. ������." );
    }

    // ��������� ������ ���������
    const ListenerWPtr lwp( lp );
    mListener.push_back( lwp );
}




template< class L >
inline void StoreListener< L >::removeListener( const std::shared_ptr< L >  lp ) {
    for (auto ftr = mListener.begin(); ftr != mListener.end(); ++ftr) {
        if ( !( *ftr ).expired() && (lp == ( *ftr ).lock()) ) {
            mListener.erase( ftr );
            // ��������� ��� ���� ������ ���� - ��. addListener()
            return;
        }
    }
}




template< class L >
inline typename StoreListener< L >::ListenerPtr StoreListener< L >::begin() {
    itr = mListener.begin();
    if (itr == mListener.end()) {
        return ListenerPtr( nullptr );
    }

    // ���������� ������� ��� ���� ����. ������ ���������
    return ( *itr ).expired() ? next() : ( *itr ).lock();
}




template< class L >
inline typename StoreListener< L >::ListenerPtr StoreListener< L >::next() {
    // @source http://schneide.wordpress.com/2008/12/08/observerlistener-structures-in-c-with-boosts-smart-pointers
    while (itr != mListener.end()) {
        if ( ( *itr ).expired() ) {
            // ������� ������������� ���������
            itr = mListener.erase( itr );
#ifdef _DEBUG
            std::cout << "(?) ��������� ��� ������� ��� ������ removeListener()." << std::endl;
#endif
        } else {
            ++itr;
            return
                (itr == mListener.end()) ?
                    ListenerPtr( nullptr ) :
                    ( *itr ).lock();
        }
    }
    return ListenerPtr( nullptr );
}




} // porte
