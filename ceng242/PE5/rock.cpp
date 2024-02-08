#include "kpop.h"
#include "metal.h"
#include "rock.h"
#include "jazz.h"

int RockBand::play(MusicBand *other)
{
    double C;
    double score;
    int remainingEnergy;
    if(dynamic_cast<KPopBand*>(other)) C = 0.5;
    else if(dynamic_cast<MetalBand*>(other)) C = 1.4;
    else if(dynamic_cast<RockBand*>(other)) C = 1.0;
    else if(dynamic_cast<JazzBand*>(other)) C = 0.8;
    score = (get_fan_count() + 0.1*get_talent()*get_energy())*C;
    remainingEnergy = 0.9*get_energy();
    set_energy(remainingEnergy);
    return score;
}

void RockBand::rehearse(void) 
{
    int remainingEnergy = get_energy()-(0.5*0.1)*get_energy();
    int talent = get_talent() + 10;
    set_energy(remainingEnergy);
    set_talent(talent);
}