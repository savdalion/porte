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
    assert( lp    && "Ќе указан слушатель." );
    assert( whose && "Ќе указан движок, которого слушают." );
    assert( who   && "Ќе указан движок, который слушает." );

    // провер€ем, что этого слушател€ ещЄ нет в списке
    for (auto ftr = mListener.begin(); ftr != mListener.end(); ++ftr) {
        const ListenerWPtr f = ftr->listener;
        assert( ( f.expired() || (lp != f.lock()) )
            && "ѕопытка добавить дубль-слушател€. ѕотер€ производительности или лог. ошибка." );
    }

    // добавл€ем нового слушател€
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
            // слушатель мог быть только один - см. addListener()
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

    // возвращаем первого или ищем след. живого слушател€
    const ListenerWPtr f = itr->listener;
    return f.expired() ? next() : &( *itr );
}




template< class L >
inline typename StoreListener< L >::store_t* StoreListener< L >::next() {
    // @source http://schneide.wordpress.com/2008/12/08/observerlistener-structures-in-c-with-boosts-smart-pointers
    while (itr != mListener.end()) {
        const ListenerWPtr f = itr->listener;
        if ( f.expired() ) {
            // удал€ем потер€вшегос€ слушател€
            itr = mListener.erase( itr );
#ifdef _DEBUG
            std::cout << "(?) —лушатель был потер€н без вызова removeListener()." << std::endl;
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

    // возвращаем живого слушател€ или nullptr
    const ListenerWPtr f = itr->listener;
    return f.expired() ? nullptr : &( *itr );
}




} // porte
