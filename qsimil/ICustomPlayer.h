#ifndef ICUSTOMPLAYER_H
#define ICUSTOMPLAYER_H

namespace qsimil {
    class ICustomPlayer {
    public:
        virtual ~ICustomPlayer( void ) { }
        virtual void cpPlay( ) = 0;
        virtual void cpPause( ) = 0;
        virtual void cpGoInit( ) = 0;
        virtual void cpGoEnd( ) = 0;
        virtual void cpStop( ) = 0;
        virtual void cpClickRestart( bool restart ) = 0;
        virtual void cpOnPlayAt( float percentage ) = 0;

        virtual void cpOnReset( void ) { }

        virtual void cpChangeStepsPerSecond( float t ) = 0;
        virtual void cpChangeStepDeltaTime( float t ) = 0;

        virtual void cpChangeGIDSimulation( std::vector< uint32_t >& _gids ) = 0;
    };
};

#endif // ICUSTOMPLAYER_H
