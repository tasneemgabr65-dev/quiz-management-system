# Quiz Management System

A console-based quiz application written in C++, with separate admin and student roles.

## Features
- **Admin:** create and delete quizzes, add, edit, and delete questions, and save data to files
- **Student:** view quizzes, take a quiz with shuffled questions, and see results with percentage and pass/fail
- **Question types:** multiple choice (MCQ) and true/false
- **Persistence:** quizzes and student results are saved to and loaded from text files (`quizzes.txt`, `students.txt`)

## Concepts used
- Object-oriented programming: abstract base class `Question` with `MCQ` and `TF` subclasses (inheritance and polymorphism)
- File handling with `ifstream` and `ofstream`
- Dynamic memory management with constructors and destructors
- STL: `vector`, `shuffle`

## How to run
1. Compile: `g++ "quiz application.cpp" -o quiz`
2. Run: `./quiz` (on Windows: `quiz.exe`)
3. Choose 1 for Admin or 2 for Student.

Default passwords (for demo only): admin `admin123`, student `student123`.

## Author
Tasneem Ahmed, Computer and Information Systems student, Egyptian Chinese University
