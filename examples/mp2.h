#ifndef MP2_H
#define MP2_H

#include <qsimil/qsimil.h>
#include <iostream>

class MP2: public qsimil::CustomPlayer
{
public:
    virtual void onPlay() {
        std::cout << "PLAY!!!" << std::endl;
    }
    virtual void onPause() {
        std::cout << "PAUSE!!!" << std::endl;
    }
    virtual void onStop() {
        std::cout << "STOP!!!" << std::endl;
    }
    virtual void onRestart() {
        std::cout << "GO BACK!!!" << std::endl;
    }
    virtual void onGoToEnd() {
        std::cout << "GO END!!!" << std::endl;
    }
    virtual void onRepeat( bool status ) {
        std::cout << "REPEAT " << std::boolalpha << status << "!!!"<< std::endl;
    }
    virtual void onPlayAt( float percentage ) {
        std::cout << "onPlayAt " << percentage << "!!!" << std::endl;
    }
    virtual ~MP2( void ) { }
};

#endif // MP2_H
