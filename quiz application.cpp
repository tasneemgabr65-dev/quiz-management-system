#include <iostream> 
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <random>
#include <ctime>

using namespace std;

// ===================== 1. QUESTION BASE ===================== 
class Question {
protected:
    string text;

public:
    Question(string t) : text(t) {}

    virtual void display() = 0;
    virtual bool checkAnswer(string ans) = 0;
    virtual string save() = 0;
    virtual string getCorrectAnswer() = 0;
    virtual ~Question() {}
};

// ===================== 2. MCQ ===================== 
class MCQ : public Question {
    vector<string> options;
    int correct;

public:
    MCQ(string t, vector<string> opts, int c)
        : Question(t), options(opts), correct(c) {
    }

    void display() override {
        cout << "\n" << text << endl;
        for (int i = 0; i < options.size(); i++)
            cout << i + 1 << ") " << options[i] << endl;
    }

    bool checkAnswer(string ans) override {
        try {
            return stoi(ans) == correct;
        }
        catch (...) {
            return false;
        }
    }

    string getCorrectAnswer() override {
        return to_string(correct) + ") " + options[correct - 1];
    }

    string save() override {
        string data = "MCQ|" + text + "|";
        for (auto& o : options)
            data += o + "|";
        data += to_string(correct);
        return data;
    }
};

// ===================== 3. TRUE / FALSE ===================== //
class TF : public Question {
    bool correct;

public:
    TF(string t, bool c) : Question(t), correct(c) {}

    void display() override {
        cout << "\n" << text << " (true/false)" << endl;
    }

    bool checkAnswer(string ans) override {
        for (auto& c : ans) c = tolower(c);
        bool user = (ans == "true" || ans == "t" || ans == "1");
        return user == correct;
    }

    string getCorrectAnswer() override {
        return correct ? "True" : "False";
    }

    string save() override {
        return "TF|" + text + "|" + (correct ? "1" : "0");
    }
};

// ===================== 4. QUIZ ===================== 
class Quiz {
    string title;
    vector<Question*> questions;

public:
    Quiz(string t) : title(t) {}

    string getTitle() { return title; }

    void addQuestion(Question* q) {
        questions.push_back(q);
    }

    vector<Question*>& getQuestions() {
        return questions;
    }

    void display() {
        cout << "Quiz: " << title << endl;
    }

    int takeQuiz() {
        int score = 0;

        vector<Question*> shuffled = questions;
        shuffle(shuffled.begin(), shuffled.end(),
            default_random_engine(time(0)));

        for (auto q : shuffled) {
            q->display();

            string ans;
            cout << "Answer: ";
            cin >> ans;

            if (q->checkAnswer(ans)) {
                cout << "Correct!\n";
                score++;
            }
            else {
                cout << "Wrong!\n";
                cout << "Correct answer: " << q->getCorrectAnswer() << endl;
            }
        }

        return score;
    }
    
    void save(ofstream& file) {
        file << "QUIZ|" << title << "\n";

        for (auto q : questions)
            file << q->save() << "\n";

        file << "END\n";
    }

    void editQuestion() {
        if (questions.empty()) {
            cout << "No questions available!\n";
            return;
        }

        for (int i = 0; i < questions.size(); i++) {
            cout << i + 1 << ") ";
            questions[i]->display();
        }

        int id;
        cout << "Choose question index: ";
        cin >> id;

        if (id < 1 || id > questions.size()) return;
        id = id - 1;

        cout << "1. Delete\n2. Replace\n";
        int choice;
        cin >> choice;

        if (choice == 1) {
            delete questions[id];
            questions.erase(questions.begin() + id);
            cout << "Deleted!\n";
        }

        else if (choice == 2) {
            int type;
            cout << "1.MCQ 2.TF: ";
            cin >> type;
            cin.ignore();

            string text;
            cout << "Enter new question text:\n";
            getline(cin, text);

            delete questions[id];

            if (type == 1) {
                vector<string> opts(4);
                cout << "Enter 4 options:\n";
                for (auto& o : opts)
                    getline(cin, o);

                int correct;
                cout << "Correct index: ";
                cin >> correct;

                questions[id] = new MCQ(text, opts, correct);
            }
            else {
                string ans;
                cout << "true/false: ";
                cin >> ans;

                questions[id] = new TF(text, ans == "true");
            }

            cout << "Updated!\n";
        }
    }

    ~Quiz() {
        for (auto q : questions)
            delete q;
    }
};

// ===================== 5. ATTEMPT ===================== 
class Attempt {
    string quizTitle;
    int score;
    int total;

public:
    Attempt(string q, int s, int t)
        : quizTitle(q), score(s), total(t) {
    }

    void display() {
        float percent = (float)score / total * 100;

        cout << quizTitle
            << " -> " << score << "/" << total
            << " (" << percent << "%) ";

        if (percent >= 50)
            cout << "PASS\n";
        else
            cout << "FAIL\n";
    }

    string save() {
        return quizTitle + "|" + to_string(score) + "|" + to_string(total);
    }
};

// ===================== 6. STUDENT ===================== 
class Student {
    string name;
    vector<Attempt> attempts;

public:
    Student(string n) : name(n) {}

    string getName() {
        return name;
    }

    void addAttempt(Attempt a) {
        attempts.push_back(a);
    }

    void viewResults() {
        cout << "\n--- RESULTS ---\n";
        for (auto& a : attempts)
            a.display();
    }

    void save(ofstream& file) {
        file << "STUDENT|" << name << "\n";
        for (auto& a : attempts)
            file << a.save() << "\n";
        file << "END\n";
    }
};

// ===================== 7. SYSTEM (LOAD / SAVE) ===================== 
class System {
    vector<Quiz*> quizzes;
    vector<Student*> students;

    const string ADMIN_PASS = "admin123";
    const string STUDENT_PASS = "student123";

public:

    void load() {
        ifstream file("quizzes.txt");
        string line;
        Quiz* current = nullptr;

        while (getline(file, line)) {

            if (line.rfind("QUIZ|", 0) == 0) {
                current = new Quiz(line.substr(5));
                quizzes.push_back(current);
            }
            else if (line == "END") {
                current = nullptr;
            }
            else if (line.rfind("MCQ|", 0) == 0 && current) {
                vector<string> p;
                string temp;

                for (char c : line) {
                    if (c == '|') {
                        p.push_back(temp);
                        temp = "";
                    }
                    else temp += c;
                }
                p.push_back(temp);

                vector<string> opts = { p[2], p[3], p[4], p[5] };
                current->addQuestion(new MCQ(p[1], opts, stoi(p[6])));
            }
            else if (line.rfind("TF|", 0) == 0 && current) {
                vector<string> p;
                string temp;

                for (char c : line) {
                    if (c == '|') {
                        p.push_back(temp);
                        temp = "";
                    }
                    else temp += c;
                }
                p.push_back(temp);

                current->addQuestion(new TF(p[1], p[2] == "1"));
            }
        }

        file.close();
        
        ifstream sfile("students.txt");

        while (getline(sfile, line)) {
            if (line.rfind("STUDENT|", 0) == 0) {
                string name = line.substr(8);
                Student* s = new Student(name);
                students.push_back(s);

                while (getline(sfile, line) && line != "END") {
                    vector<string> p;
                    string temp;

                    for (char c : line) {
                        if (c == '|') {
                            p.push_back(temp);
                            temp = "";
                        }
                        else temp += c;
                    }
                    p.push_back(temp);

                    s->addAttempt(Attempt(p[0], stoi(p[1]), stoi(p[2])));
                }
            }
        }
    }

    void save() {
        ofstream file("quizzes.txt");

        for (auto q : quizzes)
            q->save(file);

        file.close();

        ofstream sfile("students.txt");

        for (auto s : students)
            s->save(sfile);
    }

    // ===================== 8. ADMIN MENU ===================== 
    void adminMenu() {
        string pass;
        cout << "Admin password: ";
        cin >> pass;

        if (pass != ADMIN_PASS) {
            cout << "Wrong password!\n";
            return;
        }

        int choice;

        do {
            cout << "\n--- ADMIN ---\n";
            cout << "1.Create Quiz\n2.Add Question\n3.Delete Quiz\n4.View\n5.Save\n6.Edit Question\n7.Exit\n";
            cin >> choice;

            if (choice == 1) {
                cin.ignore();
                string title;
                getline(cin, title);
                quizzes.push_back(new Quiz(title));
            }
            else if (choice == 2) {
                for (int i = 0; i < quizzes.size(); i++)
                    cout << i + 1 << ") " << quizzes[i]->getTitle() << endl;

                int id;
                cin >> id;

                if (id < 1 || id > quizzes.size()) continue;

                Quiz* q = quizzes[id - 1];

                int type;
                cout << "1.MCQ 2.TF: ";
                cin >> type;
                cin.ignore();

                string text;
                getline(cin, text);

                if (type == 1) {
                    vector<string> opts(4);
                    for (auto& o : opts)
                        getline(cin, o);

                    int correct;
                    cin >> correct;

                    q->addQuestion(new MCQ(text, opts, correct));
                }
                else {
                    string ans;
                    cin >> ans;
                    q->addQuestion(new TF(text, ans == "true"));
                }
            }
           
            else if (choice == 3) {
                int id;
                cin >> id;

                if (id < 1 || id > quizzes.size()) continue;

                id = id - 1;
                delete quizzes[id];
                quizzes.erase(quizzes.begin() + id);
            }
            else if (choice == 4) {
                for (auto q : quizzes)
                    q->display();
            }
            else if (choice == 5) {
                save();
                cout << "Saved!\n";
            }
            else if (choice == 6) {
                for (int i = 0; i < quizzes.size(); i++)
                    cout << i + 1 << ") " << quizzes[i]->getTitle() << endl;

                int id;
                cin >> id;

                if (id < 1 || id > quizzes.size()) continue;

                quizzes[id - 1]->editQuestion();
            }

        } while (choice != 7);
    }

    // ===================== 9. STUDENT MENU ===================== 
    void studentMenu() {
        string pass;
        cout << "Student password: ";
        cin >> pass;

        if (pass != STUDENT_PASS) {
            cout << "Wrong password!\n";
            return;
        }

        string name;
        cout << "Name: ";
        cin >> name;

        Student* s = nullptr;

        for (auto st : students) {
            if (st->getName() == name) {
                s = st;
                break;
            }
        }

        if (!s) {
            s = new Student(name);
            students.push_back(s);
        }

        int choice;

        do {
            cout << "\n--- STUDENT ---\n";
            cout << "1.View\n2.Take Quiz\n3.Results\n4.Exit\n";
            cin >> choice;

            if (choice == 1) {
                for (auto q : quizzes)
                    q->display();
            }
            else if (choice == 2) {
                for (int i = 0; i < quizzes.size(); i++)
                    cout << i + 1 << ") " << quizzes[i]->getTitle() << endl;

                int id;
                cin >> id;

                if (id < 1 || id > quizzes.size()) continue;

                Quiz* q = quizzes[id - 1];

                int score = q->takeQuiz();
                int total = q->getQuestions().size();

                cout << "Score: " << score << "/" << total << endl;

                s->addAttempt(Attempt(q->getTitle(), score, total));
            }
            else if (choice == 3) {
                s->viewResults();
            }

        } while (choice != 4);
    }

    ~System() {
        for (auto q : quizzes)
            delete q;

        for (auto s : students)
            delete s;
    }
};

// ===================== 10. MAIN ===================== 
int main() {
    System sys;
    sys.load();

    int role;
    cout << "1.Admin 2.Student: ";
    cin >> role;

    if (role == 1)
        sys.adminMenu();
    else
        sys.studentMenu();

    sys.save();
}