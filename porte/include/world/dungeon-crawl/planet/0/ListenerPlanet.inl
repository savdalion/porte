namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace planet {
                namespace l0 {


namespace pnp = portulan::world::dungeoncrawl::planet::l0;
namespace pniop = portulan::io::world::dungeoncrawl::planet::l0;
namespace pns = portulan::world::dungeoncrawl::starsystem::l0;
namespace pes = porte::world::dungeoncrawl::starsystem::l0;


template< class E >
inline void ListenerPlanet< E >::notifyAndCompletePulse() {
    assert( mEngine &&
        "Движок не указан. Должны были позаботиться наследники." );

#ifdef _DEBUG
    // @test
    std::cout << "Пульс планеты " << mEngine->live() << std::endl;

    // визуализируем
    /* - @todo
    pniop::StatOgre3DVisual::option_t  o;
    o[ "source-options" ] = "visual-planet.json";
    pniop::StatOgre3DVisual  visual( o );
    engine->portulan()->draw( visual );
    */
#endif

    // # Просто заботимся, чтобы слушатели получили это событие.

    // отправляем *своё* событие другим слушателям
    // @see #Соглашение об отправке чужих событий в starsystem::Listener.
    for (auto etr = StoreListenerPlanet::begin();
         etr; etr = StoreListenerPlanet::next()
    ) { if ( etr ) {
        etr->listener.lock()->afterPulse( etr->whose );
    } }
}





template< class E >
inline void ListenerPlanet< E >::afterPulse( AEngine::Ptr whose ) {

    assert( mEngine &&
        "Движок не указан. Должны были позаботиться наследники." );

    // # Первый пульс выполняется всегда. Т.о. обеспечим глубокую инициализацию
    //   портуланов без необходимости делать это отдельно.
    const bool notFirstPulse = !mEngine->live().first();

    // обеспечиваем доступ к движкам и портуланам
    assert( whose && "Не получен движок, которого слушают." );
    const auto engineStarSystem =
        std::static_pointer_cast< pes::Engine >( whose );

    // отрабатываем событие

    // отрабатываем смену дня / ночи
    // # Должны получать верный результат при любом временном шаге
    //   звёздной системы.
    // сколько времени прошло с момента последнего пульса звёздной системы
    const auto timedelta =
        engineStarSystem->live().timelive() - mEngine->live().timelive();
    if ( (timedelta <= 0) && notFirstPulse ) {
        // холостой и обратный ход времени не отрабатываем
        return;
    }

    // сколько это в пульсах планеты
    assert( (mEngine->timestep() > 0)
        && "Шаг времени для планеты должен быть больше 0." );
    const auto pulsedelta =
        Pulse::pulsedelta( timedelta, mEngine->timestep() );
    // дожидаемся, когда время звёздной системы позволит отработать
    // хотя бы 1 пульс планеты (если это не 1-й пульс)
    if ( (pulsedelta == 0) && notFirstPulse ) {
        return;
    }

    // обновляем характеристики портулана планеты
    const auto& topologyStarSystem =
        engineStarSystem->portulan()->topology().topology();
    updatePlanetFromStarSystem( topologyStarSystem );

    // зависимые от звёздной системы характеристики обновлены,
    // переводим планету на след. пульс
    // # Движок не любит отсутствие пульсов.
    // инициализацию от звёздной системы сделали выше
    *mEngine << ( (pulsedelta > 0) ? pulsedelta : 1 );


    // это чужое событие - не отправляем дальше
    // @see #Соглашение об отправке событий в starsystem::Listener.
}





template< class E >
inline void ListenerPlanet< E >::updatePlanetFromStarSystem(
    const pns::topology_t&  topologyStarSystem
) {
    assert( mEngine &&
        "Движок не указан. Должны были позаботиться наследники." );

    auto& topology = mEngine->portulan()->topology().topology();

    // источники освещения
    // звёзды
    assert( (pnp::ILLUMINANCE_STAR_COUNT >= pns::STAR_COUNT)
        && "Модель освещения не может вместить все звёзды звёздной системы. Увеличьте ILLUMINANCE_STAR_COUNT." );
    static const pnp::aboutIlluminanceStar_t ZERO_AIS = {};
    pnp::aboutIlluminanceStar_t* ais = topology.aboutIlluminanceSource.star;
    for (size_t i = 0; i < pns::STAR_COUNT; ++i) {
        const pns::aboutStar_t& as = topologyStarSystem.star.content[ i ];
        /* - признак отсутствия звезды - отрицательная или нулевая масса
        assert( (as.mass >= 0.0)
            && "Масса звезды не может быть меньше 0. Возможно, ошибка при инициализации списка звёзд." );
        */
        if ( pns::absentStar( as ) ) {
            // # Звёзд больше нет.
            // добавляем свой признак - пустой radius
            ais[ i ] = ZERO_AIS;
            break;
        }
        const pnp::aboutIlluminanceStar_t newAIS = {
            static_cast< cl_float >( as.radius ),
            static_cast< cl_float >( as.surfaceTemperature ),
            static_cast< cl_float >( as.luminosity ),
            {
                static_cast< cl_float >( as.coord[ 0 ] ),
                static_cast< cl_float >( as.coord[ 1 ] ),
                static_cast< cl_float >( as.coord[ 2 ] )
            }
        };
        ais[ i ] = newAIS;

    } // for (auto i = 0; i < pns::STAR_COUNT; ++i)
}


                } // l0
            } // planet
        } // dungeoncrawl
    } // world
} // porte
