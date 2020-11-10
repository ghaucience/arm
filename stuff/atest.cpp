#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <map>

using namespace std;

map<pair<string,string>, string>	devs;

int main(int argc, char *argv[]) {

	devs[pair<string,string>("a", "b")]  = "c";

	for (map<pair<string,string>, string>::iterator it = devs.begin();
				it != devs.end();
				++it) {
		cout << it->first.first << "," << it->first.second << "," << it->second << endl;
	}

	return 0;

}
