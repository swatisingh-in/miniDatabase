#include "database.h"

void print_usage(const char *prog_name)
{
	printf("Usage: %s input_file output_file\n", prog_name);
}

int main(int argc, char const *argv[])
{
	string query;
	string aggregation_attr;
	string grouping_attr;
	char answer;

	if (argc < 3){
		print_usage(argv[0]);
		return 0;
	}

	FileInterface dataSrc(argv[1], argv[2]);

	DataBaseInterface miniDB;
	miniDB.insertRows();

	cout << "Do you have a query? Enter y or n" << endl;
	answer = getchar();
	while(tolower(answer) == 'y'){
		cout << "Please enter query: [AVG/MIN/MAX] aggregation_attr grouping_attr" << endl;
		cin.ignore();
		cin >> query >> aggregation_attr >> grouping_attr;

		int start_s=clock();
		if (miniDB.validateQuery(aggregation_attr)) {
			miniDB.queryDatabase(query, aggregation_attr, grouping_attr);
		}
		cout << "***** Query Completed *****" << endl;

		cout << "Do you have another query, Enter y or n" << endl;
		cin.ignore();
		answer = getchar();
	}	
	
	cout << "***** WRITTEN OUTPUT TO FILE *****\n" << "Bye" << endl;

	return 0;
}