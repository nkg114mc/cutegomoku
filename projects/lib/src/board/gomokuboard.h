#ifndef GOMOKUBOARD_H
#define GOMOKUBOARD_H

#include <QString>
#include <QVector>
#include <QVarLengthArray>
#include <QSharedPointer>
#include <QDebug>
#include <QCoreApplication>
#include "square.h"
#include "piece.h"
#include "move.h"
#include "genericmove.h"
#include "zobrist.h"
#include "result.h"
#include "board.h"
#include "boardtransition.h"
class QStringList;


namespace Chess {


class LIB_EXPORT GomokuBoard : public Board
{

	public:

		enum GomokuPieceType
		{
			Stone = 1
		};

		struct FiveConnectionInfo
		{
			Square startSq;
			Side color;
			int fileOffset;
			int rankOffset;
		};

		GomokuBoard();
		GomokuBoard(int sz);
		GomokuBoard(int wd, int ht);
		~GomokuBoard();
		
		/*! Creates and returns a deep copy of this board. */
		Board* copy() const;

		/*! Returns the name of the chess variant. */
		QString variant() const;
		/*!
		 * Returns true if the variant uses randomized starting positions.
		 * The default value is false.
		 */
		bool isRandomVariant() const;
		/*!
		 * Returns true if the variant allows piece drops.
		 * The default value is false.
		 *
		 * \sa CrazyhouseBoard
		 */
		bool variantHasDrops() const;
		/*!
		 * Returns true if the board accepts wall squares, else false.
		 * The default value is false.
		 */
		bool variantHasWallSquares() const;
		/*!
		 * Returns a list of piece types that can be in the reserve,
		 * ie. captured pieces that can be dropped on the board.
		 *
		 * The default implementation returns an empty list.
		 */
		QList<Piece> reservePieceTypes() const;
		/*! Returns the coordinate system used in the variant. */
		CoordinateSystem coordinateSystem() const;
		/*! Returns the width of the board in squares. */
		int width() const;
		int height() const;
		/*! Returns the variant's default starting FEN string. */
		QString defaultFenString() const;
		/*! Returns the zobrist key for the current position. */
		quint64 key() const;
		/*!
		 * Initializes the board.
		 * This function must be called before a game can be started
		 * on the board.
		 */
		void initialize();
		
		/*! Returns true if \a square is on the board. */
		bool isValidSquare(const Square& square) const;

		/*!
		 * Returns list of the pieces of \a side in current position.
		 */
		QStringList pieceList(Side side) const;
		/*!
		 * Returns the FEN string of the current board position in
		 * X-Fen or Shredder FEN notation
		 */
		QString fenString(FenNotation notation = XFen) const;
		/*!
		 * Returns the FEN string of the starting position.
		 * \note This is not always the same as \a defaultFenString().
		 */
		QString startingFenString() const;
		/*!
		 * Sets the board position according to a FEN string.
		 *
		 * The \a fen string can be in standard FEN, X-FEN or
		 * Shredder FEN notation.
		 *
		 * Returns true if successful; otherwise returns false.
		 */
		bool setFenString(const QString& fen);
		/*!
		 * Sets the board position to the default starting position
		 * of the chess variant.
		 */
		void reset();

		/*!
		 * Returns the side whose pieces are denoted by uppercase letters.
		 * The default value is White.
		 */
		Side upperCaseSide() const;
		/*! Returns the side to move. */
		Side sideToMove() const;
		/*! Returns the side that made/makes the first move. */
		Side startingSide() const;
		/*! Returns the piece at \a square. */
		Piece pieceAt(const Square& square) const;
		/*! Returns the number of halfmoves (plies) played. */
		int plyCount() const;
		/*!
		 * Returns the number of times the current position was
		 * reached previously in the game.
		 */
		int repeatCount() const;
		/*!
		 * Returns the number of consecutive reversible moves made.
		 *
		 * The default implementation always returns -1.
		 */
		int reversibleMoveCount() const;
		/*!
		 * Returns the number of reserve pieces of type \a piece.
		 *
		 * On variants that don't have piece drops this function
		 * always returns 0.
		 */
		int reserveCount(Piece piece) const;
		/*! Converts \a piece into a piece symbol. */
		QString pieceSymbol(Piece piece) const;
		/*! Converts \a pieceSymbol into a Piece object. */
		Piece pieceFromSymbol(const QString& pieceSymbol) const;
		/*! Returns the internationalized name of \a pieceType. */
		QString pieceString(int pieceType) const;
		/*! Returns symbol for graphical representation of \a piece. */
		QString representation(Piece piece) const;

		/*!
		 * Makes a chess move on the board.
		 *
		 * All details about piece movement, promotions, captures,
		 * drops, etc. are stored in \a transition. These details are
		 * useful mainly for updating a graphical representation of
		 * the board.
		 */
		void makeMove(const Move& move, BoardTransition* transition = nullptr);
		/*! Reverses the last move. */
		void undoMove();

		/*!
		 * Converts a Move into a string.
		 *
		 * \note The board must be in a position where \a move can be made.
		 * \sa moveFromString()
		 */
		QString moveString(const Move& move, MoveNotation notation);
		/*!
		 * Converts a move string into a Move.
		 *
		 * \note Returns a null move if \a move is illegal.
		 * \note Notation is automatically detected, and can be anything
		 * that's specified in MoveNotation.
		 * \sa moveString()
		 */
		Move moveFromString(const QString& str);
		/*!
		 * Converts a GenericMove into a Move.
		 *
		 * \note The board must be in a position where \a move can be made.
		 * \sa genericMove()
		 */
		Move moveFromGenericMove(const GenericMove& move) const;
		/*!
		 * Converts a Move into a GenericMove.
		 *
		 * \note The board must be in a position where \a move can be made.
		 * \sa moveFromGenericMove()
		 */
		GenericMove genericMove(const Move& move) const;

		/*! Returns true if \a move is legal in the current position. */
		bool isLegalMove(const Move& move);
		/*!
		 * Returns true if \a move repeats a position that was
		 * reached earlier in the game.
		 */
		bool isRepetition(const Move& move);
		/*! Returns a vector of legal moves in the current position. */
		QVector<Move> legalMoves();
		/*!
		 * Returns the result of the game, or Result::NoResult if
		 * the game is in progress.
		 */
		Result result();
		/*!
		 * Returns the expected game result according to endgame tablebases.
		 *
		 * If the position is a win for either player, \a dtm is
		 * set to the distance to mate, ie. the number of plies it
		 * takes to force a mate.
		 *
		 * The default implementation always returns a null result.
		 */
		Result tablebaseResult(unsigned int* dtm = nullptr) const;

		void setWidth(int wd);

		void setHeight(int ht);

		void setSize(int sz);

		QVector<Move> getHistoricalMoves();

		Square chessSquarePublic(int index) const;

	protected:

		/*!
		 * Initializes the variant.
		 *
		 * This function is called by initialize(). Subclasses shouldn't
		 * generally call it by themselves.
		 */
		void vInitialize();

		/*!
		 * Defines a piece type used in the variant.
		 * If the piece isn't already defined, it's gets added here.
		 * Unlike other initialization which happens in vInitialize(),
		 * all piece types should be defined in the constructor.
		 *
		 * \param type Type of the piece in integer format
		 * \param name The piece's name (internationalized string)
		 * \param symbol Short piece name or piece symbol
		 * \param movement A bit mask for the kinds of moves the
		 *        piece can make.
		 * \param gsymbol Select the piece's graphical representation.
		 *	  If not set the \a symbol will be used (default).
		 */
		void setPieceType(int type,
				  const QString& name,
				  const QString& symbol,
				  unsigned movement = 0,
				  const QString & gsymbol = QString());
		/*! Returns true if \a pieceType can move like \a movement. */
		bool pieceHasMovement(int pieceType, unsigned movement) const;

		/*!
		 * Makes \a move on the board.
		 *
		 * This function is called by makeMove(), and should take care
		 * of everything except changing the side to move and updating
		 * the move history.
		 *
		 * Details about piece movement, promotions, captures, drops,
		 * etc. should be stored in \a transition. If \a transition is
		 * 0 then it should be ignored.
		 */
		virtual void vMakeMove(const Move& move, BoardTransition* transition);
		/*!
		 * Reverses \a move on the board.
		 *
		 * This function is called by undoMove() after changing the
		 * side to move to the side that made it.
		 *
		 * \note Unlike vMakeMove(), this function doesn't require
		 * subclasses to update the zobrist position key.
		 */
		virtual void vUndoMove(const Move& move);

		/*! Converts a square index into a Square object. */
		Square chessSquare(int index) const;
		/*! Converts a string into a Square object. */
		Square chessSquare(const QString& str) const;
		/*! Converts a Square object into a square index. */
		int squareIndex(const Square& square) const;
		/*! Converts a string into a square index. */
		int squareIndex(const QString& str) const;
		/*! Converts a square index into a string. */
		QString squareString(int index) const;
		/*! Converts a Square object into a string. */
		QString squareString(const Square& square) const;

		/*!
		 * Converts a Move object into a string in Long
		 * Algebraic Notation (LAN)
		 */
		QString lanMoveString(const Move& move);
		/*!
		 * Converts a Move object into a string in Standard
		 * Algebraic Notation (SAN).
		 *
		 * \note Specs: http://en.wikipedia.org/wiki/Algebraic_chess_notation
		 */
		QString sanMoveString(const Move& move);
		/*! Converts a string in LAN format into a Move object. */
		Move moveFromLanString(const QString& str);
		/*! Converts a string in SAN format into a Move object. */
		Move moveFromSanString(const QString& str);
		/*! Returns the maximal length of a piece symbol */
		int maxPieceSymbolLength() const;

		/*!
		 * Returns the latter part of the current position's FEN string.
		 *
		 * This function is called by fenString(). The board state, side to
		 * move and hand pieces are handled by the base class. This function
		 * returns the rest of it, if any.
		 */
		QString vFenString(FenNotation notation) const;
		/*!
		 * Sets the board according to a FEN string.
		 *
		 * This function is called by setFenString(). The board state, side
		 * to move and hand pieces are handled by the base class. This
		 * function reads the rest of the string, if any.
		 */
		bool vSetFenString(const QStringList& fen);

		/*!
		 * Generates pseudo-legal moves for pieces of type \a pieceType.
		 *
		 * \note If \a pieceType is Piece::NoPiece (default), moves are generated
		 * for every piece type.
		 * \sa legalMoves()
		 */
		void generateMoves(QVarLengthArray<Move>& moves,
				   int pieceType = Piece::NoPiece) const;
		/*!
		 * Generates piece drops for pieces of type \a pieceType.
		 *
		 * \note If \a pieceType is Piece::NoPiece, moves are generated
		 * for every piece type.
		 * \sa generateMoves()
		 */
		void generateDropMoves(QVarLengthArray<Move>& moves, int pieceType) const;
		/*!
		 * Generates pseudo-legal moves for a piece of \a pieceType
		 * at square \a square.
		 *
		 * \note It doesn't matter if \a square doesn't contain a piece of
		 * \a pieceType, the move generator ignores it.
		 */
		void generateMovesForPiece(QVarLengthArray<Move>& moves,
						   int pieceType,
						   int square) const;
		/*!
		 * Generates hopping moves for a piece.
		 *
		 * \param sourceSquare The source square of the hopping piece
		 * \param offsets An array of offsets for the target square
		 * \note The generated \a moves include captures
		 */
		void generateHoppingMoves(int sourceSquare,
					  const QVarLengthArray<int>& offsets,
					  QVarLengthArray<Move>& moves) const;
		/*!
		 * Generates sliding moves for a piece.
		 *
		 * \param sourceSquare The source square of the sliding piece
		 * \param offsets An array of offsets for the target square
		 * \note The generated \a moves include captures
		 */
		void generateSlidingMoves(int sourceSquare,
					  const QVarLengthArray<int>& offsets,
					  QVarLengthArray<Move>& moves) const;
		/*!
		 * Returns true if the current position is a legal position.
		 * If the position isn't legal it usually means that the last
		 * move was illegal.
		 */
		bool isLegalPosition();
		/*!
		 * Returns true if \a move is a legal move.
		 *
		 * This function is called by isLegalMove() after making sure
		 * that there is a pseudo-legal move same as \a move. This
		 * function shouldn't check for the existence of \a move by
		 * generating moves.
		 *
		 * The default implementation only checks if the position
		 * after \a move is legal.
		 */
		bool vIsLegalMove(const Move& move);
		/*!
		 * Returns the type of piece captured by \a move.
		 * Returns Piece::NoPiece if \a move is not a capture.
		 */
		int captureType(const Move& move) const;
		/*! Updates the zobrist position key with \a key. */
		void xorKey(quint64 key);
		/*!
		 * Returns true if a pseudo-legal move \a move exists.
		 * \sa isLegalMove()
		 */
		bool moveExists(const Move& move) const;
		/*! Returns true if the side to move has any legal moves. */
		bool canMove();
		/*!
		 * Returns the size of the board array, including the padding
		 * (the inaccessible wall squares).
		 */
		int arraySize() const;
		/*! Returns the piece at \a square. */
		Piece pieceAt(int square) const;
		/*!
		 * Sets \a square to contain \a piece.
		 *
		 * This function also updates the zobrist position key, so
		 * subclasses shouldn't mess with it directly.
		 */
		void setSquare(int square, Piece piece);
		/*! Returns the last move made in the game. */
		const Move& lastMove() const;
		/*!
		 * Returns the reserve piece type corresponding to \a pieceType.
		 *
		 * The returned value is the type of piece a player receives
		 * (in variants that have piece drops) when he captures a piece of
		 * type \a pieceType.
		 *
		 * The default value is \a pieceType.
		 */
		int reserveType(int pieceType) const;
		/*! Adds \a count pieces of type \a piece to the reserve. */
		void addToReserve(const Piece& piece, int count = 1);
		/*! Removes a piece of type \a piece from the reserve. */
		void removeFromReserve(const Piece& piece);

		void showBoard();


		QVector<FiveConnectionInfo> findFiveConnections();
		bool checkFiveConnection(Square &sq, int fileOffset, int rankOffset);

		struct PieceData
		{
			QString name;
			QString symbol;
			unsigned movement;
			QString representation;
		};
		struct MoveData
		{
			Move move;
			quint64 key;
		};

		bool m_initialized;
		int m_width;
		int m_height;
		quint64 m_key;
		Side m_side;
		Side m_startingSide;
		QString m_startingFen;
		int m_maxPieceSymbolLength;
		QVarLengthArray<PieceData> m_pieceData;
		QVarLengthArray<Piece> m_squares;
		QVector<MoveData> m_moveHistory;
};

} // namespace Chess
#endif // GOMOKUBOARD_H
