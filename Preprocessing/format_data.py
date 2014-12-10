import collections
import logging
import json

logging.basicConfig(format = '%(asctime)s : %(levelname)s : %(message)s', level = logging.INFO)
logger = logging.getLogger(__name__)

class vocabRepresentation():

    def __init__(self, filename):
        self.textfile = open(filename,'r')
        self.vocab = collections.defaultdict(int)
        self._Wordfrequency()
        self._WordRepresentation()

    def _Wordfrequency(self):
        count =0
        for x,preprocessed_article in enumerate(self.textfile):
            for word in preprocessed_article.split():
                if word != '|':
                    if word not in self.vocab:
                        count +=1
                    self.vocab[word]+=1
            if x %1000 ==0:
                logger.info('@Frequency Map PROGRESS : @%d article, Unique Words : %d',x,count)
        self.unique_words = count
        logger.info('@Frequency Map PROGRESS : @%d article, Unique Words : %d',x,count)


    def _WordRepresentation(self):
        if not self.vocab:
            return "call getWordfrequency first"

        # list words in decreasing order of their frequencies
        # Idea : higher the frequency ==> lower representation in number ==> less space as a string
        wordList = sorted(self.vocab, key = lambda x:self.vocab[x], reverse = True)
        # have mapping from word to its number representation
        self.word_to_number = {word : number for number,word in enumerate(wordList)}
        # have mapping from number to its word representation
        self.number_to_word = {number : word for number,word in enumerate(wordList)}

    def getFrequencies(self):
        return self.vocab

    def getWord_to_number(self):
        return self.word_to_number

    def getNumber_to_word(self):
        return self.number_to_word

    def getUniquecount(self):
        return self.unique_words

def main():
    vocab = vocabRepresentation('bhlda_data.txt')
    word_number = vocab.word_to_number
    # convert text file to number representation
    with open('formatted_data.txt','w') as textfile_new:
        datafile = open('bhlda_data.txt')
        for x,line in enumerate(datafile):
            if x%1000 == 0:
                logger.info('@Formatting Data PROGRESS : @%d article',x)
            # make a list of word representation to be joined later
            formatted_line = [word_number[word] if (word != '|' and word in word_number) else "|" for word in line.split()]
            #write it in textfile_new; map helps in recursively applying first function to list's elements
            textfile_new.write(" ".join(map(str, formatted_line)) + '\n')
    
    # dump word to number map in json file
    file = open('word_to_number.json')
    json.dump(word_number, file)
    file.close
    
    #dump its counterpart
    file = open('number_to_word.json')
    json.dump(vocab.number_to_word, file)
    file.close


if __name__ == '__main__':
    main()
