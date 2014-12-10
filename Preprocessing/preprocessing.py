import pymongo
import nltk
import textblob
import functools32
import logging
import re

# log to keep track of progress of preprocessing
logging.basicConfig(format = '%(asctime)s : %(levelname)s : %(message)s', level = logging.INFO)
logger = logging.getLogger(__name__)

# cleaner class for cleaning text articles
class clean():
    def __init__(self):
        self.lemmatize = (functools32.lru_cache(maxsize = 1000000))(nltk.stem.WordNetLemmatizer().lemmatize)
        text = open('stopwords.txt').read().split(',')
        self.stopwords = [word.strip() for word in text] # proper stopwords

    def getBody(self,content):
        if not content:
            return None
        #remove white spaces from the ends
        content = content.strip()
        #remove first word if it is 'LEAD:'
        if content[:5] == 'LEAD:':
            content = content[5:]
        # if the body is emppty or none then return None
        if not content:
            return None
        content = content.strip()
        # make a blob which will automatically generate tags for words
        content = textblob.TextBlob(content)
        # infer tags; remove punctuation
        words_and_tags = content.tags  # contains (word,tag) tuples in list
        #remove stopwords; apply filters on words; get wordnet tags; replace with 'NUM' if it contans number self. v# second version simply removes numbers
        finalWords_and_tags = [(word,getWordnet_posTag(tag)) for (word,tag) in words_and_tags if (word not in self.stopwords and basic_filter(word) and not ifNumber(word))]
        #convert the words into their basest form
        words = [self.lemmatize(word,wordnet_tag).lower() for (word,wordnet_tag) in finalWords_and_tags if wordnet_tag ]
        return " ".join(words)

    # in case special formatting for heading
    def getHeading(self,head):
        return self.getBody(head)

    def getProcessed(self,body,head):
        return self.getBody(body),self.getHeading(head)


def basic_filter(word):
    #length of word
    if 4 <= len(word) <= 20:
        return True
    return False
    #others ??



#keep only those words which are | adjective | verb | noun | adverb |
def getWordnet_posTag(tag):
    if tag.startswith('J'):
        return nltk.corpus.wordnet.ADJ
    elif tag.startswith('V'):
        return nltk.corpus.wordnet.VERB
    elif tag.startswith('N'):
        return nltk.corpus.wordnet.NOUN
    elif tag.startswith('R'):
        return nltk.corpus.wordnet.ADV
    else:
        return ''


def ifNumber(token):
	if re.search('[0-9]', token):
		return True
	return False



def main():
    # establish connection with database and get its iterator through find()
    mongo_iterator = pymongo.Connection().articles.collection_1.find()

    with open('bhlda_data.txt','a') as textfile, open('data_article_ids.txt','a') as ids:
        count=0
        cleaner = clean()
        # 109000 is just because the process broke in between and it doesn't seem to be reasonable
        # to run it again. So skip the iterator to that number
        for x,i in enumerate(mongo_iterator[109000:]):
            if all(key in  i['metadata'] for key in ('heading', 'body')):
                body_head = (i['metadata']['body'], i['metadata']['heading'])
                # catching unicode decode error and others too : better not to deal with it
                try:
                    body,heading = cleaner.getProcessed(*body_head)
                    #print out progress
                    if x %1000 ==0:
                        logger.info("PROGRESS : @ %d, # USEFUL ARTICLES : %d",x,count)
                    if (body and heading):
                        textfile.write(heading + " | " + body + '\n')
                        ids.write(str(i['_id'])+'\n')
                        count+=1
                except:
                    pass

if __name__ == '__main__':
    main()

    ### REMARKS
    ## 1) sales - sale :: can appear together if they were passed with different tags in lemmatizer
    ## so can killing-killed; times-time; shares-share, etc.
    ## 2) num is noise and therefore is removed in this version
    ## 3) quotation appears in headline as 'Quotation of the Day'
    ## 
