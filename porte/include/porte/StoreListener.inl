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
    // провер€ем, что этого слушател€ ещЄ нет в списке
    for (auto ftr = mListener.begin(); ftr != mListener.end(); ++ftr) {
        assert( ( ( *ftr ).expired() || (lp != ( *ftr ).lock()) )
            && "ѕопытка добавить дубль-слушател€. ѕотер€ производительности или лог. ошибка." );
    }

    // добавл€ем нового слушател€
    const ListenerWPtr lwp( lp );
    mListener.push_back( lwp );
}




template< class L >
inline void StoreListener< L >::removeListener( const std::shared_ptr< L >  lp ) {
    for (auto ftr = mListener.begin(); ftr != mListener.end(); ++ftr) {
        if ( !( *ftr ).expired() && (lp == ( *ftr ).lock()) ) {
            mListener.erase( ftr );
            // слушатель мог быть только один - см. addListener()
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

    // возвращаем первого или ищем след. живого слушател€
    return ( *itr ).expired() ? next() : ( *itr ).lock();
}




template< class L >
inline typename StoreListener< L >::ListenerPtr StoreListener< L >::next() {
    // @source http://schneide.wordpress.com/2008/12/08/observerlistener-structures-in-c-with-boosts-smart-pointers
    while (itr != mListener.end()) {
        if ( ( *itr ).expired() ) {
            // удал€ем потер€вшегос€ слушател€
            itr = mListener.erase( itr );
#ifdef _DEBUG
            std::cout << "(?) —лушатель был потер€н без вызова removeListener()." << std::endl;
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
