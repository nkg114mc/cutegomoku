
#include "gomokuboard.h"
#include "westernzobrist.h"
#include <QStringList>
#include <iostream>

namespace {

	const int MAX_GOMUKUBOARD_SIZE = 32;

}

namespace Chess {

GomokuBoard::GomokuBoard()
	: GomokuBoard(15, 15)
{
}

GomokuBoard::GomokuBoard(int sz)
	: GomokuBoard(sz, sz)
{
}

GomokuBoard::GomokuBoard(int wd, int ht) : Board()
{
	this->m_initialized = false;
	this->m_width = wd;
	this->m_height = ht;
	this->m_side = Side::Black;
	this->m_startingSide = Side::Black;

	setPieceType(Piece::NoPiece, QString(), QString());
	setPieceType(Stone, tr("stone"), "P");

	initialize();
}

GomokuBoard::~GomokuBoard()
{
}

bool GomokuBoard::isRandomVariant() const
{
	return false;
}

bool GomokuBoard::variantHasDrops() const
{
	return false;
}

bool GomokuBoard::variantHasWallSquares() const
{
	return false;
}

QList<Piece> GomokuBoard::reservePieceTypes() const
{
	return QList<Piece>();
}

Board::CoordinateSystem GomokuBoard::coordinateSystem() const
{
	return NormalCoordinates;
}

Side GomokuBoard::upperCaseSide() const
{
	return Side::White;
}

Piece GomokuBoard::pieceAt(const Square& square) const
{
	if (!isValidSquare(square))
		return Piece::WallPiece;
	return pieceAt(squareIndex(square));
}

void GomokuBoard::initialize()
{
	std::cout << "call GomokuBoard::initialize" << std::endl;
	if (m_initialized)
		return;

	m_initialized = true;
	m_squares.clear();

	int possibleMaxSize = MAX_GOMUKUBOARD_SIZE * MAX_GOMUKUBOARD_SIZE;
	for (int i = 0; i < possibleMaxSize; i++) {
		m_squares.append(Piece::WallPiece);
	}
		
	vInitialize();

	m_maxPieceSymbolLength = 1;
	for (const PieceData& pd: m_pieceData)
		if (pd.symbol.length() > m_maxPieceSymbolLength)
			m_maxPieceSymbolLength = pd.symbol.length();

	reset();

}

int GomokuBoard::maxPieceSymbolLength() const
{
	return m_maxPieceSymbolLength;
}

void GomokuBoard::setPieceType(int type,
			 const QString& name,
			 const QString& symbol,
			 unsigned movement,
			 const QString& gsymbol)
{
	if (type >= m_pieceData.size())
		m_pieceData.resize(type + 1);

	const QString& graphicalSymbol = gsymbol.isEmpty() ? symbol : gsymbol;

	PieceData data =
		{ name, symbol.toUpper(), movement, graphicalSymbol.toUpper() };
	m_pieceData[type] = data;
}

QString GomokuBoard::pieceSymbol(Piece piece) const
{
	int type = piece.type();
	if (type <= 0 || type >= m_pieceData.size())
		return QString();

	if (piece.side() == upperCaseSide())
		return m_pieceData[type].symbol;
	return m_pieceData[type].symbol.toLower();
}

Piece GomokuBoard::pieceFromSymbol(const QString& pieceSymbol) const
{
	if (pieceSymbol.isEmpty())
		return Piece::NoPiece;

	int code = Piece::NoPiece;
	QString symbol = pieceSymbol.toUpper();

	for (int i = 1; i < m_pieceData.size(); i++)
	{
		if (symbol == m_pieceData[i].symbol)
		{
			code = i;
			break;
		}
	}
	if (code == Piece::NoPiece)
		return code;

	Side side(upperCaseSide());
	if (pieceSymbol == symbol)
		return Piece(side, code);
	return Piece(side.opposite(), code);
}

QString GomokuBoard::pieceString(int pieceType) const
{
	if (pieceType <= 0 || pieceType >= m_pieceData.size())
		return QString();
	return m_pieceData[pieceType].name;
}

QString GomokuBoard::representation(Piece piece) const
{
	int type = piece.type();
	if (type <= 0 || type >= m_pieceData.size())
		return QString();

	if (piece.side() == upperCaseSide())
		return m_pieceData[type].representation;
	return m_pieceData[type].representation.toLower();
}

int GomokuBoard::reserveType(int pieceType) const
{
	return pieceType;
}

int GomokuBoard::reserveCount(Piece piece) const
{
	return 0;
}

void GomokuBoard::addToReserve(const Piece& piece, int count)
{
	return;
}

void GomokuBoard::removeFromReserve(const Piece& piece)
{
	return;
}

bool pieceHasMovement(int pieceType, unsigned movement) {
	return false;
}

// index to square
Square GomokuBoard::chessSquare(int index) const
{
	int arwidth = m_width;
	int file = (index % arwidth);
	int rank = (index / arwidth);
	return Square(file, rank);
}

// square to index
int GomokuBoard::squareIndex(const Square& square) const
{
	if (!isValidSquare(square)) {
		return -1;
	}

	int rank = square.rank();
	return (rank * m_width + square.file());
}

bool GomokuBoard::isValidSquare(const Chess::Square& square) const
{
	if (square.file() >= m_width || 
			square.rank() >= m_height) {
		std::cout << "InvalidSq: " << square.file() << " " << square.rank() << std::endl;
		return false;
	}
	return true;
}

QString GomokuBoard::squareString(int index) const
{
	return squareString(chessSquare(index));
}

QString GomokuBoard::squareString(const Square& square) const
{
	if (!square.isValid()) {
		return QString();
	}

	QString str;
	str += QChar('a' + square.file());
	str += QChar('a' + square.rank());
	return str;
}

Square GomokuBoard::chessSquare(const QString& str) const
{
	if (str.length() < 2)
		return Square();

	bool ok = false;
	int file = 0;
	int rank = 0;

	file = str.at(0).toLatin1() - 'a';
	rank = str.at(1).toLatin1() - 'a';

	if (file >= 0 && file < m_width &&
	    rank >= 0 && rank < m_height) {
		ok = true;
	}

	if (!ok)
		return Square();
	return Square(file, rank);
}

int GomokuBoard::squareIndex(const QString& str) const
{
	return squareIndex(chessSquare(str));
}

QString GomokuBoard::lanMoveString(const Move& move)
{
	QString str = "";
	int targetIndex = move.targetSquare();
	Square sq = chessSquare(targetIndex);
	str += QString("%1").arg(sq.file());
	str += ",";
	str += QString("%1").arg(sq.rank());
	return str;
}

QString GomokuBoard::moveString(const Move& move, MoveNotation notation)
{
	return lanMoveString(move);
}

Move GomokuBoard::moveFromLanString(const QString& istr)
{
	QString str(istr);
	int commaIdx = str.indexOf(',');

	bool fOk = false;
	bool rOk = false;
	QString fstr = str.left(commaIdx);
	QString rstr = str.mid(commaIdx + 1, -1);
	int file = fstr.toInt(&fOk);
	int rank = rstr.toInt(&rOk);

	std::cout << "[" << fstr.toStdString() << "] [" << rstr.toStdString() << "]" << std::endl;
	std::cout << file << " " << rank << std::endl;

	if (fOk && rOk) {
		Square sq = Square(file, rank);
		if (isValidSquare(sq)) {
			std::cout << squareIndex(sq) << std::endl;
			return Move(squareIndex(sq), squareIndex(sq), Piece::WallPiece);
		}
	}

	// invalid move
	return Move();
}

Move GomokuBoard::moveFromString(const QString& str)
{
	Move move = moveFromLanString(str);
	return move;
}

Move GomokuBoard::moveFromGenericMove(const GenericMove& move) const
{
	int source = squareIndex(move.sourceSquare());
	int target = squareIndex(move.targetSquare());

	return Move(source, target, move.promotion());
}

GenericMove GomokuBoard::genericMove(const Move& move) const
{
	int source = move.sourceSquare();
	int target = move.targetSquare();

	return GenericMove(chessSquare(source), chessSquare(target), move.promotion());
}

QStringList GomokuBoard::pieceList(Side side) const
{
	QStringList list;
	for (int file = 0; file < height(); file++) {
		for (int rank = 0; rank < width(); rank++) {
			Square sq = Chess::Square(file, rank);
			const Piece piece = pieceAt(sq);
			if (piece.side() != side)
				continue;

			QString s = pieceSymbol(piece).toUpper();
			s.append(squareString(sq));
			list.append(s);
		}
	}
	return list;
}

QString GomokuBoard::fenString(FenNotation notation) const
{
	QString fen;

	// Squares
	int i = (m_width + 2) * 2;
	for (int y = 0; y < m_height; y++)
	{
		int nempty = 0;
		i++;
		if (y > 0)
			fen += '/';
		for (int x = 0; x < m_width; x++)
		{
			Piece pc = m_squares[i];

			if (pc.isEmpty())
				nempty++;

			// Add the number of empty successive squares
			// to the FEN string.
			if (nempty > 0
			&&  (!pc.isEmpty() || x == m_width - 1))
			{
				fen += QString::number(nempty);
				nempty = 0;
			}

			if (pc.isValid())
				fen += pieceSymbol(pc);
			else if (pc.isWall())
				fen += "*";

			i++;
		}
		i++;
	}

	// Side to move
	fen += QString(" %1 ").arg(m_side.symbol());

	return fen + vFenString(notation);
}

bool GomokuBoard::setFenString(const QString& fen)
{/*
	QStringList strList = fen.split(' ');
	if (strList.isEmpty())
		return false;

	QStringList::iterator token = strList.begin();
	if (token->length() < m_height * 2)
		return false;

	initialize();

	int square = 0;
	int rankEndSquare = 0;	// last square of the previous rank
	int boardSize = m_width * m_height;
	int k = (m_width + 2) * 2 + 1;

	for (int i = 0; i < m_squares.size(); i++)
		m_squares[i] = Piece::WallPiece;
	m_key = 0;

	// Get the board contents (squares)
	int handPieceIndex = -1;
	int maxsymlen = maxPieceSymbolLength();
	QString pieceStr;
	for (int i = 0; i < token->length(); i++)
	{
		QChar c = token->at(i);

		// Move to the next rank
		if (c == '/')
		{
			if (!pieceStr.isEmpty())
				return false;

			// Reject the FEN string if the rank didn't
			// have exactly 'm_width' squares.
			if (square - rankEndSquare != m_width)
				return false;
			rankEndSquare = square;
			k += 2;
			continue;
		}
		// Start of hand pieces
		if (c == '[')
		{
			if (!variantHasDrops())
				return false;
			handPieceIndex = i + 1;
			break;
		}
		// Wall square
		if (c == '*' && variantHasWallSquares())
		{
			if (!pieceStr.isEmpty())
				return false;
			square++;
			k++;
			continue;
		}
		// Add empty squares
		if (c.isDigit())
		{
			if (!pieceStr.isEmpty())
				return false;

			int j;
			int nempty;
			if (i < (token->length() - 1) && token->at(i + 1).isDigit())
			{
				nempty = token->midRef(i, 2).toInt();
				i++;
			}
			else
				nempty = c.digitValue();

			if (nempty > m_width || square + nempty > boardSize)
				return false;
			for (j = 0; j < nempty; j++)
			{
				square++;
				setSquare(k++, Piece::NoPiece);
			}
			continue;
		}

		if (square >= boardSize)
			return false;

		// read ahead for multi-character symbols
		for (int l = qMin(maxsymlen, token->length() - i); l > 0; l--)
		{
			pieceStr = token->mid(i, l);
			Piece piece = pieceFromSymbol(pieceStr);
			if (piece.isValid())
			{
				setSquare(k++, piece);
				i += l - 1;
				pieceStr.clear();
				square++;
				break;
			}
		}
		// left over: unknown symbols
		if (!pieceStr.isEmpty())
			return false;
	}

	// The board must have exactly 'boardSize' squares and each rank
	// must have exactly 'm_width' squares.
	if (square != boardSize || square - rankEndSquare != m_width)
		return false;

	// Hand pieces
	m_reserve[Side::White].clear();
	m_reserve[Side::Black].clear();
	if (handPieceIndex != -1)
	{
		for (int i = handPieceIndex; i < token->length(); i++)
		{
			QChar c = token->at(i);
			if (c == ']')
				break;
			if (c == '-' && i == handPieceIndex)
				continue;

			int count = 1;
			if (c.isDigit())
			{
				count = c.digitValue();
				if (count <= 0)
					return false;
				++i;
				if (i >= token->length() - 1)
					return false;
				c = token->at(i);
			}
			Piece tmp = pieceFromSymbol(c);
			if (!tmp.isValid())
				return false;
			addToReserve(tmp, count);
		}
	}

	// Side to move
	if (++token == strList.end())
		return false;
	m_side = Side(*token);
	m_startingSide = m_side;
	if (m_side.isNull())
		return false;

	m_moveHistory.clear();
	m_startingFen = fen;

	// Let subclasses handle the rest of the FEN string
	if (token != strList.end())
		++token;
	strList.erase(strList.begin(), token);
	if (!vSetFenString(strList))
		return false;

	if (m_side == Side::White)
		xorKey(m_zobrist->side());

	if (!isLegalPosition())
		return false;
*/
	return true;
}

void GomokuBoard::reset()
{
	setFenString(defaultFenString());

	for (int i = 0; i < m_squares.size(); i++) {
		Piece &pc = m_squares[i];
		pc = Piece::NoPiece;
	}

	m_side = Side::Black;
	m_moveHistory.clear();
}

void GomokuBoard::makeMove(const Move& move, BoardTransition* transition)
{
	Q_ASSERT(!m_side.isNull());
	Q_ASSERT(!move.isNull());

	MoveData md = { move, m_key };

	vMakeMove(move, transition);

	m_side = m_side.opposite();
	m_moveHistory << md;

	showBoard();
	std::cout << "MoveHistLength: " << m_moveHistory.size() << std::endl;

}

void GomokuBoard::undoMove()
{
	Q_ASSERT(!m_moveHistory.isEmpty());
	Q_ASSERT(!m_side.isNull());

	m_side = m_side.opposite();
	vUndoMove(m_moveHistory.last().move);

	m_key = m_moveHistory.last().key;
	m_moveHistory.pop_back();
}

void GomokuBoard::generateMoves(QVarLengthArray<Move>& moves, int pieceType) const
{
	Q_ASSERT(!m_side.isNull());

	for (int i = 0; i < m_squares.size(); i++) {
		Piece pc = m_squares[i];
		if (pc == Piece::NoPiece) {
			moves.append(Move(i, i, Piece::WallPiece));
		}
	}

}

void GomokuBoard::generateDropMoves(QVarLengthArray<Move>& moves, int pieceType) const
{
	return;
}

void GomokuBoard::generateHoppingMoves(int sourceSquare,
				 const QVarLengthArray<int>& offsets,
				 QVarLengthArray<Move>& moves) const
{
	return;
}

void GomokuBoard::generateSlidingMoves(int sourceSquare,
				 const QVarLengthArray<int>& offsets,
				 QVarLengthArray<Move>& moves) const
{
	return;
}

bool GomokuBoard::moveExists(const Move& move) const
{
	Q_ASSERT(!move.isNull());

	QVarLengthArray<Move> moves;
	generateMoves(moves);

	for (int i = 0; i < moves.size(); i++) {
		if (moves[i] == move) {
			return true;
		}
	}
	return false;
}

int GomokuBoard::captureType(const Move& move) const
{
	Q_ASSERT(!move.isNull());
	return Piece::NoPiece;
}

bool GomokuBoard::vIsLegalMove(const Move& move)
{
	int targetSq = move.targetSquare();
	if (pieceAt(chessSquare(targetSq)) != Piece::NoPiece) { // only possible to place stone on empty square
		return false;
	}
/*
	makeMove(move);
	bool isLegal = isLegalPosition();
	undoMove();
*/
	bool isLegal = true;
	return isLegal;
}

bool GomokuBoard::isLegalMove(const Move& move)
{
	return vIsLegalMove(move);
}

int GomokuBoard::repeatCount() const
{
	return 0;
}

int GomokuBoard::reversibleMoveCount() const
{
	return -1;
}

bool GomokuBoard::isRepetition(const Chess::Move& move)
{
	Q_ASSERT(!move.isNull());

	makeMove(move);
	bool isRepeat = (repeatCount() > 0);
	undoMove();

	return isRepeat;
}

bool GomokuBoard::canMove()
{
	QVarLengthArray<Move> moves;
	generateMoves(moves);

	for (int i = 0; i < moves.size(); i++)
	{
		if (vIsLegalMove(moves[i]))
			return true;
	}

	return false;
}

QVector<Move> GomokuBoard::legalMoves()
{
	QVarLengthArray<Move> moves;
	QVector<Move> legalMoves;

	generateMoves(moves);
	legalMoves.reserve(moves.size());

	for (int i = moves.size() - 1; i >= 0; i--)
	{
		if (vIsLegalMove(moves[i]))
			legalMoves << moves[i];
	}

	return legalMoves;
}

Result GomokuBoard::tablebaseResult(unsigned int* dtm) const
{
	Q_UNUSED(dtm);
	return Result();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Board* GomokuBoard::copy() const {
	return new GomokuBoard(*this);
}

QString GomokuBoard::variant() const {
  return "gomoku";
}

QString GomokuBoard::defaultFenString() const {
	return "15/15/15/15/15/15/15/15/15/15/15/15/15/15/15 b 0 1";
}

Result GomokuBoard::result() {

	QString str;

	// Win: Five-connection
	QVector<FiveConnectionInfo> fiveConns = findFiveConnections();
	//Q_ASSERT(fiveConns.size() <= 1); // commented out in case there is long connections (connections of more than 6)

	if (fiveConns.size() > 0) {
		//Q_ASSERT(fiveConns.size() == 1);

		GomokuBoard::FiveConnectionInfo winFive = fiveConns[0];
		Side winner = winFive.color;
		str = tr("%1 wins by five connections").arg(winner.toString());
		return Result(Result::Win, winner, str);
	}

	// Draw: Board is full
	int maxMoves = width() * height();
	if (plyCount() >= maxMoves) {
		return Result(Result::Draw, Side::NoSide, "Wokelipangdiu");
	}

	return Result();
}

QVector<GomokuBoard::FiveConnectionInfo> GomokuBoard::findFiveConnections() {

	QVector<FiveConnectionInfo> fives;
	int foffset = 0, roffset = 0;
	
	foffset = 1;
	roffset = 0;
	for (int i = 0; i < (m_width - 4); i++) {
		for (int j = 0; j < m_height; j++) {
			Square sq = Square(i, j);
			Piece pc = pieceAt(sq);
			if (pc != Piece::NoPiece) {
				bool isFive = checkFiveConnection(sq, foffset, roffset);
				if (isFive) {
					GomokuBoard::FiveConnectionInfo fInfo = { 
						sq, pc.side(), foffset, roffset 
					};
					fives.push_back(fInfo);
				}
			}
		}
	}

	foffset = 0;
	roffset = 1;
	for (int i = 0; i < m_width; i++) {
		for (int j = 0; j < (m_height - 4); j++) {
			Square sq = Square(i, j);
			Piece pc = pieceAt(sq);
			if (pc != Piece::NoPiece) {
				bool isFive = checkFiveConnection(sq, foffset, roffset);
				if (isFive) {
					GomokuBoard::FiveConnectionInfo fInfo = { 
						sq, pc.side(), foffset, roffset 
					};
					fives.push_back(fInfo);
				}
			}
		}
	}

	foffset = 1;
	roffset = 1;
	for (int i = 0; i < (m_width - 4); i++) {
		for (int j = 0; j < (m_height - 4); j++) {
			Square sq = Square(i, j);
			Piece pc = pieceAt(sq);
			if (pc != Piece::NoPiece) {
				bool isFive = checkFiveConnection(sq, foffset, roffset);
				if (isFive) {
					GomokuBoard::FiveConnectionInfo fInfo = { 
						sq, pc.side(), foffset, roffset 
					};
					fives.push_back(fInfo);
				}
			}
		}
	}

	foffset = 1;
	roffset = -1;
	for (int i = 0; i < (m_width - 4); i++) {
		for (int j = 4; j < m_height; j++) {
			Square sq = Square(i, j);
			Piece pc = pieceAt(sq);
			if (pc != Piece::NoPiece) {
				bool isFive = checkFiveConnection(sq, foffset, roffset);
				if (isFive) {
					GomokuBoard::FiveConnectionInfo fInfo = { 
						sq, pc.side(), foffset, roffset 
					};
					fives.push_back(fInfo);
				}
			}
		}
	}
	
	return fives;
}

bool GomokuBoard::checkFiveConnection(Square &sq, int fileOffset, int rankOffset) {

	const Piece pc = pieceAt(sq);
	if (pc == Piece::NoPiece) {
		return false;
	}

	for (int k = 1; k <= 4; k++) {
		int newFile = sq.file() + fileOffset * k;
		int newRank = sq.rank() + rankOffset * k;
		Square newSq = Square(newFile, newRank);
		if (pieceAt(newSq) != pc) {
			return false;
		}
	}

	return true;
}

void GomokuBoard::vInitialize() {

}

void GomokuBoard::vMakeMove(const Move& move, BoardTransition* transition) {

	Side side = sideToMove();
	int source = move.sourceSquare();
	int target = move.targetSquare();
	Piece capture = pieceAt(target);

	Q_ASSERT(capture == Piece::NoPiece);

	Piece newpc = Piece(side, Stone);
	setSquare(target, newpc);

	if (transition != nullptr) {
		transition->addMove(chessSquare(source), chessSquare(target));
	}

}

void GomokuBoard::vUndoMove(const Move& move) {
	
	int source = move.sourceSquare();
	int target = move.targetSquare();
	Piece capture = pieceAt(target);

	Q_ASSERT(capture != Piece::NoPiece);

	setSquare(target, Piece::NoPiece);

}

QString GomokuBoard::sanMoveString(const Move& move) {
	return lanMoveString(move);
}

Move GomokuBoard::moveFromSanString(const QString& str) {
	return moveFromLanString(str);
}

QString GomokuBoard::vFenString(FenNotation notation) const {
	return "";
}

bool GomokuBoard::vSetFenString(const QStringList& fen) {
	return true;
}

void GomokuBoard::generateMovesForPiece(QVarLengthArray<Move>& moves,
						   													int pieceType,
						   													int square) const {
	return;
}

bool GomokuBoard::isLegalPosition() {
	return true;
}

Side GomokuBoard::sideToMove() const
{
	return m_side;
}

Side GomokuBoard::startingSide() const
{
	return m_startingSide;
}

QString GomokuBoard::startingFenString() const
{
	return m_startingFen;
}

quint64 GomokuBoard::key() const
{
	return 0;
}

void GomokuBoard::xorKey(quint64 key)
{
	m_key ^= key;
}

Piece GomokuBoard::pieceAt(int square) const
{
	return m_squares[square];
}

void GomokuBoard::setSquare(int square, Piece piece)
{
	Piece& old = m_squares[square];
	old = piece;
}

int GomokuBoard::plyCount() const
{
	return m_moveHistory.size();
}

const Move& GomokuBoard::lastMove() const
{
	return m_moveHistory.last().move;
}

int GomokuBoard::width() const {
  return m_width;
}

int GomokuBoard::height() const {
  return m_height;
}

void GomokuBoard::setWidth(int wd) {
	Q_ASSERT(wd > 0 && wd < 32);
	m_width = wd;
}

void GomokuBoard::setHeight(int ht) {
	Q_ASSERT(ht > 0 && ht < 32);
	m_height = ht;
}

void GomokuBoard::setSize(int sz) {
	Q_ASSERT(sz > 0 && sz < 32);
	setWidth(sz);
	setHeight(sz);
}

void GomokuBoard::showBoard() {

	int w = width();
	int h = height();

	std::cout << "========================================" << std::endl;
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			std::string cell = ". ";
			Piece pc = pieceAt(Square(i, j));
			if (pc == Piece::NoPiece) {

			} else if (pc.side() == Side::White) {
				cell = "O ";
			} else if (pc.side() == Side::Black) {
				cell = "X ";
			}
			//std::cout << pc.type() << " ";
			std::cout << cell; 
		}
		std::cout << std::endl;
	}
	std::cout << "SideToMove: " << sideToMove() << std::endl;
	std::cout << "Ply: " << m_moveHistory.size() << std::endl;
	std::cout << "Moves: ";
	for (int k = 0; k < m_moveHistory.size(); k++) {
		MoveNotation nota;
		QString mstr = moveString(m_moveHistory[k].move, nota);
		std::cout << mstr.toStdString() << " ";
	}
	std::cout << std::endl;
	std::cout << "========================================" << std::endl;

}

} // namespace Chess
