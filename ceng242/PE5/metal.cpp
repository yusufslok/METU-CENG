#include "kpop.h"
#include "metal.h"
#include "rock.h"
#include "jazz.h"

int MetalBand::play(MusicBand *other)
{
    double C;
    double score;
    int remainingEnergy;
    if(dynamic_cast<KPopBand*>(other)) C = 0.5;
    else if(dynamic_cast<MetalBand*>(other)) C = 1.0;
    else if(dynamic_cast<RockBand*>(other)) C = 1.5;
    else if(dynamic_cast<JazzBand*>(other)) C = 1.1;
    score = (get_fan_count() + 0.1*get_talent()*get_energy())*C;
    remainingEnergy = 0.84*get_energy();
    set_energy(remainingEnergy);
    return score;
}

void MetalBand::rehearse(void) 
{
    int remainingEnergy = get_energy()-(0.5*0.16)*get_energy();
    int talent = get_talent() - 5;
    set_energy(remainingEnergy);
    set_talent(talent);
    
}