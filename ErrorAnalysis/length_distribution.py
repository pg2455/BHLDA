# File to take lengths of articles and number of tags in each article, then to see if there is any relation between the two

from matplotlib import pyplot as plt
from math import log

# Set up lists to store values from the data
article_length_bucket_size = 500
article_length_bucket_max = 150000
article_length_buckets_number = [0] * (article_length_bucket_max/article_length_bucket_size)
article_length_buckets_sum = [0] * (article_length_bucket_max/article_length_bucket_size)
label_length_buckets = range(0,20,1)

# Open files
article_length_file = open("./../vewpoint_files/text_length.txt","r")
label_length_file = open("./../vewpoint_files/label_length.txt","r")

# Populate lists
while (1):
    line = article_length_file.readline()
    if not line:
        break
    article_length = int(line.replace('\n',''))
    label_length = int( round(float(label_length_file.readline().replace('\n',''))))
    index =  (article_length - (article_length % article_length_bucket_size))/article_length_bucket_size
    try:
        article_length_buckets_number[index] += 1
        article_length_buckets_sum[index] += label_length
    except:
        print article_length

# Calculate average statistics
article_length_buckets_average = [article_length_buckets_sum[index]/max(article_length_buckets_number[index],1) for index in range(1,article_length_bucket_max/article_length_bucket_size)]

# Plot
plt.plot(article_length_buckets_average,'o')
plt.plot([log(a+1,2) for a in article_length_buckets_number])
plt.xlabel('Length of article = x*500')
plt.ylabel('Average number of labels (blue) \n and log of number of articles of this length (green)')
plt.title(r'Plot of average number of labels vs article length')
plt.show()
