namespace porte {


template< size_t SX, size_t SY, size_t SZ >
inline PortulanBooster< SX, SY, SZ >::PortulanBooster(
    portulan_t* portulan
) :
    mPortulan( portulan ) 
{
    assert( portulan && "Карта не задана." );
}





template< size_t SX, size_t SY, size_t SZ >
inline PortulanBooster< SX, SY, SZ >::~PortulanBooster() {
}





template< size_t SX, size_t SY, size_t SZ >
inline typename PortulanBooster< SX, SY, SZ >::portulan_t&  PortulanBooster< SX, SY, SZ >::portulan() {
    return *mPortulan.get();
}




template< size_t SX, size_t SY, size_t SZ >
inline typename PortulanBooster< SX, SY, SZ >::portulan_t const&  PortulanBooster< SX, SY, SZ >::portulan() const {
    return *mPortulan.get();
}




template< size_t SX, size_t SY, size_t SZ >
inline typename PortulanBooster< SX, SY, SZ >::booster_t&  PortulanBooster< SX, SY, SZ >::booster() {
    return mBooster;
}




template< size_t SX, size_t SY, size_t SZ >
inline typename PortulanBooster< SX, SY, SZ >::booster_t const&  PortulanBooster< SX, SY, SZ >::booster() const {
    return mBbooster;
}





template< size_t SX, size_t SY, size_t SZ >
void PortulanBooster< SX, SY, SZ >::toBooster() {

    // температура
    std::copy(
        mPortulan->topology().temperature().raw().cbegin(),
        mPortulan->topology().temperature().raw().cend(),
        std::begin( mBooster.temperature )
    );

}





template< size_t SX, size_t SY, size_t SZ >
void PortulanBooster< SX, SY, SZ >::fromBooster() {

    // температура
    std::copy(
        std::begin( mBooster.temperature ),
        std::end( mBooster.temperature ),
        mPortulan->topology().temperature().raw().begin()
    );

}




} // porte
