/* Body-Healine Latent Dirichlet Allocation (BHLDA)
This code is designed to implement BHLDA. BHLDA is similar to LDA, but each document has two text_boxes, the body and the headline, which are governed by separate distributions.
 */

#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <stdlib.h>
#include <time.h> // So that can generate random numbers with different seeds
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <math.h>

////////////////////////////////////////////////////////////////////////////////////////
//////////////// Parameters ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

const unsigned short int K = 4; // Number of topics
const float ALPHA = 0.2; // Alpha prior
const float BETA = 0.05; // Beta prior
const float KBETA(K*BETA); // Because otherwise have to calculate this value a lot
const unsigned int NUMBER_ITERATIONS = 30; // Number of iterations
const unsigned int LOG_PROB_PRINT_ITERATIONS = 5; // Print the log probabilty every LOG_PROB_PRINT_ITERATIONS iterations
const double LOG_PROB_BIG_CONST = 10000.0; // For taking -log of zero
const unsigned int NUM_WORDS_DISPLAY_PER_TOPIC = 20; // Number of words to display for each topic
//const unsigned int NUM_TEXT_BOXES_PER_DOCUMENT = 1; // Headline weighting vs body
const unsigned int GAMMA = 10; // Headline weighting vs body

////////////////////////////////////////////////////////////////////////////////////////
//////////////// Functions required for classes ////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

/* Split string into list of strings. This is used to initialize the word list of each text_body */
std::list<unsigned int> splitStringIntoList(const std::string &a_line)
{
  std::list<unsigned int> l_word_list; // Initialize word list
  std::istringstream l_oss(a_line); // Convert line into stringstream format
  unsigned int l_word; // Initialize string to store each individual word
  while(l_oss >> l_word) // Push next word in the string
    { l_word_list.push_back(l_word); } // Populate word list
  return l_word_list;
};

/* Generate random list of unsigned short int in range 0 to K. This is used to initialize the allocation list of each text body */
void initializeAllocations(const unsigned int &a_number_of_words, std::list<unsigned short int> &a_allocation_list)
{
  a_allocation_list.clear(); //CHECK IF THIS IS NECESSARY
  for (unsigned int i = 0; i!=a_number_of_words; ++i) // Loop through size of list
    { a_allocation_list.push_back( (unsigned short int)(rand() % K )); } // Populate allocation list
};

/* Create array counting the number of allocations to each topic from the allocation list. This is used to initialize d_topic_allocations for each text_body */
void initializeTopicAllocations(const std::list<unsigned short int>& a_allocation_list, unsigned short int (& a_topic_allocations)[K])
{
  for (std::list<unsigned short int>::const_iterator it = a_allocation_list.cbegin(); it!= a_allocation_list.cend(); ++it) // Iterate through allocation list
    { a_topic_allocations[*it]+=1; } // Populate topic allocations
};

////////////////////////////////////////////////////////////////////////////////////////
//////////////// Classes ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

/* Class to store the words, allocation for each word and aggregated topic allocations. This class is used to store the headlines or the body of an article */
struct text_box
{
  // Constructor. Takes the string of the text and creates the member variables
  text_box(const std::string a_line): d_word_list(splitStringIntoList(a_line))
  {
    std::fill(d_topic_allocations,d_topic_allocations+K, 0); // Need to do this or else it will be initialized to the values in the previously created text_box
    initializeAllocations(d_word_list.size(),d_allocation_list); // Initialize the allocation list
    initializeTopicAllocations(d_allocation_list,d_topic_allocations); // Initialize the array of number of allocations for each topic
  };

  // Member variables
  std::list<unsigned int> d_word_list; // List of words in the text_box
  std::list<unsigned short int> d_allocation_list; // List of allocations, one for each word
  unsigned short int d_topic_allocations[K]; // Array of number of allocations for each topic
};

struct document
{
  // Constructor
  document(const std::string a_body, const std::string a_headline):d_body(a_body),d_headline(a_headline){};

  // Member variables
  text_box d_body; // Store the body text_box
  text_box d_headline; // Store the headline text_box
};

struct topic
{
  // Constructor
  topic():d_num_words(0){std::unordered_map<unsigned int,unsigned long long int> d_word_topic_allocations;};

  // Member variables
  unsigned long long int d_num_words; // Total number of words allocated to the topic
  std::unordered_map<unsigned int,unsigned long long int> d_word_topic_allocations; // Number of allocations of each word in topic
};

////////////////////////////////////////////////////////////////////////////////////////
//////////////// General functions /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

/**************************************************************************************/
/************** Populate functions ****************************************************/
/**************************************************************************************/

/* Populate the topic classes for a single text_box */
void populateTopicsTextBox(topic (&a_topic_array)[K], const text_box &a_text_box)
{
  std::list<unsigned int>::const_iterator l_word_iterator = a_text_box.d_word_list.cbegin(); // Word iterator
  std::list<unsigned short int>::const_iterator l_allocation_iterator = a_text_box.d_allocation_list.cbegin(); // Allocation iterator
  for (; l_word_iterator!=a_text_box.d_word_list.cend(); ++l_word_iterator,++l_allocation_iterator) // Loop through words in the text_box
    {
      ++a_topic_array[*l_allocation_iterator].d_num_words; // Increase the count on words in topic
      ++a_topic_array[*l_allocation_iterator].d_word_topic_allocations[*l_word_iterator]; // Increase count of particular word in topic
    }
}

/* Populate the topic classes for the entire corpus */
void populateTopicsCorpus(const std::unordered_set<unsigned int> &a_body_vocabulary, const std::unordered_set<unsigned int> &a_headline_vocabulary, topic (&a_body_topic_array)[K],  topic (&a_headline_topic_array)[K], const std::list<document> &a_corpus)
{
  // Reserve space for body topics
  for (int i =0; i!=K;++i)
    {
      a_body_topic_array[i].d_word_topic_allocations.reserve(a_body_vocabulary.size()); // Make the map the size of the vocabulary
      for (std::unordered_set<unsigned int>::const_iterator it = a_body_vocabulary.cbegin(); it!=a_body_vocabulary.cend(); ++it)
	{ a_body_topic_array[i].d_word_topic_allocations[*it]=0; } // Set value to zero
    }
  
  // Reserve space for headline topics
  for (int i =0; i!=K;++i)
    {
      a_headline_topic_array[i].d_word_topic_allocations.reserve(a_headline_vocabulary.size()); // Make the map the size of the vocabulary
      for (std::unordered_set<unsigned int>::const_iterator it = a_headline_vocabulary.cbegin(); it!=a_headline_vocabulary.cend(); ++it)
	{ a_body_topic_array[i].d_word_topic_allocations[*it]=0; } // Set value to zero
    } // Make the map the size of the vocabulary

  // Loop through all of the documents, adding the allocations to the topics
  for (std::list<document>::const_iterator it = a_corpus.cbegin(); it!= a_corpus.cend(); ++it)
    {
      populateTopicsTextBox(a_body_topic_array,it->d_body); // Populate the topics for the headline
      populateTopicsTextBox(a_headline_topic_array,it->d_headline); // Populate the topics for the headline
    }
}

/**************************************************************************************/
/************** Reading Functions *****************************************************/
/**************************************************************************************/

/* Add the words in string of text_body to the vocabulary */
void addStringToVocabulary(std::unordered_set<unsigned int>  &a_vocabulary, const std::string a_line)
{
  std::istringstream l_oss(a_line); // Convert line into stringstream format
  unsigned int l_word; // Initialize string to store each individual word
  while(l_oss >> l_word) // Push next word in the string
    { a_vocabulary.insert(l_word); } // Add to vocabulary
}

/* Reads the given file, populates the corpus and vocabulary set */
void readFileAndPopulateCorpus(std::unordered_set<unsigned int> &a_body_vocabulary, std::unordered_set<unsigned int> &a_headline_vocabulary, std::list<document> &a_corpus)
{
  std::string l_line; // String for each file to be read into
  std::ifstream l_raw_text_file ("sample_input.txt"); // Open the file //formatted_data.txt//small_
  std::string delimiter = "|";
  if (l_raw_text_file.is_open())
    {
      while ( getline(l_raw_text_file,l_line) ) // Read line from the file
	{ 
	  std::string l_headline_line = l_line.substr(0,l_line.find(delimiter)); // Read the headline
	  std::string l_body_line = ( (l_line == l_headline_line) ? "" : l_line.erase(0, l_line.find(delimiter) + delimiter.length()) ); // Read the body. If there is no body then read empty string.
	  a_corpus.push_back(document(l_body_line,l_headline_line)); // Add document to list
	  addStringToVocabulary(a_body_vocabulary,l_body_line); // Add words to the body vocabulary
	  addStringToVocabulary(a_headline_vocabulary,l_headline_line); // Add words to the headline vocabulary
	}
    }
}

/**************************************************************************************/
/************** Updating Functions ****************************************************/
/**************************************************************************************/

/* Determine the category for give target value and cum sum array */
unsigned short int determineCategory(const float (&a_topic_weights)[K], const float &a_target_value, unsigned short int l_lower, unsigned short int l_upper)
{
  if (l_upper == l_lower +1) {return l_upper;} // If only two elements then the upper one should be returned
  unsigned short int l_middle = (l_upper-l_lower)/2+l_lower; // Initialize middle element
  if (a_topic_weights[l_middle]>a_target_value) {return determineCategory(a_topic_weights,a_target_value,l_lower,l_middle);} // If the middle element has a value larger than the target value, then make that the upper value and recurse
  return determineCategory(a_topic_weights,a_target_value,l_middle,l_upper); // Else recurse with middle element as the lower value
}

/* Update the allocation of a single variable in BHLDA */
void updateAllocation(const unsigned int &a_word, unsigned short int &a_allocation, text_box &a_text_box, unsigned short int (&a_other_topic_allocations)[K], bool a_body_or_not, topic (&a_topic_array)[K], const float &a_VBETA)
{
  // First remove current allocation from the topic array and the text_box allocation
  --a_topic_array[a_allocation].d_num_words;
  --a_topic_array[a_allocation].d_word_topic_allocations[a_word]; // TODO If it is zero then remove from the unordered_map
  --a_text_box.d_topic_allocations[a_allocation];
  
  // Create array of length K to store the categorical probabilities
  float l_topic_weights[K];
  
  // Loop through the topics doing cum sum populating array
  for (int i =0; i!=K; ++i)
    {
      l_topic_weights[i] = (i>0) ? l_topic_weights[i-1]:0; // Cumulate the variable
      // TODO Check if word is in topic distribution or if it is zero
      l_topic_weights[i] += ( a_body_or_not ? ((float)( a_text_box.d_topic_allocations[i]+ GAMMA*a_other_topic_allocations[i] +ALPHA )) : ((float)( GAMMA*a_text_box.d_topic_allocations[i]+ a_other_topic_allocations[i] + ALPHA )) )
	* ((float)( a_topic_array[i].d_word_topic_allocations[a_word] + BETA )) / (float( a_topic_array[i].d_num_words + a_VBETA )); // Add the LDA (unnormalized) probability. Depending on whether the heading or body is being updated, use one of the two formulas, indicated by the binary variable.
    }

  // Decide on the allocation using a binary search on the cum sum array
  float l_target_value =  l_topic_weights[K-1]*((float) rand() / (RAND_MAX)); // Value to be found in the binary search
  unsigned short int l_allocation; // Stores the allocation
  if (l_topic_weights[0]>l_target_value){l_allocation = 0;} // Check lower bound
  else if (l_topic_weights[K-2]<=l_target_value){l_allocation = K-1;} // Check upper bound
  else {l_allocation = determineCategory(l_topic_weights,l_target_value,0,K-1);} // Implement binary search

  // Update current allocation from the topic array and the text_box allocations
  a_allocation = l_allocation;
  ++a_topic_array[a_allocation].d_num_words;
  ++a_topic_array[a_allocation].d_word_topic_allocations[a_word];
  ++a_text_box.d_topic_allocations[a_allocation];
}

/* Apply the LDA update to a text box */
void updateTextBox(text_box &a_text_box, unsigned short int (&a_other_topic_allocations)[K], bool a_body_or_not, topic (&a_topic_array)[K], const float &a_VBETA)
{
  std::list<unsigned int>::iterator l_word_iterator = a_text_box.d_word_list.begin(); // Word iterator
  std::list<unsigned short int>::iterator l_allocation_iterator = a_text_box.d_allocation_list.begin(); // Allocation iterator
  for (; l_word_iterator!=a_text_box.d_word_list.end(); ++l_word_iterator,++l_allocation_iterator) // Loop through words in the text_box
    { updateAllocation(*l_word_iterator, *l_allocation_iterator, a_text_box, a_other_topic_allocations, a_body_or_not, a_topic_array, a_VBETA); } // Update allocation for particular word
}

/* Apply the LDA update to a document */
void updateDocument(document &a_document, topic (&a_body_topic_array)[K], topic (&a_headline_topic_array)[K],  const float &a_body_VBETA, const float &a_headline_VBETA)
{
  updateTextBox(a_document.d_body, a_document.d_headline.d_topic_allocations, true, a_body_topic_array, a_body_VBETA); // Apply update to the body
  updateTextBox(a_document.d_headline, a_document.d_body.d_topic_allocations, false, a_headline_topic_array, a_headline_VBETA); // Apply update to the headline
}

/* Apply the LDA update to the corpus */
void updateCorpus(std::list<document> &a_corpus, topic (&a_body_topic_array)[K], topic (&a_headline_topic_array)[K], const float &a_body_VBETA, const float &a_headline_VBETA)
{
  for (std::list<document>::iterator it = a_corpus.begin(); it!= a_corpus.end(); ++it) // Loop through all documents
    { updateDocument(*it, a_body_topic_array, a_headline_topic_array, a_body_VBETA, a_headline_VBETA); } // Update each document
}

/**************************************************************************************/
/************** Printing functions ****************************************************/
/**************************************************************************************/

/* Functor to sort through pairs in topic. Used for printing topics */
bool wayToSort(std::pair<unsigned int,unsigned long long int> i, std::pair<unsigned int,unsigned long long int> j)
{return i.second > j.second ;}

/* Print all of the topics with their word distribution */
void printTopics(const topic (&a_topic_array)[K],const std::string a_identifier)
{
  std::ofstream l_printed_topics_file;
  l_printed_topics_file.open( ("./../vewpoint_files/printed_" + a_identifier + "_topics_file.txt") );
  
  for (int i = 0; i!=K; ++i) // For each topic
    {
      if (a_topic_array[i].d_num_words==0) // If no words in topic then no distribution exists (this happens when the text file only contains the body, no headline)
	{ l_printed_topics_file<<"No allocations to this topic\n"; }
      else
	{
	  // Use a vector to sort the topic distribution in the order of word probabilities
	  std::vector< std::pair<unsigned int,unsigned long long int> > l_sorted_vector; // Vector to store pair values and then to sort them
	  l_sorted_vector.reserve(a_topic_array[i].d_num_words); // Reserve the space in memory
	  for (std::unordered_map<unsigned int,unsigned long long int>::const_iterator it = a_topic_array[i].d_word_topic_allocations.cbegin();it!=a_topic_array[i].d_word_topic_allocations.cend();++it) // Loop through topic distribution
	    { l_sorted_vector.push_back(std::make_pair(it->first,it->second));} // Add pair to vector
	  std::sort(l_sorted_vector.begin(),l_sorted_vector.end(),wayToSort); // Sort the vector
	  std::vector< std::pair<unsigned int,unsigned long long int> >::iterator end_it = l_sorted_vector.end();//l_sorted_vector.begin()+NUM_WORDS_DISPLAY_PER_TOPIC;
	  for (std::vector< std::pair<unsigned int,unsigned long long int> >::iterator it = l_sorted_vector.begin(); it!= end_it; ++it) // Loop through vector
	    {
	      l_printed_topics_file<< it->first << ":" << ((float)(it->second))/a_topic_array[i].d_num_words; // Print pairs
	      if ( (it+1) != end_it) {l_printed_topics_file<<",";}
	    }
	  l_printed_topics_file<<"\n";
	}
    }
  l_printed_topics_file.close();
}

/* Print textbox with its topic mixture */
void printTextBox(const text_box &a_text_box, std::ofstream &a_printed_corpus_file)
{
  for (int i = 0; i!=K;++i)
    {
      a_printed_corpus_file << ( (float)(a_text_box.d_topic_allocations[i]) )/a_text_box.d_word_list.size();
      if ( (i+1)!=K ) {a_printed_corpus_file << ",";}
    }
}

/* Print document with its topic mixture */
void printDocument(const document &a_document, std::ofstream &a_printed_corpus_file)
{
  printTextBox(a_document.d_headline, a_printed_corpus_file); // Print headline
  a_printed_corpus_file<<"|";
  printTextBox(a_document.d_body, a_printed_corpus_file); // Print body
  a_printed_corpus_file<<"\n";
}

/* Print all of the documents with their topic mixture */
void printCorpus(const std::list<document> &a_corpus)
{
  std::ofstream l_printed_corpus_file;
  l_printed_corpus_file.open("./../vewpoint_files/printed_corpus_file.txt");
  for (std::list<document>::const_iterator it = a_corpus.cbegin(); it!= a_corpus.cend(); ++it) // Loop through all documents
    { printDocument(*it,l_printed_corpus_file); } // Print each document
  l_printed_corpus_file.close();
}

/**************************************************************************************/
/************** Log probability functions *********************************************/
/**************************************************************************************/

/* Log probability for a topic given the beta distribution */
double logProbabilityTopic(topic &a_topic)
{
  double l_sum = 0;
  for (std::unordered_map<unsigned int,unsigned long long int>::iterator it = a_topic.d_word_topic_allocations.begin();it!= a_topic.d_word_topic_allocations.end(); ++it)
    { l_sum += ( (it->second==0) ? LOG_PROB_BIG_CONST : (BETA-1)*log(it->second));} // Add the probability value. If the argument is zero then add large constant so that it doesn't blow up to infinity.
  return l_sum;
}

/* Log probability for an individual document */
double logProbabilityTextBox(text_box &a_text_box, topic (&a_topic_array)[K])
{
  if (a_text_box.d_word_list.size() == 0) {return 0;}

  // Add the topic mixture probabilty given alpha
  double l_sum = -log(a_text_box.d_word_list.size());
  for (int i = 0; i!=K; ++i)
    { l_sum += ( (a_text_box.d_topic_allocations[i]==0) ? LOG_PROB_BIG_CONST : (ALPHA-1)*log(a_text_box.d_topic_allocations[i]));} // Add the probability value. If the argument is zero then add large constant so that it doesn't blow up to infinity.

  // Add the probability of each allocation
  std::list<unsigned int>::iterator l_word_iterator = a_text_box.d_word_list.begin(); // Word iterator
  std::list<unsigned short int>::iterator l_allocation_iterator = a_text_box.d_allocation_list.begin(); // Allocation iterator
  for (; l_word_iterator!=a_text_box.d_word_list.end(); ++l_word_iterator,++l_allocation_iterator) // Loop through words in the text_box
    {
      l_sum += log(a_text_box.d_topic_allocations[*l_allocation_iterator]) - log(a_text_box.d_word_list.size())
	+ log(a_topic_array[*l_allocation_iterator].d_word_topic_allocations[*l_word_iterator]) - log(a_topic_array[*l_allocation_iterator].d_num_words); // Add the probabilty for each allocation
    }
  
  return l_sum;
}

/* Log probability sum. Calculates the log probabilty of the allocations up to a constant factor */
void logProbabilitySum(std::list<document> &a_corpus, topic (&a_body_topic_array)[K], topic (&a_headline_topic_array)[K])
{
  double l_sum = 0; // Stores the current sum of the probabilities
  for (int i = 0; i!=K; ++i) // Loop through the topic distributions and add their probability
    { l_sum += logProbabilityTopic(a_body_topic_array[i]) + logProbabilityTopic(a_headline_topic_array[i]); }
  for (std::list<document>::iterator it = a_corpus.begin(); it!= a_corpus.end(); ++it) // Loop through all documents and add their probabilities
    { l_sum += logProbabilityTextBox(it->d_body,a_body_topic_array) + GAMMA*logProbabilityTextBox(it->d_headline,a_headline_topic_array) ; }

  std::cout << "The log sum probabilty is (up to a constant): "<< l_sum << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////
//////////////// Main Function /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

int main()
{
  // Initialize preamble
  srand (time(NULL)); // Create seed for generating random variables
  std::unordered_set<unsigned int> l_body_vocabulary; // Initialize the vocabulary
  std::unordered_set<unsigned int> l_headline_vocabulary; // Initialize the vocabulary
  std::list<document> l_corpus; // Initialize the list of documents
  topic l_body_topic_array[K]; // Initialize body topics
  topic l_headline_topic_array[K]; // Initialize headline topics

  std::cout<<"Begin reading"<<std::endl;

  // Read lines from file
  readFileAndPopulateCorpus(l_body_vocabulary, l_headline_vocabulary, l_corpus);

  std::cout<<"Begin populate"<<std::endl;
  
  // // Populate the topics
  populateTopicsCorpus(l_body_vocabulary,l_headline_vocabulary,l_body_topic_array,l_headline_topic_array,l_corpus); // Populate topics

  // Run LDA
  float l_body_VBETA = l_body_vocabulary.size()*BETA; // Size of the vocabulary multiplied by beta. Calculated now so that doesn't need to be recalculated every iteration.
  float l_headline_VBETA = l_headline_vocabulary.size()*BETA; // Size of the vocabulary multiplied by beta. Calculated now so that doesn't need to be recalculated every iteration.
  for (unsigned int i = 0; i!= NUMBER_ITERATIONS; ++i)
    {
      updateCorpus(l_corpus, l_body_topic_array, l_headline_topic_array, l_body_VBETA, l_headline_VBETA); // Apply the LDA update to the corpus
      if ( (i%LOG_PROB_PRINT_ITERATIONS) ==1) // Print the log probability to get an idea of the progress
	 {
	   std::cout<< "Number of iterations: "<< i <<std::endl;
	   logProbabilitySum(l_corpus, l_body_topic_array, l_headline_topic_array);
	 }
    }
  
  // // Print topics
  printTopics(l_body_topic_array,"body");
  printTopics(l_headline_topic_array,"headline");
  
  // Print corpus
  printCorpus(l_corpus);
  
  return 0;
}



////////////////////////////////////////////////////////////////////////////////////////
//////////////// Discared code /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////


  // // Test case for one string
  // std::string l_str = "650 758 659 291 22 1 43 2 399 44 0 14 1 8 153 336 144 421 394 267 43 2 399 44 0 14 1 8 153 336 144 421 394 267 431 329 709 65 594 651 798 812 609 198 65 354 1004 126 502 498 863 239 731 162 406 2 7";
  // addStringToVocabulary(l_vocabulary,l_str);
  // text_box l_headline(l_str);

////////////////////////////////////////////////////////////////////////////////////////

  // topic l_topic1(l_total_number_of_words,l_vocabulary);
  // std::cout<< l_topic1.d_num_words <<std::endl;
  // for (std::unordered_map<unsigned int,unsigned long long int>::iterator it = l_topic1.d_word_topic_allocations.begin();it!=l_topic1.d_word_topic_allocations.end();++it)
  //   {
  //     std::cout<< " " << it->first << ":" << it->second <<std::endl;
  //   }
  
////////////////////////////////////////////////////////////////////////////////////////

  // unsigned l_seed = std::chrono::system_clock::now().time_since_epoch().count();
  // std::default_random_engine l_generator (l_seed);
  // std::cout << newBinomial(100000,20,l_generator) << std::endl;

////////////////////////////////////////////////////////////////////////////////////////
  
  // std::cout << l_headline.d_word_list.front() << std::endl;
  // std::cout << l_headline.d_word_list.back() << std::endl;
  // std::cout << l_headline.d_allocation_list.front() << std::endl;
  // std::cout << l_headline.d_allocation_list.back() << std::endl;
  // std::cout << l_headline.d_topic_allocations[0] << std::endl;
  // std::cout << l_headline.d_topic_allocations[1] << std::endl;
  // std::cout << l_headline.d_topic_allocations[2] << std::endl;

  // for (std::unordered_set<unsigned intg>::iterator it = l_vocabulary.begin(); it!= l_vocabulary.end(); ++it)
  //   {
  //     std::cout<< *it <<std::endl;
  //   }

////////////////////////////////////////////////////////////////////////////////////////

  //intializeTopics(l_total_number_of_words,l_vocabulary,l_topic_array); // Populate topics with random allocations

// ////////////////////////////////////////////////////////////////////////////////////////

// /* Generate binomial variable from given number of trials and probability
//  */
// unsigned long long int newBinomial(const unsigned long long int &a_num_trials, const unsigned int  &a_num_bins, std::default_random_engine &a_generator)
// {
//   // Deal with the trivial cases first
//   if (a_num_bins==0){return 0;}
//   else if (a_num_bins==1){return a_num_trials;}

//   // If not a trivial case
//   std::binomial_distribution<unsigned long long int> l_distribution (a_num_trials,1.0/float(a_num_bins) ); // Create the distribution
//   unsigned long long int l_value_generated =  l_distribution(a_generator); // Generate value from distribution
//   return ((l_value_generated<=a_num_trials) ? l_value_generated : 0); // Generate from the distribution ensuring no overflow (this was a problem previously)
// }

////////////////////////////////////////////////////////////////////////////////////////
  
  // void populateTopic(const unsigned long long int &a_num_words, const std::unordered_set<unsigned int> &a_vocabulary)
  // {
  //   d_word_topic_allocations.reserve(a_vocabulary.size()); // Make the map the size of the vocabulary
  //   unsigned long long int l_unallocated_number_of_words = a_num_words; // Number of words not yet allocated
  //   unsigned int l_unallocated_words_in_topic = a_vocabulary.size(); // Number of words for each topic distribution
  //   unsigned l_seed = std::chrono::system_clock::now().time_since_epoch().count(); // Seed generator for randomly allocating words in topic
  //   std::default_random_engine l_generator (l_seed); // Seed generator
  //   unsigned long long int l_new_words_allocated; // Temporary variable counting number of words allocated for current topic during an iteration
  //   for (std::unordered_set<unsigned int>::const_iterator it = a_vocabulary.cbegin(); it!=a_vocabulary.cend(); ++it)
  //     {
  // 	l_new_words_allocated = newBinomial(l_unallocated_number_of_words,l_unallocated_words_in_topic,l_generator); // Number of words allocated for current iteration
  // 	d_word_topic_allocations[*it] = l_new_words_allocated; // Update map with the number of allocations for that word
  // 	--l_unallocated_words_in_topic; // Decrement the number of words in the topic not yet allocated to
  // 	l_unallocated_number_of_words -= l_new_words_allocated; // Decrease the number of words not yet allocated by the number allocated this iteration
  // 	if (l_unallocated_number_of_words==0) {break;} // If no more words left to allocate, break
  //     }
  // };

////////////////////////////////////////////////////////////////////////////////////////

// void intializeTopics(const unsigned long long int &a_total_number_of_words, const std::unordered_set<unsigned int> a_vocabulary, topic (&a_topic_array)[K])
// {
//   unsigned long long int l_unallocated_number_of_words = a_total_number_of_words; // Number of words not yet allocated
//   unsigned l_seed = std::chrono::system_clock::now().time_since_epoch().count(); // Seed generator for randomly allocating words to topics
//   std::default_random_engine l_generator (l_seed); // Seed generator
//   unsigned long long int l_new_words_allocated; // Temporary variable counting number of words allocated for current topic during an iteration
//   for (int i = 0; i!=K; ++i)
//     {
//       l_new_words_allocated = newBinomial(l_unallocated_number_of_words,K-i,l_generator); // Number of words allocated for current iteration
//       a_topic_array[i].populateTopic(l_new_words_allocated, a_vocabulary); // Populate topic
//       l_unallocated_number_of_words -= l_new_words_allocated; // Decrease the number of words not yet allocated by the number allocated this iteration
//       if (l_unallocated_number_of_words==0) {break;} // If no more words left to allocate, break
//     }
// }
