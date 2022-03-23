#include <iostream>
#include <map>
#include <functional>
#include <chrono>
#include "Name.hpp"
#include "SystemName.hpp"
#include "Buffer.hpp"
#include "LUT_IDs.h"

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// =  Testing that all characters have an even chance to be drawn when generating an ID    =
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

std::function<bool()> test_uniform_ids = [](){
	ID id;
	std::map<const char, size_t> counter;
	const size_t nb_iters = 2000000;

	for (size_t i = 0 ; i < SIZE_IDS_ENTRY ; i++){
		counter[LUT_IDs[i]] = 0;
	}

	for (size_t i = 0 ; i < nb_iters ; i++){
		id.randomize();

		for (size_t j = 0 ; j < SIZE_OF_ID ; j++){
			counter[id[j]]++;
		}
	}

	size_t diff = 0;
	
	double sz_entries = (double)SIZE_IDS_ENTRY;
	double sz_ids = (double)SIZE_OF_ID;
	double nb_iterations = (double)nb_iters;

	const size_t expected = (1.0 / sz_entries) * sz_ids * nb_iterations;
	const size_t tolerance = 0.005 * expected;


	for (size_t i = 0 ; i < SIZE_IDS_ENTRY ; i++){

		if (counter[LUT_IDs[i]] < expected){
			diff = expected - counter[LUT_IDs[i]];
		}
		else{
			diff = counter[LUT_IDs[i]] - expected;
		}

		if (diff > tolerance){
			return false;
		}
	}

	return true;
};


std::function<bool()> test_name = [](){

	// - - - - - - - - - - - - - - - - - - - - - - - -
	char valid_full[SIZE_OF_NAME+10] = {};
	for (size_t i = 0 ; i < SIZE_OF_NAME+10 ; i++){ // Argument with more chars than can stock a Name
		valid_full[i] = i % 10 + 48;
	}
	char expected_full[SIZE_OF_NAME] = {};
	for (size_t i = 0 ; i < SIZE_OF_NAME ; i++){
		expected_full[i] = i % 10 + 48;
	}
	expected_full[SIZE_OF_NAME - 1] = 0;

	// - - - - - - - - - - - - - - - - - - - - - - - -

	char valid_interupt[SIZE_OF_NAME] = {'a', 'b', 'c', 0, 48, 49, 49}; // Name with a null character in the middle
	char expected_interupt[SIZE_OF_NAME] = {'a', 'b', 'c', 0};
	
	// - - - - - - - - - - - - - - - - - - - - - - - -

	char invalid[SIZE_OF_NAME] = {1, 2, 3, 4, 5, 6, 7, 8, 9}; // Name with invalid characters
	char expected_invalid[SIZE_OF_NAME] = {VALID_CHAR, VALID_CHAR, VALID_CHAR, VALID_CHAR, VALID_CHAR, VALID_CHAR, VALID_CHAR, VALID_CHAR, VALID_CHAR};

	// - - - - - - - - - - - - - - - - - - - - - - - -

	const char* valid_reg = "Un Nom";
	char expected_reg[SIZE_OF_NAME] = {'U', 'n', ' ', 'N', 'o', 'm'}; // Regular const char

	// = = = = = = = = = = = = = = = = = = = = = = = =

	Name n1 = valid_full;
	Name n2 = valid_interupt;
	Name n3 = "Un Nom";
	Name n4 = invalid;


	for (size_t i = 0 ; i < SIZE_OF_NAME ; i++){
		if (n1[i] != expected_full[i] || !n1.is_valid()){
			return false;
		}
	}

	for (size_t i = 0 ; i < SIZE_OF_NAME ; i++){
		if (n2[i] != expected_interupt[i] || !n2.is_valid()){
			return false;
		}
	}


	for (size_t i = 0 ; i < SIZE_OF_NAME ; i++){
		if (n3[i] != expected_reg[i] || !n3.is_valid()){
			return false;
		}
	}

	for (size_t i = 0 ; i < SIZE_OF_NAME ; i++){
		if (n4[i] != expected_invalid[i] || !n4.is_valid()){
			return false;
		}
	}

	n1.override(invalid, -1);

	for (size_t i = 0 ; i < SIZE_OF_NAME ; i++){
		if (n1[i] != expected_invalid[i] || !n1.is_valid()){
			return false;
		}
	}

	return true;
};


std::function<bool()> test_id = [](){
	
	return true;
};


std::function<bool()> test_stack_segment = [](){
	constexpr size_t sz = 16;
	constexpr size_t sz_data = 10;

	StackSegment<sz> s1;

	StackSegment<sz> s2(0);
	uint8_t e2[sz] = {};

	StackSegment<sz> s3("azertyuiop", sz_data);
	uint8_t e3[sz] = {'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'};

	StackSegment<sz> s4(s3);
	uint8_t e4[sz] = {'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'};

	StackSegment<sz> s5 = s4;
	uint8_t e5[sz] = {'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'};

	StackSegment<sz> s6;
	s6 = "azertyuiop";
	uint8_t e6[sz] = {'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'};

	for (size_t i = 0 ; i < sz ; i++){
		if (s2[i] != e2[i]){
			return false;
		}
	}

	for (size_t i = 0 ; i < sz_data ; i++){
		if (s3[i] != e3[i]){
			return false;
		}
	}

	for (size_t i = 0 ; i < sz_data ; i++){
		if (s4[i] != e4[i]){
			return false;
		}
	}

	for (size_t i = 0 ; i < sz_data ; i++){
		if (s5[i] != e5[i]){
			return false;
		}
	}

	for (size_t i = 0 ; i < sz_data ; i++){
		if (s6[i] != e6[i]){
			return false;
		}
	}

	if (s4 != s5){
		return false;
	}


	uint8_t e7[sz];
	s4.copy(e7);

	for (size_t i = 0 ; i < sz_data ; i++){
		if (s4[i] != e7[i]){
			return false;
		}
	}

	return true;
};


std::function<bool()> test_basic_buffer = [](){
	constexpr size_t sz = sizeof(uint16_t);

	BasicBuffer<sz> b1;

	BasicBuffer<sz> b2(0);

	BasicBuffer<3*sizeof(int)> b3;
	b3.add<int>(3).add<int>(4).add<int>(5);

	if (!b3.is_full()){
		return false;
	}

	BasicBuffer<3*sizeof(int)> b4 = b3;

	BasicBuffer<3*sizeof(int)> b5;
	b5.copy_from(b4.c_str(), 3*sizeof(int));

	// ====================================

	BufferReader<sz> r1(b1);
	BufferReader<sz> r2(b2);
	BufferReader<3*sizeof(int)> r3(b3);
	BufferReader<3*sizeof(int)> r4(b4);
	BufferReader<3*sizeof(int)> r5(b5);

	int one, two, three;
	uint16_t ui;

	r1.get<uint16_t>(ui);

	r2.get<uint16_t>(ui);
	if (ui != 0){
		return false;
	}

	r3.get<int>(one).get<int>(two).get<int>(three);
	if ((one != 3) || (two != 4) || (three != 5)){
		return false;
	}

	r4.get<int>(one).get<int>(two).get<int>(three);
	if ((one != 3) || (two != 4) || (three != 5)){
		return false;
	}

	r5.get<int>(one).get<int>(two).get<int>(three);
	if ((one != 3) || (two != 4) || (three != 5)){
		return false;
	}

	b5.clear();
	BufferReader<3*sizeof(int)> r6(b5);

	r6.get<int>(one).get<int>(two).get<int>(three);
	if ((one != 0) || (two != 0) || (three != 0)){
		return false;
	}

	return true;
};


bool test(const char* msg, const std::function<bool()>& f){

	auto start = std::chrono::high_resolution_clock::now();

	bool res = f();

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

	std::cout << " => " << msg << ": " << duration.count() << "ms" << std::endl;

	return res;
}


int main(int argc, char* argv[], char* env[]){

	srand(time(NULL));

	std::cout << "======= TESTING BUFFERS =======" << std::endl;

	if (test("Test IDs", test_id)){
		std::cout << "DONE - Test on IDs behavior." << std::endl;
	}
	else{
		std::cout << "FAIL - Test on IDs behavior." << std::endl;
	}

	std::cout << "===============================" << std::endl;


	if (test("Test Names", test_name)){
		std::cout << "DONE - Test on Names behavior." << std::endl;
	}
	else{
		std::cout << "FAIL - Test on Names behavior." << std::endl;
	}


	std::cout << "===============================" << std::endl;


	if (test("Test Stack Segments", test_stack_segment)){
		std::cout << "DONE - Test on Stack Segments behavior." << std::endl;
	}
	else{
		std::cout << "FAIL - Test on Stack Segments behavior." << std::endl;
	}


	std::cout << "===============================" << std::endl;


	if (test("Test Basic Buffers", test_basic_buffer)){
		std::cout << "DONE - Test on Basic Buffers." << std::endl;
	}
	else{
		std::cout << "FAIL - Test on Basic Buffers." << std::endl;
	}


	std::cout << "===============================" << std::endl;


	if (test("Test Uniform Generation", test_uniform_ids)){
		std::cout << "DONE - Uniform distribution of IDs." << std::endl;
	}
	else{
		std::cout << "FAIL - Uniform distribution of IDs." << std::endl;
	}

	std::cout << "===============================" << std::endl;

	return 0;
}