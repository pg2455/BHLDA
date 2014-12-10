for unzipping in unix environment:
Run this command from Corpus/corpus/data/

# a is folder like 1987; d is each file in that; perform untar on those
for a in */;do for d in "$a"/*; do tar -zxvf "$d" -C "$a"/;done;done

# remove all .tgz files recursively # might give error -just tweak it after your research online
for a in */; do rm "$a"*.tgz;done
