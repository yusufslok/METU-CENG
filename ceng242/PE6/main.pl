:- module(main, [is_vote_wasted/2, is_candidate_elected/2, candidate_count_from_city/3, all_parties/1, all_candidates_from_party/2, all_elected_from_party/2, election_rate/2, council_percentage/2, alternative_debate_setups/2]).
:- [kb].

% DO NOT CHANGE THE UPPER CONTENT, WRITE YOUR CODE AFTER THIS LINE

is_vote_wasted(City,PolParty) :-
    party(PolParty,_), \+ (candidate(_,PolParty,City,Row),elected(City,PolParty,ElectedRepresentativeCount),Row =< ElectedRepresentativeCount).
    
is_candidate_elected(Name, PolParty) :- (candidate(Name, PolParty, City, Row),elected(City, PolParty, Count),Row =< Count).   

candidate_count_from_city(List, C, Count) :-
    count(List, C, 0, Count).

count([], _, Count, Count).
count([Candidate|Kalan], C, Val, Count) :-
    candidate(Candidate, _, City, _),
    (City \= C -> Val2 = Val ; Val2 is Val + 1),
    count(Kalan, C, Val2, Count).
    
all_parties(List) :-
    findall(List, party(List, _), List).

all_candidates_from_party(Party, List) :-
    findall(Candidate, candidate(Candidate, Party, _, _), List).

all_elected_from_party(Party, List) :-
    findall(Name,is_candidate_elected(Name,Party), List).

election_rate(Party, Percent) :-
    all_candidates_from_party(Party, Candidates),
    all_elected_from_party(Party, ElectedCands),
    length(Candidates, Total),
    length(ElectedCands, Elected),
    Percent is Elected / Total.

council_percentage(Party, Percent) :-
    all_elected_from_party(Party, ElectedCands),
    to_elect(Total),
    length(ElectedCands, Elected),
    Percent is Elected / Total.
    

alternative_debate_setups(DescriptionString, OrderedListOfCandidates) :-
    findall([Candidate1,Candidate2,Candidate3],
        a(candidate(Candidate1, Party1),
        sub_string(Party1 , N1,L1,M1, DescriptionString),
        N1 = 0,
        candidate(Candidate2, Party2),
        sub_string(Party2 , N2,L2,M2, DescriptionString),
        N2 = 1,
        candidate(Candidate3, Party3),
        sub_string(Party3 , N3,L3,M3, DescriptionString),
        N3 = 2),OrderedListOfCandidates).

    
    
    
    
    
    
    
    
    
    
    
    