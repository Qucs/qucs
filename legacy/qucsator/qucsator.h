
#ifndef QUCSATOR_H
#define QUCSATOR_H
#include <QProcess>
#include "io_trace.h"

namespace{
class Qucsator;
}

// BUG must be anonymous
// see dialogs for how to do it properly
class QucsatorProcess : public QProcess {
	Q_OBJECT;
public:
	explicit QucsatorProcess(Qucsator* q) : QProcess(), _simulator(q) {
	}

	void start(QString const& a, QStringList const& b){itested();
		trace0("QucsatorProcess::start");
		disconnect(this, 0, 0, 0);
		connect(this, SIGNAL(stateChanged(QProcess::ProcessState)),
				SLOT(slotStateChanged(QProcess::ProcessState)));
		connect(this, SIGNAL(readyReadStandardError()), SLOT(stderr_()));
		connect(this, SIGNAL(readyReadStandardOutput()), SLOT(stdout_()));

		QProcess::start(a, b);
		trace0("QucsatorProcess started");
	}

protected slots:
	void slotStateChanged(QProcess::ProcessState newState); // { incomplete(); }
	void stdout_();
	void stderr_();

private:
	QProcess::ProcessState oldState;
	Qucsator* _simulator;
}; // QucsatorProcess

#endif
