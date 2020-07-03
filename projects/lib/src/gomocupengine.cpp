/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

    Cute Chess is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cute Chess is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Cute Chess.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gomocupengine.h"

#include <QtGlobal>
#include <QString>
#include <QStringList>
#include <QTimer>

#include <climits>
#include <iostream>

#include "timecontrol.h"
//#include "enginebuttonoption.h"
//#include "enginecheckoption.h"
//#include "enginecombooption.h"
//#include "enginespinoption.h"
//#include "enginetextoption.h"

namespace {

const int s_infiniteSec = 86400;

} // anonymous namespace

GomocupEngine::GomocupEngine(QObject* parent)
	: ChessEngine(parent),
	  m_forceMode(false),
	  m_drawOnNextMove(false),
	  m_ftName(false),
	  m_ftPing(false),
	  m_ftSetboard(false),
	  m_ftTime(true),
	  m_ftUsermove(false),
	  m_ftReuse(true),
	  m_ftNps(false),
	  m_gotResult(false),
	  m_lastPing(0),
	  m_notation(Chess::Board::LongAlgebraic),
	  m_initTimer(new QTimer(this))
{
	m_initTimer->setSingleShot(true);
	m_initTimer->setInterval(8000);
	connect(m_initTimer, SIGNAL(timeout()), this, SLOT(initialize()));

	addVariant("gomoku");
	setName("GomocupEngine");
}

void GomocupEngine::startProtocol()
{
	// Tell the engine to turn on xboard mode
	m_initTimer->start();
}

void GomocupEngine::initialize()
{
	if (state() == Starting)
	{
		onProtocolStart();
		emit ready();
	}
}

void GomocupEngine::startGame()
{
	m_gotResult = false;
	m_forceMode = false;
	m_nextMove = Chess::Move();

	board()->reset();

	write(QString("ABOUT"));

	int boardSize = board()->width();
	write(QString("START %1").arg(boardSize));
	/*
	m_drawOnNextMove = false;
	m_gotResult = false;
	m_forceMode = false;
	m_nextMove = Chess::Move();
	write("new");
	
	if (board()->variant() != "standard")
		write("variant " + variantToXboard(board()->variant()));
	
	setForceMode(true);

	if (board()->isRandomVariant()
	||  board()->fenString() != board()->defaultFenString())
	{
		if (board()->sideToMove() == Chess::Side::Black)
		{
			// Use a dummy move to force the engine to play black
			write("b2b3");
		}
		if (m_ftSetboard)
			write("setboard " + board()->fenString());
		else
		{
			qWarning("%s does not support the setboard command, using the edit command now",
				 qUtf8Printable(name()));
			write("edit");
			write("#"); // clear board on engine
			const QStringList& whitePieces = board()->pieceList(Chess::Side::White);
			for (const auto& s: whitePieces)
				write(s); // set a piece
			write("c");
			const QStringList& blackPieces = board()->pieceList(Chess::Side::Black);
			for (const auto& s: blackPieces)
				write(s); // set a piece
			write("."); // finished
		}
	}
	
	// Send the time controls
	const TimeControl* myTc = timeControl();
	if (myTc->isInfinite())
	{
		if (myTc->plyLimit() == 0 && myTc->nodeLimit() == 0)
			write(QString("st %1").arg(s_infiniteSec));
	}
	else if (myTc->timePerMove() > 0)
		write(QString("st %1").arg(myTc->timePerMove() / 1000));
	else
		write(QString("level %1 %2 %3")
		      .arg(myTc->movesPerTc())
		      .arg(msToXboardTime(myTc->timePerTc()))
		      .arg(double(myTc->timeIncrement()) / 1000));

	if (myTc->plyLimit() > 0)
		write(QString("sd %1").arg(myTc->plyLimit()));
	if (myTc->nodeLimit() > 0)
	{
		if (m_ftNps)
			write(QString("st 1\nnps %1").arg(myTc->nodeLimit()));
		else
			qWarning("%s doesn't support the nps command",
				 qUtf8Printable(name()));
	}

	// Show thinking
	write("post");
	// Pondering
	if (pondering())
		write("hard");
	else
		write("easy");
	
	// Tell the opponent's type and name to the engine
	if (m_ftName)
	{
		if (!opponent()->isHuman())
			write("computer");
		write("name " + opponent()->name());
	}*/
}

bool GomocupEngine::restartsBetweenGames() const
{
	if (restartMode() == EngineConfiguration::RestartAuto)
		return !m_ftReuse;
	return ChessEngine::restartsBetweenGames();
}

void GomocupEngine::endGame(const Chess::Result& result)
{
	State s = state();
	if (s != Thinking && s != Observing)
		return;

	if (s != Thinking)
		m_gotResult = true;

	stopThinking();
	//setForceMode(true);
	//write("result " + result.toVerboseString());

	ChessEngine::endGame(result);

	// If the engine can't be pinged, we may have to wait for
	// for a move or a result, or an error, or whatever. We
	// would like to extend our middle fingers to every engine
	// developer who fails to support the ping command.
	if (!m_ftPing && m_gotResult)
		finishGame();
}

void GomocupEngine::finishGame()
{
	if (!m_ftPing && state() == FinishingGame)
	{
		// Give the engine enough time to send all pending
		// output relating to the current game
		m_gotResult = true;
		QTimer::singleShot(200, this, SLOT(pong()));
	}
}

void GomocupEngine::sendTimeLeft()
{/*
	if (!m_ftTime)
		return;
	
	if (timeControl()->isInfinite())
	{
		write(QString("time %1").arg(s_infiniteSec));
		return;
	}

	int csLeft = timeControl()->timeLeft() / 10;
	int ocsLeft = opponent()->timeControl()->timeLeft() / 10;

	if (csLeft < 0)
		csLeft = 0;
	if (ocsLeft < 0)
		ocsLeft = 0;

	write(QString("time %1\notim %2").arg(csLeft).arg(ocsLeft));*/

	if (!m_ftTime) {
		return;
	}

	int csLeft = timeControl()->timeLeft();
	int ocsLeft = opponent()->timeControl()->timeLeft();

	if (csLeft < 0)
		csLeft = 0;
	if (ocsLeft < 0)
		ocsLeft = 0;

	write(QString("INFO time_left %1").arg(csLeft));
}

void GomocupEngine::sendTurnInfo() // other infos except time left
{

}


void GomocupEngine::setForceMode(bool enable)
{
	if (enable && !m_forceMode)
	{
		m_forceMode = true;

		// If there's a move pending, and we didn't get the
		// 'go' command, we'll send the move in force mode.
		if (!m_nextMove.isNull())
			makeMove(m_nextMove);
	}
	m_forceMode = enable;
}

QString GomocupEngine::moveString(const Chess::Move& move)
{
	Q_ASSERT(!move.isNull());
	return board()->moveString(move, m_notation);
}

void GomocupEngine::makeMove(const Chess::Move& move)
{
	Q_ASSERT(!move.isNull());

	QString moveString;
	if (move == m_nextMove)
		moveString = m_nextMoveString;
	else
		moveString = this->moveString(move);

	// If we're not in force mode, we'll have to wait for the
	// 'go' command until the move can be sent to the engine.
	if (!m_forceMode)
	{
		if (m_nextMove.isNull())
		{
			m_nextMove = move;
			m_nextMoveString = moveString;
			return;
		}
		else if (move != m_nextMove) {
			setForceMode(true);
		}
	}

/*
	if (m_ftUsermove)
		write("usermove " + moveString);
	else
		write(moveString);
*/
	write("TURN " + moveString);
	m_nextMove = Chess::Move();
}

void GomocupEngine::startThinking()
{
	setForceMode(false);
	sendTimeLeft();

	if (m_nextMove.isNull()) {
		write("BEGIN");
	} else {
		makeMove(m_nextMove);
	}
}

void GomocupEngine::onTimeout()
{
	if (m_drawOnNextMove)
	{
		Q_ASSERT(state() == Thinking);

		m_drawOnNextMove = false;
		claimResult(Chess::Result(Chess::Result::Draw));
	}
	else
		ChessEngine::onTimeout();
}

void GomocupEngine::sendStop()
{
	write("STOP");
}

QString GomocupEngine::protocol() const
{
	return "gomocup";
}

bool GomocupEngine::sendPing()
{
	if (!m_ftPing)
	{
		if (state() == FinishingGame)
			return true;
		return false;
	}

	// There is no specific command in Gomocup to support ping, so we use ABOUT
	// It looks a little wired though
	write(QString("ABOUT")); 
	return true;
}

void GomocupEngine::sendQuit()
{
	write("END");
}
/*
EngineOption* GomocupEngine::parseOption(const QString& line)
{
	return nullptr;
}

void GomocupEngine::setFeature(const QString& name, const QString& val)
{
}
*/

void GomocupEngine::setGomokuBoard() {

	setForceMode(true);
	write("BOARD");

	QVector<Chess::Move> moves = board()->getHistoricalMoves();
	for (int i = 0; i < moves.size(); i++) {
		
		Chess::Move mv = moves[i];
		Chess::Square sq = board()->chessSquarePublic(mv.targetSquare());
		Chess::Piece pc = board()->pieceAt(sq);
		
		int pieceInt = 0;
		if (pc.side() == Chess::Side::White) {
			pieceInt = 2;
		} else if (pc.side() == Chess::Side::Black) {
			pieceInt = 1;
		}

		// set move
		write(QString("%1,%2,%3").arg(sq.file()).arg(sq.rank()).arg(pieceInt));
	}

	write("DONE");
	setForceMode(false);

}

// shift assumed mate scores further out
int GomocupEngine::adaptScore(int score) const
{
	constexpr static int newCECPMateScore = 100000;
	int absScore = qAbs<int>(score);

	// convert new CECP mate scores to old ones
	if (absScore > newCECPMateScore
	&&  absScore < newCECPMateScore + 100)
	{
		absScore = 2 * newCECPMateScore - 2 * absScore + m_eval.MATE_SCORE;
		if (score >= absScore)
			absScore++;
	}

	// map assumed mate scores onto equivalents w/ higher absolute values
	int distance = 1000 - (absScore % 1000);
	if (absScore > 9900 &&  distance < 100)
		score = (score > 0) ? m_eval.MATE_SCORE - distance
				    : -m_eval.MATE_SCORE + distance;

	return score;
}

void GomocupEngine::parseLine(const QString& line)
{
	const QStringRef command(firstToken(line));
	if (command.isEmpty())
		return;

	if (command.at(0).isDigit() && 
	    command.contains(","))	// engine response move
	{
		if (state() != Thinking)
		{
			if (state() == FinishingGame)
				finishGame();
			else
				qWarning("Unexpected move from %s",
					 qUtf8Printable(name()));
			return;
		}

		const QString& movestr = *command.string();
		Chess::Move move = board()->moveFromString(movestr);
		if (move.isNull())
		{
			std::cout << "Illegal move? " << movestr.toStdString() << std::endl;
			forfeit(Chess::Result::IllegalMove, movestr);
			return;
		}

		emitMove(move);
	}
	else if (command.contains("=")) // response to ABOUT
	{
		QStringRef ref(command);
		
		std::cout << "GetAbout[";
		while (!ref.isNull()) {
			std::cout << ref.toString().toStdString();
			ref = nextToken(ref);
		}
		std::cout << "]" << std::endl;

		pong();
	}
	else if (command == "OK")
	{
		pong();
	}
	else if (command.startsWith("MESSAGE"))
	{
		QStringRef ref(command);
		
		std::cout << "GetMessage[";
		while (!ref.isNull()) {
			std::cout << ref.toString().toStdString();
			ref = nextToken(ref);
		}
		std::cout << "]" << std::endl;
	}
	else if (command.startsWith("DEBUG"))
	{
		QStringRef ref(command);

		std::cout << "GetDebug[";
		while (!ref.isNull()) {
			std::cout << ref.toString().toStdString();
			ref = nextToken(ref);
		}
		std::cout << "]" << std::endl;
	}
	else if (command == "ERROR")
	{
		QStringRef ref(command);
		
		std::cout << "GetError[";
		while (!ref.isNull()) {
			std::cout << ref.toString().toStdString();
			ref = nextToken(ref);
		}
		std::cout << "]" << std::endl;
	}
	else if (command == "UNKNOWN")
	{
		QStringRef ref(command);
		
		std::cout << "GetUnknown[";
		while (!ref.isNull()) {
			std::cout << ref.toString().toStdString();
			ref = nextToken(ref);
		}
		std::cout << "]" << std::endl;
	}

}

void GomocupEngine::sendOption(const QString& name, const QVariant& value)
{
	// TODO?
}
