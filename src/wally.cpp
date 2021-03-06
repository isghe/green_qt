#include "wally.h"

#include <QSet>

#define BIP39_WORDLIST_LEN 2048
extern "C" {
struct words;
int bip39_get_wordlist(const char *lang, struct words **output);
int bip39_get_word(const struct words *w, size_t index, char **output);
int wally_free_string(char *str);
}

namespace {

QStringList GetWordlist()
{
    QStringList wordlist;
    words* ws;
    bip39_get_wordlist(nullptr, &ws);
    wordlist.reserve(BIP39_WORDLIST_LEN);
    for (size_t i = 0; i < BIP39_WORDLIST_LEN; ++i) {
        char* w;
        bip39_get_word(ws, i, &w);
        wordlist.append(QString::fromLatin1(w));
        wally_free_string(w);
    }
    return wordlist;
}

QStringList g_wordlist{GetWordlist()};
QSet<QString> g_wordset{QSet<QString>::fromList(g_wordlist)};

} // namespace

Wally::Wally(QObject* parent) : QObject(parent) {}
Wally::~Wally() {}
QStringList Wally::wordlist() const { return g_wordlist; }

WordValidator::WordValidator(QObject *parent) : QValidator(parent) {}
WordValidator::State WordValidator::validate(QString &input, int &) const
{
    return g_wordset.contains(input) ? Acceptable : Intermediate;
}
