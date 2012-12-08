namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pd = portulan::world::dungeoncrawl::starsystem::l0;


inline EngineCPU::EngineCPU(
    portulan_t* p,
    pd::real_t timestep
) :
    EngineWithoutBooster( p ),
    mTimestep( timestep )
{
}





inline EngineCPU::~EngineCPU() {
}






inline void EngineCPU::pulse( int n ) {
    // ��������� 'n' ������
    for (int i = 0; i < n; ++i) {
        pulse();
    }
}






inline void EngineCPU::pulse() {

    auto& body = mPortulan->topology().topology().body.content;

    // ������������ ���� ���������� ��� ��� ������� �������
    for (size_t i = 0; i < pd::BODY_COUNT; ++i) {
        auto& b = body[ i ];
        // # ���� ��� ���. ���. ����� ������������� � ����� �������.
        // @todo bad optimize ����������� ������� = ����������� �����������.
        switch ( b.group ) {
            case pd::GE_PLANET:
                planetInteraction( &b.content.planet, i );
                break;

            case pd::GE_STAR:
                starInteraction( &b.content.star, i );
                break;

            default:
                // �� ������������ ������ ��������
                break;
        }

    } // for (size_t i = 0; i < pd::BODY_COUNT; ++i)

}





inline void EngineCPU::planetInteraction(
    pd::aboutPlanet_t* p,
    size_t currentI
) {
    auto& body = mPortulan->topology().topology().body.content;

    // ������������ �������������� ����
    typelib::vectorDouble_t force = typelib::vectorDouble_t::ZERO();

    const typelib::coordDouble_t coordA( p->coord );
    for (size_t k = 0; k < pd::BODY_COUNT; ++k) {
        const auto& b = body[ k ];
        if (currentI == k) {
            // ���� �� ���� �� ���������
            continue;
        }

        // ���� ����������, ����������� �� ������� �� �������
        // ���� � �������� 'k'

        switch ( b.group ) {
            case pd::GE_PLANET:
                force += static_cast< typelib::coordDouble_t >(
                    forceGravityBodyBodyInteraction(
                        coordA,   typelib::coordDouble_t( b.content.planet.coord ),
                        p->mass,  b.content.planet.mass,
                        // �� ���� ���������� ���� �� ���������
                        std::max( p->radius, b.content.planet.radius )
                ) );
                break;

            case pd::GE_STAR:
                force += static_cast< typelib::coordDouble_t >(
                    forceGravityBodyBodyInteraction(
                        coordA,   typelib::coordDouble_t( b.content.star.coord ),
                        p->mass,  b.content.star.mass,
                        // �� ���� ���������� ���� �� ���������
                        std::max( p->radius, b.content.star.radius )
                ) );
                break;

            default:
                // ������ ���� �� ��������� �� �������
                continue;
        }

    } // for (size_t k = 0; k < pd::BODY_COUNT; ++k)


    // ���������� ����
    p->force[ 0 ] = force.x;
    p->force[ 1 ] = force.y;
    p->force[ 2 ] = force.z;

    // ����� ��������
    const double f = force.normalize();
    const double acceleration = f / p->mass;
    const double v = acceleration * mTimestep;
    p->velocity[ 0 ] += force.x * v;
    p->velocity[ 1 ] += force.y * v;
    p->velocity[ 2 ] += force.z * v;

    // ����� ����������
    p->coord[ 0 ] += p->velocity[ 0 ] * mTimestep;
    p->coord[ 1 ] += p->velocity[ 1 ] * mTimestep;
    p->coord[ 2 ] += p->velocity[ 2 ] * mTimestep;
    
    /* @test
    p->test[ 1 ] = f;
    p->test[ 2 ] = acceleration;
    p->test[ 3 ] = v;
    */
}





inline void EngineCPU::starInteraction(
    pd::aboutStar_t* s,
    size_t currentI
) {
    auto& body = mPortulan->topology().topology().body.content;

    // ������������ �������������� ����
    typelib::vectorDouble_t force = typelib::vectorDouble_t::ZERO();

    const typelib::coordDouble_t coordA( s->coord );
    for (size_t k = 0; k < pd::BODY_COUNT; ++k) {
        const auto& b = body[ k ];
        if (currentI == k) {
            // ���� �� ���� �� ���������
            continue;
        }

        // ���� ����������, ����������� �� ������ �� �������
        // ���� � �������� 'k'

        switch ( b.group ) {
            case pd::GE_PLANET:
                force += static_cast< typelib::coordDouble_t >(
                    forceGravityBodyBodyInteraction(
                        coordA,   typelib::coordDouble_t( b.content.planet.coord ),
                        s->mass,  b.content.planet.mass,
                        // �� ���� ���������� ���� �� ���������
                        std::max( s->radius, b.content.planet.radius )
                ) );
                break;

            case pd::GE_STAR:
                force += static_cast< typelib::coordDouble_t >(
                    forceGravityBodyBodyInteraction(
                        coordA,   typelib::coordDouble_t( b.content.star.coord ),
                        s->mass,  b.content.star.mass,
                        // �� ���� ���������� ���� �� ���������
                        std::max( s->radius, b.content.star.radius )
                ) );
                break;

            default:
                // ������ ���� �� ��������� �� ������
                continue;
        }

    } // for (size_t k = 0; k < pd::BODY_COUNT; ++k)


    // ���������� ����
    s->force[ 0 ] = force.x;
    s->force[ 1 ] = force.y;
    s->force[ 2 ] = force.z;

    // ����� ��������
    const double f = force.normalize();
    const double acceleration = f / s->mass;
    const double v = acceleration * mTimestep;
    s->velocity[ 0 ] += force.x * v;
    s->velocity[ 1 ] += force.y * v;
    s->velocity[ 2 ] += force.z * v;

    // ����� ����������
    s->coord[ 0 ] += s->velocity[ 0 ] * mTimestep;
    s->coord[ 1 ] += s->velocity[ 1 ] * mTimestep;
    s->coord[ 2 ] += s->velocity[ 2 ] * mTimestep;
    
    /* @test
    s->test[ 1 ] = f;
    s->test[ 2 ] = acceleration;
    s->test[ 3 ] = v;
    */
}





inline typelib::vectorDouble_t
EngineCPU::forceGravityBodyBodyInteraction(
    const typelib::coordDouble_t&  coordA,
    const typelib::coordDouble_t&  coordB,
    pd::real_t massA,
    pd::real_t massB,
    pd::real_t noForceDistance
) {
    // ����������
    typelib::vectorDouble_t r = coordB - coordA;
    //const double distanceSquared = r.x * r.x + r.y * r.y + r.z * r.z;
    const double distance = r.normalize();
    if (distance <= noForceDistance) {
        // ��� ������������� ��������� ����
        return typelib::vectorDouble_t::ZERO();
    }

    // ���� ����������
    const double f =
        typelib::constant::physics::G * massA * massB /
        (distance * distance);

    return r * f;
}



                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
