/*jshint esversion: 6 */

// Quality tester: https://jshint.com/

function concatenation(){
	let j = "First part";
	let k = 123;
	let l = "closing";
	console.log(`${j} ${k} ${l}`);
}

concatenation();

// => DECLARE A VARIABLE:
var v1   = 5; // Is available throughout the whole programm (extern)
let v2   = 6; // Only within scope
const v3 = 7; // Constant value (scoped ?)
v4 = 8; // Global

function iterating(){
	// Strings are immutable
	let str = "0123456789";

	for (c in str){
		console.log(c);
	}

	for (let i = 0 ; i < str.length ; i++){
		console.log(str[i]);
	}

	let arr = ['a', 'b', 'c', 'd', 'e', 'f', 'g'];
	arr.push('h'); // unshift(v) adds at the begining of the array. Low opti
	let removed = arr.pop(); // .shift() removes the first element. Low opti
	console.log(removed);
	console.log(arr);

	let l1 = [];
	let l2 = Array(); // Array constructor
	let l3 = Array(10); // We can reserve a size
}

iterating();

function math(){
	return Math.round(3.141592);
}

// Dictionaries
let d1 = {};
let d2 = {
	'field1': "a string",
	'field2': 3.141592,
	'field3': 4,
	'field5': false
};

// Both ways work to access a field, whether it exists or not.
d1.some_field = "something";
d2.field1 = "another string";
d2['field2'] *= 2.0; // The dot notation is prefered

console.log(math());
console.log(d1);
console.log(d2);

const f = function(a){
	return 2.0 * a;
};

const g = (a) => {
	return 2.0 * a;
};

console.log(g(4));

function filter_map_type(){
	let my_list = [1, -2, 3, 3.141592, -4, 5, -6, 7, -8, 9];

	const itg = (n) => {
		return Number.isInteger(n);
	};

	const pos = (n) => {
		return n >= 0;
	};

	const db = (n) => {
		return n *= 2.0;
	};

	const sum = (previous, current) => {
		return previous + current;
	};

	// /!\
	const res = my_list.filter(itg).filter(pos).map(db).reduce(sum);
	// res is equal to the sum of the double of positive integers from 'my_list'

	console.log(res);

}

filter_map_type();

class Name{
	constructor(arg){
		this.a = "aaaa";
		this.b = 42;
		this.c = [0, 0, 0];
		this.d = arg;
	}

	get a_getter(){
		let k = this.b / 2.0;
		return k;
	}

	set a_setter(arg){
		// best place to test for types, values, ...
		if (typeof(arg) === 'string'){
			this.a = arg;
		}
		else{
			throw "This argument should be a string";
		}
	}
}

let n = new Name("un string");
console.log(n.a_getter); // No parenthesis for getters, as if it was an attribute
n.a_setter = "3"; // No parenthesis for setters, as if it was an attribute


/*

	Native data-types:

undefined: variable that hasn't ever been affected
null, 
Boolean, 
String, 
Number, 
Symbol,
Float32Array exists: let arr = new Float32Array([3.14, 6.28, 9.42]);


Always use === and !== to check equality. == and != can induce type casting

*/

/*

In the past, "require" was used to import functions from other files. 
Now we have import and export. Of that way, we can only import what we need.

*/