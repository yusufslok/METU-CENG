#include "tournament_round.h"

// TournamentRound functions goes here

TournamentRound::TournamentRound() { }
TournamentRound::TournamentRound(std::list<MusicBand*> _bands) {
    this->bands = _bands;
}

TournamentRound::TournamentRound(std::vector<MusicBand*> _bands) {
    this->bands.assign(_bands.begin(), _bands.end());
}

std::size_t TournamentRound::size() {
    return bands.size();
}

    
//TournamentRound::TournamentRound(TournamentRound& other) {bands = other.bands}
//TournamentRound::TournamentRound(TournamentRound&& other) {bands = std::move(other.bands) }
TournamentRound& TournamentRound::operator=(TournamentRound&& other) { 
    if (this != &other) {
            bands = std::move(other.bands);
        }
        return *this;
    }
TournamentRound& TournamentRound::get_next_round() {
    std::list<MusicBand*> winners;
    if(bands.size() <= 1){
        auto it = bands.begin();
        winners.push_back((*it));
        return *(new TournamentRound(winners));
    }
    if (bands.size() % 2 == 0) {
        int i=0;
        for (auto it = bands.begin();i<bands.size()/2 ; it++,i++) {
            auto it_opposite = std::prev(bands.end(), std::distance(bands.begin(), it) + 1);
            int score1 = (*it)->play(*it_opposite);
            int score2 = (*it_opposite)->play(*it);
            if (score1 >= score2) {
                if ((*it_opposite)->get_fan_count() >= (score1 - score2) ) {
                    int change = score1 - score2;
                    int fan1 = (*it)->get_fan_count() + change;
                    int fan2 = (*it_opposite)->get_fan_count() - change;
                    (*it)->set_fan_count(fan1);
                    (*it_opposite)->set_fan_count(fan2);
                }
                else {
                    int change = (*it_opposite)->get_fan_count();
                    int fan1 = (*it)->get_fan_count() + change;
                    int fan2 = 0;
                    (*it)->set_fan_count(fan1);
                    (*it_opposite)->set_fan_count(fan2);
                }
                winners.push_back(*it);
            }
            else {
                if ((*it)->get_fan_count() >= (score2 - score1)) {
                    int change = score2 - score1;
                    int fan1 = (*it)->get_fan_count() - change;
                    int fan2 = (*it_opposite)->get_fan_count() + change;
                    (*it)->set_fan_count(fan1);
                    (*it_opposite)->set_fan_count(fan2);
                }
                else {
                    int change = (*it)->get_fan_count();
                    int fan1 = 0;
                    int fan2 = (*it_opposite)->get_fan_count() + change;
                    (*it)->set_fan_count(fan1);
                    (*it_opposite)->set_fan_count(fan2);
                }
                winners.push_back(*it_opposite);
            }
        }
         return *(new TournamentRound(winners));
    }
    else {
        int i=0;
        for (auto it = bands.begin();i<bands.size()/2; it++,i++) {
            auto it_opposite = std::prev(bands.end(), std::distance(bands.begin(), it) + 1);
            int score1 = (*it)->play(*it_opposite);
            int score2 = (*it_opposite)->play(*it);
            if (score1 >= score2) {
                if ((*it_opposite)->get_fan_count() >= (score1 - score2)) {
                    int change = score1 - score2;
                    int fan1 = (*it)->get_fan_count() + change;
                    int fan2 = (*it_opposite)->get_fan_count() - change;
                    (*it)->set_fan_count(fan1);
                    (*it_opposite)->set_fan_count(fan2);
                }
                else {
                    int change = (*it_opposite)->get_fan_count();
                    int fan1 = (*it)->get_fan_count() + change;
                    int fan2 = 0;
                    (*it)->set_fan_count(fan1);
                    (*it_opposite)->set_fan_count(fan2);
                }
                winners.push_back(*it);
            }
            else {
                if ((*it)->get_fan_count() >= (score2 - score1)) {
                    int change = score2 - score1;
                    int fan1 = (*it)->get_fan_count() - change;
                    int fan2 = (*it_opposite)->get_fan_count() + change;
                    (*it)->set_fan_count(fan1);
                    (*it_opposite)->set_fan_count(fan2);
                }
                else {
                    int change = (*it)->get_fan_count();
                    int fan1 = 0;
                    int fan2 = (*it_opposite)->get_fan_count() + change;
                    (*it)->set_fan_count(fan1);
                    (*it_opposite)->set_fan_count(fan2);
                }
                winners.push_back(*it_opposite);
            }
        }
        winners.push_back(*std::prev(bands.end(),bands.size()/2+1));
         return *(new TournamentRound(winners));
    }
}
std::ostream& operator<< (std::ostream &os, TournamentRound &other) {
    auto it = other.bands.begin();
    auto last = std::prev(other.bands.end());
    for (; it != last; it++){
        os << (*it)->get_name() << "\t";
    }
    os << (*last)->get_name();
    return os;
}