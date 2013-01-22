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
    assert( lp    && "Не указан слушатель." );
    //assert( whose && "Не указан движок, которого слушают." );
    //assert( who   && "Не указан движок, который слушает." );

    // проверяем, что этого слушателя ещё нет в списке
    for (auto ftr = mListener.begin(); ftr != mListener.end(); ++ftr) {
        const ListenerWPtr f = ftr->listener;
        assert( ( f.expired() || (lp != f.lock()) )
            && "Попытка добавить дубль-слушателя. Потеря производительности или лог. ошибка." );
    }

    // добавляем нового слушателя
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
inline bool StoreListener< L >::empty() const {
    return mListener.empty();
}




template< class L >
inline typename StoreListener< L >::store_t* StoreListener< L >::begin() {
    itr = mListener.begin();
    if (itr == mListener.end()) {
        return nullptr;
    }

    // возвращаем первого или ищем след. живого слушателя
    const ListenerWPtr f = itr->listener;
#ifdef _DEBUG
    if ( f.expired() ) {
        assert( false
            && "Слушатель потерян без явного вызова removeListener()." );
    }
#endif

    return f.expired() ? next() : &( *itr );
}




template< class L >
inline typename StoreListener< L >::store_t* StoreListener< L >::next() {
    // @source http://schneide.wordpress.com/2008/12/08/observerlistener-structures-in-c-with-boosts-smart-pointers
    while (itr != mListener.end()) {
        const ListenerWPtr f = itr->listener;
        if ( f.expired() ) {
            // удаляем потерявшегося слушателя
            itr = mListener.erase( itr );
#ifdef _DEBUG
            //std::cout << "(?) Слушатель потерян без явного вызова removeListener()." << std::endl;
            assert( false
                && "Слушатель потерян без явного вызова removeListener()." );
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

    // возвращаем живого слушателя или nullptr
    const ListenerWPtr f = itr->listener;
    return f.expired() ? nullptr : &( *itr );
}




} // porte
