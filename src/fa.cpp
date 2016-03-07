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


typedef ics::ArrayQueue<std::string>                InputsQueue;
typedef ics::ArrayMap<std::string,std::string>      InputStateMap;

typedef ics::ArrayMap<std::string,InputStateMap>    FA;
typedef ics::pair<std::string,InputStateMap>        FAEntry;

bool gt_FAEntry (const FAEntry& a, const FAEntry& b)
{return a.first<b.first;}

typedef ics::ArrayPriorityQueue<FAEntry,gt_FAEntry> FAPQ;

typedef ics::pair<std::string,std::string>          Transition;
typedef ics::ArrayQueue<Transition>                 TransitionQueue;


//Read an open file describing the finite automaton (each line starts with
//  a state name followed by pairs of transitions from that state: (input
//  followed by new state, all separated by semicolons), and return a Map
//  whose keys are states and whose associated values are another Map with
//  each input in that state (keys) and the resulting state it leads to.
const FA read_fa(std::ifstream &file) {
	FA answer;
	std::string line;

	while(getline(file, line)){
		std::vector<std::string> words = ics::split(line, ";"); //words on each line after split of ';'
		InputStateMap input;
		for(int i = 1; i < words.size(); i += 2)
		{
			//std::cout << "Words: " << words.front() << std::endl;
			input[words[i]] = words[i+1];
		}
		answer[words[0]] = input;
	}

	file.close();

	return answer;
}


//Print a label and all the entries in the finite automaton Map, in
//  alphabetical order of the states: each line has a state, the text
//  "transitions:" and the Map of its transitions.
void print_fa(const FA& fa) {
	std::cout << std::endl << "Finite Automaton Description" << std::endl;

	for(auto i : fa)
	{
		std::cout << "   ";
		std::cout << i.first << " transitions: " << i.second << std::endl;
	}
	std::cout << std::endl;
}


//Return a queue of the calculated transition pairs, based on the finite
//  automaton, initial state, and queue of inputs; each pair in the returned
//  queue is of the form: input, new state.
//The first pair contains "" as the input and the initial state.
//If any input i is illegal (does not lead to a state in the finite
//  automaton), then the last pair in the returned queue is i,"None".
TransitionQueue process(const FA& fa, std::string state, const InputsQueue& inputs) {

	TransitionQueue answer;

	answer.enqueue(Transition("", state));

	for (auto i : inputs)
	{
		if((fa[state].has_key(i)))
		{
			answer.enqueue(Transition(i, fa[state][i]));
		}
		else
		{
			answer.enqueue(Transition(i, "None"));
			break;
		}
		state = fa[state][i];
	}
	return answer;
}


//Print a TransitionQueue (the result of calling the process function above)
// in a nice form.
//Print the Start state on the first line; then print each input and the
//  resulting new state (or "illegal input: terminated", if the state is
//  "None") indented on subsequent lines; on the last line, print the Stop
//  state (which may be "None").
void interpret(TransitionQueue& tq) {  //or TransitionQueue or TransitionQueue&&

	std::string lastState;
	std::cout << "Start state = " << tq.peek().second << std::endl;

	for (auto i : tq)
	{
		if (i.first != "")
		{
			if (i.second == "None") //t.second checks state
			{
				std::cout << " Input = " << i.first << "; illegal input: terminated" << std::endl;
				std::cout << "Stop state = None" << std::endl;
				return;
			}

			else
			{
				std::cout << " Input = " << i.first << "; new state = " << i.second << std::endl;
				lastState = i.second;
			}
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
	    ics::safe_open(text_file,"Enter the file name with a Finite Automaton","faparity.txt");
	    FA f = read_fa(text_file);
	    print_fa(f);

	    std::ifstream text_file_inputs;
	    ics::safe_open(text_file_inputs,"\nEnter the name of the file with the start-state and input","fainputparity.txt");

	    std::string line;

	    while (getline(text_file_inputs,line)) {
	      std::cout << "\nStarting new simulation with description: " << line << std::endl;
	      std::vector<std::string> state_inputs = ics::split(line,";");
	      InputsQueue inputs;
	      for (int i = 1; i < state_inputs.size(); i++)
	    	  inputs.enqueue(state_inputs[i]);
	      TransitionQueue t = process(f,state_inputs[0],inputs);
	      interpret(t);
	    }

 } catch (ics::IcsError& e) {
   std::cout << e.what() << std::endl;
 }

 return 0;
}
