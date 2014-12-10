MongoDb commands

# list all databases
show dbs

#use particular database ; here 'name' is the name of database if it exists
use name

#list the current database
db

#list collections in a database
show collections

#get some articles
coll = db.collection_name
  #LIST few articles
coll.find()
  #list all particular article
coll.findOne()

#use search queries 
