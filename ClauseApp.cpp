#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <random>
#include <ctime>
#include <set>
#include <QApplication>

#include "ClauseApp.h"

using namespace std;

/*
 * App Engine constructor:
 * - sets rand
 * - reads file
 * - refactors clauses
 * - shuffles clauses
 */
AppEngine::AppEngine(string path) : filepath(std::move(path)) {
    srand(time(nullptr));
    readFile();
    for (Clause& clause: clauses) refactorClause(clause);
    shuffle(begin(clauses), end(clauses), mt19937{random_device{}()});
}

/*
 * Function to read file in format "sentence;translation\n"
 * Assumes correct format, not much error handing
 * No empty line at the end!
 * First line is for file info
 */
void AppEngine::readFile() {
    ifstream file(filepath);
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    if(!file) {
        throw std::invalid_argument("Error while opening the file");
    } try {
        string fileInfo;
        getline(file, fileInfo);
        set<string> answersSet;
        while (file.good()) {
            Clause clause;
            string word;
            getline(file, clause.clause, ';');
            getline(file, clause.translation);
            stringstream ss(clause.clause);
            while (getline(ss, word, ' ')) {
                auto it = word.end()-1;
                if (*it == '.' || *it == '!' || *it == '?' || *it == ',' || *it == ':') word.pop_back();
                answersSet.insert(std::move(word));
            }
            clauses.emplace_back(std::move(clause));
        }
        allAnswers.assign(answersSet.begin(), answersSet.end());
    } catch (const std::exception&) {}
    file.close();
}

/*
 * Function to refactor a clause:
 * - parses a clause
 * - sets an omitted word and its index
 * - replaces it in the clause
 * - sets first letter to uppercase
 * - sets answers
 */
void AppEngine::refactorClause(Clause &clause) {
    vector<string> possibleAnswers;
    string word, omittedWord;
    stringstream ss (clause.clause);
    while (getline(ss, word, ' ')) {
        auto it = word.end()-1;
        if (*it == '.' || *it == '!' || *it == '?' || *it == ',' || *it == ':') word.pop_back();
        possibleAnswers.push_back(word);
    }

    omittedWord = possibleAnswers[rand() % possibleAnswers.size()];
    clause.omittedWordIndex = std::find(allAnswers.begin(), allAnswers.end(), omittedWord) - allAnswers.begin();

    size_t wordIndex = clause.clause.find(omittedWord);
    clause.clause.replace(wordIndex, omittedWord.length(), "___");
    clause.clause[0] = (char) toupper(clause.clause[0]);

    clause.answersIndexes = generateRandomAnswers(allAnswers.size(), ANSWERS_COUNT, clause.omittedWordIndex);
    clause.correctAnswerIndex = rand() % ANSWERS_COUNT;
    clause.answersIndexes.insert(clause.answersIndexes.begin() + clause.correctAnswerIndex, 1, clause.omittedWordIndex);
}


/*
 * To be redone/removed
 */
void AppEngine::addQuestion(const string& clauseText, const string& translation) {
//    Clause clause;
//    clause.clause = clauseText;
//    clause.translation = translation;
//    clause.clause[0] = (char) tolower(clause.clause[0]);
//    clause.omittedWordIndex = allAnswers.size();

//    QString string = QString::fromStdString(clause.clause);
//    string.remove(".");
//    QStringList words = string.split(" ");
//    string = words.at(rand()%words.size());
//    std::string word = string.toStdString();
//    allAnswers.emplace_back(std::move(word));
//    word = string.toStdString();

//    clauses.emplace_back(std::move(clause));
//    writeToFile(clauses[clauses.size()-1]);
//    refactorClause(clauses[clauses.size()-1]);
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
    return clauses[currentIndex].correctAnswerIndex == answer;
}

void AppEngine::reset()
{
    currentIndex = 0;
    correctAnswersCount = 0;
}

void AppEngine::nextClause() {
    currentIndex += 1;
}

void AppEngine::addPoint() {
    correctAnswersCount += 1;
}

string AppEngine::getPointsString() { //wykonuje sie dwa razy - przy Next/Enter i klikneciu odpowiedzi
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

/*
 * Outside function to pick random indexes
 */
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

