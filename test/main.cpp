/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "test.h"
#include <QCoreApplication>

int main (int argc, char **argv) {
    QCoreApplication app(argc, argv);

    Test test;
    app.exit(QTest::qExec(&test));

    return 0;
}

