/*
MIT License

Copyright (c) [2019] [Joshua Blickensd�rfer]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Header/Sample_Tracer_LTL.h"



Sample_Tracer_LTL::Sample_Tracer_LTL(z3::context & context, Dag & dag, std::vector<std::string>& input_Sample_LTL, std::string sample_Name):
	Sample_Tracer(context, dag, sample_Name)
{

	create_Sample(input_Sample_LTL);

	number_Of_Variables = sample[0].first[0].size();

	for (std::pair<std::vector<std::vector<bool>>, int> word : sample) {
		sample_Sizes.push_back(std::make_pair(word.first.size(), word.second));
	}
}


Sample_Tracer_LTL::~Sample_Tracer_LTL()
{
}

void Sample_Tracer_LTL::add_Formulas_Atomic(int iteration) {

	int word_Index = 0;
	for (std::pair<std::vector<std::vector<bool>>, int> word : sample) {
		for (int p = 0; p < number_Of_Variables; p++) {
			z3::expr_vector atomic_Vector(context);
			int t = 0;
			for (std::vector<bool> letter : word.first) {
				atomic_Vector.push_back(variables_Y_Word_i_t_any[word_Index][iteration][t] == context.bool_val(letter[p]));
				atomic_Vector.push_back(variables_Y_Word_i_t_all[word_Index][iteration][t] == context.bool_val(letter[p]));
				t++;
			}
			z3::expr atomic = z3::implies(dag.variables_x_lambda_i[p][iteration], z3::mk_and(atomic_Vector));
			if (using_Incremental) {
				solver->add(atomic);
			}
			else {
				all_Formulas.push_back(atomic);
			}
		}
		word_Index++;
	}

	/*
	int word_Index = 0;
	for (std::pair<std::vector<std::vector<bool>>, int> word : sample) {
	for (int p = 0; p < number_Of_Variables; p++) {
	z3::expr_vector conjunction(context);
	int t = 0;
	for (std::vector<bool> letter : word.first) {

	if (letter[p]) {
	conjunction.push_back(variables_Y_Word_i_t[word_Index][iteration][t]);
	}
	else {
	conjunction.push_back(!variables_Y_Word_i_t[word_Index][iteration][t]);
	}
	t++;
	}
	z3::expr atomic = z3::implies(variables_x_lambda_i[p][iteration], z3::atleast(conjunction, conjunction.size()));
	if (using_Incremental) {
	solver->add(atomic);
	}
	else {
	formulas_Atomic[word_Index].push_back(atomic);
	}
	}
	word_Index++;
	}
	*/

}

void Sample_Tracer_LTL::initialize()
{
	for (unsigned int i = 0; i < sample.size(); i++) {
		variables_Y_Word_i_t_any.push_back(std::vector<z3::expr_vector>());
		variables_Y_Word_i_t_all.push_back(std::vector<z3::expr_vector>());
	}

	add_Variables(0);
	add_Formulas_Atomic(0);
}

void Sample_Tracer_LTL::add_Formulas(int iteration)
{
	add_Formulas_Atomic(iteration);

	add_Formulas_Not(iteration);

	add_Formulas_Or(iteration);

	add_Formulas_And(iteration);

	add_Formulas_Implies(iteration);

	add_Formulas_Next(iteration);

	add_Formulas_Finally(iteration);

	add_Formulas_Globally(iteration);

	add_Formulas_Until(iteration);
}

void Sample_Tracer_LTL::create_Sample(std::vector<std::string> input_Sample_LTL)
{
	std::stringstream string_stream;
	std::string line;

	for (std::string word : input_Sample_LTL) {

		string_stream = std::stringstream(word);

		// get the beginning of the repeting part

		std::getline(string_stream, line, ':');
		std::string remaining_word = line;

		std::getline(string_stream, line, ':');
		std::getline(string_stream, line, ':');

		int second_Component = std::stoi(line);

		// make the string to a vector of vector of bools

		string_stream = std::stringstream(remaining_word);

		// seperate all letters

		std::vector<std::string> initial_seperation;
		while (std::getline(string_stream, line, ';')) {
			initial_seperation.push_back(line);
		}

		// make string to bools

		std::vector<std::vector<bool>> first_Component;
		for (std::string s : initial_seperation) {
			string_stream = std::stringstream(s);
			std::vector<bool> letter;
			while (std::getline(string_stream, line, ',')) {
				bool b;
				std::istringstream(line) >> b;
				letter.push_back(b);
			}
			first_Component.push_back(letter);
		}

		sample.push_back(std::make_pair(first_Component, second_Component));
	}
}
