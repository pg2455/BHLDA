import collections
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import numpy as np
import json
import sys

# accepts a topic file
def createTable(filename,  number_to_word = None,limit = 20):
    # open the file
    textfile = open(filename)
    # table stores list of words
    table = collections.defaultdict(list)
    # load the dictionary mapping number to words
    if not number_to_word:
        number_to_word= eval(open('number_to_word.json','r').read())
    # read it line by line
    for lineno,line in enumerate(textfile):
        table[lineno] = [(number_to_word[word.split(':')[0]],float(word.split(':')[1])) for wordno,word in enumerate(line.split(',')) if wordno < limit]

    return table


def createTableTabularFormat(filename, number_to_word = None, limit = 20):
    # open the file
    textfile = open(filename)
    # table stores list of words
    table = collections.defaultdict(list)
    # load the dictionary mapping number to words
    if not number_to_word:
        number_to_word= eval(open('number_to_word.json','r').read())
    # read it line by line
    for lineno,line in enumerate(textfile):
        table[lineno] = [[number_to_word[word.split(':')[0]],float(word.split(':')[1])] for wordno,word in enumerate(line.split(',')) if wordno < limit]

    from tabulate import tabulate
    tab = []
    for x in table:
        tab.append(tabulate(table[x]))

    return tab

    #return table


def useless():
    #create a text file
    table = createTable('printed_body_only_topics_file.txt')
    with open('results_body_topics.txt','w') as f:
        for i in table:
            string =  'Topic ' + str(i) + ' : \n'
            f.write(string + str(table[i]) + '\n\n\n')

    table = createTable('printed_headline_only_topics_file.txt')
    with open('results_headline_topics.txt','w') as f:
        for i in table:
            string =  'Topic ' + str(i) + ' : \n'
            f.write(string + str(table[i]) + '\n\n\n')


# give me document components and I will visualize them to represent them on 2D map



# for topics
def generateVisTable(filename):
    result =  open(filename,'r')
    distribution = collections.defaultdict(dict)

    # get the topic distribution in to the dictionary
    for x,line in enumerate(result):
        distribution[x] = {combined.split(':')[0]:float(combined.split(':')[1]) for combined in line.split(',')}

    distribution = dict(distribution)

    # create matrix of topics in columns and rows as words
    X = []
    for key in distribution.keys():
        X.append(distribution[key].values())
    X = np.matrix(X)

    number_to_word = json.load('number_to_word.json','r')
    plt.xticks(range(len(number_to_word.values()), number_to_word.values()]
    # create visualization table
    plt.imshow(X)


# for documents
def generateDocTable(filename):
    result = open(filename)
    docmix = collections.defaultdict(dict)
    # get the document mixtures in the list
    for x,line in enumerate(result):
        docmix[x] = [float(proportion.strip()) for proportion in line.split(',')]
    docmix= dict(docmix)
    # get the numpy matrix out of it
    X = [mix for mix in docmix.values()]
    X= np.matrix(X)
    # plot color map
    plt.imshow(X)

def main():
    generateVisTable('./../vewpoint_files/printed_body_topics_file.txt')
    generateDocTable('output_docmix.txt')
    plt.show()



if __name__ == '__main__':
    #create a text file
    table = createTable(sys.argv[0])
    with open(sys.argv[1],'w') as f:
        for i in table:
            string =  'Topic ' + str(i) + ' : \n'
            f.write(string + str(table[i]) + '\n\n\n')
