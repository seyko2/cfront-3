#include <String.h>
#include <Map.h>
#include <stream.h>

main() {
        Map<String,int> count;
        String word;

        while (cin >> word)
                count[word]++;

        cout << count << endl;
	return 0;
}
