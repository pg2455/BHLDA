# get the dictionary corresponding to 19 topics for body and headline
# total kl =  sum of individual topic's kl divergence
# kl = sum_over_i(q_i*ln(p_i/q_i)) # make sure that q_i is not zero....

## take only the words where q_i is not zero
##

body,headline =  {},{}

f = open('printed_body_topics_file.txt')
for x,line in enumerate(f):
    body[x] = {i.split(':')[0] : float(i.split(':')[1]) for i in line.strip().split(',')}

f = open('printed_headline_topics_file.txt')
for x,line in enumerate(f):
    headline[x] = {i.split(':')[0] : float(i.split(':')[1]) for i in line.strip().split(',')}

def getKL(i):
    return KLDivergence(body[i],headline[i])

def KLCorpus():
    total  = 0
    for i in range(1,20):
        total = total + getKL(i)
    return total/19

## q is headline and p is body
## sum(topic2 * ln(topic1/topic2)) # consider words common in both headline and body
def KLDivergence(topic1, topic2):
    kl = 0
    for word,prob in topic2.iteritems():
        if prob !=0 and topic1[word] != 0:
            kl = kl + prob*math.log(topic1[word]/prob)
    return kl



i = [0.1,0.9]
j = [0.9,0.1]

def KL(i,j):
    for j1,i1 in zip(i,j):
        kl = kl + j1*ln(i1/j1)
