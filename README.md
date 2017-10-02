## miniDatabase

This miniDatabase is implemented in C++. Basic functionality includes reading an input file, parsing the data and storing data in Sqlite3 database. This also allows user to make queries once the data is stored in the database.

The input file contains data in below format:

First line contains column names separated by space   
second line contains column type separated by space  
third line onwards data.


Sample:

age income  
integer integer  
15 8000  
20 16000  



Query could be of the form:[AVG/MIN/MAX] aggregation_attr grouping_attr

The result of queries is exported to output file, which can be viewed later once all the queries are executed.