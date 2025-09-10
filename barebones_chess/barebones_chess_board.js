

(() => {
/** @typedef {{r:number,f:number}} Sq */
/** @typedef {{color:boolean, from:Sq, to:Sq, piece:string,capture:number, promo:'Q'|'R'|'B'|'N',flags?:{ castle?:'K'|'Q'|'.', enPassant:boolean, doublePush:boolean }}} Move */
/** @typedef {{piece:string, color:boolean, rank:number, file:number, captured:boolean, idP:number}} BoardPiece */
/** @typedef {{piece:string, turn:number}} CaptureRecord */

  const PIECES = {
    'P': '♙','N': '♘','B': '♗','R': '♖','Q': '♕','K': '♔', /* white */
    'p': '♟','n': '♞','b': '♝','r': '♜','q': '♛','k': '♚' /*black*/
  };

  const PIECES_SCORE = {
    'P': 1,'N': 3,'B': 3,'R':5,'Q': 9,'K': 10, /* white */
    'p': 1,'n': 3,'b': 3,'r':5,'q': 9,'k': 10 /*black*/
  };
  
  // --- Core state ---
  class Chess  {

    constructor(boardEl, statusEl, trayleftEl, trayrightEl, st_orientation) {
   
    this.boardEl = boardEl;
    this.statusEl = statusEl;
    this.trayLeftEl = trayleftEl;
    this.trayRightEl = trayrightEl;
    this.current_orientation = st_orientation;
    
    this.gameInfo = {mode:sessionStorage.getItem('mode') === 'computer' ? 'computer':'player' , side:sessionStorage.getItem('b_or') === 'white'? false:true};
    this.whitePromoted = false;
    this.comMove = false;

    this.state = {
      board: [],   
      piece_list: [],           
      sideToMove: true,         // true | false
      castling: { K:true, Q:true, k:true, q:true },
      enPassant: '-',         // 
      halfmove: 0,
      previous_half_move:0,
      moveNumber: 1,
      history: [],              
      white_capture: [],      
      black_capture: [], 
      white_can_move:[[]],
      black_can_move:[[]],
    };
        this._squares = [];
        this._selected = null;

        this.initGame();
    }
    // --- Bootstrap / FEN ---
    initGame() {
      this.setStartPosition();
      this.renderBoard();
      this.renderPosition();
      this.renderStatus();
      this.renderCaptures();
      document.addEventListener('keydown', this.handleKeyPress.bind(this));
      document.getElementById("toggle").addEventListener("click",this.toggleOrientation.bind(this));
      document.getElementById("tips").addEventListener("click", this.toggleTips.bind(this));
      //check if mode is com, if so check if side is w, if so generate com move
      //console.log(this.gameInfo);
      if (this.gameInfo.mode === 'computer' && this.gameInfo.side === this.state.sideToMove){this.comTurn();}

    }

    loadFEN(fen) {
    
    }

    //going to need for the exporting to stockfish
    /** @returns {string} */
    exportFEN() {
      let fen_string = "";
      let space_count = 0;
      for (let i = 0; i<8; i++){
        for (let j = 0; j<8; j++){
          if (!this.state.board[i][j]){
            space_count++;
          }
          else{
            if (space_count > 0) {
              fen_string+= space_count + this.getPieceFromBoard(i,j);
              space_count = 0;
            }
            else if (space_count===0){
              fen_string += this.getPieceFromBoard(i,j);
            }
          }
        }
        if (space_count!==0){fen_string += space_count;}
        if (i!==7) {fen_string+= '/'};
        space_count = 0;
      }
      fen_string+= ' ' + (this.state.sideToMove? 'w':'b') + ' ';

      if (this.state.castling.K){fen_string+= 'K';}
      if (this.state.castling.Q){fen_string+= 'Q';}
      if (this.state.castling.k){fen_string+= 'k';}
      if (this.state.castling.q){fen_string+= 'q';}

      // handle enpassant 
      fen_string += ' ' + this.state.enPassant;
      fen_string += ' ' + this.state.halfmove;
      fen_string += ' ' + this.state.moveNumber;

      return fen_string;
    }

    findScore(piece){ 
     return PIECES_SCORE[piece];
    }

    findMove(depth,color){
    if (depth === 0)  
      return this.moveEval();
    
    let full = this.generateAllLegalMoves(color);
    if (full.length === 0){
      if (this.inCheck(this.locateKingSquare(color),color))
        return -1000;
      return 0;
    }

    let best_score = -1002;
    //console.log(full.length);
    for (let i = 0; i<full.length; i++){ 
      this.applyMove(full[i]);

      let temp_score = -1* this.findMove(depth-1,!color)
      if (temp_score>best_score){
        best_score = temp_score;
      }

      this.undoMove();
    }
    //console.log(best_move);
    return best_score;
    }
    
    bestMove(depth,color){
      let all_legal_moves = this.generateAllLegalMoves(color);
      if (all_legal_moves.length === 0)
        return null;

      let best_score = -1002
      let best_move = {};
      for (let i = 0; i<all_legal_moves.length; i++){
        this.applyMove(all_legal_moves[i]);
        let t_score = this.findMove(depth-1,!color);
        if (t_score > best_score){
          best_score = t_score;
          best_move = all_legal_moves[i];
        }
        this.undoMove();
      }
      return best_move;
    }

    moveEval(){
      let score = 1;
      let color = true;
      let direction = 0;
      for (let i = 0; i<32; i++){
          if (this.state.piece_list[i].captured){continue;}
          color = this.state.piece_list[i].color;
          direction = color? 1:-1;
          score+=(this.findScore(this.state.piece_list[i].piece)*direction);
        }
        //console.log(score);
      return score;
    }

    // --- Move generation & legality ---
    /** @param {Sq} square @returns {Move[]} */
    generateMoves(square) {
      let all_moves = [];
      //check all squares based on piece
      //check to see if these is a piece of the square
      if (!this.state.board[square.r][square.f])
        return [];
      
      let cur_piece = this.state.board[square.r][square.f].piece;
      for (let i = 0; i<8 ; i++){
        for (let j = 0; j<8; j++){
          let temp_move = {color:this.state.board[square.r][square.f].color, from:square,to:{r:i,f:j},piece:cur_piece,capture:this.state.board[i][j] ? this.state.board[i][j].idP:-1,promo:'.',flags:{castle:'.',enPassant:false,doublePush:false}};
          if (this.isLegalMove(temp_move) && !this.moveWhileInCheck(temp_move))
            {
              all_moves.push(temp_move);
            }

        }
      }
      return all_moves;
    }

      /** @param {boolean} color */
    generateAllLegalMoves(color) {
    
    let full_moves = [];

     if (color){
      for (let i = 16; i<32; i++){
        if (this.state.piece_list[i].captured){continue;}
        full_moves = full_moves.concat(this.generateMoves({r:this.state.piece_list[i].rank,f:this.state.piece_list[i].file}));
      }
      //console.log(full_moves);
      return full_moves;
     }

     for (let i = 0; i<16; i++){
        if (this.state.piece_list[i].captured){continue;}
        full_moves = full_moves.concat(this.generateMoves({r:this.state.piece_list[i].rank,f:this.state.piece_list[i].file}));
      }
     
      //console.log(full_moves);
      return full_moves;
    }

    attackGrid(color){


    }

    /** @param {Sq} square @param {white:true | black:false} byColor */
    isSquareAttacked(square, byColor) {
      // TODO
      if (this.state.board[square.r][square.f] && this.state.board[square.r][square.f].color === byColor)
        return false;
      let cap = this.state.board[square.r][square.f] ? this.state.board[square.r][square.f].idP:-1;

      let underAttack = false;
      //look through white pieces
      if (byColor){
          for (let i = 16; i<32; i++){
          if (this.state.piece_list[i].captured){continue;}
          let tempMove = {color:byColor, from:{r:this.state.piece_list[i].rank, f:this.state.piece_list[i].file},
                          to:square, piece:this.state.piece_list[i].piece,
                          capture:cap, promo:'.', 
                          flags:{castle:'.',enPassant:false,doublePush:false}};
          
         if (tempMove.piece  === 'R') {underAttack = this.checkRookMove(tempMove);}
          else if (tempMove.piece  === 'B'){underAttack = this.checkBishopMove(tempMove);}
          else if (tempMove.piece  === 'N'){underAttack = this.checkNightMove(tempMove);}
          else if (tempMove.piece  === 'Q'){underAttack = this.checkQueenMove(tempMove);}
          else if (tempMove.piece  === 'K'){underAttack = this.checkKingMove(tempMove);}
          else if (tempMove.piece  === 'P'){underAttack = this.checkPawnMove(tempMove);/*console.log(tempMove); console.log(underAttack);*/}
          
          if (underAttack){return true;}   
        }
      }

      else if (!byColor){
        for (let i = 0; i<16; i++){
          if (this.state.piece_list[i].captured){continue;}
          let tempMove = {color:byColor, from:{r:this.state.piece_list[i].rank, f:this.state.piece_list[i].file},
                          to:square, piece:this.state.piece_list[i].piece,
                          capture:cap, promo:'.', 
                          flags:{castle:'.',enPassant:false,doublePush:false}};
          
          if (tempMove.piece  === 'r' ){underAttack = this.checkRookMove(tempMove);}
          else if (tempMove.piece === 'b'){underAttack = this.checkBishopMove(tempMove);}
          else if (tempMove.piece  === 'n'){underAttack = this.checkNightMove(tempMove);}
          else if (tempMove.piece  === 'q'){underAttack = this.checkQueenMove(tempMove);}
          else if (tempMove.piece  === 'k'){underAttack = this.checkKingMove(tempMove);}
          else if (tempMove.piece === 'p'){underAttack = this.checkPawnMove(tempMove);/*console.log(tempMove); console.log(underAttack);*/}
        
          if (underAttack){return true;}   
        }

      }

      return false;
    }

    /**@param {Sq} king_square king square to check @param {'w'|'b'} color color of the king to check */
    inCheck(king_square, kColor) {
      return this.isSquareAttacked(king_square,!kColor);
    }

    isCheckmate(color){
      //go through all possible moves for color
      //if any of them result in the king not being in check return false
      //if not, check mate, game is over
      return this.generateAllLegalMoves(color).length === 0;
    }

    locateKingSquare(colorToFind){

      if (colorToFind){
        return {r:this.state.piece_list[28].rank,f:this.state.piece_list[28].file};
      }
      return {r:this.state.piece_list[11].rank,f:this.state.piece_list[11].file};
    }

      checkCastling(move,king_square){
      
      let king_color = this.state.board[king_square.r][king_square.f].color;
      let opposite = !king_color;


      if (this.inCheck(king_square, king_color)){return false;}
      
      if (move.flags.castle === 'K'){

        if (this.state.board[king_square.r][king_square.f + 1]){return false;}

        if (this.state.board[king_square.r][king_square.f + 2]){return false;}

        if (this.isSquareAttacked({r:king_square.r,f:king_square.f+1},opposite)) {return false;}

        if (this.isSquareAttacked({r:king_square.r,f:king_square.f+2},opposite)) {return false;}
      }

      else if (move.flags.castle === 'Q'){
        if (this.state.board[king_square.r][king_square.f - 1]){return false;}
        if (this.state.board[king_square.r][king_square.f - 2]){return false;}
        if (this.state.board[king_square.r][king_square.f - 3]){return false;}


        if (this.isSquareAttacked({r:king_square.r,f:king_square.f-1},opposite)) {return false;}
        if (this.isSquareAttacked({r:king_square.r,f:king_square.f-2},opposite)) {return false;}
        if (this.isSquareAttacked({r:king_square.r,f:king_square.f-3},opposite)) {return false;}
      }

      return true;
    }

    handlePromotion(move){
         move.promo = 'T';
    }

    checkPawnMove(move) {
        //if pawn in second row
        // sq ~ to
        let color = move.color;
        let move_distance = Math.abs( move.to.r - move.from.r );
        if (move_distance > 2)
          return false;
        //don't allow move backwards
        if (color){
            if (move.from.r < move.to.r)
                return false;
            if (move.from.r!=6 && move_distance > 1)
                return false;
            //handle promotion here?
            //maybe set promotion here, and handle in apply move
            if (move.to.r === 0){
               this.handlePromotion(move);
            }

            if (move_distance === 2){
                if (this.state.board[move.from.r - 1][move.from.f] || this.state.board[move.from.r - 2][move.from.f])
                    return false;
            }
            //en passant
            if (move.from.r === 3){

              //left
              if (move.from.f !==0){
                let en_pass = this.state.history.pop();
                this.state.history.push(en_pass);
                //console.log("en_pass");
                //console.log (en_pass);
                if (en_pass.piece === 'p' && en_pass.flags.doublePush && (en_pass.to.r === move.from.r) && (en_pass.to.f === move.from.f - 1)){
                  if (move.to.r === en_pass.to.r - 1 && move.to.f === en_pass.to.f){
                    move.flags.enPassant = true;
                    return true;
                  }
                }
              }
              //right
              if (move.from.f !==7){
                let en_pass = this.state.history.pop();
                this.state.history.push(en_pass);
                //console.log(" en_pass ");
                //console.log (en_pass);
                if (en_pass.piece === 'p' && en_pass.flags.doublePush && (en_pass.to.r === move.from.r) && (en_pass.to.f === move.from.f + 1)){
                  if (move.to.r === en_pass.to.r - 1 && move.to.f === en_pass.to.f){
                    move.flags.enPassant = true;
                    return true;
                  }
                }
              }

            }
        }

        else if (!color){
            if (move.from.r > move.to.r)
                return false;
              if (move.from.r!=1 && move_distance > 1)
                return false;

              if (move.to.r === 7)/*I do not know if this is the right row*/{
               this.handlePromotion(move);
            }
            if (move_distance === 2){
                if (this.state.board[move.from.r + 1][move.from.f] || this.state.board[move.from.r + 2][move.from.f])
                    return false;
            }

            if (move.from.r === 4){
              if (move.from.f !==0){
                let en_pass = this.state.history.pop();
                this.state.history.push(en_pass);
                //console.log(" en_pass ");
                //console.log (en_pass);
                if (en_pass.piece === 'P' && en_pass.flags.doublePush && (en_pass.to.r === move.from.r) && (en_pass.to.f === move.from.f - 1)){
                  if (move.to.r === en_pass.to.r + 1 && move.to.f === en_pass.to.f){
                    move.flags.enPassant = true;
                    return true;
                  }
                }
              }
              //right
              if (move.from.f !==7){
                let en_pass = this.state.history.pop();
                this.state.history.push(en_pass);
                //console.log(" en_pass ");
                //console.log (en_pass);
                if (en_pass.piece === 'P' && en_pass.flags.doublePush && (en_pass.to.r === move.from.r) && (en_pass.to.f === move.from.f + 1)){
                  if (move.to.r === en_pass.to.r + 1 && move.to.f === en_pass.to.f){
                    move.flags.enPassant = true;
                    return true;
                  }
                }
              }
            }
          
        }
            if (move.from.f !== move.to.f){
                  if (move.from.r === move.to.r)
                    return false;
                  if (!this.state.board[move.to.r][move.to.f])
                    return false;
                  if (Math.abs(move.to.f-move.from.f) > 1)
                    return false;
                  if (Math.abs(move.to.r-move.from.r) !== 1)
                    return false;
              }

            if (move.to.f === move.from.f && this.state.board[move.to.r][move.to.f])
                  return false;
        if (move_distance === 2){move.flags.doublePush = true;}

        //account for enpassant
        
        return true;
    }

    checkRookMove(move){
      let rankMove = move.from.r === move.to.r; // 7 and 5 false
      let fileMove = move.from.f === move.to.f; // 0 and 0 true
      if (!rankMove && !fileMove) {
        return false; 
      }
      
      //return false if piece is in the path of move
      // what if moves to left or right
      if (rankMove){
        //right
        if (move.from.f < move.to.f){
        for (let i = move.from.f+1; i<=move.to.f; i++){
          if (this.state.board[move.from.r][i] && i!==move.to.f)
            return false;
          }
        }
        //left
        if (move.from.f > move.to.f){
        for (let i = move.to.f; i<move.from.f; i++){
          if (this.state.board[move.from.r][i] && i!==move.to.f)
            return false;
          }
        }

      }
      //what if moves up or down
      if (fileMove){
        //down
        if (move.from.r<move.to.r){
        for (let i = move.from.r+1; i<=move.to.r; i++){
          if (this.state.board[i][move.from.f] && i!==move.to.r)
            return false;
          }
        }
        //up
        else if (move.from.r>move.to.r){
        for (let i = move.to.r; i<move.from.r; i++){
          if (this.state.board[i][move.from.f] && i!==move.to.r)
            return false;
          }
        }
      }

      return true;
    }

    checkNightMove(move){
      let valid_move = false
      let vert_move = Math.abs(move.to.r-move.from.r);
      let horizontal_move = Math.abs(move.to.f - move.from.f);

      if (vert_move === 2 && horizontal_move === 1)
         valid_move = true;
      else if (vert_move === 1 && horizontal_move === 2)
        valid_move = true;
      
      return valid_move;
    }

    checkBishopMove(move){
      let fileDis = (move.to.f - move.from.f);
      let rankDis = (move.to.r - move.from.r);

      if (Math.abs(fileDis) !== Math.abs(rankDis))
        return false;
      
      if (fileDis < 0 && rankDis < 0)
      {
        let j = move.to.r+1;
        for (let i = move.to.f+1; i<move.from.f; i++){
           if (this.state.board[j][i])
              return false;
            j=j+1;
        }
      }

      if (fileDis > 0 && rankDis < 0)
      {
        let j = move.from.r-1;
        for (let i = move.from.f+1; i<move.to.f; i++){
           if (this.state.board[j][i])
              return false;
            j=j-1;
        }
      }

      if (fileDis > 0 && rankDis > 0)
      {
        let j = move.from.r+1;
        for (let i = move.from.f+1; i<move.to.f; i++){
           if (this.state.board[j][i])
              return false;
            j=j+1;
        }
      }

      if (fileDis < 0 && rankDis > 0)
      {
        let j = move.to.r-1;
        for (let i = move.to.f+1; i<move.from.f; i++){
           if (this.state.board[j][i])
              return false;
            j=j-1;
        }
      }
    
      return true;
    }

    checkQueenMove(move){
      return this.checkRookMove(move) != this.checkBishopMove(move); 
      //true true false - correct
      //true false true - correct
      //false true true - correct
      //false false false - correct
    }

    checkKingMove(move){
      let valid_move = false
      let rankDis = move.to.r - move.from.r;
      let fileDis = move.to.f - move.from.f;

      //up
      if (rankDis === 1 && fileDis === 0)
        valid_move = true;
      //down
      else if (rankDis === -1 && fileDis === 0)
        valid_move = true;
      //left
      else if (rankDis === 0 && fileDis === 1)
        valid_move = true;
      //right
      else if (rankDis === 0 && fileDis === -1)
        valid_move = true;
      //upper left
      else if (rankDis === 1 && fileDis === 1)
        valid_move = true;
      //upper right
      else if (rankDis === -1 && fileDis === 1)
        valid_move = true;
      //down left
      else if (rankDis === 1 && fileDis === -1)
        valid_move = true;
      //down right
      else if (rankDis === -1 && fileDis === -1)
        valid_move = true;

      if (valid_move){
        if (move.color){
          this.state.castling.K = false;
          this.state.castling.Q = false;
        }
        else if (!move.color) {
          this.state.castling.k = false;
          this.state.castling.q = false;
        }
      }

      return valid_move;
    }

    /** @param {Move} move */
    isLegalMove(move) {
      if (move.color !== this.state.sideToMove){
        return false;
      }

      if (this.state.board[move.to.r][move.to.f] && move.color === this.state.board[move.to.r][move.to.f].color)
        {  
            return false;}

        
      let piece_check = false;
      if (move.piece === 'p' || move.piece === 'P'){piece_check = this.checkPawnMove(move);}
      else if (move.piece === 'r' || move.piece === 'R'){piece_check = this.checkRookMove(move);}
      else if (move.piece === 'b' || move.piece === 'B'){piece_check = this.checkBishopMove(move);}
      else if (move.piece === 'n' || move.piece === 'N'){piece_check = this.checkNightMove(move);}
      else if (move.piece === 'q' || move.piece === 'Q'){piece_check = this.checkQueenMove(move);}
      
      else if (move.piece === 'k' || move.piece === 'K'){
        piece_check = this.checkKingMove(move);
        
           //king side castle
        if (move.to.r === move.from.r && move.to.f === move.from.f+2){
        move.flags.castle = 'K';
        if (this.checkCastling(move,move.from)){
        piece_check = true;
        move.flags.castle = 'K';
        }
        else {move.flags.castle = '.';}
        }

      //queen side castle
        else if (move.to.r === move.from.r && move.to.f == move.from.f-2){
        move.flags.castle = 'Q';
        
        if (this.checkCastling(move,move.from)){
        piece_check = true;
        move.flags.castle = 'Q';
        }
        else {move.flags.castle = '.';}
        }
      }
      return piece_check;
    }

    moveWhileInCheck(move){
            
            let before_to_piece = this.state.board[move.to.r][move.to.f];
            let before_from_piece = this.state.board[move.from.r][move.from.f];

            this.state.board[move.to.r][move.to.f] = this.state.board[move.from.r][move.from.f];
            this.state.board[move.from.r][move.from.f] = null;

            let kingIsInCheck = this.inCheck(this.locateKingSquare(this.state.sideToMove),this.state.sideToMove);

            this.state.board[move.from.r][move.from.f] = before_from_piece;
            this.state.board[move.to.r][move.to.f] = before_to_piece;
            if (kingIsInCheck){
              return true;
            }     
        return false;
    }
  
    // --- Apply / Undo ---
    /** @param {Move} move */
    applyMove(move) {

     
      // handle captures, promotion, castling, en-passant, clocks, move number
      //assuming valid move
      this.state.history.push(move);

      this.state.enPassant = '-';
      if (move.flags.doublePush){
        let rank = (move.from.r + move.to.r)/2;
        let file = (move.to.f);
        this.state.enPassant = coordText(rank,file);
      }

      if (this.getPieceFromBoard(move.to.r,move.to.f) !== '.'){
        let piece_color = this.state.board[move.from.r][move.from.f].color;
        this.state.piece_list[  (this.state.board[move.to.r][move.to.f]).idP ].captured = true;
        let cap_rec = {piece:this.getPieceFromBoard(move.to.r,move.to.f), turn: this.state.moveNumber};
        if (piece_color) {this.state.white_capture.push(cap_rec);}
        else {this.state.black_capture.push(cap_rec);}
      }

      if (move.flags.castle !== '.'){
        //Kingside
        if (move.flags.castle === 'K'){
          //first move king
          //this.state.board[move.from.r][move.from.f+2] = move.piece;
          //this.state.board[move.from.r][move.from.f] = '.';
          this.movePieceOnBoard(move.from.r, move.from.f, move.from.r, move.from.f+2);
          //then move castle
          //this.state.board[move.from.r][move.from.f+1] = this.state.board[move.from.r][move.from.f+3];
          //this.state.board[move.from.r][move.from.f+3] = '.';
          this.movePieceOnBoard(move.from.r, move.from.f+3, move.from.r, move.from.f+1);
        }
        
        //Queenside
        else {
           //first move king
          //this.state.board[move.from.r][move.from.f-2] = move.piece;
          //this.state.board[move.from.r][move.from.f] = '.';
          this.movePieceOnBoard(move.from.r, move.from.f, move.from.r, move.from.f-2);

          //then move castle
          //this.state.board[move.from.r][move.from.f-1] = this.state.board[move.from.r][move.from.f-4];
          //this.state.board[move.from.r][move.from.f-4] = '.';
          this.movePieceOnBoard(move.from.r, move.from.f-4, move.from.r, move.from.f-1);

        }
      }

      else if (move.promo === 'T'){
        this.whitePromoted =true;
        console.log("before run:" + move.promo);
          Swal.fire({
          title: "Choose promotion",
          html: `
            
          <div class="container" style="
          display: grid;
          grid-template-columns: repeat(2, 1fr);
          gap: 12px;
          align-items: center;
          justify-items: center;
        ">
            <div style="display:flex; flex-direction:column; align-items:center; gap:8px;">
              <img src="C:/Users/hoode/OneDrive/Documents/basic_chess with move eval/images/queen_e.gif" alt="Funny animation" style="max-width:50%; height:auto;">
              <button type="button" onclick="window.promoVar = 'q'; Swal.close();">Queen</button>
            </div>

            <div style="display:flex; flex-direction:column; align-items:center; gap:8px;">
              <img src="C:/Users/hoode/OneDrive/Documents/basic_chess with move eval/images/bishop.gif" alt="Funny animation" style="max-width:50%; height:auto;">
              <button type="button" onClick= "window.promoVar = 'b' ; Swal.close(); ">Bishop</button>
              </div>

            <div style="display:flex; flex-direction:column; align-items:center; gap:8px;">
              <img src="C:/Users/hoode/OneDrive/Documents/basic_chess with move eval/images/knight.gif" alt="Funny animation" style="max-width:50%; height:auto;">
                <button type="button" onClick= "window.promoVar = 'n' ; Swal.close(); ">Knight</button>
              </div>

            <div style="display:flex; flex-direction:column; align-items:center; gap:8px;">
              <img src="C:/Users/hoode/OneDrive/Documents/basic_chess with move eval/images/rook.gif" alt="Funny animation" style="max-width:50%; height:auto;">
              <button type="button" onClick= "window.promoVar = 'r' ; Swal.close(); ">Rook</button>
              </div>
          </div>
                    
              
          `,
          allowOutsideClick:false,
          allowEscapeKey: false, 
          showConfirmButton: false
          }).then (() => {
            
            move.promo  = window.promoVar;
            window.promoVar = '.';
            console.log("after run:" + move.promo);

            //make set Board Piece function just for this

            this.movePieceOnBoard(move.from.r, move.from.f, move.to.r, move.to.f);
            if (!move.color){
              //this.state.board[move.to.r][move.to.f] = move.promo;
              this.setPieceOnBoard(move.to.r,move.to.f,move.promo);

            }
            else {
              //this.state.board[move.to.r][move.to.f] = (move.promo).toUpperCase();
              this.setPieceOnBoard(move.to.r,move.to.f,(move.promo).toUpperCase());

            }
            //this.state.board[move.from.r][move.from.f] = '.';
            this.renderCaptures();
            this.renderPosition();
            this.whitePromoted = false;
          });


      }

      else if (move.flags.enPassant){

        //this.state.board[move.from.r][move.from.f] = '.';
        //this.state.board[move.to.r][move.to.f] = move.piece;
        this.movePieceOnBoard(move.from.r, move.from.f, move.to.r, move.to.f);

        // find pawn being passed 
        let cal = move.color ? -1:1;
        //move.capture = this.state.board[move.to.r - cal][move.to.f];
       // move.capture = this.getPieceFromBoard(move.to.r-cal,move.to.f);
        move.capture = this.state.board[move.to.r-cal][move.to.f].idP;
        this.state.board[move.to.r - cal][move.to.f] = null;

        if (cal < 0){this.state.white_capture.push({piece:'p',turn:this.state.moveNumber});}
        else {this.state.black_capture.push({piece:'P',turn:this.state.moveNumber});}
      }

      else {
      //this.state.board[move.from.r][move.from.f] = '.';
      //this.state.board[move.to.r][move.to.f] = move.piece;
      this.movePieceOnBoard(move.from.r, move.from.f, move.to.r, move.to.f);
      }

      if (!this.state.sideToMove){this.state.moveNumber +=1;}
      this.state.previous_half_move = this.state.halfmove;
      if (move.capture === -1 && move.piece !== 'P' && move.piece !== "p") { this.state.halfmove+=1;}
      else {this.state.halfmove = 0;}

      this._selected = null;
      this._squareAt(move.to).classList.remove('selected');
      //statusEl.textContent = `moved to ${coordName(move.to.f,move.to.r)}`;
      //toast.push(this.state.sideToMove + ` Moved to ${coordName(move.to.f,move.to.r)}`);
      this.renderPosition();
      this.toggleTurn();
      //this.state.sideToMove = !this.state.sideToMove;
      this.renderStatus();
      this.renderCaptures();
    }

    undoMove() {
      // TODO: pop from history and revert
      //console.log("undoing move...");
      if (this.state.history.length === 0)
        return;

      let previous_move = this.state.history.pop();

      // 0.5 check if castle
      if (previous_move.flags.castle !== '.'){
      //depends only if king or queen side
      
      //king side
        if (previous_move.flags.castle === 'K'){

          //this.state.board[previous_move.from.r][previous_move.from.f+2] = '.';
          //this.state.board[previous_move.from.r][previous_move.from.f] = previous_move.piece;
          this.movePieceOnBoard(previous_move.from.r, previous_move.from.f+2, previous_move.from.r, previous_move.from.f);

          //then move castle
          //this.state.board[previous_move.from.r][previous_move.from.f+3] = this.state.board[previous_move.from.r][previous_move.from.f+1];
          //this.state.board[previous_move.from.r][previous_move.from.f+1] ='.' ;
          this.movePieceOnBoard(previous_move.from.r, previous_move.from.f+1, previous_move.from.r, previous_move.from.f+3);
        }
      //queen side
        else{
          //this.state.board[previous_move.from.r][previous_move.from.f-2] = '.';
          //this.state.board[previous_move.from.r][previous_move.from.f] = previous_move.piece;
          this.movePieceOnBoard(previous_move.from.r, previous_move.from.f-2, previous_move.from.r, previous_move.from.f);
          //then move castle
          //this.state.board[previous_move.from.r][previous_move.from.f-4] = this.state.board[previous_move.from.r][previous_move.from.f-1];
          //this.state.board[previous_move.from.r][previous_move.from.f-1] ='.' ;
          this.movePieceOnBoard(previous_move.from.r, previous_move.from.f-1, previous_move.from.r, previous_move.from.f+4);


        }

      }
      //need to account for enpassant undo as the pawn probably won't be restored

      //0.9 - undo enpassant captures 
      if(previous_move.flags.enPassant){
           
        //forwards
        //this.state.board[move.from.r][move.from.f] = '.';
        //this.state.board[move.to.r][move.to.f] = move.piece;

        // find pawn being passed 
        //let cal = checkPieceColor(move.piece) === 'w' ? -1:1;
        //this.state.board[move.to.r - cal][move.to.f] = '.';

        //this.state.board[previous_move.from.r][previous_move.from.f] = previous_move.piece;
        //this.state.board[previous_move.to.r][previous_move.to.f] = '.';
        this.movePieceOnBoard(previous_move.to.r, previous_move.to.f, previous_move.from.r, previous_move.from.f);
        //find where en passant was captured
        let cal = previous_move.color ? 1:-1;
        //this.state.board[previous_move.to.r + cal][previous_move.to.f] = previous_move.capture;
        
        this.state.piece_list[previous_move.capture].captured = false;
        this.state.board[previous_move.to.r + cal][previous_move.to.f] = this.state.piece_list[previous_move.capture];
      }

      //1 - pieces need to be moved back
      else {
      
        //this.state.board[previous_move.from.r][previous_move.from.f] = previous_move.piece;
        this.movePieceOnBoard(previous_move.to.r, previous_move.to.f, previous_move.from.r, previous_move.from.f);

        if(previous_move.capture !== -1){
          //this.state.board[previous_move.to.r][previous_move.to.f] = previous_move.capture;

          this.state.piece_list[previous_move.capture].captured = false;
          this.state.board[previous_move.to.r][previous_move.to.f] = this.state.piece_list[previous_move.capture];
          if (this.state.sideToMove){
            this.state.black_capture.pop();
          }
          else {this.state.white_capture.pop();}
        }
        else {
         // this.state.board[previous_move.to.r][previous_move.to.f] = '.';
         this.state.board[previous_move.to.r][previous_move.to.f] = null;
        }

      }

      //2 - move number subtracted
      if (this.state.sideToMove){this.state.moveNumber-=1;}
      //2.1 - handle half moves
      if (previous_move.capture === -1 && previous_move.piece !== 'P' && previous_move.piece !== "p") { this.state.halfmove-=1;}
      else { this.state.halfmove = this.state.previous_half_move;}
      //3 - turn needs to be toggled back
      //this.toggleTurn();
     
      this.state.sideToMove = !this.state.sideToMove; 
      
      //4 - ensure that castling priveleges are accurate
      //5 - ensure that black capture and white list are accurate
     

      this.renderPosition();
      this.renderCaptures();
      //statusEl.textContent = `Move undone...`;
      //toast.push("Move undone...");
    }

    handleKeyPress(event) {
      //undo move
      if (event.key === 'r') {
        this.undoMove();
      }

      //for testing, will be taken out probably
      if (event.key === 's') {

        console.log(this.state.history);
        //console.log(window.Chess.state.history);
        Swal.fire({
          title: "Choose promotion",
          html:
            `
            <div class="container" style="
  display: grid;
  grid-template-columns: repeat(2, 1fr);
  gap: 12px;
  align-items: center;
  justify-items: center;
">
  <div style="display:flex; flex-direction:column; align-items:center; gap:8px;">
    <img src="C:/Users/hoode/OneDrive/Documents/basic_chess with move eval/images/queen_e.gif" alt="Funny animation" style="max-width:50%; height:auto;">
    <button type="button" onclick="window.testVar += 'rr'; Swal.close();">Queen</button>
  </div>

  <div style="display:flex; flex-direction:column; align-items:center; gap:8px;">
    <img src="C:/Users/hoode/OneDrive/Documents/basic_chess with move eval/images/bishop.gif" alt="Funny animation" style="max-width:50%; height:auto;">
    <button type="button" onClick= "window.promoVar = 'b' ; Swal.close(); ">Bishop</button>
    </div>

  <div style="display:flex; flex-direction:column; align-items:center; gap:8px;">
    <img src="C:/Users/hoode/OneDrive/Documents/basic_chess with move eval/images/knight.gif" alt="Funny animation" style="max-width:50%; height:auto;">
      <button type="button" onClick= "window.promoVar = 'n' ; Swal.close(); ">Knight</button>
    </div>

  <div style="display:flex; flex-direction:column; align-items:center; gap:8px;">
    <img src="C:/Users/hoode/OneDrive/Documents/basic_chess with move eval/images/rook.gif" alt="Funny animation" style="max-width:50%; height:auto;">
    <button type="button" onClick= "window.promoVar = 'n' ; Swal.close(); ">Knight</button>
    </div>
</div>
              
          `,
          allowOutsideClick: false,
          allowEscapeKey: false,
          showConfirmButton: false
        }).then(() => {
          console.log("after run:" + window.testVar);
        });

      }

      //show possible moves for a given piece
      if (event.key === 'g') {

        if (!this._selected) {
          //statusEl.textContent = `cannot generate moves as there is nothing selected`
          toast.push("Cannot generate moves - No piece selected");
        }
        else {
          //statusEl.textContent = `generating moves...`
          toast.push("Generating moves");
          let g_moves = this.generateMoves(this._selected);
          console.log(g_moves);
          for (let i = 0; i < g_moves.length; i++) {
            console.log(g_moves[i]);
            this._squareAt(g_moves[i].to).classList.add("generated");
          }
        }
      }

      // reset game
      if (event.key === 'm') {
        this.resetGame();
        toast.push('Game Reset!');
      }

      //in the future will press to generate move ranking
      if (event.key === 'c') {
        if (this._selected) {


          //generate moves for a piece
          //apply move one by one
          //for one move call postChessApi with exportFen
          //append move eval score to the square
          //undo move
          //repeat cycle for gen'ed moves

          let move_data = 0;


          console.log(move_data);

          let score_moves = this.generateMoves(this._selected);
          for (let i = 0; i < score_moves.length; i++) {

            this.applyMove(score_moves[i]);

            console.log("before api call");
            let score = this.moveEval();
            console.log(score);
            const note = document.createElement('span');
            note.className = 'square-note';
            note.textContent = `score: ${score}`;   // <-- your text
            this._squareAt(score_moves[i].to).appendChild(note);

            this.undoMove();

          }
          /* postChessApi({fen:this.exportFEN()}).then((data)=> {
        console.log(data);
        });*/
        }
      }

      //show pieces in danger
      if (event.key === 'd') {
        let sideToCheck = this.state.sideToMove? 'b' : 'w';
        for (let i = 0; i < 8; i++)
          for (let j = 0; j < 8; j++) {
            console.log("danger...?");
            if (this.state.board[i][j] === '.') { continue; }
            if (this.isSquareAttacked({ r: i, f: j }, sideToCheck)) { this._squareAt({ r: i, f: j }).classList.add("danger"); }
          }
      }

      if (event.key === 'f') {
        console.log(this.exportFEN());
      }

      if (event.key === 'p') {
        if (this._selected) { console.log("r: " + this._selected.r + " f: " + this._selected.f); }
      }

      if (event.key === 'l') {
        console.log("black capture:")
        console.log(this.state.black_capture);
        console.log("white capture:")
        console.log(this.state.white_capture);
      }

      if (event.key === 't'){
        console.log("starting");
        let tbefore = Date.now();
        console.log(this.moveGenTest(1,true));
        let tafter = Date.now();
        console.log("time in miliseconds");
        console.log (tafter - tbefore);

        tbefore = Date.now();
        console.log(this.moveGenTest(2,true));
        tafter = Date.now();
        console.log("time in miliseconds");
        console.log (tafter - tbefore);

        tbefore = Date.now();
        console.log(this.moveGenTest(3,true));
        tafter = Date.now();
        console.log("time in miliseconds");
        console.log (tafter - tbefore);

         tbefore = Date.now();
        console.log(this.moveGenTest(4,true));
        tafter = Date.now();
        console.log("time in miliseconds");
        console.log (tafter - tbefore);
      }

      }

    moveGenTest(depth,color){
      if (depth===0)
        return 1;

      let full_m = this.generateAllLegalMoves(color);
      if (full_m.length === 0)
        return 0;
      let pos = 0;
      for (let i = 0; i<full_m.length; i++){
        this.applyMove(full_m[i]);
       pos+= this.moveGenTest(depth-1,!color);
        this.undoMove(full_m[i]);
      }
      return pos;
    }
    // --- Turn & status ---
    getTurn() { return this.state.sideToMove; }
    
    comTurn() {

        if (!this.whitePromoted){
         setTimeout(()=> {
        // let com_moves = this.generateAllLegalMoves(this.gameInfo.side);
        // let ind = Math.random()*com_moves.length-1;
        // this.applyMove(com_moves[ind|0]);\
        this.gameInfo.mode = 'simulator';
        
                 let move_to_make = this.bestMove(2,this.gameInfo.side);
              console.log(move_to_make);
              console.log(this.gameInfo.side);
            this.applyMove(move_to_make);

        this.gameInfo.mode = 'computer';
          
            }, 1000);
        }

        else if (this.whitePromoted){
            setTimeout(()=> {
        // let com_moves = this.generateAllLegalMoves(this.gameInfo.side);
        // let ind = Math.random()*com_moves.length-1;
        // this.applyMove(com_moves[ind|0]);\
                 let move_to_make = this.findMove(1,this.gameInfo.side);
              console.log(move_to_make);

            this.applyMove(move_to_make.move);
            }, 10000);
        }
    }

    toggleTurn() {
        this.state.sideToMove = !this.state.sideToMove; 
        //
        if (this.gameInfo.mode === 'computer' && this.gameInfo.side === this.state.sideToMove){this.comTurn();}

        }

    toggleOrientation() {
        console.log(this.current_orientation);
        if(this.current_orientation === 'white'){this.current_orientation = 'black';}
        else{
          this.current_orientation = 'white';
        } 
        console.log(this.current_orientation);

        this.renderBoard();
        this.renderPosition();
        this.renderCaptures();
      }
      
    toggleTips(){
       Swal.fire({
          title: "Tips",
          html: 
            `
            <div class="container" style="
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 12px;
            align-items: center;
            justify-items: center;
                    ">

            <div style="display:flex; flex-direction:column; align-items:center; gap:8px;">
                  <p>R - Press R to Undo Move</p>
              </div>

                <div style="display:flex; flex-direction:column; align-items:center; gap:8px;">
                  <p>M - Press M to Reset the Game</p>
              </div>

                <div style="display:flex; flex-direction:column; align-items:center; gap:8px;">
                  <p>C - Press C to display Move Evaluation for a piece</p>
              </div>

                <div style="display:flex; flex-direction:column; align-items:center; gap:8px;">
                  <p>G - Press G to display </p>
              </div>

                <div style="display:flex; flex-direction:column; align-items:center; gap:8px;">
                  <p>D - Press D to display pieces in danger</p>
              </div>

          </div>
              
          `,
          allowOutsideClick:false,
          allowEscapeKey: false, 
          showConfirmButton: true
          })
        
    }


    getGameStatus() {
      // TODO: compute check, checkmate, stalemate, draw
      return {};
    }

    // --- UI Hooks ---
    /** @param {Sq} sq */
    onSquareClick(sq) {

      //remove generated lines upon click
        for (let i = 0; i<8; i++)
        for (let j = 0; j<8; j++){
          if (this._squareAt({r:i,f:j}).classList.contains("generated"))
            this._squareAt({r:i,f:j}).classList.remove("generated");
          if (this._squareAt({r:i,f:j}).classList.contains("danger"))
            this._squareAt({r:i,f:j}).classList.remove("danger");
        }

        //check_clicked = this.state.sideToMove === 'b';
        if (this._selected && this.getPieceColorFromBoard(this._selected.r,this._selected.f) !== this.state.sideToMove){
            //statusEl.textContent = 'Not your turn';
            this._selected = null;
            this._squareAt(sq).classList.remove('selected');
            this.renderStatus();
            this.renderPosition();
            return;
        }
        
        if (!this._selected) {
            if (this.getPieceFromBoard(sq.r,sq.f) === '.') { this.statusEl.textContent = `${coordName(sq.f,sq.r)} Empty square. Pick a piece.`; return; }
            this._selected = sq;
            this._squareAt(sq).classList.add('selected');
            this.statusEl.textContent = `Selected ${coordName(sq.f, sq.r)}. Choose a destination.`;
            //toast.push(`Selected ${coordName(sq.f,sq.r)}. Choose a destination`);
            }
        
        else if (this._selected.r === sq.r && this._selected.f === sq.f){
            this._selected = null;
            this._squareAt(sq).classList.remove('selected');
            this.statusEl.textContent = `Unselected ${coordName(sq.f, sq.r)}. Choose another piece.`;
            //toast.push( `Unselected ${coordName(sq.f, sq.r)}. Choose another piece.`);
            this.renderPosition();
            return;
        }
        // move - {from:Sq,to:Sq,piece:string,capture?:string,promo?:'Q'|'R'|'B'|'N',flags?:{castle?:'K'|'Q',enPassant?:boolean,doublePush?:boolean}}}
        
      else {
        const from = this._selected;
        //const piece = this.state.board[from.r][from.f];
        const piece = this.getPieceFromBoard(from.r, from.f);
        //let isCapture = this.getPieceFromBoard(sq.r,sq.f)!=='.' ? this.getPieceFromBoard(sq.r,sq.f):'.';
        let isCapture = this.getPieceFromBoard(sq.r, sq.f) !== '.' ? this.state.board[sq.r][sq.f].idP : -1;

        let move = { color: this.state.sideToMove, from, to: sq, piece, capture: isCapture, promo: 'F', flags: { castle: '.', enPassant: false, doublePush: false } };
        //this.state.current_move = move;
        let check = this.moveWhileInCheck(move);
        let legal = this.isLegalMove(move);

        console.log(move);
        if (!check && legal) { this.applyMove(move); }
        let game_over = this.isCheckmate(this.state.sideToMove);
        if (game_over) {
          Swal.fire({
            title: "Game Over!",
            text: "Checkmate",
            icon: "success",
            position: "bottom-end",
            backdrop: false
          }).then(() => {
            //add a then statement to reset game when the checkmate swal fire closes
            this.resetGame();
          });

        }


        else if (!legal) {
          this.statusEl.textContent = `Selected ${coordName(from.f, from.r)}.\n Invalid move to ${coordName(sq.f, sq.r)}...`;
          //toast.push(`Selected ${coordName(from.f, from.r)}.\n Invalid move to ${coordName(sq.f, sq.r)}...`);
        }

      } 
    }

    enableDragAndDrop() { /* TODO: wire events */ }
    onDragStart(e) { /* TODO */ }
    onDragOver(e) { /* TODO */ }
    onDrop(e) { /* TODO */ }

    highlightMoves(moves) { /* TODO */ }
    clearHighlights() { /* TODO */ }

    // --- Rendering ---
    renderBoard() {
      this._squares = [];
      this.boardEl.innerHTML = '';

      if (this.current_orientation === 'white'){
      for (let r = 0; r < 8; r++) {
        for (let f = 0; f < 8; f++) {
          const sq = document.createElement('div');
          sq.className = 'square ' + (((r + f) % 2 === 0) ? 'light' : 'dark');
          sq.dataset.rank = r; 
          sq.dataset.file = f;
          const coord = document.createElement('div');
          coord.className = 'coords';
          coord.textContent = coordName(f,r);
          sq.appendChild(coord);
            sq.addEventListener('click', () => {
            const { ri, fi } = toBoardIndex(r, f);
            this.onSquareClick({ r: ri, f: fi }); // pass *board* space to your logic
            });
          this.boardEl.appendChild(sq);
          this._squares.push(sq);
        }
      }
    }
    else if (this.current_orientation === 'black'){
      for (let r = 7; r >=0; r--) {
        for (let f = 7; f >=0; f--) {
          const sq = document.createElement('div');
          sq.className = 'square ' + (((r + f) % 2 === 0) ? 'light' : 'dark');
          sq.dataset.rank = r; 
          sq.dataset.file = f;
          const coord = document.createElement('div');
          coord.className = 'coords';
          //const fileChar = String.fromCharCode('a'.charCodeAt(0) + f);
          //const rankChar = String(r+1);
          //coord.textContent = fileChar + rankChar;

          sq.appendChild(coord);
            sq.addEventListener('click', () => {
            //const { ri, fi } = toBoardIndex(r, f);
            const {ri, fi} = {ri:r,fi:f};
            this.onSquareClick({ r: ri, f: fi }); // pass *board* space to your logic
            });
          this.boardEl.appendChild(sq);
          this._squares.push(sq);
        }
      }
    }

    }

    renderPosition() {
      //return;
      for (const sq of this._squares) {
        const r = +sq.dataset.rank;
        const f = +sq.dataset.file;

        const rankIndex = r;
        const fileIndex = f ;
        //const piece = this.state.board[rankIndex][fileIndex];   
        const piece = this.getPieceFromBoard(rankIndex,fileIndex);
        //const piece = this.state.board[7-r][7-f];   
        
        sq.innerHTML = '';
        const coord = document.createElement('div');
        coord.className = 'coords';
        coord.textContent = coordName(f,r);
        sq.appendChild(coord);

        if (piece !== '.' && PIECES[piece]) {
          const span = document.createElement('span');
          span.textContent = PIECES[piece];
          sq.appendChild(span);
        }
        sq.classList.remove('selected');
      }
    }

    renderStatus() {
      const turn = this.getTurn()? 'White' : 'Black';
      this.statusEl.textContent = `${turn} to move.`;
    }

    renderCaptures() {
        if (!this.trayLeftEl || !this.trayRightEl) return;

        // Decide which side goes to which tray based on orientation
        const leftData  = this.current_orientation === 'white' ? this.state.white_capture : this.state.black_capture;
        const rightData = this.current_orientation === 'white' ? this.state.black_capture : this.state.white_capture;

        this.trayLeftEl.innerHTML = '';
        this.trayRightEl.innerHTML = '';

        this.trayLeftEl.appendChild(this.trayHeader(this.current_orientation === 'white' ? "Black pieces captured by White" : "White pieces captured by Black"));
        this.trayRightEl.appendChild(this.trayHeader(this.current_orientation === 'white' ? "White pieces captured by Black" : "Black pieces captured by White"));

        this.trayLeftEl.appendChild(this.renderCaptureList(leftData));
        this.trayRightEl.appendChild(this.renderCaptureList(rightData));

        this.trayLeftEl.appendChild(this.trayFooter(this.sumCaptures(leftData)));
        this.trayRightEl.appendChild(this.trayFooter(this.sumCaptures(rightData)));

    }

    trayHeader(text) {
        const h = document.createElement('h3');
        h.textContent = text;
        return h;
      }

    trayFooter(text){
        const f = document.createElement('footer');
        f.textContent = text;
        return f;
    }   

    renderCaptureList(list) {
        const wrap = document.createElement('div');
        wrap.className = 'captures-list';
        for (const item of list) {
          const row = document.createElement('div');
          row.className = 'capture-item';
          const piece = document.createElement('span');
          piece.className = 'piece';
          piece.textContent = PIECES[item.piece] || item.piece;
          const meta = document.createElement('span');
          meta.className = 'meta';
          meta.textContent = `Turn ${item.turn}`;
          row.appendChild(piece);
          row.appendChild(meta);
          wrap.appendChild(row);
        }
        return wrap;
    }

    renderMoveHighlight(from, toList) { /* TODO */ }

    // --- Notation & history ---
    /** @param {Move} move */
    toAlgebraic(move) { /* TODO: SAN */ return ''; }
    pushHistory(move, san) { /* TODO */ }
    renderHistory() { /* TODO */ }

    sumCaptures(list){
        let sum = 0;
     for (let i = 0; i<list.length;i++){
        if (list[i].piece.toLowerCase() === 'p' ){
            sum+=1;
        }
        else if (list[i].piece.toLowerCase() === 'b' || list[i].piece.toLowerCase() === 'n' ){
            sum+=3;
        }
        else if (list[i].piece.toLowerCase() === 'r' ){
            sum+=5;
        }
        else if (list[i].piece.toLowerCase() === 'q' ){
            sum+=9;
        }
     }
     return sum;
    }
    
    // --- QoL ---
    setStartPosition() {

      this.state.board = [
        //black power pieces
        [{piece:'r', color:false, rank:0,file:0,captured:false,idP:15},
          {piece:'n',color:false, rank:0,file:1,captured:false,idP:14},
          {piece:'b',color:false, rank:0,file:2,captured:false,idP:13},
          {piece:'q',color:false, rank:0,file:3,captured:false,idP:12},
          {piece:'k',color:false, rank:0,file:4,captured:false,idP:11},
          {piece:'b',color:false, rank:0,file:5,captured:false,idP:10},
          {piece:'n',color:false, rank:0,file:6,captured:false,idP:9},
          {piece:'r',color:false, rank:0,file:7,captured:false,idP:8}
        ],
      
        [{piece:'p',color:false, rank:1,file:0,captured:false,idP:7},
          {piece:'p',color:false, rank:1,file:1,captured:false,idP:6},
          {piece:'p',color:false, rank:1,file:2,captured:false,idP:5},
          {piece:'p',color:false, rank:1,file:3,captured:false,idP:4},
          {piece:'p',color:false, rank:1,file:4,captured:false,idP:3},
          {piece:'p',color:false, rank:1,file:5,captured:false,idP:2},
          {piece:'p',color:false, rank:1,file:6,captured:false,idP:1},
          {piece:'p',color:false, rank:1,file:7,captured:false,idP:0}
        ],

        [null,null,null,null,null,null,null,null],
        [null,null,null,null,null,null,null,null],
        [null,null,null,null,null,null,null,null],
        [null,null,null,null,null,null,null,null],

        [{piece:'P',color:true, rank:6,file:0,captured:false,idP:16},
          {piece:'P',color:true, rank:6,file:1,captured:false,idP:17},
          {piece:'P',color:true, rank:6,file:2,captured:false,idP:18},
          {piece:'P',color:true, rank:6,file:3,captured:false,idP:19},
          {piece:'P',color:true, rank:6,file:4,captured:false,idP:20},
          {piece:'P',color:true, rank:6,file:5,captured:false,idP:21},
          {piece:'P',color:true, rank:6,file:6,captured:false,idP:22},
          {piece:'P',color:true, rank:6,file:7,captured:false,idP:23}
        ],

        [{piece:'R',color:true, rank:7,file:0,captured:false,idP:24},
          {piece:'N',color:true, rank:7,file:1,captured:false,idP:25},
          {piece:'B',color:true, rank:7,file:2,captured:false,idP:26},
          {piece:'Q',color:true, rank:7,file:3,captured:false,idP:27},
          {piece:'K',color:true, rank:7,file:4,captured:false,idP:28},
          {piece:'B',color:true, rank:7,file:5,captured:false,idP:29},
          {piece:'N',color:true, rank:7,file:6,captured:false,idP:30},
          {piece:'R',color:true, rank:7,file:7,captured:false,idP:31}],
      ];

      this.state.piece_list = [
          //all black pieces have id 0-15
          {piece:'p',color:false, rank:1,file:7,captured:false,idP:0},
          {piece:'p',color:false, rank:1,file:6,captured:false,idP:1},
          {piece:'p',color:false, rank:1,file:5,captured:false,idP:2},
          {piece:'p',color:false, rank:1,file:4,captured:false,idP:3},
          {piece:'p',color:false, rank:1,file:3,captured:false,idP:4},
          {piece:'p',color:false, rank:1,file:2,captured:false,idP:5},
          {piece:'p',color:false, rank:1,file:1,captured:false,idP:6},
          {piece:'p',color:false, rank:1,file:0,captured:false,idP:7},
          {piece:'r',color:false, rank:0,file:7,captured:false,idP:8},
          {piece:'n',color:false, rank:0,file:6,captured:false,idP:9},
          {piece:'b',color:false, rank:0,file:5,captured:false,idP:10},
          //technically** should easily be able to find king position index -- 11
          {piece:'k',color:false, rank:0,file:4,captured:false,idP:11},
          {piece:'q',color:false, rank:0,file:3,captured:false,idP:12},
          {piece:'b',color:false, rank:0,file:2,captured:false,idP:13},
          {piece:'n',color:false, rank:0,file:1,captured:false,idP:14},
          {piece:'r',color:false, rank:0,file:0,captured:false,idP:15},
          //all white pieces have id 16-31
          {piece:'P',color:true, rank:6,file:0,captured:false,idP:16},
          {piece:'P',color:true, rank:6,file:1,captured:false,idP:17},
          {piece:'P',color:true, rank:6,file:2,captured:false,idP:18},
          {piece:'P',color:true, rank:6,file:3,captured:false,idP:19},
          {piece:'P',color:true, rank:6,file:4,captured:false,idP:20},
          {piece:'P',color:true, rank:6,file:5,captured:false,idP:21},
          {piece:'P',color:true, rank:6,file:6,captured:false,idP:22},
          {piece:'P',color:true, rank:6,file:7,captured:false,idP:23},
          {piece:'R',color:true, rank:7,file:0,captured:false,idP:24},
          {piece:'N',color:true, rank:7,file:1,captured:false,idP:25},
          {piece:'B',color:true, rank:7,file:2,captured:false,idP:26},
          {piece:'Q',color:true, rank:7,file:3,captured:false,idP:27},
          //technically** should easily be able to find king position -- index 28
          {piece:'K',color:true, rank:7,file:4,captured:false,idP:28},
          {piece:'B',color:true, rank:7,file:5,captured:false,idP:29},
          {piece:'N',color:true, rank:7,file:6,captured:false,idP:30},
          {piece:'R',color:true, rank:7,file:7,captured:false,idP:31}
      ];
        this.state.white_can_move = [
          [0,0,0,0,0,0,0,0],
          [0,0,0,0,0,0,0,0],
          [0,0,0,0,0,0,0,0],
          [0,0,0,0,0,0,0,0],
          [0,0,0,0,0,0,0,0],
          [0,0,0,0,0,0,0,0],
          [0,0,0,0,0,0,0,0],
          [0,0,0,0,0,0,0,0],
        ]
         this.state.black_can_move = [
          [0,0,0,0,0,0,0,0],
          [0,0,0,0,0,0,0,0],
          [0,0,0,0,0,0,0,0],
          [0,0,0,0,0,0,0,0],
          [0,0,0,0,0,0,0,0],
          [0,0,0,0,0,0,0,0],
          [0,0,0,0,0,0,0,0],
          [0,0,0,0,0,0,0,0],
        ]
      /*const start = [
         
        'rnbqkbnr',
        'pppppppp',
        '........',
        '........',
        '........',
        '........',
        'PPPPPPPP',
        'RNBQKBNR',
      ];*/
      
      //this.state.board = start_board;
      //this.state.board = start.map(row => row.split(''));
      this.state.sideToMove = true;
      this.state.halfmove = 0;
      this.state.moveNumber = 1;
      this.state.black_capture = [];
      this.state.white_capture = [];
    }

    resetGame() { this.setStartPosition(); this.renderPosition(); this.renderStatus(); }

    /** @param {number} depth */
    perft(depth) { /* TODO */ return 0; }

    copyFENToClipboard() { /* TODO */ }
    pasteFENFromInput() { /* TODO */ }

    // --- helpers ---
    _squareAt(sq) { 
      if (this.current_orientation==='white') return this._squares[sq.r * 8 + sq.f];
         return this._squares[63-(sq.r * 8 + sq.f)];
      
    }

    // new board of BoardPiece[][] functions
    /**@param {BoardPiece[][]} board */
    getPieceFromBoard(rank,file){
      if (this.state.board[rank][file]) {return (this.state.board[rank][file]).piece}
      return '.';
    }
    // remember black = false, white = true
    getPieceColorFromBoard(rank,file){
      if (this.state.board[rank][file]) {return (this.state.board[rank][file]).color}
      return '.';
    }

    //only call if legal move has been verified
    // only for empty moves

    movePieceOnBoard(cur_rank, cur_file, to_rank, to_file){
      if (this.state.board[cur_rank][cur_file]){
        this.state.board[cur_rank][cur_file].rank = to_rank; //not need need
        this.state.board[cur_rank][cur_file].file = to_file; //not needed
        this.state.piece_list[this.state.board[cur_rank][cur_file].idP].rank = to_rank; //very neccesary
        this.state.piece_list[this.state.board[cur_rank][cur_file].idP].file = to_file; //very necessary
        this.state.board[to_rank][to_file] = this.state.board[cur_rank][cur_file];
        this.state.board[cur_rank][cur_file] = null;
        return true;
      }
      return false;
    }
    //method is used for promotion
    setPieceOnBoard(rank, file, set_piece){
      this.state.board[rank][file].piece = set_piece;
      this.state.piece_list[this.state.board[rank][file].idP].piece = set_piece;
    }

    getKingPosFromBoard(king_color){
      if (king_color) {return this.state.piece_list[28];} //for now return entire king board piece
      return this.state.piece_list[11];
    }
  }

  function coordName(file, rank) {
    return coordText(rank,file);
  }

  function coordText(r, f) {
  const fileChar = String.fromCharCode('a'.charCodeAt(0) + f);
  const rankChar = true ? String(8 - r) : String(r + 1);
  return fileChar + rankChar;
  }
  /** @returns {string} */
  function checkPieceColor(piece){
    //console.log("inside checkPieceColor\n");
    //console.log(piece);
    let color = {
    'P': 'w','N': 'w','B': 'w','R':'w','Q': 'w','K': 'w', /* white */
    'p': 'b','n': 'b','b': 'b','r':'b','q': 'b','k': 'b', /*black*/
    '.':'u' 
    };
    return color[piece];
  }

  function getPiecefromNum(pieceNum){
   let piece = ['.','P','p','B','N',"R",'b',"Q","n",'K','r','.','.','.','q','.','.','.','k'];

   return piece[pieceNum];

  }

    // Map a view square (r,f) to the board indices (ri,fi) in FEN order
    function toBoardIndex(r, f) {
    if (true) return { ri: r,      fi: f      }; // top row is rank 8 = index 0
    else                         return { ri: 7 - r,  fi: 7 - f  };
    }

    async function postChessApi(data = {}) {
    const response = await fetch("https://chess-api.com/v1", {
        method: "POST",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify(data),
    });
        return response.json();
    }

  // Expose for console debugging
  window.Chess = Chess; 
  
  window.promoVar = '.';
})();
