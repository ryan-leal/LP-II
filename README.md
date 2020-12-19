# LP-2
## Resolutions of the activities and tests of the discipline of programming language II of Federal University of Paraíba (UFPB)

Nessa disciplina estudamos e desenvolvemos conceitos de computação concorrente/paralela e sistemas distribuídos, envolvendo então **Threads, Processos e Sockets** utilizando a linguagem C.

In this discipline we study and develop concepts of concurrent / parallel computing and distributed systems, then involving **Threads, Processes and Sockets** using the C language.

## Exercises: 
- Using the concepts of "lock" and "Fair solutions", develop a roller coaster of Threads, then using the ticket algorithm.

- Using barriers, make 6 threads generate random characters and check if together they form a palindrome.

- Create a program that acts as a server and make it serve simultaneous clients using Threads and receiving a word from the client, the server checks and sends if it is a palindrome, using TCP sockets.

## Tests
### Test 1
1- Make a program that generates 10 threads that add up to a global variable.

2- Create 30 threads and 30 processes to check the difference in creation time.

3- Make a program that creates 10 threads and simulates a hares race, containing maximum values of jumping and jumping capacity per round, until one of them reaches the finish line, declaring the winner and ending the program.

### Test 2
1- Create a program that simulates a motor temperature controller, where one thread increases / decreases the temperature and another waits for the temperature to reach a fixed value and thus returns to the initial temperature (as soon as possible).

2- Create a program that makes two sets of threads add a random value to two global variables, waiting until their sum generates a value greater than or equal to 50, so closing the program, use traffic lights to manage access to the critical section.

3- Create a program that acts as a client, generating 10 random strings of size 6 and send it to the server, using TCP sockets.
