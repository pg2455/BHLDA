#### USAGE ####
# python table_string_output.py body_topic_file_path headline_topic_file_path number_to_word.json_file_path
## topic files are those which contains 'word:prob' pairs and consists of all words
## number to word json is a dictionary in text file format

import collections
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import numpy as np
import json
import sys

def printTable(filenames, number_to_word = None):
    bodyFile = filenames[0]
    headFile = filenames[1]

    if not number_to_word:
        number_to_word= eval(open('number_to_word.json','r').read())

    bodyTopics = createTable(bodyFile, number_to_word, limit = 20)
    headTopics = createTable(headFile, number_to_word,limit = 20)

    final= [] # stores final formatted strings
    for x in bodyTopics:
        res = returnString(x)
        for (bodyWord,headWord) in zip(bodyTopics[x],headTopics[x]):
            res = res + '\t' + bodyWord[0] + ' '*(16 - len(bodyWord[0])) + '|'+ ' '*8 + str(bodyWord[1]) + ' '*(16-len(str(bodyWord[1]))) \
            + '¡|!\t' + headWord[0] + ' '*(16 - len(headWord[0])) + '|' + ' '*8 + str(headWord[1]) + '\t\n'
    final.append(res)

    return final


def returnString(x):
    #this formatting worked for me. With some calculations and tweaking it experiementally. There was no specific technicality
    # behind this code.
    res = ' '*47 + 'Topic '+ str(x) + '\n'
    res = res + '-'*100 + '\n'
    res = res + ' '*23 + 'BODY' + ' '*22 + ' | ' + ' '*17 + 'HEADLINE' + '\t\t\n'
    res = res + '-'*100 + '\n'
    res = res + '\t' + 'Word' + ' '*(16-len('Word')) + '|'+' '*8 + 'Prob' + ' '*(16-len('Prob')) \
    + '¡|!\t' + 'Word' + ' '*(16-len('Word')) + '|'+' '*8 + 'Prob' + '\t\n'
    res = res + '-'*100 + '\n'
    return res

# accepts a topic file ; working fine!!
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


def main():
    filenames = "../BHLDA100000 (1)/BHLDA/printed_body_topics_file.txt", "../BHLDA100000 (1)/BHLDA/printed_headline_topics_file.txt"
    number_to_word= eval(open('../BHLDA100000 (1)/BHLDA/number_to_word.json','r').read())
    table = printTable(filenames, number_to_word)
    for i in table:
        print(i)


# it takes in three inputs: body topic file name, headline topic file name and number_to_word.json path
## topic file contains wordid:prob tuples for all topics with a topic a line.
if __name__ == '__main__':
    filenames = (sys.argv[0],sys.argv[1])
    number_to_word = eval(open(sys.argv[2],'r').read())
    table = printTable(filenames, number_to_word)
    for i in table:
        print(i)
