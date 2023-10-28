# Algorithms-and-Data-Structure-Project
Academic project regarding Algorithms and Data Structure 

# Short description
The program was built with a specific focus on optimization (especially on searching words in a dictionary).<br>
The typical workflow is:

- Dictionary creation (~ half a million in most stressful test, word length is fixed and specified at the beginning),
- Definition of "objective word" and maximum number of attempts,
- A set of attempts, for each attempt the program:
  1. Print `not_exists` if the input word is not part of the dictionary,
  2. Print `ko` if the word was not guessed,
  3. Print some symbols (+ / |) if the letter is in the correct place (+), if the letter does not appear in the word ( / ) or if it does appear but not in that specific position ( / ).
- After each attempt the program prints the number of admissible words according to learnt constraints (I.E: words with / letter are not admissible, ...).

# Used data structure
- Optimized Binary Search Trees (BST) that exploits Flexible Array Members to make the structure capable of holding variable string length.
- List/Queue for advanced attempts (to reduce search in dictionary).
