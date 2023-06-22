#include <algorithm>
#include <fstream>
#include <random>
#include <ctime>
#include <QApplication>
#include "ClauseApp.h"
using namespace std;

#include <iostream>

vector<size_t> generateRandomAnswers(size_t size, size_t elemsCount, size_t answerIndex) {
    vector<size_t> all(size);
    vector<size_t> answers;
    iota(begin(all), end(all), 0);
    all.erase(all.begin()+(long long) answerIndex);
    sample(all.begin(),
           all.end(),
           back_inserter(answers),
           elemsCount-1,
           mt19937{random_device{}()}
           );
    return answers;
}

AppEngine::AppEngine(string path) : filepath(std::move(path)) {
    srand(time(nullptr));
    readFile();
    for (Clause& clause: clauses) refactorClause(clause);
    shuffle(begin(clauses), end(clauses), mt19937{random_device{}()});
}

void AppEngine::readFile() {
    ifstream file(filepath);
    file.exceptions(std::ifstream::failbit | std::ifstream::eofbit | std::ifstream::badbit);
    if(!file) {
        throw std::invalid_argument("Error while opening the file");
    } try {
        while (file) {
            string word;
            Clause clause;
            getline(file, clause.clause, ';');
            getline(file, clause.translation, ';');
            getline(file, word, ';');
            file >> clause.omittedWordIndex;
            file.ignore(1);
            clauses.emplace_back(std::move(clause));
            allAnswers.emplace_back(std::move(word));
        }
    } catch (const std::exception&) {}
    file.close();
}

void AppEngine::refactorClause(Clause &clause) {
    size_t wordIndex = clause.clause.find(allAnswers[clause.omittedWordIndex]);
    clause.clause.replace(wordIndex, allAnswers[clause.omittedWordIndex].length(), "___");
    clause.clause[0] = (char) toupper(clause.clause[0]);

    clause.answersIndexes = generateRandomAnswers(clauses.size(), ANSWERS_COUNT, clause.omittedWordIndex);
    clause.correctIndex = rand() % ANSWERS_COUNT;
    clause.answersIndexes.insert(clause.answersIndexes.begin() + clause.correctIndex, 1, clause.omittedWordIndex);
}

void AppEngine::addQuestion(const string& clauseText, const string& translation) {
    Clause clause;
    clause.clause = clauseText;
    clause.translation = translation;
    clause.clause[0] = (char) tolower(clause.clause[0]);
    clause.omittedWordIndex = allAnswers.size();

    QString string = QString::fromStdString(clause.clause);
    string.remove(".");
    QStringList words = string.split(" ");
    string = words.at(rand()%words.size());
    std::string word = string.toStdString();
    allAnswers.emplace_back(std::move(word));
    word = string.toStdString();

    clauses.emplace_back(std::move(clause));
    writeToFile(clauses[clauses.size()-1]);
    refactorClause(clauses[clauses.size()-1]);
}

string AppEngine::getAnswer(size_t index) {
    return allAnswers[index];
}

Clause AppEngine::currentClause() const {
    if (currentIndex < clauses.size()) {
        return clauses[currentIndex];
    }
    return {};
}

bool AppEngine::isCorrect(size_t answer) {
    return clauses[currentIndex].correctIndex == answer;
}

void AppEngine::nextClause() {
    currentIndex++;
}

void AppEngine::addPoint() {
    correctAnswersCount++;
}

string AppEngine::getPointsString() {
    int percentage = correctAnswersCount*100/(currentIndex+1);
    char points[16];
    snprintf (points, 16, "%zu/%zu\t%d%%", correctAnswersCount, currentIndex+1, percentage);
    return string(points);
}

void AppEngine::writeToFile(Clause clause) { //assuming the clause is already added to engine
    fstream file;
    file.open(filepath, ios::app);
    clause.clause[0] = (char) tolower(clause.clause[0]);
    file << clause.clause << ";" << clause.translation << ";" << allAnswers[clause.omittedWordIndex] << ";" << clause.omittedWordIndex << endl;
    file.close();
}

