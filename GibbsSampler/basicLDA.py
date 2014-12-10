
#############################################################################

# Parameters
K = 10 # number of topics
alpha = 0.1 # alpha prior
beta = 0.05 # beta prior

#############################################################################

# Dictionary of non-stopwords
dict_file = open("./../vewpoint_files/dictnostops.txt","r")
dict_set = set(dict_file.read().split('\n'))
word_dict = {word : index for index , word in enumerate(dict_set)}

#############################################################################

# Class to store each article's information
#class article:
#    def __init__(self,a_article,word_dict,dict_set):
        
        # store dict with {word : number of appearances} for each word appearing in the article
        # store dict with {word : number of appearances} for each word appearing in the headline
        # print (self.id)
        # print (self.article_dict)
        

#############################################################################

output_file = open("./../vewpoint_files/text_number.txt","w")

# Populate list of articles
with open("./../vewpoint_files/text_numbered.txt","r") as text_file:
    for line in text_file:
        [id,text] = line.split(":\"") # Separate the article number from the text of the article
        article_set = set(text.split(" ")) & dict_set # Set of all non-stopwords in the article
        article_dict = { word_dict[word] : 0 for word in article_set} # Dictionary of all non-stopwords in the article and their number of occurrances
        for word in text.split(" "):
            if word in article_set:
                article_dict[word_dict[word]]+=1 # Populate dictionary
        output_file.write(str(article_dict)+"\n") # Print output

output_file.close()










#############################################################################

# Randomly initialize z values

#############################################################################

# Class topic
class topic:
    def __init__(self,beta,z):
        # Initialize topic distribution
        # Store n^{i}_{j,r} values for each possible combination
        print ("topic init")
        
# Create list to store topics
list_topics=[]

#############################################################################

#############################################################################

#############################################################################

