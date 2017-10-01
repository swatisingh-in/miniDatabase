# miniDatabase

This miniDatabase is implemented in C++ to read an input file, parse the data and store that data in Sqlite3 database. This also allows user to make queries once the data is stored.

The input file contains data in below format:
First line contains column names, second line contains column type and third line onwards is the data.

age income
integer integer
15 8000
20 16000


Query could be of the form:[AVG/MIN/MAX] aggregation_attr grouping_attr

The result of queries is exported to output file, which user can view once all the queries are executed.
