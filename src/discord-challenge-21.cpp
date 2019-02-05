//============================================================================
// Name        : discord-challenge-21.cpp
// Author      : Perry Harrington
// Version     : 1
// Copyright   : Your copyright notice
// Description : Simultaneous increment 64 uint32 values
//============================================================================

#include <iostream>
#include <string>
using namespace std;

void printbin(uint64_t n)
{
	uint64_t mask;
	for(int i=63;i>-1;i--) {
		mask = 1;
		mask <<= i;
		cout << (((n & mask) !=0) ? "1" : "0");
	}
}

int main() {
	/*
	 * Theory of operation:
	 *
	 * This code assumes you are compiling on a 64bit processor and uses quadwords to implement a 64bit wide vector by 32 bits deep.
	 * The 64 numbers are "pivoted" to store 1 bit of each number in each "row" of data.  So instead of 64 array entries of 32bits, we have 32 array entries of 64 bits,
	 * allowing the processor to simultaneously handle 64 integers.
	 *
	 * The truth table for the binary addition is:
	 *
	 *   0 1 Carry
	 *   -----
	 * 0 0 1 0
	 * 1 1 0 1
	 *
	 * This implements the traditional half-adder via XOR and AND bitwise operations.
	 *
	 * The trick is that we've made the 64 input values into a 64bit wide vector that's 32 stages deep.
	 *
	 * This effectively "pivots" the data from row format to columnar format.
	 *
	 * This conversion from row to column must be undone at the end to retrieve the data.
	 *
	 */

	uint64_t nums[32];
	uint32_t save[64];
	uint32_t a;
	uint64_t cnt=0;
	int n;
	uint64_t reg,bit,carry;

	std::fill_n(nums,32,0);

	while (cin >> a && cnt < 64 && !cin.eof()) {	// fetch up to 64 input values or stop at EOF, cnt is the current column
		save[cnt]=a;						// save for later, to show nice
		for (n=0; n<32; n++) {				// iterate through all 32 bits of each unsigned 32bit integer
			bit = (uint64_t)(a&1) << cnt;	// we examine the lowest order bit because we are shifting 32 bits out, all we care about is if the bit being examined is 0 or 1
											// then we shift that over by cnt columns
			nums[n] = nums[n] | bit; 		// mask the bit (column) 0-63 we're interested in, then OR the bit 0-31 onto that, creating a packed 64bit value sliced by 32 bits
			a>>=1;
		}
		cnt++;
	}

	// This is useful to visualize the 64bit vector of 32 bits
	cout << "Input:"<<endl;
	for (int i=0; i<32 ; i++) {
		printf("nums[%2d]",i);
		printbin(nums[i]);
		cout << endl;
	}

	carry = (uint64_t)~0;			// The carry is initialized to all 1's because XOR 0+1=1, 1+1=0, XOR implements the add, overflow (carry) is implemented via AND

	for (n=0;n<32;n++) {			// 64bit vector 32 bits deep
		reg = nums[n];
		reg = reg ^ carry;			// Implement the add and carry the previous overflow
		carry = nums[n] & carry;	// Implement overflow
		nums[n] = reg;
	}

	cout << "Output:"<<endl;
	for (int i=0; i<32 ; i++) {
		printf("nums[%2d]",i);
		printbin(nums[i]);
		cout << endl;
	}

	cout << endl;
	cout <<"Input:"<<endl;
	for (uint64_t i=0;i<cnt;i++) {
		cout << save[i] <<" ";
	}

	cout << endl;
	cout << "Output:"<<endl;
	for (uint64_t i=0;i<cnt;i++) {
		a=0;										// desired output value
		bit = (uint64_t)((uint64_t)1 << i);			// which column are we looking at
		for(n=31;n>-1;n--) {						// start with MSB first
			reg = (uint64_t)(nums[n] & bit) >> i;	// fetch state of bit in column[i] at row[n], then correct for column location
			a=a|reg<<n;								// take the bit, then shift it by the original bit position [0-31], then OR it
		}
		cout << a <<" ";
	}

	cout << endl;
	return 0;
}
