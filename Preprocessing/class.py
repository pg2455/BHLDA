class article(object):

  def __init__(self, text, tags, article_id):
    self.text = text # string of text of article
    self.tags = tags # string of tags with separator %;%
    self.id =  article_id# integer

  def text(self):
    return self.text

# It is most natural for the tags to be a set
  def tag_set(self):
    return set(self.tags.strip().replace('\n',"").replace("%;%","").replace("  "," ").split())

# Tags can also be a list
  def tag_list(self):
    return list(self.tag_set())

  def tags(self):
    return self.tags

  def id(self):
    return self.article_id

## to think upon
 ## functionalities
class corpus(object):

  def __init__(self):
    self.corpus = []

  def add(self,article):
    self.corpus.append(article)

# Example
a = article("text of article","t1 %;% t2 %;% t3",100)
b = article("text of article","t4 %;% t5 %;% t6",101)
c = corpus()
c.add(a)
c.add(b)
