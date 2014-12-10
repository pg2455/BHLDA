
#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <stdlib.h>
#include <time.h> // So that can generate random numbers with different seeds
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <random>

////////////////////////////////////////////////////////////////////////////////////////
//////////////// Parameters ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

const unsigned short int K = 2; // Number of topics
const unsigned int VOCAB_SIZE = 100;//1410004; // Vocabulary

////////////////////////////////////////////////////////////////////////////////////////
//////////////// Functions /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

/* Split string into list of strings
This is used to initialize the word list of each text_body
*/
std::list<std::string> splitStringIntoList(const std::string& a_line)
{
  std::list<std::string> l_word_list; // Initialize word list
  std::istringstream l_oss(a_line); // Convert line into stringstream format
  std::string l_word; // Initialize string to store each individual word
  while(l_oss >> l_word) // Push next word in the string
    { l_word_list.push_back(l_word); } // Populate word list
  return l_word_list;
};

////////////////////////////////////////////////////////////////////////////////////////

/* Generate random list of unsigned short int in range 0 to K
This is used to initialize the allocation list of each text body
*/
void initializeAllocations(const unsigned int& a_number_of_words,
						    std::list<unsigned short int>& a_allocation_list)
{
  for (unsigned int i = 0; i!=a_number_of_words; ++i) // Loop through size of list
    { a_allocation_list.push_back( (unsigned short int)(rand() % K )); } // Populate allocation list
};

////////////////////////////////////////////////////////////////////////////////////////

/* Create array counting the number of allocations to each topic from the allocation list
This is used to initialize d_topic_allocations for each text_body
*/
void initializeTopicAllocations(const std::list<unsigned short int>& a_allocation_list, unsigned short int (& a_topic_allocations)[K])
{
  for (std::list<unsigned short int>::const_iterator it = a_allocation_list.cbegin(); it!= a_allocation_list.cend(); ++it) // Iterate through allocation list
    { a_topic_allocations[*it]+=1; } // Populate topic allocations
};

////////////////////////////////////////////////////////////////////////////////////////

/* Add the words in string of text_body to the vocabulary
 */
void addStringToVocabulary(std::unordered_set<std::string>  &a_vocabulary, const std::string a_line)
{
  std::istringstream l_oss(a_line); // Convert line into stringstream format
  std::string l_word; // Initialize string to store each individual word
  while(l_oss >> l_word) // Push next word in the string
    { a_vocabulary.insert(l_word); } // Add to vocabulary
}

////////////////////////////////////////////////////////////////////////////////////////

/* Generate binomial variable from given number of trials and probability
 */
long newBinomial(const unsigned long long int &a_num_trials, const unsigned int  &a_num_bins, std::default_random_engine &a_generator)
{
  std::binomial_distribution<unsigned long long int> l_distribution (a_num_trials,1.0/float(a_num_bins) ); // Create the distribution
  return l_distribution(a_generator); // Generate from the distribution
}

////////////////////////////////////////////////////////////////////////////////////////
//////////////// Classes ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

/* Class to store the words, allocation for each word and aggregated topic allocations
This class is used to store the headlines or the body of an article
 */
struct text_box
{
  // Constructor. Takes the string of the text and creates the member variables
  text_box(const std::string a_line): d_word_list(splitStringIntoList(a_line))
  {
    initializeAllocations(d_word_list.size(),d_allocation_list); // Initialize the allocation list
    initializeTopicAllocations(d_allocation_list,d_topic_allocations); // Initialize the array of number of allocations for each topic
  };

  // Member variables
  std::list<std::string> d_word_list; // List of words in the text_box
  std::list<unsigned short int> d_allocation_list; // List of allocations, one for each word
  unsigned short int d_topic_allocations[K]; // Array of number of allocations for each topic
};

////////////////////////////////////////////////////////////////////////////////////////

struct topic
{
  // Constructor
  topic():d_num_words(0){std::unordered_map<std::string,unsigned long long int> d_word_topic_allocations;};

  // Function to randomly populate a topic
  void populateTopic(const unsigned long long int &a_num_words, const std::unordered_set<std::string> &a_vocabulary)
  {
    d_word_topic_allocations.reserve(a_vocabulary.size()); // Make the map the size of the vocabulary
    unsigned long long int l_unallocated_number_of_words = a_num_words; // Number of words not yet allocated
    unsigned int l_unallocated_words_in_topic = a_vocabulary.size(); // Number of words for each topic distribution
    unsigned l_seed = std::chrono::system_clock::now().time_since_epoch().count(); // Seed generator for randomly allocating words in topic
    std::default_random_engine l_generator (l_seed); // Seed generator
    unsigned long long int l_new_words_allocated; // Temporary variable counting number of words allocated for current topic during an iteration
    for (std::unordered_set<std::string>::const_iterator it = a_vocabulary.cbegin(); it!=a_vocabulary.cend(); ++it)
      {
	l_new_words_allocated = newBinomial(l_unallocated_number_of_words,l_unallocated_words_in_topic,l_generator); // Number of words allocated for current iteration
	d_word_topic_allocations[*it] = l_new_words_allocated; // Update map with the number of allocations for that word
	--l_unallocated_words_in_topic; // Decrement the number of words in the topic not yet allocated to
	l_unallocated_number_of_words -= l_new_words_allocated; // Decrease the number of words not yet allocated by the number allocated this iteration
	if (l_unallocated_number_of_words==0) {break;} // If no more words left to allocate, break
      }
  };

  // Member variables
  unsigned long long int d_num_words; // Total number of words allocated to the topic
  std::unordered_map<std::string,unsigned long long int> d_word_topic_allocations; // Number of allocations of each word in topic
};


////////////////////////////////////////////////////////////////////////////////////////
//////////////// Functions that require classes ////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

/* Initialize the topic classes
 */

void intializeTopics(const unsigned long long int &a_total_number_of_words, const std::unordered_set<std::string> a_vocabulary, topic (&a_topic_array)[K])
{
  unsigned long long int l_unallocated_number_of_words = a_total_number_of_words; // Number of words not yet allocated
  unsigned l_seed = std::chrono::system_clock::now().time_since_epoch().count(); // Seed generator for randomly allocating words to topics
  std::default_random_engine l_generator (l_seed); // Seed generator
  unsigned long long int l_new_words_allocated; // Temporary variable counting number of words allocated for current topic during an iteration
  for (int i = 0; i!=K; ++i)
    {
      l_new_words_allocated = newBinomial(l_unallocated_number_of_words,K-i,l_generator); // Number of words allocated for current iteration
      a_topic_array[i].populateTopic(l_new_words_allocated, a_vocabulary); // Populate topic
      l_unallocated_number_of_words -= l_new_words_allocated; // Decrease the number of words not yet allocated by the number allocated this iteration
      if (l_unallocated_number_of_words==0) {break;} // If no more words left to allocate, break
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//////////////// Main Function /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

int main()
{
  // Initialize preamble
  srand (time(NULL));
  std::unordered_set<std::string> l_vocabulary;

  // Test case for one string
  std::string l_str = "Hello world world world my very very very good friend a b c d e f g h i j k l";
  addStringToVocabulary(l_vocabulary,l_str);
  text_box l_headline(l_str);

  // Populate the topics
  unsigned long long int l_total_number_of_words = 100000;
  topic topic_array[K];
  intializeTopics(l_total_number_of_words,l_vocabulary,topic_array);

  // Print stuff for debugging

  // for (int i = 0; i!=K; ++i)
  //   {
  //     std::cout<< "New topic" <<std::endl;
  //     for (std::unordered_map<std::string,unsigned long long int>::iterator it = topic_array[i].d_word_topic_allocations.begin();it!=topic_array[i].d_word_topic_allocations.end();++it)
  // 	{
  // 	  std::cout<< " " << it->first << ":" << it->second <<std::endl;
  // 	}
  //   }

  // topic l_topic1(l_total_number_of_words,l_vocabulary);
  // std::cout<< l_topic1.d_num_words <<std::endl;
  // for (std::unordered_map<std::string,unsigned long long int>::iterator it = l_topic1.d_word_topic_allocations.begin();it!=l_topic1.d_word_topic_allocations.end();++it)
  //   {
  //     std::cout<< " " << it->first << ":" << it->second <<std::endl;
  //   }
  
  // unsigned l_seed = std::chrono::system_clock::now().time_since_epoch().count();
  // std::default_random_engine l_generator (l_seed);
  // std::cout << newBinomial(100000,20,l_generator) << std::endl;
  
  // std::cout << l_headline.d_word_list.front() << std::endl;
  // std::cout << l_headline.d_word_list.back() << std::endl;
  // std::cout << l_headline.d_allocation_list.front() << std::endl;
  // std::cout << l_headline.d_allocation_list.back() << std::endl;
  // std::cout << l_headline.d_topic_allocations[0] << std::endl;
  // std::cout << l_headline.d_topic_allocations[1] << std::endl;
  // std::cout << l_headline.d_topic_allocations[2] << std::endl;

  // for (std::unordered_set<std::string>::iterator it = l_vocabulary.begin(); it!= l_vocabulary.end(); ++it)
  //   {
  //     std::cout<< *it <<std::endl;
  //   }

  std::cout << "Hello world" << std::endl;
  return 0;
}
