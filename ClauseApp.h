#ifndef TEST_CLAUSEAPP_H
#define TEST_CLAUSEAPP_H

#include <string>
#include <array>
#include <vector>

using std::string, std::vector;

struct Clause{
    string clause;
    string translation;
    size_t omittedWordIndex;
    vector<size_t> answersIndexes;
    size_t correctIndex;
};

vector<size_t> generateRandomAnswers(size_t size, size_t elemsCount, size_t answerIndex);

class AppEngine {
private:
    vector<string> allAnswers;
    vector<Clause> clauses;
    size_t currentIndex{};
    size_t correctAnswersCount {};
    const string filepath;
public:
    constexpr static size_t ANSWERS_COUNT = 4;
    explicit AppEngine(string);
    void readFile();
    void refactorClause(Clause&);
    [[nodiscard]] Clause currentClause() const;
    bool isCorrect(size_t answer);
    void nextClause();
    void addPoint();
    string getPointsString();
    void addQuestion(const string& clauseText, const string& translation);
    void writeToFile(Clause);
    string getAnswer(size_t index);
};


#endif //TEST_CLAUSEAPP_H
