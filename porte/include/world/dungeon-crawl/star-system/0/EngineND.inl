namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pd = portulan::world::dungeoncrawl::starsystem::l0;


inline EngineND::EngineND(
    portulan_t* p,
    pd::real_t extent,
    pd::real_t timestep
) :
    EngineWithoutBooster( p ),
    mExtent( extent ),
    mTimestep( timestep ),
    mPhysicsWorld( NewtonCreate() )
{
    assert( mPhysicsWorld && "Не удалось инициализировать физ. движок." );

    // создаём мир Newton Dynamics (ND)
    NewtonSetPlatformArchitecture( mPhysicsWorld, 0 );
	NewtonSetSolverModel( mPhysicsWorld, 0 );
    //NewtonSetMinimumFrameRate( mPhysicsWorld, 100.0f );

    const pd::real_t halfExtent = extent / 2.0;
	const dgVector minSize( -halfExtent, -halfExtent, -halfExtent, 1.0f );
	const dgVector maxSize(  halfExtent,  halfExtent,  halfExtent, 1.0f );
	NewtonSetWorldSize( mPhysicsWorld, &minSize[0], &maxSize[0] );

    /* @test
    // #! В ND rev2086 dgVector, строка 328 убрать _ASSERTE.
    NewtonCollision* collision = NewtonCreateSphere(
        mPhysicsWorld,
        10.0f, 20.0f, 30.0f,
        0, nullptr
    );
    */

    // наполняем мир ND телами
    for (size_t i = 0; i < pd::BODY_COUNT; ++i) {
        pd::aboutBody_t* b =
            &mPortulan->topology().topology().body.content[ i ];
        incarnateBody( b );
    }
}





inline EngineND::~EngineND() {
#ifdef _DEBUG
    // грубо гасим assert'ы NewtonDynamics при выходе из приложения
    // @todo fine Сделать красиво.
    exit( 0 );
#endif
    NewtonDestroy( mPhysicsWorld );
}






inline void EngineND::sync() {
    // @todo ...
}






inline void EngineND::pulse( int n ) {
    // выполняем 'n' циклов
    for (int i = 0; i < n; ++i) {
        pulse();
    }
}






inline void EngineND::pulse() {
    NewtonUpdate( mPhysicsWorld, mTimestep );

    // # Координаты тел обновляются NewtonDynamics - берём их напрямую.
}





inline void EngineND::incarnateBody( pd::aboutBody_t* b ) {
    assert( mPhysicsWorld && "Физ. движок. не инициализирован." );

    // # Поля для физ. тел. могут располагаться в любом порядке.
    // @todo bad optimize Утверждённый порядок = возможность оптимизации.
    pd::aboutPlanet_t* contentPlanet = nullptr;
    pd::aboutStar_t*   contentStar = nullptr;
    pd::real_t mass = 0.0;
    pd::real_t radius = 0.0;
    pd::real_t coord[ 3 ] = {};
    pd::real_t rotation[ 3 ] = {};
    pd::real_t* tm = nullptr;
    NewtonCollision* collision = nullptr;
    switch ( b->group ) {
        case pd::GE_PLANET:
            contentPlanet = &b->content.planet;
            mass = contentPlanet->mass;
            radius = contentPlanet->radius;
            std::copy_n( contentPlanet->coord, 3, coord );
            std::copy_n( contentPlanet->rotation, 3, rotation );
            // #! В ND rev2086 dgVector, строка 328 убрать _ASSERTE.
            collision = NewtonCreateSphere(
                mPhysicsWorld,
                radius, radius, radius,
                0, nullptr
            );
            tm = contentPlanet->tm;
            break;

        case pd::GE_STAR:
            contentStar = &b->content.star;
            mass = contentStar->mass;
            radius = contentStar->radius;
            std::copy_n( contentStar->coord, 3, coord );
            std::copy_n( contentStar->rotation, 3, rotation );
            // #! В ND rev2086 dgVector, строка 328 убрать _ASSERTE.
            collision = NewtonCreateSphere(
                mPhysicsWorld,
                radius, radius, radius,
                0, nullptr
            );
            tm = contentStar->tm;
            break;

        default:
            // не обрабатываем другие элементы
            return;
    }

    init( tm, coord, rotation );
    auto body = NewtonCreateBody( mPhysicsWorld, collision, tm );

    // каждая форма обладает своим моментом инерции
    // @source http://ru.wikipedia.org/wiki/%D0%9C%D0%BE%D0%BC%D0%B5%D0%BD%D1%82_%D0%B8%D0%BD%D0%B5%D1%80%D1%86%D0%B8%D0%B8
    const pd::real_t momentXYZ =
        2.0 * mass * radius * radius / 5.0f;
    NewtonBodySetMassMatrix(
        body, mass,  momentXYZ, momentXYZ, momentXYZ
    );

    // конфигурируем физ. тело
    NewtonBodySetUserData( body, b );

    NewtonBodySetForceAndTorqueCallback( body, applyForceAndTorque );
    NewtonBodySetTransformCallback(      body, setTransform        );
    NewtonBodySetDestructorCallback(     body, autoDestruction     );

    // говорим, когда телу остановиться
    NewtonBodySetAutoSleep( body, 1 );
    NewtonBodySetLinearDamping( body, 0.01f );
    NewtonBodySetAngularDamping( body, &dgVector( 0.01f, 0.1f, 0.01f, 1.0f )[0] );
}






/**
* Инициирует матрицу.
*/
void EngineND::init(
    pd::real_t tm[ 16 ],
    const pd::real_t coord[ 3 ],
    const pd::real_t rotation[ 3 ]
) {
    // пространственное положение элемента
    static const pd::real_t PI_DIV_180 =
        static_cast< pd::real_t >( M_PI / 180.0 );
	const dgMatrix m(
        dgRollMatrix(  rotation[ 0 ] * PI_DIV_180 ) *
        dgYawMatrix(   rotation[ 1 ] * PI_DIV_180 ) *
        dgPitchMatrix( rotation[ 2 ] * PI_DIV_180 )
    );
    const dgMatrix t(
        dgQuaternion( m ),
        dgVector( coord[ 0 ], coord[ 1 ], coord[ 2 ], 1.0f )
    );
    tm[0]  = t[0][0];   tm[1]  = t[0][1];   tm[2]  = t[0][2];   tm[3]  = t[0][3];
    tm[4]  = t[1][0];   tm[5]  = t[1][1];   tm[6]  = t[1][2];   tm[7]  = t[1][3];
    tm[8]  = t[2][0];   tm[9]  = t[2][1];   tm[10] = t[2][2];   tm[11] = t[2][3];
    tm[12] = t[3][0];   tm[13] = t[3][1];   tm[14] = t[3][2];   tm[15] = t[3][3];
}





inline void EngineND::applyForceAndTorque(
    const NewtonBody* b,
    pd::real_t timestep,
    int threadIndex
) {
    auto pw = NewtonBodyGetWorld( b );
    NewtonBody* body = NewtonWorldGetFirstBody( pw );
    while ( body ) {
        if (b != body) {
            const auto ud =
                static_cast< pd::aboutBody_t* >( NewtonBodyGetUserData( body ) );

            // @todo ...

        } // if (mBody != body)

        body = NewtonWorldGetNextBody( pw, body );

    } // while ( body )
}




inline void EngineND::setTransform(
    const NewtonBody* b,
    const pd::real_t* matrix,
    int threadIndex
) {
    auto body = static_cast< pd::aboutBody_t* >( NewtonBodyGetUserData( b ) );
    assert( body && "Тело не найдено." );
#if 1
    const dgMatrix& m = *( ( const dgMatrix* )( matrix ) );
#else
    // корректируем координаты и углы поворота тела: работаем в 2D
    const dgMatrix m(
        dgVector( matrix[0],  matrix[1],  0,  0 ),
        dgVector( matrix[4],  matrix[5],  0,  0 ),
        dgVector( 0,          0,          1,  0 ),
        dgVector( matrix[12], matrix[13], 0,  0 )
    );
    NewtonBodySetMatrix( b, &m[0][0] );
#endif
    // # Движок использует напрямую координаты из матрицы физ. тела:
    //   нет надобности в обновлении.
}



inline void EngineND::autoDestruction( const NewtonBody* b ) {
    //auto body = static_cast< pd::aboutBody_t* >( NewtonBodyGetUserData( b ) );
    //assert( body && "Тело не найдено." );
    // предотвращаем бесконечную рекурсию, удаляем физ. тело
    NewtonBodySetDestructorCallback( b, nullptr );
    NewtonBodySetUserData( b, nullptr );
    delete b;
}



                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
