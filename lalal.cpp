#include <iostream>
#include <memory>
#include <string>
#include <cstring>
#include <winsock2.h>
#include <windows.h>

using namespace std;

struct myStruct
{
	int a;
	int b;
	int c;
	char d;
	char y;
	char u;
};


int main(int argc, char* argv[])
{
	
	// shared_ptr<int> p(new int (4));
	// shared_ptr<int> p2(new int (5));
	// cout<<p<< ' '<<*p<<endl;
	// cout<<p2<< ' '<<*p2<<endl<<endl;
	// p.swap(p2);
	// cout<<p<< ' '<<*p<<endl;
	// cout<<p2<< ' '<<*p2<<endl<<endl;
	//p.reset(); // дропає вказівник
	// cout<<p<< ' '<<*p<<endl;
	// cout<<p2<< ' '<<*p2<<endl<<endl;
	// cout << p2.get()<< endl;
	// cout << p2.use_count()<< endl;

	// string str;
	// cout<< " Enter string -> ";
	// getline(cin, str);
	// cout<< str << endl;
	// cout<<str.length()<<endl;
	// cout<<str.size()<<endl;
	
	// string str2;
	// if(str2.empty())
	// {
		// str2=str;
		// cout<<"vony rivni"<<endl;
	// }
	// else
		// cout<<"vony ne rivni\n";
	
	// str = "Hello ";
	// str2 = "World!!";
	// str = str+str2;
	// cout<<str<<endl;
	//23:43-02:57
	
	int hourZmenshuvalne=0, hourVidjemnyk=0, hourRiznycia=0;
	int minZmenshuvalne=0, minVidjemnyk=0, minRiznycia=0;
	
	int count = 0;
	
	for(int i(0); i<11; ++i)
	{
		if(argv[1][i] == ':')
		{
			count++;
		}
		
		if(count == 1)
		{
			if(isdigit(argv[1][i-2]))
				hourZmenshuvalne = atoi( (const char*)&argv[1][i-2]) ;
			else
				hourZmenshuvalne = atoi( (const char*)&argv[1][i-1]) ;
			
			minZmenshuvalne = atoi( (const char*)&argv[1][i+1]);
			count++;
		}
		else if((argv[1][i] == ':') && count > 1)
		{
			
			if(isdigit(argv[1][i-2]))
				hourVidjemnyk = atoi( (const char*)&argv[1][i-2]) ;
			else 
				hourVidjemnyk = atoi( (const char*)&argv[1][i-1]) ;
			
			minVidjemnyk = atoi( (const char*)&argv[1][i+1]);
		}
		
	}
	cout<<hourZmenshuvalne<<':'<<minZmenshuvalne<<" - "<<hourVidjemnyk<<':'<<minVidjemnyk<<" = ";
	
	if((minZmenshuvalne > minVidjemnyk) || (minZmenshuvalne == minVidjemnyk))
		minRiznycia = minZmenshuvalne - minVidjemnyk;
	else
	{
		minRiznycia = minZmenshuvalne + 60 - minVidjemnyk;
		hourZmenshuvalne--;
	}
	
	if((hourZmenshuvalne > hourVidjemnyk) || (hourZmenshuvalne == hourVidjemnyk))
		hourRiznycia = hourZmenshuvalne - hourVidjemnyk;
	else
	{
		hourRiznycia = hourZmenshuvalne + 24 - hourVidjemnyk;
	}
	
	cout<< hourRiznycia<<':'<<minRiznycia;
	
	
	
	
	return 0;
}