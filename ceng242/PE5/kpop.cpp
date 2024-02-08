#include "kpop.h"
#include "metal.h"
#include "rock.h"
#include "jazz.h"

int KPopBand::play(MusicBand* other) {
    double c;
    double score;
    int remainingEnergy;

    if (dynamic_cast<KPopBand*>(other))
        c = 2.0;
    else if (dynamic_cast<MetalBand*>(other))
        c = 0.5;
    else if (dynamic_cast<RockBand*>(other))
        c = 0.5;
    else if (dynamic_cast<JazzBand*>(other))
        c = 0.5;

    score = ((get_fan_count() + 0.1 * get_talent() * get_energy()) * c);
    remainingEnergy = (0.8 * get_energy());
    set_energy(remainingEnergy);

    return score;
}
void KPopBand::rehearse(void) 
{
    int remainingEnergy = get_energy()-(0.5*0.2)*get_energy();
    set_energy(remainingEnergy);
}