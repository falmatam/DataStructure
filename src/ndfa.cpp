#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "ics46goody.hpp"
#include "array_queue.hpp"
#include "array_priority_queue.hpp"
#include "array_set.hpp"
#include "array_map.hpp"


typedef ics::ArraySet<std::string>                     States;
typedef ics::ArrayQueue<std::string>                   InputsQueue;
typedef ics::ArrayMap<std::string,States>              InputStatesMap;

typedef ics::ArrayMap<std::string,InputStatesMap>       NDFA;
typedef ics::pair<std::string,InputStatesMap>           NDFAEntry;

bool gt_NDFAEntry (const NDFAEntry& a, const NDFAEntry& b)
{return a.first<b.first;}

typedef ics::ArrayPriorityQueue<NDFAEntry,gt_NDFAEntry> NDFAPQ;

typedef ics::pair<std::string,States>                   Transitions;
typedef ics::ArrayQueue<Transitions>                    TransitionsQueue;


//Read an open file describing the non-deterministic finite automaton (each
//  line starts with a state name followed by pairs of transitions from that
//  state: (input followed by a new state, all separated by semicolons), and
//  return a Map whose keys are states and whose associated values are another
//  Map with each input in that state (keys) and the resulting set of states it
//  can lead to.
const NDFA read_ndfa(std::ifstream &file) {

	NDFA ndfa;
	std::string line;

	while (getline(file,line))
	{
		std::vector<std::string> words = ics::split(line,";");
		std::string state = words[0];
		InputStatesMap inputSM;
		for (int i = 1; i < inputSM.size(); i += 2) {
		  std::string input     = words[i];
		  std::string new_state = words[i+1];
		  inputSM[input].insert(new_state);
		}
		ndfa[state] = inputSM;
	}

	file.close();

	return ndfa;
}


//Print a label and all the entries in the finite automaton Map, in
//  alphabetical order of the states: each line has a state, the text
//  "transitions:" and the Map of its transitions.
void print_ndfa(const NDFA& ndfa) {
	std::cout << "Non_Deterministic Finite Automaton Description" << std::endl;
	for(auto i: ndfa)
	{
		std::cout << "   ";
		std::cout << i.first << " transitions: " << i.second << std::endl;
	}
	std::cout << std::endl;
}


//Return a queue of the calculated transition pairs, based on the non-deterministic
//  finite automaton, initial state, and queue of inputs; each pair in the returned
//  queue is of the form: input, set of new states.
//The first pair contains "" as the input and the initial state.
//If any input i is illegal (does not lead to any state in the non-deterministic finite
//  automaton), ignore it.
TransitionsQueue process(const NDFA& ndfa, std::string state, const InputsQueue& inputs) {

	TransitionsQueue answer;
	States initialState;

	initialState.insert(state);
	answer.enqueue(Transitions("", (initialState)));

	for (auto x : inputs)
	{
		if((ndfa[state].has_key(x)))
		{
			answer.enqueue(Transitions(x, ndfa[state][x]));
			//std::cout << "answer after enqueue: " << answer << std::endl;
		}
		else
		{
			break;
		}
	}
	return answer;
}


//Print a TransitionsQueue (the result of calling process) in a nice form.
//Print the Start state on the first line; then print each input and the
//  resulting new states indented on subsequent lines; on the last line, print
//  the Stop state.
void interpret(TransitionsQueue& tq) {  //or TransitionsQueue or TransitionsQueue&&

	std::string lastState;
	std::cout << "Start state = " << tq.peek().second << std::endl;

	for (auto i : tq)
	{
		if (i.first != "")
		{
			std::cout << " Input = " << i.first << "; new state = " << i.second << std::endl;
			lastState = to_string(i.second);
		}
	}
	std::cout << "Stop state = " << lastState << std::endl;
}



//Prompt the user for a file, create a finite automaton Map, and print it.
//Prompt the user for a file containing any number of simulation descriptions
//  for the finite automaton to process, one description per line; each
//  description contains a start state followed by its inputs, all separated by
//  semicolons.
//Repeatedly read a description, print that description, put each input in a
//  Queue, process the Queue and print the results in a nice form.
int main() {
 try {

		std::ifstream text_file;
		ics::safe_open(text_file,"Enter the name of a file with a Non-Deterministic Finite Automaton","ndfaendin01.txt");
		NDFA f = read_ndfa(text_file);
		print_ndfa(f);

		std::ifstream text_file_inputs;
		ics::safe_open(text_file_inputs,"\nEnter the name of a file with the start-states and input","ndfainputendin01.txt");

		std::string line;

		while (getline(text_file_inputs,line)) {
		  std::cout << "\nStarting new simulation with description: " << line << std::endl;
		  std::vector<std::string> state_inputs = ics::split(line,";");
		  InputsQueue inputs;
		  for (int i = 1; i < state_inputs.size(); i++)
			  inputs.enqueue(state_inputs[i]);
		  TransitionsQueue t = process(f,state_inputs[0],inputs);
		  interpret(t);
		}
 } catch (ics::IcsError& e) {
   std::cout << e.what() << std::endl;
 }

 return 0;
}
