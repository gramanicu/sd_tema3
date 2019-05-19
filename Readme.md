# Existential Graphs Reasoner

The problem statement can be found [here](https://ocw.cs.pub.ro/courses/sd-ca/teme/tema3).

## Table of Contents

- [Existential Graphs Reasoner](#existential-graphs-reasoner)
  - [Table of Contents](#table-of-contents)
  - [Team organization](#team-organization)
  - [Implementation](#implementation)

## Team organization

From the get-go, we decided to split the homework in half. As the functions were somewhat paired two by two, I (Grama Nicolae) took the first two and the last one , and the other were done by my colleague (Matei Cristian Nicusor). This doesn't mean we weren't aware of the other's code. We had to consult each other, so we could find a solution faster when we found a problem. The only thing we didn't split is this README (which can be easily done just by one person)

As my colleague is not comfortable with using Git (and implicitly GitHub), we couldn't use that to share the code. I still kept a private repository, to have a backup for the project. We mainly used VS Live Share for collaboration.

To simplify some of our work, I modified the Makefile to include different functions, like code beautify, task and styling testing, etc..

## Implementation

Each task in the homework had a specific function that solved it :
1. possible_double_cuts()
2. double_cut()
3. possible_erasures()
4. erase()
5. possible_deiterations()
6. deiterate()

However, additional functions were implemented, because some of the tasks were easier to solve with a recursive method. Also, I felt the need to implement another function to find the path to a node, as the existing method was unsatisfactory (the function didn't find every node it should have found). More details about the code can be found in the comments.

© 2019 Grama Nicolae & Matei Cristian Nicusor