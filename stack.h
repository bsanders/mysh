/* * * * * * * * * * * * * * * * * * * * * * * * * * 
 * Author: Bill Sanders
 * Date: 09/16/2011
 * Description:
 * A Stack class header file, based on a vector.
 * This stack holds strings.
 * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Stack header file */

#include <vector>
#include <string>

//typedef string el_t;

using namespace std;

class Stack
{
	private:
		// Hide access to the vector and our tracker variable
		vector<string> el;
		int top;
	
	public:
		// Public constructor/destructor
		Stack();
		~Stack();
		
		// These empty classes are used for Exception handling.
		class Overflow {};  // The underlying vector (and thus, the stack) is over capacity.
		class Underflow {}; // The underlying vector (and thus, the stack) is empty.

		// Member functions //
		
		// Useful helper functions for a stack
		bool isEmpty();
		bool isFull(); // Always returns false.
		void displayAll();
		void clearIt();
		
		// Note, for consistency with push...
		// ... pop() and topElem() use pass-by-reference
		// Internally, these throw Overflow and Underflow, if necessary
		void push(string);
		void pop(string&);
		void topElem(string&);
};

