#!/bin/bash

# Length of text of all articles
awk '{ print length($0) }' ./../vewpoint_files/text.txt > ./../vewpoint_files/text_length.txt

# Number of labels in articles
cat ./../vewpoint_files/labels.txt | tr -d '[[:alnum:]]' | tr -d "[\;, ,\",(,)]" | awk '{ print length($0) }' | awk '{print ($1/2+1)}' > ./../vewpoint_files/label_length.txt

python length_distribution.py



#in order to link g++
sudo ln -sv /usr/bin/g++ /usr/bin/g++-4.8 ##g++-4.9 if you upgrade to it
