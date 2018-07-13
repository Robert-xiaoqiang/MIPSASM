#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qscilexerverilog.h>

class HighLight : public QsciLexerCPP
{
    Q_OBJECT
public:
    HighLight();
    const char *keywords(int set) const;
};

#endif // HIGHLIGHT_H
