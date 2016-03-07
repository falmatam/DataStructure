#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>                    //Biggest int: std::numeric_limits<int>::max()
#include "ics46goody.hpp"
#include "array_queue.hpp"
#include "array_priority_queue.hpp"
#include "array_set.hpp"
#include "array_map.hpp"


typedef ics::ArrayQueue<std::string>              CandidateQueue;
typedef ics::ArraySet<std::string>                CandidateSet;
typedef ics::ArrayMap<std::string,int>            CandidateTally;

typedef ics::ArrayMap<std::string,CandidateQueue> Preferences;
typedef ics::pair<std::string,CandidateQueue>     PreferencesEntry;
typedef ics::ArrayPriorityQueue<PreferencesEntry> PreferencesEntryPQ; //Must supply gt at construction

typedef ics::pair<std::string,int>                TallyEntry;
typedef ics::ArrayPriorityQueue<TallyEntry>       TallyEntryPQ;




//Read an open file stating voter preferences (each line is (a) a voter
//  followed by (b) all the candidates the voter would vote for, in
//  preference order (from most to least preferred candidate, separated
//  by semicolons), and return a Map of preferences: a Map whose keys are
//  voter names and whose values are a queue of candidate preferences.
Preferences read_voter_preferences(std::ifstream &file) {

	  Preferences answer;
	  std::string line;

	  while (getline(file,line)) {

	    std::vector<std::string> words = ics::split(line,";");
	    std::string voter = words[0];

	    CandidateQueue queue;

	    for (int i = 1; i < words.size(); ++i)
	    {
	      queue.enqueue(words[i]);
	    }

	    answer.put(voter,queue);
	  }

	  file.close();

	  return answer;
}


//Print a label and all the entries in the preferences Map, in alphabetical
//  order according to the voter.
//Use a "->" to separate the voter name from the Queue of candidates.
void print_voter_preferences(const Preferences& preferences) {

	std::cout << "\n" << "Voter Preferences" << std::endl;

	//Implement sort function to PQ elements accessing first items of each
	PreferencesEntryPQ sortedCollection(preferences, [](const PreferencesEntry& x,
			const PreferencesEntry& y){return x.first < y.first;});

	for (PreferencesEntry& keyVal : sortedCollection) {
		std::cout << "   ";
					//voter name           //queue of candidates
		std::cout << keyVal.first << " -> " << keyVal.second << std::endl;

	}
}


//Print the message followed by all the entries in the CandidateTally, in
//  the order specified by has_higher_priority: i is printed before j, if
//  has_higher_priority(i,j) returns true: sometimes alphabetically by candidate,
//  other times by decreasing votes for the candidate.
//Use a "->" to separate the candidat name from the number of votes they
//  received.
void print_tally(std::string message, const CandidateTally& tally, bool (*has_higher_priority)(const TallyEntry& i,const TallyEntry& j)) {

	std::cout << "\n" << message << std::endl;

	//Implement prioritizer function taking tally and priority as two args
	TallyEntryPQ prioritizedCollection(tally, has_higher_priority);

	for(auto& t : prioritizedCollection)
	{
		//std::cout << "t: " << t << std::endl;
		std::cout << "   ";
			     //candidate name	 //number of votes
		std::cout << t.first << " -> " << t.second << std::endl;

	}

}


//Return the CandidateTally: a Map of candidates (as keys) and the number of
//  votes they received, based on the unchanging Preferences (read from the
//  file) and the candidates who are currently still in the election (which changes).
//Every possible candidate should appear as a key in the resulting tally.
//Each voter should tally one vote: for their highest-ranked candidate who is
//  still in the the election.
CandidateTally evaluate_ballot(const Preferences& preferences, const CandidateSet& candidates) {

	CandidateTally answer;
	int setVal = 0;

	for (auto& i : candidates)
		answer[i] = setVal;

	for (auto& i : preferences)
	{
		//std::cout << "i.second: " << i.second << std::endl;
		for (auto& j : i.second)
		{
			if (candidates.contains(j))
			{
				answer[j] += setVal+1;
				break;
			}
		}
	}

	  return answer;
}


//Return the Set of candidates who are still in the election, based on the
//  tally of votes: compute the minimum number of votes and return a Set of
//  all candidates receiving more than that minimum; if all candidates
//  receive the same number of votes (that would be the minimum), the empty
//  Set is returned.
CandidateSet remaining_candidates(const CandidateTally& tally) {

	CandidateSet answer;
	int minimum = std::numeric_limits<int>::max(); //really useful --

	for(auto i : tally)
	{
		if(i.second < minimum)
			minimum = i.second;
	}
	for(auto i : tally)
	{
		if (i.second > minimum)
			answer.insert(i.first);
	}

	return answer;



}


//Prompt the user for a file, create a voter preference Map, and print it.
//Determine the Set of all the candidates in the election, from this Map.
//Repeatedly evaluate the ballot based on the candidates (still) in the
//  election, printing the vote count (tally) two ways: with the candidates
//  (a) shown alphabetically increasing and (b) shown with the vote count
//  decreasing (candidates with equal vote counts are shown alphabetically
//  increasing); from this tally, compute which candidates remain in the
//  election: all candidates receiving more than the minimum number of votes;
//  continue this process until there are less than 2 candidates.
//Print the final result: there may 1 candidate left (the winner) or 0 left
//   (no winner).
int main() {
 try {
	    std::ifstream text_file;
	    ics::safe_open(text_file,"Enter voter preference file name","votepref1.txt");
	    Preferences p = read_voter_preferences(text_file);
	    print_voter_preferences(p);

	    CandidateSet candidates;
	    for (auto& voter : p)
	      for (auto& i : voter.second)
	        candidates.insert(i);

	    unsigned int counter = 1;
	    while(true)
	    {
	    	CandidateTally tally = evaluate_ballot(p,candidates);
	    	std::stringstream sstream;
	    	sstream << "\nVote count on ballot #" << counter << " with candidates in alphabetical order: still in election = " << candidates;
	    	print_tally(sstream.str(),tally,[](const TallyEntry& i,const TallyEntry& j){return (i.first == j.first ? i.second < j.second : i.first < j.first);});
	    	sstream.str("");
	    	sstream << "\nVote count on ballot #" << counter << " with candidates in numerical order: still in election = " << candidates;
	    	print_tally(sstream.str(),tally,[](const TallyEntry& i,const TallyEntry& j){return (i.second == j.second ? i.first < j.first : i.second > j.second);});
	    	candidates = remaining_candidates(tally);
	    	counter += 1;

	    	if(candidates.size() == 1)
	    	{
	    		for(auto winner : candidates)
			  {
				  std::cout << std::endl << "Winner is " << winner << std::endl;
			  }
			  break;
	    	}

	    	else if(candidates.empty())
	    	{
			  std::cout << std::endl << "No winner: election is a tie among candidate remaining on the last ballot." << std::endl;
			  break;
	    	}

		  counter++;
	    }

 } catch (ics::IcsError& e) {
   std::cout << e.what() << std::endl;
 }
 return 0;
}
