#include<iostream>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include <ctime>
#include <cstdlib>
using namespace std;


string cptrtos(char* toConvert)         //Converts char array to string
{
	string str;
	for (int i = 0; toConvert[i]; i++)
		str += toConvert[i];

	return str;
}



string changeString(string str, char x)
{
    srand(time(NULL));

    int n = 0, r= 0;
    n = 1 + rand() % 5;  //How many random places

    cout<<"Total random places are = "<<n<<endl;
    
    if (n > str.length())       //Checks if random number is greater than the length of the string   
        n = str.length();
    

     for (int i=0; i<n; i++)        //Runs till the number of random places generated earlier
     {
        r = rand() % str.length();
        cout<<"The position to replace is = "<<r<<endl;
        str[r] = x;          //Replacing the character at the random place with "x"
     }

    return str;
}

int main()
{

    char buff[100];
    char replace;
    string message;
    int c1_pipe[2];
    int c2_pipe[2];

    pipe(c1_pipe);
    pipe(c2_pipe);

    cout<<"Enter the string = ";
    cin.getline(buff,100);

    //cin.ignore();
    cout<<"Enter the character you want to replace with = ";
    cin>> replace;

    pid_t pid = fork();

    if (pid > 0)        //Parent Process
    {
        wait(NULL);

        pid_t pid1 = fork();

        if (pid1 > 0)       //Parent - recieve from child 2
        {
            wait(NULL);
            close(c1_pipe[0]);
            close(c2_pipe[1]);
            close(c1_pipe[1]);
            read(c2_pipe[0],buff,sizeof(buff));     //Recieving from child 2

            cout<<"The final string is = "<<buff<<endl;
        }

        else if (pid1 == 0)      //Child 2 - has to recieve from child 1 and send info to parent
        {
            close(c1_pipe[1]);
            close(c2_pipe[0]);

            read(c1_pipe[0],buff,sizeof(buff));         //Recieving from child 1
            string buf = cptrtos (buff);
            string output1 = changeString(buf,replace);

            write(c2_pipe[1],output1.c_str(), output1.length());        //Sending to parent

        }

        else
        {
            cout<<"Fork2 failed "<<endl;
        }
        
    }

    else if (pid == 0)     //Child 1    - has to send info to child 2
    {
        close(c1_pipe[0]);      //Reading end of this child
        close(c2_pipe[1]);      //Writing end of child 2
        close (c2_pipe[0]);     //Reading end of child 2

        message = cptrtos(buff);
        string output  = changeString(message,replace);         //Stores the changed string in "output"
        write(c1_pipe[1],output.c_str(), output.length());
    }

    else
    {
        cout<<"Fork1 failed"<<endl;
    }

}
